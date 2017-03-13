#pragma once

#include <chrono>
#include <atomic>


class LeakyBucket
{
public:
    virtual int32_t GetBucket() = 0;

};

template <typename Clock, typename Duration> class TLeakyBucket : public LeakyBucket
{
private:
    const int32_t m_limit;
    std::atomic<int32_t> m_currentCount;
    typename Clock::rep m_updateInterval;
    std::atomic<typename Clock::rep> m_nextUpdateTime;

    void Refresh()
    {
        typename Clock::rep now = Clock::now().time_since_epoch().count();
        typename Clock::rep oldVal = m_nextUpdateTime.load();

        if (now > oldVal)
        {
            auto d_rep = now - oldVal;
            typename Clock::rep remainder = d_rep % m_updateInterval;
            typename Clock::rep newVal = oldVal + m_updateInterval - remainder;
            if (m_nextUpdateTime.compare_exchange_strong(oldVal, newVal))
                m_currentCount.store(m_limit);
        }
    }

public:

    TLeakyBucket(int32_t limit, Duration d, bool align_from_epoch = false) : m_limit(limit) 
    {
        m_currentCount.store(m_limit);
        auto duration = std::chrono::duration_cast<typename Clock::duration > (d);
        m_updateInterval = duration.count();
	auto now = Clock::now();
	typename Clock::rep newVal;
	if(align_from_epoch)
	{
	    typename Clock::rep now_rep = now.time_since_epoch().count();
	    typename Clock::rep remainder = now_rep % m_updateInterval;
	    newVal = now_rep + m_updateInterval - remainder;
	    m_nextUpdateTime.store(newVal);   
	}
	else
	{    
	    typename Clock::time_point nextTime = now + duration;
	    newVal = nextTime.time_since_epoch().count();
	}

	m_nextUpdateTime.store(newVal);
    }

    virtual int32_t GetBucket()
    {
        Refresh();
        int currentVal = m_currentCount.load();
        if (currentVal > 0)
            /* It is still possible that fetch_sub returns a negative value,
               but in this way we can at least avoid the possibility of value overflow.
             */
            return m_currentCount.fetch_sub(1);
        else
            return 0;
    }


};
