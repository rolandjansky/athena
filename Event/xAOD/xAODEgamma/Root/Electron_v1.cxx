/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Electron_v1$

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// Local include(s):
#include "xAODEgamma/versions/Electron_v1.h"
#include "ElectronAccessors_v1.h"

#include <stdexcept>

namespace xAOD {

  Electron_v1::Electron_v1() : Egamma_v1() {
  }

  Electron_v1::Electron_v1(const Electron_v1& el) : Egamma_v1(el){
    this->makePrivateStore(el);  
  }
 
  Electron_v1& Electron_v1::operator=(const Electron_v1& el ){

    if (this != &el){ // protect against invalid self-assignment      
      this->Egamma_v1::operator=( el );
    }
    // by convention, always return *this
    return *this;
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Electron_v1, float, charge, setCharge )

  Type::ObjectType Electron_v1::type() const {
    return Type::Electron;
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //         Implementation of the TrackParticle accessor functions
  //

  size_t xAOD::Electron_v1::nTrackParticles() const {

     static const SG::AuxElement::Accessor< Electron_v1::TPELVec_t >
        trackAcc( "trackParticleLinks" );

     if( trackAcc.isAvailable( *this ) ) {
        return trackAcc( *this ).size();
     }
     return 0;
  }

  const xAOD::TrackParticle* Electron_v1::trackParticle( size_t index ) const {

     if( index >= nTrackParticles() ) {
        return 0;
     }
     const TPELVec_t& links = trackParticleLinks();
     if( ! links[ index ].isValid() ) {
        return 0;
     }
     return *( links[ index ] );
  }

  const ElementLink< TrackParticleContainer >&
  Electron_v1::trackParticleLink( size_t index ) const {

     if( index >= nTrackParticles() ) {
        static const ElementLink< TrackParticleContainer > dummy;
        return dummy;
     }
     return trackParticleLinks()[ index ];
  }

  AUXSTORE_OBJECT_SETTER_AND_GETTER( Electron_v1, Electron_v1::TPELVec_t,
                                     trackParticleLinks, setTrackParticleLinks )

  //
  //////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  //Implementation of the summary value accessor functions
  
  bool Electron_v1::trackCaloMatchValue( float& value, const EgammaParameters::TrackCaloMatchType information ) const {

    const xAOD::Electron_v1::Accessor< float >* acc = trackCaloMatchAccessorV1( information );
    if( !acc ) { 
      return false;
    }
    if(!acc->isAvailable( *this) ){
      return  false;
    }
    // Retrieve the value:
    value = ( *acc )( *this );
    return true;
  }

  float Electron_v1::trackCaloMatchValue( const EgammaParameters::TrackCaloMatchType information ) const {
    const xAOD::Electron_v1::Accessor< float >* acc = trackCaloMatchAccessorV1( information );
    if(!acc ) throw std::runtime_error( "Unknown/Unavailable Track to Calo Match type requested" );
    return ( *acc )( *this );
  }

  bool Electron_v1::setTrackCaloMatchValue( float value, const EgammaParameters::TrackCaloMatchType information ) {

    const xAOD::Electron_v1::Accessor< float >* acc = trackCaloMatchAccessorV1( information );
     if( !acc ) return false;
    // Set the value:
    ( *acc )( *this ) = value;
    return true;
  }

  bool Electron_v1::trackParticleSummaryValue( uint8_t& value, const SummaryType information, int index ) const {

    const xAOD::TrackParticle* tempTrackParticle = trackParticle(index);
    if (!tempTrackParticle) return false;
    return tempTrackParticle->summaryValue(value,information);
  }

  bool Electron_v1::trackParticleSummaryValue( float& value, const SummaryType information, int index ) const {
    
    const xAOD::TrackParticle* tempTrackParticle = trackParticle(index);
    if (!tempTrackParticle) return false;
    return tempTrackParticle->summaryValue(value,information);
  }
  
  uint8_t Electron_v1::trackParticleSummaryIntValue( const SummaryType information, int index) const {
    const xAOD::TrackParticle* tempTrackParticle = trackParticle(index);
    if (!tempTrackParticle) {
      throw std::runtime_error( "TrackParticle not available" );
    }
    uint8_t value=0;
    if(!tempTrackParticle->summaryValue(value,information)){
      throw std::runtime_error( "Unknown/Unavailable Int Track Summary type requested" );
    }
    return value;
  }

  float Electron_v1::trackParticleSummaryFloatValue( const SummaryType information, int index) const {
    const xAOD::TrackParticle* tempTrackParticle = trackParticle(index);
    if (!tempTrackParticle) {
      throw std::runtime_error( "TrackParticle not available" );
    }
    float value=0;
    if(!tempTrackParticle->summaryValue(value,information)){
      throw std::runtime_error( "Unknown/Unavailable Float Track Summary type requested" );
    }
    return value;
  }
  /////////////////////////////////////////////////////////////////////////////////////


} // namespace xAOD
