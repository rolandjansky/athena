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

#include "PythiaExo_i/PythiaLLP.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( PythiaLLP )

DECLARE_FACTORY_ENTRIES( PythiaLLP_i ) {
    DECLARE_ALGORITHM( PythiaLLP )
}
