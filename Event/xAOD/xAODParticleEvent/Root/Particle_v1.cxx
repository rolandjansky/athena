/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Particle_v1.cxx 643631 2015-02-02 16:52:33Z kkoeneke $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODParticleEvent/versions/Particle_v1.h"


namespace xAOD {

  Particle_v1::Particle_v1()
    : IParticle() {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions implementing four-momentum
  //

  double Particle_v1::pt() const {
    return std::hypot( px(), py() );
  }

  double Particle_v1::eta() const {
    return genvecP4().Eta();
  }

  double Particle_v1::phi() const {
    return genvecP4().Phi();
  }

  double Particle_v1::m() const {
    return genvecP4().M();
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Particle_v1, float, double, e)

  double Particle_v1::rapidity() const {
    return genvecP4().Rapidity();
  }

  Particle_v1::FourMom_t Particle_v1::p4() const {
    return FourMom_t( px(), py(), pz(), e() );
  }

  Particle_v1::GenVecFourMom_t Particle_v1::genvecP4() const {
    return GenVecFourMom_t( px(), py(), pz(), e() );
  }

  Type::ObjectType Particle_v1::type() const {
    return Type::Particle;
  }


  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Particle_v1, float, double, px)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Particle_v1, float, double, py)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Particle_v1, float, double, pz)


  double Particle_v1::et() const {
    return p4().Et();
  }


  void Particle_v1::setP4( const FourMom_t& vec ) {
    static const Accessor< float > acc1( "px" );
    acc1( *this ) = (float)(vec.Px());
    static const Accessor< float > acc2( "py" );
    acc2( *this ) = (float)(vec.Py());
    static const Accessor< float > acc3( "pz" );
    acc3( *this ) = (float)(vec.Pz());
    static const Accessor< float > acc4( "e" );
    acc4( *this ) = (float)(vec.E());
  }


  void Particle_v1::setPxPyPzE( double px,
                                         double py,
                                         double pz,
                                         double e ) {
    static const Accessor< float > acc1( "px" );
    acc1( *this ) = (float)px;
    static const Accessor< float > acc2( "py" );
    acc2( *this ) = (float)py;
    static const Accessor< float > acc3( "pz" );
    acc3( *this ) = (float)pz;
    static const Accessor< float > acc4( "e" );
    acc4( *this ) = (float)e;
  }


  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Particle_v1,
                                       float, double,
                                       px, setPx )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Particle_v1,
                                       float, double,
                                       py, setPy )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Particle_v1,
                                       float, double,
                                       pz, setPz )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Particle_v1,
                                       float, double,
                                       e, setE )

  // End: Functions implementing four-momentum
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions implementing other particly-type properties
  //

  bool Particle_v1::hasCharge() const {
    static const SG::AuxElement::Accessor< float > chargeAcc( "charge" );
    return chargeAcc.isAvailable( *this );
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Particle_v1, float,
                                        charge, setCharge )


  bool Particle_v1::hasPdgId() const {
    static const SG::AuxElement::Accessor< int > pdgAcc( "pdgId" );
    return pdgAcc.isAvailable( *this );
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Particle_v1, int,
                                        pdgId, setPdgId )


  // End: Functions implementing other particly-type properties
  /////////////////////////////////////////////////////////////////////////////



  void Particle_v1::toPersistent() {
    return;
  }


} // namespace xAOD
