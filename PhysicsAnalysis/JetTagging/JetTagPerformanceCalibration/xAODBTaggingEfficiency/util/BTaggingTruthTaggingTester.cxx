#include "AsgTools/AsgTool.h"
#include "AsgTools/MessageCheck.h"
#include "AsgTools/MsgStream.h"
#include <AsgTools/AnaToolHandle.h>
#include "FTagAnalysisInterfaces/IBTaggingTruthTaggingTool.h"

#include <string>
#include <iomanip>
#include <vector>

int main() {

  asg::AnaToolHandle<IBTaggingTruthTaggingTool> tool("BTaggingTruthTaggingTool/BtagTT_Tool");


   //choose working point and CDI file
   StatusCode code = tool.setProperty("TaggerName", "MV2c10");
   StatusCode code2 = tool.setProperty("OperatingPoint", "FixedCutBEff_77");
   StatusCode code3 = tool.setProperty("JetAuthor", "AntiKt4EMTopoJets");
   StatusCode code4 = tool.setProperty("ScaleFactorFileName", "xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2018-02-09_v1.root");


   //truth tagging settings
   StatusCode code5 = tool.setProperty("IgnoreScaleFactors", false); //use the data/MC and MC/MC scale factors when computing event weights
   StatusCode code6 = tool.setProperty("UseSystematics", true); // store event weights for all systematic variations in the results object
   StatusCode code7 = tool.setProperty("MaxNtagged", 3);
   StatusCode code8 = tool.setProperty("UsePermutations", true);
   StatusCode code9 = tool.setProperty("UseQuantile", true);

   StatusCode code10 = tool.initialize();

  if (code != StatusCode::SUCCESS || code2 != StatusCode::SUCCESS || code3 != StatusCode::SUCCESS || code4 != StatusCode::SUCCESS || code5 != StatusCode::SUCCESS ||
    code6 != StatusCode::SUCCESS || code7 != StatusCode::SUCCESS || code8 != StatusCode::SUCCESS ||  code9 != StatusCode::SUCCESS || code10 != StatusCode::SUCCESS ) {
    std::cout << "Initialization of tool " << tool->name() << " failed! " << std::endl;
    return -1;
  }
  else {
    std::cout << "Initialization of tool " << tool->name() << " finished." << std::endl;
  }

  //Set the jets to be used in the tool.
  std::vector<float> pt = {44000., 66000., 77000.};
  std::vector<float> eta = {2.2, 1.6, 1.7};
  std::vector<int> flav = {0,4,5};
  std::vector<float> tagw = {0.3, 0.55, 0.99};

  Analysis::TruthTagResults results;

  if(StatusCode::SUCCESS!=tool->CalculateResults( pt,eta,flav,tagw, results) ){
    std::cout << "failed to compute truth tagging results! " << std::endl;
    return -1;
  };

  //direct tagged results:
  std::cout << "Direct tagged results:  "<< std::endl;
  for(unsigned int i=0; i< results.is_tagged.size();i++)
  {
    std::cout << "jet "<< i << " is direct tagged: " << results.is_tagged.at(i) << std::endl;
  }
  //print the results for each of the systematic variations

  for(unsigned int systindex=0; systindex< results.syst_names.size(); systindex++){
      std::string syst_name = results.syst_names.at(systindex);

      std::cout << "direct tag Event SF ( "<< syst_name  <<" ) = " <<results.getEvtDirectTagSF(syst_name) << std::endl;

  }


  //truth tagging results:
  for(int ntags=1; ntags<=3;ntags++){

    std::cout << " for " << ntags << " tagged jets: " << std::endl;
    std::cout << " --------------------------------- " << std::endl;
    //print chosen premutation, tagweight bin assinment and event weight

    std::vector<bool> exclusive_permuation = results.getEventPermutation(ntags,true);
    std::vector<int> exclusive_tagweightBin = results.getEventQuantiles(ntags,true);

    std::vector<bool> inclusive_permuation = results.getEventPermutation(ntags,false);
    std::vector<int> inclusive_tagweightBin = results.getEventQuantiles(ntags,false);

    std::cout << "        exclusive    ||      inclusive" << std::endl;
    std::cout << "      tagged | bin   ||    tagged | bin" << std::endl;
    for(unsigned int i=0; i< exclusive_permuation.size();i++)
      {
        std::cout << "jet "<< i << "       " << exclusive_permuation.at(i) << " " << exclusive_tagweightBin.at(i)
         << "      ||          " <<  inclusive_permuation.at(i) << " " << inclusive_tagweightBin.at(i) << std::endl;
      }

    std::cout << " Nominal event weight exclusive: " << results.getEventWeight(ntags,true,"Nominal") << std::endl;
    std::cout << " Nominal event weight inclusive: " << results.getEventWeight(ntags,false,"Nominal") << std::endl;
    std::cout << " ---------------------------------\n\n " << std::endl;

  }

  return 0;
}
