/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void RIO_OnTrackCnv_p2::persToTrans( const Trk::RIO_OnTrack_p2 *persObj, Trk::RIO_OnTrack *transObj, MsgStream &log )
{
//   static int count = 0;
//   if( count++ < 10 ) {
//      log << MSG::INFO << ">>>>---------  RIO_OnTrack ID = " << persObj->m_id << endmsg;
//   }
   
   transObj->m_identifier = persObj->m_id;
   fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &transObj->m_localParams, log );
   
   Trk::ErrorMatrix dummy;
   fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
   EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "RIO_OnTrackCnv_p2");
   
   // Attempt to call supertool to fill in detElements
   m_eventCnvTool->recreateRIO_OnTrack(transObj);
//   std::cout<<"RIO_OnTrackCnv_p2::persToTrans after  : detEl="<<transObj->detectorElement()<<std::endl;
   if (transObj->detectorElement()==nullptr) 
      log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
          << "probably because of a problem with the Identifier/IdentifierHash : ("
          << transObj->identify()<<"/"<<transObj->idDE()<<endmsg;
}


void RIO_OnTrackCnv_p2::transToPers( const Trk::RIO_OnTrack * /**transObj*/, Trk::RIO_OnTrack_p2 * /**persObj*/, MsgStream & /**log*/ )
{
//    // Set ELs
//    m_eventCnvTool->prepareRIO_OnTrack(const_cast<Trk::RIO_OnTrack *>(transObj));
// //   persObj->m_id = transObj->m_identifier;
//    persObj->m_id = transObj->m_identifier.get_compact();
//    persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->m_localParams, log );
//    persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &transObj->m_localErrMat, log );
   
   throw std::runtime_error("RIO_OnTrackCnv_p2::transToPers is deprecated!");
   // Take this opportunity to flatten all ROTs i.e. never use this top level convertor
}
