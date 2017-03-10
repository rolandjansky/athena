// emacs : this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_TCCPSEUDOJETGETTER_H
#define JETRECTOOLS_TCCPSEUDOJETGETTER_H
////////////////////////////////////////////
/// \class TCCPseudoJetGetter
///
/// Specialized PseudoJetGetter for TrackCaloCluster
///  The only thing done here is to specialize the templated append method 
//////////////////////////////////////////////////

#include "JetRec/PseudoJetGetter.h"
#include "xAODTrackCaloCluster/TrackCaloClusterContainer.h"

class TCCPseudoJetGetter : public PseudoJetGetter {
  ASG_TOOL_CLASS(TCCPseudoJetGetter, IPseudoJetGetter)
public:
  TCCPseudoJetGetter(const std::string &name);

  virtual int appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const;

protected:

  virtual jet::IConstituentUserInfo* buildCUI(const xAOD::IParticle* p, jet::IConstituentUserInfo::Index id, const LabelIndex* pli) const ;
  
};

#endif
