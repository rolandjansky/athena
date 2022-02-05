//dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArLATOMEMappingAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeList.h"

#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"


StatusCode LArLATOMEMappingAlg::initialize() {

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


StatusCode LArLATOMEMappingAlg::execute() {
  ATH_MSG_DEBUG("executing");

  SG::WriteCondHandle<LArLATOMEMapping> writeHandle{m_writeKey};
  
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }  

  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
  const CondAttrListCollection * catr{*readHandle};

  if (catr==nullptr) {
    ATH_MSG_ERROR("Failed to retrieve CondAttrListCollection with key " << m_readKey.key());
    return StatusCode::FAILURE;
  }


  writeHandle.addDependency(readHandle);

  std::unique_ptr<LArLATOMEMapping> latomeMap=std::make_unique<LArLATOMEMapping>();

  //Loop over COOL channels:
  CondAttrListCollection::const_iterator chanIt=catr->begin();
  CondAttrListCollection::const_iterator chanIt_e=catr->end();
  for (;chanIt!=chanIt_e;++chanIt) {
    
    const coral::AttributeList& attr = chanIt->second;
    const coral::Blob& blobsourceId=attr["SourceIDs"].data<coral::Blob>();
    unsigned nLatome=blobsourceId.size()/sizeof(uint32_t);
    const coral::Blob& blobonlineId=attr["OnlineIDs"].data<coral::Blob>();
    unsigned nChan=blobonlineId.size()/blobsourceId.size();
    const uint32_t* pblobsourceId=static_cast<const uint32_t*>(blobsourceId.startingAddress());  
    const uint32_t* pblobonlineId=static_cast<const uint32_t*>(blobonlineId.startingAddress());  
  
    if (nChan!=N_LATOME_CHANNELS) {
       ATH_MSG_ERROR("Number of channels read from DB (" << nChan << ") does not match expected "
   		  <<N_LATOME_CHANNELS << ")");
       return StatusCode::FAILURE;
    }

    latomeMap->m_sourceID.resize(nLatome);
    for (unsigned i=0;i<nLatome;++i) {
      const unsigned sid=pblobsourceId[i];
      ATH_MSG_INFO("Latome SID: "<<sid);
      latomeMap->m_sourceID[i]=sid;
      latomeMap->m_onlineID[i].resize(N_LATOME_CHANNELS);
      for (unsigned j=0;j<N_LATOME_CHANNELS ;++j) {
        const HWIdentifier hwid=HWIdentifier(Identifier32(pblobonlineId[i*N_LATOME_CHANNELS + j]));
        if (hwid.is_valid() && hwid != 0) {
          latomeMap->m_onlineID[i][j]=hwid;
        } else {
          latomeMap->m_onlineID[i][j]=latomeMap->m_hwidEmpty;
        }
      }
    }//end loop over LATOME sourceIDs
    ATH_MSG_INFO("Done reading latome mapping");
    ATH_MSG_INFO("Found " << nLatome << " LATOMEs "); 
  } // end over cool channels
  
  if(writeHandle.record(std::move(latomeMap)).isFailure()) {
    ATH_MSG_ERROR("Could not record LArLATOMEMapping object with " 
		  << writeHandle.key() 
		  << " with EventRange " << writeHandle.getRange()
		  << " into Conditions Store");
    return StatusCode::FAILURE;
    }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");
 
  return StatusCode::SUCCESS;
}

