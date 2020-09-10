/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <iostream>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuon_v2.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

namespace xAOD {

   L2StandAloneMuon_v2::L2StandAloneMuon_v2()
      : IParticle() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the xAOD::IParticle functions
   //

   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2StandAloneMuon_v2, float, double,
                                        pt )
   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2StandAloneMuon_v2, float, double,
                                        eta )
   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2StandAloneMuon_v2, float, double,
                                        phi )

   double L2StandAloneMuon_v2::m() const {

      return 105.6583715;
   }

   double L2StandAloneMuon_v2::e() const {

      return genvecP4().E();
   }

   double L2StandAloneMuon_v2::rapidity() const {

      return genvecP4().Rapidity();
   }

   L2StandAloneMuon_v2::FourMom_t L2StandAloneMuon_v2::p4() const {
     FourMom_t p4;
     p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
     return p4;	
   }

   /// this provides a GenVector (pt, eta, phi, m)
   L2StandAloneMuon_v2::GenVecFourMom_t L2StandAloneMuon_v2::genvecP4() const {
     return GenVecFourMom_t(pt(), eta(), phi(), m());
   } 

   Type::ObjectType L2StandAloneMuon_v2::type() const {

      // Create some trigger types in xAODBase in the next round...
      return Type::Other;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the 4-momentum setter functions
   //

   void L2StandAloneMuon_v2::setPt( float pt ) {

      static const Accessor< float > acc( "pt" );
      acc( *this ) = pt;
      return;
   }

   void L2StandAloneMuon_v2::setEta( float eta ) {

      static const Accessor< float > acc( "eta" );
      acc( *this ) = eta;
      return;
   }

   void L2StandAloneMuon_v2::setPhi( float phi ) {

      static const Accessor< float > acc( "phi" );
      acc( *this ) = phi;
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation of the LVL2 muon specific functions
   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Set and get methods for the outputs of L2 muon standalone muon algorithm
   ///
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         roiWord, setRoIWord )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, int,
                                         sAddress, setSAddress )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         rMS, setRMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         etaMS, setEtaMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         phiMS, setPhiMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         dirPhiMS, setDirPhiMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         zMS, setZMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         dirZMS, setDirZMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         beta, setBeta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         barrelRadius, setBarrelRadius )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         barrelSagitta, setBarrelSagitta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         endcapAlpha, setEndcapAlpha )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         endcapBeta, setEndcapBeta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         endcapRadius, setEndcapRadius )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         etaMap, setEtaMap )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         phiMap, setPhiMap )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, int,
                                         etaBin, setEtaBin )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, int,
                                         phiBin, setPhiBin )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, int,
                                         isTgcFailure, setIsTgcFailure )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, int,
                                         isRpcFailure, setIsRpcFailure )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         deltaPt, setDeltaPt )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         deltaPtParm1, setDeltaPtParm1 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         deltaPtParm2, setDeltaPtParm2 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         deltaPtParm3, setDeltaPtParm3 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         deltaEta, setDeltaEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         deltaPhi, setDeltaPhi )
   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Object for accessing the superPointR variable
   ///
   static const SG::AuxElement::Accessor< std::vector< float > >  sprAcc( "superPointR" );
   /// Object for accessing the superPointZ variable
   static const SG::AuxElement::Accessor< std::vector< float > >  spzAcc( "superPointZ" );
   /// Object for accessing the superPointSlope variable
   static const SG::AuxElement::Accessor< std::vector< float > >  spsAcc( "superPointSlope" );
   /// Object for accessing the superPointIntercept variable
   static const SG::AuxElement::Accessor< std::vector< float > >  spiAcc( "superPointIntercept" );
   /// Object for accessing the superPointChi2 variable
   static const SG::AuxElement::Accessor< std::vector< float > >  spcAcc( "superPointChi2" );

   /// Get superPoint properties
   float L2StandAloneMuon_v2::superPointR( int chamber ) const {
     if( chamber >= 0 && sprAcc( *this ).size() > (unsigned int)chamber ) {
       return sprAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::superPointZ( int chamber ) const {
     if( chamber >= 0 && spzAcc( *this ).size() > (unsigned int)chamber ) {
       return spzAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::superPointSlope( int chamber ) const {
     if( chamber >= 0 && spsAcc( *this ).size() > (unsigned int)chamber ) {
       return spsAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::superPointIntercept( int chamber ) const {
     if( chamber >= 0 && spiAcc( *this ).size() > (unsigned int)chamber ) {
       return spiAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::superPointChi2( int chamber ) const {
     if( chamber >= 0 && spcAcc( *this ).size() > (unsigned int)chamber ) {
       return spcAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   /// Set superPoint properties
   void L2StandAloneMuon_v2::setSuperPoint( int chamber, float r, float z,
                                            float slope, float intercept, float chi2 ) {

      // Make sure that the variables are big enough:
      if( sprAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         sprAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( spzAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         spzAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( spsAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         spsAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( spiAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         spiAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( spcAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         spcAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }

      if (chamber >=0 && chamber < L2MuonParameters::Chamber::MaxChamber ) {
         // Set the variables:
         sprAcc( *this ).at( chamber ) = r;
         spzAcc( *this ).at( chamber ) = z;
         spsAcc( *this ).at( chamber ) = slope;
         spiAcc( *this ).at( chamber ) = intercept;
         spcAcc( *this ).at( chamber ) = chi2;
      }

      return;
   }

   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Object for accessing track position variables
   static const SG::AuxElement::Accessor< std::vector< float > >    trkrAcc( "trackPositionR" );
   static const SG::AuxElement::Accessor< std::vector< float > >    trkzAcc( "trackPositionZ" );
   static const SG::AuxElement::Accessor< std::vector< float > >    trketaAcc( "trackPositionEta" );
   static const SG::AuxElement::Accessor< std::vector< float > >    trkphiAcc( "trackPositionPhi" );

   /// Get and set track positions
   uint32_t L2StandAloneMuon_v2::nTrackPositions() const {
     return trkrAcc( *this ).size();
   }

   float L2StandAloneMuon_v2::trackPositionR( unsigned int n ) const {
     if( trkrAcc( *this ).size() > n ) {
       return trkrAcc( *this ).at( n );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::trackPositionZ( unsigned int n ) const {
     if( trkzAcc( *this ).size() > n ) {
       return trkzAcc( *this ).at( n );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::trackPositionEta( unsigned int n ) const {
     if( trketaAcc( *this ).size() > n ) {
       return trketaAcc( *this ).at( n );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::trackPositionPhi( unsigned int n ) const {
     if( trkphiAcc( *this ).size() > n ) {
       return trkphiAcc( *this ).at( n );
     } else {
       return 0.;
     }
   }


   void L2StandAloneMuon_v2::setTrackPosition( float r, float z, float eta, float phi ) {

      // Set the variables:
      trkrAcc( *this ).push_back( r );
      trkzAcc( *this ).push_back( z );
      trketaAcc( *this ).push_back( eta );
      trkphiAcc( *this ).push_back( phi );

      return;
   }
   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   /// Below are detailed information
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   /// L1 information
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, int,
                                         algoId, setAlgoId )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         teId, setTeId )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         lvl1Id, setLvl1Id )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         lumiBlock, setLumiBlock )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         muonDetMask, setMuonDetMask )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         roiId, setRoiId )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         roiSystem, setRoiSystem )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         roiSubsystem, setRoiSubsystem )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         roiSector, setRoiSector )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         roiNumber, setRoiNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, uint32_t,
                                         roiThreshold, setRoiThreshold )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         roiEta, setRoiEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         roiPhi, setRoiPhi )
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   /// Set and get pT from different algorithms
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         tgcPt, setTgcPt )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         ptBarrelRadius, setPtBarrelRadius )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         ptBarrelSagitta, setPtBarrelSagitta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         ptEndcapAlpha, setPtEndcapAlpha )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         ptEndcapBeta, setPtEndcapBeta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         ptEndcapRadius, setPtEndcapRadius )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v2, float,
                                         ptCSC, setPtCSC )
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Object for accessing the road information
   ///
   /// chamber types
   static const SG::AuxElement::Accessor< std::vector< int > >
      ct1nAcc( "chamberType1Normal" );
   static const SG::AuxElement::Accessor< std::vector< int > >
      ct1oAcc( "chamberType1Overlap" );
   static const SG::AuxElement::Accessor< std::vector< int > >
      ct2nAcc( "chamberType2Normal" );
   static const SG::AuxElement::Accessor< std::vector< int > >
      ct2oAcc( "chamberType2Overlap" );

   /// road slope and intercept
   static const SG::AuxElement::Accessor< std::vector< float > >
      awnAcc( "roadAwNormal" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      awoAcc( "roadAwOverlap" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      bwnAcc( "roadBwNormal" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      bwoAcc( "roadBwOverlap" );

   /// Z range
   static const SG::AuxElement::Accessor< std::vector< float > >
      zminnAcc( "zMinNormal" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      zminoAcc( "zMinOverlap" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      zmaxnAcc( "zMaxNormal" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      zmaxoAcc( "zMaxOverlap" );

   /// R range
   static const SG::AuxElement::Accessor< std::vector< float > >
      rminnAcc( "rMinNormal" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      rminoAcc( "rMinOverlap" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      rmaxnAcc( "rMaxNormal" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      rmaxoAcc( "rMaxOverlap" );

   /// Eta range
   static const SG::AuxElement::Accessor< std::vector< float > >
      eminnAcc( "etaMinNormal" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      eminoAcc( "etaMinOverlap" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      emaxnAcc( "etaMaxNormal" );
   static const SG::AuxElement::Accessor< std::vector< float > >
      emaxoAcc( "etaMaxOverlap" );

   ///
   /// Get road properties
   ///
   /// Chamber types
   int L2StandAloneMuon_v2::chamberType1( int station, int sector ) const {

     if ( station < 0 ) return 0;

     if ( sector == 0 ) {
       /// normal sector

       if ( ct1nAcc( *this ).size() > (unsigned int)station ) {
          return ct1nAcc( *this ).at( station );
       } else {
          return 0;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( ct1oAcc( *this ).size() > (unsigned int)station ) {
          return ct1oAcc( *this ).at( station );
       } else {
          return 0;
       }

     }

     return 0;
   }

   int L2StandAloneMuon_v2::chamberType2( int station, int sector ) const {

     if ( station < 0 ) return 0;

     if ( sector == 0 ) {
       /// normal sector
       if ( ct2nAcc( *this ).size() > (unsigned int)station ) {
          return ct2nAcc( *this ).at( station );
       } else {
          return 0;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( ct2oAcc( *this ).size() > (unsigned int)station ) {
          return ct2oAcc( *this ).at( station );
       } else {
          return 0;
       }

     }

     return 0;
   }

   /// Road slope
   float L2StandAloneMuon_v2::roadAw( int station, int sector ) const {
 
    if ( station < 0 ) return 0;

    if ( sector == 0 ) {
       /// normal sector
       if ( awnAcc( *this ).size() > (unsigned int)station ) {
          return awnAcc( *this ).at( station );
       } else {
          return 0.;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( awoAcc( *this ).size() > (unsigned int)station ) {
          return awoAcc( *this ).at( station );
       } else {
          return 0.;
       }

     }

     return 0.;
   }

   /// Road intercept
   float L2StandAloneMuon_v2::roadBw( int station, int sector ) const {

     if ( station < 0 ) return 0;

     if ( sector == 0 ) {
       /// normal sector
       if ( bwnAcc( *this ).size() > (unsigned int)station ) {
          return bwnAcc( *this ).at( station );
       } else {
          return 0.;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( bwoAcc( *this ).size() > (unsigned int)station ) {
          return bwoAcc( *this ).at( station );
       } else {
          return 0.;
       }

     }

     return 0.;
   }

   /// Z range
   float L2StandAloneMuon_v2::zMin( int station, int sector ) const {

     if ( station < 0 ) return 0;

     if ( sector == 0 ) {
       /// normal sector
       if ( zminnAcc( *this ).size() > (unsigned int)station ) {
          return zminnAcc( *this ).at( station );
       } else {
          return 0.;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( zminoAcc( *this ).size() > (unsigned int)station ) {
          return zminoAcc( *this ).at( station );
       } else {
          return 0.;
       }

     }

     return 0.;
   }

   float L2StandAloneMuon_v2::zMax( int station, int sector ) const {

     if ( station < 0 ) return 0;

     if ( sector == 0 ) {
       /// normal sector
       if ( zmaxnAcc( *this ).size() > (unsigned int)station ) {
          return zmaxnAcc( *this ).at( station );
       } else {
          return 0.;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( zmaxoAcc( *this ).size() > (unsigned int)station ) {
          return zmaxoAcc( *this ).at( station );
       } else {
          return 0.;
       }

     }

     return 0.;
   }

   /// R range
   float L2StandAloneMuon_v2::rMin( int station, int sector ) const {

     if ( station < 0 ) return 0;

     if ( sector == 0 ) {
       /// normal sector
       if ( rminnAcc( *this ).size() > (unsigned int)station ) {
          return rminnAcc( *this ).at( station );
       } else {
          return 0.;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( rminoAcc( *this ).size() > (unsigned int)station ) {
          return rminoAcc( *this ).at( station );
       } else {
          return 0.;
       }

     }

     return 0.;
   }

   float L2StandAloneMuon_v2::rMax( int station, int sector ) const {

     if ( station < 0 ) return 0;

     if ( sector == 0 ) {
       /// normal sector
       if ( rmaxnAcc( *this ).size() > (unsigned int)station ) {
          return rmaxnAcc( *this ).at( station );
       } else {
          return 0.;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( rmaxoAcc( *this ).size() > (unsigned int)station ) {
          return rmaxoAcc( *this ).at( station );
       } else {
          return 0.;
       }

     }

     return 0.;
   }

   /// Eta range
   float L2StandAloneMuon_v2::etaMin( int station, int sector ) const {

     if ( station < 0 ) return 0;

     if ( sector == 0 ) {
       /// normal sector
       if ( eminnAcc( *this ).size() > (unsigned int)station ) {
          return eminnAcc( *this ).at( station );
       } else {
          return 0.;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( eminoAcc( *this ).size() > (unsigned int)station ) {
          return eminoAcc( *this ).at( station );
       } else {
          return 0.;
       }

     }

     return 0.;
   }

   float L2StandAloneMuon_v2::etaMax( int station, int sector ) const {

     if ( station < 0 ) return 0;

     if ( sector == 0 ) {
       /// normal sector
       if ( emaxnAcc( *this ).size() > (unsigned int)station ) {
          return emaxnAcc( *this ).at( station );
       } else {
          return 0.;
       }

     } else if ( sector == 1 ) {
       /// overlap sector

       if ( emaxoAcc( *this ).size() > (unsigned int)station ) {
          return emaxoAcc( *this ).at( station );
       } else {
          return 0.;
       }

     }

     return 0.;
   }


   /// Set road properties

   /// Chamber types
   void L2StandAloneMuon_v2::setChamberType1( int station, int sector, int chamberType ) {

      // Make sure that the variables are big enough:
      if( ct1nAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         ct1nAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( ct1oAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         ct1oAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }

      // Make sure that the variables are big enough:
      if ( station < 0 || station >= L2MuonParameters::Chamber::MaxChamber ) return;
      if ( sector < 0 || sector > 1 ) return;

      if (sector == 0 ) {
      /// Normal sector
        ct1nAcc( *this ).at( station ) = chamberType;

      } else if ( sector == 1 ) {
      /// Overlap sector
        ct1oAcc( *this ).at( station ) = chamberType;

      }

      return;
   }

   void L2StandAloneMuon_v2::setChamberType2( int station, int sector, int chamberType ) {

      // Make sure that the variables are big enough:
      if( ct2nAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         ct2nAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( ct2oAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         ct2oAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }

      // Make sure that the variables are big enough:
      if ( station < 0 || station >= L2MuonParameters::Chamber::MaxChamber ) return;
      if ( sector < 0 || sector > 1 ) return;

      if (sector == 0 ) {
      /// Normal sector
        ct2nAcc( *this ).at( station ) = chamberType;

      } else if ( sector == 1 ) {
      /// Overlap sector
        ct2oAcc( *this ).at( station ) = chamberType;

      }

      return;
   }

   /// Road
   void L2StandAloneMuon_v2::setRoad( int station, int sector, float aw, float bw ) {

      // Make sure that the variables are big enough:
      if( awnAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         awnAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( bwnAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         bwnAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( awoAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         awoAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( bwoAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         bwoAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }

      // Make sure that the variables are big enough:
      if ( station < 0 || station >= L2MuonParameters::Chamber::MaxChamber ) return;
      if ( sector < 0 || sector > 1 ) return;

      if (sector == 0 ) {
      /// Normal sector

        /// Slope
        awnAcc( *this ).at( station ) = aw;

        /// Intercept
        bwnAcc( *this ).at( station ) = bw;

      } else if ( sector == 1 ) {
      /// Overlap sector

        /// Slope
        awoAcc( *this ).at( station ) = aw;

        /// Intercept
        bwoAcc( *this ).at( station ) = bw;

      }

      return;
   }

   /// Z range
   void L2StandAloneMuon_v2::setRegionZ( int station, int sector, float min, float max ) {

      // Make sure that the variables are big enough:
      if( zminnAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         zminnAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( zmaxnAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         zmaxnAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( zminoAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         zminoAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( zmaxoAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         zmaxoAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }

      // Make sure that the variables are big enough:
      if ( station < 0 || station >= L2MuonParameters::Chamber::MaxChamber ) return;
      if ( sector < 0 || sector > 1 ) return;

      if (sector == 0 ) {
      /// Normal sector

        /// minimum Z
        zminnAcc( *this ).at( station ) = min;

        /// mazimum Z
        zmaxnAcc( *this ).at( station ) = max;

      } else if ( sector == 1 ) {
      /// Overlap sector

        /// minimum Z
        zminoAcc( *this ).at( station ) = min;

        /// maximum Z
        zmaxoAcc( *this ).at( station ) = max;

      }

      return;
   }

   /// R range
   void L2StandAloneMuon_v2::setRegionR( int station, int sector, float min, float max ) {

      // Make sure that the variables are big enough:
      if( rminnAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         rminnAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( rmaxnAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         rmaxnAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( rminoAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         rminoAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( rmaxoAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         rmaxoAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }

      // Make sure that the variables are big enough:
      if ( station < 0 || station >= L2MuonParameters::Chamber::MaxChamber ) return;
      if ( sector < 0 || sector > 1 ) return;

      if (sector == 0 ) {
      /// Normal sector

        /// minimum R
        rminnAcc( *this ).at( station ) = min;

        /// maximum R
        rmaxnAcc( *this ).at( station ) = max;

      } else if ( sector == 1 ) {
      /// Overlap sector

        /// minimum R
        rminoAcc( *this ).at( station ) = min;

        /// maximum R
        rmaxoAcc( *this ).at( station ) = max;

      }

      return;
   }

   /// Eta range
   void L2StandAloneMuon_v2::setRegionEta( int station, int sector, float min, float max ) {

      // Make sure that the variables are big enough:
      if( eminnAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         eminnAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( emaxnAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         emaxnAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( eminoAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         eminoAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }
      if( emaxoAcc( *this ).size() < L2MuonParameters::Chamber::MaxChamber ) {
         emaxoAcc( *this ).resize( L2MuonParameters::Chamber::MaxChamber, 0.0 );
      }

      // Make sure that the variables are big enough:
      if ( station < 0 || station >= L2MuonParameters::Chamber::MaxChamber ) return;
      if ( sector < 0 || sector > 1 ) return;

      if (sector == 0 ) {
      /// Normal sector

        /// minimum eta
        eminnAcc( *this ).at( station ) = min;

        /// maximum eta
        emaxnAcc( *this ).at( station ) = max;

      } else if ( sector == 1 ) {
      /// Overlap sector

        /// minimum eta
        eminoAcc( *this ).at( station ) = min;

        /// maximum eta
        emaxoAcc( *this ).at( station ) = max;

      }

      return;
   }

   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get the results of TGC fit
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcInnEta )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcInnPhi )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcInnR )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcInnZ )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcInnRhoStd )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, long,  tgcInnRhoN )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcInnPhiStd )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, long,  tgcInnPhiN )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMid1Eta )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMid1Phi )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMid1R )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMid1Z )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMid2Eta )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMid2Phi )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMid2R )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMid2Z )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMidRhoChi2 )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, long,  tgcMidRhoN )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, tgcMidPhiChi2 )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, long,  tgcMidPhiN )

   /// Set the results of TGC fit
   void L2StandAloneMuon_v2::setTgcInn( float eta, float phi, float r, float z ) {

      static const Accessor< float > tieAcc( "tgcInnEta" );
      static const Accessor< float > tipAcc( "tgcInnPhi" );
      static const Accessor< float > tirAcc( "tgcInnR" );
      static const Accessor< float > tizAcc( "tgcInnZ" );

      tieAcc( *this ) = eta;
      tipAcc( *this ) = phi;
      tirAcc( *this ) = r;
      tizAcc( *this ) = z;
 
      return;
   }

   void L2StandAloneMuon_v2::setTgcInnF( float rhoStd, long rhoN, float phiStd, long phiN ) {

      static const Accessor< float > tirsAcc( "tgcInnRhoStd" );
      static const Accessor< long >  tirnAcc( "tgcInnRhoN" );
      static const Accessor< float > tipsAcc( "tgcInnPhiStd" );
      static const Accessor< long >  tipnAcc( "tgcInnPhiN" );

      tirsAcc( *this ) = rhoStd;
      tirnAcc( *this ) = rhoN;
      tipsAcc( *this ) = phiStd;
      tipnAcc( *this ) = phiN;

      return;
   }

   void L2StandAloneMuon_v2::setTgcMid1( float eta, float phi, float r, float z ) {

      static const Accessor< float > tm1eAcc( "tgcMid1Eta" );
      static const Accessor< float > tm1pAcc( "tgcMid1Phi" );
      static const Accessor< float > tm1rAcc( "tgcMid1R" );
      static const Accessor< float > tm1zAcc( "tgcMid1Z" );

      tm1eAcc( *this ) = eta;
      tm1pAcc( *this ) = phi;
      tm1rAcc( *this ) = r;
      tm1zAcc( *this ) = z;

      return;
   }

   void L2StandAloneMuon_v2::setTgcMid2( float eta, float phi, float r, float z ) {

      static const Accessor< float > tm2eAcc( "tgcMid2Eta" );
      static const Accessor< float > tm2pAcc( "tgcMid2Phi" );
      static const Accessor< float > tm2rAcc( "tgcMid2R" );
      static const Accessor< float > tm2zAcc( "tgcMid2Z" );

      tm2eAcc( *this ) = eta;
      tm2pAcc( *this ) = phi;
      tm2rAcc( *this ) = r;
      tm2zAcc( *this ) = z;

      return;
   }

   void L2StandAloneMuon_v2::setTgcMidF( float rhoChi2, long rhoN, float phiChi2, long phiN ) {

      static const Accessor< float > tmrcAcc( "tgcMidRhoChi2" );
      static const Accessor< long >  tmrnAcc( "tgcMidRhoN" );
      static const Accessor< float > tmpcAcc( "tgcMidPhiChi2" );
      static const Accessor< long >  tmpnAcc( "tgcMidPhiN" );

      tmrcAcc( *this ) = rhoChi2;
      tmrnAcc( *this ) = rhoN;
      tmpcAcc( *this ) = phiChi2;
      tmpnAcc( *this ) = phiN;

      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get the results of TGC fit
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, rpcFitInnPhi )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, rpcFitInnSlope )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, rpcFitInnOffset )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, rpcFitMidPhi )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, rpcFitMidSlope )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, rpcFitMidOffset )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, rpcFitOutPhi )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, rpcFitOutSlope )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, float, rpcFitOutOffset )

   void L2StandAloneMuon_v2::setRpcFitInn( float phi, float slope, float offset ) {

      static const Accessor< float > rfipAcc( "rpcFitInnPhi" );
      static const Accessor< float > rfisAcc( "rpcFitInnSlope" );
      static const Accessor< float > rfioAcc( "rpcFitInnOffset" );

      rfipAcc( *this ) = phi;
      rfisAcc( *this ) = slope;
      rfioAcc( *this ) = offset;

      return;
   }

   void L2StandAloneMuon_v2::setRpcFitMid( float phi, float slope, float offset ) {

      static const Accessor< float > rfmpAcc( "rpcFitMidPhi" );
      static const Accessor< float > rfmsAcc( "rpcFitMidSlope" );
      static const Accessor< float > rfmoAcc( "rpcFitMidOffset" );

      rfmpAcc( *this ) = phi;
      rfmsAcc( *this ) = slope;
      rfmoAcc( *this ) = offset;

      return;
   }

   void L2StandAloneMuon_v2::setRpcFitOut( float phi, float slope, float offset ) {

      static const Accessor< float > rfopAcc( "rpcFitOutPhi" );
      static const Accessor< float > rfosAcc( "rpcFitOutSlope" );
      static const Accessor< float > rfooAcc( "rpcFitOutOffset" );

      rfopAcc( *this ) = phi;
      rfosAcc( *this ) = slope;
      rfooAcc( *this ) = offset;

      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Object for accessing RPC hits
   static const SG::AuxElement::Accessor< std::vector< uint32_t > > rpclAcc( "rpcHitLayer" );
   static const SG::AuxElement::Accessor< std::vector< uint32_t > > rpcmpAcc( "rpcHitMeasuresPhi" );
   static const SG::AuxElement::Accessor< std::vector< float > >    rpcxAcc( "rpcHitX" );
   static const SG::AuxElement::Accessor< std::vector< float > >    rpcyAcc( "rpcHitY" );
   static const SG::AuxElement::Accessor< std::vector< float > >    rpczAcc( "rpcHitZ" );
   static const SG::AuxElement::Accessor< std::vector< float > >    rpctAcc( "rpcHitTime" );
   static const SG::AuxElement::Accessor< std::vector< float > >    rpcdeAcc( "rpcHitDistToEtaReadout" );
   static const SG::AuxElement::Accessor< std::vector< float > >    rpcdpAcc( "rpcHitDistToPhiReadout" );
   static const SG::AuxElement::Accessor< std::vector< std::string > > rpcsnAcc( "rpcHitStationName" );

   /// Object for accessing TGC hits
   static const SG::AuxElement::Accessor< std::vector< float > >    tgceAcc( "tgcHitEta" );
   static const SG::AuxElement::Accessor< std::vector< float > >    tgcpAcc( "tgcHitPhi" );
   static const SG::AuxElement::Accessor< std::vector< float > >    tgcrAcc( "tgcHitR" );
   static const SG::AuxElement::Accessor< std::vector< float > >    tgczAcc( "tgcHitZ" );
   static const SG::AuxElement::Accessor< std::vector< float > >    tgcwAcc( "tgcHitWidth" );
   static const SG::AuxElement::Accessor< std::vector< int > >      tgcsnAcc( "tgcHitStationNum" );
   static const SG::AuxElement::Accessor< std::vector< bool > >     tgcisAcc( "tgcHitIsStrip" );
   static const SG::AuxElement::Accessor< std::vector< int > >      tgcbcAcc( "tgcHitBCTag" );
   static const SG::AuxElement::Accessor< std::vector< bool > >     tgcirAcc( "tgcHitInRoad" );

   /// Object for accessing the MDT tube variables
   static const SG::AuxElement::Accessor< std::vector< uint32_t > > mdtonAcc( "mdtHitOnlineId" );
   static const SG::AuxElement::Accessor< std::vector< int > >      mdtioAcc( "mdtHitIsOutlier" );
   static const SG::AuxElement::Accessor< std::vector< int > >      mdtcAcc( "mdtHitChamberId" );
   static const SG::AuxElement::Accessor< std::vector< float > >    mdtrAcc( "mdtHitR" );
   static const SG::AuxElement::Accessor< std::vector< float > >    mdtzAcc( "mdtHitZ" );
   static const SG::AuxElement::Accessor< std::vector< float > >    mdtpAcc( "mdtHitPhi" );
   static const SG::AuxElement::Accessor< std::vector< float > >    mdtresAcc( "mdtHitResidual" );
   static const SG::AuxElement::Accessor< std::vector< float > >    mdttAcc( "mdtHitTime" );
   static const SG::AuxElement::Accessor< std::vector< float > >    mdtspcAcc( "mdtHitSpace" );
   static const SG::AuxElement::Accessor< std::vector< float > >    mdtsigAcc( "mdtHitSigma" );

   /// Object for accessing the CSC tube variables
   static const SG::AuxElement::Accessor< std::vector< int > >      cscioAcc( "cscHitIsOutlier" );
   static const SG::AuxElement::Accessor< std::vector< int > >      csccAcc( "cscHitChamberId" );
   static const SG::AuxElement::Accessor< std::vector< uint32_t > > cscsnAcc( "cscHitStationName" );
   static const SG::AuxElement::Accessor< std::vector< int > >      cscseAcc( "cscHitStationEta" );
   static const SG::AuxElement::Accessor< std::vector< int > >      cscspAcc( "cscHitStationPhi" );
   static const SG::AuxElement::Accessor< std::vector< int > >      cscclAcc( "cscHitChamberLayer" );
   static const SG::AuxElement::Accessor< std::vector< int > >      cscwlAcc( "cscHitWireLayer" );
   static const SG::AuxElement::Accessor< std::vector< int > >      cscmpAcc( "cscHitMeasuresPhi" );
   static const SG::AuxElement::Accessor< std::vector< int > >      cscsAcc( "cscHitStrip" );
   static const SG::AuxElement::Accessor< std::vector< float > >    csceAcc( "cscHitEta" );
   static const SG::AuxElement::Accessor< std::vector< float > >    cscpAcc( "cscHitPhi" );
   static const SG::AuxElement::Accessor< std::vector< float > >    cscrAcc( "cscHitR" );
   static const SG::AuxElement::Accessor< std::vector< float > >    csczAcc( "cscHitZ" );
   static const SG::AuxElement::Accessor< std::vector< int > >      cscchAcc( "cscHitCharge" );
   static const SG::AuxElement::Accessor< std::vector< float > >    cscresAcc( "cscHitTime" );
   static const SG::AuxElement::Accessor< std::vector< float > >    csctAcc( "cscHitResidual" );

   /// Object for accessing sTGC clusters
   static const SG::AuxElement::Accessor< std::vector< unsigned int > > stgclAcc( "stgcClusterLayer" );
   static const SG::AuxElement::Accessor< std::vector< int > >          stgcioAcc( "stgcClusterIsOutlier" );
   static const SG::AuxElement::Accessor< std::vector< int > >          stgctyAcc( "stgcClusterType" );
   static const SG::AuxElement::Accessor< std::vector< float > >        stgceAcc( "stgcClusterEta" );
   static const SG::AuxElement::Accessor< std::vector< float > >        stgcpAcc( "stgcClusterPhi" );
   static const SG::AuxElement::Accessor< std::vector< float > >        stgcrAcc( "stgcClusterR" );
   static const SG::AuxElement::Accessor< std::vector< float > >        stgczAcc( "stgcClusterZ" );
   static const SG::AuxElement::Accessor< std::vector< float > >        stgcrsrAcc( "stgcClusterResidualR" );
   static const SG::AuxElement::Accessor< std::vector< float > >        stgcrspAcc( "stgcClusterResidualPhi" );
   static const SG::AuxElement::Accessor< std::vector< int > >          stgcseAcc( "stgcClusterStationEta" );
   static const SG::AuxElement::Accessor< std::vector< int > >          stgcspAcc( "stgcClusterStationPhi" );
   static const SG::AuxElement::Accessor< std::vector< int > >          stgcsnAcc( "stgcClusterStationName" );

   /// Object for accessing MM clusters
   static const SG::AuxElement::Accessor< std::vector< unsigned int > > mmlAcc( "mmClusterLayer" );
   static const SG::AuxElement::Accessor< std::vector< int > >          mmioAcc( "mmClusterIsOutlier" );
   static const SG::AuxElement::Accessor< std::vector< float > >        mmeAcc( "mmClusterEta" );
   static const SG::AuxElement::Accessor< std::vector< float > >        mmpAcc( "mmClusterPhi" );
   static const SG::AuxElement::Accessor< std::vector< float > >        mmrAcc( "mmClusterR" );
   static const SG::AuxElement::Accessor< std::vector< float > >        mmzAcc( "mmClusterZ" );
   static const SG::AuxElement::Accessor< std::vector< float > >        mmrsrAcc( "mmClusterResidualR" );
   static const SG::AuxElement::Accessor< std::vector< float > >        mmrspAcc( "mmClusterResidualPhi" );
   static const SG::AuxElement::Accessor< std::vector< int > >          mmseAcc( "mmClusterStationEta" );
   static const SG::AuxElement::Accessor< std::vector< int > >          mmspAcc( "mmClusterStationPhi" );
   static const SG::AuxElement::Accessor< std::vector< int > >          mmsnAcc( "mmClusterStationName" );
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Size of storage to be reserved
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, int, rpcHitsCapacity )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, int, tgcHitsCapacity )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, int, mdtHitsCapacity )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v2, int, cscHitsCapacity )

   /// Set size of storage for RPC hits
   void L2StandAloneMuon_v2::setRpcHitsCapacity( int value ) {
 
      static const Accessor< int > rcapAcc( "rpcHitsCapacity" );
      rcapAcc( *this ) = value;

      if ( value > 0 ) {
         rpclAcc( *this ).reserve( (unsigned int)value );
         rpcmpAcc( *this ).reserve( (unsigned int)value );
         rpcxAcc( *this ).reserve( (unsigned int)value );
         rpcyAcc( *this ).reserve( (unsigned int)value );
         rpczAcc( *this ).reserve( (unsigned int)value );
         rpctAcc( *this ).reserve( (unsigned int)value );
         rpcdeAcc( *this ).reserve( (unsigned int)value );
         rpcdpAcc( *this ).reserve( (unsigned int)value );
         rpcsnAcc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

   /// Set size of storage for TGC hits
   void L2StandAloneMuon_v2::setTgcHitsCapacity( int value ) {
 
      static const Accessor< int > tcapAcc( "tgcHitsCapacity" );
      tcapAcc( *this ) = value;

      if ( value > 0 ) {
         tgceAcc( *this ).reserve( (unsigned int)value );
         tgcpAcc( *this ).reserve( (unsigned int)value );
         tgcrAcc( *this ).reserve( (unsigned int)value );
         tgczAcc( *this ).reserve( (unsigned int)value );
         tgcwAcc( *this ).reserve( (unsigned int)value );
         tgcsnAcc( *this ).reserve( (unsigned int)value );
         tgcisAcc( *this ).reserve( (unsigned int)value );
         tgcbcAcc( *this ).reserve( (unsigned int)value );
         tgcirAcc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

   /// Set size of storage for MDT hits
   void L2StandAloneMuon_v2::setMdtHitsCapacity( int value ) {

      static const Accessor< int > mcapAcc( "mdtHitsCapacity" );
      mcapAcc( *this ) = value;

      if ( value > 0 ) {
        mdtonAcc( *this ).reserve( (unsigned int)value );
        mdtioAcc( *this ).reserve( (unsigned int)value );
        mdtrAcc( *this ).reserve( (unsigned int)value );
        mdtzAcc( *this ).reserve( (unsigned int)value );
        mdtpAcc( *this ).reserve( (unsigned int)value );
        mdtresAcc( *this ).reserve( (unsigned int)value );
        mdttAcc( *this ).reserve( (unsigned int)value );
        mdtspcAcc( *this ).reserve( (unsigned int)value );
        mdtsigAcc( *this ).reserve( (unsigned int)value );
      }
 
      return;
   }

   /// Set size of storage for CSC hits
   void L2StandAloneMuon_v2::setCscHitsCapacity( int value ) {

      static const Accessor< int > ccapAcc( "cscHitsCapacity" );
      ccapAcc( *this ) = value;

      if ( value > 0 ) {
        cscioAcc( *this ).reserve( (unsigned int)value );
        csccAcc( *this ).reserve( (unsigned int)value );
        cscsnAcc( *this ).reserve( (unsigned int)value );
        cscseAcc( *this ).reserve( (unsigned int)value );
        cscspAcc( *this ).reserve( (unsigned int)value );
        cscclAcc( *this ).reserve( (unsigned int)value );
        cscwlAcc( *this ).reserve( (unsigned int)value );
        cscmpAcc( *this ).reserve( (unsigned int)value );
        cscsAcc( *this ).reserve( (unsigned int)value );
        csceAcc( *this ).reserve( (unsigned int)value );
        cscpAcc( *this ).reserve( (unsigned int)value );
        cscrAcc( *this ).reserve( (unsigned int)value );
        csczAcc( *this ).reserve( (unsigned int)value );
        cscchAcc( *this ).reserve( (unsigned int)value );
        csctAcc( *this ).reserve( (unsigned int)value );
        cscresAcc( *this ).reserve( (unsigned int)value );
      }
 
      return;
   }

   /// Set size of storage for sTGC clusters
   void L2StandAloneMuon_v2::setStgcClustersCapacity( const int value ) {

      if ( value > 0 ) {
         stgclAcc( *this ).reserve( (unsigned int)value );
         stgcioAcc( *this ).reserve( (unsigned int)value );
         stgctyAcc( *this ).reserve( (unsigned int)value );
         stgceAcc( *this ).reserve( (unsigned int)value );
         stgcpAcc( *this ).reserve( (unsigned int)value );
         stgcrAcc( *this ).reserve( (unsigned int)value );
         stgczAcc( *this ).reserve( (unsigned int)value );
	 stgcrsrAcc( *this ).reserve( (unsigned int)value );
	 stgcrspAcc( *this ).reserve( (unsigned int)value );
	 stgcseAcc( *this ).reserve( (unsigned int)value );
	 stgcspAcc( *this ).reserve( (unsigned int)value );
	 stgcsnAcc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

   /// Set size of storage for MM clusters
   void L2StandAloneMuon_v2::setMmClustersCapacity( const int value ) {

      if ( value > 0 ) {
         mmlAcc( *this ).reserve( (unsigned int)value );
         mmioAcc( *this ).reserve( (unsigned int)value );
         mmeAcc( *this ).reserve( (unsigned int)value );
         mmpAcc( *this ).reserve( (unsigned int)value );
         mmrAcc( *this ).reserve( (unsigned int)value );
         mmzAcc( *this ).reserve( (unsigned int)value );
	 mmrsrAcc( *this ).reserve( (unsigned int)value );
	 mmrspAcc( *this ).reserve( (unsigned int)value );
	 mmseAcc( *this ).reserve( (unsigned int)value );
	 mmspAcc( *this ).reserve( (unsigned int)value );
	 mmsnAcc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

  void L2StandAloneMuon_v2::setCapacities( int rpc, int tgc, int mdt, int csc ) {

      setRpcHitsCapacity( rpc );
      setTgcHitsCapacity( tgc );
      setMdtHitsCapacity( mdt );
      setCscHitsCapacity( csc );
      
      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get RPC object
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< uint32_t >, rpcHitLayer)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< uint32_t >, rpcHitMeasuresPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,    rpcHitX)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,    rpcHitY)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,    rpcHitZ)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,    rpcHitTime)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,    rpcHitDistToEtaReadout)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,    rpcHitDistToPhiReadout)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< std::string >, rpcHitStationName)

   /// Set RPC rpc hits
   void L2StandAloneMuon_v2::setRpcHit(uint32_t layer, uint32_t measuresPhi, float x, float y, float z,
                                       float time, float distEta, float distPhi, std::string stationName ) {
      // Make sure that the size of vector is still within the capacity
      if ( rpclAcc( *this ).size() >= (unsigned int)rpcHitsCapacity() ) return;

      // Set the variables:
      rpclAcc( *this ).push_back( layer );
      rpcmpAcc( *this ).push_back( measuresPhi );
      rpcxAcc( *this ).push_back( x );
      rpcyAcc( *this ).push_back( y );
      rpczAcc( *this ).push_back( z );
      rpctAcc( *this ).push_back( time );
      rpcdeAcc( *this ).push_back( distEta );
      rpcdpAcc( *this ).push_back( distPhi );
      rpcsnAcc( *this ).push_back( stationName );

      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get TGC hits
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,   tgcHitEta )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,   tgcHitPhi )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,   tgcHitR )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,   tgcHitZ )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,   tgcHitWidth )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,     tgcHitStationNum )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< bool >,    tgcHitIsStrip )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,     tgcHitBCTag )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< bool >,    tgcHitInRoad )

   /// Set the results of TGC fit
   void L2StandAloneMuon_v2::setTgcHit( float eta, float phi, float r, float z, float width,
                                        int stationName, bool isStrip, int bcTag, bool inRoad ) {

      // Make sure that the size of vector is still within the capacity
      if ( tgceAcc( *this ).size() >= (unsigned int)tgcHitsCapacity() ) return;

      // Set the varables:
      tgceAcc( *this ).push_back( eta );
      tgcpAcc( *this ).push_back( phi );
      tgcrAcc( *this ).push_back( r );
      tgczAcc( *this ).push_back( z );
      tgcwAcc( *this ).push_back( width );
      tgcsnAcc( *this ).push_back( stationName );
      tgcisAcc( *this ).push_back( isStrip );
      tgcbcAcc( *this ).push_back( bcTag );
      tgcirAcc( *this ).push_back( inRoad );
 
      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get and set MDT hits
   uint32_t L2StandAloneMuon_v2::nMdtHits() const {
     return mdtonAcc( *this ).size();
   }

   uint32_t L2StandAloneMuon_v2::mdtHitOnlineId( unsigned int tube ) const {
     if( mdtonAcc( *this ).size() > tube ) {
       return mdtonAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::mdtHitOfflineId( unsigned int tube ) const {
     if( mdtioAcc( *this ).size() > tube ) {
       return mdtioAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::mdtHitIsOutlier( unsigned int tube ) const {
     if( mdtioAcc( *this ).size() > tube ) {
       return mdtioAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::mdtHitChamber( unsigned int tube ) const {
     if( mdtcAcc( *this ).size() > tube ) {
       return mdtcAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   float L2StandAloneMuon_v2::mdtHitR( unsigned int tube ) const {
     if( mdtrAcc( *this ).size() > tube ) {
       return mdtrAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::mdtHitZ( unsigned int tube ) const {
     if( mdtzAcc( *this ).size() > tube ) {
       return mdtzAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::mdtHitPhi( unsigned int tube ) const {
     if( mdtpAcc( *this ).size() > tube ) {
       return mdtpAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::mdtHitResidual( unsigned int tube ) const {
     if( mdtresAcc( *this ).size() > tube ) {
       return mdtresAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::mdtHitTime( unsigned int tube ) const {
     if( mdttAcc( *this ).size() > tube ) {
       return mdttAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::mdtHitSpace( unsigned int tube ) const {
     if( mdtspcAcc( *this ).size() > tube ) {
       return mdtspcAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::mdtHitSigma( unsigned int tube ) const {
     if( mdtsigAcc( *this ).size() > tube ) {
       return mdtsigAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   void L2StandAloneMuon_v2::setMdtHit( uint32_t onlineId, int isOutlier, int chamber, float r, float z, float phi,
                                        float residual, float time, float space, float sigma ) {

      // Make sure that the number of MDT hits is still within the capacity
     if ( mdtonAcc( *this ).size() >= (unsigned int)mdtHitsCapacity() ) return;

      // Set the variables:
      mdtonAcc( *this ).push_back( onlineId );
      mdtioAcc( *this ).push_back( isOutlier );
      mdtcAcc( *this ).push_back( chamber );
      mdtrAcc( *this ).push_back( r );
      mdtzAcc( *this ).push_back( z );
      mdtpAcc( *this ).push_back( phi );
      mdtresAcc( *this ).push_back( residual );
      mdttAcc( *this ).push_back( time );
      mdtspcAcc( *this ).push_back( space );
      mdtsigAcc( *this ).push_back( sigma );

      return;
   }
   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get and set CSC hits
   uint32_t L2StandAloneMuon_v2::nCscHits() const {
     return cscioAcc( *this ).size();
   }

   int L2StandAloneMuon_v2::cscHitIsOutlier( unsigned int tube ) const {
     if( cscioAcc( *this ).size() > tube ) {
       return cscioAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::cscHitChamber( unsigned int tube ) const {
     if( csccAcc( *this ).size() > tube ) {
       return csccAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   uint32_t L2StandAloneMuon_v2::cscHitStationName( unsigned int tube ) const {
     if( cscsnAcc( *this ).size() > tube ) {
       return cscsnAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::cscHitStationEta( unsigned int tube ) const {
     if( cscseAcc( *this ).size() > tube ) {
       return cscseAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::cscHitStationPhi( unsigned int tube ) const {
     if( cscspAcc( *this ).size() > tube ) {
       return cscspAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::cscHitChamberLayer( unsigned int tube ) const {
     if( cscclAcc( *this ).size() > tube ) {
       return cscclAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::cscHitWireLayer( unsigned int tube ) const {
     if( cscwlAcc( *this ).size() > tube ) {
       return cscwlAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::cscHitMeasuresPhi( unsigned int tube ) const {
     if( cscmpAcc( *this ).size() > tube ) {
       return cscmpAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   int L2StandAloneMuon_v2::cscHitStrip( unsigned int tube ) const {
     if( cscsAcc( *this ).size() > tube ) {
       return cscsAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   float L2StandAloneMuon_v2::cscHitEta( unsigned int tube ) const {
     if( csceAcc( *this ).size() > tube ) {
       return csceAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::cscHitPhi( unsigned int tube ) const {
     if( cscpAcc( *this ).size() > tube ) {
       return cscpAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::cscHitR( unsigned int tube ) const {
     if( cscrAcc( *this ).size() > tube ) {
       return cscrAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::cscHitZ( unsigned int tube ) const {
     if( csczAcc( *this ).size() > tube ) {
       return csczAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   int L2StandAloneMuon_v2::cscHitCharge( unsigned int tube ) const {
     if( cscchAcc( *this ).size() > tube ) {
       return cscchAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   float L2StandAloneMuon_v2::cscHitTime( unsigned int tube ) const {
     if( csctAcc( *this ).size() > tube ) {
       return csctAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v2::cscHitResidual( unsigned int tube ) const {
     if( cscresAcc( *this ).size() > tube ) {
       return cscresAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   void L2StandAloneMuon_v2::setCscHit( int isOutlier, int chamber, uint32_t stationName,
                                        int stationEta, int stationPhi, int chamberLayer, int wireLayer, int measuresPhi, int strip,
                                        float eta, float phi, float r, float z, int charge, float time, float residual ) {

      // Make sure that the number of CSC hits is still within the capacity
     if ( cscioAcc( *this ).size() >= (unsigned int)cscHitsCapacity() ) return;

      // Set the variables:
      cscioAcc( *this ).push_back( isOutlier );
      csccAcc( *this ).push_back( chamber );
      cscsnAcc( *this ).push_back( stationName );
      cscseAcc( *this ).push_back( stationEta );
      cscspAcc( *this ).push_back( stationPhi );
      cscclAcc( *this ).push_back( chamberLayer );
      cscwlAcc( *this ).push_back( wireLayer );
      cscmpAcc( *this ).push_back( measuresPhi );
      cscsAcc( *this ).push_back( strip );
      csceAcc( *this ).push_back( eta );
      cscpAcc( *this ).push_back( phi );
      cscrAcc( *this ).push_back( r );
      csczAcc( *this ).push_back( z );
      cscchAcc( *this ).push_back( charge );
      csctAcc( *this ).push_back( time );
      cscresAcc( *this ).push_back( residual );

      return;
   }
   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get sTGC object
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< unsigned int >, stgcClusterLayer)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,          stgcClusterIsOutlier)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,          stgcClusterType)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        stgcClusterEta)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        stgcClusterPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        stgcClusterR)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        stgcClusterZ)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        stgcClusterResidualR)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        stgcClusterResidualPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,          stgcClusterStationEta)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,          stgcClusterStationPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,          stgcClusterStationName)

   /// Set RPC rpc hits
   void L2StandAloneMuon_v2::setStgcCluster(const unsigned int layer, const int isOutlier, const int type,
					    const float eta, const float phi, const float r, const float z,
					    const float residualR, const float residualPhi,
					    const int stationEta, const int stationPhi, const int stationName ) {
      // Set the variables:
      stgclAcc( *this ).push_back( layer );
      stgcioAcc( *this ).push_back( isOutlier );
      stgctyAcc( *this ).push_back( type );
      stgceAcc( *this ).push_back( eta );
      stgcpAcc( *this ).push_back( phi );
      stgcrAcc( *this ).push_back( r );
      stgczAcc( *this ).push_back( z );
      stgcrsrAcc( *this ).push_back( residualR );
      stgcrspAcc( *this ).push_back( residualPhi );
      stgcseAcc( *this ).push_back( stationEta );
      stgcspAcc( *this ).push_back( stationPhi );
      stgcsnAcc( *this ).push_back( stationName );

      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get MM object
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< unsigned int >, mmClusterLayer)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,          mmClusterIsOutlier)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        mmClusterEta)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        mmClusterPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        mmClusterR)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        mmClusterZ)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        mmClusterResidualR)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< float >,        mmClusterResidualPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,          mmClusterStationEta)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,          mmClusterStationPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v2, std::vector< int >,          mmClusterStationName)

   /// Set RPC rpc hits
   void L2StandAloneMuon_v2::setMmCluster(const unsigned int layer, const int isOutlier,
					  const float eta, const float phi, const float r, const float z,
					  const float residualR, const float residualPhi,
					  const int stationEta, const int stationPhi, const int stationName ) {
      // Set the variables:
      mmlAcc( *this ).push_back( layer );
      mmioAcc( *this ).push_back( isOutlier );
      mmeAcc( *this ).push_back( eta );
      mmpAcc( *this ).push_back( phi );
      mmrAcc( *this ).push_back( r );
      mmzAcc( *this ).push_back( z );
      mmrsrAcc( *this ).push_back( residualR );
      mmrspAcc( *this ).push_back( residualPhi );
      mmseAcc( *this ).push_back( stationEta );
      mmspAcc( *this ).push_back( stationPhi );
      mmsnAcc( *this ).push_back( stationName );

      return;
   }
   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD

/// Helper operator for easily printing the properties of a LVL2 muon
/// object for debugging.
///
std::ostream& operator<< ( std::ostream& out,
                           const xAOD::L2StandAloneMuon_v2& mu ) {

   out << "roiWord: "       << mu.roiWord()       << "; ";
   out << "sAddress: "      << mu.sAddress()      << "; ";
   out << "pt: "            << mu.pt()            << "; ";
   out << "phi: "           << mu.phi()           << "; ";
   out << "eta: "           << mu.eta()           << "; ";
   out << "rMS: "           << mu.rMS()           << "; ";
   out << "etaMS: "         << mu.etaMS()         << "; ";
   out << "phiMS: "         << mu.phiMS()         << "; ";
   out << "dirPhiMS: "      << mu.dirPhiMS()      << "; ";
   out << "zMS: "           << mu.zMS()           << "; ";
   out << "dirZMS: "        << mu.dirZMS()        << "; ";
   out << "beta: "          << mu.beta()          << "; ";
   out << "barrelRadius: "  << mu.barrelRadius()  << "; ";
   out << "barrelSagitta: " << mu.barrelSagitta() << "; ";
   out << "endcapAlpha: "   << mu.endcapAlpha()   << "; ";
   out << "endcapBeta: "    << mu.endcapBeta()    << "; ";
   out << "endcapRadius: "  << mu.endcapRadius()  << "; ";
   out << "etaMap: "        << mu.etaMap()        << "; ";
   out << "phiMap: "        << mu.phiMap()        << "; ";
   out << "etaBin: "        << mu.etaBin()        << "; ";
   out << "phiBin: "        << mu.phiBin()        << "; ";
   out << "isTgcFailure: "  << mu.isTgcFailure()  << "; ";
   out << "isRpcFailure: "  << mu.isRpcFailure()  << "; ";
   out << "deltaPt: "       << mu.deltaPt()       << "; ";
   out << "deltaEta: "      << mu.deltaEta()      << "; ";
   out << "deltaPhi: "      << mu.deltaPhi()      << "; ";
   out << "algoId: "        << mu.algoId()        << "; ";
   out << "teId: "          << mu.teId()          << "; ";
   out << "lvl1Id: "        << mu.lvl1Id()        << "; ";
   out << "lumiBlock: "     << mu.lumiBlock()     << "; ";
   out << "muonDetMask: "   << mu.muonDetMask()   << "; ";
   out << "roiId: "         << mu.roiId()         << "; ";
   out << "roiSystem: "     << mu.roiSystem()     << "; ";
   out << "roiSubsystem: "  << mu.roiSubsystem()  << "; ";
   out << "roiSector: "     << mu.roiSector()     << "; ";
   out << "roiNumber: "     << mu.roiNumber()     << "; ";
   out << "roiThreshold: "  << mu.roiThreshold()  << "; ";
   out << "roiEta: "        << mu.roiEta()        << "; ";
   out << "roiPhi: "        << mu.roiPhi()        << "; ";
   out << "rpcHitsCapacity: "   << mu.rpcHitsCapacity()   << "; ";
   out << "tgcHitsCapacity: "   << mu.tgcHitsCapacity()   << "; ";
   out << "mdtHitsCapacity: "   << mu.mdtHitsCapacity()   << "; ";
   out << "cscHitsCapacity: "   << mu.cscHitsCapacity()   << "; ";

   // Return the stream:
   return out;
}
