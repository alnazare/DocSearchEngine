#include <ThreadPool.h>
#include <iostream>

ThreadPool::ThreadPool(unsigned int workersCount)
{
	unsigned int maxAvailable = std::thread::hardware_concurrency();
	if (maxAvailable == 0) maxAvailable = 1;
	if (workersCount == 0 || workersCount > maxAvailable)
		workersCount = maxAvailable;
	workers.reserve(workersCount);

	for (unsigned int i = 0; i < workersCount; i++)
	{
		workers.emplace_back([this]
			{
				std::unique_lock<std::mutex> lock(queueMutex);
				while (true)
				{
					// sleep until any action is available
					queueCondition.wait(lock, [this]
						{ return stopAll || !tasks.empty(); });

					if (stopAll && tasks.empty()) 
						return;

					// get the first task in the queue and run it from this thread
					auto task = std::move(tasks.front());
					tasks.pop();
					activeTasks++;
					lock.unlock();

					try { task(); }
					catch (const std::exception& e)
					{
						std::cerr << "Thread pool task exception: " << e.what() << '\n';
					}

					lock.lock();
					activeTasks--;
					queueCondition.notify_all();
				}
			});
	}

}
ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		stopAll = true;
	}
	queueCondition.notify_all();
	for (auto& worker : workers)
	{
		if (worker.joinable()) worker.join();
	}
}

void ThreadPool::wait()
{
	std::unique_lock<std::mutex> lock(queueMutex);
	queueCondition.wait(lock, [this]
		{ return tasks.empty() && activeTasks == 0; });
}