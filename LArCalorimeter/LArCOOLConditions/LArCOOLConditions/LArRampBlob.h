/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARRAMPBLOB_H
#define LARCOOLCONDITIONS_LARRAMPBLOB_H

#include "LArElecCalib/ILArRamp.h" 
#include "Identifier/IdentifierHash.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include <vector>

class CondAttrListCollection;

class LArRampBlob {  

public:
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

  LArRampBlob();
  ~LArRampBlob();

protected:
  void readBlob(const CondAttrListCollection* attrList, MsgStream& msg);

  ILArRamp::RampRef_t ADC2DACByHash(const IdentifierHash& hs, int gain) const {
    const float* start=m_pRamp[gain]+hs*m_nPoints;
    if (*start==ERRORCODE) 
      return ILArRamp::RampRef_t(NULL,NULL);
    else
      return ILArRamp::RampRef_t(start,start+m_nPoints);
  }
  
protected:
  unsigned m_nChannels;
  unsigned m_nPoints;

private:
  std::vector<const float*> m_pRamp;
};


#endif 
