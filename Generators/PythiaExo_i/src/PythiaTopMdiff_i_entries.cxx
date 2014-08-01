/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  GeneratorModules_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Generators/GeneratorModules
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "PythiaExo_i/PythiaTopMdiff.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( PythiaTopMdiff )

DECLARE_FACTORY_ENTRIES( PythiaTopMdiff_i ) {
    DECLARE_ALGORITHM( PythiaTopMdiff )
}
