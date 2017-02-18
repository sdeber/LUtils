#pragma once
#include <stdint.h>
#include <string.h>

#define AMTNODE_SET_LEAF(index_ptr) (index_ptr = (void*)(((uint64_t)1) | (uint64_t)index_ptr))
#define AMTNODE_IS_LEAF(index_ptr) (((uint64_t)1) & (uint64_t)index_ptr)
#define AMTNODE_UNSET_LEAF(index_ptr) (index_ptr = (void*)(((uint64_t)~1) & (uint64_t)index_ptr))

#define AMTNODE_SET_DATA(index_ptr) (index_ptr = (void*)(((uint64_t)3) | (uint64_t)index_ptr))
#define AMTNODE_HAS_DATA(index_ptr) (((uint64_t)3) & (uint64_t)index_ptr)
#define AMTNODE_UNSET_DATA(index_ptr) (index_ptr = (void*)(((uint64_t)~3) & (uint64_t)index_ptr))

#define AMTNODE_SET_TAIL(index_ptr) (index_ptr = (void*)(((uint64_t)4) | (uint64_t)index_ptr))
#define AMTNODE_HAS_TAIL(index_ptr) (((uint64_t)4) & (uint64_t)index_ptr)
#define AMTNODE_UNSET_TAIL(index_ptr) (index_ptr = (void*)(((uint64_t)~4) & (uint64_t)index_ptr))
#define AMTNODE_GET_INDEX(index_ptr) ((void*)(0xfffffffffffffff0 & (uint64_t)index_ptr))
#define AMTNODE_COPY_FLAGS(src_ptr, dest_ptr) ( dest_ptr = (void*)((0x000000000000000f & ((uint64_t)src_ptr)) | (uint64_t) dest_ptr) )

