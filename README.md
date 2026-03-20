# prototype_text

`prototype_text` 是一個終端機文字編輯器原型

## 1. 終端機控制與 Raw Mode（termios）

```c
void enableRawMode() {
    if (tcgetattr(STDIN_FILENO,&E.orig_termios)==-1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw=E.orig_termios;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN]=0;
    raw.c_cc[VTIME]=1;
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw)==-1) die("tcsetattr");
}
```

- `ICANON` 關閉後可逐字讀取，不需等 Enter。
- `ECHO` 關閉後由編輯器自己控制畫面輸出。
- `VMIN/VTIME` 讓 `read()` 可輪詢，避免整個程式卡死在輸入。

## 2. 特殊按鍵解碼（Escape Sequence）

```c
if (c=='\x1b') {
    char seq[3];
    if (read(STDIN_FILENO,&seq[0],1)!=1) return '\x1b';
    if (read(STDIN_FILENO,&seq[1],1)!=1) return '\x1b';
    if (seq[0]=='[' && seq[1]=='A') return ARROW_UP;
    ...
}
```

- 方向鍵不是單一字元，而是 `\x1b[` 開頭序列。
- 用 `enum editorKey`（例如 `ARROW_UP = 1000`）避免和 ASCII 衝突。

## 3. 編輯器核心狀態設計（State Model）

```c
typedef struct erow {
    int idx, size, rsize;
    char *chars;
    char *render;
    unsigned char *hl;
    int hl_open_comment;
} erow;

struct editorConfig {
    int cx, cy, rx;
    int rowoff, coloff;
    int screenrows, screencols;
    int numrows;
    erow *row;
    int dirty;
    char *filename;
    struct editorSyntax *syntax;
};
```

- `chars` 是真實檔案內容，`render` 是展開 tab 後給螢幕畫的內容。
- `cx/cy` 是「文字座標」，`rx` 是「渲染座標」。
- `dirty` 追蹤是否有未儲存變更。

## 4. Tab 與游標映射（cx <-> rx）

```c
int editorRowCxToRx(erow *row,int cx){
    int rx=0;
    for (int j=0;j<cx;j++){
        if (row->chars[j]=='\t')
            rx+=(KILO_TAB_STOP-1)-(rx%KILO_TAB_STOP);
        rx++;
    }
    return rx;
}
```

- 這是文字編輯器常見難點：tab 寬度固定，但佔用顯示欄位不固定。
- `editorRowRxToCx()` 做反向換算，讓滑鼠/搜尋/跳轉能回到真實字元索引。

## 5. 螢幕重繪與閃爍控制（ANSI + Append Buffer）

```c
struct abuf { char *b; int len; };
...
abAppend(&ab,"\x1b[?25l",6); // hide cursor
abAppend(&ab,"\x1b[H",3);    // go home
editorDrawRows(&ab);
editorDrawStatusBar(&ab);
editorDrawMessageBar(&ab);
abAppend(&ab,"\x1b[?25h",6); // show cursor
write(STDOUT_FILENO,ab.b,ab.len);
```

- 用 buffer 聚合輸出，再一次 `write()`，可減少閃爍。
- ANSI 控制序列負責清屏、定位游標、顏色切換。

## 6. 基本編輯操作（插入、換行、刪除）

```c
void editorInsertChar(int c){
    if (E.cy==E.numrows) editorInsertRow(E.numrows,"",0);
    editorRowInsertChar(&E.row[E.cy],E.cx,c);
    E.cx++;
}

void editorDelChar(){
    if (E.cx>0){
        editorRowDelChar(&E.row[E.cy],E.cx-1);
        E.cx--;
    }else{
        E.cx=E.row[E.cy-1].size;
        editorRowAppendString(&E.row[E.cy-1],row->chars,row->size);
        editorDelRow(E.cy);
        E.cy--;
    }
}
```

- 刪除在行首時會和上一行合併，這是行編輯器的核心行為之一。
- 內部使用 `realloc/memmove` 維持動態字串。

## 7. 檔案 I/O 與序列化

```c
while ((linelen=getline(&line,&linecap,fp))!=-1) {
    while (linelen>0 && (line[linelen-1]=='\n' || line[linelen-1]=='\r'))
        linelen--;
    editorInsertRow(E.numrows, line, linelen);
}
...
char *buf=editorRowsToString(&len);
int fd=open(E.filename,O_RDWR | O_CREAT,0644);
ftruncate(fd,len);
write(fd,buf,len);
```

- 載入時先去掉行尾換行，再進入內部 row 結構。
- 存檔時把所有 row 拼成單一 buffer，統一寫回磁碟。

## 8. 語法高亮（Lexical Highlight）

```c
if (E.syntax->flags & HL_HIGHLIGHT_NUMBERS){
    if ((isdigit(c) && (prev_sep || prev_hl==HL_NUMBER)) ||
        (c=='.' && prev_hl==HL_NUMBER)) {
        row->hl[i]=HL_NUMBER;
        ...
    }
}
```

- `HLDB` 定義語言規則（副檔名、關鍵字、註解起訖、flags）。
- 每行有 `hl[]` 與 `render[]` 對齊，畫面輸出時再轉 ANSI 顏色。

## 9. 互動式搜尋（Incremental Search）

```c
char *query=editorPrompt("Search: %s (Use ESC/Arrows/Enter)",editorFindCallback);
...
char *match=strstr(row->render,query);
if (match){
    E.cy=current;
    E.cx=editorRowRxToCx(row,match-row->render);
    memset(&row->hl[match-row->render],HL_MATCH,strlen(query));
}
```

- 搜尋和輸入框共用 `editorPrompt()`。
- 每次按鍵都回呼 `editorFindCallback()`，即時跳轉與高亮。

## 10. 事件循環（Editor Loop）

```c
int main(int argc,char *argv[]){
    enableRawMode();
    initEditor();
    if (argc>=2) editorOpen(argv[1]);
    while(1){
        editorRefreshScreen();
        editorProcessKeypress();
    }
}
```

- 標準 loop：`read input -> update state -> render`。
- 這個模式是大多數 terminal editor / TUI 程式的骨架。

## 建置

```bash
make
./prototype_text [filename]
```
