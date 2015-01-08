/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelDBAlg_H
#define LArBadChannelDBAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"


#include <string>

class LArBadChanTool;
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

  ToolHandle< LArBadChanTool > m_BadChanTool;
  std::string                  m_dbFolder;
  std::string                  m_dbFebFolder;
  int                          m_mode;
};

#endif
