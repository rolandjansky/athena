/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthQGDecorationTool.cxx
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
          m_jetsKey = "AntiKt4TruthWZJets",
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
  CHECK(evtStore()->retrieve(inputJets, m_jetsKey));

  SG::AuxElement::Decorator<int> output_decorator(m_decOutput);

  for (auto ajet : *inputJets){
    if (!ajet->isAvailable<int>("PartonTruthLabelID") ||
        !ajet->isAvailable<int>("HadronConeExclTruthLabelID")){
      ATH_MSG_ERROR("Did not have input decorations available");
      return StatusCode::FAILURE;
    } // Now we have the input decorations          
    /* Agreement from the HF-tagging and Jet/MET group:
        - If it is non-zero, use the label from the HF-tagging group (b, c, tau)
        - If it is zero, use the label from the Jet/MET group (q/g)
        - In the case that the two disagree (e.g. Jet/MET says b and HF says light),
           multiply the Jet/MET label by 100 to ensure this case is kept separate
    */
    if (ajet->auxdata<int>("HadronConeExclTruthLabelID")!=0){
      output_decorator(*ajet) = ajet->auxdata<int>("HadronConeExclTruthLabelID");
    } else {
      if (std::abs(ajet->auxdata<int>("PartonTruthLabelID"))!=5 &&
          std::abs(ajet->auxdata<int>("PartonTruthLabelID"))!=4 &&
          std::abs(ajet->auxdata<int>("PartonTruthLabelID"))!=15){
        output_decorator(*ajet) = ajet->auxdata<int>("PartonTruthLabelID");
      } else {
        output_decorator(*ajet) = ajet->auxdata<int>("PartonTruthLabelID")*100;
      }
    }
  } // Loop over jets

  return StatusCode::SUCCESS;
}
