/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowLookupExp.cxx
 *
 *  Created on: 20.08.2013
 *      Author: tlodd
 */

#include "eflowRec/eflowLookupExp.h"

//eflowLookupExp* eflowLookupExp::getInstance(int nExpBins = 50, int nExpSubBins = 1000) {
//    if ( m_instance ) {
//      /* Make sure the requested bin numbers are consistent with the existing instance */
//      if ( (m_instance->m_nExpBins != nExpBins) || (m_instance->m_nExpSubBins != nExpSubBins) ) {
//        throw std::runtime_error("eflowLookupExp: Instance with different bin numbers than existing requested!");
//      }
//      return m_instance;
//    } else {
//      return new eflowLookupExp(nExpBins, nExpSubBins);
//    }
//  }

//eflowLookupExp::~eflowLookupExp() { m_instance = 0; }

eflowLookupExp* eflowLookupExp::m_instance = 0;



