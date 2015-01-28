/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMMISSIONREC_COMTIMEREC
#define COMMISSIONREC_COMTIMEREC

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CommissionUtils/CosmicTriggerTimeTool.h"

class ComTimeRec : public AthAlgorithm
{

public:

  ComTimeRec(const std::string& name, ISvcLocator* pSvcLocator);
  ~ComTimeRec();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  std::string m_comTimeKey;
  std::string m_mode;
  std::string m_method;
  std::string m_TTRKey;
  double m_samplingPeriod;
  double m_velocity;
  CosmicTriggerTimeTool* m_triggerTimeTool;

};

#endif
