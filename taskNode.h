#ifndef __TASKNODE_H__
#define __TASKNODE_H__
#include <cstdlib>
#include <stdexcept>

#include "task.h"

typedef enum __PRIORITYS_ID__ : size_t
{
	LOWEST_PRIORITY,
	BELOW_NORMAL_PRIORITY,
	NORMAL_PRIORITY,
	HIGHER_NORMAIL_PRIORITY,
	HIGHEST_PRIORITY,
	TIME_CRITICAL_PRIORITY
}priority_t;

class taskNode final : public task
{
private:
	mutable size_t m_ID;
	const size_t m_RawID;
	mutable priority_t m_thisPriority;

public:
	taskNode(const taskNode&) = delete;
	taskNode(taskNode&&) = delete;
	explicit taskNode(const size_t& assignedID = 0, 
					  const priority_t& assignedPriority = NORMAL_PRIORITY) noexcept
	: task()
	, m_ID(assignedID)
	, m_RawID(reinterpret_cast<size_t>(this->get_address()))
	, m_thisPriority(assignedPriority)
	{

	}
	~taskNode(void) noexcept override
	{
		m_ID = 0;
		*const_cast<size_t*>(&m_RawID) = 0;
		m_thisPriority = LOWEST_PRIORITY;
	}

	inline size_t get_ID(void) const noexcept
	{
		return m_ID;
	}
	inline void set_ID(const size_t& _ID) const noexcept
	{
		m_ID = _ID;
	}

	inline size_t get_rawID(void) const noexcept
	{
		return m_RawID;
	}

	inline priority_t get_priority(void) const noexcept
	{
		return m_thisPriority;
	}
	inline void set_priority(const priority_t& assignedPriority) const noexcept
	{
		m_thisPriority = assignedPriority;
	}
};

#endif //__TASKNODE_H__