/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARDSPTHRESHOLDSFLAT_H
#define LARCOOLCONDITIONS_LARDSPTHRESHOLDSFLAT_H

#include "LArCOOLConditions/LArCondFlatBase.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"


class AthenaAttributeList;

class LArDSPThresholdsFlat: public LArCondFlatBase {
  
private: 
  LArDSPThresholdsFlat(); //private default constructor
  
public:
  LArDSPThresholdsFlat(const AthenaAttributeList* attrList);

  //~LArDSPThresholdsFlat();

  bool good() const { return m_isInitialized && m_nChannels>0; }

  
  // retrieving DSPThresholds using online ID
  
  float tQThr(const HWIdentifier&  CellID) const;  
  float samplesThr(const HWIdentifier&  CellID) const;
  float trigSumThr(const HWIdentifier&  CellID) const;


  float tQThrByHash(const IdentifierHash& h) const;  
  float samplesThrByHash(const IdentifierHash& h) const;  
  float trigSumThrByHash(const IdentifierHash& h) const;  

  //const std::string& name() { return m_setName;}

private:

  void readBlob(const AthenaAttributeList* attr);

  unsigned m_nChannels;
  const float* m_ptQThr;
  const float* m_psamplesThr;
  const float* m_ptrigSumThr;
  //std::string m_setName;

};

CLASS_DEF( LArDSPThresholdsFlat, 194681315 ,1  )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( LArDSPThresholdsFlat, 148006985 );
#endif 
