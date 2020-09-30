/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include <iostream>
#include <cmath>

//
//_____________________________________________________________
CaloCondBlobFlt* 
CaloCondBlobFlt::getInstance(coral::Blob& blob)
{
  return (new CaloCondBlobFlt(blob));
}
//
//_____________________________________________________________
const CaloCondBlobFlt* 
CaloCondBlobFlt::getInstance(const coral::Blob& blob)
{
  return (new CaloCondBlobFlt(blob));
}

//
//_______________________________________________________________
CaloCondBlobFlt::CaloCondBlobFlt(const coral::Blob& blob) 
  : CaloCondBlobDat<float>(blob)
{
  //=== check for correct blob type
  if(getBlobSize()){
    if(getObjType() != getType()){
      throw CaloCond::TypeConflict("CaloCondBlobFlt::Ctor",getObjType(),getType());
    }
  }
}

//
//_______________________________________________________________
CaloCondBlobFlt::CaloCondBlobFlt(coral::Blob& blob) 
  : CaloCondBlobDat<float>(blob)
{
  //=== check for correct blob type
  if(getBlobSize()){
    if(getObjType() != getType()){
      throw CaloCond::TypeConflict("CaloCondBlobFlt::Ctor",getObjType(),getType());
    }
  }
}

//
//______________________________________________________________
float 
CaloCondBlobFlt::getCalib(unsigned int channel, unsigned int adc, float x, bool invert) const
{
  //=== interpretation of data depends on the objVersion
  if(getObjVersion()==1){
    //=== Total noise parameterized as
    //=== Sigma**2 = a**2 + b**2 * Lumi
    float a = getData(channel,adc,0);
    float b = getData(channel,adc,1);
    if(invert){ x = (x*x - a*a) / (b*b)     ;}
    else      { x = std::sqrt( a*a + b*b*x );}
  }
  else if (getObjVersion()==2) {
    //== parameterization for pedestal = a + b*Lumi
    float a = getData(channel,adc,0);
    float b = getData(channel,adc,1);
    if(invert){ x = (x - a) / (b)     ;}
    else      { x = a+b*x;}
  }
  else{
    throw CaloCond::VersionConflict("CaloCondBlobFlt::getCalib",getObjVersion());
  }
  
  return x;
}
