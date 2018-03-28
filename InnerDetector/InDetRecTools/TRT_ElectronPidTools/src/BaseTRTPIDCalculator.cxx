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

//STL includes
#include <sstream>




/**************************************************************************** \
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  BaseTRTPIDCalculator class  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  common funtionality of the other two Calculator classes  %%%%%%%%%%%%%*|
|*%%%  is implemented here  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

void InDet::BaseTRTPIDCalculator::PrintBlob(){
  //Print out the array values that should be entered into the database
  ATH_MSG_DEBUG("Now printing the contents of the "<<my_name<<" Calibration Blob, "<<BLOB_SIZE<<" chars");
  ATH_MSG_DEBUG("[ ");
  for(int i=0; i < BLOB_SIZE; i++)
    ATH_MSG_DEBUG(int(Blob[i])<<", ");
  ATH_MSG_DEBUG(" ]");
}

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
    ATH_MSG_DEBUG("The "<<my_name<<"calculator is about to be used uninitialized");
    ATH_MSG_DEBUG("Loading default calibration into memory.");
    setDefaultCalibrationConstants();
    HasBeenInitialized=1;
    if (parent.msgLvl(MSG::DEBUG)) {
        PrintBlob();
    }
  }
}

float InDet::BaseTRTPIDCalculator::Limit(float prob){
  if( prob > UpperLimit ){
    return UpperLimit;
  }
  else if( prob < LowerLimit ){
    return LowerLimit;
  }
  
  return prob;
}

