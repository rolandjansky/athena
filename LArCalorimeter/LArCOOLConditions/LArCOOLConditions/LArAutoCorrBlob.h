/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARAUTOCORRBLOB_H
#define LARCOOLCONDITIONS_LARAUTOCORRBLOB_H

#include "LArElecCalib/ILArAutoCorr.h" 
#include "Identifier/IdentifierHash.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include <vector>

class CondAttrListCollection;

class LArAutoCorrBlob {  

public:
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

  LArAutoCorrBlob();
  ~LArAutoCorrBlob();

protected:
  void readBlob(const CondAttrListCollection* attrList, MsgStream& msg);

  ILArAutoCorr::AutoCorrRef_t autocorrByHash(const IdentifierHash& hs, int gain) const {
    const float* start=m_pAutoCorr[gain]+hs*m_nPoints;
    if (*start==ERRORCODE) 
      return ILArAutoCorr::AutoCorrRef_t(NULL,NULL);
    else
      return ILArAutoCorr::AutoCorrRef_t(start,start+m_nPoints);
  }
  
protected:
  unsigned m_nChannels;
  unsigned m_nPoints;

private:
  std::vector<const float*> m_pAutoCorr;
};


#endif 
