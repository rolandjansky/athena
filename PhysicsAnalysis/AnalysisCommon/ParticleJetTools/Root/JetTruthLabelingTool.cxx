/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/JetTruthLabelingTool.h"

#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/ReadDecorHandle.h"
#include "AsgDataHandles/WriteDecorHandle.h"

JetTruthLabelingTool::JetTruthLabelingTool(const std::string& name) :
  asg::AsgTool(name)
{
    
    /// Hard-code some values for R10TruthLabel_R21Consolidated
    /// Functionality to customize labeling will be added later
    if(m_truthLabelName == "R10TruthLabel_R21Consolidated") {
      m_truthJetCollectionName="AntiKt10TruthTrimmedPtFrac5SmallR20Jets";
      m_useDRMatch = true;
      m_dRTruthJet = 0.75;
      m_dRTruthPart = 0.75;
      m_mLowTop = 140.0;
      m_mLowW = 50.0;
      m_mHighW = 100.0;
      m_mLowZ = 60.0;
      m_mHighZ = 110.0;
    }

    /// Hard-code some values for R10TruthLabel_R21Precision
    if(m_truthLabelName == "R10TruthLabel_R21Precision") {
      m_truthJetCollectionName="AntiKt10TruthJets";
      m_useDRMatch = false;
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

  /// Check if TruthLabelName is supported. If not, give an error and return FAILURE
  
  bool isSupportedLabel = false;
  isSupportedLabel = isSupportedLabel || (m_truthLabelName=="R10TruthLabel_R21Consolidated");
  isSupportedLabel = isSupportedLabel || (m_truthLabelName=="R10TruthLabel_R21Precision");

  if(!isSupportedLabel) {
    ATH_MSG_ERROR("TruthLabelName " << m_truthLabelName << " is not supported. Exiting...");
    return StatusCode::FAILURE;
  }

  m_label_truthKey   = m_truthJetCollectionName.key() + "." + m_truthLabelName;
  m_dR_W_truthKey    = m_truthJetCollectionName.key() + "." + m_truthLabelName + "_dR_W";
  m_dR_Z_truthKey    = m_truthJetCollectionName.key() + "." + m_truthLabelName + "_dR_Z";
  m_dR_H_truthKey    = m_truthJetCollectionName.key() + "." + m_truthLabelName + "_dR_H";
  m_dR_Top_truthKey  = m_truthJetCollectionName.key() + "." + m_truthLabelName + "_dR_Top";
  m_NB_truthKey      = m_truthJetCollectionName.key() + "." + m_truthLabelName + "_NB";
  m_split23_truthKey = m_truthJetCollectionName.key() + ".Split23";

  if(!m_isTruthJetCol){
    m_label_recoKey  = m_jetContainerName + "." + m_truthLabelName;
    m_dR_W_recoKey   = m_jetContainerName + "." + m_truthLabelName + "_dR_W";
    m_dR_Z_recoKey   = m_jetContainerName + "." + m_truthLabelName + "_dR_Z";
    m_dR_H_recoKey   = m_jetContainerName + "." + m_truthLabelName + "_dR_H";
    m_dR_Top_recoKey = m_jetContainerName + "." + m_truthLabelName + "_dR_Top";
    m_NB_recoKey     = m_jetContainerName + "." + m_truthLabelName + "_NB";
    m_truthSplit23_recoKey = m_jetContainerName + "." + m_truthLabelName + "_TruthJetSplit23";
    m_truthJetMass_recoKey = m_jetContainerName + "." + m_truthLabelName + "_TruthJetMass";
  }

  ATH_CHECK(m_evtInfoKey.initialize());
  ATH_CHECK(m_truthParticleContainerName.initialize(!m_useTRUTH3));
  ATH_CHECK(m_truthBosonContainerName.initialize(m_useTRUTH3));
  ATH_CHECK(m_truthTopQuarkContainerName.initialize(m_useTRUTH3));
  ATH_CHECK(m_truthJetCollectionName.initialize());

  ATH_CHECK(m_label_truthKey.initialize());
  ATH_CHECK(m_dR_W_truthKey.initialize(m_useDRMatch));
  ATH_CHECK(m_dR_Z_truthKey.initialize(m_useDRMatch));
  ATH_CHECK(m_dR_H_truthKey.initialize(m_useDRMatch));
  ATH_CHECK(m_dR_Top_truthKey.initialize(m_useDRMatch));
  ATH_CHECK(m_NB_truthKey.initialize());
  ATH_CHECK(m_split23_truthKey.initialize(m_truthLabelName == "R10TruthLabel_R21Precision"));

  ATH_CHECK(m_label_recoKey.initialize(!m_isTruthJetCol));
  ATH_CHECK(m_dR_W_recoKey.initialize(!m_isTruthJetCol && m_useDRMatch));
  ATH_CHECK(m_dR_Z_recoKey.initialize(!m_isTruthJetCol && m_useDRMatch));
  ATH_CHECK(m_dR_H_recoKey.initialize(!m_isTruthJetCol && m_useDRMatch));
  ATH_CHECK(m_dR_Top_recoKey.initialize(!m_isTruthJetCol && m_useDRMatch));
  ATH_CHECK(m_NB_recoKey.initialize(!m_isTruthJetCol));
  ATH_CHECK(m_truthSplit23_recoKey.initialize(!m_isTruthJetCol && m_truthLabelName == "R10TruthLabel_R21Precision"));
  ATH_CHECK(m_truthJetMass_recoKey.initialize(!m_isTruthJetCol));

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
  
  ATH_MSG_INFO("TruthJetCollectionName:         " << m_truthJetCollectionName.key());
  ATH_MSG_INFO("dRTruthJet:    " << std::to_string(m_dRTruthJet));

  if ( m_useDRMatch ) {
    ATH_MSG_INFO("dRTruthPart:   " << std::to_string(m_dRTruthPart));
  }

  ATH_MSG_INFO("mLowTop:       " << std::to_string(m_mLowTop));
  ATH_MSG_INFO("mLowW:         " << std::to_string(m_mLowW));
  ATH_MSG_INFO("mHighW:        " << std::to_string(m_mHighW));
  ATH_MSG_INFO("mLowZ:         " << std::to_string(m_mLowZ));
  ATH_MSG_INFO("mHighZ:        " << std::to_string(m_mHighZ));

}

int JetTruthLabelingTool::getTruthJetLabelDR( const xAOD::Jet &jet, std::vector<std::pair<TLorentzVector,int> > tlv_truthParts ) const {

  /// Booleans to check associated heavy particles
  bool matchW = false;
  bool matchZ = false;
  bool matchH = false;
  bool matchTop = false;

  /// Distances to truth particles
  float dR_W = 9999;
  float dR_Z = 9999;
  float dR_H = 9999;
  float dR_Top = 9999;

  for (auto tlv_truth : tlv_truthParts) {
    float dR = tlv_truth.first.DeltaR(jet.p4());
    if( dR < m_dRTruthPart ) {

      if ( std::abs(tlv_truth.second) == 23 && !matchZ ) {
        dR_Z = dR;
        matchZ = true;
      }

      if ( std::abs(tlv_truth.second) == 24 && !matchW ) {
        dR_W = dR;
        matchW = true;
      }

      if ( std::abs(tlv_truth.second) == 25 && !matchH ) {
        dR_H = dR;
        matchH = true;
      }

      if ( std::abs(tlv_truth.second) == 6 && !matchTop ) {
        dR_Top = dR;
        matchTop = true;
      }

    }
  }

  /// Add matching criteria decorations
  SG::WriteDecorHandle<xAOD::JetContainer, float> dRWHandle(m_dR_W_truthKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> dRZHandle(m_dR_Z_truthKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> dRHHandle(m_dR_H_truthKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> dRTopHandle(m_dR_Top_truthKey);

  dRWHandle(jet) = dR_W;
  dRZHandle(jet) = dR_Z;
  dRHHandle(jet) = dR_H;
  dRTopHandle(jet) = dR_Top;

  return getLabel( jet, matchH, matchW, matchZ, matchTop );

}

int JetTruthLabelingTool::getTruthJetLabelGA( const xAOD::Jet &jet ) const {

  /// Booleans to check ghost-associated heavy particles
  bool matchW = false;
  bool matchZ = false;
  bool matchH = false;
  bool matchTop = false;

  /// Find ghost associated W bosons
  int nMatchW = getNGhostParticles( jet, "GhostWBosons" );

  if ( nMatchW ) {
    matchW = true;
  }

  /// Find ghost associated Z bosons
  int nMatchZ = getNGhostParticles( jet, "GhostZBosons" );
  
  if ( nMatchZ ) {
    matchZ = true;
  }

  /// Find ghost associated H bosons
  int nMatchH = getNGhostParticles( jet, "GhostHBosons" );
  
  if ( nMatchH ) {
    matchH = true;
  }

  /// Find ghost associated top quarks
  int nMatchTop = getNGhostParticles( jet, "GhostTQuarksFinal" );

  if ( nMatchTop ) {
    matchTop = true;
  }

  return getLabel( jet, matchH, matchW, matchZ, matchTop );

}

StatusCode JetTruthLabelingTool::decorate(const xAOD::JetContainer& jets) const {

  /// Apply label to truth jet collections
  if(m_isTruthJetCol) {
    return labelTruthJets(jets);
  }

  /// Copy label to matched reco jets
  else {
    ATH_CHECK( labelTruthJets() );
    return labelRecoJets(jets);
  }

  return StatusCode::SUCCESS;
}

StatusCode JetTruthLabelingTool::labelRecoJets(const xAOD::JetContainer& jets ) const {

  SG::ReadHandle<xAOD::JetContainer> truthJets(m_truthJetCollectionName);
  for(const xAOD::Jet *jet : jets) {

    /// Get parent ungroomed reco jet for R21Precision
    const xAOD::Jet* parent = nullptr;
    if ( m_truthLabelName == "R10TruthLabel_R21Precision" ) {
      ElementLink<xAOD::JetContainer> element_link = jet->auxdata<ElementLink<xAOD::JetContainer> >("Parent");
      if ( element_link.isValid() ) {
        parent = *element_link;
      }
      else {
        ATH_MSG_ERROR("Unable to get a link to the parent jet! Returning a NULL pointer."); 
        return StatusCode::FAILURE;
      }
    }

    /// Find matched truth jet
    float dRmin = 9999;
    const xAOD::Jet* matchTruthJet = nullptr;
    for ( const xAOD::Jet* truthJet : *truthJets ) {
      float dR = jet->p4().DeltaR( truthJet->p4() );
      /// If parent jet has been retrieved, calculate dR w.r.t. it instead
      if ( parent ) dR = parent->p4().DeltaR( truthJet->p4() );
      /// If m_dRTruthJet < 0, the closest truth jet is used as matched jet. Otherwise, only match if dR < m_dRTruthJet
      if ( m_dRTruthJet < 0 || dR < m_dRTruthJet ) { 
        if ( dR < dRmin ) {
          dRmin = dR;
          matchTruthJet = truthJet;
        }
      }
    }

    int label = LargeRJetTruthLabel::enumToInt( LargeRJetTruthLabel::notruth );
    float dR_truthJet_W = 9999;
    float dR_truthJet_Z = 9999;
    float dR_truthJet_Top = 9999;
    float dR_truthJet_H = 9999;
    int truthJetNB = -1;
    float truthJetSplit23 = -9999;
    float truthJetMass = -9999;

    if ( matchTruthJet ) {
      // WriteDecorHandles can also read
      SG::WriteDecorHandle<xAOD::JetContainer, int> labelHandle(m_label_truthKey);
      label = labelHandle(*matchTruthJet);
      if ( m_useDRMatch ) {
        SG::WriteDecorHandle<xAOD::JetContainer, float> dRWHandle(m_dR_W_truthKey);
        SG::WriteDecorHandle<xAOD::JetContainer, float> dRZHandle(m_dR_Z_truthKey);
        SG::WriteDecorHandle<xAOD::JetContainer, float> dRHHandle(m_dR_H_truthKey);
        SG::WriteDecorHandle<xAOD::JetContainer, float> dRTopHandle(m_dR_Top_truthKey);
        if(dRWHandle.isAvailable()) dR_truthJet_W = dRWHandle(*matchTruthJet);
        if(dRZHandle.isAvailable()) dR_truthJet_Z = dRZHandle(*matchTruthJet);
        if(dRHHandle.isAvailable()) dR_truthJet_H = dRHHandle(*matchTruthJet);
        if(dRTopHandle.isAvailable()) dR_truthJet_Top = dRTopHandle(*matchTruthJet);
      }
      if ( m_truthLabelName == "R10TruthLabel_R21Precision" ) {
        SG::ReadDecorHandle<xAOD::JetContainer, float> split23Handle(m_split23_truthKey);
        if(split23Handle.isAvailable()) truthJetSplit23 = split23Handle(*matchTruthJet);
      }
      SG::WriteDecorHandle<xAOD::JetContainer, int> nbHandle(m_NB_truthKey);
      if(nbHandle.isAvailable()) truthJetNB = nbHandle(*matchTruthJet);
      truthJetMass = matchTruthJet->m();
    }

    /// Decorate truth label
    SG::WriteDecorHandle<xAOD::JetContainer, int> labelHandle(m_label_recoKey);
    labelHandle(*jet) = label;

    /// Decorate additional information used for truth labeling
    if ( m_useDRMatch ) {
      SG::WriteDecorHandle<xAOD::JetContainer, float> dRWHandle(m_dR_W_recoKey);
      SG::WriteDecorHandle<xAOD::JetContainer, float> dRZHandle(m_dR_Z_recoKey);
      SG::WriteDecorHandle<xAOD::JetContainer, float> dRHHandle(m_dR_H_recoKey);
      SG::WriteDecorHandle<xAOD::JetContainer, float> dRTopHandle(m_dR_Top_recoKey);
      dRWHandle(*jet) = dR_truthJet_W;
      dRZHandle(*jet) = dR_truthJet_Z;
      dRHHandle(*jet) = dR_truthJet_H;
      dRTopHandle(*jet) = dR_truthJet_Top;
    }
    if ( m_truthLabelName == "R10TruthLabel_R21Precision" ) {
      SG::WriteDecorHandle<xAOD::JetContainer, float> split23Handle(m_truthSplit23_recoKey);
      split23Handle(*jet) = truthJetSplit23;
    }
    SG::WriteDecorHandle<xAOD::JetContainer, int> nbHandle(m_NB_recoKey);
    SG::WriteDecorHandle<xAOD::JetContainer, float> truthMassHandle(m_truthJetMass_recoKey);
    nbHandle(*jet) = truthJetNB;
    truthMassHandle(*jet) = truthJetMass;
  }

  return StatusCode::SUCCESS;
}

StatusCode JetTruthLabelingTool::labelTruthJets() const {

  /// Retrieve appropriate truth jet container
  SG::ReadHandle<xAOD::JetContainer> truthJets(m_truthJetCollectionName);

  /// Make sure the truth jet collection has been retrieved
  if ( !truthJets.isValid() ) {
    ATH_MSG_ERROR("No truth jet container retrieved. Please make sure you are using a supported TruthLabelName.");
    return StatusCode::FAILURE;
  }

  return labelTruthJets(*truthJets);

}

StatusCode JetTruthLabelingTool::labelTruthJets( const xAOD::JetContainer &truthJets ) const {

  /// Make sure there is at least 1 jet in truth collection
  if ( !(truthJets.size()) ) return StatusCode::SUCCESS;

  SG::WriteDecorHandle<xAOD::JetContainer, int> labelHandle(m_label_truthKey);

  /// Check if the truth jet collection already has labels applied
  if(labelHandle.isAvailable()){
    ATH_MSG_DEBUG("labelTruthJets: Truth jet collection already labelled with " << m_truthLabelName);
    return StatusCode::SUCCESS;
  }

  /// Get MC channel number
  int channelNumber = -999;

  /// Get the EventInfo to identify Sherpa samples
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_evtInfoKey);
  if(!eventInfo.isValid()){
    ATH_MSG_ERROR("Failed to retrieve event information.");
    return StatusCode::FAILURE;
  }

  /// Vectors of W/Z/H/Top TLorentzVectors
  std::vector<std::pair<TLorentzVector,int> > tlv_truthParts;

  /// Get truth particles directly if using dR matching
  if ( m_useDRMatch ) {
  
    channelNumber = eventInfo->mcChannelNumber();

    if ( channelNumber < 0 ) {
      ATH_MSG_ERROR("Channel number was not set correctly");
      return StatusCode::FAILURE;
    }

    /// Check if it is a Sherpa sample
    bool isSherpa = getIsSherpa(channelNumber);

    if ( m_useTRUTH3 && isSherpa ) {
      ATH_MSG_ERROR("Cannot apply truth labels to Sherpa 2.2.1 samples using TRUTH3 containers");
      return StatusCode::FAILURE;
    }

    /// TRUTH3
    if( m_useTRUTH3 ) {
      /// Check for boson container
      SG::ReadHandle<xAOD::TruthParticleContainer> truthPartsBoson(m_truthBosonContainerName);
      if(!truthPartsBoson.isValid()){
        ATH_MSG_ERROR("Unable to find " << m_truthBosonContainerName.key() << ". Please check the content of your input file.");
        return StatusCode::FAILURE;
      }
      /// Check for top quark container
      SG::ReadHandle<xAOD::TruthParticleContainer> truthPartsTop(m_truthTopQuarkContainerName);
      if(!truthPartsTop.isValid()){
        ATH_MSG_ERROR("Unable to find " << m_truthTopQuarkContainerName.key() << ". Please check the content of your input file.");
        return StatusCode::FAILURE;
      }
      /// Get truth particle TLVs
      getTLVs(tlv_truthParts, truthPartsBoson.cptr(), truthPartsTop.cptr(), isSherpa);
    }

    /// TRUTH1
    else {
      SG::ReadHandle<xAOD::TruthParticleContainer> truthParts(m_truthParticleContainerName);
      if(!truthParts.isValid()){
        ATH_MSG_ERROR("Unable to find " << m_truthParticleContainerName << ". Please check the content of your input file.");
        return StatusCode::FAILURE;
      }
      /// Get truth particle TLVs
      getTLVs(tlv_truthParts, truthParts.cptr(), truthParts.cptr(), isSherpa);
    }
  }

  int label = LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::notruth);
  /// Apply label to truth jet
  for ( const xAOD::Jet *jet : truthJets ) {

    if ( m_useDRMatch ) {
      ATH_MSG_DEBUG("Getting truth label using dR matching");
      label = getTruthJetLabelDR(*jet, tlv_truthParts);
    }
    
    else {
      ATH_MSG_DEBUG("Getting truth label using ghost-association");
      label = getTruthJetLabelGA(*jet);
    }

    labelHandle(*jet) = label;
  }

  return StatusCode::SUCCESS;
}

void JetTruthLabelingTool::getTLVs( std::vector<std::pair<TLorentzVector,int> > &tlvs, const xAOD::TruthParticleContainer *truthBosons, const xAOD::TruthParticleContainer *truthTop, bool isSherpa ) const {

  tlvs.clear();

  /// Sherpa W/Z+jets samples need special treatment
  if(isSherpa) {
    int countStatus3 = 0;

    /// Decay products
    TLorentzVector p1(0,0,0,0);
    TLorentzVector p2(0,0,0,0);

    /// W candidate
    TLorentzVector WZCand(0,0,0,0);

    /// Flag for W or Z candidates
    bool isWPCand = false;
    bool isWMCand = false;
    bool isZCand = false;

    /// Flag to check if qq' pair is in the mass window
    bool inMassWindow = false;

    for ( unsigned int ipart = 0; ipart < truthBosons->size(); ipart++ ){

      const xAOD::TruthParticle* part1 = truthBosons->at(ipart);

      /// Skip particles without status == 3
      if ( part1->status() != 3 ) continue;

      /// Skip anything that isn't a light quark
      if ( std::abs(part1->pdgId()) > 5 ) continue;

      countStatus3++;
      /// We want to look at first 2 partons except beam particles. Sometimes beam particles are dropped from DxAODs...
      if ( countStatus3 > 3 ) continue;
      /// Keep particles as first daughter
      p1 = part1->p4();

      /// Find the next particle in the list with status == 3
      for ( unsigned int jpart = ipart+1; jpart < truthBosons->size(); jpart++ ) {

        const xAOD::TruthParticle* part2 = truthBosons->at(jpart);

        /// Skip particles without status == 3
        if ( part2->status() != 3 ) continue;

        /// Skip anything that isn't a light quark
        if ( std::abs(part2->pdgId()) > 5 ) continue;

        p2 = part2->p4();

        /// Daughters of Z decay should be same-flavor but opposite charge
        if ( part1->pdgId() + part2->pdgId() == 0 ) {
          isZCand = true;
        }
        /// W+ daughters should have a positive u or c
        else if ( part1->pdgId() == 2 || part1->pdgId() == 4 || part2->pdgId() == 2 || part2->pdgId() == 4 ) {
          isWPCand = true;
        }
        /// W+ daughters should have a positive u or c
        else {
          isWMCand = true;
        }

        /// If p1 is a daughter of W/Z decay, the next one is the other daughter
        break;

      }

      WZCand = p1 + p2;

      /// ~98% efficiency to W/Z signals. (95% if it is changed to [65, 105]GeV and 90% if [70,100]GeV)
      if ( 60000 < WZCand.M() && WZCand.M() < 140000. ) {
        inMassWindow = true;
        break;
      }

    }

    if ( inMassWindow && (isWPCand || isWMCand || isZCand) ) {
      std::pair<TLorentzVector,int> WZ;
      if ( isZCand ) {
        WZ = std::make_pair(WZCand,23);
      }
      if ( isWPCand ) {
        WZ = std::make_pair(WZCand,24);
      }
      if ( isWMCand ) {
        WZ = std::make_pair(WZCand,-24);
      }
      tlvs.push_back(WZ);
    }

  }

  /// Store W/Z/H bosons
  for ( const xAOD::TruthParticle* part : *truthBosons ) {
    if ( !(selectTruthParticle(part,23) || selectTruthParticle(part,24) || selectTruthParticle(part,25)) ) continue;
    /// Save 4-vector and pdgId
    tlvs.push_back(std::make_pair(part->p4(),part->pdgId()));
  }

  /// Store top quarks
  for ( const xAOD::TruthParticle* part : *truthTop ) {
    if ( !selectTruthParticle(part,6) ) continue;
    /// Save 4-vector and pdgId
    tlvs.push_back(std::make_pair(part->p4(),part->pdgId()));
  }

}

bool JetTruthLabelingTool::selectTruthParticle( const xAOD::TruthParticle *tp, int pdgId ) const {
  if ( std::abs(tp->pdgId()) != pdgId ) return false;
  for ( unsigned int iChild = 0; iChild < tp->nChildren(); iChild++ ) {
    const xAOD::TruthParticle *child = tp->child(iChild);
    if ( !child ) continue;
    if ( child->pdgId() == tp->pdgId() ) return false;
  }
  return true;
}

float JetTruthLabelingTool::getTopSplit23CutR21Precision( float pt ) const {

  /// The functional form and parameters come from optimization studies:
  /// https://indico.cern.ch/event/931498/contributions/3921872/attachments/2064188/3463746/JSS_25June.pdf

  const float c0 = 3.3;
  const float c1 = -6.98e-4;

  float split23 = -1.0;

  split23 = std::exp( c0 + c1 * pt );

  return split23;
}

int JetTruthLabelingTool::getNGhostParticles( const xAOD::Jet &jet, std::string collection ) const {

  int nMatchPart = 0;

  if( !jet.getAttribute<int>( collection+"Count", nMatchPart ) ){

    std::vector<const xAOD::TruthParticle*> ghostParts;
    if( !jet.getAssociatedObjects<xAOD::TruthParticle>( collection, ghostParts ) ){      
      ATH_MSG_ERROR( collection + " cannot be retrieved! Truth label definition might be wrong" );
    } 
    nMatchPart = ghostParts.size();
  }

  return nMatchPart;
}

int JetTruthLabelingTool::getLabel( const xAOD::Jet &jet, bool matchH, bool matchW, bool matchZ, bool matchTop ) const {

  /// Find ghost associated B-hadrons
  int nMatchB = getNGhostParticles( jet, "GhostBHadronsFinal" );
  SG::WriteDecorHandle<xAOD::JetContainer, int> nbHandle(m_NB_truthKey);
  nbHandle(jet) = nMatchB;

  /// Booleans for containment selections
  bool isHbb = false;
  bool isTop = false;
  bool isW = false;
  bool isZ = false;

  /// Use R21Consolidated definition
  if ( m_truthLabelName == "R10TruthLabel_R21Consolidated" ) {
    isHbb = ( nMatchB > 1 );
    isTop = ( matchTop && nMatchB > 0 && jet.m() / 1000. > m_mLowTop );
    isW = matchW && nMatchB == 0 && jet.m() / 1000. > m_mLowW && jet.m() / 1000. < m_mHighW;
    isZ = matchZ && jet.m() / 1000. > m_mLowZ && jet.m() / 1000. < m_mHighZ;
  }

  /// Use R21Precision definition
  if ( m_truthLabelName == "R10TruthLabel_R21Precision" ) {
    SG::ReadDecorHandle<xAOD::JetContainer, float> split23Handle(m_split23_truthKey);
    isHbb = ( nMatchB > 1 );
    isTop = ( matchTop && matchW && nMatchB > 0 && jet.m() / 1000. > m_mLowTop && split23Handle(jet) / 1000. > getTopSplit23CutR21Precision( jet.pt() / 1000. ) );
    isW = matchW && nMatchB == 0 && jet.m() / 1000. > m_mLowW && jet.m() / 1000. < m_mHighW;
    isZ = matchZ && jet.m() / 1000. > m_mLowZ && jet.m() / 1000. < m_mHighZ;
  }

  /// This method can be expanded to include custom label priorities

  /* The default priority of labels is:
   * 1) H
   * 2) Contained top
   * 3) Contained W
   * 4) Contained Z
   * 4) Uncontained top
   * 5) Uncontained V
   */

  /// If it isn't matched to any heavy particles, is it QCD
  if( !(matchTop || matchW || matchZ || matchH) ) {
    return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::qcd);
  }

  if ( matchH ) {
    /// Contained H->bb
    if ( isHbb ) return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::Hbb);
    /// Other from H
    return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::other_From_H);
  }

  /// Contained top
  if ( isTop ) return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::tqqb); 

  if ( isW ) {
    /// Contained W from a top
    if ( matchTop ) return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::Wqq_From_t);
    /// Contained W not from a top
    return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::Wqq);
  }

  /// Contained Z
  if ( isZ ) return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::Zqq);

  /// Uncontained top
  if ( matchTop ) return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::other_From_t); 

  /// Uncontained V
  return LargeRJetTruthLabel::enumToInt(LargeRJetTruthLabel::other_From_V);

}
