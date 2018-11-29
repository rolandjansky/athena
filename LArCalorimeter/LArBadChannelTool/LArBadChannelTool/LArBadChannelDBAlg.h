/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelDBAlg_H
#define LArBadChannelDBAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"

#include <string>

class EventInfo;

class LArBadChannelDBAlg : public AthAlgorithm 
{
public:
  LArBadChannelDBAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArBadChannelDBAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey;
  SG::ReadCondHandleKey<LArBadFebCont> m_BFKey;
  std::string                  m_dbFolder;
  std::string                  m_dbFebFolder;
  int                          m_mode;
};

#endif
