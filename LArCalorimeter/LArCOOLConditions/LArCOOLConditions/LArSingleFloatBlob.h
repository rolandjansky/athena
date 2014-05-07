/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARSINGLEFLOATBLOB_H
#define LARCOOLCONDITIONS_LARSINGLEFLOATBLOB_H

#include "Identifier/IdentifierHash.h"
#include <vector>

class CondAttrListCollection;
class MsgStream;

class LArSingleFloatBlob {
  
public:
  LArSingleFloatBlob(); //private default constructor
  
  void readBlob(const CondAttrListCollection* attrList, const char* attrName, MsgStream& msg);


protected:
  
  inline const float& getDataByHash(const IdentifierHash& hs, const unsigned gain) const {
    return m_pValues[gain][hs];
  }

  std::vector<const float*> m_pValues;
  unsigned m_nChannels;
};

/*
inline const float& LArSingleFloatBlob::getData(const Identifier&  CellID, const int gain) const {
  const HWIdentifier OnId = m_larCablingSvc->createSignalChannelID(CellID);
  return getData(OnId,gain);
}
  
inline const float& LArSingleFloatBlob::getData(const HWIdentifier&  CellID, int gain) const {
  return getDataByHash(m_onlineHelper->channel_Hash(CellID),gain);
}
*/

#endif 
