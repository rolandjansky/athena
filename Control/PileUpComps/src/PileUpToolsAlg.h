/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_PILEUPTOOLSALG_H
#define PILEUPCOMPS_PILEUPTOOLSALG_H
/** @file PileUpToolsAlg.h
 * @brief an algorithm to steer the IPileUpTools
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventInfo/PileUpEventInfo.h" /* SubEvent*/

class IPileUpTool;

/////////////////////////////////////////////////////////////////////////////

class PileUpToolsAlg: public AthAlgorithm {
public:
  PileUpToolsAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  StatusCode clearXing(PileUpEventInfo::SubEvent::const_iterator& fEvt,
                       PileUpEventInfo::SubEvent::const_iterator& lEvt);
  ToolHandleArray<IPileUpTool> m_puTools;
};


#endif
