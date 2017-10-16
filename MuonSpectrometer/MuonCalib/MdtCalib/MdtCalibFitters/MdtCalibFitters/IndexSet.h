/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 09.05.2005, AUTHOR: OLIVER KORTNER
// Modified: 16.07.2006 by O. Kortner, vector size check in initialization
//                      added,
//                      != operator corrected.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef IndexSetHXX
#define IndexSetHXX

//::::::::::::::::::::
//:: CLASS IndexSet ::
//::::::::::::::::::::

/// \class IndexSet
/// This class allows the user to save a set of indices and to compare two sets
/// of indices. IndexSet is an auxiliary class for 
/// QuasianalyticLineReconstruction.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 09.05.2005

//////////////////
// HEADER FILES //
//////////////////

// standard C++ libraries //
#include <iostream>

// STEL //
#include <vector>

namespace MuonCalib {

class IndexSet {

private:
// internal index memory //
	unsigned int m_nb_indices; // number of indices
	std::vector<int> m_index; // indices

// initialization methods //
	void init(void);
	  // default initiailization method; the number of indices is set to 0
	void init(const unsigned int & r_nb_indices);
	  // initialization method; the number of indices is set to r_nb_indices
	void init(const unsigned int & r_nb_indices,
					const std::vector<int> r_index);
	  // initialization method; the number of indices is set to 
	  // r_nb_indices, the vector r_index contains the indices

public:
// Constructors //
	IndexSet(void) {
		init();
		}
	///< default constructor: the number of indices is set to 0

	IndexSet(const unsigned int & r_nb_indices) {
		init(r_nb_indices);
		}
	///< constructor: the number of indices is set to r_nb_indices

	IndexSet(const unsigned int & r_nb_indices,
					const std::vector<int> r_index) {
		init(r_nb_indices, r_index);
		}
	///< constructor: the number of indices is set to r_nb_indices,
	///<              the vector r_index contains the indices

// Methods //
	unsigned int size(void) const;
	                                ///< get the number of indices
	void resize(const unsigned int & r_size);
	                                ///< resize the index set to r_size; 
	                                ///< the index store is preserved as
	                                ///< far as possible
	int operator [] (const unsigned int & r_k);
	int operator [] (const unsigned int & r_k) const;
	                                ///< access to the indices stored in 
	                                ///< the class,
	                                ///< 0 <= r_k < size();
	                                ///< WARNING: no test on the index is 
	                                ///< performed
	void sort(void);
	                                ///< sort the indices in ascending order
	bool operator == (const IndexSet & r_index_set) const;
	bool operator != (const IndexSet & r_index_set) const;
	                                ///< comparison of two index sets;
	                                ///< two index sets are considered 
	                                ///< equal if they are of the same size
	                                ///< and their indices are the same at 
	                                ///< the same position [.]

};

}

#endif
