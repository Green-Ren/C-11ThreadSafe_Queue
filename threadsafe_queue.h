/************************************************************************/
/* threadsafe_queue.h
*  Created on : 2018��5��16��
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
	* �̰߳�ȫ����
	* TΪ����Ԫ������
	* ��Ϊ��std::mutex��std::condition_variable���Ա�����Դ��಻֧�ָ��ƹ��캯��
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
		* ʹ�õ�����Ϊ�������캯����������������������
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
		* ʹ�ó�ʼ���б�Ϊ�����Ĺ��캯��
		*/
		threadsafe_queue(std::initializer_list<value_type> list) :threadsafe_queue(list.begin(), list.end()) {}

		/*
		* ��Ԫ�ؼ������
		**/
		void push(const value_type &new_value)
		{
			std::lock_guard<std::mutex> lk(m_data_mutex);
			m_data_queue.push(std::move(new_value));
			m_data_cond.notify_one();
		}

		/*
		* �Ӷ����е���һ��Ԫ�أ��������Ϊ�վ�����
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
		* �Ӷ����е���һ��Ԫ�أ��������Ϊ�վͷ���false
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
		* ���ض����Ƿ�Ϊ��
		*/
		auto empty() const -> decltype(m_data_queue.empty())
		{
			std::lock_guard<std::mutex> lk(m_data_mutex);
			return m_data_queue.empty();
		}

		/*
		* ���ض�����Ԫ�ظ���
		*/
		auto size() const -> decltype(m_data_queue.size())
		{
			std::lock_guard<std::mutex> lk(m_data_mutex);
			return m_data_queue.size();
		}
	};/*threadsafe_queue*/
}/*namespace RGR*/