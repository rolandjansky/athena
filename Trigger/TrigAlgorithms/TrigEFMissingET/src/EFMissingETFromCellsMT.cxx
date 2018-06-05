/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// FrameWork includes
#include "AthenaMonitoring/MonitoredScope.h"
#include "GaudiKernel/IToolSvc.h"

// TrigEFMissingET includes
#include "EFMissingETFromCellsMT.h"


EFMissingETFromCellsMT::EFMissingETFromCellsMT( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) 
  : base_class( type, name, parent ) {
  //declareProperty( "Property", m_nProperty );
}

StatusCode EFMissingETFromCellsMT::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  CHECK( m_cellsKey.initialize() );

  m_maxThreshold = m_rmsOneSided;
  if ( m_rmsOneSided < fabsf( m_rmsTwoSided ) ) m_maxThreshold = fabsf( m_rmsTwoSided );


  //CHECK( m_noiseTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromCellsMT::update( xAOD::TrigMissingET */*met*/,
					   TrigEFMissingEtHelper *metHelper ) const {
  using namespace Monitored;
  auto totalTimer = MonitoredTimer::declare( "TIME_Total" );  
  const EventContext context{ Gaudi::Hive::currentContext() };
  auto caloCellsHandle = SG::makeHandle( m_cellsKey );

  auto loopTimer = MonitoredTimer::declare( "TIME_Loop" );  
  auto countUsedCells = MonitoredScalar::declare<unsigned>( "UsedCells", 0 );  

  // now it is time to iterate over the cells
  for ( const CaloCell* cell: *caloCellsHandle ) {
    const CaloDetDescrElement* cDDE = cell->caloDDE();
    if (cDDE == 0) {
      ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << cell->ID() );
      continue;
    }

    TrigEFMissingEtComponent *metComp = metHelper->GetComponent( cDDE->getSampling() );
    if (metComp == 0) {
      ATH_MSG_FATAL( "Cannot find calo sampling!" );
      return StatusCode::FAILURE;
    }

    //TB not sure about skipping logic, looks incorrect in the original code
    // 

    // if ( m_noiseTool ) { // == noise suppression required
    //   const bool  noiseCutPassed = true; //  noiseCut( cell  ); 
    //   if ( not noiseCutPassed )
    // 	continue;
    // }

    countUsedCells = countUsedCells + 1;
    
    double E = cell->e();
    double et  = E * cDDE->sinTh();
    double ez  = E * cDDE->cosTh();
    double sinPhi = cDDE->sinPhi();
    double cosPhi = cDDE->cosPhi();
    metComp->m_ex -= et*cosPhi;
    metComp->m_ey -= et*sinPhi;
    metComp->m_ez -= ez;
    metComp->m_sumEt += et;
    metComp->m_sumE += E;
    metComp->m_usedChannels += 1;

    if( cell->quality() > m_MaxLArQ ) metComp->m_sumBadEt += et;
    //    if (BSerrors) metComp->m_status |= m_maskErrBSconv; // | m_maskCompErrors;
    metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,E) + 0.5));

    // 5. auxiliary quantities for robustness checks
    if ( not m_makeRobustness) continue;
    if ( not m_doCellNoiseSupp || (m_doCellNoiseSupp &&
				   m_MinCellSNratio[cDDE->getSampling()] > m_maxThreshold)) {
      //       if (fabs(E) < m_MinCellSNratio[cDDE->getSampling()] *
      //           m_noiseTool->getNoise( cell, ICalorimeterNoiseTool::TOTALNOISE))
      continue;
    }

    float time = cell->time() * 1e-3;  // ns
    float quality = cell->quality();
    if (time < metComp->m_minTime) metComp->m_minTime = time;
    if (time > metComp->m_maxTime) metComp->m_maxTime = time;
    if (quality > metComp->m_maxQlty) metComp->m_maxQlty = quality;
    if (E < metComp->m_minE) metComp->m_minE = E;
    if (E > metComp->m_maxE) metComp->m_maxE = E;
       
  }
      
  return StatusCode::SUCCESS;
}
















