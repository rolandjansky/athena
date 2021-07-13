/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEJETTOOLS_JETTRUTHLABELINGTOOL_H
#define PARTICLEJETTOOLS_JETTRUTHLABELINGTOOL_H

#include "AsgTools/AsgTool.h"

#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetInterface/IJetDecorator.h"
#include "ParticleJetTools/LargeRJetLabelEnum.h"

class JetTruthLabelingTool :
  public asg::AsgTool,
  virtual public IJetDecorator
{
  ASG_TOOL_CLASS(JetTruthLabelingTool, IJetDecorator)

public:

  /// default constructor - to be used in all derived classes
  JetTruthLabelingTool(const std::string& name = "JetTruthLabelingTool");
  virtual StatusCode initialize() override;

  /// decorate truth label to a jet collection
  StatusCode decorate(const xAOD::JetContainer& jets) const override;

  /// Print configured parameters
  void print() const override;
  
  /// returns the name of large-R jet truth label
  std::string getLargeRJetTruthLabelName() const {
    return m_truthLabelName;
  };

protected:
  
  Gaudi::Property<std::string> m_jetContainerName{this, "RecoJetContainer", "", "Input reco jet container name"};
  Gaudi::Property<std::string> m_truthLabelName{this, "TruthLabelName", "R10TruthLabel_R21Consolidated", "Truth label name"};

  Gaudi::Property<bool> m_isTruthJetCol{this, "IsTruthJetCollection", false, "Flag indicating whether input collection is a truth jet container"};

  SG::ReadHandleKey<xAOD::EventInfo> m_evtInfoKey{this, "EventInfoKey", "EventInfo", "Name of EventInfo object"};

  /// TRUTH1 or TRUTH3
  Gaudi::Property<bool> m_useTRUTH3{this, "UseTRUTH3", false, "True for TRUTH3 format, false for TRUTH1"};
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleContainerName{this, "TruthParticleContainerName", "TruthParticles", "Truth particle container name"};
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthBosonContainerName{this, "TruthBosonContainerName", "TruthBosonsWithDecayParticles", "Truth boson container name"};
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthTopQuarkContainerName{this, "TruthTopQuarkContainerName", "TruthTopQuarkWithDecayParticles", "Truth top container name"};

  /// parameters for truth labeling
  SG::ReadHandleKey<xAOD::JetContainer> m_truthJetCollectionName{this, "TruthJetContainer", "", "Do not configure manually!"};
  bool m_useDRMatch;
  double m_dRTruthJet;
  double m_dRTruthPart;
  double m_mLowTop;
  double m_mLowW;
  double m_mHighW;
  double m_mLowZ;
  double m_mHighZ;

  /// Label truth jet collection
  StatusCode labelTruthJets() const;
  StatusCode labelTruthJets( const xAOD::JetContainer &jets ) const;

  /// Apply label to a single jet
  /// This method is included for backwards compatibility with BoostedJetTaggers
  StatusCode labelRecoJet(const xAOD::Jet &jet) const;

  /// Apply labels to all jets in a container
  StatusCode labelRecoJets(const xAOD::JetContainer &jets ) const;

  /// Get truth label using dR-matched particles
  int getTruthJetLabelDR( const xAOD::Jet &jet, std::vector<std::pair<TLorentzVector,int> > tlv_truthParts ) const;
  
  /// Get truth label using ghost-associated particles
  int getTruthJetLabelGA( const xAOD::Jet &jet ) const;

  /// Get label based on matching and containment criteria
  int getLabel( const xAOD::Jet &jet, bool matchH, bool matchW, bool matchZ, bool matchTop ) const;

  /// Get R21Precision top label Split23 cut
  float getTopSplit23CutR21Precision( float pt ) const;

  /// Get number of ghost associated particles
  int getNGhostParticles( const xAOD::Jet &jet, std::string collection ) const;

  /// Check for Sherpa DSIDs
  bool getIsSherpa( const int DSID ) const {
    if(
        (304307 <= DSID && DSID <= 304309) || // Sherpa 2.2.1 W+jets
        (304707 <= DSID && DSID <= 304709) // Sherpa 2.2.1 Z+jets
	) { 
      return true;
    }
    return false;
  };

  /// Extract heavy particle 4-vectors from truth record
  void getTLVs( std::vector< std::pair<TLorentzVector,int> > &tlvs, const xAOD::TruthParticleContainer *truthBosons, const xAOD::TruthParticleContainer *truthTop, bool isSherpa = false ) const;

  /// Check if truth particle has correct DSID and isn't self decay
  bool selectTruthParticle( const xAOD::TruthParticle *tp, int pdgId ) const;

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_label_truthKey{this, "label_TruthKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_dR_W_truthKey{this, "dR_W_TruthKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_dR_Z_truthKey{this, "dR_Z_TruthKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_dR_H_truthKey{this, "dR_H_TruthKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_dR_Top_truthKey{this, "dR_Top_TruthKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_NB_truthKey{this, "NB_TruthKey", "", "Do not configure manually!"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_split23_truthKey{this, "Split23_TruthKey", "", "Do not configure manually!"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_label_recoKey{this, "label_RecoKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_dR_W_recoKey{this, "dR_W_RecoKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_dR_Z_recoKey{this, "dR_Z_RecoKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_dR_H_recoKey{this, "dR_H_RecoKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_dR_Top_recoKey{this, "dR_Top_RecoKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_NB_recoKey{this, "NB_RecoKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthSplit23_recoKey{this, "TruthSplit23_RecoKey", "", "Do not configure manually!"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthJetMass_recoKey{this, "TruthJetMass_RecoKey", "", "Do not configure manually!"};
};

#endif
