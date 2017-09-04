/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthQGDecorationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Create a single decoration for flavor tagging of truth jets

#include "DerivationFrameworkMCTruth/TruthQGDecorationTool.h"
#include "xAODJet/JetContainer.h"
#include <string>

// Constructor
DerivationFramework::TruthQGDecorationTool::TruthQGDecorationTool(const std::string& t,
        const std::string& n,
        const IInterface* p ) :
    AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty ("JetCollection",
          m_jetsKey = "AntiKt4TruthJets",
          "Name of jet collection for decoration");
  declareProperty ("OutputDecoration",
          m_decOutput = "TrueFlavor",
          "Name of the output decoration on the jet");
}

// Destructor
DerivationFramework::TruthQGDecorationTool::~TruthQGDecorationTool() {
}

// Function to do dressing, implements interface in IAugmentationTool
StatusCode DerivationFramework::TruthQGDecorationTool::addBranches() const
{
  // Retrieve the truth collections
  const DataHandle<xAOD::JetContainer> inputJets(nullptr);
  CHECK(evtStore()->retrieve(inputJets, m_jetsKey).isFailure());

  SG::AuxElement::Decorator<int> output_decorator(m_decOutput);

  for (auto ajet : *inputJets){
    if (!ajet->isAvailable<int>("PartonTruthLabelID") ||
        !ajet->isAvailable<int>("ConeTruthLabelID")){
      ATH_MSG_ERROR("Did not have input decorations available");
      return StatusCode::FAILURE;
    } // Now we have the input decorations          
    if (ajet->auxdata<int>("ConeTruthLabelID")!=0){
      output_decorator(*ajet) = ajet->auxdata<int>("ConeTruthLabelID");
    } else {
      output_decorator(*ajet) = ajet->auxdata<int>("PartonTruthLabelID");
    }
  } // Loop over jets

  return StatusCode::SUCCESS;
}
