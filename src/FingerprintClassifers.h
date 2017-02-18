#pragma once

#include "Tokenizer.h"

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
    
public:
    TFingerprintClassifierSimple()
    {
	m_groupIDCounter = 0;
    }
    
    ~TFingerprintClassifierSimple()
    {

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
	std::array<std::tuple<int, uint32_t>, Permutor::SIZE> m_groupArray;
	
	F f = m_generator.Generate(tokens);
	F reversed_f = m_generator.Generate_r(tokens);
	
	m_permutor.Permute(f, m_permuted);
	
	uint32_t newGroup = 0;

	for(int i = 0; i < Permutor::SIZE; ++i)
	{
	    std::vector<std::pair<std::pair<F, uint32_t>, uint32_t>> fingerprints;
	    // Get all fingerprints whose top 28 bits are the same as the ones in f
	    F lowerbound = m_permutor.GetLowerbound(m_permuted[i]);
	    F upperbound = m_permutor.GetUpperbound(m_permuted[i]);
	
	    auto lowerIter = m_tables[i].lower_bound(lowerbound);
	    auto upperIter = m_tables[i].upper_bound(upperbound);
	   	 
	    while(lowerIter != upperIter)
	    {
		// Compute the Hamming distance.
		uint32_t hammingDistance = hamming_distance<F>(m_permuted[i], lowerIter->first);
		if(hammingDistance <= 3)
		{	
		    fingerprints.push_back(std::pair<std::pair<F, uint32_t>,uint32_t>(*lowerIter, hammingDistance));
		}

		++lowerIter;
	    }

	    if(fingerprints.empty())
	    {
		m_groupArray[i] = std::make_tuple<int, uint32_t>(0,(uint32_t)~0);
	    }
	    else
	    {
		std::sort(fingerprints.begin(), fingerprints.end(),
			  [](const std::pair<std::pair<F, uint32_t>, uint32_t> &left,
			     const std::pair<std::pair<F, uint32_t>, uint32_t> &right) {
			      return left.second < right.second;
			  });

		m_groupArray[i] = std::make_tuple(fingerprints[0].first.second, 
						  fingerprints[0].second);
	    
	    }		
	}

	std::sort(m_groupArray.begin(), m_groupArray.end(),
		  [](const std::tuple<int, uint32_t> &left,
		     const std::tuple<int, uint32_t> &right) {
		      return std::get<1>(left) < std::get<1>(right);
		  });

	newGroup = std::get<0>(m_groupArray[0]);
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
		    if(hammingDistance <= 3)
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

