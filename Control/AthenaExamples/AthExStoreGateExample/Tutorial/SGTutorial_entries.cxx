/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//====================================================================
//  StoreGateExample_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : StoreGate/example
//
//  Description: Implementation of <Package>_load routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================

#include "SGRead.h"
#include "SGWrite.h"

DECLARE_COMPONENT( SGRead )
DECLARE_COMPONENT( SGWrite )

