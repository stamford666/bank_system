// -lpthread 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#define COUNT 10000

int nLoop0 = 0, nLoop1 = 0; 
int nTemp1, nTemp2, nRandom;
int nAccount1 = 0, nAccount2 = 0;

// 互斥锁和条件变量
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
int turn = 0; 

void* Fun0(void* arg) {
    do {
        pthread_mutex_lock(&account_mutex); // 锁定共享资源

        while (turn != 0) { // 等待轮到 Fun0
            pthread_cond_wait(&cond_var, &account_mutex);
        }

        nRandom = rand() % 1000; // 随机转账金额
        nTemp1 = nAccount1;
        nTemp2 = nAccount2;

        // 输出当前账户资金
        printf("Fun0:\n");
        printf("nAccount1当前资金为: %d\n", nTemp1);
        printf("nAccount2当前资金为: %d\n", nTemp2);

        nAccount1 = nTemp1 + nRandom; // 账户1转入
        printf("nAccount1收到资金 %d,现资金为: %d\n", nRandom, nAccount1);
        nAccount2 = nTemp2 - nRandom; // 账户2转出
        printf("nAccount2转出资金 %d,现资金为: %d\n", nRandom, nAccount2);

        nLoop0++; // 转账次数+1
        turn = 1; // 切换到 Fun1
        pthread_cond_signal(&cond_var); // 唤醒 Fun1

        pthread_mutex_unlock(&account_mutex); // 解锁
    } while (((nAccount1 + nAccount2) == 0) && (nLoop0 < COUNT));
    
    printf("Fun0循环次数为 %d 次\n", nLoop0);
    return NULL;
}

void* Fun1(void* arg) {
    do {
        pthread_mutex_lock(&account_mutex); // 锁定共享资源

        while (turn != 1) { // 等待轮到 Fun1
            pthread_cond_wait(&cond_var, &account_mutex);
        }

        nRandom = rand() % 1000; // 随机转账金额
        nTemp1 = nAccount1;
        nTemp2 = nAccount2;

        // 输出当前账户资金
        printf("Fun1:\n");
        printf("nAccount1当前资金为: %d\n", nTemp1);
        printf("nAccount2当前资金为: %d\n", nTemp2);

        nAccount1 = nTemp1 + nRandom; // 账户1转入
        printf("nAccount1收到资金 %d,现资金为: %d\n", nRandom, nAccount1);
        nAccount2 = nTemp2 - nRandom; // 账户2转出
        printf("nAccount2转出资金 %d,现资金为: %d\n", nRandom, nAccount2);

        nLoop1++; // 转账次数+1
        turn = 0; // 切换到 Fun0
        pthread_cond_signal(&cond_var); // 唤醒 Fun0

        pthread_mutex_unlock(&account_mutex); // 解锁
    } while (((nAccount1 + nAccount2) == 0) && (nLoop1 < COUNT));
    
    printf("Fun1循环次数为 %d 次\n", nLoop1);
    return NULL;
}

int main() {
    pthread_t threads[2];
    clock_t starttime, endtime;

    srand((unsigned int)time(NULL)); // 初始化随机种子

    starttime = clock(); // 开始时间
    pthread_create(&threads[0], NULL, Fun0, NULL); // 创建线程
    pthread_create(&threads[1], NULL, Fun1, NULL);
    
    pthread_join(threads[0], NULL); // 等待线程结束
    pthread_join(threads[1], NULL);
    
    endtime = clock(); // 结束时间
    printf("运行时间为：%ld ms\n", (endtime - starttime) * 1000 / CLOCKS_PER_SEC);

    return 0;
}
