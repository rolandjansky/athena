// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

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

//Already include the DataVector specialization for this type
#include "xAODEgamma/ElectronContainerFwd.h"

namespace xAOD {

  /// @class xAOD::Electron
  /// @brief  Class describing an electron
  ///
  /// @author Christos Anastopoulos
  /// @author Anthony Morley
  ///
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
    virtual Type::ObjectType type() const final; //Always an Electron.

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

    /// @name xAOD::Electron  Track to Calo matching values.
    /// @{

    ///@brief Accessor for Track to Calo Match Values
    bool trackCaloMatchValue( float& value, const EgammaParameters::TrackCaloMatchType information ) const;

    ///@brief Accessor for Track to Calo Match values , this just returns the value without internaly checking if it exists.
    /// Will lead to an exception if the information is not available.
    float trackCaloMatchValue(  const EgammaParameters::TrackCaloMatchType information ) const;


    ///@brief Set method for Track to Calo Match values.
    bool  setTrackCaloMatchValue( const float value, const EgammaParameters::TrackCaloMatchType information );

    ///@brief Accessor to the  matching track(s) float information (index = 0 is the best match)
    /// If 'information' is stored in this xAOD::Egamma and is of the correct type,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'.
    bool trackParticleSummaryValue( uint8_t& value, const SummaryType information, int index = 0) const;
    bool trackParticleSummaryValue( float& value, const SummaryType information, int index =  0) const;

    ///@brief Accessor to the  matching track(s) int information (index = 0 is the best match)
    /// Will lead to an exception if the information is not available.
    uint8_t trackParticleSummaryIntValue( const SummaryType information, int index = 0) const;
    float  trackParticleSummaryFloatValue( const SummaryType information, int index =  0) const;

    /// @}


  }; // Electron Class

}//  xAOD namespace
//Finish declaration of Egamma as a base class of Electron
DATAVECTOR_BASE_FIN(xAOD::Electron_v1, xAOD::Egamma);

#endif // XAODEGAMMA_VERSIONS_ELECTRON_V1_H

