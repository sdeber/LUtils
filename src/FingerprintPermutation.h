#pragma once

template <typename F, unsigned int size> class FingerprintPermutation
{
public:
    const static unsigned int SIZE = size;
    void Permute(F f, F permuted[size])
    {
	
    }

    F GetLowerbound(F f)
    {
	
    }

    F GetUpperbound(F f)
    {
	
    }
};


template<> class FingerprintPermutation<uint64_t, 16>
{
private:
    void permutation1(uint64_t f, uint64_t permutated[4])
    {
	permutated[0] = f;
	permutated[1] = (f << 16) | (f >> 48);
	permutated[2] = (f << 32) | (f >> 32);
	permutated[3] = (f << 48) | (f >> 16);
    }

    void permutation2(uint64_t f, uint64_t permutated[4])
    {
	uint64_t f16 = f & 0xffff000000000000;
	uint64_t f48 = f & 0x0000ffffffffffff;
	permutated[0] = f;
	permutated[1] = (((f48 << 12) | (f48 >> 36)) & 0x0000ffffffffffff) | f16;
	permutated[2] = (((f48 << 24) | (f48 >> 24)) & 0x0000ffffffffffff) | f16;
	permutated[3] = (((f48 << 36) | (f48 >> 12)) & 0x0000ffffffffffff) | f16;
    }

public:
    const static unsigned int SIZE = 16;

    uint64_t GetLowerbound(uint64_t f)
    {
	return f & 0xfffffff000000000;
    }

    uint64_t GetUpperbound(uint64_t f)
    {
	return f | 0xfffffffff;
    }
    
    void Permute(uint64_t f, uint64_t permuted[16])
    {
	uint64_t permuted1[4];
	int index = 0;
       
	// First permutation
	permutation1(f, permuted1);

	for(int i = 0; i < 4; ++i)
	{
	    uint64_t permuted2[4];

	    //Second permutation
	    permutation2(permuted1[i], permuted2);

	    for(int j = 0; j < 4; ++j)
	    {
		permuted[index++] = permuted2[j];
	    }
	}
    }
};


template<> class FingerprintPermutation<uint32_t, 16>
{
private:
    void permutation1(uint32_t f, uint32_t permutated[4])
    {
	permutated[0] = f;
	permutated[1] = (f << 8) | (f >> 24);
	permutated[2] = (f << 16) | (f >> 16);
	permutated[3] = (f << 24) | (f >> 8);
    }

    void permutation2(uint32_t f, uint32_t permutated[4])
    {
	uint32_t upper_part = f & 0xff000000;
	uint32_t lower_part = f & 0x00ffffff;
	permutated[0] = f;
	permutated[1] = (((lower_part << 6) | (lower_part >> 18)) & 0x00ffffff) | upper_part;
	permutated[2] = (((lower_part << 12) | (lower_part >> 12)) & 0x00ffffff) | upper_part;
	permutated[3] = (((lower_part << 18) | (lower_part >> 6)) & 0x00ffffff) | upper_part;
    }

public:
    const static unsigned int SIZE = 16;

    uint32_t GetLowerbound(uint32_t f)
    {
	return f & 0xfffc0000;
    }

    uint32_t GetUpperbound(uint32_t f)
    {
	return f | 0x3ffff;
    }
    
    void Permute(uint32_t f, uint32_t permuted[16])
    {
	uint32_t permuted1[4];
	int index = 0;
	
	// First permutation
	permutation1(f, permuted1);

	for(int i = 0; i < 4; ++i)
	{
	    std::bitset<32> bf(permuted1[i]);	
	    std::cout << bf.to_string() << std::endl;
	}
	std::cout << std::endl;

	for(int i = 0; i < 4; ++i)
	{
	    uint32_t permuted2[4];

	    for(int k = 0; k < 4; ++k)
	    {
	        std::bitset<32> bf(permuted2[k]);
	        std::cout << bf.to_string() << std::endl;
	    }
	    std::cout << std::endl;

	    //Second permutation
	    permutation2(permuted1[i], permuted2);
	
	    for(int j = 0; j < 4; ++j)
	    {
		permuted[index++] = permuted2[j];
	    }
	}
    }
};

