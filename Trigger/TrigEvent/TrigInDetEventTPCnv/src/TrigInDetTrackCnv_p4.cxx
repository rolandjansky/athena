/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigInDetTrack.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p4.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p4.h"

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p4::persToTrans( const TrigInDetTrack_p4 *persObj, TrigInDetTrack *transObj, MsgStream  &log  )
{

  // log << MSG::DEBUG << "TrigInDetTrackCnv_p4::persToTrans called " << endreq;

  transObj->m_algId             = (TrigInDetTrack::AlgoId) persObj->m_allIntegers[0];
  transObj->m_NStrawHits        = persObj->m_allIntegers[1];
  transObj->m_NStraw            = persObj->m_allIntegers[2];
  transObj->m_NStrawTime        = persObj->m_allIntegers[3];
  transObj->m_NTRHits           = persObj->m_allIntegers[4];
  transObj->m_NPixelSpacePoints = persObj->m_allIntegers[5];
  transObj->m_NSCT_SpacePoints  = persObj->m_allIntegers[6];
  transObj->m_HitPattern        = persObj->m_allIntegers[7];

  transObj->m_chi2        = persObj->m_chi2;
  transObj->m_param       = createTransFromPStore( &m_fpCnv, persObj->m_param, log );
  transObj->m_endParam    = createTransFromPStore( &m_fpCnv, persObj->m_endParam, log );
  //transObj->m_rdoList.resize( persObj->m_rdoList.size() ); 
  //std::copy( persObj->m_rdoList.begin(), persObj->m_rdoList.end(), transObj->m_rdoList.begin() ); 

  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize TrigInDetTrackCnv_p4 " << endreq;
    }
  }

  transObj->m_rdoList.resize( persObj->m_rdoList.size() );
  unsigned int transIndex = 0; 
  for (std::vector<unsigned int>::const_iterator it=persObj->m_rdoList.begin(); it!=persObj->m_rdoList.end() ; ++it, ++transIndex) {
    if (m_pixId->is_shortened_pixel_id(*it)) {
     transObj->m_rdoList[transIndex]= m_pixId->pixel_id_from_shortened(*it);
    }
    else {
      transObj->m_rdoList[transIndex] = *it;
    }
  }


}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p4::transToPers( const TrigInDetTrack *transObj, TrigInDetTrack_p4 *persObj, MsgStream &log )
{

  // log << MSG::DEBUG << "TrigInDetTrackCnv_p4::transToPers called " << endreq;
  persObj->m_allIntegers[0]= transObj->m_algId      ;
  persObj->m_allIntegers[1]= transObj->m_NStrawHits ;
  persObj->m_allIntegers[2]= transObj->m_NStraw     ;
  persObj->m_allIntegers[3]= transObj->m_NStrawTime ;
  persObj->m_allIntegers[4]= transObj->m_NTRHits    ;  
  persObj->m_allIntegers[5]= transObj->m_NPixelSpacePoints;
  persObj->m_allIntegers[6]= transObj->m_NSCT_SpacePoints;
  persObj->m_allIntegers[7]= transObj->m_HitPattern;


  persObj->m_chi2        = transObj->m_chi2       ;
  persObj->m_param       = toPersistent( &m_fpCnv, transObj->m_param, log );
  persObj->m_endParam    = toPersistent( &m_fpCnv, transObj->m_endParam, log );

  if( transObj->m_rdoList.size()!=0)
    {
      persObj->m_rdoList.resize( transObj->m_rdoList.size() );

      unsigned int i=0; 
      for( std::vector<Identifier>::const_iterator it = transObj->m_rdoList.begin(); 
	   it != transObj->m_rdoList.end();++it,++i) 
	persObj->m_rdoList[i] =it->get_compact(); 
    }
}


StatusCode TrigInDetTrackCnv_p4::initialize(MsgStream &log) {
   // Do not initialize again:
   m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
   ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
   StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found !" << endreq;
      return StatusCode::FAILURE;
   }

   // get DetectorStore service
   StoreGateSvc *detStore;
   sc = svcLocator->service("DetectorStore", detStore);
   if (sc.isFailure()) {
      log << MSG::FATAL << "DetectorStore service not found !" << endreq;
      return StatusCode::FAILURE;
   } 
   //   else {
   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endreq;
   //   }

   // Get the sct helper from the detector store
   sc = detStore->retrieve(m_pixId, "PixelID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get PixelID helper !" << endreq;
      return StatusCode::FAILURE;
   } 


   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endreq;
   return StatusCode::SUCCESS;
}


void TrigInDetTrackCnv_p4::setPixelID (const PixelID* pixId)
{
  m_pixId = pixId;
  m_isInitialized = true;
}
