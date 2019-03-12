/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     RoILArHadCellContMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   C. Santamarina
// Implements the new data unpacking developped by Denis.
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigCaloRec/RoILArHadCellContMaker.h"
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

#include <math.h>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // legacy trigger code

RoILArHadCellContMaker::RoILArHadCellContMaker(const std::string & type, const std::string & name,
	 const IInterface* parent): IAlgToolEFCalo(type, name, parent),
				    m_data(NULL)
{

  declareProperty("DoLArCellsNoiseSuppression", m_do_LArCells_noise_suppression = 1);
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
  declareProperty("CutValue",m_cutvalue = 2,"Cell accepted if e>m_cutvalue*larcellnoise");
  declareProperty("AbsEinSigma",m_absEinSigma = 0,"0=asymmetric noise-sigma cut(default)"); 
}

//
RoILArHadCellContMaker::~RoILArHadCellContMaker(){
}

StatusCode RoILArHadCellContMaker::initialize(){

  if (toolSvc()->retrieveTool("TrigDataAccess", m_data).isFailure()) {
      return StatusCode::FAILURE;
  }

  if (m_do_LArCells_noise_suppression!=0){

    if (m_noiseTool.retrieve().isFailure()) return StatusCode::FAILURE;

  } else {
    m_noiseTool.disable();
  }

  // Initialize timing service

  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
     std::cout << ": Unable to locate TrigTimer Service" << std::endl;
  }

  if (m_timersvc) {
     m_timer.push_back(m_timersvc->addItem("TCRec_LArHadTotal"));
     m_timer.push_back(m_timersvc->addItem("TCRec_LArHadRegionSelector"));
     m_timer.push_back(m_timersvc->addItem("TCRec_LArHadDataAccess"));
     m_timer.push_back(m_timersvc->addItem("TCRec_LArHadPushBackCells"));
  }

  return StatusCode::SUCCESS;

}

StatusCode RoILArHadCellContMaker::finalize(){



  return StatusCode::SUCCESS;
}

StatusCode RoILArHadCellContMaker::execute(CaloCellContainer &pCaloCellContainer, 
					   const IRoiDescriptor& roi ) {

  // reset error  
  //m_error=(EFTTHEC<<28);
  m_error=0;

  if (m_timersvc) {
    (m_timer.at(0))->start();
    (m_timer.at(1))->start(); (m_timer.at(1))->pause();
    (m_timer.at(2))->start(); (m_timer.at(2))->pause();
    (m_timer.at(3))->start(); (m_timer.at(3))->pause();
  }

  for ( unsigned int sampling = 0; sampling <4; sampling++) {

     if (m_timersvc) (m_timer.at(1))->resume();
     m_data->RegionSelector(sampling,roi,TTHEC);
     if (m_timersvc) (m_timer.at(1))->pause();

     if (m_timersvc) (m_timer.at(2))->resume();
     if((m_data->LoadCollections(m_iBegin,m_iEnd,sampling,!sampling)).isFailure()) {
       ATH_MSG_DEBUG("unable to load cell collections");
       //return StatusCode::FAILURE;
     }
     if (m_data->report_error()) {
       m_error=m_data->report_error(); // + (EFTTHEC<<28);
       if (m_timersvc) (m_timer.at(2))->pause();
       //continue;
     }
       
     if (m_timersvc) (m_timer.at(2))->pause();

     if (m_timersvc) (m_timer.at(3))->resume();
     LArTT_Selector<LArCellCont>::const_iterator it;

      if (m_do_LArCells_noise_suppression!=0){
        for(it = m_iBegin;it != m_iEnd; ++it) {
          //const CaloDetDescrElement* caloDDE = (*it)->caloDDE();
          //double larcellnoise = m_noiseTool->getNoise(caloDDE,ICalorimeterNoiseTool::TOTALNOISE);
          double larcellnoise = m_noiseTool->getNoise(*it,ICalorimeterNoiseTool::TOTALNOISE);
	  float usedEnergy = m_absEinSigma ? fabs((*it)->e()) : (*it)->e();
	  if( usedEnergy > (m_cutvalue*larcellnoise)) {
            CaloCell* cell = (CaloCell*)(*it);
            pCaloCellContainer.push_back(cell);
          }
        }
      } else{

       for ( it = m_iBegin; it != m_iEnd; ++it) {
          CaloCell* cell = (CaloCell*)(*it);
          pCaloCellContainer.push_back(cell);
       }
     }

     if (m_timersvc) (m_timer.at(3))->pause();

  }

  // Including the LAREM calorimeter in the setHasCalo */
  pCaloCellContainer.setHasCalo(CaloCell_ID::LARHEC);

  if (m_timersvc) { (m_timer.at(0))->stop(); (m_timer.at(1))->stop(); (m_timer.at(2))->stop(); (m_timer.at(3))->stop(); }

  return StatusCode::SUCCESS;

}
