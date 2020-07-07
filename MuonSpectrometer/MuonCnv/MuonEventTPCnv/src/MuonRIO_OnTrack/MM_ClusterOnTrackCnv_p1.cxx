/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MM_ClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MM_ClusterOnTrackCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void MM_ClusterOnTrackCnv_p1::
persToTrans( const Muon::MM_ClusterOnTrack_p1 *persObj,
	     Muon::MMClusterOnTrack *transObj, MsgStream &log )
{
   ElementLinkToIDC_MM_Container rio;
   m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

   Trk::LocalParameters localParams;
   fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &localParams, log );

   Trk::ErrorMatrix dummy;
   Amg::MatrixX localCovariance;
   fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
   EigenHelpers::vectorToEigenMatrix(dummy.values, localCovariance, "RIO_OnTrackCnv_p2");

   std::vector<Amg::MatrixX> stripDriftDistErrors;
   stripDriftDistErrors.reserve(persObj->m_stripDriftDistErrors_0_0.size());
   for(uint i_strip = 0; i_strip < persObj->m_stripDriftDistErrors_0_0.size(); i_strip++) {
     Amg::MatrixX tmp(2, 2);
     tmp(0, 0) = persObj->m_stripDriftDistErrors_0_0.at(i_strip);
     tmp(1, 1) = persObj->m_stripDriftDistErrors_1_1.at(i_strip);
     stripDriftDistErrors.push_back(tmp);
   }


   *transObj = Muon::MMClusterOnTrack (rio,
                                       localParams,
                                       localCovariance,
                                       Identifier (persObj->m_id),
                                       nullptr,
                                       persObj->m_positionAlongStrip,
                                       persObj->m_stripDriftDists,
                                       stripDriftDistErrors
                                       );

   // Attempt to call supertool to fill in detElements
   m_eventCnvTool->recreateRIO_OnTrack(transObj);
   if (transObj->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << transObj->identify()<<"/"<<transObj->idDE()<<endmsg;   
}


void MM_ClusterOnTrackCnv_p1::
transToPers( const Muon::MMClusterOnTrack *transObj,
	     Muon::MM_ClusterOnTrack_p1 *persObj, MsgStream &log ) 
{
  // Prepare ELs
  // std::cout<<"BLAH! MM_ClusterOnTrackCnv_p1::transToPers"<<std::endl;
  
   Trk::IEventCnvSuperTool::ELKey_t key;
   Trk::IEventCnvSuperTool::ELIndex_t index;
   m_eventCnvTool->prepareRIO_OnTrackLink(transObj, key, index);
   ElementLinkToIDC_MM_Container eltmp (key, index);
  
   m_elCnv.transToPers(&eltmp, &persObj->m_prdLink,log);
   persObj->m_positionAlongStrip = transObj->positionAlongStrip();
   persObj->m_stripDriftDists = transObj->stripDriftDists();
   
   persObj->m_stripDriftDistErrors_0_0.reserve(transObj->stripDriftDistErrors().size());
   persObj->m_stripDriftDistErrors_1_1.reserve(transObj->stripDriftDistErrors().size());

   for(uint i_strip = 0; i_strip < transObj->stripDriftDistErrors().size(); i_strip++){
     persObj->m_stripDriftDistErrors_0_0.push_back(transObj->stripDriftDistErrors().at(i_strip)(0,0));
     persObj->m_stripDriftDistErrors_1_1.push_back(transObj->stripDriftDistErrors().at(i_strip)(1,1));
   }

   persObj->m_id = transObj->identify().get_identifier32().get_compact();
   persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->localParameters(), log );
   Trk::ErrorMatrix pMat;
   EigenHelpers::eigenMatrixToVector(pMat.values, transObj->localCovariance(), "CscClusterOnTrackCnv_p2");
   persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );

}


