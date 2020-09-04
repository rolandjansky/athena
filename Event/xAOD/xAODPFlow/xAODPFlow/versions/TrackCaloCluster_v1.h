// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// $Id: TrackCaloCluster_v1.h $
#ifndef XAODPFLOW_VERSIONS_TRACKCALOCLUSTER_V1_H
#define XAODPFLOW_VERSIONS_TRACKCALOCLUSTER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace xAOD {
  /** Class describing a TrackCaloCluster 
   * 
   * @author Noemi Calace <Noemi.Calace@cern.ch>
   * 
   **/
  
  class TrackCaloCluster_v1 : public IParticle {
    
  public: 
    
    /// Type of TrackCaloCluster - Charged - Neutral - Combined
    enum Taste { Charged = 0, Neutral, Combined };
    
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
    /// The taste of the particle
    virtual int              taste() const;
    
    /// Definition of the 4-momentum type.
    typedef IParticle::FourMom_t FourMom_t;
    
    /// The full 4-momentum of the particle.
    virtual FourMom_t        p4() const;

    /// Base 4 Momentum type (GenVector version)
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > GenVecFourMom_t;

    ///  The full 4-momentum of the particle : GenVector version
    GenVecFourMom_t genvecP4() const; 
    
    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;
    /// @}
    
    /// @name Defining parameters functions
    /// The 'defining parameters' are key to the concept of a TrackCaloCluster, and give the values for the IParticle interface
    /// ( pt(), phi(), eta() etc.).
    /// The \f$\phi\f$ parameter is returned by either the phi() or the phi0() methods, the difference just being whether it is returned as a float or a double (it is stored as a float)
    /// @{
    void setParameters(float pt, float eta, float phi, float m, TrackCaloCluster_v1::Taste taste, 
		       const ElementLink<xAOD::TrackParticleContainer>& particle, 
		       const std::vector< ElementLink< xAOD::CaloClusterContainer > >& clusters);       
    /// @}
    
    /// @name Links       
    /// @{
    /// @brief Returns a link (which can be invalid) to the xAOD::TrackParticle associated with this TrackCaloCluster.
    const ElementLink<xAOD::TrackParticleContainer>& trackParticleLink() const;
    /// @brief Set the link to the TrackParticle
    void setTrackParticleLink(const ElementLink< xAOD::TrackParticleContainer >& particle);
    /// @brief Returns a pointer (which can be NULL) to the xAOD::TrackParticle associated with this TrackCaloCluster.
    const xAOD::TrackParticle* trackParticle() const;       
    /// @brief Returns a vector to the links (which can be invalid) to the xAOD::CaloClusters associated with this TrackCaloCluster.
    const std::vector< ElementLink<xAOD::CaloClusterContainer> > & caloClusterLinks() const;
    /// @brief Set the links to the CaloClusters
    void setCaloClusterLinks(const std::vector< ElementLink< xAOD::CaloClusterContainer > >& caloClusterLinks);
    /// @}   
    
  }; 
  
}

#endif // XAODPFLOW_VERSIONS_TRACKCALOCLUSTER_V1_H
