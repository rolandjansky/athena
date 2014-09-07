// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Electron_v1.h 615077 2014-09-04 13:42:41Z schaffer $
#ifndef XAODEGAMMA_VERSIONS_ELECTRON_V1_H
#define XAODEGAMMA_VERSIONS_ELECTRON_V1_H
	
// Core include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODEgamma/versions/Egamma_v1.h"
//Egamma includes
#include "xAODEgamma/EgammaEnums.h"
//Tracking includes
#include "xAODTracking/TrackingPrimitives.h"
//TrackParticle Includes
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace xAOD {

  /// @class xAOD::Electron
  /// @brief  Class describing an electron  
  ///
  /// @author Christos Anastopoulos
  /// @author Anthony Morley
  ///
  /// $Revision: 615077 $
  /// $Date: 2014-09-04 15:42:41 +0200 (Thu, 04 Sep 2014) $
  ///
  class Electron_v1 :public xAOD::Egamma_v1{

  public:
    /// @name xAOD::Electron constructors
    /// @{

    /// Default constructor. No store is attached.
    Electron_v1();

    /// Copy ctor. This involves copying the entire Auxilary store (Deep copy).
    Electron_v1(const Electron_v1& el);


    /// @}

    /// @name xAOD::Electron Assignment
    /// @{

    /// Assignment Operator. Ends up using the assignment of AuxElement for the store.
    Electron_v1& operator=(const Electron_v1& el );

    /// @}

    /// @name xAOD::Electron Charge
    /// @{

    /// @brief  Obtain the charge of the object
    float charge() const;

    /// @brief set the charge of the object
    void setCharge(float charge);

    /// @}

    /// @name xAOD::Electron ObjectType
    /// @{

    /// @brief The type of the object as a simple enumeration
    virtual Type::ObjectType type() const /*final*/; //Always an Electron.

    /// @}

    /// @name xAOD::Electron Pointers to  TrackParticles
    /// @{ 

    /// @brief Return the number xAOD::TrackParticles that match the electron
    /// candidate
    size_t nTrackParticles() const;
    
    /// @brief Pointer to the xAOD::TrackParticle/s that match the electron
    /// candidate
    const xAOD::TrackParticle* trackParticle( size_t index = 0 ) const;

    /// @brief ElementLink to the xAOD::TrackParticle/s that match the electron
    /// candidate
    const ElementLink< TrackParticleContainer >&
    trackParticleLink( size_t index = 0 ) const; 

    /// Helper type definition
    typedef std::vector< ElementLink< TrackParticleContainer > > TPELVec_t;

    /// @brief Get all the track particles
    const TPELVec_t& trackParticleLinks() const;
    /// @brief Set the ElementLinks to the xAOD::TrackParticle/s that match the
    /// electron candidate
    void setTrackParticleLinks( const TPELVec_t& links );

    /// @}

    /// @name xAOD::Electron to cluster matching values.
    /// @{ 

    /// Accessor to CaloMatch Values    
    bool trackCaloMatchValue( float& value, const EgammaParameters::TrackCaloMatchType information ) const;
    
    /// Set method for CaloMatch values.
    bool  setTrackCaloMatchValue( float& value, const EgammaParameters::TrackCaloMatchType information );

    /// Accessor to best matching tracks information
    bool trackParticleSummaryValue( uint8_t& value, const SummaryType information, int index = 0) const;
    bool trackParticleSummaryValue( float& value, const SummaryType information, int index =  0) const;

    /// @}


  }; // Electron Class

}//  xAOD namespace


#endif // XAODEGAMMA_VERSIONS_ELECTRON_V1_H

