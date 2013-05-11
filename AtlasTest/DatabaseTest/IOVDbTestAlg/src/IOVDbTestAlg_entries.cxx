/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "IOVDbTestAlg/IOVDbTestAlg.h"
#include "IOVDbTestAlg/IOVDbTestCoolDCS.h"

DECLARE_ALGORITHM_FACTORY(IOVDbTestAlg)
DECLARE_ALGORITHM_FACTORY(IOVDbTestCoolDCS)

DECLARE_FACTORY_ENTRIES(IOVDbTestAlg) {
  DECLARE_ALGORITHM( IOVDbTestAlg );
  DECLARE_ALGORITHM( IOVDbTestCoolDCS );
}
