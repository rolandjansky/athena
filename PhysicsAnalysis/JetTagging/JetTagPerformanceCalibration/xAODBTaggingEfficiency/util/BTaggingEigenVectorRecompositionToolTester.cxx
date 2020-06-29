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
  // Exclude certain original uncertainties from Eigenvector scheme so that
  // it these uncertainties will be exclude from eigen vector recomposition. 
  // The original uncertainty names are separated by semicolon.
  // Here exclude two uncertainties as an example.
  // StatusCode code0 = btag_eff_tool.setProperty("ExcludeFromEigenVectorBTreatment","FT_EFF_PDF4LHC_np_19;JET_EffectiveNP_Mixed3");
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
  StatusCode code8 = evr_tool.initialize();
  if (code7 != StatusCode::SUCCESS
      ||code8 != StatusCode::SUCCESS ) {
    std::cout << "Initialization of tool" << evr_tool->name() << " failed!" << std::endl;
  } else {
    std::cout << "Initialization of tool " << evr_tool->name() << " finished." << std::endl;
  }
  
  const std::string label = "B";
  const unsigned int evIdx = 0;

  /**
     getCoefficientMap(label, EigenIdxList)
     
     input value:
     1. label: falvor label in std::string format, could be one of B, C, T, Light
     2. EigenIdxList is user defined vector containing all eigenvector index
     that user interested in.
     output:
     Map of format map<string, map<string, float>> containing decomposition
     coefficient of the list of eigenvectors defined by EigenIdxList.
  */
  std::vector<unsigned int> eigenIdxList = {1,2,3,4,5};
  std::map<std::string, std::map<std::string, float>> coefficientMap = evr_tool->getCoefficientMap(label, eigenIdxList);

  /**
     getCoefficients(label, evIdx)
     
     input value:
     1. label: falvor label in std::string format, could be one of B, C, T, Light
     2. evIdx: The index of eigenvector user interested in.
     output value:
     vector of coefficient values. The order is the same as output given by
     getListOfOriginalNuisanceParameters()
   */
  std::vector<float> coeffs = evr_tool->getCoefficients(label, evIdx);
  
  /**
     getListOfOriginalNuisanceParameters(label)
     
     input value:
     1. label: falvor label in std::string format, could be one of B, C, T, Light
     output value:
     List of original nuisance parameter names.
   */
  std::vector<std::string> orig_nps = evr_tool->getListOfOriginalNuisanceParameters(label);

  /**
     getNumEigenVectors(label)
     
     input value:
     1. label: falvor label in std::string format, could be one of B, C, T, Light
     return value:
     number of eigen vectors used for chosen label. Return 0 if error occured. 
   */
  int nEigen = evr_tool->getNumEigenVectors("B");
  if (nEigen >= 0){
    std::cout<<"There are "<<nEigen
	     <<" eigen vectors for flavour B."<<std::endl;
  }

  return retval;

}
