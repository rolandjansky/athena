/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SlowMuon_v1.cxx 631507 2014-11-27 12:58:19Z htorres $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODMuon/versions/SlowMuon_v1.h"

namespace xAOD {

  SlowMuon_v1::SlowMuon_v1()
    : SG::AuxElement() {
  }
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( SlowMuon_v1, float, beta, setBeta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( SlowMuon_v1, float, betaT, setBetaT )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( SlowMuon_v1, float, ann, setAnn )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( SlowMuon_v1, int, nRpcHits, setNRpcHits )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( SlowMuon_v1, int, nTileCells, setNTileCells )
  
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, float, rpcBetaAvg )
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, float, rpcBetaRms )
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, float, rpcBetaChi2 )
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, int, rpcBetaDof )
  
  void SlowMuon_v1::setRpcInfo(float rpcBetaAvg, float rpcBetaRms, float rpcBetaChi2, int rpcBetaDof) {
    static const Accessor< float > acc1( "rpcBetaAvg" );
    acc1( *this ) = rpcBetaAvg;
    static const Accessor< float > acc2( "rpcBetaRms" );
    acc2( *this ) = rpcBetaRms;
    static const Accessor< float > acc3( "rpcBetaChi2" );
    acc3( *this ) = rpcBetaChi2;
    static const Accessor< int > acc4( "rpcBetaDof" );
    acc4( *this ) = rpcBetaDof;
  }

  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, float, mdtBetaAvg )
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, float, mdtBetaRms )
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, float, mdtBetaChi2 )
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, int, mdtBetaDof )
  
  void SlowMuon_v1::setMdtInfo(float mdtBetaAvg, float mdtBetaRms, float mdtBetaChi2, int mdtBetaDof) {
    static const Accessor< float > acc1( "mdtBetaAvg" );
    acc1( *this ) = mdtBetaAvg;
    static const Accessor< float > acc2( "mdtBetaRms" );
    acc2( *this ) = mdtBetaRms;
    static const Accessor< float > acc3( "mdtBetaChi2" );
    acc3( *this ) = mdtBetaChi2;
    static const Accessor< int > acc4( "mdtBetaDof" );
    acc4( *this ) = mdtBetaDof;
  }

  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, float, caloBetaAvg )
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, float, caloBetaRms )
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, float, caloBetaChi2 )
  AUXSTORE_PRIMITIVE_GETTER( SlowMuon_v1, int, caloBetaDof )
  
  void SlowMuon_v1::setCaloInfo(float caloBetaAvg, float caloBetaRms, float caloBetaChi2, int caloBetaDof) {
    static const Accessor< float > acc1( "caloBetaAvg" );
    acc1( *this ) = caloBetaAvg;
    static const Accessor< float > acc2( "caloBetaRms" );
    acc2( *this ) = caloBetaRms;
    static const Accessor< float > acc3( "caloBetaChi2" );
    acc3( *this ) = caloBetaChi2;
    static const Accessor< int > acc4( "caloBetaDof" );
    acc4( *this ) = caloBetaDof;
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( SlowMuon_v1, float, dEdxPixel, setDEdxPixel )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( SlowMuon_v1, float, dEdxCalo, setDEdxCalo )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( SlowMuon_v1, int, dEdxNClusters, setDEdxNClusters )
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( SlowMuon_v1, ElementLink< MuonContainer >, muonLink, setMuonLink)
  
} // namespace xAOD
