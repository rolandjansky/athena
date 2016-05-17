// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DiTauJet_v1.h 631921 2015-09-23 23:30:59Z dkirchme $
#ifndef XAODDITAU_VERSIONS_DITAUJET_V1_H
#define XAODDITAU_VERSIONS_DITAUJET_V1_H


// Core include(s):
#include "AthLinks/ElementLink.h"


// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTau/DiTauDefs.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODPFlow/PFOContainer.h"


namespace xAOD {



  class DiTauJet_v1 : public IParticle {

  public:
    /// Default constructor
    DiTauJet_v1();
    
    
    virtual double           pt() const;
    virtual double           eta() const;
    virtual double           phi() const;
    virtual double           e() const;
    
    virtual double           m() const;
    virtual double           rapidity() const;

    /// The full 4-momentum of the particle
    virtual const FourMom_t& p4() const;

    /// Set methods for IParticle values
    void setP4(double pt, double eta, double phi, double m);

    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;

    // seed jet
    typedef ElementLink< xAOD::JetContainer > JetLink_t;

    const JetLink_t& jetLink() const;
    
    void setJetLink( const JetLink_t& jetLink ); 

    void setJet( const xAOD::JetContainer *cont, const xAOD::Jet *jet ); 

    const Jet* jet() const;

    // subjet 4-mom
    float subjetPt(unsigned int numSubjet) const;
    float subjetEta(unsigned int numSubjet) const;
    float subjetPhi(unsigned int numSubjet) const;
    float subjetE(unsigned int numSubjet) const;

    void setSubjetPtEtaPhiE(unsigned int numSubjet, 
                            float pt, float eta, float phi, float e);

    float nSubjets() const;
  
  
    // f_core
    float fCore(unsigned int numSubjet) const;
    void setfCore(unsigned int numSubjet, float fCore);


    // vertex that both taus come from
    typedef ElementLink< xAOD::VertexContainer > VertexLink_t;

    const VertexLink_t& vertexLink() const;
    
    void setVertexLink( const VertexLink_t& vertexLink ); 

    void setVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex ); 

    const Vertex* vertex() const;


    // // secondary vertices
    // // typedef ElementLink< xAOD::VertexContainer > VertexLink_t;
    // typedef std::vector< ElementLink< xAOD::VertexContainer > >  SecVertexLinks_t;

    // const SecVertexLinks_t& secVertexLinks() const;
    // void setSecVertexLinks( const SecVertexLinks_t& secondayVertices );
    // // Get the pointer to a given secondary vertex associated with this subjet
    // const Vertex* secVertex( size_t i ) const;
    // // Get the number of secondary vertex associated with this subjet
    // size_t nSecVertex() const;
    // // add a secondary vertex associated with a subjet
    // void addSecVertex( const xAOD::VertexContainer*, const xAOD::Vertex*);
    // // Remove all vertices
    // void clearSecVertexLinks();


    // tracks
    typedef std::vector< ElementLink< xAOD::TrackParticleContainer > >  TrackParticleLinks_t;
    
    // tracks within subjets
    const TrackParticleLinks_t& trackLinks() const;
    void setTrackLinks( const TrackParticleLinks_t& trackParticles );
    // Get the pointer to a given track associated with this subjet
    const TrackParticle* track( size_t i ) const;
    // Get the number of track particles associated with this subjet
    size_t nTracks() const;
    // add a track to the subjet
    void addTrack( const xAOD::TrackParticleContainer*, const xAOD::TrackParticle*);
    // Remove all tracks
    void clearTrackLinks();

    // tracks within isolation region
    const TrackParticleLinks_t& otherTrackLinks() const;
    void setOtherTrackLinks( const TrackParticleLinks_t& trackParticles );
    const TrackParticle* otherTrack( size_t i ) const;
    size_t nOtherTracks() const;
    void addOtherTrack( const xAOD::TrackParticleContainer*, const xAOD::TrackParticle*);
    void clearOtherTrackLinks();

    // tracks within seed jet, not passing quality cuts
    const TrackParticleLinks_t& isoTrackLinks() const;
    void setIsoTrackLinks( const TrackParticleLinks_t& trackParticles );
    const TrackParticle* isoTrack( size_t i ) const;
    size_t nIsoTracks() const;
    void addIsoTrack( const xAOD::TrackParticleContainer*, const xAOD::TrackParticle*);
    void clearIsoTrackLinks();


    /// Get and set values of common details variables via enum
    bool detail( DiTauJetParameters::Detail detail, int& value ) const;
    bool detail( DiTauJetParameters::Detail detail, float& value ) const;
    void setDetail( DiTauJetParameters::Detail detail, int value );
    void setDetail( DiTauJetParameters::Detail detail, float value );


  private:

    /// Cached 4-momentum object
    mutable FourMom_t m_p4;
    /// Cache state of the internal 4-momentum (reset from the streamer)
    mutable bool m_p4Cached;

  }; // class DiTauJet

} // namespace xAOD

// Set up a CLID for the class:
// #ifndef XAOD_STANDALONE
// #include "SGTools/CLASS_DEF.h"
// CLASS_DEF( xAOD::DiTauJet_v1 , 67662955 , 1 )
// #endif // not XAOD_STANDALONE

// Declare IParticle as a base class of DiTauJet_v1:
#include "AthContainers/DataVector.h"
DATAVECTOR_BASE( xAOD::DiTauJet_v1, xAOD::IParticle );

#endif // XAODDITAU_VERSIONS_DITAUJET_V1_H

