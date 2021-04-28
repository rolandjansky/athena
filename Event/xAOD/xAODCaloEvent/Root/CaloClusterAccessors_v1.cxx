/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAccessors_v1.cxx 794609 2017-01-30 15:51:25Z menke $

// System include(s):
#include <iostream>

// Local include(s):
#include "CaloClusterAccessors_v1.h"

/// Helper macro for managing cluster moment Accessor objects
#define DEFINE_ACCESSOR( NAME, TYPE )				  \
  case xAOD::CaloCluster_v1::NAME:				  \
  {								  \
     static const SG::AuxElement::Accessor< TYPE > a( #NAME );	  \
     return &a;							  \
   }                                                              \
  break

namespace xAOD {

   const SG::AuxElement::Accessor< float >*
   momentAccessorV1( xAOD::CaloCluster_v1::MomentType moment ) {
     
     switch( moment ) {
       DEFINE_ACCESSOR( FIRST_PHI                    , float );
       DEFINE_ACCESSOR( FIRST_ETA                    , float );
       DEFINE_ACCESSOR( SECOND_R                     , float );
       DEFINE_ACCESSOR( SECOND_LAMBDA                , float );
       DEFINE_ACCESSOR( DELTA_PHI                    , float );
       DEFINE_ACCESSOR( DELTA_THETA                  , float );
       DEFINE_ACCESSOR( DELTA_ALPHA                  , float );
       DEFINE_ACCESSOR( CENTER_X                     , float );
       DEFINE_ACCESSOR( CENTER_Y                     , float );
       DEFINE_ACCESSOR( CENTER_Z                     , float );
       DEFINE_ACCESSOR( CENTER_MAG                   , float );
       DEFINE_ACCESSOR( CENTER_LAMBDA                , float );
       DEFINE_ACCESSOR( LATERAL                      , float );
       DEFINE_ACCESSOR( LONGITUDINAL                 , float );
       DEFINE_ACCESSOR( ENG_FRAC_EM                  , float );
       DEFINE_ACCESSOR( ENG_FRAC_MAX                 , float );
       DEFINE_ACCESSOR( ENG_FRAC_CORE                , float );
       DEFINE_ACCESSOR( FIRST_ENG_DENS               , float );
       DEFINE_ACCESSOR( SECOND_ENG_DENS              , float );
       DEFINE_ACCESSOR( ISOLATION                    , float );
       DEFINE_ACCESSOR( ENG_BAD_CELLS                , float );
       DEFINE_ACCESSOR( N_BAD_CELLS                  , float );
       DEFINE_ACCESSOR( N_BAD_CELLS_CORR             , float );
       DEFINE_ACCESSOR( BAD_CELLS_CORR_E             , float );
       DEFINE_ACCESSOR( BADLARQ_FRAC                 , float );
       DEFINE_ACCESSOR( ENG_POS                      , float );
       DEFINE_ACCESSOR( ENG_BAD_HV_CELLS             , float );
       DEFINE_ACCESSOR( N_BAD_HV_CELLS               , float );
       DEFINE_ACCESSOR( SIGNIFICANCE                 , float );
       DEFINE_ACCESSOR( PTD                          , float );
       DEFINE_ACCESSOR( MASS                         , float );
       DEFINE_ACCESSOR( SECOND_TIME                  , float );
       DEFINE_ACCESSOR( CELL_SIGNIFICANCE            , float );
       DEFINE_ACCESSOR( CELL_SIG_SAMPLING            , float );
       DEFINE_ACCESSOR( AVG_LAR_Q                    , float );
       DEFINE_ACCESSOR( AVG_TILE_Q                   , float );
       DEFINE_ACCESSOR( EM_PROBABILITY               , float );
       DEFINE_ACCESSOR( HAD_WEIGHT                   , float );
       DEFINE_ACCESSOR( OOC_WEIGHT                   , float );
       DEFINE_ACCESSOR( DM_WEIGHT                    , float );
       DEFINE_ACCESSOR( TILE_CONFIDENCE_LEVEL        , float );
       DEFINE_ACCESSOR( VERTEX_FRACTION              , float );
       DEFINE_ACCESSOR( NVERTEX_FRACTION             , float );
       DEFINE_ACCESSOR( ETACALOFRAME                 , float );
       DEFINE_ACCESSOR( PHICALOFRAME                 , float ); 
       DEFINE_ACCESSOR( ETA1CALOFRAME                , float );
       DEFINE_ACCESSOR( PHI1CALOFRAME                , float );
       DEFINE_ACCESSOR( ETA2CALOFRAME                , float );
       DEFINE_ACCESSOR( PHI2CALOFRAME                , float );
       DEFINE_ACCESSOR( ENG_CALIB_TOT                , float );
       DEFINE_ACCESSOR( ENG_CALIB_OUT_L              , float );
       DEFINE_ACCESSOR( ENG_CALIB_OUT_M              , float );
       DEFINE_ACCESSOR( ENG_CALIB_OUT_T              , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_L             , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_M             , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_T             , float );
       DEFINE_ACCESSOR( ENG_CALIB_EMB0               , float );
       DEFINE_ACCESSOR( ENG_CALIB_EME0               , float );
       DEFINE_ACCESSOR( ENG_CALIB_TILEG3             , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_TOT           , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_EMB0          , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_TILE0         , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_TILEG3        , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_EME0          , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_HEC0          , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_FCAL          , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_LEAKAGE       , float );
       DEFINE_ACCESSOR( ENG_CALIB_DEAD_UNCLASS       , float );
       DEFINE_ACCESSOR( ENG_CALIB_FRAC_EM            , float );
       DEFINE_ACCESSOR( ENG_CALIB_FRAC_HAD           , float );
       DEFINE_ACCESSOR( ENG_CALIB_FRAC_REST          , float );
       					             
       DEFINE_ACCESSOR( ENERGY_DigiHSTruth           , float );
       DEFINE_ACCESSOR( ETA_DigiHSTruth              , float );
       DEFINE_ACCESSOR( PHI_DigiHSTruth              , float );
       DEFINE_ACCESSOR( TIME_DigiHSTruth             , float );
       DEFINE_ACCESSOR( ENERGY_CALIB_DigiHSTruth     , float );
       DEFINE_ACCESSOR( ETA_CALIB_DigiHSTruth        , float );
       DEFINE_ACCESSOR( PHI_CALIB_DigiHSTruth        , float );
       DEFINE_ACCESSOR( TIME_CALIB_DigiHSTruth       , float );
	 					     
       DEFINE_ACCESSOR( FIRST_PHI_DigiHSTruth        , float );
       DEFINE_ACCESSOR( FIRST_ETA_DigiHSTruth        , float );
       DEFINE_ACCESSOR( SECOND_R_DigiHSTruth         , float );
       DEFINE_ACCESSOR( SECOND_LAMBDA_DigiHSTruth    , float );
       DEFINE_ACCESSOR( DELTA_PHI_DigiHSTruth        , float );
       DEFINE_ACCESSOR( DELTA_THETA_DigiHSTruth      , float );
       DEFINE_ACCESSOR( DELTA_ALPHA_DigiHSTruth      , float );
       DEFINE_ACCESSOR( CENTER_X_DigiHSTruth         , float );
       DEFINE_ACCESSOR( CENTER_Y_DigiHSTruth         , float );
       DEFINE_ACCESSOR( CENTER_Z_DigiHSTruth         , float );
       DEFINE_ACCESSOR( CENTER_MAG_DigiHSTruth       , float );
       DEFINE_ACCESSOR( CENTER_LAMBDA_DigiHSTruth    , float );
       DEFINE_ACCESSOR( LATERAL_DigiHSTruth          , float );
       DEFINE_ACCESSOR( LONGITUDINAL_DigiHSTruth     , float );
       DEFINE_ACCESSOR( ENG_FRAC_EM_DigiHSTruth      , float );
       DEFINE_ACCESSOR( ENG_FRAC_MAX_DigiHSTruth     , float );
       DEFINE_ACCESSOR( ENG_FRAC_CORE_DigiHSTruth    , float );
       DEFINE_ACCESSOR( FIRST_ENG_DENS_DigiHSTruth   , float );
       DEFINE_ACCESSOR( SECOND_ENG_DENS_DigiHSTruth  , float );
       DEFINE_ACCESSOR( ISOLATION_DigiHSTruth        , float );
       DEFINE_ACCESSOR( ENG_BAD_CELLS_DigiHSTruth    , float );
       DEFINE_ACCESSOR( N_BAD_CELLS_DigiHSTruth      , float );
       DEFINE_ACCESSOR( N_BAD_CELLS_CORR_DigiHSTruth , float );
       DEFINE_ACCESSOR( BAD_CELLS_CORR_E_DigiHSTruth , float );
       DEFINE_ACCESSOR( BADLARQ_FRAC_DigiHSTruth     , float );
       DEFINE_ACCESSOR( ENG_POS_DigiHSTruth          , float );
       DEFINE_ACCESSOR( ENG_BAD_HV_CELLS_DigiHSTruth , float );
       DEFINE_ACCESSOR( N_BAD_HV_CELLS_DigiHSTruth   , float );
       DEFINE_ACCESSOR( SIGNIFICANCE_DigiHSTruth     , float );
       DEFINE_ACCESSOR( CELL_SIGNIFICANCE_DigiHSTruth, float );
       DEFINE_ACCESSOR( CELL_SIG_SAMPLING_DigiHSTruth, float );
       DEFINE_ACCESSOR( AVG_LAR_Q_DigiHSTruth        , float );
       DEFINE_ACCESSOR( AVG_TILE_Q_DigiHSTruth       , float );
       DEFINE_ACCESSOR( EM_PROBABILITY_DigiHSTruth   , float );
       DEFINE_ACCESSOR( HAD_WEIGHT_DigiHSTruth       , float );
       DEFINE_ACCESSOR( OOC_WEIGHT_DigiHSTruth       , float );
       DEFINE_ACCESSOR( DM_WEIGHT_DigiHSTruth        , float );

      default:
         std::cerr << "xAOD::CaloCluster_v1 ERROR Unknown scalar moment ("
                   << moment << ") requested" << std::endl;
         return nullptr;
     }
   }

  const SG::AuxElement::Accessor<xAOD::CaloCluster_v1::ncells_store_t>* 
  momentContainerAccessorV1(xAOD::CaloCluster_v1::MomentType moment) { 
    switch ( moment ) {
      DEFINE_ACCESSOR( NCELL_SAMPLING, xAOD::CaloCluster_v1::ncells_store_t ); 
    default:
      std::cerr << "xAOD::CaloCluster_v1 ERROR Unknown compound moment type ("
		<< moment << ") requested" << std::endl;
      return nullptr; 
    }
  }
  
} // namespace xAOD
