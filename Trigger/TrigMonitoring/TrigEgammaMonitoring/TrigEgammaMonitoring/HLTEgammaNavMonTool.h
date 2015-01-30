/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTEGAMMANAVTOOL_H
#define HLTEGAMMANAVTOOL_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "xAODTrigCalo/TrigEMCluster.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"

// Simple description to come
#include "TH1F.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigEgammaMonitoring/HLTEgammaFEXBaseTool.h"

#include <map>

class TrigInDetTrackCollection;

namespace Rec {
  class TrackParticle;
}

class HLTEgammaNavMonTool : public HLTEgammaFEXBaseTool {

  public:
  
  // Constructor
  HLTEgammaNavMonTool (const std::string & type, const std::string & name, const IInterface* parent);
  virtual ~HLTEgammaNavMonTool();

  StatusCode init();
  /** In this case, the book drives the book_per_signature for each
	of the valid requested signatures */
  StatusCode book(); // called by bookHistograms
  
  StatusCode book_per_signature(const std::string signature); // called by bookHistograms
  /** In this case, the fill drives the fille_per_signature and
	the rate_per_signature for each of the valid requested signatures */
  StatusCode fill(); // called by fillHistograms
  StatusCode fill_per_signature(const std::string signature ); // called by fillHistograms
  StatusCode rate_per_signature(const std::string signature ); // called by fillHistograms
  /** In this case, the proc drives the proc_per_signature for each
	of the valid requested signatures. Only used for the rates */
  StatusCode proc(); // called by procHistograms
  StatusCode proc_per_signature(const std::string signature); // called by procHistograms
  static void trigger_description(const std::string signature, std::string& trigDesc); //called by *_per_signature

  private:
  unsigned int m_lastTimeStamp; // last timestamp
  unsigned int m_firstTimeStamp; // timestamp of the first event
  std::vector<std::string> m_signatures; // configuration for the signatures to be used
  unsigned int m_fill_condition;
  unsigned int m_rate_condition;
  std::map< std::string, std::vector<unsigned int> > m_counters;
  std::string trigDesc;
};

#endif

