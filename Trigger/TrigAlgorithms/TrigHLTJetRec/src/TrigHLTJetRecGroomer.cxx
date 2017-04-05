/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetRecGroomer: Provide the method for taking an ungroomed jet collection
// and trimming it, creating an output trimmed jet collection

#include "TrigHLTJetRec/TrigHLTJetRecGroomer.h"

#include "JetRec/JetRecTool.h"



TrigHLTJetRecGroomer::TrigHLTJetRecGroomer(const std::string& name,
                                           ISvcLocator* pSvcLocator)
    : TrigHLTJetRecFromCluster(name,pSvcLocator)
{
    declareProperty("jetTrimTool",m_jetTrimTool);
}

TrigHLTJetRecGroomer::~TrigHLTJetRecGroomer()
{ }


HLT::ErrorCode TrigHLTJetRecGroomer::hltInitialize()
{
    // Apply the default initialization
    HLT::ErrorCode ec = TrigHLTJetRecBase<xAOD::CaloClusterContainer>::hltInitialize();
    if (ec != HLT::OK)
        return ec;

    // Retrieve the trimmer ToolHandle
    StatusCode sc = m_jetTrimTool.retrieve();
    if (sc.isSuccess())
        ATH_MSG_INFO("Retrieved jetTrimTool: " << m_jetTrimTool->name());
    else
    {
        ATH_MSG_ERROR("Failed to retrieve the jetTrimTool: " << m_jetTrimTool->name());
        return HLT::ERROR;
    }
    
    return HLT::OK;
}




//const xAOD::JetContainer* TrigHLTJetRecGroomer::build() const
HLT::ErrorCode
TrigHLTJetRecGroomer::build(fastjet::ClusterSequence*& cs,
                            xAOD::JetContainer*& trimmedJets) const{
  
  ATH_MSG_VERBOSE("Preparing to groom jets...");
  
  // get the ungroomed jet collection
  ATH_MSG_VERBOSE("Building ungroomed input jet container");
  xAOD::JetContainer* ungroomedJets = new xAOD::JetContainer;
  // auto ungroomedJets =
  HLT::ErrorCode status =
    TrigHLTJetRecBase<xAOD::CaloClusterContainer>::defaultBuild(cs,
                                                                ungroomedJets);
  if (status != HLT::OK){return status;}
    
  // Convert the trimming tool from a ToolHandle<IJetRecTool> to a JetRecTool so we can skip StoreGate
  // JetRecTool* trimmerTool = const_cast<JetRecTool*>(dynamic_cast<const JetRecTool*>(&*m_jetTrimTool));
  // Set the input jet container to be trimmed
  // trimmerTool->setInputJetContainer(ungroomedJets);
  
  const PseudoJetContainer* pseudojets = getPseudoJetContainer();
  // Now trim it
  ATH_MSG_VERBOSE("Applying jet trimming");
  trimmedJets = m_jetTrimTool->groom(ungroomedJets, *pseudojets);
  ATH_MSG_VERBOSE("Done trimming jets");
  
  
  
  // DEBUGGING
  /*
    for (size_t iJet = 0; iJet < trimmedJets->size(); ++iJet)
    {
        const float orig_pt = ungroomedJets->at(iJet)->pt();
        const float trim_pt = trimmedJets->at(iJet)->pt();
        const float orig_const_pt = ungroomedJets->at(iJet)->getAttribute<xAOD::JetFourMom_t>("JetConstitScaleMomentum").Pt();
        const float trim_const_pt = trimmedJets->at(iJet)->getAttribute<xAOD::JetFourMom_t>("JetConstitScaleMomentum").Pt();
        const float orig_em_pt = ungroomedJets->at(iJet)->getAttribute<xAOD::JetFourMom_t>("JetEMScaleMomentum").Pt();
        const float trim_em_pt = trimmedJets->at(iJet)->getAttribute<xAOD::JetFourMom_t>("JetEMScaleMomentum").Pt();
        const float ratio = trim_pt != 0 ? orig_pt / trim_pt : -1;
        const float ratio_const = trim_const_pt != 0 ? orig_const_pt / trim_const_pt : -1;
        const float ratio_em = trim_em_pt != 0 ? orig_em_pt / trim_em_pt : -1;
        
        ATH_MSG_DEBUG("Ungroomed/groomed jet number " << iJet << " has constituents of " << ungroomedJets->at(iJet)->numConstituents() << "/" << trimmedJets->at(iJet)->numConstituents() << ", pT ratio is " << orig_pt << "/" << trim_pt << " = " << ratio << " , constscale ratio is " << orig_const_pt << "/" << trim_const_pt << " = " << ratio_const << " , emscale ratio is " << orig_em_pt << "/" << trim_em_pt << " = " << ratio_em);
        }
    */

  // Get rid of the intermediate (ungroomed) jets
  auto ungroomedStore = ungroomedJets->getStore();
  delete ungroomedStore;
  delete ungroomedJets;
    
  // return trimmedJets;
  return HLT::OK;
}



