#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include <cstdlib>
#include <stdexcept>
#include <mutex>
#include <condition_variable>
#include <atomic>

class semaphore final
{
private:
	std::mutex* m_pmtxLock;
	std::atomic<size_t>* m_patomicCount;
	std::condition_variable* m_pcvNotifier;

public:
	semaphore(const semaphore&) = delete;
	semaphore(semaphore&&) = delete;
	explicit semaphore(void) noexcept(false)
	: m_pmtxLock(nullptr)
	, m_patomicCount(nullptr)
	, m_pcvNotifier(nullptr)
	{
		try {
			m_pmtxLock = new std::mutex;
		}
		catch (const std::bad_alloc & allocError) {
			throw allocError;
		}

		try {
			m_patomicCount = new std::atomic<size_t>(0);
		}
		catch (const std::bad_alloc & allocError) {
			throw allocError;
		}

		try {
			m_pcvNotifier = new std::condition_variable;
		}
		catch (const std::bad_alloc & allocError) {
			throw allocError;
		}
	}
	~semaphore(void) noexcept
	{
		delete m_pmtxLock;
		m_pmtxLock = nullptr;

		delete m_patomicCount;
		m_patomicCount = nullptr;

		delete m_pcvNotifier;
		m_pcvNotifier = nullptr;
	}

	semaphore& operator =(const semaphore&) = delete;
	semaphore& operator =(semaphore&&) = delete;

	void semaNotify(void) const noexcept
	{
		std::unique_lock<std::mutex> _LOCK(*m_pmtxLock);
		m_patomicCount->store(m_patomicCount->load() + 1);
		m_pcvNotifier->notify_one();
	}
	void semaWait(void) const noexcept
	{
		std::unique_lock<std::mutex> _LOCK(*m_pmtxLock);
		while (m_patomicCount->load() == 0) {
			m_pcvNotifier->wait(_LOCK);
		}
		m_patomicCount->store(m_patomicCount->load() - 1);
	}
};

#endif //__SEMAPHORE_H__