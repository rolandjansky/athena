// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_FASTJETLINKBASE_H
#define XAODJET_FASTJETLINKBASE_H
//////////////////////////////////////
/// \class FastJetLinkBase 
/// 
/// FastJetLinkBase is a base class defining only pointers to forwared declared fastjet classes
/// This allows the Jet interface to return such objects without introducing an explicit
/// dependency on fastjet
///
/// For this to work, a derived class must be implemented to be in charge of the actual management
/// of the fastjet objects.
/// Such a class is FastJetLink defined in the JetEDM package.
/// It's used from the Jet_PseudoJet.icc header file 
///////////////////////////////////////

namespace fastjet {
  class PseudoJet;
  class ClusterSequence;
}

namespace xAOD {
  class FastJetLinkBase {
  public:
    virtual FastJetLinkBase* clone(){return NULL;}
    const fastjet::PseudoJet* pseudoJet;
    const fastjet::ClusterSequence * clusterSequence;
    virtual ~FastJetLinkBase(){}
  };
}
#endif
