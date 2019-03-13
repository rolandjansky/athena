/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARFEBCONFIG_H
#define LARRECCONDITIONS_LARFEBCONFIG_H

#include "AthenaBaseComps/AthMessaging.h"
#include "Identifier/HWIdentifier.h"
#include "CoralBase/AttributeList.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include <vector>
#include <mutex>

class LArOnlineID;

class LArFebConfig:  public AthMessaging {

  friend class LArFEBConfigCondAlg; //The conditions alg filling this object

 public:
  LArFebConfig()=delete;
  LArFebConfig(const LArOnlineID* onlineId); // we do not own this pointer

  //Accessor methods from ILArFEBConfigReader
  short lowerGainThreshold(const HWIdentifier& id) const;
  short upperGainThreshold(const HWIdentifier& id) const;

 private:
  const LArOnlineID* m_onlineID;

  std::map<HWIdentifier,const coral::AttributeList*> m_attrPerFeb;
  mutable std::map<HWIdentifier,const coral::AttributeList*>::const_iterator m_lastIt;
  mutable std::mutex m_itMtx;

  short getThreshold(const char* MedLow, const HWIdentifier& chid) const;

  enum {ERRORCODE = LArElecCalib::ERRORCODE};

};

inline short LArFebConfig::lowerGainThreshold(const HWIdentifier& chid) const {
  return getThreshold("lower",chid);
}


inline short LArFebConfig::upperGainThreshold(const HWIdentifier& chid) const {
  return getThreshold("upper",chid);
}


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArFebConfig, 155518838 , 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArFebConfig>, 148539538 , 1)

#endif
