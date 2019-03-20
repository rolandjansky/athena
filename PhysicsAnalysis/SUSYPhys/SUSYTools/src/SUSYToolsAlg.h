// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// SUSYToolsAlg.h

#ifndef SUSYToolsAlg_H
#define SUSYToolsAlg_H

// Base class
#include "AnaAlgorithm/AnaAlgorithm.h"

// Tool handles
#include "AsgTools/AnaToolHandle.h"

// For SystInfo needs to be included
#include "SUSYTools/ISUSYObjDef_xAODTool.h"

// Timing
#include "TStopwatch.h"

// Standard library includes
#include <string>
#include <vector>

// Need truth matching for TauJet CP tools 
namespace TauAnalysisTools {
  class ITauTruthMatchingTool;
}

class SUSYToolsAlg : public EL::AnaAlgorithm {

  public: 

    /// Constructor with parameters:
    SUSYToolsAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~SUSYToolsAlg();

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    SUSYToolsAlg();

    int  m_dataSource;
    bool m_usePRWAutoconfig;
    std::vector<std::string> m_PRWConfigs;
    std::vector<std::string> m_PRWLumiCalcFiles;
    std::string m_mcCampaign;
    int m_mcChannel;

    bool m_doSyst;
    std::vector<ST::SystInfo> sysInfoList;

    std::vector<std::string> syst_el_weights;
    std::vector<std::string> syst_mu_weights;
    std::vector<std::string> syst_ph_weights;
    std::vector<std::string> syst_tau_weights;
    std::vector<std::string> syst_jet_weights;
    std::vector<std::string> syst_fatjet_weights;
    std::vector<std::string> syst_btag_weights;
    std::vector<std::string> syst_event_weights;

    asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool> m_SUSYTools;

    unsigned int m_Nevts;
    std::string m_configFile;
    std::string m_FatJetCollection;

    TStopwatch m_clock0;
    TStopwatch m_clock1;
    TStopwatch m_clock2;

    std::string m_rateMonitoringPath;

    std::vector<std::string> el_triggers;
    std::vector<std::string> ph_triggers;
    std::vector<std::string> mu_triggers;

    int count_el_signal;
    int count_ph_signal;
    int count_mu_signal;

    asg::AnaToolHandle< TauAnalysisTools::ITauTruthMatchingTool > m_tauTruthTool;

    bool m_CheckTruthJets;
}; 

#endif
