/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellRec/LArCellRescaler.h" 
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"

LArCellRescaler::LArCellRescaler (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
  CaloCellCorrection(type, name, parent) { 
  declareInterface<CaloCellCorrection>(this); 
  declareProperty("CorrectionKey",m_key="ZeeCalibration",
		  "Key of the CaloCellFactor object to be used");

}
                                                                                

LArCellRescaler::~LArCellRescaler() {}


StatusCode LArCellRescaler::initialize() {
  ATH_MSG_INFO( " initialization "  );

  // sc=m_detStore->regHandle(m_factors,m_key);
//   if (sc.isFailure()) {
//     log << MSG::ERROR << "Cound not register DataHandle<CaloRec::CaloCellFactor> with key "
// 	<< m_key << endmsg;
//     return sc;
//   }


  ATH_CHECK( detStore()->regFcn(&LArCellRescaler::checkConstants,
                                dynamic_cast<LArCellRescaler*>(this),
                                m_factors,m_key) );

  return StatusCode::SUCCESS;
}


StatusCode LArCellRescaler::checkConstants(IOVSVC_CALLBACK_ARGS) {
  const CaloCell_ID* cellID;
  ATH_CHECK( detStore()->retrieve(cellID) );
  IdentifierHash emMin, emMax;
  cellID->calo_cell_hash_range(CaloCell_ID::LAREM,emMin,emMax);
  if (m_factors->size() != emMax) {
    ATH_MSG_ERROR( "CaloCellFactor object with key " << m_key 
                   << " has wrong size " << m_factors->size() 
                   << " HashMax is " <<  emMax  );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "CaloCellFactor object with key " << m_key << " has proper size."  );
  return StatusCode::SUCCESS;
}



void LArCellRescaler::MakeCorrection(CaloCell* theCell) {
  const IdentifierHash& hash_id=theCell->caloDDE()->calo_hash();
  if (m_factors.isValid() && hash_id<m_factors->size())
    theCell->setEnergy(theCell->energy()*(*m_factors)[hash_id]);
  return;
}
