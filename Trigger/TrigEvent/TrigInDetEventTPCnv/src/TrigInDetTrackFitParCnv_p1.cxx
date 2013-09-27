/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#undef private

#include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p1.h"

void TrigInDetTrackFitParCnv_p1 :: persToTrans( const TrigInDetTrackFitPar_p1 *persObj,
                                            TrigInDetTrackFitPar    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigInDetTrackFitParCnv_p1::persToTrans" << endreq;
  
  transObj->m_a0                = persObj->m_a0    ;
  transObj->m_phi0              = persObj->m_phi0  ;
  transObj->m_z0                = persObj->m_z0    ;
  transObj->m_eta               = persObj->m_eta   ;
  transObj->m_pT                = persObj->m_pT    ;
  transObj->m_ea0               = persObj->m_ea0   ;
  transObj->m_ephi0             = persObj->m_ephi0 ;
  transObj->m_ez0               = persObj->m_ez0   ;
  transObj->m_eeta              = persObj->m_eeta  ;
  transObj->m_epT               = persObj->m_epT   ;

  if(persObj->m_cov){
    if (!transObj->m_cov)
      transObj->m_cov = new std::vector<double> (*persObj->m_cov);
    else
      *const_cast<std::vector<double>*>(transObj->m_cov) = *persObj->m_cov;
  }

  transObj->m_surfaceType       = persObj->m_surfaceType;
  transObj->m_surfaceCoordinate = persObj->m_surfaceCoordinate;
}

void TrigInDetTrackFitParCnv_p1 :: transToPers( const TrigInDetTrackFitPar    *transObj,
                                            TrigInDetTrackFitPar_p1 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigInDetTrackFitParCnv_p1::transToPers" << endreq;
  
  persObj->m_a0                = transObj->m_a0    ;
  persObj->m_phi0              = transObj->m_phi0  ;
  persObj->m_z0                = transObj->m_z0    ;
  persObj->m_eta               = transObj->m_eta   ;
  persObj->m_pT                = transObj->m_pT    ;
  persObj->m_ea0               = transObj->m_ea0   ;
  persObj->m_ephi0             = transObj->m_ephi0 ;
  persObj->m_ez0               = transObj->m_ez0   ;
  persObj->m_eeta              = transObj->m_eeta  ;
  persObj->m_epT               = transObj->m_epT   ;

  if(transObj->m_cov){
    if (!persObj->m_cov){
      persObj->m_cov = &persObj->m_covtmp;
    }
    *persObj->m_cov = *transObj->m_cov;
  }

  persObj->m_surfaceType       = transObj->m_surfaceType;
  persObj->m_surfaceCoordinate = transObj->m_surfaceCoordinate;
}



TrigInDetTrackFitPar_p1::TrigInDetTrackFitPar_p1(const TrigInDetTrackFitPar_p1& p)
  : m_a0 (p.m_a0),
    m_phi0 (p.m_phi0),
    m_z0 (p.m_z0),
    m_eta (p.m_eta),
    m_pT (p.m_pT),
    m_ea0 (p.m_ea0),
    m_ephi0 (p.m_ephi0),
    m_ez0 (p.m_ez0),
    m_eeta (p.m_eeta),
    m_epT (p.m_epT),
    m_cov (p.m_cov),
    m_surfaceType (p.m_surfaceType),
    m_covtmp (p.m_covtmp)
{
  if (m_cov == &p.m_covtmp)
    m_cov = &m_covtmp;
}


TrigInDetTrackFitPar_p1& TrigInDetTrackFitPar_p1::operator=(const TrigInDetTrackFitPar_p1& p)
{
  m_a0 = p.m_a0;
  m_phi0 = p.m_phi0;
  m_z0 = p.m_z0;
  m_eta = p.m_eta;
  m_pT = p.m_pT;
  m_ea0 = p.m_ea0;
  m_ephi0 = p.m_ephi0;
  m_ez0 = p.m_ez0;
  m_eeta = p.m_eeta;
  m_epT = p.m_epT;
  m_cov = p.m_cov;
  m_surfaceType = p.m_surfaceType;
  m_covtmp = p.m_covtmp;
  
  if (m_cov == &p.m_covtmp)
    m_cov = &m_covtmp;
  return *this;
}
