// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompositeParticle_v1.h 618587 2014-09-25 17:27:28Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLE_V1_H
#define XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLE_V1_H


// STL includes
#include <vector>

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "AthLinks/ElementLink.h"
#include "xAODMissingET/MissingET.h"

// Local include(s):
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"



namespace xAOD {

  /// Description of a composite particle, i.e. a particle
  /// that is composed out of several other particles/objects,
  /// e.g., a Z boson that consists out of two muons.
  /// @author Karsten Koeneke <karsten.koeneke@cern.ch>
  ///
  /// $Revision: 618587 $
  /// $Date: 2014-09-25 19:27:28 +0200 (Thu, 25 Sep 2014) $
  ///
  class CompositeParticle_v1 : public IParticle {

  public:
    /// Default constructor
    CompositeParticle_v1();

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

    /// Check if this composite particle has its charge set
    bool hasCharge() const;
    /// Return the electric charge.
    float charge() const;
    /// Set the electric charge
    void setCharge( float charge );

    /// Check if this composite particle has its charge set
    bool hasPdgId() const;
    /// Return the PDG identifier
    int pdgId() const;
    /// Set the PDG identifier
    void setPdgId( int pdgID );

    /// @}


    /// @name Functions implementing handling of constituents
    /// @{

    /// @brief Add a particle as an additional constituent.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void addConstituent( const xAOD::IParticle* part,
                         bool updateFourMom=true );

    /// @brief Add a particle as an additional constituent.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void addConstituent( const xAOD::IParticleLink& partLink,
                         bool updateFourMom=true );

    /// @brief Add many constituents at the same time
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void addConstituents( const xAOD::IParticleLinkContainer& partLinkCont,
                          bool updateFourMom=true );

    /// @brief Remove a particle as a constituent from this
    ///        CompostiteParticle.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    ///        If the given IParticle is not already a constituent
    ///        of this CompositeParticle, nothing happens
    void removeConstituent( const xAOD::IParticle* part,
                            bool updateFourMom=true );

    /// @brief Remove a particle as a constituent from this
    ///        CompostiteParticle.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    ///        If the given IParticle is not already a constituent
    ///        of this CompositeParticle, nothing happens
    void removeConstituent( const xAOD::IParticleLink& partLink,
                            bool updateFourMom=true );



    /// @brief Get the MissingET object that is associated with this
    ///        CompositeParticle. If no (valid) MissingET object is associated
    ///        with this CompositeParticle, a null pointer will be returned
    const xAOD::MissingET* missingET() const;

    /// @brief Set the MissingET for this CompositeParticle.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void setMissingET( const xAOD::MissingET* met, bool updateFourMom=true );

    /// @brief Remove the existing MissingET object from this CompositeParticle.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void removeMissingET( bool updateFourMom=true );


    /// Check if a given xAOD::MissingET object is part of this CompositeParticle
    bool contains( const xAOD::MissingET* met          ) const;
    /// Check if a given xAOD::IParticle is a constituent
    bool contains( const xAOD::IParticle* part         ) const;
    /// Check if a given xAOD::IParticle is a constituent
    bool contains( const xAOD::IParticleLink& partLink ) const;


    /// Number of constituent particles.
    /// Note that MissingET is NOT counted as a constituent in this context
    std::size_t nConstituents() const;

    /// Get the constituent IParticle number i
    const xAOD::IParticle* constituent( std::size_t index = 0 ) const;

    /// Get the constituent IParticle number i as an ElementLink
    const xAOD::IParticleLink& constituentLink( std::size_t index = 0 ) const;

    /// Get all constituents in one go
    const xAOD::IParticleLinkContainer& constituentLinks() const;

    /// The constituent iterators are missing for the moment



  protected:
    /// @brief Set all constituents in one go.
    ///        Warning: This is meant for usage internal to this class only
    ///        because no addition of the constituent properties will be done!
    void setConstituentLinks( const xAOD::IParticleLinkContainer& constitLinks);


  public:

    /// Function preparing the object to be persistified
    void toPersistent();

  private:
    /// Cached 4-momentum object
    mutable FourMom_t m_p4;

    /// Cache state of the internal 4-momentum (reset from the streamer)
    mutable bool m_p4Cached;

  }; // class CompositeParticle_v1


} // namespace xAOD


// Set up a CLID for the object:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CompositeParticle_v1, 172352727, 1 )

#endif // XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLE_V1_H
