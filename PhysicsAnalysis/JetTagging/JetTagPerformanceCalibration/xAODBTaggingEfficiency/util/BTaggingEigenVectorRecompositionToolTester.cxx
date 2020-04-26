/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include <AsgTools/AnaToolHandle.h>
#include <AsgTools/ToolHandle.h>
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "FTagAnalysisInterfaces/IBTaggingEigenVectorRecompositionTool.h"


int main() {
  bool retval = true;

  std::string taggerName = "DL1";
  std::string workingPointName = "FixedCutBEff_60";
  

  asg::AnaToolHandle<IBTaggingEfficiencyTool> btag_eff_tool("BTaggingEfficiencyTool/BTagEffTest");
  StatusCode code1 = btag_eff_tool.setProperty("ScaleFactorFileName","xAODBTaggingEfficiency/13TeV/2020-21-13TeV-MC16-CDI-2020-03-11_v3.root" );
  StatusCode code2 = btag_eff_tool.setProperty("TaggerName",    taggerName  );
  StatusCode code3 = btag_eff_tool.setProperty("OperatingPoint", workingPointName);
  StatusCode code4 = btag_eff_tool.setProperty("JetAuthor", "AntiKt4EMPFlowJets_BTagging201810" );
  StatusCode code5 = btag_eff_tool.setProperty("MinPt", 20. );
  StatusCode code6 = btag_eff_tool.initialize();
  if (code1 != StatusCode::SUCCESS 
      || code2 != StatusCode::SUCCESS 
      || code3 != StatusCode::SUCCESS 
      || code4 != StatusCode::SUCCESS 
      || code5 != StatusCode::SUCCESS 
      || code6 != StatusCode::SUCCESS ){
    std::cout << "Initialization of tool " << btag_eff_tool->name() << " failed! " << std::endl;
    return -1;
  }
  else {
    std::cout << "Initialization of tool " << btag_eff_tool->name() << " finished." << std::endl;
  }


  asg::AnaToolHandle<IBTaggingEigenVectorRecompositionTool> evr_tool("BTaggingEigenVectorRecompositionTool/BTagEVRTest");
  StatusCode code7 = evr_tool.setProperty("BTaggingEfficiencyTool", btag_eff_tool);
  if (code7 != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool" << evr_tool->name() << " failed!" << std::endl;
  } else {
    std::cout << "Initialization of tool " << evr_tool->name() << " finished." << std::endl;
  }
  
  auto correction_code = evr_tool->printListOfCoefficients();
  if (correction_code != CP::CorrectionCode::Ok) {
    std::cout << "getListOfCoefficients failed!" << std::endl;
  }
  auto coeffs = evr_tool->getCoefficients("Eigen_B_0");
  auto orig_nps = evr_tool->getListOfOriginalNuisanceParameters();

  return retval;




}
