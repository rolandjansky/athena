/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p4.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p4.h"

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p4::persToTrans( const TrigInDetTrack_p4 *persObj, TrigInDetTrack *transObj, MsgStream  &log  )
{

  // log << MSG::DEBUG << "TrigInDetTrackCnv_p4::persToTrans called " << endreq;

  transObj->algorithmId       ((TrigInDetTrack::AlgoId) persObj->m_allIntegers[0]);
  transObj->StrawHits         (persObj->m_allIntegers[1]);
  transObj->Straw             (persObj->m_allIntegers[2]);
  transObj->StrawTime         (persObj->m_allIntegers[3]);
  transObj->TRHits            (persObj->m_allIntegers[4]);
  transObj->NPixelSpacePoints (persObj->m_allIntegers[5]);
  transObj->NSCT_SpacePoints  (persObj->m_allIntegers[6]);
  transObj->HitPattern        (persObj->m_allIntegers[7]);

  transObj->chi2              (persObj->m_chi2)       ;

  transObj->param      (createTransFromPStore( &m_fpCnv, persObj->m_param, log ));
  transObj->endParam   (createTransFromPStore( &m_fpCnv, persObj->m_endParam, log ));
  //transObj->m_rdoList.resize( persObj->m_rdoList.size() ); 
  //std::copy( persObj->m_rdoList.begin(), persObj->m_rdoList.end(), transObj->m_rdoList.begin() ); 

  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize TrigInDetTrackCnv_p4 " << endreq;
    }
  }

  std::vector<Identifier> rdoList;
  rdoList.reserve( persObj->m_rdoList.size() );
  for (unsigned int id : persObj->m_rdoList) {
    if (m_pixId->is_shortened_pixel_id(id)) {
      rdoList.emplace_back (m_pixId->pixel_id_from_shortened(id));
    }
    else {
      rdoList.emplace_back (id);
    }
  }
  transObj->rdoList (std::move(rdoList));

}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p4::transToPers( const TrigInDetTrack *transObj, TrigInDetTrack_p4 *persObj, MsgStream &log )
{
  // log << MSG::DEBUG << "TrigInDetTrackCnv_p4::transToPers called " << endreq;
  persObj->m_allIntegers[0]= transObj->algorithmId()      ;
  persObj->m_allIntegers[1]= transObj->NStrawHits() ;
  persObj->m_allIntegers[2]= transObj->NStraw()     ;
  persObj->m_allIntegers[3]= transObj->NStrawTime() ;
  persObj->m_allIntegers[4]= transObj->NTRHits()    ;  
  persObj->m_allIntegers[5]= transObj->NPixelSpacePoints();
  persObj->m_allIntegers[6]= transObj->NSCT_SpacePoints();
  persObj->m_allIntegers[7]= transObj->HitPattern();

  persObj->m_chi2        = transObj->chi2()       ;

  persObj->m_param       = toPersistent( &m_fpCnv, transObj->param(), log );
  persObj->m_endParam    = toPersistent( &m_fpCnv, transObj->endParam(), log );

  const std::vector<Identifier>& rdoList = transObj->rdoList();
  persObj->m_rdoList.clear();
  persObj->m_rdoList.reserve( rdoList.size() );
  for (const Identifier& id : rdoList)
    persObj->m_rdoList.push_back (id.get_identifier32().get_compact());
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
