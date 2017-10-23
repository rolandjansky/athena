/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include <string>
#include <iomanip>

using CP::CorrectionCode;

int main() {


  std::string taggerName = "DL1";
  std::string workingPointName = "HybBEff_77";



  BTaggingSelectionTool * tool = new BTaggingSelectionTool("BTagSelecTest");
  StatusCode code1 = tool->setProperty( "FlvTagCutDefinitionsFileName","xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2017-07-02_v1.root" );
  StatusCode code2 = tool->setProperty("TaggerName",    taggerName  );
  StatusCode code3 = tool->setProperty("OperatingPoint", workingPointName);
  StatusCode code4 = tool->setProperty("JetAuthor",      "AntiKt4EMTopoJets" );

  // A successful initialisation ought to be checked for
  StatusCode code5 = tool->initialize();

  if (code1 != StatusCode::SUCCESS || code2 != StatusCode::SUCCESS || code3 != StatusCode::SUCCESS || code4 != StatusCode::SUCCESS || code5 != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool " << tool->name() << " failed! " << std::endl;
    return -1;
  }
  else {
    std::cout << "Initialization of tool " << tool->name() << " finished." << std::endl;
  }


  //load some jets to show how to use the tool

  xAOD::TEvent event;

  TFile* m_file = TFile::Open("/afs/cern.ch/work/j/jshlomi/public/DAOD_FTAG2.12165764._000400.pool.root.1","read");

  if(!event.readFrom(m_file).isSuccess()){ std::cout << "failed to load file" << std::endl; return -1; }

  event.getEntry(0);

  const xAOD::JetContainer* jets = 0;

  if (!event.retrieve( jets, "AntiKt4EMTopoJets" ).isSuccess() ){ std::cout << " error retrieving jets " << std::endl; return -1;}

  int jet_index = 0;
  for (auto jet : *jets) {

    //getting a tagging decision, is the jet tagged or not
    bool tagged = tool->accept(*jet);

    //you can get the tagger weight,
    double tagweight;
    if( tool->getTaggerWeight( *jet ,tagweight)!=CorrectionCode::Ok ){ std::cout << " error retrieving tagger weight " << std::endl; return -1; }

    std::cout << "jet " << jet_index << " " <<  taggerName  << "  " << workingPointName << " is tagged " << tagged << " tag weight " << tagweight << std::endl;


    //if you have DL1 weights, you can get the tagger weight this way
    const xAOD::BTagging *btag = jet->btagging();

    double dl1_pb = btag->auxdata<double>("DL1_pb");
    double dl1_pc = btag->auxdata<double>("DL1_pc");
    double dl1_pu = btag->auxdata<double>("DL1_pu");


    if(  tool->getTaggerWeight(dl1_pb,dl1_pc,dl1_pu, tagweight) !=CorrectionCode::Ok ){ std::cout << " error retrieving tagger weight " << std::endl; return -1; }

    std::cout << " tagweight " << tagweight << std::endl;

    //you can also extract the cut value (which may or may not depend on the jet pt)

    double cutval;
     //provide the pt in MeV
    if( tool->getCutValue( jet->pt() , cutval)!=CorrectionCode::Ok ){ std::cout << " error retrieving cut value " << std::endl; return -1; }

    std::cout << " cut value " << cutval << std::endl;


    jet_index++;

  }


  return 0;
}
