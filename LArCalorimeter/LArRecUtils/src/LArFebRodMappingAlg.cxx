//dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFebRodMappingAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include "LArIdentifier/LArOnlineID.h"

#include <set>
#include <algorithm>
#include <iterator>

StatusCode LArFebRodMappingAlg::initialize() {

  ATH_MSG_DEBUG("initializing");

  // Read Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode LArFebRodMappingAlg::execute() {
  ATH_MSG_DEBUG("executing");

  SG::WriteCondHandle<LArFebRodMapping> writeHandle{m_writeKey};
  
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


  const LArOnlineID* onlineID;
  ATH_CHECK(detStore()->retrieve(onlineID,"LArOnlineID"));
  
  std::unique_ptr<LArFebRodMapping> febRodMap=std::make_unique<LArFebRodMapping>(onlineID);


  //The following lines are copied from the callback method of the old-style LArCablingService
  ATH_MSG_DEBUG("Start reading Feb/Rod mapping");

  const coral::Blob& blobFebRod=(*attr)["FebHashToRODs"].data<coral::Blob>();
  const unsigned nFebRod=blobFebRod.size()/sizeof(uint32_t);
  if (nFebRod!=onlineID->febHashMax()) {
    ATH_MSG_ERROR("Unexpected number of FEBs in the FEB/ROD mapping database. Expected " << onlineID->febHashMax() 
		  << " from identifier dictionary, got " << nFebRod << " from mapping database.");
    return StatusCode::FAILURE;
  }

  febRodMap->m_pFebHashtoROD=static_cast<const uint32_t*>(blobFebRod.startingAddress());

  //Create a set of (unique) ROD ids
  std::set<HWIdentifier> allReadoutModuleIds;
  for (unsigned i=0;i<nFebRod;++i) {
    allReadoutModuleIds.insert(HWIdentifier(febRodMap->m_pFebHashtoROD[i]));
  }

  //Copy the set of unique ROD-ids to rod-id vector
  febRodMap->m_readoutModuleIDVec.reserve(allReadoutModuleIds.size());
  std::copy(allReadoutModuleIds.begin(),allReadoutModuleIds.end(),std::back_inserter(febRodMap->m_readoutModuleIDVec));

  msg(MSG::INFO) << "Done reading Feb/Rod mapping. Found " << nFebRod << " Febs and " <<  febRodMap->m_readoutModuleIDVec.size() << " Rods" << endmsg;

  if(writeHandle.record(std::move(febRodMap)).isFailure()) {
    ATH_MSG_ERROR("Could not record LArFebRodMapping object with " 
		  << writeHandle.key() 
		  << " with EventRange " << writeHandle.getRange()
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

 
  return StatusCode::SUCCESS;
}

