#pragma once

#include "Tokenizer.h"
#include <mutex>

template <typename F> uint32_t hamming_distance(F x, F y, int displacement = 28)
{
    uint32_t dist = 0;
    F  val = (x ^ y);// << displacement;
    int total_loop = sizeof(F)*8 - displacement;
    // Count the number of bits set
    int count = 0;
    while (val != 0 /*&& (count++ < total_loop)*/)
    {
        // A bit is set, so increment the count and clear the bit
        dist++;
        val &= val - 1;
    }

    // Return the number of differing bits
    return dist;
}

class FingerprintsClassifier
{
public:

    virtual uint32_t GetGroupID(const std::vector<std::u16string>& tokens) = 0;
    virtual void Clear() = 0;
    virtual ~FingerprintsClassifier()
    {
	
    }
};


template <typename F, typename Generator, typename Permutor> class TFingerprintClassifierSimple : public FingerprintsClassifier
{
private:
    uint32_t m_groupIDCounter;
    
    std::array<std::map<F, uint32_t>, Permutor::SIZE> m_tables;
    std::map<uint32_t, std::vector<std::pair<F, uint32_t>>> m_secondary;
    
    Generator m_generator;
    Permutor m_permutor;
    const static uint32_t MAX_ALLOWED_DISTANCE = 4;
public:
    TFingerprintClassifierSimple()
    {
	m_groupIDCounter = 0;
    }
    
    virtual void Clear()
    {
	for(int i = 0; i < Permutor::SIZE; ++i)
	{
	    m_tables[i].clear();	       
	}
	
	m_secondary.clear();
	m_groupIDCounter = 0;
    }

    virtual uint32_t GetGroupID(const std::vector<std::u16string>& tokens)
    {

	F m_permuted[Permutor::SIZE];
       
	F f = m_generator.Generate(tokens);
	F reversed_f = m_generator.Generate_r(tokens);
	
	m_permutor.Permute(f, m_permuted);
	
	uint32_t newGroup = 0;
	uint32_t globalMinHamming = MAX_ALLOWED_DISTANCE;

	for(int i = 0; i < Permutor::SIZE; ++i)
	{
	    uint32_t partitionLocalMinHamming = MAX_ALLOWED_DISTANCE;
	    uint32_t localGroupId = 0;
	    
	    // Get all fingerprints whose top 28 bits are the same as the ones in f
	    F lowerbound = m_permutor.GetLowerbound(m_permuted[i]);
	    F upperbound = m_permutor.GetUpperbound(m_permuted[i]);
	
	    auto lowerIter = m_tables[i].lower_bound(lowerbound);
	    auto upperIter = m_tables[i].upper_bound(upperbound);
	   	 
	    while(lowerIter != upperIter)
	    {
		// Compute the Hamming distance.
		uint32_t hammingDistance = hamming_distance<F>(m_permuted[i], lowerIter->first);
		if(hammingDistance < partitionLocalMinHamming)
		{
		    partitionLocalMinHamming = hammingDistance;
		    localGroupId = lowerIter->second;	
		}

		++lowerIter;
	    }

	    if(partitionLocalMinHamming < globalMinHamming)
	    {
		newGroup = localGroupId;
	    }		
	}

	if(newGroup == 0)
	{
	    newGroup = ++m_groupIDCounter;
	    for(int i = 0; i < Permutor::SIZE; ++i)
	    {
		m_tables[i].insert(std::pair<F,uint32_t>(m_permuted[i], newGroup));
	    }

	    std::vector<std::pair<F, uint32_t>> newMap;
	    newMap.push_back(std::pair<F,uint32_t>(reversed_f, newGroup));
	   
	    m_secondary[newGroup] = newMap;
	
	}
	else
	{
	    auto it = m_secondary.find(newGroup);
	    if(it != m_secondary.end())
	    {
		bool matched = false;
		int i = 0;
		for(; i < it->second.size(); ++i)
		{
		    uint32_t hammingDistance = hamming_distance<F>(reversed_f, (it->second)[i].first);
		    if(hammingDistance <= MAX_ALLOWED_DISTANCE)
		    {	
			matched = true;
			break;
		    }

		}

		if(!matched)
		{
		    newGroup = ++m_groupIDCounter;
		    it->second.push_back(std::pair<F,uint32_t>(reversed_f, newGroup));
		}
		else
		{
		    newGroup = (it->second)[i].second;
		}
	    }
	}
	
	return newGroup;
    }
    
};


template <typename F, typename Generator, typename Permutor, typename Mutex> class TFingerprintClassifierMT : public FingerprintsClassifier
{
private:
    std::atomic<uint32_t> m_groupIDCounter;
    std::atomic<bool> m_workingFlags[Permutor::SIZE];
    std::array<std::map<F /* The fingerprint */, uint32_t /* reference into the secondary table */>, Permutor::SIZE> m_tables;
    std::map<uint32_t, std::vector<std::pair<F, uint32_t>>> m_secondary;
    Mutex m_mutexForSecondary;
    Generator m_generator;
    Permutor m_permutor;
    const static uint32_t MAX_ALLOWED_DISTANCE = 4;
public:
    TFingerprintClassifierMT()
    {
	m_groupIDCounter = 0;
	for(int i = 0; i < Permutor::SIZE; ++i)
	{
	    m_workingFlags[i] = false;	       
	}
	
    }
    
    virtual void Clear()
    {
	bool finished[Permutor::SIZE];
	for(int i = 0; i < Permutor::SIZE; ++i)
	{
	    finished[i] = false;
	}

	for(int count = 0; count < Permutor::SIZE; )
	{
	    for(int i = 0; i < Permutor::SIZE; ++i)
	    {
		if(!finished[i])
		{
		    bool flag = m_workingFlags[i].exchange(true);
		    if(flag)
			continue;
		    m_tables[i].clear();
		    ++count;
		    finished[i] = true;
		    m_workingFlags[i].store(false);
		}
		
	    }
	}
	std::lock_guard<Mutex> l(m_mutexForSecondary);
	m_secondary.clear();
	m_groupIDCounter = 0;
    }

    virtual uint32_t GetGroupID(const std::vector<std::u16string>& tokens)
    {

	F permuted[Permutor::SIZE];
	
	F f = m_generator.Generate(tokens);
	F reversed_f = m_generator.Generate_r(tokens);
	
	m_permutor.Permute(f, permuted);
	
	uint32_t newGroup = 0;
	uint32_t globalMinHamming = MAX_ALLOWED_DISTANCE;
	
	bool finished[Permutor::SIZE];
	for(int i = 0; i < Permutor::SIZE; ++i)
	{
	    finished[i] = false;
	}

	for(int count = 0; count < Permutor::SIZE; )
	{
	    for(int i = 0; i < Permutor::SIZE; ++i)
	    {
		if(!finished[i])
		{
		    bool flag = m_workingFlags[i].exchange(true);
		    if(flag)
			continue;

		    uint32_t partitionLocalMinHamming = MAX_ALLOWED_DISTANCE;
		    uint32_t localGroupId = 0;
		    // Get all fingerprints whose top 28 bits are the same as the ones in f
		    F lowerbound = m_permutor.GetLowerbound(permuted[i]);
		    F upperbound = m_permutor.GetUpperbound(permuted[i]);
	
		    auto lowerIter = m_tables[i].lower_bound(lowerbound);
		    auto upperIter = m_tables[i].upper_bound(upperbound);
	   	 
		    while(lowerIter != upperIter)
		    {
			// Compute the Hamming distance.
			uint32_t hammingDistance = hamming_distance<F>(permuted[i], lowerIter->first);
			if(hammingDistance < partitionLocalMinHamming)
			{
			    partitionLocalMinHamming = hammingDistance;
			    localGroupId = lowerIter->second;	
			}

			++lowerIter;
		    }

		    m_workingFlags[i].store(false);
		
		    if(partitionLocalMinHamming < globalMinHamming)
		    {
			newGroup = localGroupId;
		    }
		
		    ++count;
		    finished[i] = true;
		}
		
	    }

	}

	if(newGroup == 0)
	{
	    newGroup = ++m_groupIDCounter;

	    for(int i = 0; i < Permutor::SIZE; ++i)
	    {
		finished[i] = false;
	    }
	    
	    for(int count = 0; count < Permutor::SIZE; )
	    {
		for(int i = 0; i < Permutor::SIZE; ++i)
		{
		    if(!finished[i])
		    {
			bool flag = m_workingFlags[i].exchange(true);
			if(flag)
			    continue;
			
			m_tables[i].insert(std::pair<F,uint32_t>(permuted[i], newGroup));
			++count;
			finished[i] = true;
			m_workingFlags[i].store(false);
		    }
		}
	    }
	    std::vector<std::pair<F, uint32_t>> newMap;
	    newMap.push_back(std::pair<F,uint32_t>(reversed_f, newGroup));

	    std::lock_guard<Mutex> l(m_mutexForSecondary);
	    m_secondary[newGroup] = newMap;
	
	}
	else
	{
	    std::lock_guard<Mutex> l(m_mutexForSecondary);
	    auto it = m_secondary.find(newGroup);
	    if(it != m_secondary.end())
	    {
	    	bool matched = false;
	    	int i = 0;
	    	for(; i < it->second.size(); ++i)
	    	{
	    	    uint32_t hammingDistance = hamming_distance<F>(reversed_f, (it->second)[i].first);
	    	    if(hammingDistance < MAX_ALLOWED_DISTANCE)
	    	    {	
	    		matched = true;
	    		break;
	    	    }

	    	}

	    	if(!matched)
	    	{
	    	    newGroup = ++m_groupIDCounter;
	    	    it->second.push_back(std::pair<F,uint32_t>(reversed_f, newGroup));
	    	}
	    	else
	    	{
	    	    newGroup = (it->second)[i].second;
	    	}
	    }
	}
	
	return newGroup;
    }
    
};
