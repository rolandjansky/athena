/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  @file PairBuilder.cpp
 *  Contains class definition for PairBuilder and overloaded "<< operator" for same.
 * 
 *  @author Shaun Roe
 *  @date Sat Apr 10 2004.
 */

#include "PairBuilder.h"

PairBuilder :: PairBuilder()
{
}
PairBuilder ::~ PairBuilder()
{
	
}

void PairBuilder ::inputx(const int xstrip){
	bool xValueInserted = m_xvalues.insert(xstrip).second;
	if ((not m_yvalues.empty()) and xValueInserted)  formNewPairsWithX(xstrip);	
}
void PairBuilder ::inputy(const int ystrip){
	bool yValueInserted = m_yvalues.insert(ystrip).second;
	if ((not m_xvalues.empty()) and yValueInserted) formNewPairsWithY(ystrip);	
}
void PairBuilder ::inputxy(const int xystrip){
        bool xValueInserted = m_xvalues.insert(xystrip).second;
	bool yValueInserted = m_yvalues.insert(xystrip).second;
	if ((not m_xvalues.empty()) and (not m_yvalues.empty()) and xValueInserted and  yValueInserted) {
 	  formNewPairsWithX(xystrip);	
	  formNewPairsWithY(xystrip);
	}
}
int PairBuilder::numberOfPairs() const {
	return m_pairs.size();
}
PairBuilder::PairVector PairBuilder::pairs() const {
	return m_pairs;
}
float PairBuilder::weight() const {
	return 1.0/numberOfPairs();
}
PairBuilder::XYPair PairBuilder::pairAtIndex(const int indx) const {
	return m_pairs.at(indx);
}

int PairBuilder::xOfPairAt(const int indx) const {
	return pairAtIndex(indx).first;
}

int PairBuilder::yOfPairAt(const int indx) const {
	return pairAtIndex(indx).second;
}

void PairBuilder::formNewPairsWithX(const int xval){
	IntSet::iterator pthisY;
	for(pthisY = m_yvalues.begin(); pthisY != m_yvalues.end(); ++pthisY) {
		m_pairs.push_back(XYPair(xval,*pthisY)); 
	}
}
void PairBuilder::formNewPairsWithY(const int yval){
	IntSet::iterator pthisX;
	for(pthisX = m_xvalues.begin(); pthisX != m_xvalues.end(); ++pthisX) {
		m_pairs.push_back(XYPair(*pthisX, yval)); 
	}
}

std::ostream & operator<< (std::ostream &os, const PairBuilder &e) {
	int arrSize = e.numberOfPairs();
	for(int i=0; i<arrSize;++i){
		os <<  "( " << e.xOfPairAt(i) << ", " << e.yOfPairAt(i) << " )" << std::endl;
	}
	return os;
}
