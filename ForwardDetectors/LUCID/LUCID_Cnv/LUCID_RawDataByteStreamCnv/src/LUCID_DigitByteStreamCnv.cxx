/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "LUCID_RawDataByteStreamCnv/LUCID_DigitByteStreamCnv.h"

LUCID_DigitByteStreamCnv::LUCID_DigitByteStreamCnv(ISvcLocator* svcloc) : 
  Converter(storageType(), classID(), svcloc),
  m_RodBlockVersion      (0),
  m_BCs_per_LVL1ID       (1)
{
  m_ByteStreamEventAccess = 0;
  m_StoreGate             = 0;
}

StatusCode LUCID_DigitByteStreamCnv::initialize() {

  StatusCode sc = Converter::initialize(); 
  
  if (StatusCode::SUCCESS != sc) return sc; 
  
  if (StatusCode::SUCCESS != service("ByteStreamCnvSvc", m_ByteStreamEventAccess) || !m_ByteStreamEventAccess) {
    
    if (msgLevel(MSG::FATAL)) msg(MSG::FATAL) << "Can't get ByteStreamEventAccess interface" << endmsg;
    
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS; 
}

const CLID& LUCID_DigitByteStreamCnv::classID() {

  return ClassID_traits<LUCID_DigitContainer>::ID();
}

long LUCID_DigitByteStreamCnv::storageType() {

  return ByteStreamAddress::storageType();
}

StatusCode LUCID_DigitByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  
  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " LUCID_DigitByteStreamCnv::createRep" << endmsg;

   RawEventWrite*        re = m_ByteStreamEventAccess->getRawEvent();
   LUCID_DigitContainer* RDO_container = 0;

// dynmanic cast of the pObj to RDO_container based in clid of pObj
   StoreGateSvc::fromStorable(pObj, RDO_container);

   if (!RDO_container) {

     if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Can not cast to LUCID_DigitContainer" << endmsg;
     
     return StatusCode::RECOVERABLE;
   }

// get name of the persistent object (pObj)
   std::string nm = pObj->registry()->name();

// and create a empty generic BytestreamAddress  for the specific clid of pObj
   ByteStreamAddress* addr = new ByteStreamAddress(classID(), nm, "");

   pAddr = addr;

   StatusCode sc = fillFEA(RDO_container, re);
   
   if (sc.isFailure()){

     if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << " Could not convert RawData with to ByteStream " << endmsg;
     
     return StatusCode::RECOVERABLE;
   }

  return StatusCode::SUCCESS;
}

StatusCode LUCID_DigitByteStreamCnv::fillFEA(LUCID_DigitContainer* RDO_container, RawEventWrite* re){

  m_fea.clear(); 
  // type of RODDATA is std::vector<uint32_t> 
  FullEventAssembler<SrcIdMap>::RODDATA* theROD;

  m_fea.setRodMinorVersion(RodBlockVersion());
  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Setting ROD Minor Version Number to: " << m_RodBlockVersion << endmsg;

  LucidRodEncoder_map RDOEncoder_map;

  LUCID_DigitContainer::const_iterator it_cont     = RDO_container->begin(); 
  LUCID_DigitContainer::const_iterator it_cont_end = RDO_container->end();

  for( ; it_cont != it_cont_end; ++it_cont) {
    
    if ((*it_cont) != NULL) {

      uint32_t rodId = getSourceID();
      
      RDOEncoder_map[rodId].addDigit((*it_cont));
    } 
    else if (msgLevel(MSG::WARNING)) msg(MSG::WARNING) << " Digit is empty, skipping digit." << endmsg;
  }

  LucidRodEncoder_map::iterator it_map     = RDOEncoder_map.begin();
  LucidRodEncoder_map::iterator it_map_end = RDOEncoder_map.end();
  
  for (; it_map != it_map_end; ++it_map) { 

    // (*it_map) is now a pointer of the type std::pair<const uint32_t, LUCID_RDOEncoder >
    
    theROD = m_fea.getRodData((*it_map).first);
    
    ((*it_map).second).encode(*theROD);

    (*theROD).push_back(0); // add status word
  
    LUCID_RawData* lrd = new LUCID_RawData(*theROD);
    
    lrd->encodeLumatMapping();
    
    *theROD = lrd->getDataBlock();
  }
  
  m_fea.fill(re, msg(MSG::INFO)); 
  
  return StatusCode::SUCCESS;
}
