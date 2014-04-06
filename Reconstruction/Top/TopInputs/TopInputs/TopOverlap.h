/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// TopOverlap - container to hold overlaped particles with the TopInputs
// Automatically mapped to the input particles

#ifndef TOP_OVERLAP_H
#define TOP_OVERLAP_H

#include "DataModel/ElementLinkVector.h"

#include "PhysicsElementLinks/MuonLinks.h"
#include "PhysicsElementLinks/ElectronLinks.h"
#include "PhysicsElementLinks/PhotonLinks.h"
#include "PhysicsElementLinks/TauJetLinks.h"
#include "PhysicsElementLinks/JetLinks.h"
#include "PhysicsElementLinks/TrackLinks.h"

#include "SGTools/CLASS_DEF.h"

class TopOverlapCnv_p1;

template<class Container> class TopInputMapCnv_p1;
template<class Container> class TopInputMapTriggerCnv_p1;

namespace TopRec {

  typedef  ElementLinkVector<Analysis::MuonContainer>     MuonLinkVector;
  typedef  ElementLinkVector<ElectronContainer>           ElectronLinkVector;
  typedef  ElementLinkVector<PhotonContainer>             PhotonLinkVector;
  typedef  ElementLinkVector<Analysis::TauJetContainer>   TauJetLinkVector;
  typedef  ElementLinkVector<JetCollection>               JetLinkVector;
  typedef  ElementLinkVector<Rec::TrackParticleContainer> TrackLinkVector; 
 
  class TopOverlap {

  public:
    
    TopOverlap();

    ~TopOverlap() {;}
    
    bool use() const { return m_use; }
    void setUse(const bool& use) { m_use=use; }
    
    bool hasOverlap() const;

    bool overlapsWithMuon() const     { return m_overlapMuons.size()>0; }
    bool overlapsWithElectron() const { return m_overlapElectrons.size()>0; }
    bool overlapsWithPhoton() const   { return m_overlapPhotons.size()>0; }
    bool overlapsWithTauJet() const   { return m_overlapTauJets.size()>0; }
    bool overlapsWithJet() const      { return m_overlapJets.size()>0; }
    bool overlapsWithTrack() const    { return m_overlapTracks.size()>0; }
    
    /*
     * Get Methods
     */

    // ParticleLinks
    const MuonLinks     overlapMuons()     const;
    const ElectronLinks overlapElectrons() const;
    const PhotonLinks   overlapPhotons()   const;
    const TauJetLinks   overlapTauJets()   const;
    const JetLinks      overlapJets()      const;
    const TrackLinks    overlapTracks()    const;


    // ElementLinkVectors
    const MuonLinkVector&     overlapMuonsVec()     const { return m_overlapMuons; }
    const ElectronLinkVector& overlapElectronsVec() const { return m_overlapElectrons; }
    const PhotonLinkVector&   overlapPhotonsVec()   const { return m_overlapPhotons; }
    const TauJetLinkVector&   overlapTauJetsVec()   const { return m_overlapTauJets; }
    const JetLinkVector&      overlapJetsVec()      const { return m_overlapJets; }
    const TrackLinkVector&    overlapTracksVec()    const { return m_overlapTracks; }

    /*
     * set methods
     */
    
    // insert individual links
    void insertOverlapMuon(const MuonLink& overlapMuon);
    void insertOverlapElectron(const ElectronLink& overlapElectron);
    void insertOverlapPhoton(const PhotonLink& overlapPhoton);
    void insertOverlapTauJet(const TauJetLink& overlapTauJet);
    void insertOverlapJet(const JetLink& overlapJet);
    void insertOverlapTrack(const TrackLink& overlapTrack);

    void insertLink(const ElementLink<Analysis::MuonContainer>& overlapMuon);
    void insertLink(const ElementLink<ElectronContainer>& overlapElectron);
    void insertLink(const ElementLink<PhotonContainer>& overlapPhoton);
    void insertLink(const ElementLink<Analysis::TauJetContainer>& overlapTauJet);
    void insertLink(const ElementLink<JetCollection>& overlapJet);    
    void insertLink(const ElementLink<Rec::TrackParticleContainer>& overlapTrack);    
  
    // set the whole vector (ParticleLinks)
    void setOverlapMuons(const MuonLinks& overlapMuons); 
    void setOverlapElectrons(const ElectronLinks& overlapElectrons); 
    void setOverlapPhotons(const PhotonLinks& overlapPhotons);
    void setOverlapTauJets(const TauJetLinks& overlapTauJets);
    void setOverlapJets(const JetLinks& overlapJets);
    void setOverlapTracks(const TrackLinks& overlapTracks);

    // set the whole vector (ElementLinkVector)
    void setOverlapMuons(const MuonLinkVector& overlapMuons); 
    void setOverlapElectrons(const ElectronLinkVector& overlapElectrons); 
    void setOverlapPhotons(const PhotonLinkVector& overlapPhotons);
    void setOverlapTauJets(const TauJetLinkVector& overlapTauJets);
    void setOverlapJets(const JetLinkVector& overlapJets);
    void setOverlapTracks(const TrackLinkVector& overlapTracks);

  protected:

    // only to be used by the converter (TopOverlapCnv_p1)
    //unsigned flag() const { return m_flag; }
    //void setFlag(const unsigned& flag) { m_flag=flag; }
      
    unsigned index() const { return m_index; }
    void setIndex(const unsigned& index) { m_index = index; }

  private:

    //unsigned m_flag;
    unsigned m_index;
    bool     m_use;

    MuonLinkVector     m_overlapMuons;
    ElectronLinkVector m_overlapElectrons;
    PhotonLinkVector   m_overlapPhotons;
    TauJetLinkVector   m_overlapTauJets;
    JetLinkVector      m_overlapJets;
    TrackLinkVector    m_overlapTracks;

    friend class ::TopOverlapCnv_p1;
    template<class Container> friend class ::TopInputMapCnv_p1;
    template<class Container> friend class ::TopInputMapTriggerCnv_p1;
    
  }; // TopOverlap class definition

} // namespace

//CLASS_DEF(TopRec::TopOverlap, 62839478, 1)

#endif // TOP_OVERLAP_H
