/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowLookupExp.h
 *
 *  Created on: 20.08.2013
 *      Author: tlodd
 */

#ifndef EFLOWLOOKUPEXP_H_
#define EFLOWLOOKUPEXP_H_

#include <vector>
#include <stdexcept>
#include <math.h>
#include <memory>
#include <mutex>

/** Lookup-table based exponential function to save CPU time, which is used by eflowCellIntegrator */
class eflowLookupExp{
public:
  static const eflowLookupExp* getInstance(int nExpBins = 50, int nExpSubBins = 1000);

  //private:
  eflowLookupExp(int nExpBins, int nExpSubBins) :
      m_nExpBins(nExpBins), m_nExpSubBins(nExpSubBins), m_exp(nExpBins+1), m_subExp(nExpSubBins+1) {
    /* Lookup table for the integer part of a number */
    for (int i = 0; i <= nExpBins; ++i) {
      m_exp[i] = exp(-i);
    }
    /* Lookup table for the fractional part of a number */
    const double substep = 1./nExpSubBins;
    for (int iSub = 0; iSub <= nExpSubBins; ++iSub){
      m_subExp[iSub] = exp(-substep* iSub);
    }
  }
public:
  ~eflowLookupExp(){ }

  double evaluate(double x) const {
    int iExpBin = (int) x;
    int iSubBin(((x-iExpBin)*m_nExpSubBins));

    if (iExpBin > m_nExpBins){
      return exp(-x);
    }
    return m_exp[iExpBin]*m_subExp[iSubBin];
  }

private:
  int m_nExpBins;
  int m_nExpSubBins;
  std::vector<double> m_exp;
  std::vector<double> m_subExp;
};



#endif /* EFLOWLOOKUPEXP_H_ */
