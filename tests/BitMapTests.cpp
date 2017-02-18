#include "src/details/BitMaps.h"
#include <boost/chrono.hpp>

void InsertionTest128_()
{
    BitMap128_ bitmap128_;
   
    for(int i = 0; i < 128; ++i)
    {
	bitmap128_.set(i);
    }

    for(int i = 0; i < 128; ++i)
    {
	int index = bitmap128_.find(i);
	if(index != i)
	{
	    std::cout << "Error: The index for the number " << i << " is wrong. It is currently " << index << std::endl;
	}
    }
}

void DeletionTest128_()
{
    BitMap128_ bitmap128_;
   
    for(int i = 0; i < 128; ++i)
    {
	bitmap128_.set(i);
    }

    for(int i = 0; i < 128; ++i)
    {
	int index = bitmap128_.find(i);
	if(index != i)
	{
	    std::cout << "Error: The index for the number " << i << " is wrong. It is currently " << index << std::endl;
	}
    }

    for(int i = 0; i < 128; ++i)
    {
	bitmap128_.unset(i);
	int index = bitmap128_.find(i);
	if(index != -1)
	{
	    std::cout << "Error: The index for the number " << i << " is wrong. It is currently " << index << std::endl;
	}
    }
}

void InsertionTest128()
{
    BitMap128 bitmap128;
   
    for(int i = 0; i < 128; ++i)
    {
	bitmap128.set(i);
    }

    for(int i = 0; i < 128; ++i)
    {
	int index = bitmap128.find(i);
	if(index != i)
	{
	    std::cout << "Error: The index for the number " << i << " is wrong. It is currently " << index << std::endl;
	}
    }
}

void InsertionTest64()
{
    Bitmap64 bitmap;
   
    for(int i = 0; i < 64; ++i)
    {
	bitmap.set(i);
    }

    for(int i = 0; i < 64; ++i)
    {
	int index = bitmap.find(i);
	if(index != i)
	{
	    std::cout << "Error: The index for the number " << i << " is wrong. It is currently " << index << std::endl;
	}
    }
}

int main(int argc, char ** argv)
{
  
    InsertionTest128_();
    DeletionTest128_();
    InsertionTest64();
    BitMap128 bitmap128;
    BitMap128_ bitmap128_;
    const int COUNT = 100000;
    
    for(int i = 0; i < 128; ++i)
    {
	bitmap128.set(i);
	bitmap128_.set(i);
    }
    
    boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now(), end;
    for(int i = 0; i < COUNT; ++i)
    {
	bitmap128.find(127);
    }
    end = boost::chrono::steady_clock::now();
    
    auto test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end - now);
    std::cout << "1. Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;

    now = boost::chrono::steady_clock::now();
    for(int i = 0; i < COUNT; ++i)
    {
	bitmap128_.find(127);
    }
    end = boost::chrono::steady_clock::now();
    test_duration = boost::chrono::duration_cast<boost::chrono::nanoseconds>(end - now);
    std::cout << "2. Time cost (total): " << test_duration << ", Single: " << test_duration.count() / (double)COUNT << std::endl;

}
