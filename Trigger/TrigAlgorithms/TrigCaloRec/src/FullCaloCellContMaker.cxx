/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     FullCaloCellContMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   C. Santamarina/B. Brelier
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigCaloRec/FullCaloCellContMaker.h"
#include "CaloEvent/CaloCellContainer.h"


#include "TrigT2CaloCommon/Calo_Def.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
//#include "CaloDetDescr/CaloDetDescrElement.h"
// noise suppression
//#include "CaloDetDescr/CaloDetDescrManager.h"
//#include <Identifier/HWIdentifier.h>
#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "AthenaMonitoring/IMonitorToolBase.h"

#include <math.h>


FullCaloCellContMaker::FullCaloCellContMaker(const std::string & type, const std::string & name,
//         const IInterface* parent): IAlgToolEFCalo(type, name, parent), 
//         m_timersvc("TrigTimerSvc","FullCaloCellContMaker"){
	 const IInterface* parent): IAlgToolEFCalo(type, name, parent),
				    m_data(NULL)
{

  declareProperty("DoLArCellsNoiseSuppression",  m_do_LArCells_noise_suppression = 0);
  declareProperty("DoTileCellsNoiseSuppression", m_do_TileCells_noise_suppression = 0);
  //declareProperty("TrigTimerSvc",m_timersvc,"TrigTimerSvc");
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
  declareProperty("CutValue",m_cutvalue = 2,"Cell accepted if e>m_cutvalue*larcellnoise");
  declareProperty("AbsEinSigma",m_absEinSigma = 0,"0=asymmetric noise-sigma cut(default)"); 
  declareProperty("TwoGaussianNoise",m_twogaussiannoise = false,"false=do not use double gaussian description for tile(default)"); 
}


//
FullCaloCellContMaker::~FullCaloCellContMaker(){
}

StatusCode FullCaloCellContMaker::initialize(){
  
  
  if (toolSvc()->retrieveTool("TrigDataAccess", m_data).isFailure()) {
    return StatusCode::FAILURE;
  }

  std::cout << name() << " m_do_LArCells_noise_suppression  set to " << (m_do_LArCells_noise_suppression  ? "true" : "false") << std::endl;
  std::cout << name() << " m_do_TileCells_noise_suppression set to " << (m_do_TileCells_noise_suppression ? "true" : "false") << std::endl;

  if(m_do_LArCells_noise_suppression!=0 || m_do_TileCells_noise_suppression!=0 ) {
    if(m_noiseTool.retrieve().isFailure()){
      ATH_MSG_WARNING("Unable to find CaloNoiseTool");
      
      return StatusCode::FAILURE;
    }
   // if ( m_do_TileCells_noise_suppression!=0 ) {
      std::cout << name() << "     Two-Gaussian noise for Tile set to " << (m_twogaussiannoise ? "true" : "false") << std::endl;
      std::cout << name() << "     Asbolute E in sigma         set to " << (m_absEinSigma      ? "true" : "false") << std::endl;
   // }
  }
  

  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
     std::cout << ": Unable to locate TrigTimer Service" << std::endl;
  } 

  // Initialize timing service

   if (m_timersvc) {
     m_timer.push_back( m_timersvc->addItem("FullCalo_Total"));
     m_timer.push_back( m_timersvc->addItem("FullCalo_TTEM"));
     m_timer.push_back( m_timersvc->addItem("FullCalo_TTHEC"));
     m_timer.push_back( m_timersvc->addItem("FullCalo_FCALHAD"));
     m_timer.push_back( m_timersvc->addItem("FullCalo_FCALEM"));
     m_timer.push_back( m_timersvc->addItem("FullCalo_TILE"));
   }
   

  return StatusCode::SUCCESS;
}

StatusCode FullCaloCellContMaker::finalize(){



  return StatusCode::SUCCESS;
}

StatusCode FullCaloCellContMaker::execute(CaloCellContainer &pCaloCellContainer){

  if (m_timersvc) (m_timer.at(0))->start();
  // reset error
  //m_error=(EFFULLCALO<<28);
  m_error = 0;
  
  /* use LoadFullCollections
     in TrigDataAccess */
    /** Load TTEM */
  if (m_timersvc) (m_timer.at(1))->start();
  //if(addFullLArCells(pCaloCellContainer, TTEM, true).isFailure() )
  if(addFullDetCells(pCaloCellContainer).isFailure() )
    return StatusCode::FAILURE;
  if (m_timersvc) (m_timer.at(1))->stop();
  
#ifdef OLDCODE
  /** Load TTHEC */
  if (m_timersvc) (m_timer.at(2))->start();
  if(addFullLArCells(pCaloCellContainer, TTHEC, true).isFailure() )
    return StatusCode::FAILURE;
  if (m_timersvc) (m_timer.at(2))->stop();
  
  /** Load FCALHAD */
  if (m_timersvc) (m_timer.at(3))->start();
  if(addFullLArCells(pCaloCellContainer, FCALHAD, true).isFailure() )
    return StatusCode::FAILURE;
  if (m_timersvc) (m_timer.at(3))->stop();
  
  /** Load FCALEM */
  if (m_timersvc) (m_timer.at(4))->start();
  if(addFullLArCells(pCaloCellContainer, FCALEM, true).isFailure() )
    return StatusCode::FAILURE;
  if (m_timersvc) (m_timer.at(4))->stop();

  /** Load Tile */
  if (m_timersvc) (m_timer.at(5))->start();
  if(addFullTileCells(pCaloCellContainer,/* 0,*/ true).isFailure() )
     return StatusCode::FAILURE;
  if (m_timersvc) (m_timer.at(5))->stop();
#endif

  if (m_timersvc) (m_timer.at(0))->stop();
  
  return StatusCode::SUCCESS;

}

StatusCode FullCaloCellContMaker::addFullLArCells(CaloCellContainer& pCaloCellContainer,
					    DETID detectorID, 
					    bool prepare)
{
  
  CaloCell_ID::SUBCALO iSubCalo=CaloCell_ID::NOT_VALID;

  //int iDet=0;
  //double const_cal=1.0;
  switch (detectorID) {
  case TTEM:
    iSubCalo=CaloCell_ID::LAREM;
    //iDet=0;
    //const_cal=const_cal_em;
    break;
  case TTHEC:
    iSubCalo=CaloCell_ID::LARHEC;
    //iDet=1;
    //const_cal=const_cal_had;
    break;
  case FCALEM:
  case FCALHAD:
    iSubCalo=CaloCell_ID::LARFCAL;
    //iDet=3;
    //const_cal=1.0; // ???
    break;
  default:
    break;
  }

  // Prepare data, load collections
  //  if (m_timersvc) m_timer[iDet][1]->resume();
  if ( (m_data->LoadFullCollections(m_iBegin,m_iEnd, detectorID, prepare)).isFailure()) return StatusCode::FAILURE;
  if (m_data->report_error()) {
    m_error |= m_data->report_error(); // + (EFFULLCALO<<28);
    //if (m_timersvc) m_timer[iDet][1]->stop();
    //return StatusCode::SUCCESS;  //don't return if problem with unpacking occurs
  }
  // Algorithmic time
  //  if (m_timersvc) m_timer[iDet][2]->resume();

  int counter = 0;

  // noise suppression
  if(m_do_LArCells_noise_suppression!=0){
    //const CaloDetDescrManager* m_CaloDetDescrManager = CaloDetDescrManager::instance();
    //if ( m_CaloDetDescrManager == 0 )
    //  {
	//std::cout << "ERROR : cannot allocate CaloDetDescrManager"
	 //    << endmsg;
//	return StatusCode::FAILURE;
 //     }

    for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) { // Main Loop
      //const CaloDetDescrElement* caloDDE = (*m_it)->caloDDE();
      //double larcellnoise = m_noiseTool->getNoise(caloDDE,ICalorimeterNoiseTool::TOTALNOISE);
      double larcellnoise = m_noiseTool->getNoise(*m_it,ICalorimeterNoiseTool::TOTALNOISE);
      float usedEnergy = m_absEinSigma ? fabs((*m_it)->e()) : (*m_it)->e();
      if(usedEnergy > (m_cutvalue*larcellnoise)){
	CaloCell* cell = (CaloCell*)(*m_it);
        //pCaloCellContainer.push_back(cell);
        pCaloCellContainer.push_back_fast(cell);
	++counter;
      }
    } // end loop
  } // with noise suppression
  else{

    for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) { // Main Loop
      CaloCell* cell = (CaloCell*)(*m_it);
      //pCaloCellContainer.push_back(cell);
      pCaloCellContainer.push_back_fast(cell);
      ++counter;
    } // end loop
  } // without noise suppression

  pCaloCellContainer.setHasCalo(iSubCalo);

  return StatusCode::SUCCESS;
}

StatusCode FullCaloCellContMaker::addFullTileCells(CaloCellContainer& pCaloCellContainer,
						   /* int sampling,*/
						   bool /*prepare*/)
{

  //int iDet=2;

  //  std::cout << "Full Tile container size " << m_data->TileFullContSize() << " " <<  m_data->TileContSize() << std::endl;

  // Prepare data, load collections
  for( unsigned int iR=0; iR!=m_data->TileFullContSize();++iR ){

    // Time to access Collection
    // if (m_timersvc) m_timer[iDet][1]->resume();
    if( (m_data->LoadFullCollections(m_itBegin,m_itEnd, iR, !iR)).isFailure()) return StatusCode::FAILURE;
     if (m_data->report_error()) {
       m_error |= m_data->report_error(); // + (EFFULLCALO<<28);
       //  if (m_timersvc) m_timer[iDet][1]->pause();
       //continue; // do not stop and process other layer.
     }
    // Algorithmic time
    // if (m_timersvc) m_timer[iDet][2]->resume();

    int counter = 0;
    //double const_cal = const_cal_had;

    if (m_do_TileCells_noise_suppression!=0) {
      for ( m_itt = m_itBegin; m_itt != m_itEnd; ++m_itt) {
	TileCellCollection::const_iterator& it = m_itt;
	CaloCell* cell = (CaloCell*)(*it);	 
	double tilecellnoise = m_twogaussiannoise ? 
	  m_noiseTool->getEffectiveSigma(cell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::TOTALNOISE) :
	  m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::TOTALNOISE);
	float usedEnergy = m_absEinSigma ? fabs((*it)->e()) : (*it)->e();
	if ( (usedEnergy > (m_cutvalue*tilecellnoise)) ) { 
	  pCaloCellContainer.push_back(cell);
	  ++counter;
	}
      }
    } 
    else {
      for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) { // Main Loop
    
	// possibly do some tower building
	
	CaloCell* cell = (CaloCell*)(*m_itt);
	//pCaloCellContainer.push_back(cell);
	pCaloCellContainer.push_back_fast(cell);
	++counter;
      } // end loop
    }

  } // end loop iR



pCaloCellContainer.setHasCalo(CaloCell_ID::TILE);
  
  return StatusCode::SUCCESS;
}

StatusCode FullCaloCellContMaker::addFullDetCells(CaloCellContainer& pCaloCellContainer)
{

  // Prepare data, load collections
  if ( (m_data->LoadFullCollections(m_icBegin,m_icEnd)).isFailure()) return StatusCode::FAILURE;
  if (m_data->report_error()) {
    m_error |= m_data->report_error(); // + (EFFULLCALO<<28);
    //if (m_timersvc) m_timer[iDet][1]->stop();
  }

  int counter = 0;

  // noise suppression
  if(m_do_LArCells_noise_suppression!=0){

    for(m_itc = m_icBegin;m_itc != m_icEnd; ++m_itc) { // Main Loop
      //const CaloDetDescrElement* caloDDE = (*m_itc)->caloDDE();
      //double larcellnoise = m_noiseTool->getNoise(caloDDE,ICalorimeterNoiseTool::TOTALNOISE);
      double larcellnoise = m_noiseTool->getNoise(*m_it,ICalorimeterNoiseTool::TOTALNOISE);
      float usedEnergy = m_absEinSigma ? fabs((*m_itc)->e()) : (*m_itc)->e();
      if(usedEnergy > (m_cutvalue*larcellnoise)){
        CaloCell* cell = (CaloCell*)(*m_itc);
        //pCaloCellContainer.push_back(cell);
        pCaloCellContainer.push_back_fast(cell);
        ++counter;
      }
    } // end loop
  } // with noise suppression
  else{

    // FIXME: const-cast allows changing cells in SG.
    for (CaloCellContainer::const_iterator it = m_icBegin;
         it != m_icEnd;
         ++it)
    {
      pCaloCellContainer.push_back (const_cast<CaloCell*>(*it));
    }
  } // without noise suppression

  pCaloCellContainer.setHasCalo(CaloCell_ID::LAREM);
  pCaloCellContainer.setHasCalo(CaloCell_ID::LARHEC);
  pCaloCellContainer.setHasCalo(CaloCell_ID::LARFCAL);
  pCaloCellContainer.setHasCalo(CaloCell_ID::TILE);

  // Assume data preparation made full container (by construction!)
  pCaloCellContainer.setHasTotalSize(true);
  pCaloCellContainer.setIsOrderedAndComplete(true);

  return StatusCode::SUCCESS;
}
