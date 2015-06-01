/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArConditionsTest/LArFebRodMapConvert.h"
          
//#include "LArTools/LArCablingBlob.h"
//#include "LArTools/LArOnOffIdMap.h"
#include "LArIdentifier/LArOnlineID.h"
//#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include <fstream>

LArFebRodMapConvert::LArFebRodMapConvert( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_onlineID(NULL),
  //m_caloCellID(NULL),
  m_cablingSvc("LArCablingService")
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

LArFebRodMapConvert::~LArFebRodMapConvert() 

{}

StatusCode LArFebRodMapConvert::initialize() {

  StatusCode sc = detStore()->retrieve(m_onlineID, "LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  
  /*
  sc = detStore()->retrieve(m_caloCellID, "CaloCell_ID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get CaloCell_ID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  */
  sc=m_cablingSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get LArCablingService!" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArFebRodMapConvert::finalize()
{

  return StatusCode::SUCCESS;
}


StatusCode LArFebRodMapConvert::execute() {  

  const uint32_t onlHashMax=m_onlineID-> febHashMax();

  coral::AttributeListSpecification* spec_febrod = new coral::AttributeListSpecification();
  spec_febrod->extend("FebHashToRODs", "blob");
  spec_febrod->extend<unsigned>("version");
  AthenaAttributeList* al_febrod = new AthenaAttributeList(*spec_febrod);
  (*al_febrod)["version"].setValue(0U);
  coral::Blob& blobFebRod=(*al_febrod)["FebHashToRODs"].data<coral::Blob>();
  blobFebRod.resize(onlHashMax*sizeof(uint32_t));
  uint32_t* pBlobFebRod=static_cast<uint32_t*>(blobFebRod.startingAddress());

  size_t index=0;

  std::ofstream outfile("febrod.txt");

  outfile << "hash id rodid" << std::endl;


  for (uint32_t onlHash=0;onlHash<onlHashMax;++onlHash) {
    const HWIdentifier hwid=m_onlineID->feb_Id(onlHash);
#ifdef LARREADOUTMODULEID_H //Old version
    const uint32_t rodid=m_cablingSvc->getReadoutModuleID(hwid).id();
#else //New version, LArReadoutModuleID replaced my HWIdentifier
    const uint32_t rodid=m_cablingSvc->getReadoutModuleID(hwid).get_identifier32().get_compact();
#endif
    pBlobFebRod[index++]=rodid;
    outfile << onlHash << " 0x" << std:: hex << hwid.get_compact() << " 0x"<< rodid << std::dec << std::endl;
  } 

  
  outfile.close();

  msg(MSG::INFO) << "BlobSize OnOffId:" << index << endreq;


  StatusCode sc=detStore()->record(al_febrod,"/LAR/Identifier/FebRodMap");
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Failed to record LArFebRodMap" << endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}
    
