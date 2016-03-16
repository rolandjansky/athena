/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PixelClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrackCnv_p1.h"
#include "AthenaKernel/errorcheck.h"


void PixelClusterOnTrackCnv_p1::
persToTrans( const InDet::PixelClusterOnTrack_p1 *persObj,
	     InDet::PixelClusterOnTrack *transObj, MsgStream &log )
{
   if(!m_isInitialized) {
     if (this->initialize(log) != StatusCode::SUCCESS) {
       log << MSG::FATAL << "Could not initialize PixelClusterOnTrackCnv_p1 " << endreq;
     }
   }

    Trk::LocalParameters localParams;
    Amg::MatrixX localCovariance;
    localCovariance.setZero();

    ElementLinkToIDCPixelClusterContainer rio;
   m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  
    *transObj = InDet::PixelClusterOnTrack(rio,
                                           localParams,
                                           localCovariance,
                                           0, //idDE
                                           Identifier(),
                                           persObj->m_energyLoss,
                                           persObj->m_isFake,
                                           persObj->m_hasClusterAmbiguity,
                                           false
                                           );

   fillTransFromPStore( &m_scCnv, persObj->m_RIO,  transObj, log );
}


void PixelClusterOnTrackCnv_p1::
transToPers( const InDet::PixelClusterOnTrack *transObj,
	     InDet::PixelClusterOnTrack_p1 *persObj, MsgStream &log ) 
{
   persObj->m_RIO = baseToPersistent( &m_scCnv,  transObj, log );
//    persObj->m_idDE = transObj->m_idDE;
   
   persObj->m_hasClusterAmbiguity = transObj->hasClusterAmbiguity();
   persObj->m_isFake              = transObj->isFake();
   persObj->m_energyLoss          = transObj->energyLoss();
//   m_elementLinkConverter.resetForCnv(persObj->m_elementLinkToIDCPixelClusterContainerNames);
//   m_elementLinkConverter.transToPers(&transObj->m_rio,&persObj->m_elementLinkToIDCPixelClusterContainer,log);
   // m_elCnv.transToPers(&transObj->m_rio,&persObj->m_prdLink,log);  
   persObj->m_prdLink.m_contName          = transObj->prepRawDataLink().dataID();// New suggestion from RD - above crashes
   persObj->m_prdLink.m_elementIndex     = transObj->prepRawDataLink().index();
}

StatusCode PixelClusterOnTrackCnv_p1::initialize(MsgStream &/*log*/) {
   // Do not initialize again:
   m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
   ISvcLocator* svcLocator = Gaudi::svcLocator();
   StoreGateSvc *detStore;
   CHECK( svcLocator->service("DetectorStore", detStore) );
   CHECK( detStore->retrieve(m_pixId, "PixelID") );

   return StatusCode::SUCCESS;
}

