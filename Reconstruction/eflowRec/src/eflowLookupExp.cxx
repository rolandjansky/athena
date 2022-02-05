/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowLookupExp.cxx
 *
 *  Created on: 20.08.2013
 *      Author: tlodd
 */

#include "eflowRec/eflowLookupExp.h"
#include "CxxUtils/CachedUniquePtr.h"
#include "CxxUtils/checker_macros.h"


const eflowLookupExp* eflowLookupExp::getInstance(int nExpBins /*= 50*/,
                                                  int nExpSubBins /*= 1000*/)
{
  static CxxUtils::CachedUniquePtr<eflowLookupExp> cached ATLAS_THREAD_SAFE;
  if (!cached) {
    cached.set (std::make_unique<eflowLookupExp>(nExpBins, nExpSubBins));
  }

  if ( (cached->m_nExpBins != nExpBins) || (cached->m_nExpSubBins != nExpSubBins) )
  {
    throw std::runtime_error("eflowLookupExp: Instance with different bin numbers than existing requested!");
  }
  return cached.get();
}
