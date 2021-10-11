// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
// GRL
#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"


// Timing
#include "TStopwatch.h"
#include "TEfficiency.h"

// Standard library includes
#include <string>
#include <vector>

// Need truth matching for TauJet CP tools 
namespace TauAnalysisTools {
  class ITauTruthMatchingTool;
}

class SUSYToolsAlg : public EL::AnaAlgorithm {

  public: 

    // Constructor with parameters:
    SUSYToolsAlg(const std::string& name, ISvcLocator* pSvcLocator);

    // Destructor:
    ~SUSYToolsAlg();

    // Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    // Default constructor:
    SUSYToolsAlg();

    // helper functions
    void groupSysts(void);
    StatusCode bookHistograms(void);
    void stdHistsForObj(xAOD::IParticle *obj, std::string objtype, std::string objlevel, std::map<std::string,std::string> config = std::map<std::string,std::string>());

    // configuration and main tools
    int  m_dataSource;
    bool m_usePRWAutoconfig;
    std::vector<std::string> m_PRWConfigs;
    std::vector<std::string> m_PRWLumiCalcFiles;
    std::vector<std::string> m_GRLFiles;
    std::string m_mcCampaign;
    int m_mcChannel;
    std::map<std::string,bool> m_slices;
    std::map<std::string,TH1*> m_hists;
    std::map<std::string,TEfficiency*> m_heffs;
    std::map<std::string,std::string> m_configDict;

    asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool> m_SUSYTools;
    asg::AnaToolHandle<IGoodRunsListSelectionTool> m_grl;
    asg::AnaToolHandle<TauAnalysisTools::ITauTruthMatchingTool> m_tauTruthMatchingTool;

    unsigned int m_Nevts; 
    int m_maxEvts;
    int m_lbfilter;
    std::string m_kernel;
    std::string m_configFile;
    std::string m_FatJetCollection;
    std::string m_TrkJetCollection;
    std::string m_TrkJetTimeStamp;

    // systematics
    bool m_doSyst;
    std::vector<ST::SystInfo> sysInfoList;
    std::map<std::string,std::vector<std::string>> syst_all, syst_weights;

    // timing
    TStopwatch m_clock0;
    TStopwatch m_clock1;
    TStopwatch m_clock2;

    // triggers
    std::map<std::string,std::vector<std::string>> m_triggers;

    // histograms and counts
    std::vector<std::string> m_objects;
    std::vector<std::string> m_levels; 
    std::vector<std::string> m_vars;   
    std::map<std::string,std::map<std::string,int>> m_obj_count;
}; 

#endif
