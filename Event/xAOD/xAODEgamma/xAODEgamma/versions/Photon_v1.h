// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Photon_v1.h 617559 2014-09-18 21:37:54Z christos $
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

namespace xAOD {

  /// @class xAOD::Photon
  /// @brief  Class describing an photon  
  ///
  /// @author Christos Anastopoulos
  /// @author Anthony Morley
  ///
  /// $Revision: 617559 $
  /// $Date: 2014-09-18 23:37:54 +0200 (Thu, 18 Sep 2014) $
  ///
  class Photon_v1 :public xAOD::Egamma_v1 {

  public:
    /// @name xAOD::Photon constructors
    /// @{

    /// Default constructor
    Photon_v1();

    /// Copy ctor. This involves copying the entire Auxilary store (Deep copy), and is a slow operation which should be used sparingly.
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
    virtual Type::ObjectType type() const /*final*/; //Always a Photon .
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

    /// @name xAOD::Photon to cluster matching values.
    /// @{ 

    ///@brief Accessor to CaloMatch Values
    /// If 'information' is stored in this xAOD::Egamma and is of the correct type,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'.
    bool vertexCaloMatchValue( float& value, const EgammaParameters::VertexCaloMatchType information ) const;
    
    /// Accessor to CaloMatch Values , this just returns the value without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float vertexCaloMatchValue( const EgammaParameters::VertexCaloMatchType information ) const;


    /// Set method for CaloMatch values.
    bool  setVertexCaloMatchValue( float& value, const EgammaParameters::VertexCaloMatchType information );
    /// @}


  }; // Photon Class

}//  xAOD namespace


#endif // XAODEGAMMA_VERSIONS_PHOTON_V1_H
