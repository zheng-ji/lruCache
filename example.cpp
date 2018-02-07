#include "lrucache.h"
#include <sys/time.h>
#include <iostream>
#include <string>
#include <thread>

int Process(int32_t iKey, int32_t & iValue) {
    static LRUCache<int32_t, int32_t> tCache(100000, 3600);
    if (tCache.GetCache(iKey, iValue) > 0)
        return 1;
    iValue = random(); // 未命中cache，进行查询处理后得到结果再入cache
    tCache.UpdateCache(iKey, iValue);
    return 0;
}

void TestCacheUpdateThread() {
    int iCnt = 10000;
    int iHit = 0;
    for (auto i = 0; i < iCnt; i++) {
        int32_t iKey = random() % 200000;
        int32_t iValue = 0;
        if (Process(iKey, iValue) > 0) {
            iHit++;
        }
    }
    std::cout << "Process cnt:" << iCnt << " hitcache:" << iHit << std::endl;;
}

int main(int argc, char ** argv) {
    struct timeval tStart;
    gettimeofday(&tStart, NULL);

    srand(time(NULL));

    std::thread threads[50];
    for (auto i = 0; i < 50; i++) {
        threads[i] = std::thread(TestCacheUpdateThread);
    }
    for (auto & th : threads) {
        th.join();
    }

    struct timeval tEnd;
    gettimeofday(&tEnd, NULL);
    std::cout << "cost" << 1000*(tEnd.tv_sec-tStart.tv_sec) + (tEnd.tv_usec-tStart.tv_usec)/1000 << " ms" << std::endl;

    return 0;
}
