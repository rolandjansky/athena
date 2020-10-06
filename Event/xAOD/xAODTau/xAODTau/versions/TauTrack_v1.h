/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAU_VERSIONS_TAUTRACK_V1_H
#define XAODTAU_VERSIONS_TAUTRACK_V1_H

#include <cstdint>

// Core include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

// local include(s):
#include "xAODTau/TauDefs.h"

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {



  class TauTrack_v1 : public IParticle {

  public:
    /// Default constructor
    TauTrack_v1();
    
    
    virtual double           pt() const;
    virtual double           eta() const;
    virtual double           phi() const;
    virtual double           e() const;
    
    virtual double           m() const;
    virtual double           rapidity() const;

    /// The full 4-momentum of the particle
    virtual FourMom_t        p4() const;

    /// Base 4 Momentum type for taus
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > GenVecFourMom_t;

    ///  The full 4-momentum of the particle : internal taus type.
    GenVecFourMom_t genvecP4() const; 

    /// Set methods for IParticle values
    void setP4(double pt, double eta, double phi, double m);

    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;

    //track flag
    bool flag(TauJetParameters::TauTrackFlag flag) const;
    bool flagWithMask(unsigned int) const;
    void setFlag(TauJetParameters::TauTrackFlag flag, bool value);
    typedef uint16_t TrackFlagType;
    TrackFlagType flagSet() const;
    void setFlagSet(TrackFlagType flag);

    // additional bdt input variables
    float z0sinThetaTJVA(const xAOD::IParticle&) const;
    //    void setZ0sinThetaTJVA(float z0sinThetaTJVA);

    // track IP w.r.t. TJVA vertex
    float d0TJVA() const;
    float d0SigTJVA() const;
    float z0sinthetaTJVA() const;
    float z0sinthetaSigTJVA() const;

    float rConv() const;
    //    void setRConv(float rConv);
    float rConvII() const;
    //    void setRConvII(float rConvII);
    float dRJetSeedAxis(const xAOD::IParticle&) const;
    //    void setDRJetSeedAxis(float dRJetSeedAxis);
      
    //bdt decision
    const std::vector<float>& bdtScores() const;
    void setBdtScores(const std::vector<float>& bdtScores);

    float bdtScore( size_t i) const;
    size_t nBdtScores() const;
    void addBdtScore( const float score);
    void clearBdtScores();

      
    // //name accessors for trackEtaStrip
    // float etaStrip() const;
    // void setEtaStrip(float eta);

    // //name accessors for trackPhiStrip
    // float phiStrip() const;
    // void setPhiStrip(float phi);

    bool detail( TauJetParameters::TrackDetail detail, float& value ) const;
    void setDetail( TauJetParameters::TrackDetail detail, float value );

    // tracks
    typedef std::vector< ElementLink< xAOD::TrackParticleContainer > > TrackParticleLinks_t;
    
    const TrackParticleLinks_t& trackLinks() const;
    void setTrackLinks( const TrackParticleLinks_t& trackParticle );
    // Get the pointer to a associated track 
    const TrackParticle* track() const;
    /// add a track to the collection
    void addTrackLink( const ElementLink< TrackParticleContainer >& tr);
    /// Remove all tracks from the collection
    void clearTrackLinks();

  }; // class TauTrack

} // namespace xAOD

// Declare IParticle as a base class of TauTrack_v1:
#include "AthContainers/DataVector.h"
DATAVECTOR_BASE( xAOD::TauTrack_v1, xAOD::IParticle );

#endif // XAODTAU_VERSIONS_TAUTRACK_V1_H

