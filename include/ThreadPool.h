#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <functional>

class ThreadPool
{
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex queueMutex;
	std::condition_variable queueCondition;

	std::atomic<bool> stopAll{ false };
	std::atomic<size_t> activeTasks{ 0 };

public:
	ThreadPool(unsigned int maxWorkers = 0);
	~ThreadPool();

	void wait();

	template <typename F>
	void addTask(F&& task)
	{
		{
			std::lock_guard<std::mutex> lock(queueMutex);
			if (stopAll) throw std::runtime_error("Trying to add a task to a destroyed ThreadPool");
			tasks.emplace(std::forward<F>(task));
		}
		queueCondition.notify_one();
	}
};

#endif // !THREADPOOL_H