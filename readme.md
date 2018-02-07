## lruCache

A LruCache Written By C++ 11

### 原理

* LRU 保存一个缓存的链表，新加入的数据会放到链表的头部，命中缓存的数据会从其原来的位置移动到链表的头部，当 cache 数量超过限制时，通过删除链表末尾的节点，能够保持cache数量稳定。
* 使用 HashMap 存储数据
* 线程安全



### How To Use

```
g++ example.cpp -lpthread -std=c++11 -o main
```



