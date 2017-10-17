/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include <string>
#include <iomanip>

int main() {


  std::string taggerName = "DL1";
  std::string workingPointName = "HybBEff_77";


  BTaggingSelectionTool * tool = new BTaggingSelectionTool("BTagSelecTest");
  tool->setProperty( "FlvTagCutDefinitionsFileName","xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2017-07-02_v1.root" );
  tool->setProperty("TaggerName",    taggerName  );
  tool->setProperty("OperatingPoint", workingPointName);
  tool->setProperty("JetAuthor",      "AntiKt4EMTopoJets" );

  // A successful initialisation ought to be checked for
  StatusCode code = tool->initialize();

  if (code != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool " << tool->name() << " failed! Subsequent results may not make sense." << std::endl;
  }
  else {
    std::cout << "Initialization of tool " << tool->name() << " finished." << std::endl;
  }


  //load some jets to show how to use the tool

  xAOD::TEvent* m_event = new xAOD::TEvent();

  TFile* m_file = TFile::Open("/afs/cern.ch/work/j/jshlomi/public/DAOD_FTAG2.12165764._000400.pool.root.1","read");

  if(!m_event->readFrom(m_file).isSuccess()){ std::cout << "failed to load file" << std::endl; }

  m_event->getEntry(0);

  const xAOD::JetContainer* jets = 0;

  if (!m_event->retrieve( jets, "AntiKt4EMTopoJets" ).isSuccess() ){ std::cout << " error retrieving jets " << std::endl; return 0;}

  int jet_index = 0;
  for (auto jet : *jets) {

    //getting a tagging decision, is the jet tagged or not
    bool tagged = tool->accept(*jet);

    //you can get the tagger weight,
    double tagweight = tool->getTaggerWeight( *jet );

    std::cout << "jet " << jet_index << " " <<  taggerName  << "  " << workingPointName << " is tagged " << tagged << " tag weight " << tagweight << std::endl;


    //if you have DL1 weights, you can get the tagger weight this way
    const xAOD::BTagging *btag = jet->btagging();

    double dl1_pb = btag->auxdata<double>("DL1_pb");
    double dl1_pc = btag->auxdata<double>("DL1_pc");
    double dl1_pu = btag->auxdata<double>("DL1_pu");

    tagweight = tool->getTaggerWeight(dl1_pb,dl1_pc,dl1_pu);

    std::cout << " tagweight " << tagweight << std::endl;

    //you can also extract the cut value (which may or may not depedn on the jet pt)

    double cutval = tool->getCutValue( jet->pt() ); //provide the pt in MeV

    std::cout << " cut value " << cutval << std::endl;


    jet_index++;

  }


  return 0;
}
