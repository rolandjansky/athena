// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEDM_FASTJETLINK_H
#define JETEDM_FASTJETLINK_H
/////////////////////////////////////////////////
/// \class FastJetLink
///  A concrete implementation of FastJetLinkBase
///
/// This class holds the fastjet::PseudoJet as a data member.
/// It sets its FastJetLinkBase pointers to point this member and its ClusterSequence
/// Important : il also makes sure ClusterSequence::delete_self_when_unused() is called so 
/// the ClusterSequence is alive as long as needed.
//////////////////////////////////////////////////

#include "fastjet/PseudoJet.hh"
#include "xAODJet/FastJetLinkBase.h"


namespace xAOD {
  
  class FastJetLink : public FastJetLinkBase {
  public:
    FastJetLink(const fastjet::PseudoJet & pj ){
      m_actualPseudoJet = pj;
      pseudoJet = &m_actualPseudoJet;
      if( pj.has_valid_cluster_sequence () ) {
        clusterSequence = pj.validated_cluster_sequence ();
      }
    }
    virtual FastJetLinkBase* clone() { return new FastJetLink(*this); }
    ~FastJetLink() {}
  protected:
    fastjet::PseudoJet m_actualPseudoJet;
  };
  
}

#endif
