/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuon_v1.cxx 706292 2015-11-06 05:33:24Z ssnyder $

// System include(s):
#include <iostream>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuon_v1.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

namespace xAOD {

   L2StandAloneMuon_v1::L2StandAloneMuon_v1()
      : IParticle(), m_p4(), m_p4Cached( false ) {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the xAOD::IParticle functions
   //

   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2StandAloneMuon_v1, float, double,
                                        pt )
   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2StandAloneMuon_v1, float, double,
                                        eta )
   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2StandAloneMuon_v1, float, double,
                                        phi )

   double L2StandAloneMuon_v1::m() const {

      return 105.6583715;
   }

   double L2StandAloneMuon_v1::e() const {

      return p4().E();
   }

   double L2StandAloneMuon_v1::rapidity() const {

      return p4().Rapidity();
   }

   const L2StandAloneMuon_v1::FourMom_t& L2StandAloneMuon_v1::p4() const {

      // Update the cached object if necessary:
      if( ! m_p4Cached ) {
         m_p4Cached = true;
         m_p4.SetPtEtaPhiM( pt(), eta(), phi(), m() );
      }

      // Return the cached object:
      return m_p4;
   }

   Type::ObjectType L2StandAloneMuon_v1::type() const {

      // Create some trigger types in xAODBase in the next round...
      return Type::Other;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the 4-momentum setter functions
   //

   void L2StandAloneMuon_v1::setPt( float pt ) {

      static Accessor< float > acc( "pt" );
      acc( *this ) = pt;
      m_p4Cached = false;
      return;
   }

   void L2StandAloneMuon_v1::setEta( float eta ) {

      static Accessor< float > acc( "eta" );
      acc( *this ) = eta;
      m_p4Cached = false;
      return;
   }

   void L2StandAloneMuon_v1::setPhi( float phi ) {

      static Accessor< float > acc( "phi" );
      acc( *this ) = phi;
      m_p4Cached = false;
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
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         roiWord, setRoIWord )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, int,
                                         sAddress, setSAddress )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         rMS, setRMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         etaMS, setEtaMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         phiMS, setPhiMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         dirPhiMS, setDirPhiMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         zMS, setZMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         dirZMS, setDirZMS )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         beta, setBeta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         barrelRadius, setBarrelRadius )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         barrelSagitta, setBarrelSagitta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         endcapAlpha, setEndcapAlpha )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         endcapBeta, setEndcapBeta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         endcapRadius, setEndcapRadius )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         slopeInner, setSlopeInner )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         interceptInner, setInterceptInner )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         deltaR, setDeltaR )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         etaMap, setEtaMap )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         phiMap, setPhiMap )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, int,
                                         etaBin, setEtaBin )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, int,
                                         phiBin, setPhiBin )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, int,
                                         isTgcFailure, setIsTgcFailure )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, int,
                                         isRpcFailure, setIsRpcFailure )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         deltaPt, setDeltaPt )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         deltaEta, setDeltaEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         deltaPhi, setDeltaPhi )
   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Object for accessing the superPointR variable
   ///
   static SG::AuxElement::Accessor< std::vector< float > >  sprAcc( "superPointR" );
   /// Object for accessing the superPointZ variable
   static SG::AuxElement::Accessor< std::vector< float > >  spzAcc( "superPointZ" );
   /// Object for accessing the superPointSlope variable
   static SG::AuxElement::Accessor< std::vector< float > >  spsAcc( "superPointSlope" );
   /// Object for accessing the superPointIntercept variable
   static SG::AuxElement::Accessor< std::vector< float > >  spiAcc( "superPointIntercept" );
   /// Object for accessing the superPointChi2 variable
   static SG::AuxElement::Accessor< std::vector< float > >  spcAcc( "superPointChi2" );

   /// Get superPoint properties
   float L2StandAloneMuon_v1::superPointR( int chamber ) const {
     if( chamber >= 0 && sprAcc( *this ).size() > (unsigned int)chamber ) {
       return sprAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::superPointZ( int chamber ) const {
     if( chamber >= 0 && spzAcc( *this ).size() > (unsigned int)chamber ) {
       return spzAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::superPointSlope( int chamber ) const {
     if( chamber >= 0 && spsAcc( *this ).size() > (unsigned int)chamber ) {
       return spsAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::superPointIntercept( int chamber ) const {
     if( chamber >= 0 && spiAcc( *this ).size() > (unsigned int)chamber ) {
       return spiAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::superPointChi2( int chamber ) const {
     if( chamber >= 0 && spcAcc( *this ).size() > (unsigned int)chamber ) {
       return spcAcc( *this ).at( chamber );
     } else {
       return 0.;
     }
   }

   /// Set superPoint properties
   void L2StandAloneMuon_v1::setSuperPoint( int chamber, float r, float z,
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
   static SG::AuxElement::Accessor< std::vector< float > >    trkrAcc( "trackPositionR" );
   static SG::AuxElement::Accessor< std::vector< float > >    trkzAcc( "trackPositionZ" );
   static SG::AuxElement::Accessor< std::vector< float > >    trketaAcc( "trackPositionEta" );
   static SG::AuxElement::Accessor< std::vector< float > >    trkphiAcc( "trackPositionPhi" );

   /// Get and set track positions
   uint32_t L2StandAloneMuon_v1::nTrackPositions() const {
     return trkrAcc( *this ).size();
   }

   float L2StandAloneMuon_v1::trackPositionR( unsigned int n ) const {
     if( trkrAcc( *this ).size() > n ) {
       return trkrAcc( *this ).at( n );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::trackPositionZ( unsigned int n ) const {
     if( trkzAcc( *this ).size() > n ) {
       return trkzAcc( *this ).at( n );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::trackPositionEta( unsigned int n ) const {
     if( trketaAcc( *this ).size() > n ) {
       return trketaAcc( *this ).at( n );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::trackPositionPhi( unsigned int n ) const {
     if( trkphiAcc( *this ).size() > n ) {
       return trkphiAcc( *this ).at( n );
     } else {
       return 0.;
     }
   }


   void L2StandAloneMuon_v1::setTrackPosition( float r, float z, float eta, float phi ) {

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
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, int,
                                         algoId, setAlgoId )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         teId, setTeId )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         lvl1Id, setLvl1Id )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         lumiBlock, setLumiBlock )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         muonDetMask, setMuonDetMask )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         roiId, setRoiId )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         roiSystem, setRoiSystem )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         roiSubsystem, setRoiSubsystem )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         roiSector, setRoiSector )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         roiNumber, setRoiNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
                                         roiThreshold, setRoiThreshold )
   //AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
   //                                      roiEta, setRoiEta )
   //AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, uint32_t,
   //                                      roiPhi, setRoiPhi )
   uint32_t L2StandAloneMuon_v1::roiEta() const {
      static Accessor< uint32_t > acc( "roiEtaUint" );
      return acc( *this );                 
   }                                       
   void L2StandAloneMuon_v1::setRoiEta( uint32_t value ) {         
      static Accessor< uint32_t > acc( "roiEtaUint" );
      acc( *this ) = value;                
      return;                              
   }
   uint32_t L2StandAloneMuon_v1::roiPhi() const {                 
      static Accessor< uint32_t > acc( "roiPhiUint" );
      return acc( *this );                 
   }                                       
   void L2StandAloneMuon_v1::setRoiPhi( uint32_t value ) {         
      static Accessor< uint32_t > acc( "roiPhiUint" );
      acc( *this ) = value;                
      return;                              
   }

   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   /// Set and get pT from different algorithms
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         ptBarrelRadius, setPtBarrelRadius )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         ptBarrelSagitta, setPtBarrelSagitta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         ptEndcapAlpha, setPtEndcapAlpha )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         ptEndcapBeta, setPtEndcapBeta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float,
                                         ptEndcapRadius, setPtEndcapRadius )
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Object for accessing the road information
   ///
   /// chamber types
   static SG::AuxElement::Accessor< std::vector< int > >
      ct1nAcc( "chamberType1Normal" );
   static SG::AuxElement::Accessor< std::vector< int > >
      ct1oAcc( "chamberType1Overlap" );
   static SG::AuxElement::Accessor< std::vector< int > >
      ct2nAcc( "chamberType2Normal" );
   static SG::AuxElement::Accessor< std::vector< int > >
      ct2oAcc( "chamberType2Overlap" );

   /// road slope and intercept
   static SG::AuxElement::Accessor< std::vector< float > >
      awnAcc( "roadAwNormal" );
   static SG::AuxElement::Accessor< std::vector< float > >
      awoAcc( "roadAwOverlap" );
   static SG::AuxElement::Accessor< std::vector< float > >
      bwnAcc( "roadBwNormal" );
   static SG::AuxElement::Accessor< std::vector< float > >
      bwoAcc( "roadBwOverlap" );

   /// Z range
   static SG::AuxElement::Accessor< std::vector< float > >
      zminnAcc( "zMinNormal" );
   static SG::AuxElement::Accessor< std::vector< float > >
      zminoAcc( "zMinOverlap" );
   static SG::AuxElement::Accessor< std::vector< float > >
      zmaxnAcc( "zMaxNormal" );
   static SG::AuxElement::Accessor< std::vector< float > >
      zmaxoAcc( "zMaxOverlap" );

   /// R range
   static SG::AuxElement::Accessor< std::vector< float > >
      rminnAcc( "rMinNormal" );
   static SG::AuxElement::Accessor< std::vector< float > >
      rminoAcc( "rMinOverlap" );
   static SG::AuxElement::Accessor< std::vector< float > >
      rmaxnAcc( "rMaxNormal" );
   static SG::AuxElement::Accessor< std::vector< float > >
      rmaxoAcc( "rMaxOverlap" );

   /// Eta range
   static SG::AuxElement::Accessor< std::vector< float > >
      eminnAcc( "etaMinNormal" );
   static SG::AuxElement::Accessor< std::vector< float > >
      eminoAcc( "etaMinOverlap" );
   static SG::AuxElement::Accessor< std::vector< float > >
      emaxnAcc( "etaMaxNormal" );
   static SG::AuxElement::Accessor< std::vector< float > >
      emaxoAcc( "etaMaxOverlap" );

   ///
   /// Get road properties
   ///
   /// Chamber types
   int L2StandAloneMuon_v1::chamberType1( int station, int sector ) const {

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

   int L2StandAloneMuon_v1::chamberType2( int station, int sector ) const {

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
   float L2StandAloneMuon_v1::roadAw( int station, int sector ) const {
 
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
   float L2StandAloneMuon_v1::roadBw( int station, int sector ) const {

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
   float L2StandAloneMuon_v1::zMin( int station, int sector ) const {

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

   float L2StandAloneMuon_v1::zMax( int station, int sector ) const {

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
   float L2StandAloneMuon_v1::rMin( int station, int sector ) const {

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

   float L2StandAloneMuon_v1::rMax( int station, int sector ) const {

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
   float L2StandAloneMuon_v1::etaMin( int station, int sector ) const {

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

   float L2StandAloneMuon_v1::etaMax( int station, int sector ) const {

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
   void L2StandAloneMuon_v1::setChamberType1( int station, int sector, int chamberType ) {

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

   void L2StandAloneMuon_v1::setChamberType2( int station, int sector, int chamberType ) {

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
   void L2StandAloneMuon_v1::setRoad( int station, int sector, float aw, float bw ) {

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
   void L2StandAloneMuon_v1::setRegionZ( int station, int sector, float min, float max ) {

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
   void L2StandAloneMuon_v1::setRegionR( int station, int sector, float min, float max ) {

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
   void L2StandAloneMuon_v1::setRegionEta( int station, int sector, float min, float max ) {

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
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2StandAloneMuon_v1, float, tgcPt, setTgcPt )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcInnEta )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcInnPhi )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcInnR )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcInnZ )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcInnRhoStd )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, long,  tgcInnRhoN )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcInnPhiStd )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, long,  tgcInnPhiN )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMid1Eta )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMid1Phi )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMid1R )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMid1Z )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMid2Eta )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMid2Phi )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMid2R )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMid2Z )

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMidRhoChi2 )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, long,  tgcMidRhoN )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, tgcMidPhiChi2 )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, long,  tgcMidPhiN )

   /// Set the results of TGC fit
   void L2StandAloneMuon_v1::setTgcInn( float eta, float phi, float r, float z ) {

      static Accessor< float > tieAcc( "tgcInnEta" );
      static Accessor< float > tipAcc( "tgcInnPhi" );
      static Accessor< float > tirAcc( "tgcInnR" );
      static Accessor< float > tizAcc( "tgcInnZ" );

      tieAcc( *this ) = eta;
      tipAcc( *this ) = phi;
      tirAcc( *this ) = r;
      tizAcc( *this ) = z;
 
      return;
   }

   void L2StandAloneMuon_v1::setTgcInnF( float rhoStd, long rhoN, float phiStd, long phiN ) {

      static Accessor< float > tirsAcc( "tgcInnRhoStd" );
      static Accessor< long >  tirnAcc( "tgcInnRhoN" );
      static Accessor< float > tipsAcc( "tgcInnPhiStd" );
      static Accessor< long >  tipnAcc( "tgcInnPhiN" );

      tirsAcc( *this ) = rhoStd;
      tirnAcc( *this ) = rhoN;
      tipsAcc( *this ) = phiStd;
      tipnAcc( *this ) = phiN;

      return;
   }

   void L2StandAloneMuon_v1::setTgcMid1( float eta, float phi, float r, float z ) {

      static Accessor< float > tm1eAcc( "tgcMid1Eta" );
      static Accessor< float > tm1pAcc( "tgcMid1Phi" );
      static Accessor< float > tm1rAcc( "tgcMid1R" );
      static Accessor< float > tm1zAcc( "tgcMid1Z" );

      tm1eAcc( *this ) = eta;
      tm1pAcc( *this ) = phi;
      tm1rAcc( *this ) = r;
      tm1zAcc( *this ) = z;

      return;
   }

   void L2StandAloneMuon_v1::setTgcMid2( float eta, float phi, float r, float z ) {

      static Accessor< float > tm2eAcc( "tgcMid2Eta" );
      static Accessor< float > tm2pAcc( "tgcMid2Phi" );
      static Accessor< float > tm2rAcc( "tgcMid2R" );
      static Accessor< float > tm2zAcc( "tgcMid2Z" );

      tm2eAcc( *this ) = eta;
      tm2pAcc( *this ) = phi;
      tm2rAcc( *this ) = r;
      tm2zAcc( *this ) = z;

      return;
   }

   void L2StandAloneMuon_v1::setTgcMidF( float rhoChi2, long rhoN, float phiChi2, long phiN ) {

      static Accessor< float > tmrcAcc( "tgcMidRhoChi2" );
      static Accessor< long >  tmrnAcc( "tgcMidRhoN" );
      static Accessor< float > tmpcAcc( "tgcMidPhiChi2" );
      static Accessor< long >  tmpnAcc( "tgcMidPhiN" );

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
   /// Get the results of RPC fit
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, rpc1x)
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, rpc1y)
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, rpc1z)

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, rpc2x)
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, rpc2y)
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, rpc2z)

   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, rpc3x)
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, rpc3y)
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, float, rpc3z)

   /// Set the results of RPC fit
   void L2StandAloneMuon_v1::setRpc1( float x, float y, float z ) {

      static Accessor< float > r1xAcc( "rpc1x" );
      static Accessor< float > r1yAcc( "rpc1y" );
      static Accessor< float > r1zAcc( "rpc1z" );

      r1xAcc( *this ) = x;
      r1yAcc( *this ) = y;
      r1zAcc( *this ) = z;

      return;
   }

   void L2StandAloneMuon_v1::setRpc2( float x, float y, float z ) {

      static Accessor< float > r2xAcc( "rpc2x" );
      static Accessor< float > r2yAcc( "rpc2y" );
      static Accessor< float > r2zAcc( "rpc2z" );

      r2xAcc( *this ) = x;
      r2yAcc( *this ) = y;
      r2zAcc( *this ) = z;

      return;
   }

   void L2StandAloneMuon_v1::setRpc3( float x, float y, float z ) {

      static Accessor< float > r3xAcc( "rpc3x" );
      static Accessor< float > r3yAcc( "rpc3y" );
      static Accessor< float > r3zAcc( "rpc3z" );

      r3xAcc( *this ) = x;
      r3yAcc( *this ) = y;
      r3zAcc( *this ) = z;

      return;
   }

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Object for accessing extenstion variables
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext0Acc( "extension0" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext1Acc( "extension1" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext2Acc( "extension2" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext3Acc( "extension3" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext4Acc( "extension4" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext5Acc( "extension5" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext6Acc( "extension6" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext7Acc( "extension7" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext8Acc( "extension8" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > ext9Acc( "extension9" );

   /// Object for level 1 emulation info
   static SG::AuxElement::Accessor< std::vector< uint32_t > > lvl1eAcc( "lvl1Emulation" );    

   /// Object for ROB info
   static SG::AuxElement::Accessor< std::vector< uint32_t > > robidAcc( "robId" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > robrmAcc( "removedRobId" );

   /// Object for CSM info
   static SG::AuxElement::Accessor< std::vector< uint32_t > > csmidAcc( "csmId" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > csmszAcc( "csmSize" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > csmerAcc( "csmError" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > csmrmAcc( "removedCsmId" );

   /// Object for accessing RPC pad hits
   static SG::AuxElement::Accessor< std::vector< uint32_t > > padiAcc( "padHitOnlineId" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > padcAcc( "padHitCode" );
   static SG::AuxElement::Accessor< std::vector< float > >    padxAcc( "padHitX" );
   static SG::AuxElement::Accessor< std::vector< float > >    padyAcc( "padHitY" );
   static SG::AuxElement::Accessor< std::vector< float > >    padzAcc( "padHitZ" );
   static SG::AuxElement::Accessor< std::vector< float > >    padrAcc( "padHitR" );
   static SG::AuxElement::Accessor< std::vector< float > >    padpAcc( "padHitP" );

   /// Object for accessing TGC hits
   static SG::AuxElement::Accessor< std::vector< float > >    tirpAcc( "tgcInnRhoHitPhi" );
   static SG::AuxElement::Accessor< std::vector< float > >    tirrAcc( "tgcInnRhoHitR" );
   static SG::AuxElement::Accessor< std::vector< float > >    tirzAcc( "tgcInnRhoHitZ" );
   static SG::AuxElement::Accessor< std::vector< float > >    tirwAcc( "tgcInnRhoHitWidth" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > tiriAcc( "tgcInnRhoHitInSeg" );
   static SG::AuxElement::Accessor< std::vector< float > >    tippAcc( "tgcInnPhiHitPhi" );
   static SG::AuxElement::Accessor< std::vector< float > >    tiprAcc( "tgcInnPhiHitR" );
   static SG::AuxElement::Accessor< std::vector< float > >    tipzAcc( "tgcInnPhiHitZ" );
   static SG::AuxElement::Accessor< std::vector< float > >    tipwAcc( "tgcInnPhiHitWidth" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > tipiAcc( "tgcInnPhiHitInSeg" );
   static SG::AuxElement::Accessor< std::vector< float > >    tmrpAcc( "tgcMidRhoHitPhi" );
   static SG::AuxElement::Accessor< std::vector< float > >    tmrrAcc( "tgcMidRhoHitR" );
   static SG::AuxElement::Accessor< std::vector< float > >    tmrzAcc( "tgcMidRhoHitZ" );
   static SG::AuxElement::Accessor< std::vector< float > >    tmrwAcc( "tgcMidRhoHitWidth" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > tmriAcc( "tgcMidRhoHitInSeg" );
   static SG::AuxElement::Accessor< std::vector< float > >    tmppAcc( "tgcMidPhiHitPhi" );
   static SG::AuxElement::Accessor< std::vector< float > >    tmprAcc( "tgcMidPhiHitR" );
   static SG::AuxElement::Accessor< std::vector< float > >    tmpzAcc( "tgcMidPhiHitZ" );
   static SG::AuxElement::Accessor< std::vector< float > >    tmpwAcc( "tgcMidPhiHitWidth" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > tmpiAcc( "tgcMidPhiHitInSeg" );

   /// Object for accessing the MDT tube variables
   static SG::AuxElement::Accessor< std::vector< uint32_t > > mdtonAcc( "mdtHitOnlineId" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > mdtoffAcc( "mdtHitOfflineId" );
   static SG::AuxElement::Accessor< std::vector< uint32_t > > mdtcAcc( "mdtHitChamber" );
   static SG::AuxElement::Accessor< std::vector< float > >    mdtrAcc( "mdtHitR" );
   static SG::AuxElement::Accessor< std::vector< float > >    mdtzAcc( "mdtHitZ" );
   static SG::AuxElement::Accessor< std::vector< float > >    mdtresAcc( "mdtHitResidual" );
   static SG::AuxElement::Accessor< std::vector< float > >    mdttAcc( "mdtHitTime" );
   static SG::AuxElement::Accessor< std::vector< float > >    mdtspcAcc( "mdtHitSpace" );
   static SG::AuxElement::Accessor< std::vector< float > >    mdtsigAcc( "mdtHitSigma" );
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Size of storage to be reserved
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, int, extensionCapacity )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, int, robCapacity )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, int, csmCapacity )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, int, lvl1EmuCapacity )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, int, rpcHitsCapacity )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, int, tgcHitsCapacity )
   AUXSTORE_PRIMITIVE_GETTER( L2StandAloneMuon_v1, int, mdtHitsCapacity )

   /// Set size of extension stirage
   void L2StandAloneMuon_v1::setExtensionCapacity( int value ) {
 
      static Accessor< int > ecapAcc( "extensionCapacity" );
      ecapAcc( *this ) = value;

      if ( value > 0 ) {
        ext0Acc( *this ).reserve( (unsigned int)value );
        ext1Acc( *this ).reserve( (unsigned int)value );
        ext2Acc( *this ).reserve( (unsigned int)value );
        ext3Acc( *this ).reserve( (unsigned int)value );
        ext4Acc( *this ).reserve( (unsigned int)value );
        ext5Acc( *this ).reserve( (unsigned int)value );
        ext6Acc( *this ).reserve( (unsigned int)value );
        ext7Acc( *this ).reserve( (unsigned int)value );
        ext8Acc( *this ).reserve( (unsigned int)value );
        ext9Acc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

   /// Set size of ROB storage
   void L2StandAloneMuon_v1::setRobCapacity( int value ) {
 
      static Accessor< int > rcapAcc( "robCapacity" );
      rcapAcc( *this ) = value;

      if ( value > 0 ) {
         robidAcc( *this ).reserve( (unsigned int)value );
         robrmAcc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

   /// Set size of CSM storage
   void L2StandAloneMuon_v1::setCsmCapacity( int value ) {
 
      static Accessor< int > ccapAcc( "csmCapacity" );
      ccapAcc( *this ) = value;

      if ( value > 0 ) {
         csmidAcc( *this ).reserve( (unsigned int)value );
         csmszAcc( *this ).reserve( (unsigned int)value );
         csmerAcc( *this ).reserve( (unsigned int)value );
         csmrmAcc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

   /// Set size of L1 emulation storage
   void L2StandAloneMuon_v1::setLvl1EmuCapacity( int value ) {
 
      static Accessor< int > lcapAcc( "lvl1EmuCapacity" );
      lcapAcc( *this ) = value;

      if ( value > 0 ) {
         lvl1eAcc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

   /// Set size of storage for RPC hits
   void L2StandAloneMuon_v1::setRpcHitsCapacity( int value ) {
 
      static Accessor< int > rcapAcc( "rpcHitsCapacity" );
      rcapAcc( *this ) = value;

      if ( value > 0 ) {
         padiAcc( *this ).reserve( (unsigned int)value );
         padcAcc( *this ).reserve( (unsigned int)value );
         padxAcc( *this ).reserve( (unsigned int)value );
         padyAcc( *this ).reserve( (unsigned int)value );
         padzAcc( *this ).reserve( (unsigned int)value );
         padrAcc( *this ).reserve( (unsigned int)value );
         padpAcc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

   /// Set size of storage for TGC hits
   void L2StandAloneMuon_v1::setTgcHitsCapacity( int value ) {
 
      static Accessor< int > tcapAcc( "tgcHitsCapacity" );
      tcapAcc( *this ) = value;

      if ( value > 0 ) {
         tirpAcc( *this ).reserve( (unsigned int)value );
         tirrAcc( *this ).reserve( (unsigned int)value );
         tirzAcc( *this ).reserve( (unsigned int)value );
         tirwAcc( *this ).reserve( (unsigned int)value );
         tiriAcc( *this ).reserve( (unsigned int)value );
         tippAcc( *this ).reserve( (unsigned int)value );
         tiprAcc( *this ).reserve( (unsigned int)value );
         tipzAcc( *this ).reserve( (unsigned int)value );
         tipwAcc( *this ).reserve( (unsigned int)value );
         tipiAcc( *this ).reserve( (unsigned int)value );
         tmrpAcc( *this ).reserve( (unsigned int)value );
         tmrrAcc( *this ).reserve( (unsigned int)value );
         tmrzAcc( *this ).reserve( (unsigned int)value );
         tmrwAcc( *this ).reserve( (unsigned int)value );
         tmriAcc( *this ).reserve( (unsigned int)value );
         tmppAcc( *this ).reserve( (unsigned int)value );
         tmprAcc( *this ).reserve( (unsigned int)value );
         tmpzAcc( *this ).reserve( (unsigned int)value );
         tmpwAcc( *this ).reserve( (unsigned int)value );
         tmpiAcc( *this ).reserve( (unsigned int)value );
      }

      return;
   }

   /// Set size of storage for MDT hits
   void L2StandAloneMuon_v1::setMdtHitsCapacity( int value ) {

      static Accessor< int > mcapAcc( "mdtHitsCapacity" );
      mcapAcc( *this ) = value;

      if ( value > 0 ) {
        mdtonAcc( *this ).reserve( (unsigned int)value );
        mdtoffAcc( *this ).reserve( (unsigned int)value );
        mdtrAcc( *this ).reserve( (unsigned int)value );
        mdtzAcc( *this ).reserve( (unsigned int)value );
        mdtresAcc( *this ).reserve( (unsigned int)value );
        mdttAcc( *this ).reserve( (unsigned int)value );
        mdtspcAcc( *this ).reserve( (unsigned int)value );
        mdtsigAcc( *this ).reserve( (unsigned int)value );
      }
 
      return;
   }


   void L2StandAloneMuon_v1::setCapacities( int ext, int rob, int csm, int lvl1, int rpc, int tgc, int mdt ) {

      setExtensionCapacity( ext );
      setRobCapacity( rob );
      setCsmCapacity( csm );
      setLvl1EmuCapacity( lvl1 );
      setRpcHitsCapacity( rpc );
      setTgcHitsCapacity( tgc );
      setMdtHitsCapacity( mdt );
      
      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   /// Get extension object
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension0 )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension1 )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension2 )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension3 )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension4 )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension5 )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension6 )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension7 )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension8 )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, extension9 )

   /// Set and get methods for extenstions
   void L2StandAloneMuon_v1::setExtension0( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext0Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext0Acc( *this ).push_back( value );
   }
   void L2StandAloneMuon_v1::setExtension1( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext1Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext1Acc( *this ).push_back( value );
   }
   void L2StandAloneMuon_v1::setExtension2( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext2Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext2Acc( *this ).push_back( value );
   }
   void L2StandAloneMuon_v1::setExtension3( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext3Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext3Acc( *this ).push_back( value );
   }
   void L2StandAloneMuon_v1::setExtension4( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext4Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext4Acc( *this ).push_back( value );
   }
   void L2StandAloneMuon_v1::setExtension5( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext5Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext5Acc( *this ).push_back( value );
   }
   void L2StandAloneMuon_v1::setExtension6( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext6Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext6Acc( *this ).push_back( value );
   }
   void L2StandAloneMuon_v1::setExtension7( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext7Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext7Acc( *this ).push_back( value );
   }
   void L2StandAloneMuon_v1::setExtension8( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext8Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext8Acc( *this ).push_back( value );
   }
   void L2StandAloneMuon_v1::setExtension9( uint32_t value ) {
      // Make sure that the size of vector is still within the capacity
     if ( ext9Acc( *this ).size() >= (unsigned int)extensionCapacity() ) return;
      // Set the variables:
      ext9Acc( *this ).push_back( value );
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get level 1 emulation object
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, lvl1Emulation )

   /// Set level 1 emulation object
   void L2StandAloneMuon_v1::setEtaPivot(uint32_t ly0, uint32_t ly1 ) {

     if ( lvl1eAcc( *this ).size() < 2 ) lvl1eAcc( *this ).resize( 2 );

     lvl1eAcc( *this ).at( 0 ) = ly0;
     lvl1eAcc( *this ).at( 1 ) = ly1;

     return;
   }

   void L2StandAloneMuon_v1::setEtaLow( uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11 ) {

     if ( lvl1eAcc( *this ).size() < 6 ) lvl1eAcc( *this ).resize( 6 );

     lvl1eAcc( *this ).at( 2 ) = ly00;
     lvl1eAcc( *this ).at( 3 ) = ly01;
     lvl1eAcc( *this ).at( 4 ) = ly10;
     lvl1eAcc( *this ).at( 5 ) = ly11;

     return;
   }

   void L2StandAloneMuon_v1::setEtaHigh( uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11 ) {

     if ( lvl1eAcc( *this ).size() < 10 ) lvl1eAcc( *this ).resize( 10 );

     lvl1eAcc( *this ).at( 6 ) = ly00;
     lvl1eAcc( *this ).at( 7 ) = ly01;
     lvl1eAcc( *this ).at( 8 ) = ly10;
     lvl1eAcc( *this ).at( 9 ) = ly11;

     return;
   }

   void L2StandAloneMuon_v1::setPhiPivot (uint32_t ly0, uint32_t ly1 ) {

     if ( lvl1eAcc( *this ).size() < 12 ) lvl1eAcc( *this ).resize( 12 );

     lvl1eAcc( *this ).at( 10 ) = ly0;
     lvl1eAcc( *this ).at( 11 ) = ly1;

     return;
   }

   void L2StandAloneMuon_v1::setPhiLow( uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11 ) {

     if ( lvl1eAcc( *this ).size() < 16 ) lvl1eAcc( *this ).resize( 16 );

     lvl1eAcc( *this ).at( 12 ) = ly00;
     lvl1eAcc( *this ).at( 13 ) = ly01;
     lvl1eAcc( *this ).at( 14 ) = ly10;
     lvl1eAcc( *this ).at( 15 ) = ly11;

     return;
   }

   void L2StandAloneMuon_v1::setPhiHigh( uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11 ) {

     if ( lvl1eAcc( *this ).size() < 20 ) lvl1eAcc( *this ).resize( 20 );

     lvl1eAcc( *this ).at( 16 ) = ly00;
     lvl1eAcc( *this ).at( 17 ) = ly01;
     lvl1eAcc( *this ).at( 18 ) = ly10;
     lvl1eAcc( *this ).at( 19 ) = ly11;

     return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get ROB information object
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, robId )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, removedRobId )

   /// Set ROB information object
   void L2StandAloneMuon_v1::setRequestedRob( uint32_t rob ) {

      // Make sure that the size of vector is still within the capacity
      if ( robidAcc( *this ).size() >= (unsigned int)robCapacity() ) return;

      // Set the variables:
      robidAcc( *this ).push_back( rob );

      return;
   }

   void L2StandAloneMuon_v1::setRemovedRob( uint32_t rob ) {

      // Make sure that the size of vector is still within the capacity
      if ( robrmAcc( *this ).size() >= (unsigned int)robCapacity() ) return;

      // Set the variables:
      robrmAcc( *this ).push_back( rob );

      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get CSM information object
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, csmId )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, csmSize )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, csmError )
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, removedCsmId )

   /// Set CSM information object
   void L2StandAloneMuon_v1::setConvertedCsm( uint32_t csm, uint32_t sz ) {

      // Make sure that the size of vector is still within the capacity
      if ( csmidAcc( *this ).size() >= (unsigned int)csmCapacity() ) return;

      // Set the variables:
      csmidAcc( *this ).push_back( csm );
      csmszAcc( *this ).push_back( sz );

      return;
   }

   void L2StandAloneMuon_v1::setCsmError( uint32_t err ) {

      // Make sure that the size of vector is still within the capacity
      if ( csmerAcc( *this ).size() >= (unsigned int)csmCapacity() ) return;

      // Set the variables:
      csmerAcc( *this ).push_back( err );

      return;
   }


  void L2StandAloneMuon_v1::setRemovedCsm( uint32_t csm ) {
      // Make sure that the size of vector is still within the capacity
      if ( csmrmAcc( *this ).size() >= (unsigned int)csmCapacity() ) return;

      // Set the variables:
      csmrmAcc( *this ).push_back( csm );

      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get RPC pad object
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, padHitOnlineId)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, padHitCode)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    padHitX)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    padHitY)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    padHitZ)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    padHitR)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    padHitP)

   /// Set RPC pad hits
   void L2StandAloneMuon_v1::setPadHit(uint32_t onlineId, uint32_t code, float x, float y, float z,
                                       float r, float p) {
      // Make sure that the size of vector is still within the capacity
      if ( padiAcc( *this ).size() >= (unsigned int)rpcHitsCapacity() ) return;

      // Set the variables:
      padiAcc( *this ).push_back( onlineId );
      padcAcc( *this ).push_back( code );
      padxAcc( *this ).push_back( x );
      padyAcc( *this ).push_back( y );
      padzAcc( *this ).push_back( z );
      padrAcc( *this ).push_back( r );
      padpAcc( *this ).push_back( p );

      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get TGC hits
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcInnRhoHitPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcInnRhoHitR)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcInnRhoHitZ)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcInnRhoHitWidth)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, tgcInnRhoHitInSeg)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcInnPhiHitPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcInnPhiHitR)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcInnPhiHitZ)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcInnPhiHitWidth)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, tgcInnPhiHitInSeg)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcMidRhoHitPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcMidRhoHitR)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcMidRhoHitZ)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcMidRhoHitWidth)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, tgcMidRhoHitInSeg)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcMidPhiHitPhi)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcMidPhiHitR)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcMidPhiHitZ)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< float >,    tgcMidPhiHitWidth)
   AUXSTORE_OBJECT_GETTER( L2StandAloneMuon_v1, std::vector< uint32_t >, tgcMidPhiHitInSeg)

   /// Set TGC hits
   void L2StandAloneMuon_v1::setTgcInnRhoHit( float phi, float r, float z, float width, uint32_t in_seg ) {

      // Make sure that the size of vector is still within the capacity
      if ( tirpAcc( *this ).size() >= (unsigned int)tgcHitsCapacity() ) return;

      // Set the variables:
      tirpAcc( *this ).push_back( phi );
      tirrAcc( *this ).push_back( r );
      tirzAcc( *this ).push_back( z );
      tirwAcc( *this ).push_back( width );
      tiriAcc( *this ).push_back( in_seg );

      return;
   }

   void L2StandAloneMuon_v1::setTgcInnPhiHit( float phi, float r, float z, float width, uint32_t in_seg ) {

      // Make sure that the size of vector is still within the capacity
      if ( tippAcc( *this ).size() >= (unsigned int)tgcHitsCapacity() ) return;

      // Set the variables:
      tippAcc( *this ).push_back( phi );
      tiprAcc( *this ).push_back( r );
      tipzAcc( *this ).push_back( z );
      tipwAcc( *this ).push_back( width );
      tipiAcc( *this ).push_back( in_seg );

      return;
   }

   void L2StandAloneMuon_v1::setTgcMidRhoHit( float phi, float r, float z, float width, uint32_t in_seg ) {

      // Make sure that the size of vector is still within the capacity
      if ( tmrpAcc( *this ).size() >= (unsigned int)tgcHitsCapacity() ) return;

      // Set the variables:
      tmrpAcc( *this ).push_back( phi );
      tmrrAcc( *this ).push_back( r );
      tmrzAcc( *this ).push_back( z );
      tmrwAcc( *this ).push_back( width );
      tmriAcc( *this ).push_back( in_seg );

      return;
   }

   void L2StandAloneMuon_v1::setTgcMidPhiHit( float phi, float r, float z, float width, uint32_t in_seg ) {

      // Make sure that the size of vector is still within the capacity
      if ( tmppAcc( *this ).size() >= (unsigned int)tgcHitsCapacity() ) return;

      // Set the variables:
      tmppAcc( *this ).push_back( phi );
      tmprAcc( *this ).push_back( r );
      tmpzAcc( *this ).push_back( z );
      tmpwAcc( *this ).push_back( width );
      tmpiAcc( *this ).push_back( in_seg );

      return;
   }
   ///
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   ///
   /// Get and set MDT hits
   uint32_t L2StandAloneMuon_v1::nMdtHits() const {
     return mdtonAcc( *this ).size();
   }

   uint32_t L2StandAloneMuon_v1::mdtHitOnlineId( unsigned int tube ) const {
     if( mdtonAcc( *this ).size() > tube ) {
       return mdtonAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   uint32_t L2StandAloneMuon_v1::mdtHitOfflineId( unsigned int tube ) const {
     if( mdtoffAcc( *this ).size() > tube ) {
       return mdtoffAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   uint32_t L2StandAloneMuon_v1::mdtHitChamber( unsigned int tube ) const {
     if( mdtcAcc( *this ).size() > tube ) {
       return mdtcAcc( *this ).at( tube );
     } else {
       return 0;
     }
   }

   float L2StandAloneMuon_v1::mdtHitR( unsigned int tube ) const {
     if( mdtrAcc( *this ).size() > tube ) {
       return mdtrAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::mdtHitZ( unsigned int tube ) const {
     if( mdtzAcc( *this ).size() > tube ) {
       return mdtzAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::mdtHitResidual( unsigned int tube ) const {
     if( mdtresAcc( *this ).size() > tube ) {
       return mdtresAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::mdtHitTime( unsigned int tube ) const {
     if( mdttAcc( *this ).size() > tube ) {
       return mdttAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::mdtHitSpace( unsigned int tube ) const {
     if( mdtspcAcc( *this ).size() > tube ) {
       return mdtspcAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   float L2StandAloneMuon_v1::mdtHitSigma( unsigned int tube ) const {
     if( mdtsigAcc( *this ).size() > tube ) {
       return mdtsigAcc( *this ).at( tube );
     } else {
       return 0.;
     }
   }

   void L2StandAloneMuon_v1::setMdtHit( uint32_t onlineId, uint32_t offlineId, int chamber, float r, float z,
                                        float residual, float time, float space, float sigma ) {

      // Make sure that the number of MDT hits is still within the capacity
     if ( mdtonAcc( *this ).size() >= (unsigned int)mdtHitsCapacity() ) return;

      // Set the variables:
      mdtonAcc( *this ).push_back( onlineId );
      mdtoffAcc( *this ).push_back( offlineId );
      mdtcAcc( *this ).push_back( chamber );
      mdtrAcc( *this ).push_back( r );
      mdtzAcc( *this ).push_back( z );
      mdtresAcc( *this ).push_back( residual );
      mdttAcc( *this ).push_back( time );
      mdtspcAcc( *this ).push_back( space );
      mdtsigAcc( *this ).push_back( sigma );

      return;
   }
   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD

/// Helper operator for easily printing the properties of a LVL2 muon
/// object for debugging.
///
std::ostream& operator<< ( std::ostream& out,
                           const xAOD::L2StandAloneMuon_v1& mu ) {

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
   out << "extensionCapacity: " << mu.extensionCapacity() << "; ";
   out << "robCapacity: "       << mu.robCapacity()       << "; ";
   out << "csmCapacity: "       << mu.csmCapacity()       << "; ";
   out << "lvl1EmuCapacity: "   << mu.lvl1EmuCapacity()   << "; ";
   out << "rpcHitsCapacity: "   << mu.rpcHitsCapacity()   << "; ";
   out << "tgcHitsCapacity: "   << mu.tgcHitsCapacity()   << "; ";
   out << "mdtHitsCapacity: "   << mu.mdtHitsCapacity()   << "; ";

   // Return the stream:
   return out;
}
