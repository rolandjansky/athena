/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BaseTRTPIDCalculator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "TRT_ElectronPidTools/BaseTRTPIDCalculator.h"

// StoreGate, Athena, and Database stuff:
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Blob.h"

// Math functions:
#include <cmath>

/**************************************************************************** \
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  BaseTRTPIDCalculator class  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  common funtionality of the other two Calculator classes  %%%%%%%%%%%%%*|
|*%%%  is implemented here  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/
bool InDet::BaseTRTPIDCalculator::FillBlob(const unsigned char* source){
  //Copy the Coral Blob into a local array
  for(int i=0; i < BLOB_SIZE; i++){
    Blob[i]=source[i];
  }
  HasBeenInitialized=1;
  return 1;
}

void InDet::BaseTRTPIDCalculator::checkInitialization(){
  if( not HasBeenInitialized ) {
    setDefaultCalibrationConstants();
    HasBeenInitialized=1;
  }
}

float InDet::BaseTRTPIDCalculator::Limit(float prob){
  if( prob > UpperLimit ){
    return UpperLimit;
  }
  if( prob < LowerLimit ){
    return LowerLimit;
  }
  
  return prob;
}

