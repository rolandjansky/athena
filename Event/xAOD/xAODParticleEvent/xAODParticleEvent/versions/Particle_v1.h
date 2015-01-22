// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Particle_v1.h 637086 2014-12-19 16:02:19Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_PARTICLE_V1_H
#define XAODPARTICLEEVENT_VERSIONS_PARTICLE_V1_H


// STL includes
#include <vector>

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "AthLinks/ElementLink.h"


namespace xAOD {

  /// Description of a generic particle.
  /// @author Karsten Koeneke <karsten.koeneke@cern.ch>
  ///
  /// $Revision: 637086 $
  /// $Date: 2014-12-19 17:02:19 +0100 (Fri, 19 Dec 2014) $
  ///
  class Particle_v1 : public IParticle {

  public:
    /// Default constructor
    Particle_v1();

    /// Copy constructor
    //Particle_v1(const Particle_v1& other);

    //virtual ~Particle_v1();

    /// @name Functions implementing the xAOD::IParticle interface
    /// @{

    /// The transverse momentum (\f$p_T\f$) of the particle
    virtual double           pt() const;
    /// The pseudorapidity (\f$\eta\f$) of the particle
    virtual double           eta() const;
    /// The azimuthal angle (\f$\phi\f$) of the particle
    virtual double           phi() const;
    /// The invariant mass of the particle
    virtual double           m() const;
    /// The total energy of the particle
    virtual double           e() const;
    /// The true rapidity (y) of the particle
    virtual double           rapidity() const;

    /// Definition of the 4-momentum type
    typedef IParticle::FourMom_t FourMom_t;

    /// The full 4-momentum of the particle
    virtual const FourMom_t& p4() const;

    ///  The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;
    /// @}


    /// @name Functions implementing additional 4-momentum properties
    /// @{

    /// Get the px momentum component of the particle
    double   px() const;
    /// Get the py momentum component of the particle
    double   py() const;
    /// Get the pz momentum component of the particle
    double   pz() const;
    /// The transverse energy (\f$e_T\f$) of the particle.
    /// Note that this differs from (\f$p_T\f$) for massive particles.
    double   et() const;


    /// Set the 4-vec
    void setP4(      const FourMom_t& vec );
    /// Set the 4-vec
    void setPxPyPzE( double px, double py, double pz, double e );
    /// Set the x-component of the momentum
    void setPx(      double px );
    /// Set the y-component of the momentum
    void setPy(      double py );
    /// Set the z-component of the momentum
    void setPz(      double pz );
    /// Set the energy
    void setE(       double e  );
    /// @}


    /// @name Functions implementing other particly-type properties
    /// @{

    /// Check if this particle has its charge set
    bool hasCharge() const;
    /// Return the electric charge.
    float charge() const;
    /// Set the electric charge
    void setCharge( float charge );

    /// Check if this particle has its charge set
    bool hasPdgId() const;
    /// Return the PDG identifier
    int pdgId() const;
    /// Set the PDG identifier
    void setPdgId( int pdgID );

    /// @}


  public:

    /// Function preparing the object to be persistified
    void toPersistent();

  private:
    /// Cached 4-momentum object
    mutable FourMom_t m_p4;

    /// Cache state of the internal 4-momentum (reset from the streamer)
    mutable bool m_p4Cached;

  }; // class Particle_v1


} // namespace xAOD

#endif // XAODPARTICLEEVENT_VERSIONS_PARTICLE_V1_H
