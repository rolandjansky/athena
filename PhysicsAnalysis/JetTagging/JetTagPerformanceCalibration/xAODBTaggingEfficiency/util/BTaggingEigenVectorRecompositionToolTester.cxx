/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include <AsgTools/AnaToolHandle.h>
#include <AsgTools/ToolHandle.h>
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "FTagAnalysisInterfaces/IBTaggingEigenVectorRecompositionTool.h"


int main() {
  bool retval = true;

  std::string taggerName = "MV2c10";
  std::string workingPointName = "Continuous";
  
  asg::AnaToolHandle<IBTaggingEfficiencyTool> btag_eff_tool("BTaggingEfficiencyTool/BTagEffTest");
  StatusCode code1 = btag_eff_tool.setProperty("ScaleFactorFileName","xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2019-07-30_v1.root" );
  StatusCode code2 = btag_eff_tool.setProperty("TaggerName",    taggerName  );
  StatusCode code3 = btag_eff_tool.setProperty("OperatingPoint", workingPointName);
  StatusCode code4 = btag_eff_tool.setProperty("JetAuthor", "AntiKt4EMTopoJets" );
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
  std::vector<int> eigenIdxList = {1,2,3,4,5};
  auto coefficientMap = evr_tool->getCoefficientMap("B", eigenIdxList);
  auto coeffs = evr_tool->getCoefficients("B", 2);
  auto orig_nps = evr_tool->getListOfOriginalNuisanceParameters("B");

  //int cnt = 0 ;
  /*
  for (std::map<std::string, std::map<std::string, double>>::iterator out=coefficientMap.begin();
       out!=coefficientMap.end(); ++out){
    cnt++;
    std::cout<<"cnt: "<<cnt<<std::endl;
    std::cout<<"____________________________________________"<<std::endl;
    std::cout<<out->first<<std::endl;
    for (std::map<std::string, double>::iterator in=out->second.begin();
	 in!=out->second.end(); ++in){
      std::cout<<in->first<<" "<<in->second<<std::endl;
    }
  }
  */

  //for(auto s:orig_nps){
  //std::cout<<s<<std::endl;
  //}

  for(double val:coeffs){
    std::cout<<val<<std::endl;
  }

  return retval;

}
