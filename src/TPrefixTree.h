#pragma once

#include <vector>
#include <type_traits>

template <typename NodeType, typename ValueType, typename Enable = void> class TPrefixTree
{
private:
    NodeType m_root;
    std::vector<ValueType> m_values;
    typedef typename NodeType::String String;
    typedef typename NodeType::Character Character;

public:
    
    bool SearchPrefix(const String& key, ValueType& value) const
    {
	void * index;
	const NodeType * pNode = &m_root, * pNext = nullptr;
	typename String::const_iterator it = key.cbegin(), end = key.cend();

	for(; it != end; ++it)
	{
	    pNext = pNode->findChild(*it);
	    if(pNext != nullptr)
	    {
		if(pNext->getData(index))
		{
		    value = m_values[(uint64_t)index];
		    return true;
		}	
	    }
	    else
		break;
	    
	    pNode = pNext;
	}
	
	if(it != end)
	{
	    if(pNode->hasTail())
	    {
		int ret = pNode->compareTail(it, end);
		if(ret >= 0)
		{
		    pNode->getTailData(index);
		    value = m_values[(uint64_t)index];
		}
		else
		    return false;
 
	    }
	    else
		return false;
	}
	
	return false;
    }

    void Insert(const String& key, const ValueType& value)
    {
	NodeType * pCurrentNode = &m_root, * pNext = nullptr;	
	m_values.push_back(value);
	typename String::const_iterator end = key.cend(), it = key.cbegin();

	for(; it != end; ++it)
	{
	    pNext = pCurrentNode->findChild(*it);
	    if(pNext == nullptr)
	    {
		break;
	    }
	    else
	    {
		pCurrentNode = pNext;
	    }
	}
	
	if(pNext != nullptr)
	{
	    // All characters of the key have been consumed.
	    if(pCurrentNode->isLeaf())
	    {
		void * data;
		pCurrentNode->getData(data);
		m_values.erase(m_values.begin() + (uint64_t)data);
	    }
	    
	    pCurrentNode->setLeaf(true, (void*)(m_values.size()-1));
	    
	}
	else
	{
	    if(pCurrentNode->hasTail() && it != end)
	    {

		if(pCurrentNode->compareTail(it,end) == 0)
		{
		    // The new key matches exactly the tail.
		    pCurrentNode->clearTail();
		    pCurrentNode->setTail(it, end, true, (void*)(m_values.size() - 1));
		    return;
		}
		else
		{
		    // The tail does not match the remaining key exactly.
		    pCurrentNode->flattenTail(it, end, true, (void*)(m_values.size() - 1));
		}
		
	    }
	    else
	    {
		
		if(pCurrentNode->m_nodeCount == 0 && !pCurrentNode->isLeaf())
		{   
		    pCurrentNode->setTail(it, end, true, (void*)(m_values.size() - 1));
		}
		else
		{
		    pCurrentNode = pCurrentNode->addChild(*(it++));
		    if(it != end)
		    {
			pCurrentNode->setTail(it, end, true, (void*)(m_values.size() - 1));
		    }
		    else
		    {
			pCurrentNode->setLeaf(true, (void*)(m_values.size() - 1));	
		    }
		    
		}
	    }
	    
	}
		
    }

    bool Search(const String& key, ValueType& value) const
    {
	void * index;
	const NodeType * pNode = &m_root, * pNext = nullptr;
      
	typename String::const_iterator it = key.cbegin(), end = key.cend();
	for(; it != end; ++it)
	{
	    pNext = pNode->findChild(*it);
	    if(pNext == nullptr)
	    {
		break;
	    }
	    pNode = pNext;
	}
		
	if(pNext == nullptr)
	{
	    if(pNode->hasTail())
	    {
		if(pNode->compareTail(it, end) == 0)
		{
		    pNode->getTailData(index);
		    value = m_values[(uint64_t)index];
		    return true;
		}
	    }
	    return false;
	}
	else if(pNode->getData(index))
	{
	    value = m_values[(uint64_t)index];
	    return true;
	}
	
	return false;	
    }

    bool SearchLongestPrefix(const String& key, ValueType& value) const
    {
	void * index;
	const NodeType * pCurrent = &m_root, *pNext = nullptr, * pResult = nullptr;
	typename String::const_iterator it = key.cbegin(), end = key.cend();

	for(; it != end; ++it)
	{
	    pNext = pCurrent->findChild(*it);
	    if(pNext != nullptr)
	    {
		if(pNext->isLeaf())
		{
		    pResult = pNext;
		}
	    }
	    else
	    {
		break;
	    }
	    pCurrent = pNext;
	}
	
	if(it != end)
	{
	    if(pCurrent->compareTail(it, end) >= 0)
	    {
		pCurrent->getTailData(index);
		value = m_values[(uint64_t)index];
		return true;
	    }
	}
	
	if(pResult != nullptr)
	{
	    if(pResult->getData(index))
	    {
		value = m_values[(uint64_t)index];
		return true;
	    }
	}
	
	return false;
    }

    bool Remove(const String& key)
    {
	NodeType * pCurrent = &m_root, * pNext = nullptr, * pDeletionPoint = nullptr;
	int deletion_char_index = -1;
	int i = 0;
	typename String::const_iterator it = key.cbegin(), end = key.cend(), deleteChar = key.end();
	for(; it != end; ++it)
	{
	    if(pNext != nullptr)
	    {
		pCurrent = pNext;
	    }
	    
	    pNext = pCurrent->findChild(*it);
	    
	    if(pNext == nullptr)
	    {
		if(pCurrent->compareTail(it, end) == 0)
		{
		    if(pDeletionPoint == nullptr)
		    {
			deleteChar = it;
			pDeletionPoint = pCurrent;   		    
		    }

		    pDeletionPoint->deleteChild(*deleteChar);
		    return true;
		}
		
		return false;
	    }
	    else
	    {
		auto next_it = it + 1;
		if(pCurrent->compareTail(next_it, end) == 0)
		{
		    if(pDeletionPoint == nullptr)
		    {
			deleteChar = it;
			pDeletionPoint = pCurrent;   		    
		    }

		    pDeletionPoint->deleteChild(*deleteChar);
		    return true;
		}
		
		if(pNext->m_nodeCount == 1 && !pNext->isLeaf())
		{
		    if(pDeletionPoint == nullptr)
		    {
			deleteChar = it;
			pDeletionPoint = pCurrent;
		    }
		}
		else
		{
		    pDeletionPoint = nullptr;
		}
	    }

	}
	
	if(pDeletionPoint != nullptr)
	{
	    pDeletionPoint->deleteChild(*deleteChar);
	    return true;
	}

	if(pNext != nullptr)
	{
	    if(pNext->isLeaf())
	    {
		pNext->clearData();
		return true;
	    }
	}

	return false;
	
    }

};


template <typename NodeType, typename ValueType>
class TPrefixTree<NodeType, ValueType, typename std::enable_if<std::is_void<ValueType>::value>::type>
{
private:
    NodeType m_root;
    typedef typename NodeType::String String;
    typedef typename NodeType::Character Character;

public:
    
    bool SearchPrefix(const String& key) const
    {
	const NodeType * pNode = &m_root, * pNext = nullptr;
	typename String::const_iterator it = key.cbegin(), end = key.cend();

	for(; it != end; ++it)
	{
	    pNext = pNode->findChild(*it);
	    if(pNext != nullptr)
	    {
		if(pNext->isLeaf())
		{
		    return true;
		}	
	    }
	    else
		break;
	    
	    pNode = pNext;
	}
	
	if(it != end)
	{
	    if(pNode->compareTail(it, end) >= 0)
		return true;
	    else
		return false;
	}
	
	return false;
    }

    void Insert(const String& key)
    {
	NodeType * pCurrentNode = &m_root, * pNext = nullptr;	
	typename String::const_iterator end = key.cend(), it = key.cbegin();

	for(; it != end; ++it)
	{
	    pNext = pCurrentNode->findChild(*it);
	    if(pNext == nullptr)
	    {
		break;
	    }
	    else
	    {
		pCurrentNode = pNext;
	    }
	}
	
	if(pNext != nullptr)
	{
	    // All characters of the key have been consumed.
	    pCurrentNode->setLeaf(false, nullptr);
	}
	else
	{
	    if(it != end && pCurrentNode->hasTail())
	    {
		if(pCurrentNode->compareTail(it,end) == 0)
		{
		    return;
		}
		else
		{
		    // The tail does not match the remaining key exactly.
		    pCurrentNode->flattenTail(it, end, false, nullptr);
		}
		
	    }
	    else
	    {
		
		if(pCurrentNode->m_nodeCount == 0 && !pCurrentNode->isLeaf())
		{   
		    pCurrentNode->setTail(it, end, false, nullptr);
		}
		else
		{
		    pCurrentNode = pCurrentNode->addChild(*(it++));
		    if(it != end)
		    {
			pCurrentNode->setTail(it, end, false, nullptr);
		    }
		    else
		    {
			pCurrentNode->setLeaf(false, nullptr);	
		    }
		    
		}
	    }
	    
	}
		
    }

    bool Search(const String& key) const
    {
	void * index;
	const NodeType * pNode = &m_root, * pNext = nullptr;
      
	typename String::const_iterator it = key.cbegin(), end = key.cend();
	for(; it != end; ++it)
	{
	    pNext = pNode->findChild(*it);
	    if(pNext == nullptr)
	    {
		break;
	    }
	    pNode = pNext;
	}
		
	if(pNext == nullptr)
	{
	    if(pNode->compareTail(it, end) == 0)
		return true;
	    else
		return false;
	}
	else if(pNode->isLeaf())
	{
	    return true;
	}
	
	return false;	
    }

    bool SearchLongestPrefix(const String& key) const
    {
	const NodeType * pCurrent = &m_root, *pNext = nullptr, * pResult = nullptr;
	typename String::const_iterator it = key.cbegin(), end = key.cend();

	for(; it != end; ++it)
	{
	    pNext = pCurrent->findChild(*it);
	    if(pNext != nullptr)
	    {
		if(pNext->isLeaf())
		{
		    pResult = pNext;
		}
	    }
	    else
	    {
		break;
	    }
	    pCurrent = pNext;
	}
	
	if(it != end)
	{
	    if(pCurrent->compareTail(it, end) >= 0)
	    {
		return true;
	    }
	}
	
	if(pResult != nullptr)
	{
	    return pResult->isLeaf();
	}
	
	return false;
    }

    bool Remove(const String& key)
    {
	NodeType * pCurrent = &m_root, * pNext = nullptr, * pDeletionPoint = nullptr;
	int deletion_char_index = -1;
	int i = 0;
	typename String::const_iterator it = key.cbegin(), end = key.cend(), deleteChar = key.end();
	for(; it != end; ++it)
	{
	    if(pNext != nullptr)
	    {
		pCurrent = pNext;
	    }
	    
	    pNext = pCurrent->findChild(*it);
	    
	    if(pNext == nullptr)
	    {
		if(pCurrent->compareTail(it, end) == 0)
		{
		    if(pDeletionPoint == nullptr)
		    {
			deleteChar = it;
			pDeletionPoint = pCurrent;   		    
		    }

		    pDeletionPoint->deleteChild(*deleteChar);
		    return true;
		}
		
		return false;
	    }
	    else
	    {
		auto next_it = it + 1;
		if(pNext->compareTail(next_it, end) == 0)
		{
		    if(pDeletionPoint == nullptr)
		    {
			deleteChar = it;
			pDeletionPoint = pCurrent;   		    
		    }

		    pDeletionPoint->deleteChild(*deleteChar);
		    return true;
		}
		
		if(pNext->m_nodeCount == 1 && !pNext->isLeaf())
		{
		    if(pDeletionPoint == nullptr)
		    {
			deleteChar = it;
			pDeletionPoint = pCurrent;
		    }
		}
		else
		{
		    pDeletionPoint = nullptr;
		}
	    }

	}
	
	if(pDeletionPoint != nullptr)
	{
	    pDeletionPoint->deleteChild(*deleteChar);
	    return true;
	}

	if(pNext != nullptr)
	{
	    if(pNext->isLeaf())
	    {
		pNext->clearData();
		return true;
	    }
	}

	return false;
	
    }


};
