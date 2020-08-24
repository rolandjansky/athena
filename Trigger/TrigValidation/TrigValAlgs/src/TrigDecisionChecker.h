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

  uint32_t m_smk; ///< Super Master Key
  uint32_t m_l1psk; ///< LVL1 Prescale Key
  uint32_t m_hltpsk; ///< HLT Prescale Key
  uint32_t m_smKey; //!< Super Master Key number to select (0=ignore)
  std::string m_trigDecisionKey; //!< SG key of the trigger data (TrigDecision object)
  bool m_event_decision_printout;
  // write out trigger counts
  bool          m_printout_counts;
  std::string   m_printout_filename;
  std::ofstream m_printout_file;

  
  
  
    

  // useful for bookkeeping:
  std::map<std::string,int> m_L1_summary;
  std::vector<std::string> m_summary;
  std::vector<int> m_summary_pass;
  std::vector<int> m_summary_passraw;
  std::vector<int> m_summary_passphys;
  std::vector<int> m_summary_chain_pass;
  std::vector<int> m_summary_chain_passraw;
  std::vector<int> m_summary_chain_passphys;
  std::vector<int> m_summary_chain_PT;
  std::vector<int> m_summary_chain_PS;
  bool m_first_event;

  // tests: check efficiency for a few signatures in blocks on 100 events
  int m_monitoring_block_size;
  std::vector<int> m_count_event;
  std::vector<int> m_run_count_sigs;
  std::vector<std::vector<int>* > m_count_sigs;
  std::vector<std::string> m_count_sig_names;
  int   m_event_number;
  float m_mu_sum; 

  ToolHandle<Trig::TrigDecisionTool> m_trigDec; //!< interface to use the trigger data: TriggerTool
  /// The trigger configuration service to get the information from
  ServiceHandle< TrigConf::ITrigConfigSvc > m_configSvc;

  /// Muon triggers to test output for
  std::vector<std::string> m_muonItems;

  // Bjet triggers to test output for
  std::vector<std::string> m_bjetItems;

    std::vector<std::string> m_bphysItems; //! Bphysics chains to test
  
    // Electron triggers to test output for
  std::vector<std::string> m_electronItems;
  
  // Photon triggers to test output for
  std::vector<std::string> m_photonItems;

  // Tau triggers to test output for 
  std::vector<std::string> m_TauItems;

  // MinBias triggers to test output for
  std::vector<std::string> m_minBiasItems;
    
  // Jet triggers to test output for
  std::vector<std::string> m_jetItems;

  // Met triggers to test output for
  std::vector<std::string> m_metItems;
    
  // ...check prescale and passthrough factors 
  std::vector<float> m_chain_prescales;
  std::vector<float> m_chain_prescales_calculated;
  std::vector<float> m_chain_passthrough;
  std::vector<float> m_chain_passthrough_calculated;
  std::vector<int> m_lower_chain_accept;
  std::map<std::string,std::string> m_lower_chain_map;

  // needed for mu value
  std::string m_eventInfoName;

  // Check pass bits
  bool m_checkBits;


  // Tool to dump info about xAOD muons
  ToolHandle<Rec::IMuonPrintingTool>            m_muonPrinter;
};

#endif
