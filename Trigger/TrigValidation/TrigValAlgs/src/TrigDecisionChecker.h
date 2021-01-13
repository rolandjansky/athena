/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** R.Goncalo - 21/10/2007 - add tests for TrigDecisionTool: 
TrigDecisionChecker based on TrigDecisionMaker/TrigDecisionTest */

#ifndef TrigDecisionChecker_TrigDecisionChecker_H
#define TrigDecisionChecker_TrigDecisionChecker_H

// std stuff
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

// core stuff
#include "GaudiKernel/ToolHandle.h"

// Trigger includes
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

// fwd declare muon printing tool
namespace Rec {
  class IMuonPrintingTool;
}

/**
 * @class TrigDecisionChecker
 * bSimple TopAlg to read & check TrigDecision data
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 * @author Roger Moore     <rwmoore@ualberta.ca>     - University of Alberta, Canada
 * @author Mark Owen <markowen@cern.ch>
 */
class TrigDecisionChecker : public AthAlgorithm
{
  
 public:
  
  TrigDecisionChecker(const std::string &name, ISvcLocator *pSvcLocator); //!< std Gaudi Algorithm constructor
  ~TrigDecisionChecker();//!< std  deconstructor
  
  // IAlgorithm virtual methods to implement
  StatusCode initialize();//!< std Gaudi initialize method
  StatusCode execute(); //!< std Gaudi execute method -> use TrigDecisionTool to 'look' at the trigger data
  StatusCode finalize();//!< std Gaudi finalize method
  
 private:
  template <class T> StatusCode checkEDM(const std::string& trigItem);
  StatusCode checkTauEDM(const std::string& trigItem);

  StatusCode checkMuonEDM(const std::string& trigItem);

  StatusCode checkBphysEDM(const std::string& trigItem); //!< Bphysics EDM checker

  StatusCode checkBjetEDM(const std::string& trigItem);
  
  StatusCode checkElectronEDM(const std::string& trigItem);

  StatusCode checkPhotonEDM(const std::string& trigItem);

  StatusCode checkMinBiasEDM(const std::string& trigItem);
  void checkTrigSpacePointCounts(const Trig::FeatureContainer& fc);
  void checkTrigT2MBTSBits(const Trig::FeatureContainer& fc);
  void checkTrigVertexCounts(const Trig::FeatureContainer& fc);
  void checkTrigTrackCounts(const Trig::FeatureContainer& fc);

  StatusCode checkJetEDM(const std::string& trigItem);

  StatusCode checkMetEDM(const std::string& trigItem);

  Gaudi::Property<uint32_t> m_smKey{ this, "SMK", 0, "The super master key to use" }; //!< Super Master Key number to select (0=ignore)
  Gaudi::Property<bool> m_eventDecisionPrintout{this, "WriteEventDecision", true};

  // write out trigger counts
  Gaudi::Property<std::string>   m_printoutFileName{ this, "WriteOutFilename", "", "when set the stat is saved in this file"};

  
  
  
    

  // useful for bookkeeping:
  std::map<std::string,int> m_L1Summary;
  std::vector<std::string> m_summary;
  std::vector<int> m_summaryPass;
  std::vector<int> m_summaryPassRaw;
  std::vector<int> m_summaryPassPhys;
  std::vector<int> m_summaryChainPass;
  std::vector<int> m_summaryChainPassRaw;
  std::vector<int> m_summaryChainPassPhys;
  std::vector<int> m_summaryChainPT;
  std::vector<int> m_summaryChainPS;
  bool m_first_event;

  // tests: check efficiency for a few signatures in blocks on 100 events
  Gaudi::Property<int> m_monitoringBlockSize{ this, "MonitoringBlock",  100};

  std::vector<int> m_countEvent;
  std::vector<int> m_runCountSigs;
  std::vector<std::vector<int>* > m_countSigs;
  std::vector<std::string> m_countSigNames;
  int   m_eventNumber = 0;
  float m_muSum = 0; 

  ToolHandle<Trig::TrigDecisionTool> m_trigDec{this, "TrigDecisionTool", "Trig::TrigDecisionTool/TrigDecisionTool"}; //!< interface to use the trigger data: TriggerTool
  /// The trigger configuration service to get the information from
  ServiceHandle< TrigConf::ITrigConfigSvc > m_configSvc{ this, "TrigConfigSvc", "TrigConf::TrigConfigSvc/TrigConfigSvc"} ;

  /// Muon triggers to test output for
  Gaudi::Property<std::vector<std::string>> m_muonItems{ this, "MuonItems", {} };

  // Bjet triggers to test output for
  Gaudi::Property<std::vector<std::string>> m_bjetItems{ this, "BjetItems", {} };

  Gaudi::Property<std::vector<std::string>> m_bphysItems{ this, "BphysicsItems", {} }; //! Bphysics chains to test
  
    // Electron triggers to test output for
  Gaudi::Property<std::vector<std::string>> m_electronItems{ this, "ElectronItems", {} };
  
  // Photon triggers to test output for
  Gaudi::Property<std::vector<std::string>> m_photonItems{ this, "PhotonItems", {} };

  // Tau triggers to test output for 
  Gaudi::Property<std::vector<std::string>> m_TauItems{ this, "TauItems", {} };

  // MinBias triggers to test output for
  Gaudi::Property<std::vector<std::string>> m_minBiasItems{ this, "MinBiasItems", {} };
    
  // Jet triggers to test output for
  Gaudi::Property<std::vector<std::string>> m_jetItems{ this, "JetItems", {} };

  // Met triggers to test output for
  Gaudi::Property<std::vector<std::string>> m_metItems{ this, "MetItems", {} };
    
  // ...check prescale and passthrough factors 
  std::vector<float> m_chainPrescales;
  std::vector<float> m_chainPrescalesCalculated;
  std::vector<float> m_chain_passthrough;
  std::vector<float> m_chain_passthrough_calculated;
  std::vector<int> m_lower_chain_accept;
  std::map<std::string,std::string> m_lowerChainMap;

  // needed for mu value
  std::string m_eventInfoName;

  // Check pass bits
  Gaudi::Property<bool> m_checkBits{ this, "CheckTrigPassBits", false, "TrigPassBits retrieval from TDT"};

};

#endif
