
#include <boost/random.hpp>

#include "../src/details/TAMTNode.h"
#include "../src/TPrefixTree.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/chrono.hpp>
#include <set>
#include <unordered_set>

template<> struct Alphabet<char,std::string,16>
{
    typedef char Character;
    typedef std::string String;
    
    static unsigned int to_index(char c)
    {
	return c - 48;
    }

    static int to_index_array(std::string::const_iterator& it,
			      std::string::const_iterator& end,
			      uint8_t * array,
			      unsigned int arrayLen)
    {
	int i = 0;
	for(; it != end; ++it, ++i)
	{
	    array[i] = to_index(*it);
	}

	return i;
    }
};

typedef struct Alphabet<char,std::string,16> Digits;
typedef TAMTNode<Digits, Bitmap16> AMT16;

void performace_test()
{
    std::string digits = "0123456789";
    TPrefixTree<AMT16, void> digitTrie;
    std::vector<std::string> numbers;
    boost::random::random_device gen;
    std::vector<std::string> nums;
    std::set<std::string> setBased;
    std::unordered_set<std::string> hashBased;
    const unsigned int COUNT = 500000;
    
    for(int j = 0; j < COUNT; ++j)
    {
	std::string number;
	for(int i = 0; i < 10; ++i)
	{
	    boost::random::uniform_int_distribution<> index_dist(0, 9);
	    number.push_back(digits[index_dist(gen)]);
	}
	nums.push_back(number);

    }

    
    boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now(), end;
    
    for(int j = 0; j < COUNT; ++j)
    {
	digitTrie.Insert(nums[j]);
    }
    end = boost::chrono::steady_clock::now();
    auto test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;

    now = boost::chrono::steady_clock::now();
    for(int j = 0; j < COUNT; ++j)
    {
	setBased.insert(nums[j]);
    }
    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;

    
    now = boost::chrono::steady_clock::now();
    for(int j = 0; j < COUNT; ++j)
    {
	hashBased.insert(nums[j]);
    }
    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;

    	
	
	
    

    for(int j = 0; j < COUNT; ++j)
    {
	digitTrie.Search(nums[j]);
    }

    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;


    now = boost::chrono::steady_clock::now();
    for(int j = 0; j < COUNT; ++j)
    {
    	setBased.find(nums[j]);
    }

    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;


    now = boost::chrono::steady_clock::now();
    for(int j = 0; j < COUNT; ++j)
    {
    	hashBased.find(nums[j]);
    }

    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end -now);
    std::cout << "Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;
}

void correctness_test()
{
    TPrefixTree<AMT16, void> digitTrie; 
   
    std::vector<std::string> numbers = {"123456789", "123456", "1234568", "123457", "123458", "012345", "0123456789"};
    boost::random::random_device gen;
   
    std::cout << "Numbers Generated" << std::endl;
    
    
    for(int i = 0; i < numbers.size(); ++i)
    {
    	digitTrie.Insert(numbers[i]);
    }
    
    std::cout << "Numbers Inserted" << std::endl;
   
    for(int i = 0; i < numbers.size(); ++i)
    {
    	bool result = digitTrie.Search(numbers[i]);
    	if(!result)
    	{
    	    std::cout << "Failed to search the key: " << numbers[i] << ", at position: " << i << std::endl;
    	    break;
    	}
    }

    std::cout << "Starting to test deletion." << std::endl;

    for(int i = 0; i < numbers.size(); ++i)
    {
	if(!digitTrie.Remove(numbers[i]))
	{
	    std::cout << "Error. Failed to delete the key: " << numbers[i] << ", at position: " << i << std::endl;
	    break;
	}

	bool result = digitTrie.Search(numbers[i]);
	if(result)
	{
	    std::cout << "Failed to search the key: " << numbers[i] << ", at position: " << i << std::endl;
	}
    }
 
}

int main(int argc, char** argv)
{
    int num = 0;
    if(argc > 1)
    {
	num = atoi(argv[1]);
    }

    if(num == 0)
	num = 1000000;

    performace_test();
    correctness_test();
}
