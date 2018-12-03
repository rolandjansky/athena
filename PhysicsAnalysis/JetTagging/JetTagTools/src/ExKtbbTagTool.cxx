/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/ExKtbbTagTool.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetContainerInfo.h"
#include "JetEDM/IConstituentUserInfo.h"
#include "JetEDM/FastJetUtils.h"

#include "TObjArray.h"
#include "TObjString.h"

namespace Analysis{

ExKtbbTagTool::ExKtbbTagTool(std::string myname):
  JetModifierBase(myname),
  m_JetAlgorithm(""),
  m_JetRadius(0.),
  m_PtMin(0.),
  m_ExclusiveNJets(0),
  m_doTrack(false),
  m_InputJetContainerName(""),
  m_SubjetRecorderTool("SubjetRecorderTool"),
  m_SubjetLabel(""),
  m_SubjetContainerName(""),
  m_SubjetAlgorithm_BTAG(""),
  m_SubjetRadius_BTAG(0.),
  m_TrkSelTool("InDet::InDetTrackSelectionTool/TrackSelectionTool"),
  m_PrimaryVtxContainerName("PrimaryVertices"),
  m_SubjetBoostConstituent(false),
  m_GhostSources(""),
  m_GhostLabels("")
{
  declareProperty("JetAlgorithm", m_JetAlgorithm);
  declareProperty("JetRadius", m_JetRadius);
  declareProperty("PtMin", m_PtMin);
  declareProperty("ExclusiveNJets", m_ExclusiveNJets);
  declareProperty("doTrack", m_doTrack);

  declareProperty("InputJetContainerName", m_InputJetContainerName);
  declareProperty("SubjetRecorder", m_SubjetRecorderTool);
  declareProperty("SubjetLabel", m_SubjetLabel);
  declareProperty("SubjetContainerName", m_SubjetContainerName);
  declareProperty("SubjetAlgorithm_BTAG", m_SubjetAlgorithm_BTAG);
  declareProperty("SubjetRadius_BTAG", m_SubjetRadius_BTAG);

  declareProperty("TrackSelectionTool", m_TrkSelTool);
  declareProperty("PrimaryVtxContainerName", m_PrimaryVtxContainerName);

  declareProperty("SubjetBoostConstituent", m_SubjetBoostConstituent);

  declareProperty("GhostSources", m_GhostSources);
  declareProperty("GhostLabels", m_GhostLabels);

  m_GhostLabelVector.clear();
  m_priVtx = 0;
  m_jets_original = 0;
}

StatusCode ExKtbbTagTool::initialize(){
  // Initialize subjet recorder
  if(m_SubjetRecorderTool.retrieve().isFailure()){
    ATH_MSG_ERROR("#BTAG# Unable to retrieve SubjetRecorder Tool");
    return StatusCode::FAILURE;
  }
  else{
    ATH_MSG_INFO("#BTAG# Successfully retrieve SubjetRecorder Tool");
  }

  // Initialize the track selection tool
  if(m_doTrack && m_TrkSelTool.retrieve().isFailure()){
    ATH_MSG_ERROR("#BTAG# Unable to retrieve TrackSelectionTool");
    return StatusCode::FAILURE;
  }

  // Convert m_GhostSources / m_GhostLabels to vector of strings
  m_GhostSourceVector = ConvertStringToVector(m_GhostSources, ",");
  m_GhostSourceVector.push_back("_THIS_");    // reserved for the input jet itself
  m_GhostLabelVector  = ConvertStringToVector(m_GhostLabels, ",");

  return StatusCode::SUCCESS;
}

std::vector<fastjet::PseudoJet> ExKtbbTagTool::getBoostedConstituents(xAOD::Jet& jet) const {
  std::vector<fastjet::PseudoJet> vec_conboost;

  // prepare the boost
  TLorentzVector tlv_jet(0, 0, 0, 0); 
  double jetMass = jet.m();
  if (jetMass < 1000. ) jetMass = 1000.; // 100. MeV...
  tlv_jet.SetPtEtaPhiM(jet.pt(), jet.eta(), jet.phi(), jetMass);
  TVector3 t3_boost = -1. * tlv_jet.BoostVector();

  double minimumPt = 9e99;    // need to record down the minimum pT after boost, since it could be very small! 

  if(m_doTrack){
    // get associated tracks
    std::vector<const xAOD::TrackParticle*> jet_tracks;
    if(!jet.getAssociatedObjects("GhostTrack", jet_tracks)){
      ATH_MSG_WARNING("Unable to fetech association \"GhostTrack\"!");
    }

    for(unsigned int index = 0; index < jet_tracks.size(); index++){
      auto track = jet_tracks[index];

      // apply selection on tracks
      if(!TrackSelection(track)) continue;

      TLorentzVector tlv_con = track->p4();
      tlv_con.Boost(t3_boost);
      fastjet::PseudoJet constituent_pj = fastjet::PseudoJet( tlv_con.Px(), tlv_con.Py(), tlv_con.Pz(), tlv_con.E() );    // this is guaranteed to be the scale used in jet finding
      constituent_pj.set_user_index(index);        // each constituent_pj will store index in the ghost track vector associated to source jet
      vec_conboost.push_back( constituent_pj );

      if(constituent_pj.perp() < minimumPt) minimumPt = constituent_pj.perp();
    }
  }
  else{
    xAOD::JetConstituentVector constituents_tmp = jet.getConstituents();

    if (!constituents_tmp.isValid()){
      ATH_MSG_ERROR("#BTAG# Unable to retrieve valid constituents from parent of large R jet");
      return vec_conboost;
    }   

    xAOD::JetConstituentVector::iterator it = constituents_tmp.begin();
    xAOD::JetConstituentVector::iterator itE = constituents_tmp.end();

    int index = 0;
    for( ; it !=itE; ++it){

      TLorentzVector tlv_con(0, 0, 0, 0); 
      tlv_con.SetPxPyPzE( it->Px(), it->Py(), it->Pz(), it->E());
      tlv_con.Boost(t3_boost);
      fastjet::PseudoJet constituent_pj = fastjet::PseudoJet( tlv_con.Px(), tlv_con.Py(), tlv_con.Pz(), tlv_con.E() );
      constituent_pj.set_user_index(index); 
      vec_conboost.push_back( constituent_pj );

      if(constituent_pj.perp() < minimumPt) minimumPt = constituent_pj.perp();

      index++;
    } 
  }

  // now add in ghost associated particles
  for(auto JetSourceName : m_GhostSourceVector){
    const xAOD::Jet* jetSource = 0;

    if(JetSourceName == "_THIS_"){
      jetSource = (&jet);
    }
    else{
      auto el_jetsource = jet.auxdata<ElementLink<xAOD::JetContainer> >(JetSourceName);

      if(!el_jetsource.isValid()){
        ATH_MSG_WARNING("ElementLink " << JetSourceName << " is invalid. This jet source will be skipped");
        continue;
      }

      jetSource = (*el_jetsource);
    }

    // protection over null ptr
    if(jetSource == 0){
      ATH_MSG_WARNING("Null ptr returned for jet source " << JetSourceName << ". It will be skipped");
      continue;
    }

    for(unsigned int index_GhostLabel = 0; index_GhostLabel < m_GhostLabelVector.size(); index_GhostLabel++){
      std::string GhostLabel = m_GhostLabelVector[index_GhostLabel];

      std::vector<const xAOD::IParticle*> GhostParticles;
      if(!jetSource->getAssociatedObjects<xAOD::IParticle>(GhostLabel, GhostParticles)){
        ATH_MSG_WARNING("Unable to fetch ghost associated collection " << GhostLabel);
        GhostParticles.clear();
      }

      for(unsigned int index_GhostParticle = 0; index_GhostParticle < GhostParticles.size(); index_GhostParticle++){
        auto GhostParticle = GhostParticles[index_GhostParticle];

        if(GhostParticle->p4().E() <= 0.0) continue;     // yes, we always skip negative energy

        TLorentzVector tlv_con(0, 0, 0, 0);
        tlv_con.SetPxPyPzE( GhostParticle->p4().Px(), GhostParticle->p4().Py(), GhostParticle->p4().Pz(), GhostParticle->p4().E() );
        tlv_con.Boost(t3_boost);
        fastjet::PseudoJet constituent_pj = fastjet::PseudoJet( tlv_con.Px(), tlv_con.Py(), tlv_con.Pz(), tlv_con.E() );

        // determine ghost dynamically (100 times smaller than the minimum Pt), making sure that ghost is still much smaller than all real constituents (after boosting back to CoM, the constituent might be nearly static)
        double ghostScale = (minimumPt/1000.)/(constituent_pj.perp());
        if(ghostScale == 0.){
          ATH_MSG_WARNING("Ghost scale is 0. Reset to 1e-40. Minimum pT is " << minimumPt);
          ghostScale = 1e-40;
        }

        constituent_pj *= ghostScale;                         // a pretty standard ghost scale
        constituent_pj.set_user_index(-1);
        constituent_pj.set_user_info(new Analysis::SimplePseudoJetUserInfo(JetSourceName, GhostLabel, index_GhostParticle));
        vec_conboost.push_back( constituent_pj );
      }
    }
  }

  return vec_conboost;
}

int ExKtbbTagTool::modify(xAOD::JetContainer& jets) const{
  // get primary vertex container (and primary vertex) here for each event
  // only necessary when m_doTrack is turned on
  if(m_doTrack){
    const xAOD::VertexContainer* vertices = 0;
    if(evtStore()->retrieve(vertices, m_PrimaryVtxContainerName).isFailure()){
      ATH_MSG_ERROR("Could not retrieve the VertexContainer from evtStore: " << m_PrimaryVtxContainerName);
      return 0;
    }

    if(!vertices){
      ATH_MSG_ERROR("Null ptr returned for VertexContainer: " << m_PrimaryVtxContainerName);
      return 0;
    }

    if(vertices->size() == 0){
      ATH_MSG_WARNING("There are no vertices in this container. Exiting");
      return 0;
    }

    m_priVtx = findHSVertex(vertices);
  }

  // fetch the pointer to original jet container
  m_jets_original = 0;
  if(evtStore()->retrieve(m_jets_original, m_InputJetContainerName).isFailure()){
    ATH_MSG_ERROR("#BTAG# Unable to retrieve input jet container " << m_InputJetContainerName);
    return StatusCode::FAILURE;
  }

  // always create subjet container here, so that there would be no "Check number of writes failed" ERROR when the parent jet container is empty
  // move the relevant code fragment from SubjetRecorderTool.cxx to here
  xAOD::JetContainer *subjet_container = 0;
  subjet_container = evtStore()->tryRetrieve<xAOD::JetContainer>(m_SubjetContainerName);
  if(subjet_container == 0) {
    StatusCode sc;
    subjet_container = new xAOD::JetContainer;
    subjet_container->setStore(new xAOD::JetAuxContainer);
    sc = evtStore()->record(subjet_container, m_SubjetContainerName);
    if(sc.isFailure()) {
      ATH_MSG_ERROR("Error recording subjet container (" << m_SubjetContainerName << ")");
      return 0;
    }
    sc = evtStore()->record(dynamic_cast<xAOD::JetAuxContainer*>(subjet_container->getStore()), m_SubjetContainerName + "Aux.");
    if(sc.isFailure()) {
      ATH_MSG_ERROR("Error recording subjet aux container (" << m_SubjetContainerName << "Aux.)");
      return 0;
    }
  }
  else{
    // Not failing immediately, but this is really NOT expected!
    ATH_MSG_ERROR("Is it really expected that subjet collection " << m_SubjetContainerName << " has already been created ?!");
  }

  // copy from base class JetModifierBase
  for ( xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ++ijet) {
    modifyJet(**ijet);
  }
  return 0;
}

int ExKtbbTagTool::modifyJet(xAOD::Jet& jet) const {

  // run subjet finding //

  xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_JetAlgorithm);
  fastjet::JetAlgorithm fjalg = xAOD::JetAlgorithmType::fastJetDef(ialg);

  JetSubStructureUtils::SubjetFinder subjetFinder(fjalg, m_JetRadius, m_PtMin, m_ExclusiveNJets);


  std::vector<fastjet::PseudoJet> constituents_pj;
  if (m_SubjetBoostConstituent) constituents_pj = getBoostedConstituents(jet);
  else                          constituents_pj = constituentPseudoJets(jet);

  // std::vector<fastjet::PseudoJet> constituents = jet::JetConstituentFiller::constituentPseudoJets(jet);
  fastjet::PseudoJet sum_constituents_pj = fastjet::join(constituents_pj);
  std::vector<fastjet::PseudoJet> subjets_pj = subjetFinder.result(sum_constituents_pj);

  // boost subjet for CoM
  double jetMass = jet.m();
  if (jetMass < 1000. ) jetMass = 1000.; // 1GeV
  TLorentzVector tlv_jet(0, 0, 0, 0);
  tlv_jet.SetPtEtaPhiM(jet.pt(), jet.eta(), jet.phi(), jetMass);
  TVector3 t3_boost = tlv_jet.BoostVector();

  if (m_SubjetBoostConstituent){
    for (unsigned int jsub=0; jsub<subjets_pj.size(); jsub++){
      TLorentzVector tlv_subj(0, 0, 0, 0);
      tlv_subj.SetPxPyPzE(subjets_pj.at(jsub).px(), subjets_pj.at(jsub).py(), subjets_pj.at(jsub).pz(), subjets_pj.at(jsub).e());
      tlv_subj.Boost(t3_boost);
      subjets_pj.at(jsub).reset_momentum(tlv_subj.Px(), tlv_subj.Py(), tlv_subj.Pz(), tlv_subj.E());
    }
  }

  // apply pT cut //
  // Note: in fastjet exclusive clustering mode, no pT cut is applied 

  std::vector<fastjet::PseudoJet> subjets_pj_selected;
  for(auto pj : subjets_pj){
    if(pj.perp() < m_PtMin) continue;
    subjets_pj_selected.push_back(pj);
  }
  subjets_pj = subjets_pj_selected;

  // record subjets
 // since we are using customized constituent pseudo-jet, constituents information will not be stored here
  auto subjets_nonconst = m_SubjetRecorderTool->recordSubjets(subjets_pj, jet);  

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
    if(m_doTrack){
      subjet_nonconst->setInputType(xAOD::JetInput::Track);
    }
    else{
      subjet_nonconst->setInputType(jet.getInputType());
    }    

    // setup constituents

    // check if constituents has been filled before
    if(subjet_nonconst->numConstituents() == 0){

      std::map<std::string, std::vector<const xAOD::IParticle*> > ghostMap;
      std::map<std::string, float>                                ghostPtMap;
      std::vector<std::string>                                    ghostLabelList;

      for(auto pj_constituent : subjet_pj.constituents()){

        int index_constituent = pj_constituent.user_index();  // index in parent jet constituent vector

        if(index_constituent >= 0){ // constituents
          ElementLink<xAOD::IParticleContainer> el_constituent;

          if(m_doTrack){
            try{
              std::vector<const xAOD::TrackParticle*> jet_tracks;
              jet.getAssociatedObjects("GhostTrack", jet_tracks);
              auto track = jet_tracks[index_constituent];
              el_constituent.toIndexedElement(*(dynamic_cast<const xAOD::IParticleContainer*>(track->container())), track->index());
            } catch (...){
              ATH_MSG_WARNING("#BTAG# Unable to restore \"GhostTrack\"!");
            }
          }
          else{
            el_constituent = jet.constituentLinks()[index_constituent];
          }

          if(!el_constituent.isValid()){
            ATH_MSG_WARNING("#BTAG# Element link to jet constituent is inValid! It will still be stored anyway ... ");
          }

          subjet_nonconst->addConstituent(el_constituent, 1.0);
        }
        else{ // ghost particles -- just build the map here
          auto pj_user_info = pj_constituent.user_info<Analysis::SimplePseudoJetUserInfo>();
          std::string pj_user_jetsource = pj_user_info.jetsource();
          std::string pj_user_label     = pj_user_info.label();
          int pj_user_index             = pj_user_info.index();

          // prepare jet source
          const xAOD::Jet* jetSource = 0;

          if(pj_user_jetsource == "_THIS_"){
            jetSource = (&jet);
          }
          else{
            auto el_jetsource = jet.auxdata<ElementLink<xAOD::JetContainer> >(pj_user_jetsource);

            if(!el_jetsource.isValid()){
              ATH_MSG_WARNING("ElementLink " << pj_user_jetsource << " is invalid. This constituent will be skipped");
              continue;
            }

            jetSource = (*el_jetsource);
          }

          // protection over null ptr
          if(jetSource == 0){
            ATH_MSG_WARNING("Null ptr returned for jet source " << pj_user_jetsource << ". This constituent will be skipped");
            continue;
          }

          // get ghost particle
          const xAOD::IParticle* GhostParticle = 0;
          try{
            GhostParticle = jetSource->getAssociatedObjects<xAOD::IParticle>(pj_user_label)[pj_user_index];
          }
          catch(...){
            GhostParticle = 0;
          }

          if(GhostParticle == 0){
            // would not crash the code immediately, but will eventually
            ATH_MSG_ERROR("Failure in retrieving ghost particle after clustering through label " << pj_user_label << " and index " << pj_user_index << " under jet source " << pj_user_jetsource);
          }
          else{
            std::string FinalGhostLabel = pj_user_label;
            if(pj_user_jetsource != "_THIS_"){
              FinalGhostLabel += "_";
              FinalGhostLabel += pj_user_jetsource;
            }

            ghostLabelList.push_back(FinalGhostLabel);
            ghostMap[FinalGhostLabel].push_back( GhostParticle );
            ghostPtMap[FinalGhostLabel] += GhostParticle->pt();
          }
        }

        // now add ghost particles in
        for(std::string JetSourceName : m_GhostSourceVector){
          for(std::string GhostLabel : m_GhostLabelVector){
            std::string FinalGhostLabel = GhostLabel;
            if(JetSourceName != "_THIS_"){
              FinalGhostLabel += "_";
              FinalGhostLabel += JetSourceName;
            }

            subjet_nonconst->setAssociatedObjects(FinalGhostLabel, ghostMap[FinalGhostLabel]);
            subjet_nonconst->setAttribute<int>(FinalGhostLabel + "Count", ghostMap[FinalGhostLabel].size());
            subjet_nonconst->setAttribute<float>(FinalGhostLabel + "Pt", ghostPtMap[FinalGhostLabel]);
          }
        }       
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
      // Overwrite the "Parent" EL
      // NB: As a jet modifier, this tool needs to be passed into JetRecTool
      //     However, for JetRecTool, the new jet container is recorded into the SG AFTER jet modifier
      //     Therefore, the pointer to original jet container is different to the pointer to jet container as passed into this tool
      //     We need the original jet container in order to make our EL pointing to parent jet more robust
      //     Of course, this is under the assumption that JetRecTool is in "update" mode (i.e. copy of itself) and jet sequence in container is unchanged
      ElementLink<xAOD::JetContainer> el_parent_new(*m_jets_original, jet.index());
      el_parent_new.toTransient();
      subjet_nonconst->setAttribute("Parent", el_parent_new);

      subjet_nonconst->auxdata<std::string>("Parent_ContainerName") = m_InputJetContainerName;
      subjet_nonconst->auxdata<int>("Parent_Index") = jet.index();
    }

  }

  return 0;
}

std::vector<fastjet::PseudoJet> ExKtbbTagTool::constituentPseudoJets(xAOD::Jet& jet) const{
  // code adapted from JetConstituentFiller.cxx
  // Cannot use JetConstituentFiller utils due to code crash from unknown reason
  // It seems the fastjet link is broken after a deep copy of parent jet
  // but anyway ... 

  std::vector<fastjet::PseudoJet> constituents;

  if(m_doTrack){
    // get associated tracks
    std::vector<const xAOD::TrackParticle*> jet_tracks;
    if(!jet.getAssociatedObjects("GhostTrack", jet_tracks)){
      ATH_MSG_WARNING("Unable to fetech association \"GhostTrack\"!");
    }

    for(unsigned int index = 0; index < jet_tracks.size(); index++){
      auto track = jet_tracks[index];

      // apply selection on tracks
      if(!TrackSelection(track)) continue;

      fastjet::PseudoJet constituent_pj = fastjet::PseudoJet( track->p4().Px(), track->p4().Py(), track->p4().Pz(), track->p4().E() );    // this is guaranteed to be the scale used in jet finding
      constituent_pj.set_user_index(index);        // each constituent_pj will store index in the ghost track vector associated to source jet
      constituents.push_back( constituent_pj );
    }
  }
  else{
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
  }

  // now add in ghost associated ones
  for(auto JetSourceName : m_GhostSourceVector){
    const xAOD::Jet* jetSource = 0;

    if(JetSourceName == "_THIS_"){
      jetSource = (&jet);
    }
    else{
      auto el_jetsource = jet.auxdata<ElementLink<xAOD::JetContainer> >(JetSourceName);

      if(!el_jetsource.isValid()){
        ATH_MSG_WARNING("ElementLink " << JetSourceName << " is invalid. This jet source will be skipped");
        continue;
      }

      jetSource = (*el_jetsource);
    }

    // protection over null ptr
    if(jetSource == 0){
      ATH_MSG_WARNING("Null ptr returned for jet source " << JetSourceName << ". It will be skipped");
      continue;
    }

    for(unsigned int index_GhostLabel = 0; index_GhostLabel < m_GhostLabelVector.size(); index_GhostLabel++){
      std::string GhostLabel = m_GhostLabelVector[index_GhostLabel];

      std::vector<const xAOD::IParticle*> GhostParticles;
      if(!jetSource->getAssociatedObjects<xAOD::IParticle>(GhostLabel, GhostParticles)){
        ATH_MSG_WARNING("Unable to fetch ghost associated collection " << GhostLabel << " under jet source " << JetSourceName);
        GhostParticles.clear();
      }

      for(unsigned int index_GhostParticle = 0; index_GhostParticle < GhostParticles.size(); index_GhostParticle++){
        auto GhostParticle = GhostParticles[index_GhostParticle];

        if(GhostParticle->p4().E() <= 0.0) continue;     // yes, we always skip negative energy

        fastjet::PseudoJet constituent_pj = fastjet::PseudoJet( GhostParticle->p4().Px(), GhostParticle->p4().Py(), GhostParticle->p4().Pz(), GhostParticle->p4().E() );
        constituent_pj *= 1e-40;                         // a pretty standard ghost scale
        constituent_pj.set_user_index(-1);
        constituent_pj.set_user_info(new Analysis::SimplePseudoJetUserInfo(JetSourceName, GhostLabel, index_GhostParticle));
        constituents.push_back( constituent_pj );
      }
    }
  }

  return constituents;
}

//
// (Wrapper) Function to retrieve parent/ungroomed jet of a groommed large-R jet
//
const xAOD::Jet* ExKtbbTagTool::GetParentJet(const xAOD::Jet *jet) const{
  ElementLink<xAOD::JetContainer> element_link = jet->auxdata<ElementLink<xAOD::JetContainer> >("Parent");

  if (element_link.isValid()) {
    return *element_link;
  }
  else {
    ATH_MSG_WARNING("Unable to get a link to the parent jet! Returning a NULL pointer.");
    return 0;
  }
}

std::vector<std::string> ExKtbbTagTool::ConvertStringToVector(std::string inputString, std::string delim) const{
  std::vector<std::string> output;

  TString inputTString(inputString);
  TObjArray* inputTString_tokenize = inputTString.Tokenize(delim.data());

  for(int istring = 0; istring < inputTString_tokenize->GetEntries(); istring++){
    TObjString* string_tmp = dynamic_cast<TObjString*>(inputTString_tokenize->At(istring));
    output.push_back(string_tmp->GetString().Data());
  }

  delete inputTString_tokenize;

  return output;
}

// The track selection is a bit hard-coded here --> It is the Run 2 track selection used bosth for track-jet and JVT calculation
// See reference here https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JvtManualRecalculation
bool ExKtbbTagTool::TrackSelection(const xAOD::TrackParticle* track) const{
  // First it should pass standard track selection tool (it should be setup as Loose selection in jobOption)
  if(track->pt() < 500.) return false;
  if(!m_TrkSelTool->accept(*track, m_priVtx)) return false;

  return ( track->vertex() == m_priVtx ) || ( (!track->vertex()) && ( std::abs( (track->z0() + track->vz() - m_priVtx->z()) * sin(track->theta()) ) < 3. ) );
}

const xAOD::Vertex* ExKtbbTagTool::findHSVertex(const xAOD::VertexContainer* vertices) const{
  for(unsigned int ivtx = 0; ivtx < vertices->size(); ivtx++){
    if(vertices->at(ivtx)->vertexType() == xAOD::VxType::PriVtx){
      return vertices->at(ivtx);
    }
  }

  ATH_MSG_WARNING("There is no vertex of type PriVtx. Taking default vertex.");
  return vertices->at(0);
}

}
