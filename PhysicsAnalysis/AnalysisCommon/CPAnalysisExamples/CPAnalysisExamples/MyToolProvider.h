/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CPANALYSISEXAMPLES_MYTOOLPROVIDER
#define CPANALYSISEXAMPLES_MYTOOLPROVIDER

#include "AsgTools/AsgTool.h"       
#include "AsgTools/IAsgTool.h"
#include "AsgTools/AsgMessaging.h"
///////////////////////// -*- C++ -*- /////////////////////////////  

// Tool interfaces                              

#include "AsgTools/ToolHandle.h"
                    
#include "JetCalibTools/JetCalibrationTool.h"  
#include "JetSelectorTools/JetCleaningTool.h"
#include "JetUncertainties/JetUncertaintiesTool.h"  
#include "MuonAnalysisInterfaces/MuonCalibrationAndSmearingTool.h"          
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"           
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"       
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h" 
#include "TauAnalysisTools/TauSelectionTool.h"                          
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"   
#include "TauAnalysisTools/TauSmearingTool.h"               
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"       
#include "METUtilities/METRebuilder.h"

// Simple class used to initialize CP tools for usage in more than one ROOT examples


class MyToolProvider : public asg::AsgMessaging
{
 public:

  // class methods

  MyToolProvider(const std::string& name);
  ~MyToolProvider();

  StatusCode initialize(bool isData = false, bool isAtlfast = true);

  // Jet tools 

  JetCalibrationTool * jetCalibTool;
  JetUncertaintiesTool * jetUncertaintiesTool;
  JetCleaningTool * jetCleaningTool;

  // muon tools 

  CP::MuonCalibrationAndSmearingTool *muonCalibrationAndSmearingTool;
  CP::MuonEfficiencyScaleFactors *muonEfficiencySFTool;
  CP::MuonSelectionTool *muonSelectionTool;

  // electron tools

  AsgElectronEfficiencyCorrectionTool *elecEfficiencySFTool;
  CP::EgammaCalibrationAndSmearingTool *egammaCalibTool;

  // tau tools 

  TauAnalysisTools::TauSelectionTool *tauSelTool;
  TauAnalysisTools::TauSmearingTool *tauSmearingTool;
  TauAnalysisTools::TauEfficiencyCorrectionsTool *tauEffTool;

  // b-tagging tools 

  BTaggingEfficiencyTool *btagTool;

  // MET tools 

  met::METRebuilder *metRebuilder;

};

#endif // ifndef CPANALYSISEXAMPLES_MYTOOLPROVIDER
