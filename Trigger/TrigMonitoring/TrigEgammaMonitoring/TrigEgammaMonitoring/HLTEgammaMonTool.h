/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTEGAMMATOOL_H
#define HLTEGAMMATOOL_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "TrigCaloEvent/TrigEMCluster.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"

// Simple description to come
#include "TH1F.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigEgammaMonitoring/HLTEgammaFEXBaseTool.h"

class TrigInDetTrackCollection;

namespace Rec {
  class TrackParticle;
}

class IExtrapolateToCaloTool;
class CaloDepthTool;

class HLTEgammaMonTool : public HLTEgammaFEXBaseTool {

  public:
  
  // Constructor
  HLTEgammaMonTool (const std::string & type, const std::string & name, const IInterface* parent);
  virtual ~HLTEgammaMonTool();

  StatusCode init();
  StatusCode book(); // called by bookHistograms
  StatusCode fill(); // called by fillHistograms
  StatusCode proc(); // called by procHistograms

  private:
  unsigned int m_lastTimeStamp; // last timestamp
  unsigned int m_firstTimeStamp; // timestamp of the first event
};

#endif

