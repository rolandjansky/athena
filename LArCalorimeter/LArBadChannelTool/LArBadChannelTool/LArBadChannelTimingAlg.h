/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelTimingAlg_H
#define LArBadChannelTimingAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"

class LArBadChanTool;
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

  ToolHandle< LArBadChanTool > m_BadChanTool;
  const CaloCell_ID* m_cellID;
  const DataHandle<LArOnlineID> m_onlineID;
  const DataHandle<LArEM_ID>    m_emID;
  std::vector<unsigned int> m_hwarray;
  bool m_reallyCheck;

  void fillChannelArray();

  void timeOfflineAccess();
  void timeOnlineAccess();
  void timeFEBAccess();

  int testsPerEvent() const {return 1000000;}
};

#endif
