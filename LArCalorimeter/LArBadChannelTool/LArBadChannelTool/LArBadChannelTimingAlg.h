/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelTimingAlg_H
#define LArBadChannelTimingAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"

class EventInfo;
class LArEM_ID;


class LArBadChannelTimingAlg : public AthAlgorithm 
{
public:
  LArBadChannelTimingAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArBadChannelTimingAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey;
  const CaloCell_ID* m_cellID;
  const LArOnlineID* m_onlineID = nullptr;
  const LArEM_ID*    m_emID = nullptr;
  std::vector<unsigned int> m_hwarray;
  bool m_reallyCheck;

  void fillChannelArray();

  void timeOfflineAccess();
  void timeOnlineAccess();
  void timeFEBAccess();

  int testsPerEvent() const {return 1000000;}
};

#endif
