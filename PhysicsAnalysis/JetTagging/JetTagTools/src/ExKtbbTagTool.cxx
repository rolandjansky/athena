/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/ExKtbbTagTool.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetContainerInfo.h"
#include "JetEDM/IConstituentUserInfo.h"
#include "JetEDM/FastJetUtils.h"

namespace Analysis{

ExKtbbTagTool::ExKtbbTagTool(std::string myname):
  JetModifierBase(myname),
  m_JetAlgorithm(""),
  m_JetRadius(0.),
  m_PtMin(0.),
  m_ExclusiveNJets(0),
  m_InputJetContainerName(""),
  m_SubjetRecorderTool("SubjetRecorderTool"),
  m_SubjetLabel(""),
  m_SubjetContainerName(""),
  m_SubjetAlgorithm_BTAG(""),
  m_SubjetRadius_BTAG(0.)
{
  declareProperty("JetAlgorithm", m_JetAlgorithm);
  declareProperty("JetRadius", m_JetRadius);
  declareProperty("PtMin", m_PtMin);
  declareProperty("ExclusiveNJets", m_ExclusiveNJets);

  declareProperty("InputJetContainerName", m_InputJetContainerName);
  declareProperty("SubjetRecorder", m_SubjetRecorderTool);
  declareProperty("SubjetLabel", m_SubjetLabel);
  declareProperty("SubjetContainerName", m_SubjetContainerName);
  declareProperty("SubjetAlgorithm_BTAG", m_SubjetAlgorithm_BTAG);
  declareProperty("SubjetRadius_BTAG", m_SubjetRadius_BTAG);
}

StatusCode ExKtbbTagTool::initialize(){
  if(m_SubjetRecorderTool.retrieve().isFailure()){
    ATH_MSG_ERROR("#BTAG# Unable to retrieve SubjetRecorder Tool");
    return StatusCode::FAILURE;
  }
  else{
    ATH_MSG_INFO("#BTAG# Successfully retrieve SubjetRecorder Tool");
  }

  return StatusCode::SUCCESS;
}

int ExKtbbTagTool::modifyJet(xAOD::Jet& jet) const {

  // run subjet finding //

  xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_JetAlgorithm);
  fastjet::JetAlgorithm fjalg = xAOD::JetAlgorithmType::fastJetDef(ialg);

  JetSubStructureUtils::SubjetFinder subjetFinder(fjalg, m_JetRadius, m_PtMin, m_ExclusiveNJets);
  std::vector<fastjet::PseudoJet> constituents_pj = constituentPseudoJets(jet);
  // std::vector<fastjet::PseudoJet> constituents = jet::JetConstituentFiller::constituentPseudoJets(jet);
  fastjet::PseudoJet sum_constituents_pj = fastjet::join(constituents_pj);
  std::vector<fastjet::PseudoJet> subjets_pj = subjetFinder.result(sum_constituents_pj);

  // record subjets //

  auto subjets_nonconst = m_SubjetRecorderTool->recordSubjets(subjets_pj, jet);   // since we are using customized constituent pseudo-jet, constituents information will not be stored here

  // store the subjet container name and index //

  // We do this since ElementLink to the subjet could be broken after the deep-copy in b-tagging part. This was not a problem before 20.7. The reason behind it is still unknown.
  // Assuming the subjet container order is un-changed during b-tagging deep-copy, which SHOULD be valid.

  std::vector<const xAOD::Jet*> ExKtSubJets;
  if(!jet.getAssociatedObjects<xAOD::Jet>(m_SubjetLabel.c_str(), ExKtSubJets)){
    ATH_MSG_WARNING("Unable to fetch subjet collection in ExKtbbTagTool::modifyJet : " << m_SubjetLabel.c_str());
    ATH_MSG_WARNING("Nothing to be done for this problem. But you might crash very soon.");
  }

  jet.auxdata<std::string>(m_SubjetLabel + "_ContainerName") = m_SubjetContainerName;

  std::vector<int> SubjetIndexVector;
  for(auto subjet : ExKtSubJets){
    SubjetIndexVector.push_back(subjet->index());
  }
  jet.auxdata<std::vector<int> >(m_SubjetLabel + "_IndexList") = SubjetIndexVector;

  // overwrite something / store constituents //

  for(unsigned int index_subjet = 0; index_subjet < subjets_nonconst.size(); index_subjet++){
    auto subjet_nonconst = subjets_nonconst[index_subjet];
    auto subjet_pj = subjets_pj[index_subjet];

    // jet finding 
    subjet_nonconst->setAlgorithmType(xAOD::JetAlgorithmType::algId(m_SubjetAlgorithm_BTAG));
    subjet_nonconst->setSizeParameter(m_SubjetRadius_BTAG);

    // jet input type
    subjet_nonconst->setInputType(jet.getInputType());

    // setup constituents

    // check if constituents has been filled before
    if(subjet_nonconst->numConstituents() == 0){

      for(auto pj_constituent : subjet_pj.constituents()){
        int index_constituent = pj_constituent.user_index();  // index in parent jet constituent vector

        auto el_constituent = jet.constituentLinks()[index_constituent];

        if(!el_constituent.isValid()){
          ATH_MSG_WARNING("#BTAG# Element link to jet constituent is inValid! It will still be stored anyway ... ");
        }

        subjet_nonconst->addConstituent(el_constituent, 1.0);
      }

    }
    else{
      ATH_MSG_WARNING("#BTAG# Constituent link to subjet is already built, which is NOT expected!");
    }

    // set correct constituent signal state
    subjet_nonconst->setConstituentsSignalState(jet.getConstituentsSignalState());

    // add parent jet information, in case the "parent" link is broken
    auto el_parent = subjet_nonconst->auxdata<ElementLink<xAOD::JetContainer> >("Parent");
    if(!el_parent.isValid()){
      ATH_MSG_WARNING("#BTAG# Element link from ExKtSubjet to parent jet is invalid! No backup parent link information will be stored.");
    }
    else{
      subjet_nonconst->auxdata<std::string>("Parent_ContainerName") = m_InputJetContainerName;
      subjet_nonconst->auxdata<int>("Parent_Index") = jet.index();
    }

  }

  // for(auto subjet : ExKtSubJets){
  //   if(!subjet){
  //     std::cout << "Empty ptr to subjet in ExKtbbTagTool::modifyJet at position 2!" << std::endl;
  //   }
  // }

  return 0;
}

std::vector<fastjet::PseudoJet> ExKtbbTagTool::constituentPseudoJets(xAOD::Jet& jet) const{
  // code adapted from JetConstituentFiller.cxx
  // Cannot use JetConstituentFiller utils due to code crash from unknown reason
  // It seems the fastjet link is broken after a deep copy of parent jet
  // but anyway ... 

  std::vector<fastjet::PseudoJet> constituents;

  xAOD::JetConstituentVector constituents_tmp = jet.getConstituents();
  xAOD::JetConstituentVector::iterator it = constituents_tmp.begin();
  xAOD::JetConstituentVector::iterator itE = constituents_tmp.end();

  int index = 0;
  for( ; it !=itE; ++it){
    fastjet::PseudoJet constituent_pj = fastjet::PseudoJet( it->Px(), it->Py(), it->Pz(), it->E() );    // this is guaranteed to be the scale used in jet finding
    constituent_pj.set_user_index(index);        // each constituent_pj will store index in parent jet constituent ve tor
    constituents.push_back( constituent_pj );

    index++;
  }

  return constituents;
}

}
