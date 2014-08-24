/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAuxContainer_v1.cxx 585678 2014-02-28 14:20:54Z wlampl $

// Local include(s):
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"

namespace xAOD {

   CaloClusterAuxContainer_v1::CaloClusterAuxContainer_v1()
      : AuxContainerBase() {

     AUX_VARIABLE( time );	          	  
     //AUX_VARIABLE( samplingPattern );  
     AUX_VARIABLE( clusterSize );      
     AUX_VARIABLE( eta0 );	          	  
     AUX_VARIABLE( phi0 );	          	  
     AUX_VARIABLE( rawE );	          	  
     AUX_VARIABLE( rawEta );	           
     AUX_VARIABLE( rawPhi );	           
     AUX_VARIABLE( rawM );	          	  
     AUX_VARIABLE( altE );	          	  
     AUX_VARIABLE( altEta );	           
     AUX_VARIABLE( altPhi );	           
     AUX_VARIABLE( altM );
     AUX_VARIABLE( calE );	          	  
     AUX_VARIABLE( calEta );	           
     AUX_VARIABLE( calPhi );	           
     AUX_VARIABLE( calM );

     /*
      // The cluster moments:
      AUX_VARIABLE( FIRST_PHI );
      AUX_VARIABLE( FIRST_ETA );
      AUX_VARIABLE( SECOND_R );
      AUX_VARIABLE( SECOND_LAMBDA );
      AUX_VARIABLE( DELTA_PHI );
      AUX_VARIABLE( DELTA_THETA );
      AUX_VARIABLE( DELTA_ALPHA );
      AUX_VARIABLE( CENTER_X );
      AUX_VARIABLE( CENTER_Y );
      AUX_VARIABLE( CENTER_Z );
      AUX_VARIABLE( CENTER_MAG );
      AUX_VARIABLE( CENTER_LAMBDA );
      AUX_VARIABLE( LATERAL );
      AUX_VARIABLE( LONGITUDINAL );
     */
      // Energies in various samplings

     AUX_VARIABLE ( e_sampl );
     AUX_VARIABLE ( eta_sampl );
     AUX_VARIABLE ( phi_sampl );

     /*
      AUX_VARIABLE( E_PreSamplerB );
      AUX_VARIABLE( E_PreSamplerE );
      AUX_VARIABLE( E_EMB1 );
      AUX_VARIABLE( E_EMB2 );
      AUX_VARIABLE( E_EMB3 );
      AUX_VARIABLE( E_EME1 );
      AUX_VARIABLE( E_EME2 );
      AUX_VARIABLE( E_EME3 );
      AUX_VARIABLE( E_HEC0 );
      AUX_VARIABLE( E_HEC1 );
      AUX_VARIABLE( E_HEC2 );
      AUX_VARIABLE( E_HEC3 );
      AUX_VARIABLE( E_TileBar0 );
      AUX_VARIABLE( E_TileBar1 );
      AUX_VARIABLE( E_TileBar2 );
      AUX_VARIABLE( E_TileGap1 );
      AUX_VARIABLE( E_TileGap2 );
      AUX_VARIABLE( E_TileGap3 );
      AUX_VARIABLE( E_TileExt0 );
      AUX_VARIABLE( E_TileExt1 );
      AUX_VARIABLE( E_TileExt2 );
      AUX_VARIABLE( E_FCAL0 );
      AUX_VARIABLE( E_FCAL1 );
      AUX_VARIABLE( E_FCAL2 );

      // Pseudorapidities in various samplings
      AUX_VARIABLE( eta_PreSamplerB );
      AUX_VARIABLE( eta_PreSamplerE );
      AUX_VARIABLE( eta_EMB1 );
      AUX_VARIABLE( eta_EMB2 );
      AUX_VARIABLE( eta_EMB3 );
      AUX_VARIABLE( eta_EME1 );
      AUX_VARIABLE( eta_EME2 );
      AUX_VARIABLE( eta_EME3 );
      AUX_VARIABLE( eta_HEC0 );
      AUX_VARIABLE( eta_HEC1 );
      AUX_VARIABLE( eta_HEC2 );
      AUX_VARIABLE( eta_HEC3 );
      AUX_VARIABLE( eta_TileBar0 );
      AUX_VARIABLE( eta_TileBar1 );
      AUX_VARIABLE( eta_TileBar2 );
      AUX_VARIABLE( eta_TileGap1 );
      AUX_VARIABLE( eta_TileGap2 );
      AUX_VARIABLE( eta_TileGap3 );
      AUX_VARIABLE( eta_TileExt0 );
      AUX_VARIABLE( eta_TileExt1 );
      AUX_VARIABLE( eta_TileExt2 );
      AUX_VARIABLE( eta_FCAL0 );
      AUX_VARIABLE( eta_FCAL1 );
      AUX_VARIABLE( eta_FCAL2 );

      // Azimuthal angles in various samplings
      AUX_VARIABLE( phi_PreSamplerB );
      AUX_VARIABLE( phi_PreSamplerE );
      AUX_VARIABLE( phi_EMB1 );
      AUX_VARIABLE( phi_EMB2 );
      AUX_VARIABLE( phi_EMB3 );
      AUX_VARIABLE( phi_EME1 );
      AUX_VARIABLE( phi_EME2 );
      AUX_VARIABLE( phi_EME3 );
      AUX_VARIABLE( phi_HEC0 );
      AUX_VARIABLE( phi_HEC1 );
      AUX_VARIABLE( phi_HEC2 );
      AUX_VARIABLE( phi_HEC3 );
      AUX_VARIABLE( phi_TileBar0 );
      AUX_VARIABLE( phi_TileBar1 );
      AUX_VARIABLE( phi_TileBar2 );
      AUX_VARIABLE( phi_TileGap1 );
      AUX_VARIABLE( phi_TileGap2 );
      AUX_VARIABLE( phi_TileGap3 );
      AUX_VARIABLE( phi_TileExt0 );
      AUX_VARIABLE( phi_TileExt1 );
      AUX_VARIABLE( phi_TileExt2 );
      AUX_VARIABLE( phi_FCAL0 );
      AUX_VARIABLE( phi_FCAL1 );
      AUX_VARIABLE( phi_FCAL2 );
     */
   }

} // namespace xAOD
