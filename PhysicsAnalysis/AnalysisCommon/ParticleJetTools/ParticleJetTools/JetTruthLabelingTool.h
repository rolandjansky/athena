/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEJETTOOLS_JETTRUTHLABELINGTOOL_H
#define PARTICLEJETTOOLS_JETTRUTHLABELINGTOOL_H

#include "AsgTools/AsgTool.h"

#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetInterface/IJetModifier.h"
#include "ParticleJetTools/LargeRJetLabelEnum.h"

class JetTruthLabelingTool :   virtual public asg::AsgTool, 
			       virtual public IJetModifier
{
  ASG_TOOL_CLASS(JetTruthLabelingTool, IJetModifier)

public:

  /// default constructor - to be used in all derived classes
  JetTruthLabelingTool(const std::string& name = "JetTruthLabelingTool");
  virtual StatusCode initialize() override;

  /// decorate truth label to a jet collection
  int modify(xAOD::JetContainer& jets) const override;

  /// decorate truth label to a const jet
  StatusCode modifyJet(const xAOD::Jet& jet) const;

  /// Print configured parameters
  void print() const override;
  
  /// returns the name of large-R jet truth label
  std::string getLargeRJetTruthLabelName() const {
    return m_truthLabelName;
  };

protected:
  
  /// truth label name
  std::string m_truthLabelName;

  /// Flag indicating whether input collection is a truth jet container
  bool m_isTruthJetCol;

  /// TRUTH1 or TRUTH3
  bool m_useTRUTH3;
  std::string m_truthParticleContainerName;
  std::string m_truthBosonContainerName;
  std::string m_truthTopQuarkContainerName;

  /// parameters for truth labeling
  std::string m_truthJetCollectionName;
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
  int labelRecoJet( const xAOD::Jet &jet, const xAOD::JetContainer *truthJets = nullptr ) const;

  /// Apply labels to all jets in a container
  int labelRecoJets( xAOD::JetContainer &jets ) const;

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

  /// Accessors and decorators
  std::unique_ptr< SG::AuxElement::Accessor<int> > acc_label;
  std::unique_ptr< SG::AuxElement::Accessor<float> > acc_dR_W;
  std::unique_ptr< SG::AuxElement::Accessor<float> > acc_dR_Z;
  std::unique_ptr< SG::AuxElement::Accessor<float> > acc_dR_H;
  std::unique_ptr< SG::AuxElement::Accessor<float> > acc_dR_Top;
  std::unique_ptr< SG::AuxElement::Accessor<int> > acc_NB;
  std::unique_ptr< SG::AuxElement::Accessor<float> > acc_Split23;

  std::unique_ptr< SG::AuxElement::Decorator<int> > dec_label;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_dR_W;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_dR_Z;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_dR_H;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_dR_Top;
  std::unique_ptr< SG::AuxElement::Decorator<int> > dec_NB;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_TruthJetMass;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_TruthJetSplit23;

};

#endif
