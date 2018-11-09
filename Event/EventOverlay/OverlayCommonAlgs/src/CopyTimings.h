/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOMMONALGS_COPYTIMINGS_H
#define OVERLAYCOMMONALGS_COPYTIMINGS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "RecEvent/RecoTimingObj.h"

class CopyTimings : public AthAlgorithm
{
public:

  CopyTimings(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  SG::ReadHandleKey<RecoTimingObj> m_bkgInputKey{ this, "BkgInputKey", "OriginalEvent_SG+EVNTtoHITS_timings", "ReadHandleKey for Background EVNTtoHITS Timings" };
  SG::ReadHandleKey<RecoTimingObj> m_signalInputKey{ this, "SignalInputKey", "BkgEvent_0_SG+EVNTtoHITS_timings", "ReadHandleKey for Signal EVNTtoHITS Timings" };
  SG::WriteHandleKey<RecoTimingObj> m_outputKey{ this, "OutputKey", "StoreGateSvc+EVNTtoHITS_timings", "WriteHandleKey for Output EVNTtoHITS Timings" };

};

#endif // OVERLAYCOMMONALGS_COPYTIMINGS_H
