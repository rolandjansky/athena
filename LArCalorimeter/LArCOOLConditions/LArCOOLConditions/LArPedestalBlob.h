/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARPEDESTALBLOB_H
#define LARCOOLCONDITIONS_LARPEDESTALBLOB_H

//#include "LArElecCalib/ILArPedestal.h" #include "LArCOOLConditions/LArCondBlobBase.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/MsgStream.h"

class CondAttrListCollection;

class LArPedestalBlob {
  
public:
  LArPedestalBlob();
  ~LArPedestalBlob();
  //enum {ERRORCODE = LArElecCalib::ERRORCODE};

protected:
  void readBlob(const CondAttrListCollection* attrList, MsgStream& msg);

  float pedestalByHash(const IdentifierHash& hs, const unsigned gain) const {
    // check ranges ???
    return m_pPedestal[gain][hs];
  }

  float pedestalRMSByHash(const IdentifierHash& hs, const unsigned gain) const {
    // check ranges ???
    return m_pPedestalRMS[gain][hs];
  }

private:
  std::vector<const float*> m_pPedestal;
  std::vector<const float*> m_pPedestalRMS;
protected:
  unsigned m_nChannels;


};

#endif 
