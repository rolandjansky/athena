/* -*- C++ -*- */

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_PILEUPTOOLSALG_H
#define PILEUPCOMPS_PILEUPTOOLSALG_H
/** @file PileUpToolsAlg.h
 * @brief an algorithm to steer the IPileUpTools
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include "AthenaBaseComps/AthAlgorithm.h"

#include "PileUpTools/IPileUpTool.h"
#include "xAODEventInfo/EventInfo.h" /* SubEvent*/

#include "GaudiKernel/ToolHandle.h"



/////////////////////////////////////////////////////////////////////////////

class PileUpToolsAlg : public AthAlgorithm
{
public:
  PileUpToolsAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  StatusCode clearXing(SubEventIterator& fEvt,
                       const SubEventIterator& lEvt);
  ToolHandleArray<IPileUpTool> m_puTools{this,"PileUpTools",{},"IPileUpTools to be run for each event"};

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{ this, "EventInfoKey", "EventInfo", "ReadHandleKey for xAOD::EventInfo" };
};


#endif
