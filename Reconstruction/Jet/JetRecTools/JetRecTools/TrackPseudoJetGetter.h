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


namespace jet{
  class TrackVertexAssociation;
}

class TrackPseudoJetGetter : public PseudoJetGetter {
  ASG_TOOL_CLASS(TrackPseudoJetGetter, IPseudoJetGetter)
public:
  TrackPseudoJetGetter(const std::string &name);

  virtual int appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const;

protected:

  virtual jet::IConstituentUserInfo* buildCUI(const xAOD::IParticle* p, jet::IConstituentUserInfo::Index id, const LabelIndex* pli) const ;

  std::string m_trkVtxAssocName;
  mutable const jet::TrackVertexAssociation* m_trkVtxAssoc;
};

#endif
