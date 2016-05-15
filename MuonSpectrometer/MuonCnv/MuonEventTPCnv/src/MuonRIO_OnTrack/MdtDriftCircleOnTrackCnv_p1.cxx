/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MdtDriftCircleOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MdtDriftCircleOnTrackCnv_p1.h"

//nclude "TrkEventTPCnv/helpers/CLHEPHelpers.h"
#include <iostream>

void MdtDriftCircleOnTrackCnv_p1::
persToTrans( const Muon::MdtDriftCircleOnTrack_p1 *persObj,
	     Muon::MdtDriftCircleOnTrack *transObj, MsgStream &log ) 
{
   Muon::MuonDriftCircleErrorStrategy errorStrategy(0);
   errorStrategy.setStrategy(Muon::MuonDriftCircleErrorStrategy::UnknownStrategy);
   if (persObj->m_rotCreationParameters&0x10)  errorStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::BroadError, true); 
   if (persObj->m_rotCreationParameters&0x20)  errorStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::ScaledError, true); 
   if (persObj->m_rotCreationParameters&0x40)  errorStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::ErrorAtPredictedPosition, true); 
   if (persObj->m_rotCreationParameters&0x80)  errorStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::PropCorrection, true); 
   if (persObj->m_rotCreationParameters&0x100) errorStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::MagFieldCorrection, true); 
   if (persObj->m_rotCreationParameters&0x200) errorStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::WireSagGeomCorrection, true); 
   if (persObj->m_rotCreationParameters&0x200) errorStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::WireSagTimeCorrection, true); 

   ElementLinkToIDC_MDT_Container rio;
   m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

   *transObj = Muon::MdtDriftCircleOnTrack (rio,
                                            Trk::LocalParameters(),
                                            Amg::MatrixX(),
                                            Identifier(),
                                            nullptr, // detEl,
                                            persObj->m_driftTime,
                                            static_cast<Trk::DriftCircleStatus>( persObj->m_status ),
                                            persObj->m_positionAlongWire,
                                            persObj->m_localAngle,
                                            errorStrategy,
                                            this->createTransFromPStore( &m_surfCnv, persObj->m_saggedSurface, log )
                                            );

   fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
}

void MdtDriftCircleOnTrackCnv_p1::
transToPers( const Muon::MdtDriftCircleOnTrack    *,
	     Muon::MdtDriftCircleOnTrack_p1 *, MsgStream & /**log*/) 
{
  throw std::runtime_error("MdtDriftCircleOnTrackCnv_p1::transToPers is deprecated!");    
}


