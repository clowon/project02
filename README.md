<div align="center"><h1>資料結構 Project II</h1></div>
<div align="center"><h2>Data Type Graph Contraol</h2></div>
<div align="center"><h4>組別：J、組員：賴瑄融 B10902114、詹鈞皓 B10902108</h4></div>

本程式使用到的函式庫為`C++`內建的輸入輸出流函式庫及計時器函式庫，編譯器必須版本為`C++14`以上。不支援DevC++。
## 所使用標準函式庫為 (省略`std::`) ： 
- `<iostream>` 標準輸入輸出流： `cin`, `cout` 等 stream。
- `<fstream>` 標準文件輸入輸出流： 用於文件的輸入輸出，可以重載運算子，在輸出矩陣資料時很方便。
- `<iomanip>` 格式化輸出： 用於標準輸出流的排版，`setw`, `hex` 等

## 輸入文件格式說明
- 副檔名為`.txt`純文字格式，col 與 col 之間以空格分隔，空格數不限；換行字元支援`\r\n`(Windows)及`\n`(Linux & macOS)，接受換行前空格。
- 文件輸入方式： 開啟執行檔後手動輸入文件路徑。
- 呼叫`make_2d_array(row,col)` 動態產生出一個二維矩陣。
- 先將文件疊代過一輪取得矩陣的長寬，忽略行尾前空格。獲得輸入的長寬後分配記憶體空間，回到文件起始位置，將各項資料放入矩陣。程式片段如下（省略excaption）：
```cpp
while (test >> input) 			                                        //get row and column
        {
            if (test.peek() == ' ')						// ignore the space before newline
                test.get();
            if (test.peek() == '\n' || test.peek() == '\r')			// for Win32, the end of line will be "\r\n",
                row++;						                // but for linux or macOS, that will be "\n"
        }
        col /= row;								// total / rows = cols; obviously
        matrix = make_2d_array(row, col); 					//create 2D array
        test.clear();                     					//reset the file-postion pointer(seek get)
        test.seekg(0, test.beg);          					//reset the file-postion pointer(seek get)
        input = 0;
        while (test >> input) 							//read the data
        {
            if (input) 
                matrix[i][j] = input;
            else
                matrix[i][j] = 0;
            j++;
            if (j == col)
            {
                i++;
                j = 0;
            }
        }
```
- `.txt` 格式參考，0為未相連，1為相連：
```
  0   1   0   1   0   0   0   0
  1   0   1   0   1   0   0   1
  0   1   0   1   0   0   0   0
  1   0   1   0   1   0   1   0
  0   1   0   1   0   1   0   0
  0   0   0   0   1   0   1   1
  0   0   0   1   0   1   0   1
  0   1   0   0   0   1   1   0
```
## 計算連接的白色區塊數量
- 計算連接區塊需要確認從起始位置算起需要"填充"多少次，從1開始填值，不同區依序遞增，被填充過後的區塊會忽略。
- 下圖為gif演示。參見`res/filldemo.gif`
<div align="center">
  <a href="https://imgur.com/7b4sMLO"><img src="https://i.imgur.com/7b4sMLO.gif" title="source: imgur.com" /></a>
</div>

- 填充使用到的演算法為Flood Fill －洪水演算法，將起始位相鄰白塊位置倒入queue（或 stack）中，將queue存的第一個位置設為當前的值後pop，之後將下一個位置的相鄰白塊存入queue，重複直到queue中沒有任何東西為止。
- `queue<T>` 為自己寫的佇列class，建構子參數為初始容量，預留空間能確保程式不會花太多時間在擴充容量（`reallocate`）。
- `grid` 為記錄座標的struct type。`matrix2d<T>::width`是`uint16_t`型別。
- 以下為部分演算法片段，最外層的for迴圈會將整個矩陣疊代一次：
```cpp
queue<grid> sp(256);                                            // queue for grid type
// init with given capacity can activate faster
size_t count = 0, num = 0;                                      // counts how many grids connected
grid tmp;                                                       // temporary of queue pop value
for (matrix2d<int>::width row = 0; row < mat.maxrow(); row++) {
    for (matrix2d<int>::width col = 0; col < mat.maxcol(); col++) {
        if (mat[row][col] == replace_val) {                     // first of blocks
            count++;
            sp.push(grid {col, row});                           // x = col, y = row
            while(!sp.empty()) {
                num++;
                tmp = sp.pop();                                 // compute the first element in container
                if (mat[tmp.y][tmp.x] == replace_val) {
                    mat[tmp.y][tmp.x] = count;
                    if (tmp.y > 0 && mat[tmp.y - 1][tmp.x] == replace_val)                  // up
            	        sp.push(make_grid(tmp.x, tmp.y - 1));                               // for next stage
            	    if (tmp.x > 0 && mat[tmp.y][tmp.x - 1] == replace_val)                  // left
                        sp.push(make_grid(tmp.x - 1, tmp.y));                               // same as above
            	    if (tmp.y < mat.maxrow() - 1 && mat[tmp.y + 1][tmp.x] == replace_val)   // down
            	        sp.push(make_grid(tmp.x, tmp.y + 1));                               // same as above
                    if (tmp.x < mat.maxcol() - 1 && mat[tmp.y][tmp.x + 1] == replace_val)   // right
                        sp.push(make_grid(tmp.x + 1, tmp.y));                               // same as above
            	}
            }
        }
    }
}
```
- 佇列中儲存的位置必定為非零項。
- 變數`num`用來計算演算法共執行了幾次佇列操作； `make_grid`用於避免gnu編譯器的整數縮小轉換錯誤。

## 演算法時間複雜度big-O
- 矩陣當中的非零項最多只會被存入一次到容器中，而佇列（或堆疊）的`pop`與`push`所花費的時間固定，因此每個非零項所需的處裡時間為`O(1)`，時間複雜度與矩陣中非零項總數成正比關係，因此可以得出總時間複雜度為`O(n)`，n為非零項總數。
- 若矩陣邊長為m，則時間與邊長關係為`O(m^2)`。
<div align="center">
    <a href="https://imgur.com/yc8fDka"><img src="https://i.imgur.com/yc8fDka.jpg" title="source: imgur.com" /></a>
</div>
- 依輸出圖表所示，計算時間與非零項總數成正比關係。
    
## 演算法輸出結果
- 這部分使用到了C++的運算子重載部分，能將矩陣以`cout`方式列印，或是輸出到文件中。
- 輸出使用到`setw()`讓輸出矩陣能夠對齊。
```cpp
// print this matrix, standard ostream << fuction override
std::ostream& operator<<(std::ostream& os, const matrix2d& mat) {
    size_t r = 0, c = 0, n = 0;
    while (r != mat.row) {
        os << std::setw(3) << mat.data[n] << " ";
        c++; n++;
        if (c == mat.col) {
            r++; c = 0;
            os << "\n";
        }
    }
    return os;
}
```
- `logfile.txt` 輸出格式範例：
```
  0   0   0   0   0   0   0   0 
  0   1   1   0   0   2   2   0 
  0   1   1   0   0   2   2   0 
  0   0   0   3   3   0   0   0 
  0   0   3   3   3   3   0   0 
  0   0   3   3   3   3   0   0 
  0   0   3   0   0   3   0   0 
  0   0   0   0   0   0   0   0 
```

## 容器結構 Class
- 此程式所用到的容器類歸類在`mycontainer.hpp`中，包含`dynamic matrix`, `stack`, `queue`。
- 所有容器的底層形式均為一維陣列，並共用`mem_allocator`。
- `mem_allocator class` 共用記憶體分配，包含`alloc`, `realloc`, `dealloc`功能。
    - `allocate()` 回傳一個分配的記憶體空間。
    - `reallocate()` 分配新的記憶體空間，舊資料會複製過去。
    - `deallocate()` 釋放已分配的記憶體空間。
    - 若是執行中出現錯誤會拋出異常。
```cpp
template<class T>
class mem_allocator
```
- `matrix2d class` 動態二維矩陣。
    - `array()` 以一維陣列方式回傳data。
    - `resize()` 重新設定matrix的長寬，會重新分配空間。
    - `operator[]()` 回傳對應rol位置的指標，假裝是二維陣列。
    - `operator<<()` 自定義`std::ostream`輸出重載。
```cpp
template<class T>
class matrix2d : mem_allocator<T> 
```
- `stack class` 未使用
- `queue class` 簡易佇列結構。
    - 在記憶體中是以環狀記憶體的方式實作，分為`head`與`tail`兩個寫入讀出指標。
```cpp
template<class T>
class queue : mem_allocator<T> 
```

## 附加文件
- `chromat.cpp` 與 `mycontainer.hpp` 為使用到template容器的版本，會計算各部分花費的時間。
- `project1.cpp` 為不帶template容器的版本，功能基本相同。
- `res/input/` 資料夾中是所有使用到的輸入測試文字文件。從`10x10`到`960x960`大小矩陣輸入。部分是用畫出來的圖檔轉換而來。
- `res/input/ratio/` 資料夾中為同矩陣等比放大的測試檔，前綴為倍數，分為`aa`和`bb41`兩種矩陣。
- `execute/` 資料夾中為編譯後的執行檔，不同環境下可能會無法執行。
- `README.md` 此文件的markdown版本。

## 附加工具
- 於 `tools/` 子目錄下。 
- `bmpinfo.cpp`, `bmpreader.hpp` 將指定的`.bmp`圖檔輸出為`.txt`格式矩陣。會額外生成`.palette`供gif產生器使用。
- `algo_visual.cpp`, `gif_editor.h` 將`.txt`與`.palette`合併成gif動畫格式，用於生成可視化演算法。注意：若輸入文件非零項數過大，輸出gif的幀數可能會破萬。
- `res/` 資料夾中為測試時使用到的`.bmp`, `.txt`, `.palette` 及 `.gif`輸出文件。
