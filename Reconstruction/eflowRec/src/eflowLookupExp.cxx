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

//eflowLookupExp* eflowLookupExp::m_instance = nullptr;
std::unique_ptr<eflowLookupExp> eflowLookupExp::m_instance  = nullptr;


