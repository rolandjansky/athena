/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAccessors_v1.cxx 727661 2016-03-04 12:35:27Z menke $

// System include(s):
#include <iostream>

// Local include(s):
#include "CaloClusterAccessors_v1.h"

/// Helper macro for managing cluster moment Accessor objects
#define DEFINE_ACCESSOR( NAME )                                   \
   case xAOD::CaloCluster_v1::NAME:                               \
   {                                                              \
      static SG::AuxElement::Accessor< float > a( #NAME );        \
      return &a;                                                  \
   }                                                              \
   break
namespace xAOD {

   SG::AuxElement::Accessor< float >*
   momentAccessorV1( xAOD::CaloCluster_v1::MomentType moment ) {

      switch( moment ) {

         DEFINE_ACCESSOR( FIRST_PHI );
         DEFINE_ACCESSOR( FIRST_ETA );
         DEFINE_ACCESSOR( SECOND_R );
         DEFINE_ACCESSOR( SECOND_LAMBDA );
         DEFINE_ACCESSOR( DELTA_PHI );
         DEFINE_ACCESSOR( DELTA_THETA );
         DEFINE_ACCESSOR( DELTA_ALPHA );
         DEFINE_ACCESSOR( CENTER_X );
         DEFINE_ACCESSOR( CENTER_Y );
         DEFINE_ACCESSOR( CENTER_Z );
         DEFINE_ACCESSOR( CENTER_MAG );
         DEFINE_ACCESSOR( CENTER_LAMBDA );
         DEFINE_ACCESSOR( LATERAL );
         DEFINE_ACCESSOR( LONGITUDINAL );
         DEFINE_ACCESSOR( ENG_FRAC_EM );
         DEFINE_ACCESSOR( ENG_FRAC_MAX );
         DEFINE_ACCESSOR( ENG_FRAC_CORE );
         DEFINE_ACCESSOR( FIRST_ENG_DENS );
         DEFINE_ACCESSOR( SECOND_ENG_DENS );
         DEFINE_ACCESSOR( ISOLATION );
         DEFINE_ACCESSOR( ENG_BAD_CELLS );
         DEFINE_ACCESSOR( N_BAD_CELLS );
         DEFINE_ACCESSOR( N_BAD_CELLS_CORR );
         DEFINE_ACCESSOR( BAD_CELLS_CORR_E );
         DEFINE_ACCESSOR( BADLARQ_FRAC );
         DEFINE_ACCESSOR( ENG_POS );
         DEFINE_ACCESSOR( ENG_BAD_HV_CELLS );
         DEFINE_ACCESSOR( N_BAD_HV_CELLS );
         DEFINE_ACCESSOR( SIGNIFICANCE );
         DEFINE_ACCESSOR( PTD );
         DEFINE_ACCESSOR( CELL_SIGNIFICANCE );
         DEFINE_ACCESSOR( CELL_SIG_SAMPLING );
         DEFINE_ACCESSOR( AVG_LAR_Q );
         DEFINE_ACCESSOR( AVG_TILE_Q );
         DEFINE_ACCESSOR( EM_PROBABILITY );
         DEFINE_ACCESSOR( HAD_WEIGHT );
         DEFINE_ACCESSOR( OOC_WEIGHT );
         DEFINE_ACCESSOR( DM_WEIGHT );
         DEFINE_ACCESSOR( TILE_CONFIDENCE_LEVEL );
	 DEFINE_ACCESSOR( VERTEX_FRACTION );
	 DEFINE_ACCESSOR( NVERTEX_FRACTION );
         DEFINE_ACCESSOR( ETACALOFRAME );
         DEFINE_ACCESSOR( PHICALOFRAME ); 
	 DEFINE_ACCESSOR( ETA1CALOFRAME );
	 DEFINE_ACCESSOR( PHI1CALOFRAME );
	 DEFINE_ACCESSOR( ETA2CALOFRAME );
	 DEFINE_ACCESSOR( PHI2CALOFRAME );
         DEFINE_ACCESSOR( ENG_CALIB_TOT );
         DEFINE_ACCESSOR( ENG_CALIB_OUT_L );
         DEFINE_ACCESSOR( ENG_CALIB_OUT_M );
         DEFINE_ACCESSOR( ENG_CALIB_OUT_T );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_L );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_M );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_T );
         DEFINE_ACCESSOR( ENG_CALIB_EMB0 );
         DEFINE_ACCESSOR( ENG_CALIB_EME0 );
         DEFINE_ACCESSOR( ENG_CALIB_TILEG3 );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_TOT );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_EMB0 );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_TILE0 );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_TILEG3 );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_EME0 );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_HEC0 );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_FCAL );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_LEAKAGE );
         DEFINE_ACCESSOR( ENG_CALIB_DEAD_UNCLASS );
         DEFINE_ACCESSOR( ENG_CALIB_FRAC_EM );
         DEFINE_ACCESSOR( ENG_CALIB_FRAC_HAD );
         DEFINE_ACCESSOR( ENG_CALIB_FRAC_REST );


         DEFINE_ACCESSOR( ENERGY_Truth );
         DEFINE_ACCESSOR( ETA_Truth );
         DEFINE_ACCESSOR( PHI_Truth );
         DEFINE_ACCESSOR( TIME_Truth );
         DEFINE_ACCESSOR( ENERGY_CALIB_Truth );
         DEFINE_ACCESSOR( ETA_CALIB_Truth );
         DEFINE_ACCESSOR( PHI_CALIB_Truth );
         DEFINE_ACCESSOR( TIME_CALIB_Truth );

         DEFINE_ACCESSOR( FIRST_PHI_Truth );
         DEFINE_ACCESSOR( FIRST_ETA_Truth );
         DEFINE_ACCESSOR( SECOND_R_Truth );
         DEFINE_ACCESSOR( SECOND_LAMBDA_Truth );
         DEFINE_ACCESSOR( DELTA_PHI_Truth );
         DEFINE_ACCESSOR( DELTA_THETA_Truth );
         DEFINE_ACCESSOR( DELTA_ALPHA_Truth );
         DEFINE_ACCESSOR( CENTER_X_Truth );
         DEFINE_ACCESSOR( CENTER_Y_Truth );
         DEFINE_ACCESSOR( CENTER_Z_Truth );
         DEFINE_ACCESSOR( CENTER_MAG_Truth );
         DEFINE_ACCESSOR( CENTER_LAMBDA_Truth );
         DEFINE_ACCESSOR( LATERAL_Truth );
         DEFINE_ACCESSOR( LONGITUDINAL_Truth );
         DEFINE_ACCESSOR( ENG_FRAC_EM_Truth );
         DEFINE_ACCESSOR( ENG_FRAC_MAX_Truth );
         DEFINE_ACCESSOR( ENG_FRAC_CORE_Truth );
         DEFINE_ACCESSOR( FIRST_ENG_DENS_Truth );
         DEFINE_ACCESSOR( SECOND_ENG_DENS_Truth );
         DEFINE_ACCESSOR( ISOLATION_Truth );
         DEFINE_ACCESSOR( ENG_BAD_CELLS_Truth );
         DEFINE_ACCESSOR( N_BAD_CELLS_Truth );
         DEFINE_ACCESSOR( N_BAD_CELLS_CORR_Truth );
         DEFINE_ACCESSOR( BAD_CELLS_CORR_E_Truth );
         DEFINE_ACCESSOR( BADLARQ_FRAC_Truth );
         DEFINE_ACCESSOR( ENG_POS_Truth );
         DEFINE_ACCESSOR( ENG_BAD_HV_CELLS_Truth );
         DEFINE_ACCESSOR( N_BAD_HV_CELLS_Truth );
         DEFINE_ACCESSOR( SIGNIFICANCE_Truth );
         DEFINE_ACCESSOR( CELL_SIGNIFICANCE_Truth );
         DEFINE_ACCESSOR( CELL_SIG_SAMPLING_Truth );
         DEFINE_ACCESSOR( AVG_LAR_Q_Truth );
         DEFINE_ACCESSOR( AVG_TILE_Q_Truth );
         DEFINE_ACCESSOR( EM_PROBABILITY_Truth );
         DEFINE_ACCESSOR( HAD_WEIGHT_Truth );
         DEFINE_ACCESSOR( OOC_WEIGHT_Truth );
         DEFINE_ACCESSOR( DM_WEIGHT_Truth );

      default:
         std::cerr << "xAOD::CaloCluster_v1 ERROR Unknown moment ("
                   << moment << ") requested" << std::endl;
         return 0;
      }
   }
} // namespace xAOD
