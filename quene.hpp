//#ifdef QUENE.HPP
#pragma once
#include<iostream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::ios;
using std::bad_alloc;
template<class fromptr, class toptr>
toptr datacopy(fromptr first, fromptr last, toptr dest) {
    while (first != last)
        *dest++ = *first++;
    return dest;
}
template<class T>
class queue_round
{
    public:
        queue_round()
        {
            tail=0;
            head=0;
            data=new T[capacity];
        }
        ~queue_round()
        {
            if(data)
                delete []data;
        }
        inline bool isEmpty()
        {
            return head==tail;
        }
        void resize_reserve()
        {
            T *temp;
            try
            {    
                temp=new T[capacity*2];
            }
            catch( const std :: bad_alloc & e )
            {
                cout << "Allocation failed: " << e.what() << '\n';
            }
            datacopy(data+tail,data+capacity,temp);
            datacopy(data,data+head,temp+capacity-tail);
            tail=0;
            head=capacity;
            capacity*=2;
            delete []data;
            data=temp;
            return;
        }
        bool push(T value)
        {
            if(head+1==tail)
                resize_reserve();
            data[head++]=value;
            if(head>=capacity)
                head=0;
            return true;
        }
        inline int getTail()
        {
            return tail;
        }
        inline int getHead()
        {
            return head;
        }
        T pop()
        {
            T temp;
            if(isEmpty())
                return false;
            temp=data[tail++];
            if(tail>=capacity)
                tail=0;
            return temp;
        }
        inline int size(void){
			return (tail+capacity-head) % capacity;
		}
        inline int size_data()
        {
            return sizeof(data);
        }
        inline int getCapacity()
        {
            return capacity;
        }
        inline int getsize_of_point()
        {
            return size_of_point;
        }
    private:
        int tail,head,capacity=65536,size_of_point=sizeof(T);
        T *data;
};
//#endif