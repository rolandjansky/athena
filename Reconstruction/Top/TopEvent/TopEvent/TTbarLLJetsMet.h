/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLLJETSMET_H
#define TTBARLLJETSMET_H

#include "DataModel/DataLink.h"
#include "TopEvent/TTbarLLJets.h"
#include "MissingETEvent/MissingET.h"

namespace TopRec {

  //================================================================
  //! Dileptonic top that includes jets and met, no associations.

  template<class LeptonCollection1, class LeptonCollection2>
  class TTbarLLJetsMet : 
    public TTbarLLJets<LeptonCollection1,LeptonCollection2> {

  private:
    
    DataLink<MissingET> m_metLink;

  public:
    
        //! All jets are from one collection
    TTbarLLJetsMet(const ElementLink<LeptonCollection1>& lepton1,
		   const ElementLink<LeptonCollection2>& lepton2,
		   const DataLink<MissingET>& metLink,
		   const JetCollection* jc, const IndexSet& jetNumbers)
      : TTbarLLJets<LeptonCollection1,LeptonCollection2>(lepton1, lepton2, jc, jetNumbers),m_metLink(metLink)
    {}

    //! General case
    TTbarLLJetsMet(const ElementLink<LeptonCollection1>& lepton1,
		   const ElementLink<LeptonCollection2>& lepton2,
		   const DataLink<MissingET>& metLink,
		   const JetSet& jets)
      : TTbarLLJets<LeptonCollection1,LeptonCollection2>(lepton1, lepton2, jets),m_metLink(metLink)
    {}

    //! General case
    TTbarLLJetsMet(const ElementLink<LeptonCollection1>& lepton1,
		   const ElementLink<LeptonCollection2>& lepton2,
		   const DataLink<MissingET>& metLink,
		   const JetVector& jets)
      : TTbarLLJets<LeptonCollection1,LeptonCollection2>(lepton1, lepton2, jets),m_metLink(metLink)
    {}

    //! default constructor is needed for POOL and dictionaries
    TTbarLLJetsMet() {}

    void setMetLink(const DataLink<MissingET>& metLink) { m_metLink = metLink; }

    const MissingET* metLink() const { return m_metLink.cptr(); }

  };

  //================================================================

  typedef TTbarLLJetsMet<ElectronContainer,ElectronContainer>                 TTbarEEJetsMet;
  typedef TTbarLLJetsMet<ElectronContainer,Analysis::MuonContainer>           TTbarEMuJetsMet;
  typedef TTbarLLJetsMet<ElectronContainer,Analysis::TauJetContainer>         TTbarETauJetsMet;
  typedef TTbarLLJetsMet<ElectronContainer,Rec::TrackParticleContainer>       TTbarETrackJetsMet;
  typedef TTbarLLJetsMet<Analysis::MuonContainer,Analysis::MuonContainer>     TTbarMuMuJetsMet;
  typedef TTbarLLJetsMet<Analysis::MuonContainer,Analysis::TauJetContainer>   TTbarMuTauJetsMet;
  typedef TTbarLLJetsMet<Analysis::MuonContainer,Rec::TrackParticleContainer> TTbarMuTrackJetsMet;
  typedef TTbarLLJetsMet<Analysis::TauJetContainer,Analysis::TauJetContainer> TTbarTauTauJetsMet;
  typedef TTbarLLJetsMet<TruthParticleContainer,TruthParticleContainer>       TTbarTruthTruthJetsMet;

} // namespace TopRec

#endif //TTBARLLJETSMET_H
