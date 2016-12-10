/*
 * MemMemoryWise.hpp
 *
 *  Created on: Mar 22, 2015
 *  Last modified: June 25, 2015
 *      Authors: Pietro Incardona, Yaroslav Zaluzhnyi
 */

#ifndef CELLISTMEM_HPP_
#define CELLISTMEM_HPP_

#include "CellList.hpp"

/*! \brief Class for MEMORY-WISE cell list implementation
 *
 * This class implement the MEMORY-WISE cell list
 * The memory allocation is small.
 * The memory allocation is (in byte) Size = O(N*size_of(ele))
 *
 * Where
 *
 * N = total number of elements
 * M = number of cells
 * sizeof(ele) = the size of the element the cell list is storing, example if
 *               the cell list store the particle id (64bit) is 8 byte
 *
 * \note It is useful when M >> N
 *
 * \tparam dim Dimensionality of the space
 * \tparam T type of the space float, double, complex
 *
 */
template<unsigned int dim, typename T, typename transform = no_transform<dim,T>, typename base=openfpm::vector<size_t>>
class Mem_mw
{
	// each cell has a dynamic structure
	// that store the elements in the cell
	std::unordered_map<size_t,base> cl_base;

	//Origin point
	Point<dim,T> orig;

public:

	// Object type that the structure store
	typedef T value_type;


	void init_to_zero(size_t slot, size_t tot_n_cell)
	{
		//resize the map to needed number of cells

		cl_base.rehash(tot_n_cell);

		//filling a map with "base" structures
		for (size_t i = 0; i < tot_n_cell; i++)
		{   base b;
			//cl_base.insert(tot_n_cell, b);
			cl_base[i] = b;
		}
	}

	void operator=(const Mem_mw & cell)
	{
		cl_base = cell.cl_base;
	}

	void addCell(size_t cell_id, typename base::value_type ele)
	{
		//add another neighbor element

		cl_base[cell_id].add(ele);
	}

	void add(size_t cell_id, typename base::value_type ele)
	{
		this->addCell(cell_id,ele);
	}

	void remove(size_t cell, size_t ele)
	{
		cl_base[cell].remove(ele);
	}

	size_t getNelements(const size_t cell_id) const
	{
		return cl_base.find(cell_id)->second.size();
	}

	auto get(size_t cell, size_t ele) -> decltype(cl_base[cell].get(ele)) &
	{
		return cl_base[cell].get(ele);
	}

	void swap(Mem_mw & cl)
	{
		//cl_base.swap(cl.cl_base);
		swap(cl_base, cl.cl_base);
	}

	void swap(Mem_mw && cell)
	{
		//cl_base.swap(cell.cl_base);
		swap(cl_base, cell.cl_base);
	}

	void clear()
	{
		cl_base.clear();
	}

	inline size_t * getStartId(size_t cell_id)
	{
		return &cl_base[cell_id].get(0);
	}

	inline size_t * getStopId(size_t cell_id)
	{
		return (&cl_base[cell_id].last()) + 1;
	}

	inline size_t & get_lin(size_t * part_id)
	{
		return *part_id;
	}

public:

	Mem_mw(size_t slot)
	{}

};


#endif /* CELLISTMEM_HPP_ */
