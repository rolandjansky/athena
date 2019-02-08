/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARSHAPEBLOB_H
#define LARCOOLCONDITIONS_LARSHAPEBLOB_H

#include "LArElecCalib/ILArShape.h" 
//#include "LArCOOLConditions/LArCondBlobBase.h"
#include "Identifier/IdentifierHash.h"
//#include "LArIdentifier/LArOnlineID.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include <vector>

class CondAttrListCollection;

class LArShapeBlob {

 public:   
  //typedef ILArShape::ShapeRef_t ShapeRef_t;
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

  LArShapeBlob();
  ~LArShapeBlob( );


  void readBlob(const CondAttrListCollection* attrList, MsgStream& msg);

  ILArShape::ShapeRef_t ShapeByHash(const IdentifierHash& hs,int gain) const {
    const float* pStart=m_pShape[gain]+(hs*m_nSamples);
    if (*pStart==ERRORCODE) 
      return ILArShape::ShapeRef_t(NULL,NULL);
    else
      return ILArShape::ShapeRef_t(pStart,pStart+m_nSamples);
  }

  ILArShape::ShapeRef_t ShapeDerByHash(const IdentifierHash& hs,int gain) const {
    const float* pStart=m_pShapeDer[gain]+(hs*m_nSamples);
    if (*pStart==ERRORCODE) 
      return ILArShape::ShapeRef_t(NULL,NULL);
    else
      return ILArShape::ShapeRef_t(pStart,pStart+m_nSamples);
  }


protected:
  std::vector<const float*> m_pShape;
  std::vector<const float*> m_pShapeDer;
  std::vector<const float*> m_pTimeOffset;

  unsigned m_nChannels;
  unsigned m_nSamples;
};  
  

#endif 
