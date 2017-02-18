#pragma once
#include "details/farmhash.h"
#include <string>
#include <vector>

template <typename F, F (*hashfunc)(const char*, unsigned long)> class TFingerprintGenerator
{
    
public:
    TFingerprintGenerator()
    {
     
    }
    
    F Generate(const std::vector<std::u16string>& tokens)
    {
	F result = 0;
	int weightVector[sizeof(F)*8];
	memset(weightVector, 0, sizeof(int)*sizeof(F)*8);
	
	for(int i = 0; i < tokens.size(); ++i)
	{
	   
	    F hashValue = hashfunc((const char*)tokens[i].data(), tokens[i].size() * sizeof(char16_t));
	    for(int j = 0; j < sizeof(F)*8; ++j)
	    {
		if((hashValue & (1 << j)) > 0)
		{
		    ++weightVector[j];
		}
		else
		{
		    --weightVector[j];
		}
	    }
	}

	for(int i = 0; i < sizeof(F)*8; ++i)
	{
	    if(weightVector[i] > 0)
		result = result | (1 << i);
	}
	
	return result;
    }

    F Generate_r(const std::vector<std::u16string>& tokens)
    {
	F result = 0;
	int weightVector[sizeof(F)*8];
	memset(weightVector, 0, sizeof(int)*sizeof(F)*8);

	std::u16string token;
	
	for(int i = 0; i < tokens.size(); ++i)
	{
	    token.assign(tokens[i].rbegin(), tokens[i].rend());
	    F hashValue = hashfunc((const char*)token.data(), token.size() * sizeof(char16_t));
	    for(int j = 0; j < sizeof(F)*8; ++j)
	    {
		if((hashValue & (1 << j)) > 0)
		{
		    ++weightVector[j];
		}
		else
		{
		    --weightVector[j];
		}
	    }
	}

	for(int i = 0; i < sizeof(F)*8; ++i)
	{
	    if(weightVector[i] > 0)
		result = result | (1 << i);
	}
	
	return result;
    }

};


