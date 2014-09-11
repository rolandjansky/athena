///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthMemMacros.h 
// Header file for useful macros when comes to using SegMemSvc
// Author: S.Binet<binet@cern.ch>
// $Revision$
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHMEMMACROS_H 
#define ATHENABASECOMPS_ATHMEMMACROS_H 1

// framework includes
#include "StoreGate/SegMemSvc.h"

// non-caching macros
#define ATH_NEW(T) \
  new(this->memSvc()->SegMemSvc::allocate< T >(::SegMemSvc::EVENT,false))T

// caching macros
#define ATH_CNEW(T) \
  new(this->memSvc()->SegMemSvc::allocate< T >(::SegMemSvc::EVENT, true))T

#endif //> !ATHENABASECOMPS_ATHMEMMACROS_H

