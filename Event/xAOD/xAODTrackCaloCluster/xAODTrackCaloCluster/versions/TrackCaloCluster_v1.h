// Dear emacs, this is -*- c++ -*-
// $Id: TrackCaloCluster_v1.h $
#ifndef XAODTRACKCALOCLUSTER_VERSIONS_TRACKCALOCLUSTER_V1_H
#define XAODTRACKCALOCLUSTER_VERSIONS_TRACKCALOCLUSTER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"

namespace xAOD {
  /** Class describing a TrackCaloCluster 
   * 
   * @author Noemi Calace <Noemi.Calace@cern.ch>
   * 
   **/
  
  class TrackCaloCluster_v1 : public IParticle {
    
  public: 
    
    /// Type of TrackCaloCluster - Charged - Neutral - Both
    enum Taste { Charged = 0, Neutral, Both };
    
    // Default Constructor
    TrackCaloCluster_v1();
    // Destuctor
    ~TrackCaloCluster_v1();
    
    /// @name xAOD::IParticle functions
    /// @{
    /// The transverse momentum (\f$p_T\f$) of the particle.
    virtual double           pt() const;
    /// The pseudorapidity (\f$\eta\f$) of the particle.
    virtual double           eta() const;
    /// The azimuthal angle (\f$\phi\f$) of the particle (has range \f$-\pi\f$ to \f$+\pi\f$.)
    virtual double           phi() const;
    /// The invariant mass of the particle..
    virtual double           m() const;
    /// The total energy of the particle.
    virtual double           e() const;
    /// The true rapidity (y) of the particle.
    virtual double           rapidity() const;
    
    /// Definition of the 4-momentum type.
    typedef IParticle::FourMom_t FourMom_t;
    
    /// The full 4-momentum of the particle.
    virtual const FourMom_t& p4() const;
    
    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;
    /// @}
    
    /// @name Defining parameters functions
    /// The 'defining parameters' are key to the concept of a TrackCaloCluster, and give the values for the IParticle interface
    /// ( pt(), phi(), eta() etc.).
    /// They use the Trk::Perigee coordinate system, and are defined as:
    ///  \f$( d_0, z_0, \phi, \theta, q/p )\f$.
    /// The parameters are expressed with respect to an origin (returned by vx(), vy() and vy() ), currently intended to be the 'beamspot'.
    /// This origin is expected to be the same for all track particles in a collection (and this may be be enforced).
    /// The \f$\phi\f$ parameter is returned by either the phi() or the phi0() methods, the difference just being whether it is returned as a float or a double (it is stored as a float)
    /// @{
    /// Returns the charge.
    float charge() const;
    /// Returns the \f$\phi\f$ parameter, which has range \f$-\pi\f$ to \f$+\pi\f$.
    float phi0() const;
    /// Returns the \f$\theta\f$  parameter, which has range 0 to \f$\pi\f$.
    float theta() const;
    /// Returns the \f$q/p\f$  parameter
    float qOverP() const;
    /// Set the taste of the TrackCaloCluster
    void setTaste(Taste&);
    /// Get the taste of the TrackCaloCluster
    Taste& getTaste() const;
    /// @}
    
  private:
    /// Cached 4-momentum object.
    mutable FourMom_t m_p4;
    /// Cache state of the internal 4-momentum (reset from the streamer).
    mutable bool m_p4Cached;
    /// This is caching the Type of TrackCaloCluster - Charged - Neutral - Both
    mutable TrackCaloCluster_v1::Taste m_taste;
    
  }; 
  
}

#endif // XAODTRACKCALOCLUSTER_VERSIONS_TRACKCALOCLUSTER_V1_H