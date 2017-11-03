/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 09.05.2005, AUTHOR: OLIVER KORTNER
// Modified: 16.07.2006 by O. Kortner, vector size check in initialization
//                      added,
//                      != operator corrected.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATIONS OF METHODS DEFINED IN THE CLASS IndexSet ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MdtCalibFitters/IndexSet.h"
#include <algorithm>
#include <cstdlib>

//:::::::::::::::::::::::
//:: NAMESPACE SETTING ::
//:::::::::::::::::::::::

using namespace MuonCalib;
using namespace std;

//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

void IndexSet::init(void) {

	m_nb_indices = 0;
	return;

}

//*****************************************************************************

//::::::::::::::::::::
//:: METHOD init(.) ::
//::::::::::::::::::::

void IndexSet::init(const unsigned int & r_nb_indices) {

	m_nb_indices = r_nb_indices;
	m_index = vector<int>(m_nb_indices);
	return;

}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD init(.,.) ::
//::::::::::::::::::::::::

void IndexSet::init(const unsigned int & r_nb_indices,
					const vector<int> r_index) {

///////////////////////
// CHECK VECTOR SIZE //
///////////////////////

	if (r_index.size()<r_nb_indices) {
		cerr << endl
			<< "Class IndexSet, method init: ERROR!\n"
			<< "Index vector too short!\n";
		exit(1);
	}

////////////////////
// INITIALIZATION //
////////////////////

	m_nb_indices = r_nb_indices;
	m_index = vector<int>(m_nb_indices);
	for (unsigned int k=0; k<m_nb_indices; k++) {
		if (k<r_index.size()) {
			m_index[k] = r_index[k];
		} else {
			m_index[k] = 0;
		}
	}

	return;

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD size ::
//:::::::::::::::::

unsigned int IndexSet::size(void) const {

	return m_nb_indices;

}

//*****************************************************************************

//:::::::::::::::::::
//:: METHOD resize ::
//:::::::::::::::::::

void IndexSet::resize(const unsigned int & r_size) {

//:::::::::::::::
//:: VARIABLES ::
//:::::::::::::::

	vector<int> aux_index = m_index; // vector for temporary storage of
	                                 // the indices

//:::::::::::::::::::::::::::::::::::::
//:: RESIZE AND COPY THE OLD INDICES ::
//:::::::::::::::::::::::::::::::::::::

	m_index = vector<int>(r_size);
	for (unsigned int k=0; k<r_size; k++) {
		if (k<m_nb_indices) {
			m_index[k] = aux_index[k];
		} else {
			m_index[k] = 0;
		}
	}
	m_nb_indices = r_size;

	return;

}

//*****************************************************************************

///:::::::::::::::::::::::
//:: METHOD operator [] ::
//::::::::::::::::::::::::

int IndexSet::operator [] (const unsigned int & r_k) {

	return m_index[r_k];

}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD operator [] ::
//::::::::::::::::::::::::

int IndexSet::operator [] (const unsigned int & r_k) const {

	return m_index[r_k];

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD sort ::
//:::::::::::::::::

void IndexSet::sort(void) {

	std::stable_sort(m_index.begin(), m_index.end());
	return;

}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD operator == ::
//::::::::::::::::::::::::

bool IndexSet::operator == (const IndexSet & r_index_set) const {

	if (r_index_set.size()!=m_nb_indices) {
		return false;
	}

	for (unsigned int k=0; k<m_nb_indices; k++) {
		if (m_index[k]!=r_index_set[k]) {
			return false;
		}
	}

	return true;

}

//*****************************************************************************

//:::::::::::::::
//:: METHOD != ::
//:::::::::::::::

bool IndexSet::operator != (const IndexSet & r_index_set) const {

	return !(*this==r_index_set);

}
