/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoTowerMaker.cxx,v 1.26 2009-04-18 02:56:18 ssnyder Exp $
//
// Description: see CaloTopoTowerMaker.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
// Modified: Feb 09, 2005 (DLelas)
//          - CaloTopoTowerMaker applies correction type tool on a whole 
//            cluster collection now. Needed for Sliding Window
//            cell weights calculation.
//
// Modified Feb 2014 (W Lampl)
//          - Migrate to xAOD::CaloCluster(Container)
//          - Move to AthAlgorithm, some simplifications
//
// Modified Sep 2020 (T Chowdhury)
//          - 
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloTopoTowerMaker.h"

//---------------
// C++ Headers --
//---------------
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "CaloEvent/CaloCellClusterWeights.h"

#include "CaloUtils/CaloClusterSignalState.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "AthenaKernel/errorcheck.h"

#include "GaudiKernel/IChronoStatSvc.h"

#include "CaloTopoClusterFromTowerHelpers.h"

#include <memory> // for the std::unique_ptr
#include <cmath>

//###############################################################################
CaloTopoTowerMaker::CaloTopoTowerMaker(const std::string& name, 
				   ISvcLocator* pSvcLocator) 
  : AthReentrantAlgorithm(name, pSvcLocator)
  , m_towerOutput("")
  , m_towerCellLinkOutput("")
  , m_towerMakerTool(this)
  , m_towerCorrectionTools(this)
  , m_towerCalibratorTool(this)
  , m_chrono("ChronoStatSvc", name)
{
  // Name of Cluster Container to be registered in TDS
  declareProperty("TowersOutputName",m_towerOutput);  
  declareProperty("TowerCellLinkOutputName",m_towerCellLinkOutput);  
  
  // Name(s) of Cluster Maker Tools
  declareProperty("TowerMakerTool",m_towerMakerTool);

  // Name(s) of Cluster Correction Tools
  declareProperty("TowerCorrectionTools",m_towerCorrectionTools);

  // Tower calibrator tool
  declareProperty("TowerCalibratorTool",m_towerCalibratorTool);

  // save uncalibrated cluster signal state 
  declareProperty("SaveUncalibratedSignalState",m_saveSignalState);

  //Make Chrono Auditors for Cluster maker and correction tools
  declareProperty("ChronoTools", m_chronoTools);

  // apply LCW calibration for towers
  declareProperty("UseLCWCalibration",m_useLCWCalibration);
}

//###############################################################################

CaloTopoTowerMaker::~CaloTopoTowerMaker()
{ }

//###############################################################################

StatusCode CaloTopoTowerMaker::initialize()
{
  // -- allocate tower geometry service
  ATH_MSG_INFO("Allocate tower geometry service:");
  if ( !m_towerGeometrySvc.isValid() ) { 
    ATH_MSG_ERROR("[reject] cannot allocate tower geometry service - fatal");
    return StatusCode::FAILURE; 
  } else {
    ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("Tower geometry service is allocated, describes %6zu towers in grid:", m_towerGeometrySvc->towerBins()) );
    ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("[accept] %3zu eta bins in [%5.2f,%5.2f]",m_towerGeometrySvc->etaBins(),m_towerGeometrySvc->etaMin(),m_towerGeometrySvc->etaMax()) );
    ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("[accept] %3zu phi bins in [%5.2f,%5.2f]",m_towerGeometrySvc->phiBins(),m_towerGeometrySvc->phiMin(),m_towerGeometrySvc->phiMax()) );
  }

  // -- retrieve the needed tower maker tool
  if (m_towerMakerTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve essential tower maker tool"
                  << m_towerMakerTool);
  } else {
    ATH_MSG_DEBUG("Successfully retrieved tower maker tool "
                  << m_towerMakerTool);
  }
  
  // -- retrieve the (optional) list of correction tools
  if (m_towerCorrectionTools.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve requested tower correction tools "
                  << m_towerCorrectionTools);
  } else {
    ATH_MSG_DEBUG("Successfully retrieved tower correction tools "
                  << m_towerCorrectionTools);
  }

  // -- retrieve tower calibrator tool
  if ( m_useLCWCalibration ) {
    if ( m_towerCalibratorTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve requested tower calibrator tool " << m_towerCalibratorTool);
    } else {
      ATH_MSG_DEBUG("Sucessfully retrieved requested tower calibrator tool " << m_towerCalibratorTool);
    }
  }

  if (m_chronoTools) {
    msg(MSG::INFO) << "Will use ChronoStatSvc to monitor ClusterMaker and ClusterCorrection tools" << endmsg;
    ATH_CHECK( m_chrono.retrieve() );
  }

  ATH_CHECK( m_towerOutput.initialize() );

  if (m_towerCellLinkOutput.key().empty()) {
    m_towerCellLinkOutput = m_towerOutput.key() + "_links";
  }
  ATH_CHECK( m_towerCellLinkOutput.initialize() );

  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloTopoTowerMaker::finalize() {
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloTopoTowerMaker::execute (const EventContext& ctx) const
{

  // make a Cluster Container 
  SG::WriteHandle<xAOD::CaloClusterContainer> clusColl (m_towerOutput, ctx);
  ATH_CHECK(CaloClusterStoreHelper::AddContainerWriteHandle(&(*evtStore()), clusColl, msg()));

  std::string chronoName(this->name()+std::string("_"));
  if ( !m_useLCWCalibration ) {  
    // fill the towers without preparing for calibratoin
    if ( m_chronoTools ) { m_chrono->chronoStart(chronoName+m_towerMakerTool->name()); }
    ATH_CHECK( m_towerMakerTool->execute(ctx,clusColl.ptr(),nullptr) );
    if ( m_chronoTools ) { m_chrono->chronoStop(chronoName+m_towerMakerTool->name()); }
    // save original signals bevore corrections and calibrations
    if ( m_saveSignalState ) {
      for (xAOD::CaloCluster* fClus : *clusColl) {
	fClus->setRawE  (fClus->e()  );
	fClus->setRawEta(fClus->eta());
	fClus->setRawPhi(fClus->phi());
	fClus->setRawM  (fClus->m()  );	
      }//end loop over clusters
    }
    // apply the correction tools (really moment calculators)
    for ( auto iter(m_towerCorrectionTools.begin()); iter != m_towerCorrectionTools.end(); ++iter ) {
      if ( m_chronoTools ) { m_chrono->chronoStart(chronoName+iter->name()); }
      ATH_CHECK( (*iter)->execute(ctx,clusColl.ptr()) );
      if ( m_chronoTools ) { m_chrono->chronoStop(chronoName+iter->name()); }
    } // correction tool loop
  } else {
    // fill the towers with LCW calibration 
    std::unique_ptr<CaloCellClusterWeights> cellWeights(new CaloCellClusterWeights(std::max(m_towerGeometrySvc->maxCellHash(),m_towerGeometrySvc->totalNumberCells())));
    if ( m_chronoTools ) { m_chrono->chronoStart(chronoName+m_towerMakerTool->name()); }
    ATH_CHECK( m_towerMakerTool->execute(ctx,clusColl.ptr(),cellWeights.get()) );
    if ( m_chronoTools ) { m_chrono->chronoStop(chronoName+m_towerMakerTool->name()); }
    // save original signals bevore corrections and calibrations
    if ( m_saveSignalState ) {
      for (xAOD::CaloCluster* fClus : *clusColl) {
	fClus->setRawE  (fClus->e()  );
	fClus->setRawEta(fClus->eta());
	fClus->setRawPhi(fClus->phi());
	fClus->setRawM  (fClus->m()  );	
      }//end loop over clusters
    }
    // apply the correction tools (really moment calculators)
    for ( auto iter(m_towerCorrectionTools.begin()); iter != m_towerCorrectionTools.end(); ++iter ) {
      if ( m_chronoTools ) { m_chrono->chronoStart(chronoName+iter->name()); }
      ATH_CHECK( (*iter)->execute(ctx,clusColl.ptr()) );
      if ( m_chronoTools ) { m_chrono->chronoStop(chronoName+iter->name()); }
    } // correction tool loop
    // apply calibration 
    if ( m_chronoTools ) { m_chrono->chronoStart(chronoName+m_towerCalibratorTool->name()); }
    ATH_CHECK( m_towerCalibratorTool->execute(ctx,clusColl.ptr(),cellWeights.get()) );
    if ( m_chronoTools ) { m_chrono->chronoStop(chronoName+m_towerCalibratorTool->name()); }
  } // end of processing blocks

  ATH_MSG_DEBUG("Created tower container (of type xAOD::CaloClusterContainer) with " << clusColl->size() << " towers");
  SG::WriteHandle<CaloClusterCellLinkContainer> cellLinks (m_towerCellLinkOutput, ctx);
  ATH_CHECK(CaloClusterStoreHelper::finalizeClusters (cellLinks,
                                                      clusColl.ptr()));
  return StatusCode::SUCCESS;
}


const std::string& CaloTopoTowerMaker::getOutputContainerName() const {
  return m_towerOutput.key();
}
