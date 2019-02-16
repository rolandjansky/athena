// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_JETACCESSSORMAP_V1_H
#define XAODJET_JETACCESSSORMAP_V1_H
///////////////////////////////////////////
/// This header defines functions to map an Attribute enum to 
/// a static accessor object for the corresponding attribute.
///
/// This file contains and defines the implementation for the _v1 version
/// of the attributes and associated object content of Jet.
/////////////////////////////////////////////
#include <iostream>

// EDM include(s):
#include "xAODBase/IParticleContainer.h"

#include "AthContainers/AuxElement.h"
#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetAccessors.h"
#include "xAODJet/JetTypes.h"

#define JET_DEFINE_ATT_ACCESSOR( NAME )                                 \
  case xAOD::JetAttribute::NAME:                                        \
  {                                                                     \
    static  AccessorWrapper< T > a( #NAME );   \
    return &a;                                                          \
  }                                                                     \
  break



#define JET_DEFINE_ASSO_ACCESSOR( NAME )            \
  case xAOD::JetAttribute::NAME:                    \
  {                                                 \
    static VectorIParticleAccessor_t a( #NAME );    \
    return &a;                                      \
  }                                                 \
  break

#define JET_DEFINE_ASSOBJ_ACCESSOR( NAME )            \
  case xAOD::JetAttribute::NAME:                    \
  {                                                 \
    static  ObjectAccessorWrapper< T > a( #NAME );   \
    return &a;                                      \
  }                                                 \
  break


namespace xAOD {

 
  namespace JetAttributeAccessor {

    /// Define a template struct. 
    template<class T>
    struct AccessorMap_v1 {
      static  AccessorWrapper<T>*
      accessor( xAOD::JetAttribute::AttributeID moment ) {
        switch( moment ) {          

          JET_DEFINE_ATT_ACCESSOR( JetGhostArea );
          JET_DEFINE_ATT_ACCESSOR( ActiveArea );
          JET_DEFINE_ATT_ACCESSOR( ActiveArea4vec );
          JET_DEFINE_ATT_ACCESSOR( VoronoiArea );
          JET_DEFINE_ATT_ACCESSOR( VoronoiAreaE );
          JET_DEFINE_ATT_ACCESSOR( VoronoiAreaPx );
          JET_DEFINE_ATT_ACCESSOR( VoronoiAreaPy );
          JET_DEFINE_ATT_ACCESSOR( VoronoiAreaPz );
          JET_DEFINE_ATT_ACCESSOR( GhostMuonSegmentCount );
          JET_DEFINE_ATT_ACCESSOR( GhostTrackCount );
          JET_DEFINE_ATT_ACCESSOR( GhostTruthParticleCount );
          JET_DEFINE_ATT_ACCESSOR( TransformType );
          JET_DEFINE_ATT_ACCESSOR( MuMax );
          JET_DEFINE_ATT_ACCESSOR( YMin ); 
          JET_DEFINE_ATT_ACCESSOR( RClus );
          JET_DEFINE_ATT_ACCESSOR( BDRS );
          JET_DEFINE_ATT_ACCESSOR( DRFilt ); 
          JET_DEFINE_ATT_ACCESSOR( YFilt ); 
          JET_DEFINE_ATT_ACCESSOR( MuFilt );
          JET_DEFINE_ATT_ACCESSOR( NSubjet );
          JET_DEFINE_ATT_ACCESSOR( PtFrac );
          JET_DEFINE_ATT_ACCESSOR( NTrimSubjets );
          JET_DEFINE_ATT_ACCESSOR( RCut ); 
          JET_DEFINE_ATT_ACCESSOR( ZCut );
          JET_DEFINE_ATT_ACCESSOR( JetOriginConstitScaleMomentum );      
          JET_DEFINE_ATT_ACCESSOR( GhostTruthAssociationFraction );
          JET_DEFINE_ATT_ACCESSOR( GhostTrackAssociationFraction );
          JET_DEFINE_ATT_ACCESSOR( HECQuality );
          JET_DEFINE_ATT_ACCESSOR( LArBadHVEnergy );
          JET_DEFINE_ATT_ACCESSOR( LArBadHVRatio );
          JET_DEFINE_ATT_ACCESSOR( LArQuality );
          JET_DEFINE_ATT_ACCESSOR( NegativeE );
          JET_DEFINE_ATT_ACCESSOR( AverageLArQF );
          JET_DEFINE_ATT_ACCESSOR( OotFracClusters10 );
          JET_DEFINE_ATT_ACCESSOR( OotFracClusters5 );
          JET_DEFINE_ATT_ACCESSOR( OotFracCells5 );
          JET_DEFINE_ATT_ACCESSOR( OotFracCells10 );
          JET_DEFINE_ATT_ACCESSOR( Timing );
          JET_DEFINE_ATT_ACCESSOR( N90Constituents );
          JET_DEFINE_ATT_ACCESSOR( N90Cells );
          JET_DEFINE_ATT_ACCESSOR( BchCorrCell ); 
          JET_DEFINE_ATT_ACCESSOR( IsoDeltaR2SumPt );
          JET_DEFINE_ATT_ACCESSOR( IsoDeltaR3SumPt );
          JET_DEFINE_ATT_ACCESSOR( JVF );
          JET_DEFINE_ATT_ACCESSOR( JVFLoose );
          JET_DEFINE_ATT_ACCESSOR( Jvt ); 
          JET_DEFINE_ATT_ACCESSOR( JvtRpt ); 
          JET_DEFINE_ATT_ACCESSOR( JvtJvfcorr );
          JET_DEFINE_ATT_ACCESSOR( NumTrkPt1000 );
          JET_DEFINE_ATT_ACCESSOR( NumTrkPt500 );
          JET_DEFINE_ATT_ACCESSOR( SumPtTrkPt1000 );
          JET_DEFINE_ATT_ACCESSOR( SumPtTrkPt500 );
          JET_DEFINE_ATT_ACCESSOR( TrackWidthPt1000 );
          JET_DEFINE_ATT_ACCESSOR( TrackWidthPt500 );
          JET_DEFINE_ATT_ACCESSOR( EMFrac );
          JET_DEFINE_ATT_ACCESSOR( HECFrac );
          JET_DEFINE_ATT_ACCESSOR( EnergyPerSampling );
          JET_DEFINE_ATT_ACCESSOR( FracSamplingMax );
          JET_DEFINE_ATT_ACCESSOR( FracSamplingMaxIndex );
          JET_DEFINE_ATT_ACCESSOR( ECPSFraction );
          JET_DEFINE_ATT_ACCESSOR( SamplingMax );
          JET_DEFINE_ATT_ACCESSOR( Width );
          JET_DEFINE_ATT_ACCESSOR( WidthPhi );
          JET_DEFINE_ATT_ACCESSOR( LeadingClusterPt ); 
          JET_DEFINE_ATT_ACCESSOR( LeadingClusterSecondLambda ); 
          JET_DEFINE_ATT_ACCESSOR( LeadingClusterCenterLambda ); 
          JET_DEFINE_ATT_ACCESSOR( LeadingClusterSecondR  );
          JET_DEFINE_ATT_ACCESSOR( CentroidR );
          JET_DEFINE_ATT_ACCESSOR( KtDR );
          JET_DEFINE_ATT_ACCESSOR( Tau1 ); 
          JET_DEFINE_ATT_ACCESSOR( Tau2 ); 
          JET_DEFINE_ATT_ACCESSOR( Tau3 );
          JET_DEFINE_ATT_ACCESSOR( Tau1_wta ); 
          JET_DEFINE_ATT_ACCESSOR( Tau2_wta ); 
          JET_DEFINE_ATT_ACCESSOR( Tau3_wta );
          JET_DEFINE_ATT_ACCESSOR( Tau21 ); 
          JET_DEFINE_ATT_ACCESSOR( Tau32 );
          JET_DEFINE_ATT_ACCESSOR( Tau21_wta ); 
          JET_DEFINE_ATT_ACCESSOR( Tau32_wta );
          JET_DEFINE_ATT_ACCESSOR( Split12 );
          JET_DEFINE_ATT_ACCESSOR( Split23 );
          JET_DEFINE_ATT_ACCESSOR( Split34 );
          JET_DEFINE_ATT_ACCESSOR( ZCut12 ); 
          JET_DEFINE_ATT_ACCESSOR( ZCut23 ); 
          JET_DEFINE_ATT_ACCESSOR( ZCut34 ); 
          JET_DEFINE_ATT_ACCESSOR( Dip12 ); 
          JET_DEFINE_ATT_ACCESSOR( Dip13 ); 
          JET_DEFINE_ATT_ACCESSOR( Dip23 ); 
          JET_DEFINE_ATT_ACCESSOR( DipExcl12 );
          JET_DEFINE_ATT_ACCESSOR( ECF1 ); 
          JET_DEFINE_ATT_ACCESSOR( ECF2 ); 
          JET_DEFINE_ATT_ACCESSOR( ECF3 ); 
          JET_DEFINE_ATT_ACCESSOR( ECF1_Beta2 ); 
          JET_DEFINE_ATT_ACCESSOR( ECF2_Beta2 ); 
          JET_DEFINE_ATT_ACCESSOR( ECF3_Beta2 );
          JET_DEFINE_ATT_ACCESSOR( C1 ); 
          JET_DEFINE_ATT_ACCESSOR( C2 ); 
          JET_DEFINE_ATT_ACCESSOR( D2 ); 
          JET_DEFINE_ATT_ACCESSOR( C1_Beta2 ); 
          JET_DEFINE_ATT_ACCESSOR( C2_Beta2 ); 
          JET_DEFINE_ATT_ACCESSOR( D2_Beta2 );
          JET_DEFINE_ATT_ACCESSOR( ThrustMin ); 
          JET_DEFINE_ATT_ACCESSOR( ThrustMaj ); 
          JET_DEFINE_ATT_ACCESSOR( FoxWolfram0 ); 
          JET_DEFINE_ATT_ACCESSOR( FoxWolfram1 ); 
          JET_DEFINE_ATT_ACCESSOR( FoxWolfram2 ); 
          JET_DEFINE_ATT_ACCESSOR( FoxWolfram3 ); 
          JET_DEFINE_ATT_ACCESSOR( FoxWolfram4 ); 
          JET_DEFINE_ATT_ACCESSOR( Sphericity ); 
          JET_DEFINE_ATT_ACCESSOR( Aplanarity );
          JET_DEFINE_ATT_ACCESSOR( PullMag ); 
          JET_DEFINE_ATT_ACCESSOR( PullPhi ); 
          JET_DEFINE_ATT_ACCESSOR( Pull_C00 ); 
          JET_DEFINE_ATT_ACCESSOR( Pull_C01 ); 
          JET_DEFINE_ATT_ACCESSOR( Pull_C10 ); 
          JET_DEFINE_ATT_ACCESSOR( Pull_C11 );
          JET_DEFINE_ATT_ACCESSOR( Charge );
          JET_DEFINE_ATT_ACCESSOR( ShowerDeconstructionW ); 
          JET_DEFINE_ATT_ACCESSOR( ShowerDeconstructionTop );
          JET_DEFINE_ATT_ACCESSOR( Volatility );
          JET_DEFINE_ATT_ACCESSOR( isBadLoose );
          JET_DEFINE_ATT_ACCESSOR( isBadMedium );
          JET_DEFINE_ATT_ACCESSOR( isBadTight );
          JET_DEFINE_ATT_ACCESSOR( isUgly );
          JET_DEFINE_ATT_ACCESSOR( PtTruth );
          JET_DEFINE_ATT_ACCESSOR( IsoKR20Par );
          JET_DEFINE_ATT_ACCESSOR( IsoKR20Perp );
          JET_DEFINE_ATT_ACCESSOR( HighestJVFVtx );
          JET_DEFINE_ATT_ACCESSOR( JetLabel );
          JET_DEFINE_ATT_ACCESSOR( NumTowers );
          JET_DEFINE_ATT_ACCESSOR( BchCorrDotx ); 
          JET_DEFINE_ATT_ACCESSOR( BchCorrJet ); 
          JET_DEFINE_ATT_ACCESSOR( BchCorrJetForCell ); 
        
        default:
          std::cerr << "xAOD::Jet_v1 ERROR Unknown attribute ("
                    << moment << ") requested. (Did you ask the wrong type ?)" << std::endl;
          return 0;
        }
      }
    };


    /// Returns a name given the AttributeID.
    std::string name_v1(xAOD::JetAttribute::AttributeID id );


    template<typename T>
    struct ObjectAccessorMap_v1 {
      static  ObjectAccessorWrapper< T > *
      accessor( xAOD::JetAttribute::AssoParticlesID id ) {
        switch(id) {
          JET_DEFINE_ASSOBJ_ACCESSOR( GhostTrack );
          JET_DEFINE_ASSOBJ_ACCESSOR( GhostTruthParticle ); 
          JET_DEFINE_ASSOBJ_ACCESSOR( GhostMuonSegment );
          JET_DEFINE_ASSOBJ_ACCESSOR( OriginVertex );
       default:
          std::cerr << "xAOD::Jet_v1 ERROR Unknown associated particles ("
                    << id << ") requested" << std::endl;
          return 0;
        }
      }

    };
    
    /// Returns a name given the AssoParticlesID
    std::string assoName_v1(xAOD::JetAttribute::AssoParticlesID id );
    
    // struct JetAssoParticleAccessors_v1 {
    //   typedef SG::AuxElement::Accessor< std::vector< ElementLink< xAOD::IParticleContainer> > > VectorIParticleAccessor_t;

    //   static VectorIParticleAccessor_t *
    //   accessor( xAOD::JetAttribute::AssoParticlesID id ) {
    //     switch( id ) {
          
    //       JET_DEFINE_ASSO_ACCESSOR( GhostTracks ) ;
    //       JET_DEFINE_ASSO_ACCESSOR( GhostTruthParticles ) ;
    //       JET_DEFINE_ASSO_ACCESSOR( GhostMuonSegment ) ;
          
    //     default:
    //       std::cerr << "xAOD::Jet_v1 ERROR Unknown associated particles ("
    //                 << id << ") requested" << std::endl;
    //       return 0;
    //     }
    //   }

    //   static const char* assoName(xAOD::JetAttribute::AssoParticlesID id){ return s_map[id] ; }

    // protected:
    //   static const char* s_map[] ;  
    // };

  
  }
}
#endif
