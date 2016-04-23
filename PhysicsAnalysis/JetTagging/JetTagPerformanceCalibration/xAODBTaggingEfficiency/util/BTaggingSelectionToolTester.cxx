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

  BTaggingSelectionTool * tool = new BTaggingSelectionTool("BTagSelecTest");
  tool->setProperty( "MaxEta", 2.5 );
  tool->setProperty( "MinPt", 20000. );  
  tool->setProperty( "FlvTagCutDefinitionsFileName", "xAODBTaggingEfficiency/cutprofiles_22072015.root" );
  tool->setProperty("TaggerName",     "MV2c20"  );
  tool->setProperty("OperatingPoint", "FixedCutBEff_70"   );
  tool->setProperty("JetAuthor",      "AntiKt4EMTopoJets" );

  // A successful initialisation ought to be checked for
  StatusCode code = tool->initialize();
  if (code != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool " << tool->name() << " failed! Subsequent results may not make sense." << std::endl;
  }
  else {
    std::cout << "Initialization of tool " << tool->name() << " finished." << std::endl;
  }

  bool retval = true;

  // std::cout << "\nTesting function calls for a large pT range..." << std::endl;
  // for (unsigned pt=15000; pt<1200000; pt+=100000){
  //   xAOD::JetFourMom_t p4(pt,0.7,0.3,1000.);

  //   xAOD::Jet * jet = new xAOD::Jet();

  //   jet->makePrivateStore();
  //   jet->setJetP4(p4);
  //   jet->setAttribute("ConeTruthLabelID", 0);
  //   jet->setAttribute("HadronConeExclTruthLabelID", 0);
  //   jet->setAttribute("MV2c20", 0.33333);
  //   if (tool->accept(jet)){
  //     std::cout << "Jet is tagged" << std::endl;
  //   } else {
  //     std::cout << "Jet is untagged" << std::endl;
  //   }
  // }


  auto JetAuxContainer = new xAOD::JetAuxContainer();
  auto JetContainer = new xAOD::JetContainer();
  JetContainer->setStore(JetAuxContainer);
  auto tjet = new xAOD::Jet();
  JetContainer->push_back(tjet);

  std::cout << "jet container ready, now btagging ones" << std::endl;

  auto BTaggingAuxContainer = new xAOD::BTaggingAuxContainer();
  auto BTaggingContainer = new xAOD::BTaggingContainer();
  BTaggingContainer->setStore(BTaggingAuxContainer);
  auto BTagging = new xAOD::BTagging();
  BTaggingContainer->push_back(BTagging);

  std::cout << "BTagging container ready, now the store" << std::endl;
  xAOD::TStore store;
  store.record(JetContainer, "JetTest");
  store.record(JetAuxContainer, "JetTestAux.");
  store.record(BTaggingContainer, "BTagTest");
  store.record(BTaggingAuxContainer, "BTagTestAux.");
  std::cout << "Store ready" << std::endl;
  std::cout << tjet << "\t\t" << BTaggingContainer << "\t\t" << BTagging << std::endl;

  ElementLink< xAOD::BTaggingContainer> linkBTagger;
  linkBTagger.toContainedElement(*BTaggingContainer, BTagging);
  // tjet->setBTaggingLink(linkBTagger);
  // BTagging->setVariable("MV2c20", "discriminant", 0.20);

  return retval;
}
