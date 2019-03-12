/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     RoITileCellContMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   C. Santamarina
// Implements the new data unpacking developped by Denis.
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigCaloRec/RoITileCellContMaker.h"
#include "CaloEvent/CaloCellContainer.h"


#include "TrigT2CaloCommon/Calo_Def.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include <math.h>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // legacy trigger code

RoITileCellContMaker::RoITileCellContMaker(const std::string & type, const std::string & name,
        const IInterface* parent): IAlgToolEFCalo(type, name, parent),
				   m_data(NULL)
{
  declareProperty("CheckCellWithinRoI", m_CheckCellWithinRoI = false);
  declareProperty("DoTileCellsNoiseSuppression", m_do_TileCells_noise_suppression = 0);
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
  declareProperty("CutValue",m_cutvalue = 2,"Cell accepted if e>m_cutvalue*tilecellnoise");
  declareProperty("AbsEinSigma",m_absEinSigma = 0,"0=asymmetric noise-sigma cut(default)"); 
  declareProperty("TwoGaussianNoise",m_twogaussiannoise = false,"false=do not use double gaussian description for tile(default)"); 
}

//
RoITileCellContMaker::~RoITileCellContMaker(){
}

StatusCode RoITileCellContMaker::initialize(){
  
  if (toolSvc()->retrieveTool("TrigDataAccess", m_data).isFailure()) {
    return StatusCode::FAILURE;
  }
  
  if (m_do_TileCells_noise_suppression!=0){
    
    if (m_noiseTool.retrieve().isFailure()) return StatusCode::FAILURE;

    std::cout << name() << " Two-Gaussian noise for Tile set to " << (m_twogaussiannoise ? "true" : "false") << std::endl;

  } else {
    m_noiseTool.disable();
  }

  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
     std::cout << ": Unable to locate TrigTimer Service" << std::endl;
  }

  // Initialize timing service
  if (m_timersvc) {
     m_timer.push_back(m_timersvc->addItem("TCRec_TileTotal"));
     m_timer.push_back(m_timersvc->addItem("TCRec_TileRegionSelector"));
     m_timer.push_back(m_timersvc->addItem("TCRec_TileDataAccess"));
     m_timer.push_back(m_timersvc->addItem("TCRec_TilePushBackCells"));
  }
  
  return StatusCode::SUCCESS;
}

StatusCode RoITileCellContMaker::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode RoITileCellContMaker::execute(CaloCellContainer &pCaloCellContainer,
					 const IRoiDescriptor& roi ) { 
  // reset error
  //m_error=(EFTILE<<28);
  m_error=0;

  if (m_timersvc) {
    (m_timer.at(0))->start();
    (m_timer.at(1))->start(); (m_timer.at(1))->pause();
    (m_timer.at(2))->start(); (m_timer.at(2))->pause();
    (m_timer.at(3))->start(); (m_timer.at(3))->pause();
  }

  // Region Selector, Tile Calorimeter...
  if (m_timersvc) (m_timer.at(1))->resume();
  m_data->RegionSelector(0,roi,TILE);
  if (m_timersvc) (m_timer.at(1))->pause();


  for (unsigned int iR=0;iR<m_data->TileContSize();iR++) {

     if (m_timersvc) (m_timer.at(2))->resume();
     if((m_data->LoadCollections(m_iBegin,m_iEnd,iR,!iR)).isFailure()) {
       ATH_MSG_DEBUG("unable to load cell collections");
       //return StatusCode::FAILURE;
     }
     if (m_data->report_error()) {
       m_error=m_data->report_error(); //+(EFTILE<<28);
       if (m_timersvc) (m_timer.at(2))->pause();
       //continue;
     }
     if (m_timersvc) (m_timer.at(2))->pause();

     if (m_timersvc) (m_timer.at(3))->resume();
     TileCellCollection::const_iterator it;

     if (m_do_TileCells_noise_suppression!=0) {
       for ( it = m_iBegin; it != m_iEnd; ++it) {
         CaloCell* cell = (CaloCell*)(*it);	 
	 double tilecellnoise = m_twogaussiannoise ? 
	   m_noiseTool->getEffectiveSigma(cell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::TOTALNOISE) :
	   m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::TOTALNOISE);
	 float usedEnergy = m_absEinSigma ? fabs((*it)->e()) : (*it)->e();
	 if ( (usedEnergy > (m_cutvalue*tilecellnoise)) &&
	      (!m_CheckCellWithinRoI  || this->CellInTheRoi(cell,roi)) ) {
	   pCaloCellContainer.push_back(cell);
	 }
       }
     } else {
       for ( it = m_iBegin; it != m_iEnd; ++it) {
         CaloCell* cell = (CaloCell*)(*it);	 
	 if ( !m_CheckCellWithinRoI  || this->CellInTheRoi(cell,roi) ) 
	   pCaloCellContainer.push_back(cell);
       }
     }
     if (m_timersvc) (m_timer.at(3))->pause();

  }

  // Including the TILE calorimeter in the setHasCalo */
  pCaloCellContainer.setHasCalo(CaloCell_ID::TILE);

  if (m_timersvc) { (m_timer.at(0))->stop(); (m_timer.at(1))->stop(); (m_timer.at(2))->stop(); (m_timer.at(3))->stop(); }

  return StatusCode::SUCCESS;
}

