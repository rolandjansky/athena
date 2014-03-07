/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLJMET_H
#define TTBARLJMET_H

#include "DataModel/DataLink.h"
#include "TopEvent/TTbarLJ.h"

#include "MissingETEvent/MissingET.h"

namespace TopRec {
  
  //================================================================
  //! Here we have MET in addition to the lepton and jets, still 
  //! no associations.

  template<class LeptonCollection>
  class TTbarLJMet : public TTbarLJ<LeptonCollection> {

  private:

    DataLink<MissingET> m_metLink;

  public:
    
    //! All jets are from one collection
    TTbarLJMet(const ElementLink<LeptonCollection>& lepton, const DataLink<MissingET>& metLink,
	       const JetCollection* jc, const IndexSet& jetNumbers)
      : TTbarLJ<LeptonCollection>(lepton, jc, jetNumbers), m_metLink(metLink)
    {}      

    //! General case
    TTbarLJMet(const ElementLink<LeptonCollection>& lepton, const DataLink<MissingET>& metLink,
	       const JetSet& jets)
      : TTbarLJ<LeptonCollection>(lepton, jets), m_metLink(metLink)
    {}

    //! General case
    TTbarLJMet(const ElementLink<LeptonCollection>& lepton, const DataLink<MissingET>& metLink,
	       const JetVector& jets)
      : TTbarLJ<LeptonCollection>(lepton, jets), m_metLink(metLink)
    {}

    //! default constructor is needed for POOL and dictionaries
    TTbarLJMet() {}

    void setMetLink(const DataLink<MissingET>& metLink) { m_metLink = metLink; }

    const MissingET* metLink() const { return m_metLink.cptr(); }

  };

  //================================================================

  typedef TTbarLJMet<ElectronContainer>         TTbarEJMet;
  typedef TTbarLJMet<Analysis::MuonContainer>   TTbarMuJMet;
  typedef TTbarLJMet<Analysis::TauJetContainer> TTbarTauJMet;
  typedef TTbarLJMet<TruthParticleContainer>    TTbarTruthJMet;
  typedef TTbarLJMet<JetCollection>             TTbarJeJMet;

} // namespace TopRec

#endif //TTBARLJMET_H
