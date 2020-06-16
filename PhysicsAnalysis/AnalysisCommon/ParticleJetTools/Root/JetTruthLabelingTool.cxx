/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/JetTruthLabelingTool.h"

JetTruthLabelingTool::JetTruthLabelingTool(const std::string& name) :
  asg::AsgTool(name)
{

    declareProperty( "TruthLabelName",                m_truthLabelName = "R10TruthLabel_R21Consolidated");
    declareProperty( "IsTruthJetCollection",          m_isTruthJetCol = false);
    declareProperty( "UseTRUTH3",                     m_useTRUTH3 = false);
    declareProperty( "TruthParticleContainerName",    m_truthParticleContainerName = "TruthParticles");
    declareProperty( "TruthBosonContainerName",       m_truthBosonContainerName = "TruthBosonsWithDecayParticles");
    declareProperty( "TruthTopQuarkContainerName",    m_truthTopQuarkContainerName = "TruthTopQuarkWithDecayParticles");
    
    // Hard-code some values for R10TruthLabel_R21Consolidated
    // Functionality to customize labeling will be added later
    if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
      m_dRTruthJet = 0.75;
      m_dRTruthPart = 0.75;
      m_mLowTop = 140.0;
      m_mLowW = 50.0;
      m_mHighW = 100.0;
      m_mLowZ = 60.0;
      m_mHighZ = 110.0;
    }
}

StatusCode JetTruthLabelingTool::initialize(){

  ATH_MSG_INFO("Initializing " << name());
  print();

  // Check if TruthLabelName is supported. If not, give an error and
  // return FAILURE
  if(m_truthLabelName != "R10TruthLabel_R21Consolidated") {
    ATH_MSG_ERROR("TruthLabelName " << m_truthLabelName << " is not supported. Exiting...");
    return StatusCode::FAILURE;
  }

  m_acc_label = std::make_unique< SG::AuxElement::Accessor<int> >(m_truthLabelName);
  m_acc_dR_W = std::make_unique< SG::AuxElement::Accessor<float> >(m_truthLabelName+"_dR_W");
  m_acc_dR_Z = std::make_unique< SG::AuxElement::Accessor<float> >(m_truthLabelName+"_dR_Z");
  m_acc_dR_H = std::make_unique< SG::AuxElement::Accessor<float> >(m_truthLabelName+"_dR_H");
  m_acc_dR_Top = std::make_unique< SG::AuxElement::Accessor<float> >(m_truthLabelName+"_dR_Top");
  m_acc_NB = std::make_unique< SG::AuxElement::Accessor<int> >(m_truthLabelName+"_NB");

  m_dec_label = std::make_unique< SG::AuxElement::Decorator<int> >(m_truthLabelName);
  m_dec_dR_W = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_W");
  m_dec_dR_Z = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_Z");
  m_dec_dR_H = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_H");
  m_dec_dR_Top = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_Top");
  m_dec_NB = std::make_unique< SG::AuxElement::Decorator<int> >(m_truthLabelName+"_NB");
  m_dec_TruthJetMass = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_TruthJetMass");

  return StatusCode::SUCCESS;
}

void JetTruthLabelingTool::print() const {
  ATH_MSG_INFO("Parameters for " << name());

  ATH_MSG_INFO("xAOD information:");
  ATH_MSG_INFO("TruthLabelName:               " << m_truthLabelName);
  ATH_MSG_INFO("UseTRUTH3:                    " << ( m_useTRUTH3 ? "True" : "False"));
  if(m_useTRUTH3) {
    ATH_MSG_INFO("TruthBosonContainerName:      " << m_truthBosonContainerName);
    ATH_MSG_INFO("TruthTopQuarkContainerName:   " << m_truthTopQuarkContainerName);
  }
  else {
    ATH_MSG_INFO("TruthParticleContainerName:   " << m_truthParticleContainerName);
  }
  if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
    ATH_MSG_INFO("dRTruthJet:    " << std::to_string(m_dRTruthJet));
    ATH_MSG_INFO("dRTruthPart:   " << std::to_string(m_dRTruthPart));
    ATH_MSG_INFO("mLowTop:       " << std::to_string(m_mLowTop));
    ATH_MSG_INFO("mLowW:         " << std::to_string(m_mLowW));
    ATH_MSG_INFO("mHighW:        " << std::to_string(m_mHighW));
    ATH_MSG_INFO("mLowZ:         " << std::to_string(m_mLowZ));
    ATH_MSG_INFO("mHighZ:        " << std::to_string(m_mHighZ));
  }
}

int JetTruthLabelingTool::getTruthJetLabelR21Consolidated( const xAOD::Jet &jet, std::vector<std::pair<TLorentzVector,int> > tlv_truthParts ) const {

  bool isMatchW = false;
  bool isMatchZ = false;
  bool isMatchH = false;
  bool isMatchTop = false;

  // Distances to truth particles
  float dR_W = 9999;
  float dR_Z = 9999;
  float dR_H = 9999;
  float dR_Top = 9999;

  for (auto tlv_truth : tlv_truthParts) {
    float dR = tlv_truth.first.DeltaR(jet.p4());
    if( dR < m_dRTruthPart ) {

      if ( std::abs(tlv_truth.second) == 23 && !isMatchZ) {
        dR_Z = dR;
        isMatchZ = true;
      }

      if ( std::abs(tlv_truth.second) == 24 && !isMatchW) {
        dR_W = dR;
        isMatchW = true;
      }

      if ( std::abs(tlv_truth.second) == 25 && !isMatchH) {
        dR_H = dR;
        isMatchH = true;
      }

      if ( std::abs(tlv_truth.second) == 6 && !isMatchTop) {
        dR_Top = dR;
        isMatchTop = true;
      }

    }
  }

  // find ghost associated B-hadrons
  int nMatchB = 0;
  if( !jet.getAttribute<int>( "GhostBHadronsFinalCount", nMatchB ) ){

    std::vector<const xAOD::TruthParticle*> ghostB; // Ghost associated B hadrons after FSR
    if( !jet.getAssociatedObjects<xAOD::TruthParticle>( "GhostBHadronsFinal", ghostB ) ){      
      ATH_MSG_ERROR("GhostBHadronsFinal cannot be retrieved! Truth label definition of W/top tagger might be wrong");
    } 
    nMatchB = ghostB.size();
  }

  // Add matching criteria decorations
  (*m_dec_dR_W)(jet) = dR_W;
  (*m_dec_dR_Z)(jet) = dR_Z;
  (*m_dec_dR_H)(jet) = dR_H;
  (*m_dec_dR_Top)(jet) = dR_Top;
  (*m_dec_NB)(jet) = nMatchB;

  if( !isMatchTop && !isMatchW && !isMatchZ && !isMatchH) {
    return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::qcd);
  }

  if(isMatchH) {
    if(nMatchB > 1) return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::Hbb);
    else return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::other_From_H);
  }
  else if( isMatchTop && nMatchB > 0 && m_mLowTop < jet.m()*0.001 ) {
    return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::tqqb); 
  }
  else if( isMatchW && nMatchB == 0 && m_mLowW < jet.m()*0.001 && jet.m()*0.001 < m_mHighW ) {
    if ( isMatchTop ) {
      return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::Wqq_From_t);
    }
    else {
      return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::Wqq);
    } 
  }
  else if( isMatchZ && m_mLowZ < jet.m()*0.001 && jet.m()*0.001 < m_mHighZ ) {
    return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::Zqq);
  }
  else {
    if ( isMatchTop ) {
      return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::other_From_t); 
    }
    else {
      return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::other_From_V);
    }
  }

  return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::qcd);
}

StatusCode JetTruthLabelingTool::modify(xAOD::JetContainer& jets) const {

  // Apply label to truth jet collections
  if(m_isTruthJetCol) ATH_CHECK( labelTruthJets(jets) );

  // Copy label to matched reco jets
  if(!m_isTruthJetCol) {
    ATH_CHECK( labelTruthJets() );
    ATH_CHECK( labelRecoJets(jets) );
  }

  return StatusCode::SUCCESS;
}

StatusCode JetTruthLabelingTool::modifyJet(const xAOD::Jet& jet) const {

  // Apply label to truth jet collections
  ATH_CHECK( labelTruthJets() );

  // Copy label to matched reco jets
  ATH_CHECK( labelRecoJet(jet) );

  return StatusCode::SUCCESS;
}

StatusCode JetTruthLabelingTool::labelRecoJets(xAOD::JetContainer& jets) const {

  // Retrieve appropriate truth jet container
  const xAOD::JetContainer* truthJets=nullptr;
  if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
    ATH_CHECK( evtStore()->retrieve(truthJets, "AntiKt10TruthTrimmedPtFrac5SmallR20Jets") );
  }

  for(xAOD::Jet *jet : jets) {

    ATH_CHECK( labelRecoJet(*jet, truthJets) );

  }

  return StatusCode::SUCCESS;
}

StatusCode JetTruthLabelingTool::labelRecoJet(const xAOD::Jet& jet, const xAOD::JetContainer *truthJets) const {

  // Retrieve appropriate truth jet container
  if(!truthJets) {
    if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
      ATH_CHECK( evtStore()->retrieve(truthJets, "AntiKt10TruthTrimmedPtFrac5SmallR20Jets") );
    }
  }

  // Find matched truth jet
  float dRmin = 9999;
  const xAOD::Jet* matchTruthJet = nullptr;
  for ( const xAOD::Jet* truthJet : *truthJets ) {
    float dR = jet.p4().DeltaR( truthJet->p4() );
    if ( m_dRTruthJet < 0 || dR < m_dRTruthJet ) { // if m_dRTruthJet < 0, the closest truth jet is used as matched jet. Otherwise, only match if dR < m_dRTruthJet
      if( dR < dRmin ){
        dRmin = dR;
        matchTruthJet = truthJet;
      }
    }
  }

  int label = LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::notruth);
  float dR_truthJet_W = 9999;
  float dR_truthJet_Z = 9999;
  float dR_truthJet_Top = 9999;
  float dR_truthJet_H = 9999;
  float truthJetNB = -1;
  float truthJetMass = -9999;

  if ( matchTruthJet ) {
    label = (*m_acc_label)(*matchTruthJet);
    if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
      if(m_acc_dR_W->isAvailable(*matchTruthJet)) dR_truthJet_W = (*m_acc_dR_W)(*matchTruthJet);
      if(m_acc_dR_Z->isAvailable(*matchTruthJet)) dR_truthJet_Z = (*m_acc_dR_Z)(*matchTruthJet);
      if(m_acc_dR_H->isAvailable(*matchTruthJet)) dR_truthJet_H = (*m_acc_dR_H)(*matchTruthJet);
      if(m_acc_dR_Top->isAvailable(*matchTruthJet)) dR_truthJet_Top = (*m_acc_dR_Top)(*matchTruthJet);
      if(m_acc_NB->isAvailable(*matchTruthJet)) truthJetNB = (*m_acc_NB)(*matchTruthJet);
      truthJetMass = matchTruthJet->m();
    }
  }

  // decorate truth label
  (*m_dec_label)(jet) = label;

  // decorate additional information used for truth labeling
  if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
    // These most likely won't be used in the R21Precision labeling definition
    (*m_dec_dR_W)(jet) = dR_truthJet_W;
    (*m_dec_dR_Z)(jet) = dR_truthJet_Z;
    (*m_dec_dR_H)(jet) = dR_truthJet_H;
    (*m_dec_dR_Top)(jet) = dR_truthJet_Top;
    (*m_dec_NB)(jet) = truthJetNB;
    (*m_dec_TruthJetMass)(jet) = truthJetMass;
  }

  return StatusCode::SUCCESS;
}

StatusCode JetTruthLabelingTool::labelTruthJets() const {

  // Retrieve appropriate truth jet container
  const xAOD::JetContainer* truthJets = nullptr;
  if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
    ATH_CHECK( evtStore()->retrieve(truthJets, "AntiKt10TruthTrimmedPtFrac5SmallR20Jets") );
  }

  // Make sure the truth jet collection has been retrieved
  if(!truthJets) {
    ATH_MSG_ERROR("No truth jet container retrieved. Please make sure you are using a supported TruthLabelName.");
    return StatusCode::FAILURE;
  }

  return labelTruthJets(*truthJets);

}

StatusCode JetTruthLabelingTool::labelTruthJets(const xAOD::JetContainer &truthJets) const {

  // Make sure there is at least 1 jet in truth collection
  if(!(truthJets.size())) return StatusCode::SUCCESS;

  // Check if the truth jet collection already has labels applied
  const xAOD::Jet *jet = truthJets.at(0);
  if(m_acc_label->isAvailable(*jet)) {
    ATH_MSG_DEBUG("labelTruthJets: Truth jet collection already labelled with " << m_truthLabelName);
    return StatusCode::SUCCESS;
  }

  // Get MC channel number
  int channelNumber = -999;

  //Get the EventInfo to identify Sherpa samples
  const xAOD::EventInfo * eventInfo = nullptr;
  if ( evtStore()->retrieve(eventInfo,"EventInfo").isFailure() || !eventInfo ) {
    ATH_MSG_ERROR("Failed to retrieve event information.");
    return StatusCode::FAILURE;
  }

  channelNumber = eventInfo->mcChannelNumber();

  if( channelNumber < 0 ) {
    ATH_MSG_ERROR("Channel number was not set correctly");
    return StatusCode::FAILURE;
  }

  // Check if it is a Sherpa sample
  bool isSherpa = getIsSherpa(channelNumber);

  if(m_useTRUTH3 && isSherpa) {
    ATH_MSG_ERROR("Cannot apply truth labels to Sherpa 2.2.1 samples using TRUTH3 containers");
    return StatusCode::FAILURE;
  }

  int label = LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::notruth);
  const xAOD::TruthParticleContainer* truthPartsBoson = nullptr;
  const xAOD::TruthParticleContainer* truthPartsTop = nullptr;

  // TRUTH3
  if( m_useTRUTH3 ) {
    // Check for boson container
    if( evtStore()->contains<xAOD::TruthParticleContainer>( m_truthBosonContainerName ) ) ATH_CHECK(evtStore()->retrieve(truthPartsBoson, m_truthBosonContainerName));
    else {
      ATH_MSG_ERROR("Unable to find " << m_truthBosonContainerName << ". Please check the content of your input file.");
      return StatusCode::FAILURE;
    }
    
    // Check for top quark container
    if( evtStore()->contains<xAOD::TruthParticleContainer>( m_truthTopQuarkContainerName ) ) ATH_CHECK(evtStore()->retrieve(truthPartsTop, m_truthTopQuarkContainerName));
    else {
      ATH_MSG_ERROR("Unable to find " << m_truthTopQuarkContainerName << ". Please check the content of your input file.");
      return StatusCode::FAILURE;
    }
  }
  
  // TRUTH1
  else {
    if( evtStore()->contains<xAOD::TruthParticleContainer>( m_truthParticleContainerName ) ) {
      ATH_CHECK(evtStore()->retrieve(truthPartsBoson, m_truthParticleContainerName));
      ATH_CHECK(evtStore()->retrieve(truthPartsTop, m_truthParticleContainerName));
    }
    else {
      ATH_MSG_ERROR("Unable to find " << m_truthParticleContainerName << ". Please check the content of your input file.");
      return StatusCode::FAILURE;
    }
  }

  // Vectors of W/Z/H/Top TLorentzVectors
  std::vector<std::pair<TLorentzVector,int> > tlv_truthParts;

  // Get truth particle TLVs
  if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
    getTLVs(tlv_truthParts, truthPartsBoson, truthPartsTop, isSherpa);
  }

  // Apply label to truth jet
  for(const xAOD::Jet* jet : truthJets) {

    if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
      label = getTruthJetLabelR21Consolidated(*jet, tlv_truthParts);
    }

    (*m_dec_label)(*jet) = label;
  }

  return StatusCode::SUCCESS;
}

void JetTruthLabelingTool::getTLVs(std::vector<std::pair<TLorentzVector,int> > &tlvs, const xAOD::TruthParticleContainer *truthBosons, const xAOD::TruthParticleContainer *truthTop, bool isSherpa) const {

  tlvs.clear();

  // Sherpa W/Z+jets samples need special treatment
  if(isSherpa) {
    int countStatus3 = 0;
    
    // Decay products
    TLorentzVector p1(0,0,0,0);
    TLorentzVector p2(0,0,0,0);

    // W candidate
    TLorentzVector WZCand(0,0,0,0);

    // Flag for W or Z candidates
    bool isWPCand = false;
    bool isWMCand = false;
    bool isZCand = false;

    // Flag to check if qq' pair is in the mass window
    bool inMassWindow = false;

    for ( unsigned int ipart = 0; ipart < truthBosons->size(); ipart++ ){

      const xAOD::TruthParticle* part1 = truthBosons->at(ipart);

      if ( part1->status() != 3 ) continue; /// Skip particles without status == 3

      if ( std::abs(part1->pdgId()) > 5 ) continue; /// Skip anything that isn't a light quark

      countStatus3++;
      if ( countStatus3 > 3 ) continue; /// We want to look at first 2 partons except beam particles. Sometimes beam particles are dropped from DxAODs...
      p1 = part1->p4(); /// Keep particles as first daughter

      // Find the next particle in the list with status == 3
      for ( unsigned int jpart = ipart+1; jpart < truthBosons->size(); jpart++ ) {

        const xAOD::TruthParticle* part2 = truthBosons->at(jpart);

        if ( part2->status() != 3 ) continue; /// Skip particles without status == 3

        if ( std::abs(part2->pdgId()) > 5 ) continue; /// Skip anything that isn't a light quark

        p2 = part2->p4();
  
        if ( part1->pdgId() + part2->pdgId() == 0 ) {
          isZCand = true; /// Daughters of Z decay should be same-flavor but opposite charge
        }
        else if ( part1->pdgId() == 2 || part1->pdgId() == 4 || part2->pdgId() == 2 || part2->pdgId() == 4 ) {
          isWPCand = true; /// W+ daughters should have a positive u or c
        }
        else {
          isWMCand = true; /// W+ daughters should have a positive u or c
        }
        
        break; /// If p1 is a daughter of W/Z decay, the next one is the other daughter
      }

      WZCand = p1 + p2;

      if ( 60000 < WZCand.M() && WZCand.M() < 140000. ) {
        inMassWindow = true;
        break; /// ~98% efficiency to W/Z signals. (95% if it is changed to [65, 105]GeV and 90% if [70,100]GeV)
      }

    }

    if( inMassWindow && (isWPCand || isWMCand || isZCand) ) {
      std::pair<TLorentzVector,int> WZ;
      if(isZCand) {
        WZ = std::make_pair(WZCand,23);
      }
      if(isWPCand) {
        WZ = std::make_pair(WZCand,24);
      }
      if(isWMCand) {
        WZ = std::make_pair(WZCand,-24);
      }
      tlvs.push_back(WZ);
    }

  }

  // Store W/Z/H bosons
  for( const xAOD::TruthParticle* part : *truthBosons ){
    if(!(selectTruthParticle(part,23) || selectTruthParticle(part,24) || selectTruthParticle(part,25))) continue;
    tlvs.push_back(std::make_pair(part->p4(),part->pdgId())); /// Save 4-vector and pdgId
  }

  // Store top quarks
  for( const xAOD::TruthParticle* part : *truthTop ){
    if(!selectTruthParticle(part,6)) continue;
    tlvs.push_back(std::make_pair(part->p4(),part->pdgId())); /// Save 4-vector and pdgId
  }

}

bool JetTruthLabelingTool::selectTruthParticle(const xAOD::TruthParticle *tp, int pdgId) const {
  if(std::abs(tp->pdgId())!=pdgId) return false;
  for(unsigned int iChild = 0; iChild < tp->nChildren(); iChild++) {
    const xAOD::TruthParticle *child = tp->child(iChild);
    if(!child) continue;
    if(child->pdgId() == tp->pdgId()) return false;
  }
  return true;
}
