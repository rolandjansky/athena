/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Photon_v1.cxx 741489 2016-04-20 01:30:34Z christos $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODEgamma/versions/Photon_v1.h"
#include "PhotonAccessors_v1.h"

#include <stdexcept>

namespace xAOD {

   Photon_v1::Photon_v1()
      : Egamma_v1() {

   }

  Photon_v1::Photon_v1(const Photon_v1& ph) : Egamma_v1(ph){
    makePrivateStore(ph);  
  }

  Photon_v1& Photon_v1::operator=(const Photon_v1& ph ){

    if (this != &ph){ // protect against invalid self-assignment      
      this->Egamma_v1::operator=( ph );
    }
    // by convention, always return *this
    return *this;
  }  

   Type::ObjectType Photon_v1::type() const {

      return Type::Photon;
   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //       Implementation of the vertex handling functions
   //

   const xAOD::Vertex* Photon_v1::vertex( size_t index ) const {

      if( index >= nVertices() ) {
         return 0;
      }
      const VxELVec_t& links = vertexLinks();
      if( ! links[ index ].isValid() ) {
         return 0;
      }
      return *( links[ index ] );
   }

   const ElementLink< VertexContainer >&
   Photon_v1::vertexLink( size_t index ) const {

      if( index >= nVertices() ) {
         static const ElementLink< VertexContainer > dummy;
         return dummy;
      }
      return vertexLinks()[ index ];
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( Photon_v1, Photon_v1::VxELVec_t,
                                      vertexLinks, setVertexLinks )

   size_t xAOD::Photon_v1::nVertices() const {

      static const SG::AuxElement::Accessor< Photon_v1::VxELVec_t >
         vertexAcc( "vertexLinks" );

      if( vertexAcc.isAvailable( *this ) ) {
         return vertexAcc( *this ).size();
      }
      return 0;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  //Implementation of the summary value accessor functions

  bool Photon_v1::vertexCaloMatchValue( float& value, const EgammaParameters::VertexCaloMatchType information ) const {

    const xAOD::Photon_v1::Accessor< float >* acc = vertexCaloMatchAccessorV1( information );
    if( ! acc ) { 
      return false;
    }
    if(! acc->isAvailable( *this) ) {
      return  false;
    }
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  float Photon_v1::vertexCaloMatchValue( const EgammaParameters::VertexCaloMatchType information ) const {

    const xAOD::Photon_v1::Accessor< float >* acc = vertexCaloMatchAccessorV1( information );
    if(! acc ) throw std::runtime_error( "Unknown/Unavailable Vertex to Calo Match type requested" );
    // Retrieve the value:
    return ( *acc )( *this );
  }

  bool Photon_v1::setVertexCaloMatchValue( float& value, const EgammaParameters::VertexCaloMatchType information ) {

    const xAOD::Photon_v1::Accessor< float >* acc =  vertexCaloMatchAccessorV1( information );
    if( ! acc ) return false;
    // Set the value:
    ( *acc )( *this ) = value;
    return true;
  }
   /////////////////////////////////////////////////////////////////////////////


} // namespace xAOD
