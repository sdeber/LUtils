#pragma once
#include <stdint.h>
#include <string.h>
#include <iostream>
class Bitmap16
{
private:
    uint16_t m_map;
public:
    Bitmap16(): m_map(0)
    {}
    
    int set(unsigned int ch)
    {
	if(ch > 15)
	    return -1;
	
	uint16_t tmp = (uint16_t)1 << ch; 
	m_map |= tmp;
	return __builtin_popcount(m_map & (tmp - 1));
    }

    int find(unsigned int ch) const
    {
	if(ch > 15)
	    return -1;
	
	uint16_t tmp = (uint16_t)1 << ch; 
	if(m_map & tmp)
	{
	    return __builtin_popcount(m_map & (tmp - 1));
	}
	else
	    return -1;
    }
    
    int unset(unsigned int ch)
    {
	if(ch > 15)
	    return -1;
	
	int index = find(ch);
	if(index != -1)
	{
	    m_map &= ~(1 << ch);
	}
	return index;
    }

    void clear()
    {
	m_map = 0;
    }
    
};

class Bitmap32
{
private:
    uint32_t m_map;
public:
    Bitmap32(): m_map(0)
    {}
    
    int set(unsigned int ch)
    {
	if(ch > 31)
	    return -1;
	
	uint32_t tmp = (uint32_t)1 << ch;
	m_map |= tmp;
	return __builtin_popcount(m_map & (tmp - 1));
    }

    int find(unsigned int ch) const
    {
	if(ch > 31)
	    return -1;
	
	uint32_t tmp = (uint32_t)1 << ch;
	if(m_map & tmp)
	{
	    return __builtin_popcount(m_map & (tmp - 1));
	}
	else
	    return -1;
    }
    
    int unset(unsigned int ch)
    {
	if(ch > 31)
	    return -1;
	
	int index = find(ch);
	if(index != -1)
	{
	    m_map &= ~(1 << ch);
	}
	return index;
    }

    void clear()
    {
	m_map = 0;
    }
    
};


class Bitmap64
{
private:
    uint64_t m_map;
public:
    Bitmap64(): m_map(0)
    {}
    
    int set(unsigned int ch)
    {
	if(ch > 63)
	    return -1;
	
	uint64_t tmp = (uint64_t)1 << ch;
	m_map |= tmp;
	return __builtin_popcountll(m_map & (tmp - 1));
    }

    int find(unsigned int ch) const
    {
	if(ch > 63)
	    return -1;
	
	uint64_t tmp = (uint64_t)1 << ch;
	if(m_map & tmp)
	{
	    return __builtin_popcountll(m_map & (tmp -1));
	}
	else
	    return -1;
    }
    
    int unset(unsigned int ch)
    {
	if(ch > 63)
	    return -1;
	
	int index = find(ch);
	if(index != -1)
	{
	    m_map &= ~(1 << ch);
	}
	return index;
    }

    void clear()
    {
	m_map = 0;
    }
    
};


struct BitMap128
{

    struct entry_t
    {
        uint32_t BitMap;
        uint32_t Offset;
    };

    entry_t bitmap[4];

    BitMap128()
    {
        bzero(&bitmap, sizeof (entry_t)*4);
    }

    void clear()
    {
        bzero(&bitmap, sizeof (entry_t)*4);
    }

    int set(unsigned int ch)
    {
	if(ch > 127)
	    return -1;
	
        int idx = (ch >> 5); // word index
        ch &= 0x1f;
        uint32_t map = bitmap[idx].BitMap;
        map |= (1 << ch);
        bitmap[idx].BitMap = map;
        for (int i = idx + 1; i < 4; i++)
        {
            bitmap[i].Offset++;
        }

        /**
         * Find the proper index for the new child.
         */
        map &= (~((~0) << ch));
        return __builtin_popcount(map) + bitmap[idx].Offset;

    }

    int unset(unsigned int ch)
    {
	if(ch > 127)
	    return -1;
	
        int idx = (ch >> 5); // word index
        ch &= 0x1f;
        uint32_t map = bitmap[idx].BitMap;

        uint32_t old_index = __builtin_popcount(map & (~((~0) << ch))) + bitmap[idx].Offset;

        map &= ~(1 << ch);
        bitmap[idx].BitMap = map;
        for (int i = idx + 1; i < 4; i++)
        {
            bitmap[i].Offset--;
        }

        return old_index;
    }

    int find(unsigned int ch) const
    {
	if(ch > 127)
	    return -1;
	
        int idx = (ch >> 5); // word index
        ch &= 0x1f;
        uint32_t map = bitmap[idx].BitMap;
        if (!(map & (1 << ch)))
            return -1;
        /**
         * Find the proper index for the new child.
         */
        map &= (~((~0) << ch));
        return __builtin_popcount(map) + bitmap[idx].Offset;

    }
};


struct BitMap128_
{

    uint64_t lower;
    uint64_t upper;
    
    BitMap128_()
    {
	lower = upper = 0;
    }

    void clear()
    {
	lower = upper = 0;
    }

    int set(unsigned int ch)
    {
	if(ch > 127)
	    return -1;
	
        int idx = (ch >> 6); // word index
        ch &= 0x3f;
	uint64_t tmp = (uint64_t)1 << ch;
	if(idx == 0)
	{
	    lower |= tmp;
	    return __builtin_popcountll(lower & (tmp - 1));

	}
	else
	{
	    upper |= tmp;
	    return __builtin_popcountll(lower) + __builtin_popcountll(upper & (tmp - 1)); 

	}
        
    }

    int unset(unsigned int ch)
    {
	if(ch > 127)
	    return -1;
	
	int oldIndex = find(ch);
        if(oldIndex != -1)
	{
	    int idx = (ch >> 6); // word index
	    ch &= 0x3f;
	
	    if(idx == 0)
	    {
		lower &= ~(1 << ch);
	    }
	    else
	    {
		upper &= ~(1 << ch);
	    }
	}

	return oldIndex;
    }

    int find(unsigned int ch) const
    {
	if(ch > 127)
	    return -1;
	
	int idx = (ch >> 6); // word index
        ch &= 0x3f;
	uint64_t tmp = (uint64_t)1 << ch; 
	if(idx == 0)
	{
	    if(lower & tmp)
	    {
		return __builtin_popcountll(lower & (tmp - 1));
	    }
	}
	else
	{
	    if(upper & tmp)
	    {
		return __builtin_popcountll(lower) + __builtin_popcountll(upper & (tmp - 1)); 
	    }
	}

	return -1;
    }
};

struct BitMap256
{

    struct entry_t
    {
        uint32_t BitMap;
        uint32_t Offset;
    };

    entry_t bitmap[8];

    BitMap256()
    {
        bzero(&bitmap, sizeof (entry_t)*8);
    }

    void clear()
    {
        bzero(&bitmap, sizeof (entry_t)*8);
    }

    int set(unsigned int ch)
    {
	if(ch > 255)
	    return -1;
	
        int idx = (ch >> 5); // word index
        ch &= 0x1f;
        uint32_t map = bitmap[idx].BitMap;
        map |= (1 << ch);
        bitmap[idx].BitMap = map;
        for (int i = idx + 1; i < 8; i++)
        {
            bitmap[i].Offset++;
        }

        /**
         * Find the proper index for the new child.
         */
        map &= (~((~0) << ch));
        return __builtin_popcount(map) + bitmap[idx].Offset;

    }

    int unset(unsigned int ch)
    {
	if(ch > 255)
	    return -1;
	
        int idx = (ch >> 5); // word index
        ch &= 0x1f;
        uint32_t map = bitmap[idx].BitMap;


        uint32_t old_index = __builtin_popcount(map & (~((~0) << ch))) + bitmap[idx].Offset;

        map &= ~(1 << ch);
        bitmap[idx].BitMap = map;
        for (int i = idx + 1; i < 8; i++)
        {
            bitmap[i].Offset--;
        }

        return old_index;
    }

    int find(unsigned int ch) const
    {
	if(ch > 255)
	    return -1;
	
        int idx = (ch >> 5); // word index
        ch &= 0x1f;
        uint32_t map = bitmap[idx].BitMap;
        if (!(map & (1 << ch)))
            return -1;
        /**
         * Find the proper index for the new child.
         */
        map &= (~((~0) << ch));
        return __builtin_popcount(map) + bitmap[idx].Offset;

    }
};
