/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p1.h"

void TrigInDetTrackFitParCnv_p1 :: persToTrans( const TrigInDetTrackFitPar_p1 *persObj,
                                            TrigInDetTrackFitPar    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigInDetTrackFitParCnv_p1::persToTrans" << endreq;
  

  *transObj = TrigInDetTrackFitPar (persObj->m_a0,
                                    persObj->m_phi0,
                                    persObj->m_z0,
                                    persObj->m_eta,
                                    persObj->m_pT,
                                    persObj->m_ea0,
                                    persObj->m_ephi0,
                                    persObj->m_ez0,
                                    persObj->m_eeta,
                                    persObj->m_epT,
                                    (TrigInDetTrackFitPar::TrigSurfaceType) persObj->m_surfaceType,
                                    persObj->m_surfaceCoordinate,
                                    new std::vector<double> (*persObj->m_cov));
}

void TrigInDetTrackFitParCnv_p1 :: transToPers( const TrigInDetTrackFitPar    *transObj,
                                            TrigInDetTrackFitPar_p1 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigInDetTrackFitParCnv_p1::transToPers" << endreq;
  
  persObj->m_a0                = transObj->a0()    ;
  persObj->m_phi0              = transObj->phi0()  ;
  persObj->m_z0                = transObj->z0()    ;
  persObj->m_eta               = transObj->eta()   ;
  persObj->m_pT                = transObj->pT()    ;
  persObj->m_ea0               = transObj->ea0()   ;
  persObj->m_ephi0             = transObj->ephi0() ;
  persObj->m_ez0               = transObj->ez0()   ;
  persObj->m_eeta              = transObj->eeta()  ;
  persObj->m_epT               = transObj->epT()   ;

  persObj->m_surfaceType       = transObj->surfaceType();
  persObj->m_surfaceCoordinate = transObj->surfaceCoordinate();

  if(transObj->cov()){
    if (!persObj->m_cov){
      persObj->m_cov = &persObj->m_covtmp;
    }
    *persObj->m_cov = *transObj->cov();
  }
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
    m_surfaceCoordinate (p.m_surfaceCoordinate),
    m_covtmp (p.m_covtmp)
{
  if (m_cov == &p.m_covtmp)
    m_cov = &m_covtmp;
}


TrigInDetTrackFitPar_p1& TrigInDetTrackFitPar_p1::operator=(const TrigInDetTrackFitPar_p1& p)
{
  if (this != &p) {
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
    m_surfaceCoordinate = p.m_surfaceCoordinate;
    m_covtmp = p.m_covtmp;
  
    if (m_cov == &p.m_covtmp)
      m_cov = &m_covtmp;
  }
  return *this;
}
