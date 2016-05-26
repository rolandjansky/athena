/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LArConditionsTest/LArConditionsTestAlg.h"
#include "LArConditionsTest/LArCondDataTest.h"
//#include "LArConditionsTest/FixLArIdMap.h"
#include "LArConditionsTest/LArFecLvTempDcsTest.h"
#include "LArConditionsTest/FixLArElecCalib.h"
#include "LArConditionsTest/TestLArConditionsTools.h" 
#include "LArConditionsTest/LArCablingTest.h"
#include "LArConditionsTest/LArIdMapConvert.h"
#include "LArConditionsTest/LArFebRodMapConvert.h"
#include "LArConditionsTest/LArShapeToSCShape.h"
#include "LArConditionsTest/LArOFCtoOFC.h"

DECLARE_ALGORITHM_FACTORY( LArConditionsTestAlg )
DECLARE_ALGORITHM_FACTORY( LArCondDataTest )
//DECLARE_ALGORITHM_FACTORY( FixLArIdMap )
DECLARE_ALGORITHM_FACTORY( LArFecLvTempDcsTest )
DECLARE_ALGORITHM_FACTORY( FixLArElecCalib )
DECLARE_ALGORITHM_FACTORY( TestLArConditionsTools ) 
DECLARE_ALGORITHM_FACTORY ( LArCablingTest )
DECLARE_ALGORITHM_FACTORY ( LArIdMapConvert )
DECLARE_ALGORITHM_FACTORY ( LArFebRodMapConvert )
DECLARE_ALGORITHM_FACTORY ( LArShapeToSCShape )
DECLARE_ALGORITHM_FACTORY ( LArOFCtoOFC )


DECLARE_FACTORY_ENTRIES( LArConditionsTest ) {
  DECLARE_ALGORITHM( LArConditionsTestAlg )
  DECLARE_ALGORITHM( LArCondDataTest )
    //DECLARE_ALGORITHM( FixLArIdMap )
  DECLARE_ALGORITHM( LArFecLvTempDcsTest )
  DECLARE_ALGORITHM( FixLArElecCalib )
  DECLARE_ALGORITHM( TestLArConditionsTools ) 
  DECLARE_ALGORITHM (LArCablingTest )
  DECLARE_ALGORITHM (LArIdMapConvert)
  DECLARE_ALGORITHM (LArFebRodMapConvert)
  DECLARE_ALGORITHM (LArShapeToSCShape)
  DECLARE_ALGORITHM (LArOFCtoOFC)
}
