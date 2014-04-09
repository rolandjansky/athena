/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  McEventSelector_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Generators/McEventSelector
//
//  Description: Implementation of <Package>_load routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================

#include "McEventSelector/McEventCnv.h"
#include "McEventSelector/McCnvSvc.h"
#include "McEventSelector/McEventSelector.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_CONVERTER_FACTORY(McEventCnv)

DECLARE_SERVICE_FACTORY(McCnvSvc)
DECLARE_SERVICE_FACTORY(McEventSelector)

DECLARE_FACTORY_ENTRIES( McEventSelector ) {
    DECLARE_CONVERTER( McEventCnv );
    DECLARE_SERVICE  ( McEventSelector );
    DECLARE_SERVICE  ( McCnvSvc );
}
