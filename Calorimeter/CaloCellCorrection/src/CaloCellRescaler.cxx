/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellCorrection/CaloCellRescaler.h" 
#include "GaudiKernel/MsgStream.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

CaloCellRescaler::CaloCellRescaler (const std::string& type, 
                                  const std::string& name, 
                                  const IInterface* parent) :
  CaloCellCorrection(type, name, parent) { 
  declareInterface<CaloCellCorrection>(this);
  declareProperty("FactorToCellsEMB0",  m_factorToCells[CaloCell_ID::PreSamplerB] = 1.0, "cells in emb0 are scaled by FactorToCellsEMB0");
  declareProperty("FactorToCellsEMB1",  m_factorToCells[CaloCell_ID::EMB1] = 1.0,        "cells in emb1 are scaled by FactorToCellsEMB1");
  declareProperty("FactorToCellsEMB2",  m_factorToCells[CaloCell_ID::EMB2] = 1.0,        "cells in emb2 are scaled by FactorToCellsEMB2");
  declareProperty("FactorToCellsEMB3",  m_factorToCells[CaloCell_ID::EMB3] = 1.0,        "cells in emb3 are scaled by FactorToCellsEMB3");
  declareProperty("FactorToCellsEME0",  m_factorToCells[CaloCell_ID::PreSamplerE] = 1.0, "cells in eme0 are scaled by FactorToCellsEME0");
  declareProperty("FactorToCellsEME1",  m_factorToCells[CaloCell_ID::EME1] = 1.0,        "cells in eme1 are scaled by FactorToCellsEME1");
  declareProperty("FactorToCellsEME2",  m_factorToCells[CaloCell_ID::EME2] = 1.0,        "cells in eme2 are scaled by FactorToCellsEME2");
  declareProperty("FactorToCellsEME3",  m_factorToCells[CaloCell_ID::EME3] = 1.0,        "cells in eme3 are scaled by FactorToCellsEME3");
  declareProperty("FactorToCellsHEC0",  m_factorToCells[CaloCell_ID::HEC0] = 1.0,        "cells in hec0 are scaled by FactorToCellsHEC0");
  declareProperty("FactorToCellsHEC1",  m_factorToCells[CaloCell_ID::HEC1] = 1.0,        "cells in hec1 are scaled by FactorToCellsHEC1");
  declareProperty("FactorToCellsHEC2",  m_factorToCells[CaloCell_ID::HEC2] = 1.0,        "cells in hec2 are scaled by FactorToCellsHEC2");
  declareProperty("FactorToCellsHEC3",  m_factorToCells[CaloCell_ID::HEC3] = 1.0,        "cells in hec3 are scaled by FactorToCellsHEC3");
  declareProperty("FactorToCellsTILEB0",m_factorToCells[CaloCell_ID::TileBar0] = 1.0,"cells in tilebar0 are scaled by FactorToCellsTILEB0");
  declareProperty("FactorToCellsTILEB1",m_factorToCells[CaloCell_ID::TileBar1] = 1.0,"cells in tilebar1 are scaled by FactorToCellsTILEB1");
  declareProperty("FactorToCellsTILEB2",m_factorToCells[CaloCell_ID::TileBar2] = 1.0,"cells in tilebar2 are scaled by FactorToCellsTILEB2");
  declareProperty("FactorToCellsTILEG1",m_factorToCells[CaloCell_ID::TileGap1] = 1.0,"cells in tilegap1 are scaled by FactorToCellsTILEG1");
  declareProperty("FactorToCellsTILEG2",m_factorToCells[CaloCell_ID::TileGap2] = 1.0,"cells in tilegap2 are scaled by FactorToCellsTILEG2");
  declareProperty("FactorToCellsTILEG3",m_factorToCells[CaloCell_ID::TileGap3] = 1.0,"cells in tilegap3 are scaled by FactorToCellsTILEG3");
  declareProperty("FactorToCellsTILEE0",m_factorToCells[CaloCell_ID::TileExt0] = 1.0,"cells in tileext0 are scaled by FactorToCellsTILEE0");
  declareProperty("FactorToCellsTILEE1",m_factorToCells[CaloCell_ID::TileExt1] = 1.0,"cells in tileext1 are scaled by FactorToCellsTILEE1");
  declareProperty("FactorToCellsTILEE2",m_factorToCells[CaloCell_ID::TileExt2] = 1.0,"cells in tileext2 are scaled by FactorToCellsTILEE2");
  declareProperty("FactorToCellsFCAL0", m_factorToCells[CaloCell_ID::FCAL0] = 1.0,      "cells in fcal0 are scaled by FactorToCellsFCAL0");
  declareProperty("FactorToCellsFCAL1", m_factorToCells[CaloCell_ID::FCAL1] = 1.0,      "cells in fcal1 are scaled by FactorToCellsFCAL1");
  declareProperty("FactorToCellsFCAL2", m_factorToCells[CaloCell_ID::FCAL2] = 1.0,      "cells in fcal2 are scaled by FactorToCellsFCAl2");
  declareProperty("FactorToCellsMBTS",  m_factorToCells[CaloCell_ID::Unknown] = 1.0,     "cells in MBTS are scaled by FactorToCellsMBTS");
}


CaloCellRescaler::~CaloCellRescaler() {}


StatusCode CaloCellRescaler::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " initialization " << endreq;
  return StatusCode::SUCCESS;
}


void CaloCellRescaler::MakeCorrection(CaloCell* theCell) {

  const CaloDetDescrElement* caloDDE = theCell->caloDDE();
  if (caloDDE) {
    theCell->scaleEnergy( m_factorToCells[caloDDE->getSampling()] );
  }
  return;
}
