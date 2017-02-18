#pragma once
#include <string>
#include <vector>

class Tokenizer
{
public:
    virtual void Tokenize(const std::u16string& sms, std::vector<std::u16string>& tokens) = 0;
    
};

class ChunkTokenizer : public Tokenizer
{
public:
    virtual void Tokenize(const std::u16string& sms, std::vector<std::u16string>& tokens)
    {
	std::u16string token;
	unsigned int len = sms.size();
	for (unsigned int i = 0; i < len; i++)
	{
	    if(i % 10 == 0)
	    {
		tokens.push_back(token);
		token.clear();
	    }
	    
	    char16_t ch = sms[i];
	    if (isdigit(ch))
		ch = u'0';
	    token.push_back(toupper(ch));
	}

	if(!token.empty())
	{
	    tokens.push_back(token);
	}
    }
    
};

class WordTokenizer : public Tokenizer
{
public:
    virtual void Tokenize(const std::u16string& sms, std::vector<std::u16string>& tokens)
    {
	std::u16string token;
	unsigned int len = sms.size();
	for (unsigned int i = 0; i < len; i++)
	{
	    char16_t ch = sms[i];
	    
	    if(ispunct(ch) || isspace(ch))
	    {
		if(!token.empty())
		    tokens.push_back(token);
		token.clear();
		continue;
	    }
	    
	    if (isdigit(ch))
		ch = u'0';
	    token.push_back(toupper(ch));
	}

	if(!token.empty())
	{
	    tokens.push_back(token);
	}
    }
    
};
