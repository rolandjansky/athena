/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPProton_v1.cxx
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Implementation file of the AFPProton_v1 class.
 * 
 */


// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPProton_v1.h"

namespace xAOD {

  AFPProton_v1::AFPProton_v1() : IParticle(), m_p4(), m_p4Cached( false ) {}

  double AFPProton_v1::pt() const {
    return p4().Pt();
  }

  double AFPProton_v1::eta() const {
    return p4().Eta(); 
  }

  double AFPProton_v1::phi() const {
    return p4().Phi();
  }

  double AFPProton_v1::m() const {
    return p4().M();
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(AFPProton_v1, float, double, e)

  double AFPProton_v1::rapidity() const {
    return p4().Rapidity();
  }

  const AFPProton_v1::FourMom_t& AFPProton_v1::p4() const {
    // Check if we need to reset the cached object:
    if( ! m_p4Cached ) {
      m_p4.SetPxPyPzE( px(), py(), pz(), e() ); 
      m_p4Cached = true;
    }
    // Return the cached object:
    return m_p4;
  }

  Type::ObjectType AFPProton_v1::type() const {
     return Type::Particle;
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST (AFPProton_v1, float, double, px)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST (AFPProton_v1, float, double, py)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST (AFPProton_v1, float, double, pz)

  void AFPProton_v1::setPxPyPzE( double px, double py, double pz, double e ) {
    static Accessor< float > acc1( "px" );
    acc1( *this ) = (float)px;
    static Accessor< float > acc2( "py" );
    acc2( *this ) = (float)py;
    static Accessor< float > acc3( "pz" );
    acc3( *this ) = (float)pz;
    static Accessor< float > acc4( "e" );
    acc4( *this ) = (float)e;

    // Need to recalculate m_p4 if requested after update
    m_p4Cached = false;
  }

  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST (AFPProton_v1, float, double, px, setPx)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST (AFPProton_v1, float, double, py, setPy)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST (AFPProton_v1, float, double, pz, setPz)
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST (AFPProton_v1, float, double, e, setE)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPProton_v1, float, chi2, setChi2)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPProton_v1, int, side, setSide)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPProton_v1, int, methodID, setMethodID)

  AUXSTORE_OBJECT_SETTER_AND_GETTER (AFPProton_v1, std::vector<AFPProton_v1::AFPTrackLink_t>, tracks, setTracks)
  static SG::AuxElement::Accessor<std::vector<AFPProton_v1::AFPTrackLink_t>> tracksAcc("tracks");

  void AFPProton_v1::addTrack(const AFPTrackLink_t& newTrack) {
    tracksAcc(*this).push_back(newTrack);
  }

  void AFPProton_v1::toPersistent() {
    if (tracksAcc.isAvailableWritable(*this)) {
      std::vector<AFPProton_v1::AFPTrackLink_t>::iterator end = tracksAcc(*this).end();
      for (std::vector<AFPProton_v1::AFPTrackLink_t>::iterator itr = tracksAcc(*this).begin(); itr != end; ++itr)
        itr->toPersistent();
    }
  }

} // namespace xAOD
