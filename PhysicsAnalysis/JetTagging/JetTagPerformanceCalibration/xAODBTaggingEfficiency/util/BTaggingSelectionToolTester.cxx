/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_STANDALONE
#include "xAODRootAccess/TEvent.h"
#else
#include "POOLRootAccess/TEvent.h"
#endif
#include <AsgTools/StandaloneToolHandle.h>
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"

#include "PATInterfaces/CorrectionCode.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingUtilities.h"



#include <string>
#include <iomanip>
#include "TFile.h"

using CP::CorrectionCode;

int main() {


  std::string taggerName = "DL1";
  std::string workingPointName = "HybBEff_77";


  asg::StandaloneToolHandle<IBTaggingSelectionTool> tool("BTaggingSelectionTool/BTagSelecTest");
  StatusCode code1 = tool.setProperty( "FlvTagCutDefinitionsFileName","xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2018-02-09_v1.root" );
  StatusCode code2 = tool.setProperty("TaggerName",    taggerName  );
  StatusCode code3 = tool.setProperty("OperatingPoint", workingPointName);
  StatusCode code4 = tool.setProperty("JetAuthor",      "AntiKt4EMTopoJets" );
  // A successful initialisation ought to be checked for
  StatusCode code5 = tool.initialize();

  if (code1 != StatusCode::SUCCESS || code2 != StatusCode::SUCCESS || code3 != StatusCode::SUCCESS || code4 != StatusCode::SUCCESS || code5 != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool " << tool->name() << " failed! " << std::endl;
    return -1;
  }
  else {
    std::cout << "Initialization of tool " << tool->name() << " finished." << std::endl;
  }


  //load some jets to show how to use the tool

  //  xAOD::TEvent event;
  #ifdef XAOD_STANDALONE
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  #else
  POOL::TEvent event(POOL::TEvent::kClassAccess);
  #endif


  TFile* m_file = TFile::Open("/afs/cern.ch/work/j/jshlomi/public/DAOD_FTAG2.root","read");

  if(!event.readFrom(m_file).isSuccess()){ std::cout << "failed to load file" << std::endl; return -1; }

  event.getEntry(0);

  const xAOD::JetContainer* jets = 0;

  if (!event.retrieve( jets, "AntiKt4EMTopoJets" ).isSuccess() ){ std::cout << " error retrieving jets " << std::endl; return -1;}

  int jet_index = 0;
  for (const xAOD::Jet* jet : *jets) {

    //getting a tagging decision, is the jet tagged or not
    bool tagged = static_cast<bool>(tool->accept(*jet));

    //you can get the tagger weight,
    double tagweight;
    if( tool->getTaggerWeight( *jet ,tagweight)!=CorrectionCode::Ok ){ std::cout << " error retrieving tagger weight " << std::endl; return -1; }

    std::cout << "jet " << jet_index << " " <<  taggerName  << "  " << workingPointName << " is tagged " << tagged << " tag weight " << tagweight << std::endl;




    //if you have DL1 weights, you can get the tagger weight this way
    const xAOD::BTagging *btag = xAOD::BTaggingUtilities::getBTagging( *jet );

    double dl1_pb = btag->auxdata<double>("DL1_pb");
    double dl1_pc = btag->auxdata<double>("DL1_pc");
    double dl1_pu = btag->auxdata<double>("DL1_pu");


    if(  tool->getTaggerWeight(dl1_pb,dl1_pc,dl1_pu, tagweight) !=CorrectionCode::Ok ){ std::cout << " error retrieving tagger weight " << std::endl; return -1; }

    std::cout << " tagweight " << tagweight << std::endl;

    double pT = jet->pt();
    double eta = jet->eta();

    // you can see if the jet is tagged using its pt/eta and tagweight

    tagged = static_cast<bool>(tool->accept(pT,eta,tagweight));

    //you can also extract the cut value (which may or may not depend on the jet pt)

    double cutval;
     //provide the pt in MeV
    if( tool->getCutValue( jet->pt() , cutval)!=CorrectionCode::Ok ){ std::cout << " error retrieving cut value " << std::endl; return -1; }

    std::cout << " tagged " << tagged << " cut value " << cutval << std::endl;

    jet_index++;

  }

  //Continuous working points
  //*************************
  //with a selection tool using the Continuous working point,
  //you can get the jets tag weight bin (between the different fixedcutBEff working points, 60,70,77,85)
  taggerName = "DL1";
  workingPointName = "Continuous";
  asg::StandaloneToolHandle<IBTaggingSelectionTool> tool_Continuous("BTaggingSelectionTool/BTagSelContinuousTest");
  code1 = tool_Continuous.setProperty( "FlvTagCutDefinitionsFileName","xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2018-02-09_v1.root" );
  code2 = tool_Continuous.setProperty("TaggerName",    taggerName  );
  code3 = tool_Continuous.setProperty("OperatingPoint", workingPointName );
  code4 = tool_Continuous.setProperty("JetAuthor",      "AntiKt4EMTopoJets" );
  code5 = tool_Continuous.initialize();

  if (code1 != StatusCode::SUCCESS || code2 != StatusCode::SUCCESS || code3 != StatusCode::SUCCESS || code4 != StatusCode::SUCCESS || code5 != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool " << tool_Continuous->name() << " failed! " << std::endl;
    return -1;
  }
  else {
    std::cout << "Initialization of tool " << tool_Continuous->name() << " finished." << std::endl;
  }

  jet_index = 0;
  for (const xAOD::Jet* jet : *jets) {

    double tagweight;
    if( tool->getTaggerWeight( *jet ,tagweight)!=CorrectionCode::Ok ){ 
      std::cout << " error retrieving tagger weight " << std::endl; return -1; 
    }
    int quantile = tool_Continuous->getQuantile(*jet);
    

    std::cout << "jet " << jet_index << " " <<  taggerName  << "  " << workingPointName << " tag weight " << tagweight <<  " quantile " << quantile << std::endl;

    jet_index++;

  }


  //Veto working points
  //**************************
  //by setting the OperatingPoint to a string with the format WP1_Veto_Tagger2_WP2
  //for example, FixedCutBEff_70_Veto_DL1_CTag_Loose
  //the selection tool will require the jet to be
  //tagged by the standard working point
  //and to not be tagged by the secondary tagger and working point

  taggerName = "MV2c10";
  workingPointName = "FixedCutBEff_70_Veto_DL1_CTag_Loose";

  asg::StandaloneToolHandle<IBTaggingSelectionTool> tool_veto("BTaggingSelectionTool/BTagSelecVetoTest");
  code1 = tool_veto.setProperty( "FlvTagCutDefinitionsFileName","xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2018-02-09_v1.root" );
  code2 = tool_veto.setProperty("TaggerName",    taggerName  );
  code3 = tool_veto.setProperty("OperatingPoint", workingPointName );
  code4 = tool_veto.setProperty("JetAuthor",      "AntiKt4EMTopoJets" );
  code5 = tool_veto.initialize();

  if (code1 != StatusCode::SUCCESS || code2 != StatusCode::SUCCESS || code3 != StatusCode::SUCCESS || code4 != StatusCode::SUCCESS || code5 != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool " << tool_veto->name() << " failed! " << std::endl;
    return -1;
  }
  else {
    std::cout << "Initialization of tool " << tool_veto->name() << " finished." << std::endl;
  }


  jet_index = 0;
  for (const xAOD::Jet* jet : *jets) {

    //getting a tagging decision, is the jet tagged or not
    bool tagged = static_cast<bool>(tool_veto->accept(*jet));


    // if you are using a format without xAOD::Jets or where the jet does not have a properly filled b-tagging object,
    // you need the two tagger weights, for the nominal tagger and the veto tagger.

    //you can get the tagger weight,
    double tagweight_nominal;
    double tagweight_veto_tagger;

    if( tool_veto->getTaggerWeight( *jet ,tagweight_nominal)!=CorrectionCode::Ok ){ std::cout << " error retrieving nominal tagger weight " << std::endl; return -1; }
    //use a third argument set to true to getTaggerWeight for the veto tagger weight
    if( tool_veto->getTaggerWeight( *jet ,tagweight_veto_tagger,true)!=CorrectionCode::Ok ){ std::cout << " error retrieving veto tagger weight " << std::endl; return -1; }


    //if you have DL1 weights, you can get the tagger weight this way
    const xAOD::BTagging *btag = xAOD::BTaggingUtilities::getBTagging( *jet );

    double dl1_pb = btag->auxdata<double>("DL1_pb");
    double dl1_pc = btag->auxdata<double>("DL1_pc");
    double dl1_pu = btag->auxdata<double>("DL1_pu");

    //the 5th argument tells it to retrive the veto tagger weight
    if(  tool_veto->getTaggerWeight(dl1_pb,dl1_pc,dl1_pu, tagweight_veto_tagger,true) !=CorrectionCode::Ok ){ std::cout << " error retrieving tagger weight " << std::endl; return -1; }



    double pT = jet->pt();
    double eta = jet->eta();

    bool tagged_withtagweights = static_cast<bool>(tool_veto->accept(pT,eta,tagweight_nominal,tagweight_veto_tagger));


    std::cout << "jet " << jet_index << " " <<  taggerName  << "  " << workingPointName << " is tagged " << tagged << " tagged (with tagweights) "<< tagged_withtagweights << std::endl;

    jet_index++;

  }





  return 0;
}
