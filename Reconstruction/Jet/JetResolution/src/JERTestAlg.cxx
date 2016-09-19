/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM includes
#include "xAODJet/JetContainer.h"

// Local includes
#include "JERTestAlg.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
JERTestAlg::JERTestAlg(const std::string& name, ISvcLocator* svcLoc)
    : AthAlgorithm(name, svcLoc),
      m_jerSmearingTool("JERSmearingTool/JERSmearingTool")
{
  declareProperty("JERSmearingTool", m_jerSmearingTool);
  declareProperty("ApplySystematics", m_applySystematics = true);
}

//-----------------------------------------------------------------------------
// Initialize
//-----------------------------------------------------------------------------
StatusCode JERTestAlg::initialize()
{
  ATH_MSG_INFO("Initialize");

  // Try to retrieve the tools
  ATH_CHECK( m_jerSmearingTool.retrieve() );

  // Build systematic list: nominal, +1 jer, -1 jer
  m_sysList.clear();
  if(m_applySystematics){
    m_sysList.resize(2);
    m_sysList[1].insert(CP::SystematicVariation("JET_JER_SINGLE_NP", 1));
  }
  else m_sysList.resize(1);

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execute
//-----------------------------------------------------------------------------
StatusCode JERTestAlg::execute()
{
  // Retrieve the jets
  const xAOD::JetContainer* jets = 0;
  ATH_CHECK( evtStore()->retrieve(jets, "AntiKt4LCTopoJets") );

  // Print some event info
  ATH_MSG_DEBUG("Number of jets: " << jets->size());

  // Loop over systematics
  for(auto sysSet : m_sysList){

    ATH_MSG_DEBUG("Applying systematic: " <<
                  (sysSet.size() == 0 ? "Nominal" : sysSet.name()));

    if(m_jerSmearingTool->applySystematicVariation(sysSet)
       != CP::SystematicCode::Ok){
      ATH_MSG_ERROR("Problem applying systematic");
      return StatusCode::FAILURE;
    }

    // Loop over the jets
    for(auto jet : *jets){

      // Copy the jet and smear it
      xAOD::Jet* newJet = 0;
      if(m_jerSmearingTool->correctedCopy(*jet, newJet)
         != CP::CorrectionCode::Ok){
        ATH_MSG_ERROR("Problem correcting jet");
        return StatusCode::FAILURE;
      }

      // Dump some input jet info
      ATH_MSG_DEBUG("Input  jet: pt = " << jet->pt()*0.001
                    << ", eta = " << jet->eta());

      // Dump jet info after applying the tool
      ATH_MSG_DEBUG("Output jet: pt = " << newJet->pt()*0.001
                    << ", eta = " << newJet->eta());

      // Clean up the copy
      delete newJet;

    }

  }

  return StatusCode::SUCCESS;
}

