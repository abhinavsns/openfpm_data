/*
 * HDF5_reader_vd.hpp
 *
 *  Created on: May 1, 2017
 *      Author: i-bird
 */

#ifndef OPENFPM_IO_SRC_HDF5_WR_HDF5_READER_VD_HPP_
#define OPENFPM_IO_SRC_HDF5_WR_HDF5_READER_VD_HPP_


template <>
class HDF5_reader<VECTOR_DIST>
{
private:

	template<typename vector_pos_type, typename vector_prp_type>
	bool load_block(long int bid,
			        hssize_t mpi_size_old,
					int * metadata_out,
					openfpm::vector<size_t> metadata_accum,
					hid_t plist_id,
					hid_t dataset_2,
					size_t & g_m,
					vector_pos_type & v_pos,
					vector_prp_type & v_prp)
	{
		hsize_t offset[1];
		hsize_t block[1];

		if (bid < mpi_size_old && bid != -1)
		{
			offset[0] = metadata_accum.get(bid);
			block[0] = metadata_out[bid];
		}
		else
		{
			offset[0] = 0;
			block[0] = 0;
		}

	    hsize_t count[1] = {1};


		//Select file dataspace
		hid_t file_dataspace_id_2 = H5Dget_space(dataset_2);
		if (file_dataspace_id_2 < 0)	{return false;}

        herr_t err = H5Sselect_hyperslab(file_dataspace_id_2, H5S_SELECT_SET, offset, NULL, count, block);
        if (err < 0)	{return false;}

        hsize_t mdim_2[1] = {block[0]};


		//Create data space in memory
		hid_t mem_dataspace_id_2 = H5Screate_simple(1, mdim_2, NULL);
		if (mem_dataspace_id_2 < 0)	{return false;}

		size_t sum = 0;

		for (int i = 0; i < mpi_size_old; i++)
			sum += metadata_out[i];

		// allocate the memory
		HeapMemory pmem;

		ExtPreAlloc<HeapMemory> & mem = *(new ExtPreAlloc<HeapMemory>(block[0],pmem));
		mem.incRef();

	  	// Read the dataset.
	    err = H5Dread(dataset_2, H5T_NATIVE_CHAR, mem_dataspace_id_2, file_dataspace_id_2, plist_id, (char *)mem.getPointer());
	    if (err < 0)	{return false;}

		mem.allocate(pmem.size());

		Unpack_stat ps;

		vector_pos_type v_pos_unp;

		vector_prp_type v_prp_unp;

		Unpacker<decltype(v_pos_unp),HeapMemory>::unpack(mem,v_pos_unp,ps,1);
		Unpacker<decltype(v_prp_unp),HeapMemory>::unpack(mem,v_prp_unp,ps,1);

		mem.decRef();
		delete &mem;

		for (size_t i = 0; i < v_pos_unp.size(); i++)
			v_pos.add(v_pos_unp.get(i));

		for (size_t i = 0; i < v_prp_unp.size(); i++)
			v_prp.add(v_prp_unp.get(i));

		g_m = v_pos.size();

		H5Sclose(file_dataspace_id_2);
		H5Sclose(mem_dataspace_id_2);

		return true;
	}

public:

	template<typename vector_pos_type, typename vector_prp_type> inline bool load(const std::string & filename,
			                                                               vector_pos_type & v_pos,
																		   vector_prp_type & v_prp,
																		   size_t & g_m)
	{
		Vcluster<> & v_cl = create_vcluster();

		v_pos.clear();
		v_prp.clear();

		g_m = 0;

		MPI_Comm comm = v_cl.getMPIComm();
		MPI_Info info  = MPI_INFO_NULL;

		int mpi_rank = v_cl.getProcessUnitID();

		// Set up file access property list with parallel I/O access
		hid_t plist_id = H5Pcreate(H5P_FILE_ACCESS);
		if (plist_id == -1)	{return false;}
		herr_t err = H5Pset_fapl_mpio(plist_id, comm, info);
		if (err < 0)
			return false;

		//Open a file
	    hid_t file = H5Fopen (filename.c_str(), H5F_ACC_RDONLY, plist_id);
	    if (file < 0)	{return false;}
	    H5Pclose(plist_id);

	    //Open dataset
	    hid_t dataset = H5Dopen (file, "metadata", H5P_DEFAULT);
	    if (dataset < 0)	{return false;}

	    //Create property list for collective dataset read
	  	plist_id = H5Pcreate(H5P_DATASET_XFER);
	  	if (plist_id == -1)	{return false;}
	  	H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);

		//Select file dataspace
		hid_t file_dataspace_id = H5Dget_space(dataset);
		if (file_dataspace_id < 0)	{return false;}

		hssize_t mpi_size_old = H5Sget_select_npoints (file_dataspace_id);
		if (mpi_size_old < 0)	{return false;}

	  	//Where to read metadata
	  	int metadata_out[mpi_size_old];

	  	for (int i = 0; i < mpi_size_old; i++)
	  	{metadata_out[i] = 0;}

		//Size for data space in memory
		hsize_t mdim[1] = {(size_t)mpi_size_old};

		//Create data space in memory
		hid_t mem_dataspace_id = H5Screate_simple(1, mdim, NULL);
		if (mem_dataspace_id < 0)	{return false;}

	  	// Read the dataset.
	    err = H5Dread(dataset, H5T_NATIVE_INT, mem_dataspace_id, file_dataspace_id, plist_id, metadata_out);
	    if (err < 0)	{return false;}

	    openfpm::vector<size_t> metadata_accum;
	    metadata_accum.resize(mpi_size_old);

	    metadata_accum.get(0) = 0;
	    for (int i = 1 ; i < mpi_size_old ; i++)
	    	metadata_accum.get(i) = metadata_accum.get(i-1) + metadata_out[i-1];

	    //Open dataset
	    hid_t dataset_2 = H5Dopen (file, "vector_dist", H5P_DEFAULT);
	    if (dataset_2 < 0)	{return false;}

	    //Create property list for collective dataset read
	  	plist_id = H5Pcreate(H5P_DATASET_XFER);
	  	if (plist_id == -1)	{return false;}

	  	err = H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
	  	if (err < 0)	{return false;}

	  	openfpm::vector<size_t> n_block;
	  	n_block.resize(v_cl.getProcessingUnits());


	  	for(size_t i = 0 ; i < n_block.size() ; i++)
	  		n_block.get(i) = mpi_size_old / v_cl.getProcessingUnits();

	  	size_t rest_block = mpi_size_old % v_cl.getProcessingUnits();

	  	size_t max_block;

	  	if (rest_block != 0)
	  		max_block = n_block.get(0) + 1;
	  	else
	  		max_block = n_block.get(0);

	  	//for(size_t i = 0 ; i < n_block.size() ; i++)
	  	for(size_t i = 0 ; i < rest_block ; i++)
	  		n_block.get(i) += 1;

	  	size_t start_block = 0;
	  	size_t stop_block = 0;


	  	if (v_cl.getProcessUnitID() != 0)
	  	{
			for(size_t i = 0 ; i < v_cl.getProcessUnitID() ; i++)
				start_block += n_block.get(i);
	  	}

	  	stop_block = start_block + n_block.get(v_cl.getProcessUnitID());

	  	if (mpi_rank >= mpi_size_old)
	  	{load_block(start_block,mpi_size_old,metadata_out,metadata_accum,plist_id,dataset_2,g_m,v_pos,v_prp);}
	  	else
	  	{
	  		size_t n_bl = 0;
	  		size_t lb = start_block;
			for ( ; lb < stop_block ; lb++, n_bl++)
			{load_block(lb,mpi_size_old,metadata_out,metadata_accum,plist_id,dataset_2,g_m,v_pos,v_prp);}

			if (n_bl < max_block)
			{load_block(-1,mpi_size_old,metadata_out,metadata_accum,plist_id,dataset_2,g_m,v_pos,v_prp);}
	  	}

	  	// Close open object
	  	H5Sclose(mem_dataspace_id);
	  	H5Sclose(file_dataspace_id);
	    // Close the dataset.
	    H5Dclose(dataset);
	    H5Dclose(dataset_2);
	    // Close the file.
	    H5Fclose(file);
	    H5Pclose(plist_id);

	    return true;
	}
};


#endif /* OPENFPM_IO_SRC_HDF5_WR_HDF5_READER_VD_HPP_ */
