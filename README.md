
<div align="center"><h1>資料結構 Project II</h1></div>
<div align="center"><h2>Data Type Graph Control</h2></div>
<div align="center"><h4>組別：J、組員：賴瑄融 B10902114、詹鈞皓 B10902108</h4></div>

本程式使用到的函式庫為`C++`內建的輸入輸出流函式庫，編譯器必須版本為`C++14`以上。不支援DevC++。
## 所使用標準函式庫為 (省略`std::`) ： 
- `<iostream>` 標準輸入輸出流： `cin`, `cout` 等 stream。
- `<fstream>` 標準文件輸入輸出流： 用於文件的輸入輸出，可以重載運算子，在輸出矩陣資料時很方便。
- `<iomanip>` 格式化輸出： 用於標準輸出流的排版，`setw`, `hex` 等
- `quene.hpp` 自己寫的quene: BFS 需要


## 輸入文件格式說明
- 副檔名為`.txt`純文字格式，col 與 col 之間以空格分隔，空格數不限；換行字元支援`\r\n`(Windows)及`\n`(Linux & macOS)，接受換行前空格。
- 文件輸入方式： 開啟執行檔後手動輸入文件路徑。
- 呼叫`make_2d_array(row,col)` 動態產生出一個二維矩陣。
- 先將文件疊代過一輪取得矩陣的長寬，忽略行尾前空格。獲得輸入的長寬後分配記憶體空間，回到文件起始位置，將各項資料放入矩陣。程式片段如下（省略excaption）：
```cpp
while (test >> input) 			                          //get row and column
        {
            if (test.peek() == ' ')	                          // ignore the space before newline
                test.get();
            if (test.peek() == '\n' || test.peek() == '\r')	  // for Win32, the end of line will be "\r\n",
                row++;						  // but for linux or macOS, that will be "\n"
        }
        col /= row;					          // total / rows = cols; obviously
        matrix = make_2d_array(row, col); 			  //create 2D array
        test.clear();                                             //reset the file-postion pointer(seek get)
        test.seekg(0, test.beg);          			  //reset the file-postion pointer(seek get)
        input = 0;
        while (test >> input) 		                          //read the data
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
## adj matrix 轉換成 adj list
- 讀取矩陣裡的值，1 是相連 ，0 是為相連
- 若是自己走到自己則不insert到adj list中
- 在轉換過程時，也會將 edge 算出
- 時間複雜度 *big-O* 等於 *O(v^2)* (v = number of vertices)
```cpp
for (int i = 0; i < row; i++)                                      //adj matrix ->adj list
    {
        for (int j = 0; j < col; j++)
        {
            if (matrix[i][j])                                      //insert if matrix[i][j] != 0
            {
                if (i != j)                                        //dont insert if itselt go to itself
                {
                    graph[i].insert_back(j);                       //insert into the list
                    edge++;                                        //count the edge of the graph
                }
            }
        }
    }
```
## Constructed a Tree by Breadth-First Search(BFS)
- 此副程式是我們的程式中許多副程式的核心邏輯
- 尋找兩點最短距離、建構一顆樹 by BFS 並給出第幾level有幾個點、有幾個 connected components，都是以此副程式為基礎去完成的
- Tree constructed by BFS 每個 level 有幾個點等於上個 level 推入幾個點進 quene 
- 我們用`pre_push_in_times`、`now_push_in_times`來記錄上次推入幾個點進 quene ，和這次推入幾個點進 quene
- 時間複雜度 *big-O* 等於 *O(v+e)* (v = number of vertices , e = number of edges)
```cpp
void bfs(int vertex, adjList *graph)
{
    bool *visit = new bool[number_of_vertex];                //an array to know whether the vertix is visited before or not
    queue_round<int> quene;                                  //we need a quene to complete this task
    adjList::node *temp;                        
    int now_push_in_times = 0, pre_push_in_times = 0;        //counstrut the tree
    for (int i = 0; i < number_of_vertex; i++)
        visit[i] = false;
    cout << vertex << "->";
    visit[vertex] = true;
    quene.push(vertex);
    now_push_in_times = 0;
    pre_push_in_times = 1;
    while (!quene.isEmpty())
    {
        for (int i = 0; i < pre_push_in_times; i++)
        {
            vertex = quene.pop();
            for (temp = graph[vertex].getHead(); temp; temp = temp->link)               
            {
                if (!visit[temp->data])
                {
                    cout << temp->data << "->";
                    quene.push(temp->data);
                    now_push_in_times++;                //count how many times we push into the quene in this level 
                    visit[temp->data] = true;                   
                }
            }
        }
        cout << endl;
        pre_push_in_times = now_push_in_times;                 
        now_push_in_times = 0;
    }
    cout << "end" << endl;
}
```
## 尋找兩點最短路徑長度
- 由副程式 `bfs` 為基礎修改而成
- 因為這次的圖都是 Undirected Graph 和 每條邊上的權重都是1，所以兩點 v ,u 最短路徑長度等於 L-1 (tree constructed by BFS(從vertex v 開始)，而 u 在這棵樹的第 L level 上 )
- 時間複雜度 *big-O* 等於 *O(v+e)* (v = number of vertices , e = number of edges)，最壞的情況便是 u 在樹最大 level 上 => O(v+e) ， 最好狀況便是 u 在 level 2上 => O(1)
```cpp
int short_path(adjList *graph, int v, int u)                       //begining v, distanation u short bath
{
    if(u >= number_of_vertex || u<0 || v>=number_of_vertex || v<0)//if the vertex is out of the graph we return 9999
    {
        return 9999;
    }
    bool *sure = new bool[number_of_vertex];                 //flag that identify if the vertix's shortest lengh has been figured out or not
    int *distance = new int[number_of_vertex];               //the distance from vertex v
    int now_push_in_times = 0, pre_push_in_times = 0;
    adjList::node *temp;
    int temp_vertex = 0, pre_vertex = v, now_vertex = v, dis = 0;
    queue_round<int> quene;
    for (int i = 0; i < number_of_vertex; i++)              //initial the array
    {
        distance[i] = 9999;
        sure[i] = false;
    }
    distance[v] = 0;
    sure[v] = true;
    quene.push(v);
    now_push_in_times = 0;
    pre_push_in_times = 1;
    while (!quene.isEmpty())
    {
        dis++;
        for (int i = 0; i < pre_push_in_times; i++)
        {
            temp_vertex = quene.pop();
            for (temp = graph[temp_vertex].getHead(); temp; temp = temp->link)
            {
                if (!sure[temp->data])
                {
                    quene.push(temp->data);
                    now_push_in_times++;
                    if (dis + 1 < distance[temp->data])
                        distance[temp->data] = dis;
                    sure[temp->data] = true;
                }
                if (sure[u])                  //if the distance we what to know has been figured out then return our answer
                    break;
            }
            if (sure[u])
                break;
        }
        if (sure[u])
            break;
        pre_push_in_times = now_push_in_times;
        now_push_in_times = 0;
    }
    return distance[u];
}
```
## 測試此圖形是不是樹
- 如果是樹的話，則 number of vertex 會等於 edge+1
- 時間複雜度 *big-O* 等於 *O(1)* (edge 會在 adj matrix 轉換成 adj list 中計算出來)
```cpp
bool isTree(adjList *graph) //vertex=edge+1
{
    return number_of_vertex == (edge + 1);    // edge has been calculated when we transfer adj matrix into adj list 
}
```
## 計算在 Tree constructed by BFS 的 level L 上有幾個node
- 由副程式 `bfs` 為基礎修改而成
- 基本上和副程式`short_path`一樣
- 將各個 node 是在哪個 level 存在一個陣列中，最後去記算 level L 上有幾個 node
- 時間複雜度 *big-O* 等於 *O(v+e)* (v = number of vertices , e = number of edges)
```cpp
int level_number_of_node(adjList *graph, int L)
{
    bool *visit = new bool[number_of_vertex];                                   
    int *level = new int[number_of_vertex];
    int now_push_in_times = 0, pre_push_in_times = 0;
    adjList::node *temp;
    int temp_vertex = 0, pre_vertex = 0, now_vertex = 0, dis = 1, node = 0;
    queue_round<int> quene;
    for (int i = 1; i < number_of_vertex; i++) //initial
    {
        level[i] = 9999;
        visit[i] = false;
    }
    level[0] = 1;
    visit[0] = true;
    quene.push(0);
    now_push_in_times = 0;
    pre_push_in_times = 1;
    while (!quene.isEmpty())
    {
        dis++;
        for (int i = 0; i < pre_push_in_times; i++)
        {
            temp_vertex = quene.pop();
            for (temp = graph[temp_vertex].getHead(); temp; temp = temp->link)
            {
                if (!visit[temp->data])
                {
                    quene.push(temp->data);
                    now_push_in_times++;
                    if (dis + 1 < level[temp->data])
                        level[temp->data] = dis;
                    visit[temp->data] = true;
                }
            }
        }
        pre_push_in_times = now_push_in_times;
        now_push_in_times = 0;
    }
    for (int i = 0; i < number_of_vertex; i++)//count how many node at given level L in tree construced by BFS(starting from vertex 0)
    {
        if (level[i] == L)
        {
            node++;
        }
        if (level[i] == 9999)
        {
            level[i] = -1;
        }
    }
    return node;
}
```
## 計算有幾個 Connected Components
- 由副程式 `bfs` 為基礎修改而成
- 走幾次 BFS 能把圖上所有的 vertex 走過一輪，便是有幾個 Connected Components
- 時間複雜度 big-O 等於各個Connected Components 的 BFS 總和
```cpp
int count_connected_compoment(adjList *graph)
{
    bool *visit = new bool[number_of_vertex];
    bool ok = false;
    int temp_vertex, number_of_connected_compoment = 0;
    adjList::node *temp;
    queue_round<int> quene;
    for (int i = 1; i < number_of_vertex; i++)
        visit[i] = false;
    visit[0] = true;
    quene.push(0);
    while (!ok)                                      //check whether all vertexs are visited before or not
    {
        while (!quene.isEmpty())                     //BFS
        {
            temp_vertex = quene.pop();
            if (graph[temp_vertex].getHead())
            {
                temp = graph[temp_vertex].getHead();
                do
                {
                    if (!visit[temp->data])
                    {
                        quene.push(temp->data);
                        visit[temp->data] = true;
                    }
                    temp = temp->link;
                } while (temp);
            }
            else
            {
                visit[temp_vertex] = true;
            }
        }
        ok = true;
        for (int i = 0; i < number_of_vertex; i++)//if there is still have vertex didnt be visited before then run one more time BFS
        {
            if (!visit[i])
            {
                quene.push(i);
                ok = false;
                break;
            }
        }
        number_of_connected_compoment++;
    }
    return number_of_connected_compoment;
}
```
    
## 程式控制模式
- 利用 switch-case 來提供使用者想要的功能
```cpp
while (!end)
    {
        cout << endl;
        cout << "print out all vertices for each vertex => input 1" << endl
             << "find shortest path in gragh from v to w => input 2" << endl
             << "test whether the graph is a tree of not => input 3" << endl
             << "the number of nodes at level L in a tree constructed by BFS of the gragh(start from 0) => input 4" << endl
             << "count the number of conneceted components in the grapg => input 5 " << endl
             << "exit => input -1" << endl
             << "input = ";
        cin >> input;
        cout<<endl;
        switch (input)
        {
        cas 1:
            for (int i = 0; i < number_of_vertex; i++)                                  //print out adj list
            {
                cout << i << " => ";
                graph[i].print_data();
            }
            break;
        case 2:
            /* code */
            cout << "input format v w " << endl;
            cin >> v >> w;
            cout << "the shortest path from " << v << " to " << w << " are " << short_path(graph, v, w) << " unit lenghs" << endl;
            break;
        case 3:
            /* code */
            if (isTree(graph))
                cout << "the graph is a tree" << endl;
            else
                cout << "the graph is not a tree" << endl;
            break;
        case 4:
            /* code */
            cout << "L= ";
            cin >> L;
            cout << "level = " << L << " have " << level_number_of_node(graph, L) << " nodes" << endl;
            break;
        case 5:
            /* code */
            cout << "have " << count_connected_compoment(graph) << " connected compoment" << endl;
            break;
        case -1:
            /* code */
            end = true;
            break;
        default:
            break;
        }
    }
```

## 容器結構 Class
- 此程式所用到的queue歸類在`queue.hpp`中，包含 `queue_round`。
- `queue class` 簡易佇列結構。
    - 在記憶體中是以環狀記憶體的方式實作，分為`head`與`tail`兩個寫入讀出指標。
```cpp
template<class T>
class queue_round
```
* adjList class ，包含 print_data , get_Number_of_ndoe , insert_back , getHead
  * print_data : 印出此 list 中的資料
  * get_Number_of_ndoe : 回傳有幾筆資料在此 list 中
  * insert_back : isert 資料在 list 的尾巴
  * getHead : 回傳此 list 的起始指標
```cpp
class adjList
{
public:
    class node
    {
    public:
        int data;
        node *link;
    };
    adjList()
    {
        tail = NULL;
        head = tail;
        number_of_node = 0;
    }
    void print_data(void)
    {
        node *temp = head;
        if (temp)
        {
            for (temp; temp != NULL; temp = temp->link)
            {
                cout << temp->data << "->";
            }
            cout << "NULL" << endl;
        }
        else
            cout << "empty" << endl;
    }
    inline int get_Number_of_ndoe()
    {
        return number_of_node;
    }
    void insert_back(int value)
    {
        node *insert;
        try
        {
            insert = new node;
        }
        catch (const std ::bad_alloc &e)
        {
            cout << "Allocation failed: " << e.what() << '\n';
            exit(1);
        }
        insert->data = value;
        insert->link = NULL;
        if (head)
        {
            tail->link = insert;
            tail = insert;
            number_of_node++;
        }
        else
        {
            head = insert;
            tail = insert;
            number_of_node++;
        }
    }
    node *getHead(void)
    {
        return head;
    }
private:
    node *head, *tail;
    int number_of_node;
};
```
 
- adjacent list 是用 array 存取 list 的指標來建構的
```cpp
adjList *graph = new adjList[number_of_vertex];
```

## 附加文件
- `queue.hpp` 為使用到template容器的版本，會計算各部分花費的時間。
- `project2.cpp` 主程式cpp檔
- `project2.exe` 主程式exe檔
- `README.md` 此文件的markdown版本。
- `GraphGeneratorRan` 測試資料產生執行檔
