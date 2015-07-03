/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MdtDriftCircleOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonRIO_OnTrack/MdtDriftCircleOnTrackCnv_p1.h"

//nclude "TrkEventTPCnv/helpers/CLHEPHelpers.h"
#include <iostream>
using namespace std;

void MdtDriftCircleOnTrackCnv_p1::
persToTrans( const Muon::MdtDriftCircleOnTrack_p1 *persObj,
	     Muon::MdtDriftCircleOnTrack *transObj, MsgStream &log ) 
{
   fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
   transObj->m_status    = static_cast<Trk::DriftCircleStatus>( persObj->m_status );
   transObj->m_saggedSurface = this->createTransFromPStore( &m_surfCnv, persObj->m_saggedSurface, log );
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
   
   //Fixme - can we do better here?
   using namespace Muon;
   transObj->m_errorStrategy.setStrategy(MuonDriftCircleErrorStrategy::UnknownStrategy);
   if (persObj->m_rotCreationParameters&0x10)  transObj->m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::BroadError, true); 
   if (persObj->m_rotCreationParameters&0x20)  transObj->m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::ScaledError, true); 
   if (persObj->m_rotCreationParameters&0x40)  transObj->m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::ErrorAtPredictedPosition, true); 
   if (persObj->m_rotCreationParameters&0x80)  transObj->m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::PropCorrection, true); 
   if (persObj->m_rotCreationParameters&0x100) transObj->m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::MagFieldCorrection, true); 
   if (persObj->m_rotCreationParameters&0x200) transObj->m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::WireSagGeomCorrection, true); 
   if (persObj->m_rotCreationParameters&0x200) transObj->m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::WireSagTimeCorrection, true); 
}

void MdtDriftCircleOnTrackCnv_p1::
transToPers( const Muon::MdtDriftCircleOnTrack    *,
	     Muon::MdtDriftCircleOnTrack_p1 *, MsgStream & /**log*/) 
{
  throw std::runtime_error("MdtDriftCircleOnTrackCnv_p1::transToPers is deprecated!");    
}


