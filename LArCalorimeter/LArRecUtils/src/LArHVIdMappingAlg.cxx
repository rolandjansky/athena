/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHVIdMappingAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArCabling/LArHVIdMappingReader.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"

#include "PathResolver/PathResolver.h"

#include <fstream>

StatusCode LArHVIdMappingAlg::initialize() {

  ATH_MSG_DEBUG("initializing");

  // Read Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode LArHVIdMappingAlg::execute() {
  ATH_MSG_DEBUG("executing");

  SG::WriteCondHandle<LArHVIdMapping> writeHandle{m_writeKey};
  
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }  

  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_readKey};
  const AthenaAttributeList* attr{*readHandle};
  writeHandle.addDependency(readHandle);

  if (!attr) {
    ATH_MSG_WARNING( " Cannot find /LAR/IdentifierOfl/HVLineToElectrodeMap from database, Use ASCII file indeed !!!");
  }

  const CaloCell_ID* calocellID = nullptr;
  ATH_CHECK(detStore()->retrieve(calocellID,"CaloCell_ID"));

  const LArHVLineID* hvlineID = nullptr;
  ATH_CHECK(detStore()->retrieve(hvlineID,"LArHVLineID"));
  
  const LArElectrodeID* electrodeID = nullptr;
  ATH_CHECK(detStore()->retrieve(electrodeID,"LArElectrodeID"));

  std::unique_ptr<LArHVIdMapping> HVIdMap = std::make_unique<LArHVIdMapping>(calocellID, hvlineID, electrodeID);
  std::map<HWIdentifier,HWIdentifier> &elecLineMap = HVIdMap->m_electrode_hvline_map; //basically a shorthand
  std::map<HWIdentifier,std::vector<HWIdentifier> > &lineElecMap = HVIdMap->m_hvline_electrode_map;

  LArHVIdMappingReader reader;
  ATH_CHECK( reader.read (attr,
                          *hvlineID,
                          *electrodeID,
                          elecLineMap,
                          lineElecMap) );


  if(writeHandle.record(std::move(HVIdMap)).isFailure()) {
    ATH_MSG_ERROR("Could not record LArCalibLineMapping object with " 
		  << writeHandle.key() 
		  << " with EventRange " << writeHandle.getRange()
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

 
  return StatusCode::SUCCESS;
}

