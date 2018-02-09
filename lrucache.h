#pragma once
#include <list>
#include <unordered_map>
#include <mutex>

template<typename T, typename Value>
class LRUCache 
{
    typedef typename std::list<T> List;
    typedef typename std::list<T>::iterator ListIter;

    struct CacheItem 
    {
        CacheItem(const Value & v, const ListIter & iter) 
        {
            value = v;
            listiter = iter;
            updatetime = time(NULL);
        }
        Value value;
        ListIter listiter;
        time_t updatetime;
    };

    typedef typename std::unordered_map<T, CacheItem> HashMap;
    typedef typename std::unordered_map<T, CacheItem>::iterator HashMapIter;

    public:

        // 需要自动删除cache则设置iCacheLimit为cache总量，否则置为0
        // 需要设置cache的超时，则设置iTimeLimit为超时时间，按秒计算，否则置为0
        // 默认不自动清理cache，cache不过期
        LRUCache(int iCacheLimit = 0, int iTimeLimit = 0)
            :m_iCacheSize(0),
            m_iCacheLimit(iCacheLimit),
            m_iEraseCnt(0),
            m_iTimeLimit(iTimeLimit)
        {}

        // bUpdate，获取cache时是否更新cache访问时间，默认更新
        int GetCache(const T & t, Value & value, bool bUpdate = true) 
        {
            std::lock_guard<std::mutex> lck(mt);
            HashMapIter mapIter = m_LRUCache.find(t);

            if (mapIter == m_LRUCache.end())
                return -1;

            mapIter = m_LRUCache.find(t);

            if (mapIter != m_LRUCache.end()) 
            {
                if (m_iTimeLimit > 0) 
                {
                    time_t tNow = time(NULL);
                    if (tNow - mapIter->second.updatetime > m_iTimeLimit) 
                    {
                        return -2;
                    }
                }

                value = mapIter->second.value;

                if (bUpdate) 
                {
                    m_List.erase(mapIter->second.listiter);
                    m_List.push_front(t);
                    mapIter->second.listiter = m_List.begin();
                }

                return m_iCacheSize;
            }

            return -3;
        }

        int UpdateCache(const T & t, const Value & value) 
        {
            std::lock_guard<std::mutex> lck(mt);
            HashMapIter mapIter = m_LRUCache.find(t);
            if (mapIter != m_LRUCache.end()) 
            {
                m_List.erase(mapIter->second.listiter);
                m_List.push_front(t);
                mapIter->second.listiter = m_List.begin();
                mapIter->second.updatetime = time(NULL);
                mapIter->second.value = value;
                return m_iCacheSize;
            }
            if (m_iCacheLimit > 0) 
            {
                while (m_iCacheLimit < m_iCacheSize) 
                {
                    T t = m_List.back();
                    m_List.pop_back();
                    HashMapIter mapIter = m_LRUCache.find(t);
                    if (mapIter == m_LRUCache.end())
                        break;
                    m_LRUCache.erase(mapIter);
                    m_iCacheSize--;
                    m_iEraseCnt++;
                }
            }
            m_List.push_front(t);
            CacheItem mapItem(value, m_List.begin());
            m_LRUCache.insert(std::make_pair(t, mapItem));
            m_iCacheSize++;
            return m_iCacheSize;
        }

        int PopCache(T & t, Value & value) 
        {
            std::lock_guard<std::mutex> lck(mt);
            if (m_iCacheSize == 0)
                return -1;

            t = m_List.back();
            m_List.pop_back();
            HashMapIter mapIter = m_LRUCache.find(t);
            if (mapIter == m_LRUCache.end())
                return -2;

            value = mapIter->second.value;
            m_LRUCache.erase(mapIter);
            m_iCacheSize--;
            return 0;
        }

        inline int CacheSize() 
        {
            return m_iCacheSize;
        }

    private:
        List m_List;
        HashMap m_LRUCache;
        int m_iCacheSize;
        int m_iCacheLimit;
        int m_iEraseCnt;
        int m_iTimeLimit;
        std::mutex mt;
};
