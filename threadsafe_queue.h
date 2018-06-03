/************************************************************************/
/* threadsafe_queue.h
*  Created on : 2018年5月16日
*		Author: rgrui
*/
/************************************************************************/
#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "disable_copy.h"
namespace RGR
{
	/*
	* 线程安全队列
	* T为队列元素类型
	* 因为有std::mutex和std::condition_variable类成员，所以此类不支持复制构造函数
	*/
	template<typename T>
	class threadsafe_queue : private disable_copy
	{
	private:
		mutable std::mutex					m_data_mutex;
		mutable std::condition_variable		m_data_cond;
		using queue_type = std::queue<T>;
		queue_type							m_data_queue;
	public:
		using value_type = typename queue_type::value_type;
		using container_type = typename queue_type::container_type;
		threadsafe_queue() = default;
		/*
		* 使用迭代器为参数构造函数，适用于所有容器对象
		*/
		template<typename _InputIterator>
		threadsafe_queue(_InputIterator first, _InputIterator last)
		{
			for (auto itor = first; itor != last; ++itor)
			{
				m_data_queue.push(*itor);
			}
		}

		explicit threadsafe_queue(const container_type &c):m_data_queue(c){}

		/*
		* 使用初始化列表为参数的构造函数
		*/
		threadsafe_queue(std::initializer_list<value_type> list) :threadsafe_queue(list.begin(), list.end()) {}

		/*
		* 将元素加入队列
		**/
		void push(const value_type &new_value)
		{
			std::lock_guard<std::mutex> lk(m_data_mutex);
			m_data_queue.push(std::move(new_value));
			m_data_cond.notify_one();
		}

		/*
		* 从队列中弹出一个元素，如果队列为空就阻塞
		*/
		value_type wait_and_pop()
		{
			std::unique_lock<std::mutex> lk(m_data_mutex);
			m_data_cond.wait(lk, [this]{return !this->m_data_queue.empty();});
			auto value = std::move(m_data_queue.front());
			m_data_queue.pop();
			return value;
		}

		/*
		* 从队列中弹出一个元素，如果队列为空就返回false
		*/
		bool try_pop(value_type &value)
		{
			std::lock_guard<std::mutex> lk(m_data_mutex);
			if (m_data_queue.empty())
			{
				return false
			}

			value = std::move(m_data_queue.front());
			m_data_queue.pop();
			return true;
		}

		/*
		* 返回队列是否为空
		*/
		auto empty() const -> decltype(m_data_queue.empty())
		{
			std::lock_guard<std::mutex> lk(m_data_mutex);
			return m_data_queue.empty();
		}

		/*
		* 返回队列中元素个数
		*/
		auto size() const -> decltype(m_data_queue.size())
		{
			std::lock_guard<std::mutex> lk(m_data_mutex);
			return m_data_queue.size();
		}
	};/*threadsafe_queue*/
}/*namespace RGR*/