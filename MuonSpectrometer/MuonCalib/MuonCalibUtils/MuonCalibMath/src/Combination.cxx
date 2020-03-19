/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "TMath.h" // for TMath::Binomial()
#include <TString.h> // for Form

#include "MuonCalibMath/Combination.h"

using namespace MuonCalib;

//*****************************************************************************

//:::::::::::::::::::::::::
//:: DEFAULT CONSTRUCTOR ::
//:::::::::::::::::::::::::

Combination::Combination(void) {

	init(1,1);

}

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

Combination::Combination(const unsigned int & n, const unsigned int & k) {

	init(n, k);

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

void Combination::init(const unsigned int & n, const unsigned int & k) {

///////////////////
// CHECK IF k<=n //
///////////////////

	if (k>n) {
		throw std::runtime_error(Form("File: %s, Line: %d\nCombination::init() - error, class number greater than number of elements!", __FILE__, __LINE__));
	}

/////////////////////
// ALLOCATE MEMORY //
/////////////////////

	m_k = k;
	m_n = n;
	m_index = std::vector<unsigned int>(m_k);
	m_flag = std::vector<unsigned int>(m_k);

///////////////////////////////
// SETUP START CONFIGURATION //
///////////////////////////////

	reset();

	return;

}

//*****************************************************************************

//:::::::::::::::::::::::::::::
//:: METHOD numberOfElements ::
//:::::::::::::::::::::::::::::

unsigned int Combination::numberOfElements(void) const {

	return m_n;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD whichClass ::
//:::::::::::::::::::::::

unsigned int Combination::whichClass(void) const {

	return m_k;

}

//*****************************************************************************

//:::::::::::::::::::::::::::::::::
//:: METHOD numberOfCombinations ::
//:::::::::::::::::::::::::::::::::

unsigned int Combination::numberOfCombinations(void) const {

///////////////
// VARIABLES //
///////////////

	int ncomb = TMath::Binomial(m_n, m_k);

///////////////////////////////////////
// RETURN THE NUMBER OF COMBINATIONS //
///////////////////////////////////////

	return ncomb;

}

//*****************************************************************************

//:::::::::::::::::::::::::::::::
//:: METHOD currentCombination ::
//:::::::::::::::::::::::::::::::

void Combination::currentCombination(
			std::vector<unsigned int> & index_array) const {

	if (index_array.size()<m_k) {
		index_array = std::vector<unsigned int>(m_k);
	}
	for (unsigned int j=0; j<m_k; j++) {
		index_array[j] = m_index[j];
	}

	return;

}

//*****************************************************************************

//::::::::::::::::::::::::::::
//:: METHOD nextCombination ::
//::::::::::::::::::::::::::::

void Combination::nextCombination(std::vector<unsigned int> & index_array) {

///////////////
// VARIABLES //
///////////////

	int loc_flag; // local flag

//////////////////////////////
// CHECK SIZE OF index_array//
//////////////////////////////

	if (index_array.size()<m_k) {
		index_array = std::vector<unsigned int>(m_k);
	}

/////////////////
// CHECK FLAGS //
/////////////////

	for (unsigned int j=0; j<m_k; j++) {
		if (m_index[j] == (m_n-(m_k-j-1))) {
			m_flag[j] = 1;
		}
	}
	if (m_flag[0] == 1) {
		reset();
		currentCombination(index_array);
		return;
	}
	loc_flag = 0;
	for (unsigned int j=1; j<m_k; j++) {
		if (m_flag[j] == 1) {
			m_index[j-1] = m_index[j-1]+1;
			for (unsigned int l=j; l<m_k; l++) {
				m_index[l] = m_index[l-1]+1;
			}
			loc_flag = 1;
			break;
		}
	}
	if (loc_flag == 0) {
		m_index[m_k-1] = m_index[m_k-1]+1;
	}
	for (unsigned int j=0; j<m_k; j++) {
		if (m_index[j] == (m_n-(m_k-j-1))) {
			m_flag[j] = 1;
		}
		else {
			m_flag[j] = 0;
		}
		index_array[j] = m_index[j];
	}

////////////////////////////////////
// RETURN THE DESIRED COMBINATION //
////////////////////////////////////

	return;

}

//*****************************************************************************

//::::::::::::::::::
//:: METHOD reset ::
//::::::::::::::::::

void Combination::reset(void) {

	for (unsigned int j=0; j<m_k; j++) {
		m_index[j] = j+1;
		m_flag[j] = 0;
	}

	return;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::::
//:: METHOD setNewParameters ::
//::::::::::::::::::::::::::::::::

void Combination::setNewParameters(const unsigned int & nb_elements,
					const unsigned int & wh_class) {

	init(nb_elements, wh_class);

	return;

}
