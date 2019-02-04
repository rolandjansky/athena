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
  bool getIsSherpa(const int DSID) const {
    if( (364100 <= DSID && DSID <=364193) || // Sherpa 2.2.1 W/Z+jets
	(308092 <= DSID && DSID <=309098) || // Sherpa 2.2.1 W/Z+jets
	(344773 <= DSID && DSID <=309095)  // Sherpa 2.2.1 Z+jets
	){ // to be updated
      return true;
    }
    return false;
  };
};

#endif
