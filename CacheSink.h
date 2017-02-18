#pragma once

template <typename Key, typename Value> class CacheSink
{
public:
    virtual void OnEntryExpire(const Key& key, const Value& value) = 0;
};
