#ifndef __TASK_H__
#define __TASK_H__
#include <cstdlib>
#include <stdexcept>
#include <thread>
#include <atomic>
#include <chrono>

#include "semaphore.h"

constexpr signed int THREAD_STARTUP(15);

class task
{
private:
	static size_t s_numTasks;
	const void* m_pvoidThis_address;
	semaphore* m_psemaTask_run;
	std::atomic<bool>* m_paboolRun_thread;
	mutable std::thread* m_pthreadWorker;

	template <typename __obj, 
			  typename __func, 
			  typename ...__params>
	void run_member_function(__obj* __restrict __object, 
							 __func&& __restrict __function,
							 __params& ...__args) const noexcept
	{
		do {
			m_psemaTask_run->semaWait();
			if (m_paboolRun_thread->load())
				(__object->*__function)(__args...);
		} while (m_paboolRun_thread->load());
	}
	
	template <typename __func, 
			  typename ...__params>
	void run_standalone_function(__func&& __restrict __function,
								 __params& ...__args) const noexcept
	{
		do {
			m_psemaTask_run->semaWait();
			if (m_paboolRun_thread->load())
				__function(__args...);
		} while (m_paboolRun_thread->load());
	}

	inline void stop_thread(void) const noexcept
	{
		m_paboolRun_thread->store(false);
		m_psemaTask_run->semaNotify();
	}

protected:
	const void* get_address(void) const noexcept
	{
		return m_pvoidThis_address;
	}

public:
	task(const task&) = delete;
	task(task&&) = delete;
	explicit task(void) noexcept(false)
	: m_pvoidThis_address(reinterpret_cast<void*>(this))
	, m_psemaTask_run(nullptr)
	, m_paboolRun_thread(nullptr)
	, m_pthreadWorker(nullptr)
	{
		try {
			m_psemaTask_run = new semaphore;
		}
		catch (const std::bad_alloc & allocError) {
			throw allocError;
		}

		try {
			m_paboolRun_thread = new std::atomic<bool>(false);
		}
		catch (const std::bad_alloc & allocError) {
			throw allocError;
		}

		s_numTasks++;
	}
	virtual ~task(void) noexcept
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_STARTUP));
		this->stop_thread();

		if (m_pthreadWorker)
		{
			m_pthreadWorker->join();
			delete m_pthreadWorker;
			m_pthreadWorker = nullptr;
		}

		delete m_psemaTask_run;
		m_psemaTask_run = nullptr;

		delete m_paboolRun_thread;
		m_paboolRun_thread = nullptr;

		s_numTasks--;
	}

	task& operator =(const task&) = delete;
	task& operator =(task&&) = delete;

	template <typename __obj, 
			  typename __func, 
			  typename ...__params>
	void install_member_task(__obj* __restrict __object, 
							 __func&& __restrict __function,
							 __params& ...__args) const noexcept
	{
		auto __lambdaExpression([&](void)mutable noexcept->void { this->run_member_function(__object, __function, __args...); });

		if (m_paboolRun_thread->load())
		{
			this->stop_thread();
			m_pthreadWorker->join();
		}

		if (m_pthreadWorker)
			delete m_pthreadWorker;
		m_pthreadWorker = nullptr;

		m_paboolRun_thread->store(true);
		m_pthreadWorker = new std::thread(__lambdaExpression);

		std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_STARTUP));
	}
	template <typename __func, 
			  typename ...__params>
	void install_standalone_task(__func&& __restrict __function,
								 __params& ...__args) const noexcept
	{
		auto __lambdaExpression([&](void)mutable noexcept->void { this->run_standalone_function(__function, __args...); });

		if (m_paboolRun_thread->load())
		{
			this->stop_thread();
			m_pthreadWorker->join();
		}

		if (m_pthreadWorker)
			delete m_pthreadWorker;
		m_pthreadWorker = nullptr;

		m_paboolRun_thread->store(true);
		m_pthreadWorker = new std::thread(__lambdaExpression);

		std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_STARTUP));
	}

	inline void run_task(void) const noexcept
	{
		m_psemaTask_run->semaNotify();
	}

	static size_t number_created_task(void) noexcept
	{
		return s_numTasks;
	}
};

#endif //__TASK_H__