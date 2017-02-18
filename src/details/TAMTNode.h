#pragma once
#include "AMTCommon.h"
#include "BitMaps.h"
#include "String.h"
/**
   A node can have four possible states: 
   1. It carries data only. 
   2. It carries both data and children.
   3. Children only.
   4. It carries tails.

   When a node carries data, the most siginificant bit of its m_ch is set to one, i.e. in case 1 and 2. If the node has no
   chidlren, then the index base pointer is used to store the data. Otherwise, extra spaces of 8 bytes are allocated on
   the children array to store it. 

   When the node has children or it carries a tail, then the node count/tail size is non-zero. In case of a tail is carried,
   the tail flag on the index base pointer is set.

   This version seems to have some problems with cache locality. A further improvement may be done by introducing customized 
   (consecutive) memeory allocation to reduce the number of cache misses especially during key lookup. More experiments are
   needed. 

   The node needs a convertion function to convert a character to its index in the alphabet.
**/

template <typename CharT, typename _string, unsigned int cardinality> struct Alphabet
{
    typedef CharT Character;
    typedef _string String;
    
    static unsigned int to_index(Character c)
    {
	return -1;
    }

    static int to_index_array(typename String::const_iterator& begin,
			      typename String::const_iterator& end,
			      uint8_t * array,
			      unsigned int arrayLen)
    {
	return -1;
    }

};

template <typename Alphabet, typename Index> class TAMTNode
{

public:
    typedef Alphabet Alphabet_t;
    typedef typename Alphabet::Character Character;
    typedef typename Alphabet::String String;
private:
    bool setTail(const uint8_t * pTail, unsigned int len)
    {
	if(!isEmpty())
	    return false;
	
	pIndexBase = new uint8_t[len];
	memcpy(pIndexBase, pTail,len);
	AMTNODE_SET_TAIL(pIndexBase);
	m_tailSize = len;
	return true;
    }

    bool setTail(const uint8_t * pTail, unsigned int len, void * data)
    {	
	if(!isEmpty())
	    return false;
	
	pIndexBase = new uint8_t[len + sizeof(void*)];
	memcpy(pIndexBase, pTail,len);
	memcpy((uint8_t*)pIndexBase + len, &data, sizeof(void*));
	AMTNODE_SET_TAIL(pIndexBase);
	m_tailSize = len;
	return true;
    }

    TAMTNode * _findChild(unsigned int ch)
    {
	int index = m_nodeIdx.find(ch);
	if(index != -1)
	{
	    return (TAMTNode*)AMTNODE_GET_INDEX(pIndexBase) + index;
	}
	return nullptr;
    }
    
    TAMTNode * _addChild(unsigned int ch)
    {
	
	void * pOldBase = nullptr;
	unsigned int dataOffset = 0; 
	unsigned int newSize = 0; 
	TAMTNode * pNewNode = nullptr;

	unsigned int dataSize = AMTNODE_HAS_DATA(pIndexBase) == 3 ? sizeof(void*) : 0;
	
	if(m_nodeCount == 0)
	{     
	    pOldBase = pIndexBase;
	    pIndexBase = new char[dataSize + sizeof(TAMTNode)];
	    ++m_nodeCount;
	    m_nodeIdx.set(ch);
	    pNewNode = new (pIndexBase) TAMTNode;
	    pNewNode->m_ch = static_cast<uint8_t>(ch);
	    if(dataSize > 0)
		memcpy((TAMTNode*)pIndexBase + 1, &pOldBase,dataSize);
	    AMTNODE_COPY_FLAGS(pOldBase, pIndexBase);
	    delete[] (char*)AMTNODE_GET_INDEX(pOldBase);
	    return pNewNode;
	}
	
	int i = m_nodeIdx.find(ch);
	if(i != -1)
	    return (TAMTNode*)AMTNODE_GET_INDEX(pIndexBase) + i;

	i = m_nodeIdx.set(ch);
	pOldBase = pIndexBase;

	unsigned int before_size = sizeof(TAMTNode)*i;
	unsigned int after_size = (i < m_nodeCount? sizeof(TAMTNode)*(m_nodeCount - i) : 0) + dataSize;

	pIndexBase = new char[(m_nodeCount + 1)*sizeof(TAMTNode) + dataSize ];
	
	if(before_size > 0)
	{
	    memcpy(pIndexBase, AMTNODE_GET_INDEX(pOldBase), before_size);
	}

	if(after_size > 0)
	{
	    memcpy((TAMTNode*)pIndexBase + i + 1, (TAMTNode*)AMTNODE_GET_INDEX(pOldBase) + i, after_size);
	}

	pNewNode = new ((TAMTNode*)pIndexBase + i) TAMTNode;
	pNewNode->m_ch = static_cast<uint8_t>(ch);
	AMTNODE_COPY_FLAGS(pOldBase, pIndexBase);
	delete[] (char*)AMTNODE_GET_INDEX(pOldBase);
	++m_nodeCount;

	return pNewNode;
	
    }

    // Delete a child branch. 
    bool _deleteChild(unsigned int ch)
    {
	int index = m_nodeIdx.find(ch);
	
	if(index != -1)
	{
	    TAMTNode * pTargetChild = (TAMTNode*)AMTNODE_GET_INDEX(pIndexBase) + index;
	    pTargetChild->clear();
	    
	    void * pOldBase = nullptr;
	    unsigned int dataOffset = 0;  
	    	
	    pOldBase = pIndexBase;

	    unsigned int dataSize = AMTNODE_HAS_DATA(pIndexBase) == 3 ? sizeof(void*) : 0;
	    unsigned int before_size = sizeof(TAMTNode)*index;
	    unsigned int after_size = sizeof(TAMTNode)*(m_nodeCount - index - 1) + dataSize;
	    unsigned int newSize = (m_nodeCount - 1)*sizeof(TAMTNode) + dataSize;

	    if(m_nodeCount > 1)
	    {
		pIndexBase = new char[newSize];
	
		if(before_size > 0)
		{
		    memcpy(pIndexBase, AMTNODE_GET_INDEX(pOldBase), before_size);
		}

		if(after_size > 0)
		{
		    memcpy((TAMTNode*)pIndexBase + index, (TAMTNode*)AMTNODE_GET_INDEX(pOldBase) + index + 1, after_size);
		}

		AMTNODE_COPY_FLAGS(pOldBase, pIndexBase);
	    }
	    else
	    {
		if(dataSize > 0)
		{
		    pIndexBase = new char[dataSize];
		    memcpy(pIndexBase,(TAMTNode*)AMTNODE_GET_INDEX(pOldBase) + 1,dataSize);
		    AMTNODE_COPY_FLAGS(pOldBase, pIndexBase);
		}
		else
		{
		    pIndexBase = nullptr;
		}
	
	    }

	    delete[] (char*)AMTNODE_GET_INDEX(pOldBase);
	    --m_nodeCount;
	    m_nodeIdx.unset(ch);
	    
	    return true;
	}
	
	return false;
    }

    bool isEmpty() const
    {
	if(isLeaf())
	    // it has data
	    return false;
	else
	{
	    if(m_nodeCount != 0)
		// it has children or tail
		return false;
	}
	   
	return true;
    }

    char * getTail(int& len)
    {
	len = m_tailSize;
	return (char*)AMTNODE_GET_INDEX(pIndexBase);	
    }

public:
    const static uint32_t MAX_TAIL_LENGTH = 255;
    
    // Store the character the node carries. Valid 'chars' should be the integers from 0 to 15. 
    uint8_t m_ch;

    // Can be the count of the children or the size of the tail. Note that the tail size cannot exceed 255
    union
    {
	uint8_t m_nodeCount;
	uint8_t m_tailSize;
    };

    //Children index
    Index m_nodeIdx;

    // This pointer can either point to the array of children or to the tail.
    void * pIndexBase;
    
    TAMTNode()
    {
	pIndexBase = nullptr;
	m_nodeCount = 0;
    }
    
    ~TAMTNode()
    {
	clear();
    }

    
    TAMTNode * findChild(const Character& c)
    {
	return _findChild(Alphabet::to_index(c));
    }

    TAMTNode * addChild(const Character& c)
    {
	return _addChild(Alphabet::to_index(c));
    }

    bool deleteChild(const Character& c)
    {
	return _deleteChild(Alphabet::to_index(c));
    }
    
    bool setTail(typename String::const_iterator it, typename String::const_iterator end, bool hasData, void * data)
    {
	uint8_t tailArray[MAX_TAIL_LENGTH];
	unsigned int actualSize = Alphabet::to_index_array(it, end, tailArray, MAX_TAIL_LENGTH);	
	// BOOST_ASSERT_MSG(actualSize <= MAX_TAIL_LENGTH, "Error! The tail cannot exceed the size ");
	if(hasData)
	    return setTail(tailArray, actualSize, data);
	return setTail(tailArray, actualSize);
    }

    int compareTail(typename String::const_iterator begin, typename String::const_iterator end)
    {
	if(hasTail())
	{
	    uint8_t tailArray[MAX_TAIL_LENGTH];
	    unsigned int remaining_key_len = Alphabet::to_index_array(begin, end, tailArray, MAX_TAIL_LENGTH);	
	    int len = 0;
	    const char * pTail = getTail(len);
	
	    if(remaining_key_len < len)
		return -1;
	
	    if(memcmp(tailArray, pTail, remaining_key_len) == 0)
	    {
		if(len == remaining_key_len)
		    return 0; // The tail is equal to the remaining part of the key
		else 
		    return 1; // The tail is a prefix of the remaining part of the key.
	    }
	}
	
	return -1;
    }
    
    bool hasTail() const
    {
	return !isLeaf() && (AMTNODE_HAS_TAIL(pIndexBase) > 0);
    }

    void flattenTail(typename String::const_iterator it, typename String::const_iterator end, bool hasData, void * data)
    {
	// The tail does not match the remaining key exactly.
	void * oldData;
	TAMTNode * pCurrentNode = this;
	unsigned int len = m_tailSize;
	uint8_t * pOldBase = (uint8_t*)AMTNODE_GET_INDEX(pIndexBase);
	pIndexBase = nullptr;
	m_nodeCount = 0;
	int k = 0;
	
	if(hasData)
	    oldData = *((void**)(&pOldBase[len]));

	for(; it != end && k < len; ++it, ++k)
	{
	    if(Alphabet::to_index(*it) != pOldBase[k])
		break;
	}
	
	// Adding common path
	for(int j = 0; j < k; ++j)
	{
	    pCurrentNode = pCurrentNode->_addChild(pOldBase[j]);
	}

	TAMTNode * pChildNodeForOldTail = nullptr, * pChildNodeForNewTail = nullptr;

	if(it != end)
	{
	    pChildNodeForNewTail = pCurrentNode->addChild(*(it++)); 
	    if(it == end)
	    {
		pChildNodeForNewTail->setLeaf(hasData, data);
	    }
	    else
	    {
		pChildNodeForNewTail->setTail(it, end, hasData, data);
	    }
	}
	else
	{
	    pCurrentNode->setLeaf(hasData,data);
	}
		
	if(k < len)
	{
	    pChildNodeForOldTail = pCurrentNode->_addChild(pOldBase[k++]);
	    if(k == len)
	    {
		pChildNodeForOldTail->setLeaf(hasData, oldData);
	    }
	    else
	    {
			   
		if(k < len)
		{
		    if(hasData)
			pChildNodeForOldTail->setTail((uint8_t*)pOldBase+k, len - k, oldData);
		    else
			pChildNodeForOldTail->setTail((uint8_t*)pOldBase+k, len - k);
		}
		else
		{   
		    pChildNodeForOldTail->setLeaf(hasData, oldData);
		}
		       
	    }
	}
	else
	{
	    pCurrentNode->setLeaf(hasData, oldData);
	}
	
	delete[] pOldBase;
    
    }
    
    void clearTail()
    {
	m_nodeCount = 0;
	delete[] (char*)AMTNODE_GET_INDEX(pIndexBase);
	pIndexBase = nullptr;
    }

    void getTailData(void*& data)
    {
	memcpy(&data, ((char*)AMTNODE_GET_INDEX(pIndexBase)) + m_tailSize, sizeof(void*)); 	
    }
        
    void setLeaf(bool hasData = false, void * data = nullptr)
    {
	if(!hasData)
	    AMTNODE_SET_LEAF(pIndexBase);
	else
	{
	    void * pOldBase = nullptr;
	    unsigned int dataOffset = 0; 
	    unsigned int newSize = 0; 

	    if(m_nodeCount == 0)
	    {
		// This node has no children, it only carries data;
		pIndexBase = new char[sizeof(void*)];
		*((void**)pIndexBase) = data;
		AMTNODE_SET_DATA(pIndexBase);
		return;
	    }
	
	    if(AMTNODE_HAS_TAIL(pIndexBase))
	    {
		dataOffset = m_tailSize;
	    }
	    else
	    {
		dataOffset = sizeof(TAMTNode)*m_nodeCount;
	    }
	
	    if(!isLeaf())
	    {
		pOldBase = pIndexBase;
		newSize = sizeof(void*) + dataOffset;
		pIndexBase = new char[newSize];
		memcpy(pIndexBase, AMTNODE_GET_INDEX(pOldBase), dataOffset);	
		AMTNODE_COPY_FLAGS(pOldBase, pIndexBase);
	    }
	
	    memcpy((char*)AMTNODE_GET_INDEX(pIndexBase) + dataOffset, &data, sizeof(void*));

	    if(pOldBase != nullptr)
		delete[] (char*)AMTNODE_GET_INDEX(pOldBase);
	
	    AMTNODE_SET_DATA(pIndexBase);
	}
    }

    bool isLeaf() const
    {
	return AMTNODE_IS_LEAF(pIndexBase);
    }
    

    // Clear the contents of the node. 
    void clear()
    {
	m_nodeIdx.clear();
	if(m_nodeCount == 0)
	{
	    return;
	}

	if(AMTNODE_HAS_TAIL(pIndexBase))
	{   
	    delete[] ((char*)AMTNODE_GET_INDEX(pIndexBase));	  
	}
	else
	{
	    // The node has children.
	    TAMTNode * pNode = static_cast<TAMTNode*>(pIndexBase);
	
	    // It has children
	    for(int i = 0; i < m_nodeCount; ++i)
	    {
		pNode[i].clear();
	    }
	
	    delete[] ((char*)pIndexBase);
	}
	m_nodeCount = 0;
	pIndexBase = nullptr;
    }

        
    // Get the data carried by the node
    bool getData(void *& result)
    {  
	if(isLeaf())
	{
	    if(m_nodeCount == 0)
	    {
		result = pIndexBase;
	    }
	    else if(!AMTNODE_HAS_TAIL(pIndexBase))
	    {
		memcpy(&result, ((char*)pIndexBase) + sizeof(TAMTNode)*m_nodeCount, sizeof(void*)); 
	    }
	    else
	    {
		// Support for tail compression. TODO
	    }
	    return true;
	}
    
	return false;
    }

    // Clear the data carried by this node. 
    void clearData()
    {
	if(isLeaf())
	{
	    AMTNODE_UNSET_LEAF(pIndexBase);
	    if(m_nodeCount != 0)
	    {
		void * pOldBase = pIndexBase;
		pIndexBase = new char[sizeof(TAMTNode)*m_nodeCount];
		memcpy(pIndexBase, pOldBase,sizeof(TAMTNode)*m_nodeCount);
		delete[] (char*)pOldBase;
	    }
	    else
	    {
		delete [] (char*)pIndexBase;
		pIndexBase = nullptr;
	    }
	}
    }
};




