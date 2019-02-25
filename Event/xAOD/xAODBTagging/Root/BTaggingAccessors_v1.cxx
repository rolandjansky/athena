/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAccessors_v1.cxx 602978 2014-06-20 20:39:23Z aad $

// System include(s):
#include <iostream>

// Local include(s):
#include "BTaggingAccessors_v1.h"


/// Helper macro for Accessor objects
#define DEFINE_ACCESSOR(TYPE, NAME )                               \
   case xAOD::NAME:                                                \
   {                                                               \
     static const SG::AuxElement::Accessor< TYPE > a( #NAME );	   \
     return &a;							   \
   }                                                               \
   break;

namespace xAOD {

  const SG::AuxElement::Accessor< int >* taggerInfoAccessorV1int(BTagInfo info ) 
  {   
    switch( info ) {
      DEFINE_ACCESSOR( int, SV0_NGTinJet );
      DEFINE_ACCESSOR( int, SV0_NGTinSvx );
      DEFINE_ACCESSOR( int, SV0_N2Tpair );
      DEFINE_ACCESSOR( int, SV1_NGTinJet );
      DEFINE_ACCESSOR( int, SV1_NGTinSvx );
      DEFINE_ACCESSOR( int, SV1_N2Tpair );
      DEFINE_ACCESSOR( int, JetFitter_nVTX );
      DEFINE_ACCESSOR( int, JetFitter_nSingleTracks );
      DEFINE_ACCESSOR( int, JetFitter_nTracksAtVtx );
      DEFINE_ACCESSOR( int, JetFitter_N2Tpair );
      DEFINE_ACCESSOR( int, IP2D_ntrk );
      DEFINE_ACCESSOR( int, IP3D_ntrk );
    default:                  
      std::cerr << "xAOD::BTagging_v1 ERROR Unknown int TaggerInfo ("
                << info << ") requested" << std::endl;
      return 0;
    }
  }

  const SG::AuxElement::Accessor< float >* taggerInfoAccessorV1float(BTagInfo info ) 
  {   
    switch( info ) {
      DEFINE_ACCESSOR( float, SV0_masssvx );
      DEFINE_ACCESSOR( float, SV0_efracsvx );
      DEFINE_ACCESSOR( float, SV0_normdist );
      DEFINE_ACCESSOR( float, SV1_masssvx );
      DEFINE_ACCESSOR( float, SV1_efracsvx );
      DEFINE_ACCESSOR( float, SV1_normdist );
      DEFINE_ACCESSOR( float, JetFitter_mass );
      DEFINE_ACCESSOR( float, JetFitter_energyFraction );
      DEFINE_ACCESSOR( float, JetFitter_significance3d );
      DEFINE_ACCESSOR( float, JetFitter_deltaeta );
      DEFINE_ACCESSOR( float, JetFitter_deltaphi );
    default:                  
      std::cerr << "xAOD::BTagging_v1 ERROR Unknown float TaggerInfo ("
                << info << ") requested" << std::endl;
      return 0;
    }
  }

  const SG::AuxElement::Accessor< uint8_t >* taggerInfoAccessorV1uint8_t(BTagInfo info ) 
  {   
    switch( info ) {
      //DEFINE_ACCESSOR( uint8_t, IP2D_isFromV0 );
      //DEFINE_ACCESSOR( uint8_t, IP3D_isFromV0 );
    default:                  
      std::cerr << "xAOD::BTagging_v1 ERROR Unknown uint8_t TaggerInfo ("
                << info << ") requested" << std::endl;
      return 0;
    }
  }

  const SG::AuxElement::Accessor< std::string >* taggerInfoAccessorV1string(BTagInfo info ) 
  {   
    switch( info ) {
      //DEFINE_ACCESSOR( std::string, IP2D_gradeName );
      //DEFINE_ACCESSOR( std::string, IP3D_gradeName );
    default:                  
      std::cerr << "xAOD::BTagging_v1 ERROR Unknown std::string TaggerInfo ("
                << info << ") requested" << std::endl;
      return 0;
    }
  }

  const SG::AuxElement::Accessor< std::vector<int> >* taggerInfoAccessorV1vectorOfInts(BTagInfo info ) 
  {   
    switch( info ) {
      DEFINE_ACCESSOR( std::vector<int>, IP2D_gradeOfTracks );
      DEFINE_ACCESSOR( std::vector<int>, IP3D_gradeOfTracks );
    default:                  
      std::cerr << "xAOD::BTagging_v1 ERROR Unknown std::vector<int> TaggerInfo ("
                << info << ") requested" << std::endl;
      return 0;
    }
  }

  const SG::AuxElement::Accessor< std::vector<float> >* taggerInfoAccessorV1vectorOfFloats(BTagInfo info ) 
  {   
    switch( info ) {
      DEFINE_ACCESSOR( std::vector<float>, IP3D_valD0wrtPVofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP3D_sigD0wrtPVofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP3D_valZ0wrtPVofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP3D_sigZ0wrtPVofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP3D_weightBofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP3D_weightUofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP3D_weightCofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP2D_valD0wrtPVofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP2D_sigD0wrtPVofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP2D_weightBofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP2D_weightUofTracks );
      DEFINE_ACCESSOR( std::vector<float>, IP2D_weightCofTracks );
    default:                  
      std::cerr << "xAOD::BTagging_v1 ERROR Unknown std::vector<float> TaggerInfo ("
                << info << ") requested" << std::endl;
      return 0;
    }
  }

  const SG::AuxElement::Accessor< std::vector<bool> >* taggerInfoAccessorV1vectorOfBools(BTagInfo info ) 
  {   
    switch( info ) {
      DEFINE_ACCESSOR( std::vector<bool>, IP3D_flagFromV0ofTracks );
      DEFINE_ACCESSOR( std::vector<bool>, IP2D_flagFromV0ofTracks );
    default:                  
      std::cerr << "xAOD::BTagging_v1 ERROR Unknown std::vector<bool> TaggerInfo ("
                << info << ") requested" << std::endl;
      return 0;
    }
  }

} // namespace xAOD
