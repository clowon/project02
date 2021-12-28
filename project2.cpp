
// include 
#include <iostream>
#include <fstream>
#include "quene.hpp"
#include <iomanip>      //I/O format
//-----------------------------------------------------------------------------------------------------------------------------//
// using
using std::bad_alloc;
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::fstream;
using std::ios;
using std::left;
using std::right;
using std::setw;
//-----------------------------------------------------------------------------------------------------------------------------//
// define data type list
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
//-----------------------------------------------------------------------------------------------------------------------------//
// declare functions and global variable
int **make_2d_array(int rows, int cols);
void bfs(int vertex, adjList *graph);
int count_connected_compoment(adjList *graph);
bool isTree(adjList *graph);
int short_path(adjList *graph, int v, int u);
int level_number_of_node(adjList *graph, int L);
int number_of_vertex = 0,edge=0;
//-----------------------------------------------------------------------------------------------------------------------------//
// main function
int main()
{
    // define the variable needed
    int **matrix;
    int row = 0, col = 0;
    int input, v, w, L;
    bool end = false;
    char buf[256];
    //-----------------------------------------------------------------------------------------------------------------------------//
    // input the matrix
    cout << "no input files, using manual input" << endl;
    cin >> buf;
    fstream test(buf, ios::in); 								                          //open file
    if (!test) 													                          //check file open or not
    {
        cerr << "File cant be opened" << endl;
        exit(1);
    }
    while (test >> input) 										                          //get row and column
    {
        col++;
        if (test.peek() == ' ')									                          // ignore the space before newline
            test.get();
        if (test.peek() == '\n' || test.peek() == '\r')			                          // for Win32, the end of line will be "\r\n",
            row++;									       		                          // but for linux or macOS, that will be "\n"
    }
    col /= row;													                          // total / rows = cols; obviously
    matrix = make_2d_array(row, col); 							                          //create 2D array
    test.clear();                     							                          //reset the file-postion pointer(seek get)
    test.seekg(0, test.beg);          							                          //reset the file-postion pointer(seek get)
    input = 0;
    int i=0,j=0;
    while (test >> input) 										                          //read the data
    {
        matrix[i][j] = input;
        j++;
        if (j == row)
        {
            i++;
            j = 0;
        }
    }
    //-----------------------------------------------------------------------------------------------------------------------------//
    // initial the data
    number_of_vertex = row;                                                                // get the number of vertex
    cout << "number_of_vertex= " << number_of_vertex << endl;
    adjList *graph = new adjList[number_of_vertex];				                           // create the adjlist
    for (int i = 0; i < row; i++)                                                          // adj matrix ->adj list
    {
        for (int j = 0; j < col; j++)
        {
            if (matrix[i][j])                                                              // insert if matrix[i][j] != 0
            {
                if (i != j)                                                                // dont insert if itselt go to itself
                {
                    graph[i].insert_back(j);                                               // insert into the list
                    edge++;                                                                // count the edge of the graph
                }
            }
        }
    }
    edge/=2;
    //-----------------------------------------------------------------------------------------------------------------------------//
    while (!end)
    {
        cout << endl;
        cout << "print out all adjacent vertices for each vertex => input 1"<<endl
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
        case 1:
            cout<<"all adjacent vertex or each vertex (using adjacent list) "<<endl;
            for(int i=0; i<number_of_vertex;i++)
            {
                cout<< i << " => ";
                graph[i].print_data();
            }
            break;
        case 2:
            cout << "input format v w " << endl;
            cin >> v >> w;
            cout << "the shortest path from " << v << " to " << w << " are " << short_path(graph, v, w) << " unit lenghs" << endl;
            break;
        case 3:
            if (isTree(graph))
                cout << "the graph is a tree" << endl;
            else
                cout << "the graph is not a tree" << endl;
            break;
        case 4:
            cout << "L= ";
            cin >> L;
            cout << "level = " << L << " have " << level_number_of_node(graph, L) << " nodes" << endl;
            break;
        case 5:
            cout << "have " << count_connected_compoment(graph) << " connected compoment" << endl;
            break;
        case -1:
            end = true;
            break;
        default:
            break;
        }
    }
    return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------//
// defining the functions
int level_number_of_node(adjList *graph, int L)
{
    bool    *visit =   new bool[number_of_vertex];
    int     *level =   new int[number_of_vertex];
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
    for (int i = 0; i < number_of_vertex; i++)
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
int short_path(adjList *graph, int v, int u) //begining v, distanation u short bath
{
    if(u >= number_of_vertex || u<0 || v>=number_of_vertex || v<0)
    {
        return 9999;
    }
    bool *sure = new bool[number_of_vertex];
    int *distance = new int[number_of_vertex];
    int now_push_in_times = 0, pre_push_in_times = 0;
    adjList::node *temp;
    int temp_vertex = 0, pre_vertex = v, now_vertex = v, dis = 0;
    queue_round<int> quene;
    for (int i = 0; i < number_of_vertex; i++) //initial
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
                if (sure[u])
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
void bfs(int vertex, adjList *graph)
{
    bool *visit = new bool[number_of_vertex];                       //an array to know whether the vertix is visited before or not
    queue_round<int> quene;                                         //we need a quene to complete this task
    adjList::node *temp;                        
    int now_push_in_times = 0, pre_push_in_times = 0;               //counstrut the tree
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
                    now_push_in_times++;                        //count how many times we push into the qquene in this level 
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
bool isTree(adjList *graph) //vertex=edge+1
{
    return number_of_vertex == (edge + 1);
}
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
    while (!ok)
    {
        while (!quene.isEmpty())
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
        for (int i = 0; i < number_of_vertex; i++)
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
int **make_2d_array(int rows, int cols)
{
    int **x;
    x = new int *[rows];
    for (int i = 0; i < rows; i++)
        x[i] = new int[cols];
    return x;
}