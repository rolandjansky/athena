/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

/* Lookup-table based exponential function to save CPU time */
class eflowLookupExp{
public:
  static eflowLookupExp* getInstance(int nExpBins = 50, int nExpSubBins = 1000){
    if ( !m_instance ) {
      m_instance = new eflowLookupExp(nExpBins, nExpSubBins);
    } else {
      /* Make sure the requested bin numbers are consistent with the existing instance */
      if ( (m_instance->m_nExpBins != nExpBins) || (m_instance->m_nExpSubBins != nExpSubBins) ) {
        throw std::runtime_error("eflowLookupExp: Instance with different bin numbers than existing requested!");
      }
    }
    return m_instance;
  }

  private:
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
    m_instance = this;
  }
public:
  ~eflowLookupExp(){ m_instance = 0; }

  double evaluate(double x) {
    int iExpBin = (int) x;
    int iSubBin(((x-iExpBin)*m_nExpSubBins));

    if (iExpBin > m_nExpBins){
//      std::cout << "Exceeded # exp bins: " << iExpBin << " > " << nExpBins << std::endl;
      return exp(-x);
    }
    return m_exp[iExpBin]*m_subExp[iSubBin];
  }

private:
  int m_nExpBins;
  int m_nExpSubBins;
  std::vector<double> m_exp;
  std::vector<double> m_subExp;
  static eflowLookupExp* m_instance;
};



#endif /* EFLOWLOOKUPEXP_H_ */
