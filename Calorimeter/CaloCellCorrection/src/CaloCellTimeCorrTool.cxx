/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellTimeCorrTool.h" 
#include "CaloEvent/CaloCellContainer.h"
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"


CaloCellTimeCorrTool::CaloCellTimeCorrTool (const std::string& type, 
						const std::string& name, 
						const IInterface* parent) :
  base_class (type, name, parent) {}

CaloCellTimeCorrTool::~CaloCellTimeCorrTool() = default;


StatusCode CaloCellTimeCorrTool::initialize() {
  ATH_CHECK(m_timeShiftFldr.initialize());
  return StatusCode::SUCCESS;
}



StatusCode CaloCellTimeCorrTool::process (CaloCellContainer* theCaloCellContainer,
					  const EventContext& ctx) const {
  
  SG::ReadCondHandle<AthenaAttributeList> timeShiftHdl(m_timeShiftFldr,ctx);


  //Possible optimization: If the following lines turn out to be slow to be executed on every event, 
  //put them into a conditions algo
  const coral::Blob& blob = (**timeShiftHdl)["CaloCondBlob16M"].data<coral::Blob>();
  if (blob.size()<3) {
    ATH_MSG_DEBUG("Found empty blob, no corretion needed");
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<const CaloCondBlobFlt> corrValues(CaloCondBlobFlt::getInstance(blob));
  ATH_MSG_DEBUG("Database folder has values for " << corrValues->getNChans() << " channels and " << corrValues->getNGains() << " gains.");
  
  CaloCellContainer::iterator it=theCaloCellContainer->begin();
  CaloCellContainer::iterator it_e=theCaloCellContainer->end();
  for(;it!=it_e;++it) {
    CaloCell* theCell=(*it);
    const IdentifierHash& hash_id=theCell->caloDDE()->calo_hash();
    if (hash_id<corrValues->getNChans()) {
      const float& shift= corrValues->getData(hash_id);
      theCell->setTime(theCell->time()+shift);
    }//end if hash_id<NChans
  }//end loop over cells
  return StatusCode::SUCCESS;
}
