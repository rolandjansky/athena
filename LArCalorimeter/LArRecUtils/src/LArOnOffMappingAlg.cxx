//dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArOnOffMappingAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"


StatusCode LArOnOffMappingAlg::initialize() {

  ATH_MSG_DEBUG("initializing");

  // Read Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode LArOnOffMappingAlg::execute() {
  ATH_MSG_DEBUG("executing");

  SG::WriteCondHandle<LArOnOffIdMapping> writeHandle{m_writeKey};
  
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

  const LArOnlineID_Base* larOnlineID=nullptr;
  const CaloCell_Base_ID* calocellID=nullptr;

  //Identifier helper:
  if (m_isSuperCell) {
    const LArOnline_SuperCellID* scidhelper = nullptr;
    ATH_CHECK(detStore()->retrieve(scidhelper,"LArOnline_SuperCellID"));
    larOnlineID=scidhelper; //cast to base-class
    
    const CaloCell_SuperCell_ID* sccaloid = nullptr;
    ATH_CHECK(detStore()->retrieve(sccaloid,"CaloCell_SuperCell_ID"));
    calocellID=sccaloid; //cast to base-class

  }
  else {//regular cells
    const LArOnlineID* idhelper = nullptr;
    ATH_CHECK(detStore()->retrieve(idhelper,"LArOnlineID"));
    larOnlineID=idhelper; //cast to base-class

    const CaloCell_ID* sccaloid = nullptr;
    ATH_CHECK(detStore()->retrieve(sccaloid,"CaloCell_ID"));
    calocellID=sccaloid; //cast to base-class

  }


  std::unique_ptr<LArOnOffIdMapping> onOffMap=std::make_unique<LArOnOffIdMapping>(larOnlineID,calocellID);

  const coral::Blob& blobOnOff=(*attr)["OnlineHashToOfflineId"].data<coral::Blob>();
  unsigned nChan=blobOnOff.size()/sizeof(uint32_t);
  const uint32_t* pBlobOnOff=static_cast<const uint32_t*>(blobOnOff.startingAddress());  

  if (nChan!=larOnlineID->channelHashMax()) {
    ATH_MSG_ERROR("Number of channels read from DB (" << nChan << ") does not match online hash max ("
		  <<larOnlineID->channelHashMax() << ")");
    return StatusCode::FAILURE;
  }


  unsigned nConnected=0;

  for (unsigned i=0;i<nChan;++i) {
    const Identifier id=Identifier(Identifier32(pBlobOnOff[i]));
    if (id.is_valid()) {
      const IdentifierHash oflHash=calocellID->calo_cell_hash(id);
      const HWIdentifier hwid=larOnlineID->channel_Id(IdentifierHash(i));
      ++nConnected;
      onOffMap->m_onlHashToOffline[i]=id;
      onOffMap->m_oflHashToOnline[oflHash]=hwid;
    }
  }//end loop over channels

  ATH_MSG_INFO("Done reading online/offline identifier mapping");
  ATH_MSG_INFO("Found " << nChan << " online identifier and " << nConnected << " offline identifier. "
	       << nChan-nConnected << " disconnected channels.");
  
  if(writeHandle.record(std::move(onOffMap)).isFailure()) {
    ATH_MSG_ERROR("Could not record LArOnOffMapping object with " 
		  << writeHandle.key() 
		  << " with EventRange " << writeHandle.getRange()
		  << " into Conditions Store");
    return StatusCode::FAILURE;
    }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");
 
  return StatusCode::SUCCESS;
}

