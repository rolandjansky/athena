/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYTIMINGS_H
#define OVERLAYCOPYALGS_COPYTIMINGS_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "RecEvent/RecoTimingObj.h"

class CopyTimings : public AthReentrantAlgorithm
{
public:

  CopyTimings(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<RecoTimingObj> m_bkgInputKey{ this, "BkgInputKey", "", "ReadHandleKey for Background EVNTtoHITS Timings" };
  SG::ReadHandleKey<RecoTimingObj> m_signalInputKey{ this, "SignalInputKey", "Sig_EVNTtoHITS_timings", "ReadHandleKey for Signal EVNTtoHITS Timings" };
  SG::WriteHandleKey<RecoTimingObj> m_outputKey{ this, "OutputKey", "EVNTtoHITS_timings", "WriteHandleKey for Output EVNTtoHITS Timings" };

};

#endif // OVERLAYCOPYALGS_COPYTIMINGS_H
