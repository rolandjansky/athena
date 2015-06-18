/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************************
//
// To correct average energy per cell, computed from CaloMBAverageCorrTool
//  this is aimed for MC where pileup effects are computed on the fly
//  and is relevant only for bunch spacing not equal to 25 ns
//
// G.Unal    19 novembre 2008 ....  first version
//
// ****************************************************************************************

#include "CaloCellCorrection/CaloCellMBAverageCorr.h"
#include "CaloEvent/CaloCell.h"
#include "GaudiKernel/MsgStream.h"


// ======================================================
// Constructor

CaloCellMBAverageCorr::CaloCellMBAverageCorr(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  : CaloCellCorrection(type, name, parent),
    m_caloMBAverageTool("CaloMBAverageTool")
{
 declareInterface<CaloCellCorrection>(this);
 declareProperty("CaloMBAverageTool",m_caloMBAverageTool,"Tool to compute shift from MB average");
}

//========================================================
// Initialize

StatusCode CaloCellMBAverageCorr::initialize()
{
 MsgStream  log(msgSvc(),name());
 log << MSG::INFO << " in CaloCellMBAverageCorr::initialize() " << endreq;

 if (m_caloMBAverageTool.retrieve().isFailure()) {
  log << MSG::ERROR << " cannot retrieve m_caloMBAverageTool";
  return StatusCode::FAILURE;
 }

  return StatusCode::SUCCESS;

}


// ============================================================================

void CaloCellMBAverageCorr::MakeCorrection(CaloCell* theCell)
{
   float pedestal = m_caloMBAverageTool->average(theCell);
   theCell->addEnergy(-pedestal);
}
