/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MdtDriftCircleOnTrackCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonRIO_OnTrack/MdtDriftCircleOnTrackCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

//nclude "TrkEventTPCnv/helpers/CLHEPHelpers.h"
#include <iostream>
using namespace std;

void MdtDriftCircleOnTrackCnv_p2::
persToTrans( const Muon::MdtDriftCircleOnTrack_p2 *persObj,
	     Muon::MdtDriftCircleOnTrack *transObj, MsgStream &log ) 
{

  transObj->m_identifier = persObj->m_id;
  fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &transObj->m_localParams, log );
  // fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &transObj->m_localErrMat, log );
  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "MdtDriftCircleOnTrackCnv_p2");
  
  transObj->m_status    = static_cast<Trk::DriftCircleStatus>( persObj->m_status );
  // std::cout<<"About to call m_elCnv.persToTrans"<<std::endl;

  m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  
  // if (transObj->m_rio.isValid()!=true) {
  //     std::cout<<"ERROR! m_rio still not valid."<<std::endl;
  // }else{
  //     std::cout<<"ACE! m_rio valid and links to :"<<*( transObj->prepRawData() )<<std::endl;
  // }

  transObj->m_localAngle           = persObj->m_localAngle ;
  transObj->m_positionAlongWire    = persObj->m_positionAlongWire ;
  transObj->m_driftTime            = persObj->m_driftTime ;
  transObj->m_errorStrategy.m_bits = persObj->m_muonDriftCircleErrorStrategy;
  
  // Attempt to call supertool to fill in detElements
   
  m_eventCnvTool->recreateRIO_OnTrack(transObj);
  if (transObj->detectorElement()==0) 
       log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
           << "probably because of a problem with the Identifier/IdentifierHash : ("
           << transObj->identify()<<"/"<<transObj->idDE()<<endreq;
}

void MdtDriftCircleOnTrackCnv_p2::
transToPers( const Muon::MdtDriftCircleOnTrack    *transObj,
	     Muon::MdtDriftCircleOnTrack_p2 *persObj, MsgStream &log) 
{
  // Prepare ELs
  m_eventCnvTool->prepareRIO_OnTrack(const_cast<Muon::MdtDriftCircleOnTrack *>(transObj));  
  m_elCnv.transToPers(&transObj->m_rio,&persObj->m_prdLink,log);  
  
  persObj->m_id = transObj->m_identifier.get_identifier32().get_compact();
  persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->m_localParams, log );
  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_localCovariance, "MdtDriftCircleOnTrackCnv_p2");
  persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );
  

  persObj->m_status    = static_cast<unsigned int>( transObj->m_status );
  persObj->m_localAngle            = transObj->m_localAngle ;
  persObj->m_positionAlongWire     = transObj->m_positionAlongWire ;
  persObj->m_driftTime             = transObj->m_driftTime ;
  persObj->m_muonDriftCircleErrorStrategy = transObj->m_errorStrategy.m_bits.to_ulong();
}

