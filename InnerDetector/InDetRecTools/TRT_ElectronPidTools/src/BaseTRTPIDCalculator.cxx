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
  parent.msg(MSG::ERROR)<<"Now printing the contents of the "<<my_name<<" Calibration Blob, "<<BLOB_SIZE<<" chars"<<endmsg;
  parent.msg(MSG::ERROR)<<"This should never appear in a production: [ ";
  for(int i=0; i < BLOB_SIZE; i++)
    parent.msg(MSG::ERROR)<<int(Blob[i])<<", ";
  parent.msg(MSG::ERROR)<<" ]"<<endmsg;
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
    parent.msg(MSG::ERROR)<<"The "<<my_name<<"calculator is about to be used uninitialized"<<endmsg;
    parent.msg(MSG::ERROR)<<"Loading default calibration into memory."<<endmsg;
    setDefaultCalibrationConstants();
    HasBeenInitialized=1;
    PrintBlob();
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

