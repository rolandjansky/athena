/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     RoILArEMCellContMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   C. Santamarina
// Implements the new data unpacking developped by Denis.
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigCaloRec/RoILArEMCellContMaker.h"
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


RoILArEMCellContMaker::RoILArEMCellContMaker(const std::string & type, const std::string & name,
	 const IInterface* parent): IAlgToolEFCalo(type, name, parent),
				    m_data(NULL)
{

  declareProperty("DoLArCellsNoiseSuppression", m_do_LArCells_noise_suppression = 1);
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
  declareProperty("CutValue",m_cutvalue = 2,"Cell accepted if e>m_cutvalue*larcellnoise");
  declareProperty("AbsEinSigma",m_absEinSigma = 0,"0=asymmetric noise-sigma cut(default)");  
}

//
RoILArEMCellContMaker::~RoILArEMCellContMaker(){
}

StatusCode RoILArEMCellContMaker::initialize(){

  if (toolSvc()->retrieveTool("TrigDataAccess", m_data).isFailure()) {
     return StatusCode::FAILURE;
  }

  if (m_do_LArCells_noise_suppression!=0){

    if (m_noiseTool.retrieve().isFailure()) return StatusCode::FAILURE;

  } else {
    m_noiseTool.disable();
  }

  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
     std::cout << ": Unable to locate TrigTimer Service" << std::endl;
  }

  // Initialize timing service
  if (m_timersvc) {
     m_timer.push_back(m_timersvc->addItem("TCRec_LArEMTotal"));
     m_timer.push_back(m_timersvc->addItem("TCRec_LArEMRegionSelector"));
     m_timer.push_back(m_timersvc->addItem("TCRec_LArEMDataAccess"));
     m_timer.push_back(m_timersvc->addItem("TCRec_LArEMPushBackCells"));
  }

  return StatusCode::SUCCESS;
}

StatusCode RoILArEMCellContMaker::finalize(){
            return StatusCode::SUCCESS;
}

StatusCode RoILArEMCellContMaker::execute(CaloCellContainer &pCaloCellContainer, 
					  const IRoiDescriptor& roi ) { 
    // reset error  
    //m_error=(EFTTEM<<28);
    m_error=0;

    if (m_timersvc) {
      (m_timer.at(0))->start();
      (m_timer.at(1))->start(); (m_timer.at(1))->pause();
      (m_timer.at(2))->start(); (m_timer.at(2))->pause();
      (m_timer.at(3))->start(); (m_timer.at(3))->pause();
    }

    for (int sampling = 0; sampling <4; sampling ++) {
        // Get detector offline ID's for Collections
      if (m_timersvc) (m_timer.at(1))->resume();
      m_data->RegionSelector(sampling,roi);
      if (m_timersvc) (m_timer.at(1))->pause();

      if (m_timersvc) (m_timer.at(2))->resume();
      if((m_data->LoadCollections(m_iBegin,m_iEnd,sampling,false)).isFailure()) {
	ATH_MSG_DEBUG("unable to load cell collections");
	//return StatusCode::FAILURE;
      }
      if (m_data->report_error()) {
	m_error=m_data->report_error(); // + (EFTTEM<<28);
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
    pCaloCellContainer.setHasCalo(CaloCell_ID::LAREM);

    if (m_timersvc) { (m_timer.at(0))->stop(); (m_timer.at(1))->stop(); (m_timer.at(2))->stop(); (m_timer.at(3))->stop(); }

    return StatusCode::SUCCESS;
}

