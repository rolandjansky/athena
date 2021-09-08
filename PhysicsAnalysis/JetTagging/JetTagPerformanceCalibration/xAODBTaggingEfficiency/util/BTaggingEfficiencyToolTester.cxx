/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//#ifdef XAOD_STANDALONE
#   include "xAODRootAccess/TStore.h"
//#endif // XAOD_STANDALONE

#include <AsgTools/AnaToolHandle.h>
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"

#include <string>
#include <iomanip>

using CP::CorrectionCode;
using CP::SystematicCode;

int main() {
  bool retval = true;
  xAOD::TStore store;
  

  std::string taggerName = "DL1";
  std::string workingPointName = "FixedCutBEff_70";

  asg::AnaToolHandle<IBTaggingEfficiencyTool> tool("BTaggingEfficiencyTool/BTagEffTest");
  StatusCode code1 = tool.setProperty("ScaleFactorFileName","xAODBTaggingEfficiency/13TeV/2019-21-13TeV-MC16-CDI-2019-10-07_v1.root" );
  StatusCode code2 = tool.setProperty("TaggerName",    taggerName  );
  StatusCode code3 = tool.setProperty("OperatingPoint", workingPointName);
  StatusCode code4 = tool.setProperty("JetAuthor",      "AntiKt4EMPFlowJets_BTagging201810" );
  
  //provide a file which tells the tool which efficiency maps to use, and connects sample DSIDs to the right efficiency map.
  //you can find a tool to automatically create this config file here: https://gitlab.cern.ch/mstamenk/automate-hadronisation-information
  StatusCode code5 = tool.setProperty("EfficiencyConfig","/afs/cern.ch/work/j/jshlomi/public/example_eff_config.txt");
  
  // specify an aggressive ("Tight") or not-so-aggressive ("Medium") eigenvector suppression scheme
  // tool.setProperty("EigenvectorReductionB", "Medium");
  // tool.setProperty("EigenvectorReductionC", "Medium");
  // tool.setProperty("EigenvectorReductionLight", "Medium");
  
  // uncomment this if the "Envelope" systematics model is to be used instead of the eigenvector variations
  // code = tool.setProperty("SystematicsStrategy", "Envelope");

  StatusCode code6 = tool.initialize();

  if (code1 != StatusCode::SUCCESS || code2 != StatusCode::SUCCESS || code3 != StatusCode::SUCCESS || code4 != StatusCode::SUCCESS
    || code5 != StatusCode::SUCCESS
    || code6 != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool " << tool->name() << " failed! " << std::endl;
    return -1;
  }
  else {
    std::cout << "Initialization of tool " << tool->name() << " finished." << std::endl;
  }


  // select your efficiency map based on the DSID of your sample:
  unsigned int sample_dsid = 410470;

  tool->setMapIndex(sample_dsid);


  std::cout << "-----------------------------------------------------" << std::endl;
  const std::map<CP::SystematicVariation, std::vector<std::string> > allowed_variations = tool->listSystematics();
  std::cout << "Allowed systematics variations for tool " << tool->name() << ":" << std::endl;
  for (auto var : allowed_variations) {
    std::cout << std::setw(40) << std::left << var.first.name() << ":";
    for (auto flv : var.second) std::cout << " " << flv;
    std::cout << std::endl;
  }
  std::cout << "-----------------------------------------------------" << std::endl;
  


  std::cout << "Creating a jet" << std::endl;
  xAOD::JetFourMom_t p4(50000.,0.7,0.3,1000.);

  xAOD::Jet * jet = new xAOD::Jet();
  jet->makePrivateStore();
  std::cout << "Setting jet 4 momentum" << std::endl;
  jet->setJetP4(p4);
  std::cout << "Setting jet attribute" << std::endl;
  // b jet (label==5)
  jet->setAttribute("HadronConeExclTruthLabelID", 5);
  float sf=0;
  float eff=0;
  CorrectionCode result;
  std::cout << "Testing function calls without systematics..." << std::endl;
  result = tool->getEfficiency(*jet,eff);
  if( result!=CorrectionCode::Ok) { std::cout << "b jet get efficiency failed"<<std::endl; retval=false;}
  else {
    std::cout << "b jet get efficiency succeeded: " << eff << std::endl;
  }
  result = tool->getScaleFactor(*jet,sf);
  if( result!=CorrectionCode::Ok) { std::cout << "b jet get scale factor failed"<<std::endl; retval=false;}
  else {
    std::cout << "b jet get scale factor succeeded: " << sf << std::endl;
  }

  std::cout << "Testing function calls with systematics..." << std::endl;
  CP::SystematicSet systs = tool->affectingSystematics();
  for( CP::SystematicSet::const_iterator iter = systs.begin();
       iter!=systs.end(); ++iter) {
    CP::SystematicVariation var = *iter;
    CP::SystematicSet set;
    set.insert(var);
    SystematicCode sresult = tool->applySystematicVariation(set);
    if( sresult !=SystematicCode::Ok) {
      std::cout << var.name() << " apply systematic variation FAILED " << std::endl;
  }
    result = tool->getScaleFactor(*jet,sf);
    if( result!=CorrectionCode::Ok) {
      std::cout << var.name() << " getScaleFactor FAILED" << std::endl;
    } else {
      std::cout << var.name() << " " << sf << std::endl;
    }
  }
  // don't forget to switch back off the systematics...
  CP::SystematicSet defaultSet;
  SystematicCode dummyResult = tool->applySystematicVariation(defaultSet);
  if (dummyResult != SystematicCode::Ok)
    std::cout << "problem disabling systematics setting!" << std::endl;



  return retval;
}
