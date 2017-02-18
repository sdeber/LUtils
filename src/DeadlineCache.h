#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/optional.hpp>
#include <boost/chrono.hpp>
#include <boost/utility.hpp>
#include "CacheSink.h"


template <typename Key, typename Value, typename Validator = void> class DeadlineCache
{
private:

    struct Entry
    {

        Entry(const Key& id, const boost::chrono::steady_clock::duration& ttl) : m_id(id), m_ttl(ttl)
        {
            m_expireTime = boost::chrono::steady_clock::now() + m_ttl;
        }

        mutable boost::chrono::steady_clock::duration m_ttl;
        mutable boost::chrono::steady_clock::time_point m_expireTime;


        Key m_id;
        Value m_value;

        void UpdateTTL(const boost::chrono::steady_clock::duration& ttl) const
        {
            m_ttl = ttl;
        }

        template <typename T = Validator>
        bool IsValid(const boost::chrono::steady_clock::time_point& now,
                     typename boost::enable_if<boost::is_void<T>>::type* dummy = 0) const
        {
            return now < m_expireTime;
        }

        template <typename T = Validator>
        bool IsValid(const boost::chrono::steady_clock::time_point& now,
                     typename boost::disable_if<boost::is_void<T>>::type* dummy = 0) const
        {
            return (now < m_expireTime) || Validator(m_value);
        }
    };

    class ExtendLife
    {
    private:
        boost::chrono::steady_clock::duration m_ttl;
    public:

        ExtendLife(const boost::chrono::steady_clock::duration& ttl) : m_ttl(ttl)
        {

        }

        void operator()(Entry& entry)
        {
            entry.m_expireTime += m_ttl;
        }
    };

    class RefreshEntry
    {
    private:
        boost::chrono::steady_clock::duration m_ttl;
        const Value& m_value;
    public:

        RefreshEntry(const boost::chrono::steady_clock::duration ttl, const Value& value)
        : m_ttl(ttl), m_value(value)
        {

        }

        void operator()(Entry& entry)
        {
            boost::system::error_code ignore;
            entry.m_expireTime = boost::chrono::steady_clock::now(ignore) + m_ttl;
            entry.m_value = m_value;
        }
    };

    typedef boost::multi_index_container<Entry,
    boost::multi_index::indexed_by<
    boost::multi_index::ordered_unique<boost::multi_index::member<Entry, Key, &Entry::m_id> >,
    boost::multi_index::ordered_non_unique<
    boost::multi_index::member<Entry, boost::chrono::steady_clock::time_point, &Entry::m_expireTime> >
    > > Cache;

    Cache m_cache;

    CacheSink<Key, Value> * m_pSink;

public:

    DeadlineCache()
    {
        m_pSink = NULL;
    }

    void SetSink(ScaleCacheSink<Key, Value>* pSink)
    {
        m_pSink = pSink;
    }
    // Insert a new entry to the cache

    bool Insert(const Key& key,
                const boost::chrono::steady_clock::duration& ttl,
                const Value& value,
                bool replace_if_exists = false)
    {
        auto it = m_cache.template get<0>().find(key);
        if (it != m_cache.template get<0>().end())
        {
            if (replace_if_exists)
            {
                m_cache.template get<0>().modify(it, RefreshEntry(ttl, value));
                return true;

            }
            else
                return false;
        }
        else
        {
            Entry e(key, ttl);
            e.m_value = value;
            m_cache.insert(e);
            return true;
        }
    }

    // Remove an existing entry from the cache

    bool Remove(const Key& key)
    {
        auto it = m_cache.template get<0>().find(key);
        if (it != m_cache.template get<0>().end())
        {
            m_cache.erase(it);
            return true;
        }
        return false;
    }

    // Get an existing entry from the cache

    boost::optional<Value> Get(const Key& key)
    {
        boost::optional<Value> optValue;
        auto it = m_cache.template get<0>().find(key);
        if (it != m_cache.template get<0>().end())
        {
            boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now();
            if (it->IsValid(now))
            {
                optValue = it->m_value;
            }
        }
        return optValue;
    }
    // Reinsert an existing entry. 

    bool ExtendLife(const Key& key, const boost::chrono::steady_clock::duration& ttl)
    {
        auto it = m_cache.template get<0>().find(key);
        if (it != m_cache.template get<0>().end())
        {
            auto iter = m_cache.project<1>(it);
            m_cache.modify(iter, ExtendLife(ttl));
            return true;
        }
        return false;
    }

    int Expire(unsigned int limit = 10000)
    {
        boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now();
        unsigned int count = 0;
        auto it_lower = m_cache.template get<1>().lower_bound(boost::chrono::steady_clock::time_point::min());
        auto it_upper = m_cache.template get<1>().upper_bound(now);
        while ((it_lower != it_upper))
        {
            if (++count <= limit)
            {
                if (!it_lower->IsValid(now))
                {
                    if (m_pSink != NULL)
                        m_pSink->OnEntryExpire(it_lower->m_id, it_lower->m_value);
                    m_cache.template get<1>().erase(it_lower++);
                }
            }
        }

        return count;
    }

    void Clear()
    {
        m_cache.clear();
    }

};
