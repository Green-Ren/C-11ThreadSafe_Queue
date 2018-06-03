// threadsafe_queue.cpp : 定义控制台应用程序的入口点。
//
#include "threadsafe_queue.h"
#include <iostream>
#include <thread>
int main()
{
	std::cout << "threadsafe_queue test!" << std::endl;

	RGR::threadsafe_queue<size_t> myQueue;

	std::thread td01([&] {
		for (size_t i = 0; i < 10; i++)
		{
			myQueue.push(i);
		}
	});

	std::thread td02([&] {
		for (size_t i = 10; i < 20; i++)
		{
			myQueue.push(i);
		}
	});

	td01.join();
	td02.join();

	std::cout << "myQueue, size: " << myQueue.size() << std::endl;

	size_t myQueueLen = myQueue.size();
	for (size_t i = 0; i < myQueueLen; i++)
	{
		std::cout << myQueue.wait_and_pop() << " size: " << myQueue.size() << std::endl;
	}

	std::getchar();
    return 0;
}

