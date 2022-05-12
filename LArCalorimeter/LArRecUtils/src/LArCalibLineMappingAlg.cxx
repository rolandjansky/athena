//dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibLineMappingAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"


StatusCode LArCalibLineMappingAlg::initialize() {
  ATH_MSG_DEBUG("initializing");

  // Read Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode LArCalibLineMappingAlg::execute() {
  ATH_MSG_DEBUG("executing");

  SG::WriteCondHandle<LArCalibLineMapping> writeHandle{m_writeKey};
  
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }  

  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_readKey};
  const AthenaAttributeList* attr{*readHandle};

  if (attr==nullptr) {
    ATH_MSG_ERROR("Failed to retrieve CondAttributeListCollection with key " << m_readKey.key());
    return StatusCode::FAILURE;
  }
  writeHandle.addDependency(readHandle);

  const LArOnlineID_Base* onlineID;
  if(m_isSuperCell) {
    const LArOnline_SuperCellID* scidhelper;
    ATH_CHECK(detStore()->retrieve(scidhelper,"LArOnline_SuperCellID"));
    onlineID=scidhelper; //cast to base-class
  } else {
    const LArOnlineID* idhelper;
    ATH_CHECK(detStore()->retrieve(idhelper,"LArOnlineID"));
    onlineID=idhelper; //cast to base-class
  }   

  std::unique_ptr<LArCalibLineMapping> calibLineMap=std::make_unique<LArCalibLineMapping>(onlineID);

  std::vector<std::vector<HWIdentifier> >&onlHashToCalibLines=calibLineMap->m_onlHashToCalibLines; //basically a shorthand

  //The following lines are copied from the callback method of the old-style LArCablingService.cxx
  const coral::Blob& blobCalib=(*attr)["OnlineHashToCalibIds"].data<coral::Blob>();
  const unsigned nEntries=blobCalib.size()/sizeof(uint32_t);
  const uint32_t* pBlobCalib=static_cast<const uint32_t*>(blobCalib.startingAddress());

  const unsigned chanMax=onlineID->channelHashMax();

  onlHashToCalibLines.resize(chanMax);
  
  unsigned blobIdx=0;
  unsigned chanIdx=0;
  unsigned totCalibLines=0;
  for (;blobIdx<nEntries && chanIdx<chanMax;++chanIdx) {
    const unsigned nCalibLines=pBlobCalib[blobIdx++];
    totCalibLines+=nCalibLines;
    if (nCalibLines>m_maxCalibLines) {
      msg(MSG::ERROR) << "Found unresonable large number of calib lines (" << nCalibLines << ") for channel hash " << chanIdx << endmsg;
      return StatusCode::FAILURE;
    }
    onlHashToCalibLines[chanIdx].clear();
    for (unsigned i=0;i<nCalibLines && blobIdx<nEntries;++i) {
      onlHashToCalibLines[chanIdx].emplace_back(pBlobCalib[blobIdx++]);
    }//End loop over calib-lines
  }//end loop over channels
  msg() << MSG::INFO << "Done reading readout/calibration line mapping." << endmsg;
  ATH_MSG_DEBUG("BlobIdx=" << blobIdx<<", chanIdx=" << chanIdx << ", totCalibLines=" << totCalibLines);


  if(writeHandle.record(std::move(calibLineMap)).isFailure()) {
    ATH_MSG_ERROR("Could not record LArCalibLineMapping object with " 
		  << writeHandle.key() 
		  << " with EventRange " << writeHandle.getRange()
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");
 
  return StatusCode::SUCCESS;
}

