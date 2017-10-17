/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "EvgenProdTools/CopyEventWeight.h"
#include "EvgenProdTools/CountHepMC.h"
#include "EvgenProdTools/FixHepMC.h"
#include "EvgenProdTools/TestHepMC.h"
#include "EvgenProdTools/EvgenOTFTopUpSvc.h"
#include "EvgenProdTools/SimTimeEstimate.h"

DECLARE_ALGORITHM_FACTORY(CopyEventWeight)
DECLARE_ALGORITHM_FACTORY(CountHepMC)
DECLARE_ALGORITHM_FACTORY(FixHepMC)
DECLARE_ALGORITHM_FACTORY(TestHepMC)
DECLARE_SERVICE_FACTORY(EvgenOTFTopUpSvc)
DECLARE_ALGORITHM_FACTORY(SimTimeEstimate)



DECLARE_FACTORY_ENTRIES( EvgenProdTools ) {
  DECLARE_ALGORITHM( ExpressionGenFilter )
  DECLARE_ALGORITHM(CopyEventWeight)
  DECLARE_ALGORITHM(CountHepMC)
  DECLARE_ALGORITHM(FixHepMC)
  DECLARE_ALGORITHM(TestHepMC)
  DECLARE_SERVICE(EvgenOTFTopUpSvc)
  DECLARE_SERVICE(HepMCWeightSvc)
  DECLARE_ALGORITHM(SimTimeEstimate)
}
