/*
 * sort_ofp.cuh
 *
 *  Created on: Aug 23, 2019
 *      Author: i-bird
 */

#ifndef SORT_OFP_CUH_
#define SORT_OFP_CUH_


#ifdef __NVCC__

#if !defined(CUDA_ON_CPU) 
#include "cub/cub.cuh"
#else
#include "util/cuda_launch.hpp"
#endif

#if CUDART_VERSION < 11000
#include "util/cuda/moderngpu/kernel_mergesort.hxx"
#else
#ifndef SORT_WITH_CUB
#define SORT_WITH_CUB
#endif
#endif
#include "util/cuda/ofp_context.hxx"

template<typename key_t, typename val_t>
struct key_val_ref;

template<typename key_t, typename val_t>
struct key_val
{
	key_t key;
	val_t val;

	key_val(const key_t & k, const val_t & v)
	:key(k),val(v)
	{}

	key_val(const key_val_ref<key_t,val_t> & tmp)
	{
		this->operator=(tmp);
	}

	bool operator<(const key_val & tmp) const
	{
		return key < tmp.key;
	}

	bool operator>(const key_val & tmp) const
	{
		return key > tmp.key;
	}

	key_val & operator=(const key_val_ref<key_t,val_t> & tmp)
	{
		key = tmp.key;
		val = tmp.val;

		return *this;
	}
};


template<typename key_t, typename val_t>
struct key_val_ref
{
	key_t & key;
	val_t & val;

	key_val_ref(key_t & k, val_t & v)
	:key(k),val(v)
	{}

	key_val_ref(key_val_ref<key_t,val_t> && tmp)
	:key(tmp.key),val(tmp.val)
	{}

	key_val_ref & operator=(const key_val<key_t,val_t> & tmp)
	{
		key = tmp.key;
		val = tmp.val;

		return *this;
	}

	key_val_ref & operator=(const key_val_ref<key_t,val_t> & tmp)
	{
		key = tmp.key;
		val = tmp.val;

		return *this;
	}

	bool operator<(const key_val_ref<key_t,val_t> & tmp)
	{
		return key < tmp.key;
	}

	bool operator>(const key_val_ref<key_t,val_t> & tmp)
	{
		return key > tmp.key;
	}

	bool operator<(const key_val<key_t,val_t> & tmp)
	{
		return key < tmp.key;
	}

	bool operator>(const key_val<key_t,val_t> & tmp)
	{
		return key > tmp.key;
	}
};


template<typename key_t, typename val_t>
struct key_val_it
{
	key_t * key;
	val_t * val;

	bool operator==(const key_val_it & tmp)
	{
		return (key == tmp.key && val == tmp.val);
	}

	key_val_ref<key_t,val_t> operator*()
	{
		return key_val_ref<key_t,val_t>(*key,*val);
	}

	key_val_ref<key_t,val_t> operator[](int i)
	{
		return key_val_ref<key_t,val_t>(*key,*val);
	}

	key_val_it operator+(size_t count) const
	{
		key_val_it tmp(key+count,val+count);

		return tmp;
	}


	size_t operator-(key_val_it & tmp) const
	{
		return key - tmp.key;
	}

	key_val_it operator-(size_t count) const
	{
		key_val_it tmp(key-count,val-count);

		return tmp;
	}

	key_val_it & operator++()
	{
		++key;
		++val;

		return *this;
	}

	key_val_it & operator--()
	{
		--key;
		--val;

		return *this;
	}

	bool operator!=(const key_val_it & tmp) const
	{
		return key != tmp.key && val != tmp.val;
	}

	bool operator<(const key_val_it & tmp) const
	{
		return key < tmp.key;
	}

	key_val_it<key_t,val_t> & operator=(key_val_it<key_t,val_t> & tmp)
	{
		key = tmp.key;
		val = tmp.val;

		return *this;
	}

	key_val_it()	{}

	key_val_it(const key_val_it<key_t,val_t> & tmp)
	:key(tmp.key),val(tmp.val)
	{}

	key_val_it(key_t * key, val_t * val)
	:key(key),val(val)
	{}
};

template<typename key_t, typename val_t>
void swap(key_val_ref<key_t,val_t> a, key_val_ref<key_t,val_t> b)
{
	key_t kt = a.key;
	a.key = b.key;
	b.key = kt;

	val_t vt = a.val;
	a.val = b.val;
	b.val = vt;
}

template<typename key_t, typename val_t>
struct std::iterator_traits<key_val_it<key_t,val_t>>
{        
    typedef size_t difference_type; //almost always ptrdiff_t
    typedef key_val<key_t,val_t> value_type; //almost always T
    typedef key_val<key_t,val_t> & reference; //almost always T& or const T&
    typedef key_val<key_t,val_t> & pointer; //almost always T* or const T*
    typedef std::random_access_iterator_tag iterator_category;  //usually std::forward_iterator_tag or similar
};



namespace openfpm
{
	template<typename key_t, typename val_t,
	  typename comp_t>
	void sort(key_t* keys_input, val_t* vals_input, int count,
	  comp_t comp, mgpu::ofp_context_t& context)
	{
#ifdef CUDA_ON_CPU

	key_val_it<key_t,val_t> kv(keys_input,vals_input);

	std::sort(kv,kv+count,comp);

#else

	#ifdef SORT_WITH_CUB

			if (std::is_same<mgpu::template less_t<key_t>,comp_t>::value == true)
			{
				void *d_temp_storage = NULL;
				size_t temp_storage_bytes = 0;
				cub::DeviceRadixSort::SortPairs(d_temp_storage, temp_storage_bytes,keys_input,keys_input,vals_input,vals_input,count);

				auto & temporal = context.getTemporalCUB();
				temporal.resize(temp_storage_bytes);

				// Run
				cub::DeviceRadixSort::SortPairs(temporal.template getDeviceBuffer<0>(), temp_storage_bytes,keys_input,keys_input,vals_input,vals_input,count);
			}
			else if (std::is_same<mgpu::template greater_t<key_t>,comp_t>::value == true)
			{
				void *d_temp_storage = NULL;
				size_t temp_storage_bytes = 0;
				cub::DeviceRadixSort::SortPairsDescending(d_temp_storage, temp_storage_bytes,keys_input,keys_input,vals_input,vals_input,count);

				auto & temporal = context.getTemporalCUB();
				temporal.resize(temp_storage_bytes);

				// Run
				cub::DeviceRadixSort::SortPairsDescending(temporal.template getDeviceBuffer<0>(), temp_storage_bytes,keys_input,keys_input,vals_input,vals_input,count);
			}

	#else
			mgpu::mergesort(keys_input,vals_input,count,comp,context);
	#endif

#endif
	}
}

#endif


#endif /* SORT_OFP_CUH_ */
