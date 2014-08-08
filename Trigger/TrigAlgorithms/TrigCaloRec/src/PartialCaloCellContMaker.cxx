/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PartialCaloCellContMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   Ademar Delgado, tavares@cern.ch
//
// Description: Fill an partial CellContainer with the cells of the 
//           SuperRoI using the new unpacking prepared for L2
//              Important: Need to receive an TrigSuperRoi that should
//           be made by the CaloSuperRoIMaker
// Updates:
// 
// ********************************************************************

#include "TrigCaloRec/PartialCaloCellContMaker.h"


//--------------------------------------------------------------
PartialCaloCellContMaker::PartialCaloCellContMaker(const std::string & type, 
  const std::string & name, const IInterface* parent)
  : IAlgToolEFCalo(type, name, parent)
{
//  declareProperty("CheckCellWithinRoI", m_CheckCellWithinRoI = false);
//  declareProperty("DoTileCellsNoiseSuppression", do_TileCells_noise_suppression = 0);
//  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
//  declareProperty("CutValue",m_cutvalue = 2,"Cell accepted if e>m_cutvalue*tilecellnoise");
//  declareProperty("AbsEinSigma",m_absEinSigma = 0,"0=asymmetric noise-sigma cut(default)"); 
//  declareProperty("TwoGaussianNoise",m_twogaussiannoise = false,"false=do not use double gaussian description for tile(default)"); 

/// need to use message service
//std::printf("#######################   PartialCaloCellContMaker - tmpAlg - v-0.0.0   #######################\n");
}

//--------------------------------------------------------------
PartialCaloCellContMaker::~PartialCaloCellContMaker(){
}

//--------------------------------------------------------------
StatusCode PartialCaloCellContMaker::initialize(){
  m_log = new MsgStream(msgSvc(), name());
  MsgStream &logStream = *m_log;
  
  if (toolSvc()->retrieveTool("TrigDataAccess", m_data).isFailure()) {
    logStream << MSG::FATAL << "Unable to retrieve TrigDataAccess" << endreq;
    return StatusCode::FAILURE;
  }
  
  /*  if (do_TileCells_noise_suppression!=0){
    
    if (m_noiseTool.retrieve().isFailure()) return StatusCode::FAILURE;

    std::cout << name() << " Two-Gaussian noise for Tile set to " << (m_twogaussiannoise ? "true" : "false") << std::endl;

  }*/

  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
     logStream<<MSG::ERROR << ": Unable to locate TrigTimer Service" << std::endl;
  }

  // Initialize timing service
  if (m_timersvc) {
     m_timer.push_back(m_timersvc->addItem("TCRec_PartialTotal"));
     m_timer.push_back(m_timersvc->addItem("TCRec_PartialRegionSelector"));
     m_timer.push_back(m_timersvc->addItem("TCRec_PartialDataAccess"));
     m_timer.push_back(m_timersvc->addItem("TCRec_PartialPushBackCells"));
  }
  
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------
StatusCode PartialCaloCellContMaker::finalize(){  
//  delete m_log;
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------
StatusCode PartialCaloCellContMaker::execute(CaloCellContainer &pCaloCellContainer, 
                                    const IRoiDescriptor &superRoi){
  // reset error
  m_error=(EFTILE<<28);

  if (m_timersvc) {
    (m_timer.at(0))->start();
    (m_timer.at(1))->start(); (m_timer.at(1))->pause();
    (m_timer.at(2))->start(); (m_timer.at(2))->pause();
    (m_timer.at(3))->start(); (m_timer.at(3))->pause();
  }
  
  //--------------------- begin getting data ---------------------
	//! Liquid Argon Trigger Tower EM Calorimeter
  for(int sa=0;sa<4;sa++){ //for TTEM     
    fillCellContainer(pCaloCellContainer, superRoi, TTEM, sa);
  }//end TTEM
  
  //! Liquid Argon Trigger Tower HEC Calorimeter  
  for(int sa=0;sa<4;sa++){ //for TTHEC
    //		fillCellContainer(pCaloCellContainer, superRoi, TTHEC, sa);
  }//end TTHEC
  
  //! Liquid Argon Trigger Tower FCALEM Calorimeter
  //	fillCellContainer(pCaloCellContainer, superRoi, FCALEM, 0);
  
  //! Liquid Argon Trigger Tower FCALHAD Calorimeter
  for(int sa=0;sa<2;sa++){ //for FCALHAD
    //		fillCellContainer(pCaloCellContainer, superRoi, FCALHAD, sa);
  }//end FCALHAD
  
  //! Tile Calorimeters
  fillCellContainer(pCaloCellContainer, superRoi, TILE, 0);
  //--------------------- finish getting data ---------------------
  
#if 0
  /// need to use message service
  
  //!*****************************************************************************
  std::printf("-PartialCaloCellContMaker::finalizeOutput------------------ cheking PS container    ------------------------------ \n");  
  if(pCaloCellContainer.hasCalo(CaloCell_ID::LAREM)){
    std::printf("-PartialCaloCellContMaker::finalizeOutput-------CaloCell_ID::LAREM----------- have calo    ------------------------------ \n");
  }else{
    std::printf("-PartialCaloCellContMaker::finalizeOutput-------CaloCell_ID::LAREM----------- haven't calo ------------------------------ \n");
  }
  if(pCaloCellContainer.hasCalo(CaloCell_ID::LARHEC)){
    std::printf("-PartialCaloCellContMaker::finalizeOutput-------CaloCell_ID::LARHEC---------- have calo    ------------------------------ \n");
  }else{
    std::printf("-PartialCaloCellContMaker::finalizeOutput-------CaloCell_ID::LARHEC---------- haven't calo ------------------------------ \n");
  }
  if(pCaloCellContainer.hasCalo(CaloCell_ID::LARFCAL)){
    std::printf("-PartialCaloCellContMaker::finalizeOutput-------CaloCell_ID::LARFCAL--------- have calo    ------------------------------ \n");
  }else{
    std::printf("-PartialCaloCellContMaker::finalizeOutput-------CaloCell_ID::LARFCAL--------- haven't calo ------------------------------ \n");
  }
  if(pCaloCellContainer.hasCalo(CaloCell_ID::TILE)){
    std::printf("-PartialCaloCellContMaker::finalizeOutput-------CaloCell_ID::TILE------------ have calo    ------------------------------ \n");
  }else{
    std::printf("-PartialCaloCellContMaker::finalizeOutput-------CaloCell_ID::TILE------------ haven't calo ------------------------------ \n");
  }
  std::printf("-PartialCaloCellContMaker::finalizeOutput------------------------------------------------------------------------- \n");  
  //!*****************************************************************************  
#endif
  
  if (m_timersvc) { (m_timer.at(0))->stop(); (m_timer.at(1))->stop(); (m_timer.at(2))->stop(); (m_timer.at(3))->stop(); }
  return StatusCode::SUCCESS;
}


//!____________________ core functions _____________________
void PartialCaloCellContMaker::fillCellContainer(CaloCellContainer &pCaloCellContainer, const IRoiDescriptor& superRoi, 
						 const DETID detid, const int sampling){
  //  MsgStream &logStream = *m_log;
  
  if (m_timersvc) (m_timer.at(1))->resume();
  m_data->RegionSelector(sampling,superRoi, detid);// 0 and TILE without meaning
  if (m_timersvc) (m_timer.at(1))->pause();
  
  CaloCell_ID::SUBCALO iSubCalo=CaloCell_ID::NOT_VALID;
  
  if( detid != TILE ){                   // LAr
    //Load cell Collections
    if (m_timersvc) (m_timer.at(2))->resume();
    if(m_data->LoadCollections(m_iBegin,m_iEnd,sampling,!sampling).isFailure()){
      //logStream<<MSG::ERROR<<" Failed to load cell collections "<<endreq;
      /*		  if (m_data->report_error()) {
			  m_error=m_data->report_error()+(EFTILE<<28);
			  }*/
      if (m_timersvc) (m_timer.at(2))->pause();
      return;
    }
    if (m_timersvc) (m_timer.at(2))->pause();
    
    //set the CellContainer subCalo
    if(m_iBegin!=m_iEnd){ 
      switch (detid) {
      case TTEM:
        iSubCalo=CaloCell_ID::LAREM;
        break;
      case TTHEC:
        iSubCalo=CaloCell_ID::LARHEC;
        break;
      case FCALEM:
      case FCALHAD:
        iSubCalo=CaloCell_ID::LARFCAL;
        break;
      default:
        break;
      } 
    }
    
    //Fill the cell container
    if (m_timersvc) (m_timer.at(3))->resume();
    for(m_it = m_iBegin;m_it != m_iEnd; ++m_it){
      CaloCell* cell = (CaloCell*)(*m_it);
      pCaloCellContainer.push_back(cell);
    }
    if (m_timersvc) (m_timer.at(3))->pause();
    
  }else if(m_data->TileContSize()!=0){   // TILE
    for( unsigned int iR=0; iR!=m_data->TileContSize();++iR ){ 
      //Load cell Collections
      if (m_timersvc) (m_timer.at(2))->resume();
      if(m_data->LoadCollections(m_itBegin,m_itEnd,sampling,!sampling).isFailure()){
	//logStream<<MSG::ERROR<<" Failed to load cell collections "<<endreq;
	if (m_data->report_error()) {
          m_error=m_data->report_error()+(EFTILE<<28);
        }
	if (m_timersvc) (m_timer.at(2))->pause();
	//continue;
      }
      if (m_timersvc) (m_timer.at(2))->pause();
      
      //set the CellContainer subCalo
      if(m_itBegin!=m_itEnd){ 
        iSubCalo=CaloCell_ID::TILE;
      }
      
      //Fill the cell container
      if (m_timersvc) (m_timer.at(3))->resume();
      for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt){
	CaloCell* cell = (CaloCell*)(*m_itt);
	pCaloCellContainer.push_back(cell);
      }
      if (m_timersvc) (m_timer.at(3))->pause();
    }
  }           
  
  pCaloCellContainer.setHasCalo(iSubCalo);
}





