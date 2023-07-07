#pragma once

InterruptableThread::InterruptableThread(ThreadPool* pool, int qindex):
    m_pFlag(nullptr),
    m_thread(&InterruptableThread::startFunc, this, pool, qindex) {
    }
void InterruptableThread::startFunc(ThreadPool* pool, int qindex) {
    {
        lock_guard<mutex> l(m_defender);
        m_pFlag = &thread_interrupt_flag;
    }
    pool->threadFunc(qindex);
    {
        lock_guard<mutex> l(m_defender);
        m_pFlag = nullptr;
    }
}
void ThreadPool::start() {
    for (int i = 0; i < m_thread_count; i++) {
        m_threads.push_back(new InterruptableThread(this, i));
    }
}void taskFunc(int id, int delay) {
    for (int i = 0; i < delay; i++) {
        if (InterruptableThread::checkInterrupted()) {
            unique_lock<mutex> l(coutLocker);
            cout << "task " << id << " was interrupted" << endl;
            return;
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
    unique_lock<mutex> l(coutLocker);
    cout << "task " << id << " made by thread_id " << this_thread::get_id() << endl;
}
void ThreadPool::threadFunc(int qindex) {
    while (true) {
        if (InterruptableThread::checkInterrupted()) {
            cout << "thread was interrupted" << endl;
            return;
        }
        task_type task_to_do;
        bool res;
        int i = 0;
        for (; i < m_thread_count; i++) {
            if (res = m_thread_queues[(qindex + i) % m_thread_count].fast_pop(task_to_do))
                break;
        }
        if (!res) {
            m_thread_queues[qindex].pop(task_to_do);
        }
        else if (!task_to_do) {
            m_thread_queues[(qindex + i) % m_thread_count].push(task_to_do);
        }

        if (!task_to_do) {
            return;
        }

        task_to_do();
    }
}
}void ThreadPool::interrupt() {
    for (auto& t : m_threads) {
        t->interrupt();
    }
}
void RequestHandler::interruptPool() {
    m_tpool.interrupt();
}
//Почему код, пример 30.4 для из модуля не работает правильно ? Без аналога сделать дз невозможно.