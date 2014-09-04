/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellRec/LArCellHVCorr.h" 
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCell.h"
#include "LArRecUtils/LArHVCorrTool.h"


LArCellHVCorr::LArCellHVCorr (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
  CaloCellCorrection(type, name, parent),
  m_hvCorrTool("LArHVCorrTool"),
  m_keyHVScaleCorr("LArHVScaleCorr"),
  m_undoHVonline(true)
{ 
  declareInterface<CaloCellCorrection>(this); 
  declareProperty("HVCorrTool",m_hvCorrTool);
  declareProperty("keyHVScaleCorr",m_keyHVScaleCorr);
  declareProperty("undoHVOnline",m_undoHVonline);
}


LArCellHVCorr::~LArCellHVCorr() {}


StatusCode LArCellHVCorr::initialize() {

  StatusCode  sc = m_hvCorrTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to find tool for LArHVCorrTool" << endreq; 
    return StatusCode::FAILURE;
  }

  if (m_undoHVonline) {
    sc = detStore()->regHandle(m_dd_HVScaleCorr,m_keyHVScaleCorr);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Unable to register handle to HVScaleCorr " << endreq;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}


void LArCellHVCorr::MakeCorrection(CaloCell* theCell) {

  Identifier id = theCell->ID();
  float hvcorr = m_hvCorrTool->Scale(id);

  if (m_undoHVonline) {
    float hvonline = m_dd_HVScaleCorr->HVScaleCorr(id);
    if (hvonline>0. && hvonline<100.) hvcorr = hvcorr/hvonline;
  }

  theCell->setEnergy(theCell->energy()*hvcorr);
  return;
}
