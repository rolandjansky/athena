/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// FrameWork includes
#include "AthenaMonitoring/Monitored.h"
#include "GaudiKernel/IToolSvc.h"

// TrigEFMissingET includes
#include "TrigEFMissingET/IMissingETTool.h"
#include "EFMissingETFromCellsMT.h"
#include "TrigEFMissingET/EFMissingETHelper.h"


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

StatusCode EFMissingETFromCellsMT::update( xAOD::TrigMissingET *met,
					   TrigEFMissingEtHelper *metHelper,
             const EventContext& ctx ) const {

  auto totalTimer = Monitored::Timer( "TIME_Total" );
  auto caloCellsHandle = SG::makeHandle( m_cellsKey, ctx );

  auto loopTimer = Monitored::Timer( "TIME_Loop" );
  auto countUsedCells = Monitored::Scalar<unsigned>( "UsedCells", 0 );

  // now it is time to iterate over the cells
  int nCells(0), nZeroCells(0);
  for ( const CaloCell* cell: *caloCellsHandle ) {
    nCells++;
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
    
    const double E = cell->e(); if(E<1e-6) nZeroCells++;
    const double et  = E * cDDE->sinTh();
    const double ez  = E * cDDE->cosTh();
    const double sinPhi = cDDE->sinPhi();
    const double cosPhi = cDDE->cosPhi();
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
    // if ( not m_makeRobustness) continue;

    // if ( not m_doCellNoiseSupp || (m_doCellNoiseSupp &&
				//    m_MinCellSNratio[cDDE->getSampling()] > m_maxThreshold)) {
    //   //       if (fabs(E) < m_MinCellSNratio[cDDE->getSampling()] *
    //   //           m_noiseTool->getNoise( cell, ICalorimeterNoiseTool::TOTALNOISE))
    //   continue;
    // }

    const float time = cell->time() * 1e-3;  // ns
    float quality = cell->quality();
    if (time < metComp->m_minTime) metComp->m_minTime = time;
    if (time > metComp->m_maxTime) metComp->m_maxTime = time;
    if (quality > metComp->m_maxQlty) metComp->m_maxQlty = quality;
    if (E < metComp->m_minE) metComp->m_minE = E;
    if (E > metComp->m_maxE) metComp->m_maxE = E;

    // ATH_MSG_INFO("metComp->m_ex" << metComp->m_ex);
    // ATH_MSG_INFO("metComp->m_ey" << metComp->m_ey);
    // ATH_MSG_INFO("metComp->m_ez" << metComp->m_ez);
    // ATH_MSG_INFO("metComp->m_sumEt" << metComp->m_sumEt);
    // ATH_MSG_INFO("metComp->m_sumE" << metComp->m_sumE);
       
  }

  // After iterating over all cells, fill the final met object and include calibrations
  for (uint helper_i=0; helper_i<metHelper->GetElements(); ++helper_i){
    switch(static_cast<TrigEFMissingEtComponent::Component>(helper_i))
    {
      case TrigEFMissingEtComponent::PreSamplB:
      case TrigEFMissingEtComponent::EMB1: case TrigEFMissingEtComponent::EMB2: case TrigEFMissingEtComponent::EMB3:

      case TrigEFMissingEtComponent::PreSamplE:
      case TrigEFMissingEtComponent::EME1: case TrigEFMissingEtComponent::EME2: case TrigEFMissingEtComponent::EME3: 

      case TrigEFMissingEtComponent::HEC0: case TrigEFMissingEtComponent::HEC1:
      case TrigEFMissingEtComponent::HEC2: case TrigEFMissingEtComponent::HEC3:

      case TrigEFMissingEtComponent::TileBar0: case TrigEFMissingEtComponent::TileBar1: case TrigEFMissingEtComponent::TileBar2: 
      case TrigEFMissingEtComponent::TileGap1: case TrigEFMissingEtComponent::TileGap2: case TrigEFMissingEtComponent::TileGap3:
      case TrigEFMissingEtComponent::TileExt0: case TrigEFMissingEtComponent::TileExt1: case TrigEFMissingEtComponent::TileExt2:

      case TrigEFMissingEtComponent::FCalEM:
      case TrigEFMissingEtComponent::FCalHad1: case TrigEFMissingEtComponent::FCalHad2:
      {
        TrigEFMissingEtComponent* metComp = metHelper->GetComponent(helper_i);

        float ex =            metComp->m_ex;
        float ey =            metComp->m_ey;
        float ez =            metComp->m_ez;
        float sumE =          metComp->m_sumE;
        float sumEt =         metComp->m_sumEt;
        float c0 =            metComp->m_calib0;
        float c1 =            metComp->m_calib1;
        short sumOfSigns =    metComp->m_sumOfSigns;

        met->setEx(       met->ex() + sumOfSigns * c0 + c1 * ex );
        met->setEy(       met->ey() + sumOfSigns * c0 + c1 * ey );
        met->setEz(       met->ez() + sumOfSigns * c0 + c1 * ez );
        met->setSumE(   met->sumE() + sumOfSigns * c0 + c1 * sumE );
        met->setSumEt( met->sumEt() + sumOfSigns * c0 + c1 * sumEt );
      } break;

      default:
        break;
    }
  }
  ATH_MSG_DEBUG("Number of cells considered: " << nCells);
  ATH_MSG_DEBUG("Number of cells with E==0: " << nZeroCells);

      
  return StatusCode::SUCCESS;
}
















