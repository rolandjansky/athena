/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  @file PairBuilder.h
 *  Contains class declaration for PairBuilder,
 *  and overloaded "operator <<" for output of same.
 *
 *  @author Shaun Roe
 *  @date Sat Apr 10 2004.
 *  
 */
#ifndef PairBuilder_H
#define PairBuilder_H

#include <vector>
#include <utility>
#include <ostream>
#include <set>

/** @class PairBuilder
 *  @brief Class to build pairs of XY values. 
 *
 *  Given a sequence of x and y integer values,
 *  PairBuilder will make all possible XY pairs
 *  and return the pairs in vector.
 */

class  PairBuilder {
public:
	PairBuilder();
	~PairBuilder();
	
	typedef std::pair<int, int> XYPair;
	typedef std::vector< XYPair > PairVector;
	typedef std::set<int> IntSet;
	
	/// Input a new x value
	void inputx(const int xstrip);
	/// Input a new y value
	void inputy(const int ystrip);
	/// Input a new y value
	void inputxy(const int ystrip);
	/// Return the number of XY pairs made
	int numberOfPairs() const;
	/// Return the pairs as a vector of pairs of integers
	PairVector pairs() const;
	/// Return the 'weight', = 1/(number of pairs)
	float weight() const;
	/// Return a specific XY pair at some vector index
	XYPair pairAtIndex(const int indx) const;
	/// Return the X value of a specific pair
	int xOfPairAt(const int indx) const;
	/// Return the Y value of a specific pair
	int yOfPairAt(const int indx) const;
	
private:
	/**    @name Sets to hold x values and y values. */
	//@{
	IntSet m_yvalues;
	/// use sets to avoid duplicate values
	IntSet m_xvalues;
	//@}
	///Vector to hold the pairs produced.
	PairVector m_pairs;
	/**    @name Functions which form the pairs. */
	//@{
	void formNewPairsWithX(const int xval);
	void formNewPairsWithY(const int yval);
	//@}

};

///Overloaded "operator <<" for output of PairBuilder objects
std::ostream& operator<< (std::ostream& os, const PairBuilder& e);

#endif
