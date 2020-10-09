/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include "AsgMessaging/MessageCheck.h"

#include <string>
#include <iomanip>

int main() {

  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  BTaggingSelectionTool * tool = new BTaggingSelectionTool("BTagSelecTest");
  ANA_CHECK (tool->setProperty( "MaxEta", 2.5 ));
  ANA_CHECK (tool->setProperty( "MinPt", 20000. ));  
  //  ANA_CHECK (tool->setProperty( "FlvTagCutDefinitionsFileName","xAODBTaggingEfficiency/13TeV/2016-20_7-13TeV-MC15-CDI-May31_v1.root" ));
  ANA_CHECK (tool->setProperty( "FlvTagCutDefinitionsFileName","xAODBTaggingEfficiency/share/AntiKt2TrackJets_20160615.root" ));
  ANA_CHECK (tool->setProperty("TaggerName",     "MV2c00_MV2c100"  ));
  ANA_CHECK (tool->setProperty("OperatingPoint", "2DFixedCutBEff_85"   ));
  ANA_CHECK (tool->setProperty("JetAuthor",      "AntiKt2PV0TrackJets" ));

  // A successful initialisation ought to be checked for
  StatusCode code = tool->initialize();
  if (code != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool " << tool->name() << " failed! Subsequent results may not make sense." << std::endl;
  }
  else {
    std::cout << "Initialization of tool " << tool->name() << " finished." << std::endl;
  }

  bool retval = true;

  std::cout << "\nTesting function calls for a large pT range..." << std::endl;
  for (unsigned pt=15000; pt<1200000; pt+=100000){
    if (tool->accept(pt, 0.5, 0.5543, 0.)){
      std::cout << "Jet is tagged" << std::endl;
    } else {
      std::cout << "Jet is untagged" << std::endl;
    }
  }

  // #### Trying to build a b-jet on the fly
  // auto JetAuxContainer = new xAOD::JetAuxContainer();
  // auto JetContainer = new xAOD::JetContainer();
  // JetContainer->setStore(JetAuxContainer);
  // auto tjet = new xAOD::Jet();
  // JetContainer->push_back(tjet);

  // std::cout << "jet container ready, now btagging ones" << std::endl;

  // auto BTaggingAuxContainer = new xAOD::BTaggingAuxContainer();
  // auto BTaggingContainer = new xAOD::BTaggingContainer();
  // BTaggingContainer->setStore(BTaggingAuxContainer);
  // auto BTagging = new xAOD::BTagging();
  // BTaggingContainer->push_back(BTagging);

  // std::cout << "BTagging container ready, now the store" << std::endl;
  // xAOD::TStore store;
  // store.record(JetContainer, "JetTest");
  // store.record(JetAuxContainer, "JetTestAux.");
  // store.record(BTaggingContainer, "BTagTest");
  // store.record(BTaggingAuxContainer, "BTagTestAux.");
  // std::cout << "Store ready" << std::endl;
  // std::cout << tjet << "\t\t" << BTaggingContainer << "\t\t" << BTagging << std::endl;

  // ElementLink< xAOD::BTaggingContainer> linkBTagger;
  // linkBTagger.toContainedElement(*BTaggingContainer, BTagging);
  // // tjet->setBTaggingLink(linkBTagger);
  // // BTagging->setVariable("MV2c20", "discriminant", 0.20);
  // ######

  return retval;
}
