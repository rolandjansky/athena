/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "IdDictTest/IdDictTest.h"
#include "IdDictSimpleTest.h"

DECLARE_ALGORITHM_FACTORY( IdDictTest )
DECLARE_ALGORITHM_FACTORY( IdDictSimpleTest )

DECLARE_FACTORY_ENTRIES	( IdDictTest ) {
    DECLARE_ALGORITHM	( IdDictTest   )
    DECLARE_ALGORITHM	( IdDictSimpleTest   )
}


