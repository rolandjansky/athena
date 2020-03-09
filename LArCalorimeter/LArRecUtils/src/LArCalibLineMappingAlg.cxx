//dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibLineMappingAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"


LArCalibLineMappingAlg::LArCalibLineMappingAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_readKey("/LAR/Identifier/CalibIdMap"),
  m_writeKey("LArCalibLineMap"),
  m_condSvc("CondSvc",name),
  m_isSuperCell(false),
  m_maxCalibLines(4)
{
  declareProperty("ReadKey",m_readKey);
  declareProperty("WriteKey",m_writeKey);
  declareProperty("isSuperCell",m_isSuperCell);
  declareProperty("MaxCL",m_maxCalibLines,"in case of SuperCell should be set to higher value then default 4");
}

LArCalibLineMappingAlg::~LArCalibLineMappingAlg() {}

StatusCode LArCalibLineMappingAlg::initialize() {

  ATH_MSG_DEBUG("initializing");

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );
  // Read Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );
  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
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


  // Define validity of the output cond object and record it
  EventIDRange rangeW;
  if(!readHandle.range(rangeW)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  if(writeHandle.record(rangeW,calibLineMap.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record LArCalibLineMapping object with " 
		  << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");
 
  return StatusCode::SUCCESS;
}

