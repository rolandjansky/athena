/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARFEBCONFIG_H
#define LARRECCONDITIONS_LARFEBCONFIG_H

#include "AthenaBaseComps/AthMessaging.h"
#include "Identifier/HWIdentifier.h"
#include "CoralBase/AttributeList.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include <unordered_map>

class LArOnlineID;

class LArFebConfig:  public AthMessaging {
 public:
  LArFebConfig()=delete;
  LArFebConfig(const LArOnlineID* onlineId); // we do not own this pointer

  void add (HWIdentifier febid,
            const coral::AttributeList* attrList);

  //Accessor methods from ILArFEBConfigReader
  short lowerGainThreshold(const HWIdentifier& id) const;
  short upperGainThreshold(const HWIdentifier& id) const;
  void thresholds (const HWIdentifier& chid, short& lower, short& upper) const;
  

 private:
  const LArOnlineID* m_onlineID;

  std::unordered_map<HWIdentifier::value_type,const coral::AttributeList*> m_attrPerFeb;

  const coral::AttributeList* getAttrList (const HWIdentifier& chid,
                                           int& channel) const;
  short
  getThresholdFromAttrList(const std::string& MedLow,
                           const coral::AttributeList* attrList,
                           const std::string& chanstr) const;
  short getThreshold(const std::string& MedLow, const HWIdentifier& chid) const;

  static const std::string s_lower;
  static const std::string s_upper;

  enum {ERRORCODE = LArElecCalib::ERRORCODE};

};

inline short LArFebConfig::lowerGainThreshold(const HWIdentifier& chid) const {
  return getThreshold(s_lower, chid);
}


inline short LArFebConfig::upperGainThreshold(const HWIdentifier& chid) const {
  return getThreshold(s_upper,chid);
}


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArFebConfig, 155518838 , 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArFebConfig>, 148539538 , 1)

#endif
