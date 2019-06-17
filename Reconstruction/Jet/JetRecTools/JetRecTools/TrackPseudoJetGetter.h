// emacs : this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_TRACKPSEUDOJETGETTER_H
#define JETRECTOOLS_TRACKPSEUDOJETGETTER_H
////////////////////////////////////////////
/// \class TrackPseudoJetGetter
///
/// Specialized PseudoJetGetter for TrackParticle
///  The only thing done here is to specialize the templated append method and 
///  define a property for the TrackVertexAssociation
/// \author P-A Delsart
//////////////////////////////////////////////////

#include "JetRec/PseudoJetGetter.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "JetRec/PseudoJetContainer.h"

namespace jet{
  class TrackVertexAssociation;
}

class TrackPseudoJetGetter : public PseudoJetGetter {
  ASG_TOOL_CLASS(TrackPseudoJetGetter, IPseudoJetGetter)
public:
  TrackPseudoJetGetter(const std::string &name);

  virtual StatusCode initialize() override;

  /// Method to construct the PseudoJetVector and record in StoreGate
  virtual StatusCode createAndRecord() const override;

protected:
  /// This will identify the input tracks (and so is templated on TrackParticleContainer).
  /// We do not declare it as a property to avoid duplication with the base PseudoJetGetter::m_incoll. Instead we'll copy the key from PseudoJetGetter::m_incoll during initialize() (See this function)
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_incolltrk;

  SG::ReadHandleKey<jet::TrackVertexAssociation> m_inTVA;


private:

  std::vector<fastjet::PseudoJet> 
  createPseudoJets(const xAOD::TrackParticleContainer*) const;

  const PseudoJetContainer* getC() const;

};

#endif
