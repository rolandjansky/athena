/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 27.10.2007, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_CombinationH
#define MuonCalib_CombinationH

//:::::::::::::::::::::::
//:: CLASS Combination ::
//:::::::::::::::::::::::

/// \class Combination
///
/// This class provides a combination very quickly.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 27.10.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>

namespace MuonCalib {

class Combination {

public:
// Constructors //
	Combination(void);
	///< Default construcor. A combination of 1 out of 1 is created.

	Combination(const unsigned int & n, const unsigned int & k);
	///< Constructor.
	///< @param n Number of elements.
	///< @param k Class of the combination.

// Methods //
// get-methods //
	unsigned int numberOfElements(void) const;
				///< get the number of elements
	unsigned int whichClass(void) const;
				///< get the class of which the combination is
	unsigned int numberOfCombinations(void) const;
				///< get the number of combinations
	void currentCombination(std::vector<unsigned int> & index_array) const;
				///< get the current combination; the result
				///< is stored in the vector index_array
	void nextCombination(std::vector<unsigned int> & index_array);
				///< get the next combination; the results is
				///< stored in the array index_array

// set-methods //
	void reset(void);
				///< go back to the first combination
	void setNewParameters(const unsigned int & nb_elements,
					const unsigned int & wh_class);
				///< set the number of elements = nb_elements;
				///< set the class of which the combination is
				///< = wh_class

private:
	unsigned int m_n = 0U; // the n of Comb(n, k)
	unsigned int m_k = 0U; // the k of Comb(n, k)
	std::vector<unsigned int> m_index; // vector containing the indices of
					   // the current combination
	std::vector<unsigned int> m_flag; // vector of flags

	void init(const unsigned int & n, const unsigned int & k);
					// initialization method;
					// n: number of elements;
					// k: Class of the combination

};

}


#endif
