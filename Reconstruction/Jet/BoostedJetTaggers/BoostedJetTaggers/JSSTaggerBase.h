// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_JSSTAGGERBase_H_
#define BOOSTEDJETSTAGGERS_JSSTAGGERBase_H_


#include "AsgTools/AsgTool.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "JetInterface/IJetSelector.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

#include "PATCore/TAccept.h"

#include <stdio.h>
#include <stdlib.h>

enum class WTopLabel : int {t = 1, W = 2, Z = 3, b = 4, other = 5, notruth = 6, unknown = -1};
const WTopLabel WTopLabel_types [] = {WTopLabel::t, WTopLabel::W, WTopLabel::Z, WTopLabel::b, WTopLabel::other, WTopLabel::notruth, WTopLabel::unknown, WTopLabel::Z};
  
class JSSTaggerBase :   public asg::AsgTool ,
			virtual public IJetSelectorTool,
			virtual public IJetSelector {
  ASG_TOOL_CLASS2(JSSTaggerBase, IJetSelectorTool, IJetSelector )
  
  protected:
  
  // the object where you store the contents of what the jet has passed
  mutable Root::TAccept m_accept;
  
  // the kinematic bounds for the jet - these are in MeV (not GeV!)
  float m_jetPtMin;
  float m_jetPtMax;
  float m_jetEtaMax;

  // whether you will decorate information onto the jet
  bool m_decorate;

   // the location where CVMFS files live
  std::string m_calibarea;

  // flag to calculate scale factor
  bool m_calcSF;
  int m_DSID;
  // truth label
  std::string m_truthLabelDecorationName;
  // TRUTH1 or TRUTH3
  std::string m_truthJetContainerName;
  std::string m_truthParticleContainerName;
  std::string m_truthWBosonContainerName;
  std::string m_truthZBosonContainerName;
  std::string m_truthTopQuarkContainerName;

   // default constructor - to be used in all derived classes
  JSSTaggerBase(const std::string &name);

  // provide a default implementation for IJetSelector::keep :
  virtual int keep(const xAOD::Jet& jet) const {
    return tag(jet);
  }

  // show the user what cuts are configured
  void showCuts() const;

  public:
  WTopLabel getWTopContainment(const xAOD::Jet& jet, const xAOD::TruthParticleContainer* truthPartsW, const xAOD::TruthParticleContainer* truthPartsZ, const xAOD::TruthParticleContainer* truthPartsTop, bool isSherpa, double dRmax, double mLowTop, double mHighTop, double mLowW, double mHighW, double mLowZ, double mHighZ) const;
  int matchToWZ_Sherpa(const xAOD::Jet& jet,
			const xAOD::TruthParticleContainer* truthParts,
			double dRmax) const;
  void DecorateMatchedTruthJet(const xAOD::Jet& jet,
			       const xAOD::JetContainer* truthJets,
			       double dRmax,
			       std::string decorName) const;
  virtual StatusCode decorateTruthLabel(const xAOD::Jet& jet, std::string decorName="WTopContainmentTruthLabel", double dR_truthJet=0.75, double dR_truthPart=0.75, double mLowTop=140, double mHighTop=200, double mLowW=50, double mHighW=100, double mLowZ=60, double mHighZ=110) const;
  virtual StatusCode decorateTruthLabel(const xAOD::Jet& jet, const xAOD::TruthParticleContainer* truthPartsW, const xAOD::TruthParticleContainer* truthPartsZ, const xAOD::TruthParticleContainer* truthPartsTop, const xAOD::JetContainer* truthJets, std::string decorName, double dR_truthJet, double dR_truthPart, double mLowTop, double mHighTop, double mLowW, double mHighW, double mLowZ, double mHighZ) const;
  bool getIsSherpa(const int DSID) const {
    if( (304307 <= DSID && DSID <=304309) || // Sherpa 2.2.1 W+jets
	(304707 <= DSID && DSID <=304709) || // Sherpa 2.2.1 Z+jets
	(364375 <= DSID && DSID <=364380)  // Sherpa 2.2.5 W/Z+jets
	){ // to be updated
      return true;
    }
    return false;
  };
};

#endif
