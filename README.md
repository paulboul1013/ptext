# ptext

## 學習筆記

1. 基礎 I/O 函式庫
read() 與 STDIN_FILENO：
來自 <unistd.h> 標頭檔。
STDIN_FILENO 是標準輸入的檔案描述符（File Descriptor），通常數值為 0。
read() 函式會回傳它成功讀取到的位元組（bytes）數量。如果回傳 0 代表讀到了檔案結尾（EOF）；回傳 -1 代表發生錯誤。


2. Canonical Mode (標準模式) vs. Raw Mode (原始模式)
Canonical Mode (Cooked Mode)：
終端機的預設模式。
在此模式下，輸入是「行緩衝（Line-buffered）」的。只有當你按下 Enter 鍵後，輸入的資料才會被傳送給程式。這對文字編輯器來說是不行的，因為我們需要即時處理每一個按鍵。
Raw Mode (原始模式)：
我們需要的模式。
讓程式能即時處理每一個按鍵輸入（byte-by-byte）。
這不是一個簡單的開關，而是透過關閉終端機的一系列特定旗標（Flags）來達成的。


3. 設定終端機屬性 (termios)
tcgetattr()：
用來讀取當前終端機的屬性（Attributes）並存入 struct termios 結構中。
修改屬性：
我們手動修改這個 struct 中的旗標。
tcsetattr()：
將修改後的 struct 寫回終端機以套用新設定。
TCSAFLUSH：
這是傳給 tcsetattr() 的參數。它的作用是在套用變更前，丟棄（Discard）所有尚未被程式讀取的輸入，確保設定轉換時不會有殘留數據干擾。


4. 關鍵旗標設定 (Flags)
ECHO：
功能：當你打字時，終端機預設會把你打的字顯示在螢幕上。
為何關閉：在 Raw Mode 下，我們希望手動控制顯示內容（就像輸入 sudo 密碼時一樣），所以必須關閉它。否則你打字時螢幕會出現雙重字元或亂碼。
ICANON：
來自 <termios.h>，用來關閉 Canonical Mode（行緩衝），開啟逐字元輸入。
ISIG：
用來關閉訊號發送字元，如 Ctrl-C (SIGINT) 和 Ctrl-Z (SIGTSTP)。
IXON：
來自 <termios.h>，其中的 I 代表 "Input flag"，XON 代表流控制。
Ctrl-S：暫停數據傳輸（這是早期用來讓印表機跟上速度的設計）。
Ctrl-Q：恢復傳輸。
設定：我們需要關閉 IXON，否則按下 Ctrl-S 會讓編輯器畫面凍結。
ICRNL：
I = Input, CR = Carriage Return, NL = New Line.
Ctrl-M：發送的是 13 (\r)，但終端機通常將其讀為 10 (\n)，因為 Ctrl-J 也是 10。
設定：關閉此旗標可以區分 \r 和 \n，讓我們能正確處理 Enter 鍵。
OPOST：
輸出處理旗標。預設情況下，終端機會將程式輸出的 \n 自動轉換為 \r\n。
設定：關閉它，讓我們完全控制輸出格式（需要手動輸出 \r\n 來換行）。
其他 Legacy 旗標：
BRKINT, INPCK, ISTRIP, CS8。
這些在現代終端機上可能沒有明顯效果，但為了保持良好的 Raw Mode 習慣，通常會一併關閉（這被視為開啟 Raw Mode 的標準儀式）。


## 二、訊號處理與程式退出 (Signals & Exit)
atexit()：
來自 <stdlib.h>。
可以註冊一個函數，當程式正常結束（例如從 main 返回或呼叫 exit()）時，該函數會被自動執行。
用途：用來在程式結束時恢復終端機的原始設定（disableRawMode），否則使用者的終端機可能會留在 Raw Mode 導致無法使用（需要盲打 reset 指令來修復）。
Ctrl-C：
發送 SIGINT 訊號，預設會終止程式。
Ctrl-Z：
發送 SIGTSTP 訊號，預設會將程式掛起（Suspend）到背景。
Ctrl-Q：
在我們的編輯器中，通常將其映射為「退出程式」的指令。
## 三、輸入控制與超時 (Input Control & Timeout)
iscntrl()：
來自 <ctype.h>。用來測試一個字元是否為「控制字元」（不可列印字元，如 ASCII 0-31 和 127）。我們不想把這些字元直接印在螢幕上。
read() 的超時設定 (VMIN 與 VTIME)：
來自 <termios.h> 的 c_cc (control characters) 陣列。
預設 read() 會無限期等待輸入。
VMIN：設定 read() 返回前所需的最小位元組數。設為 0 表示只要有數據就讀，不用等滿。
VTIME：設定等待輸入的最大時間（單位是 1/10 秒）。
用途：可以用來製作簡單的動畫或定時刷新，或者避免程式完全卡死在等待輸入上。
## 四、錯誤處理 (Error Handling)
errno 與 EAGAIN：
來自 <errno.h>。
大多數 C 函式庫失敗時會設定全域變數 errno。
tcsetattr(), tcgetattr(), read() 失敗時都會回傳 -1。
除錯技巧：如果執行 echo test | ./main，tcgetattr 會報錯（如 "Inappropriate ioctl for device"），因為標準輸入不是終端機。
## 五、特殊按鍵處理 (Special Keys Handling)
Ctrl 鍵組合：
Ctrl 鍵搭配字母鍵會將該字母的 ASCII 碼映射到 1-26 (例如 Ctrl-A = 1)。
Ctrl-V：
在許多終端機中，按下 Ctrl-V 後，終端機會等待下一個字元並按原樣發送（Literal send）。
Esc 鍵 (\x1b)：
ASCII 值為 27。它是所有 Escape Sequences（跳脫序列）的開頭。
方向鍵處理：
方向鍵不會只發送一個 byte，而是發送以 \x1b[ 開頭的序列：
上：\x1b[A
下：\x1b[B
右：\x1b[C
左：\x1b[D
策略：我們需要用一個較大的整數（超出 char 範圍，例如 enum）來表示這些方向鍵，避免與普通 ASCII 字元衝突。
## 六、螢幕繪製與游標控制 (Output & Drawing)
1. Escape Sequences (ANSI 跳脫序列)
格式通常為：\x1b + [ + 參數 + 指令字元。
write() 與 STDOUT_FILENO：來自 <unistd.h>，用來向螢幕輸出指令。
常用指令：
J (Erase in Display)：清除螢幕。\x1b[2J 清除整個螢幕。
H (Cursor Position)：移動游標。格式為 \x1b[row;colH。例如 \x1b[1;1H 將游標移到左上角。
K (Erase in Line)：清除行。\x1b[K 清除從游標位置到行尾的內容。這比每次都清除整個螢幕 (2J) 更高效，能減少閃爍。
h 與 l：
\x1b[?25l：隱藏游標 (Hide)。
\x1b[?25h：顯示游標 (Show)。
用途：在重新繪製畫面時先隱藏游標，畫完後再顯示，可以避免游標在螢幕上亂跳的視覺干擾。
2. 獲取終端機視窗大小 (Window Size)
ioctl() 與 TIOCGWINSZ：
來自 <sys/ioctl.h>。
TIOCGWINSZ：Terminal IOCtl Get WINdow SiZe。
如果成功，會將行數和列數填入 struct winsize 結構中。
Fallback 方法 (如果 ioctl 失敗)：
將游標移到螢幕最右下角（發送 \x1b[999C (右) 和 \x1b[999B (下)）。
查詢游標位置：發送 \x1b[6n。
讀取回應：終端機會透過標準輸入回傳類似 \x1b[24;80R 的字串。
解析回應：忽略前兩個字元 (\x1b[), 使用 sscanf() 解析 rows;cols。
3. Append Buffer (輸出緩衝區)
問題：如果每次更新螢幕都呼叫多次小的 write()，會造成畫面閃爍（Flicker）。
解法：建立一個動態字串緩衝區（Dynamic String Buffer，這裡稱為 abuf）。
實作：將所有要輸出的 Escape Sequences 和文字內容先 append 到這個 buffer，最後只呼叫一次 write() 將整個 buffer 輸出到螢幕。
## 七、編輯器資料結構與邏輯 (Editor State & Logic)
1. 全域狀態 (E struct)
initEditor()：負責初始化所有狀態變數。
游標位置：
E.cx (水平座標), E.cy (垂直座標)。
初始值皆為 0（左上角）。
需要限制在檔案內容的有效範圍內。
2. 行資料結構 (erow)
erow (Editor Row)：用來儲存一行文字的結構。
包含一個指向動態分配字元陣列的指標 (chars) 和長度 (size)。
編輯器初版只顯示單行文字時，numrows 初始化為 0。
3. 畫面呈現 (Rendering)
歡迎畫面：
使用 snprintf() 格式化版本號字串 (KILO_VERSION)。
置中演算法：(螢幕寬度 / 2) - (字串長度 / 2) = 起始列印位置。前面補空白，第一行可加上波浪號 ~（類似 Vim）。
文字繪製：
檢查當前繪製的行是否在 erow 緩衝區內。如果是，則寫入 erow.chars；否則繪製波浪號 ~ 表示檔案結尾。
需處理行長度超過螢幕寬度時的截斷（Truncate）。
4. 捲動 (Scrolling)
Offset 變數：
coloff (Column Offset)：紀錄目前水平捲動了多少字元。
rowoff (Row Offset)：紀錄目前垂直捲動了多少行。
限制：
限制 E.cx 和 E.cy 只能在合法的檔案範圍內移動。
Phantom Cursor 問題：使用者可能會將游標移到長行的末端，然後下移到短行。此時 E.cx 沒變，游標會跑到短行的右邊虛空處。需要邏輯修正（將 E.cx 校正為短行的長度）。
