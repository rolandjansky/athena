// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEGAMMA_VERSIONS_PHOTON_V1_H
#define XAODEGAMMA_VERSIONS_PHOTON_V1_H

// Core include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "xAODEgamma/versions/Egamma_v1.h"
#include "xAODEgamma/PhotonxAODHelpers.h"

//Already include the DataVector specialization for this type
#include "xAODEgamma/PhotonContainerFwd.h"


namespace xAOD {

  /// @class xAOD::Photon
  /// @brief  Class describing an photon  
  ///
  /// @author Christos Anastopoulos
  /// @author Anthony Morley
  ///
  /// $Revision: 667341 $
  /// $Date: 2015-05-15 16:39:34 +0200 (Fri, 15 May 2015) $
  ///
  class Photon_v1 :public xAOD::Egamma_v1 {

  public:
    /// @name xAOD::Photon constructors
    /// @{

    /// Default constructor
    Photon_v1();

    /// Copy ctor. This involves copying the entire Auxilary store (Deep copy).
    ///It is a slow operation which should be used sparingly.
    Photon_v1(const Photon_v1& ph);

    /// @}

    /// @name xAOD::Photon Assignment
    /// @{

    /// Assignment Operator. Ends up using the assignment of AuxElement for the store.
    Photon_v1& operator=(const Photon_v1& el );

    /// @}

    /// @name xAOD::Photon ObjectType
    /// @{

    /// @brief The type of the object as a simple enumeration
    virtual Type::ObjectType type() const final; //Always a Photon .
    /// @}

    /// @name xAOD::vertex Pointers to  vertices
    /// @{

    /// @brief Pointer to the xAOD::Vertex/es that match the photon candidate
    const xAOD::Vertex* vertex( size_t index = 0 ) const; 

    /// @brief ElementLink to the xAOD::vertex/es that match the electron
    ///        candidate
    const ElementLink< VertexContainer >& vertexLink( size_t index = 0 ) const;

    /// Helper type definition
    typedef std::vector< ElementLink< VertexContainer > > VxELVec_t;

    /// @brief Get all vertex links
    const VxELVec_t& vertexLinks() const;
    /// @brief set Pointer to the xAOD::vertex/vertices that match  the
    ///        photon candidate
    void setVertexLinks( const VxELVec_t& links );

    /// @brief Return the number xAOD::Vertex/vertices  that match the photon
    ///        candidate
    size_t nVertices() const;

    /// @}

    /// @name xAOD::Photon vertex to cluster matching values.
    /// @{ 

    ///@brief Accessor to vertex to Calo Match Values
    /// If 'information' is stored in this xAOD::Egamma and is of the correct type,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'.
    bool vertexCaloMatchValue( float& value, const EgammaParameters::VertexCaloMatchType information ) const;
    
    /// @brief Accessor to vertex to Calo Match Values , this just returns the value without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float vertexCaloMatchValue( const EgammaParameters::VertexCaloMatchType information ) const;

    ///@brief Set method for CaloMatch values.
    bool  setVertexCaloMatchValue( const float value, const EgammaParameters::VertexCaloMatchType information );

    ///@brief return the photon conversion type (see EgammaEnums)
    xAOD::EgammaParameters::ConversionType conversionType() const{
      return xAOD::EgammaHelpers::conversionType(this);
    }
    
    ///@brief return the photon conversion radius
    float conversionRadius() const {
      return xAOD::EgammaHelpers::conversionRadius(this);
    }
    

    /// @}


  }; // Photon Class

}//  xAOD namespace

//Finish declaration of Egamma as base class of Photon_v1
DATAVECTOR_BASE_FIN(xAOD::Photon_v1, xAOD::Egamma);
#endif // XAODEGAMMA_VERSIONS_PHOTON_V1_H
