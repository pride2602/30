#include "thread_pool.h"


int main() {
    srand(0);
    RequestHandler rh;
    for (int i = 0; i < 20; i++)
        rh.pushRequest(taskFunc, i, 1 + rand() % 4);
    this_thread::sleep_for(chrono::seconds(1));
    rh.interruptPool();
    return 0;
}
//Почему код, пример 30.4 для из модуля не работает правильно ? Без аналога сделать дз невозможно.