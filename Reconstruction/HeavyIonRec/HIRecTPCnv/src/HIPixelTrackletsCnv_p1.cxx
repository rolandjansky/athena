/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "HIGlobal/HIPixelTracklets.h"
#include "CLHEP/Geometry/Point3D.h"
#undef private
#undef protected

#include "HIRecTPCnv/HIPixelTrackletsCnv_p1.h"

void HIPixelTrackletsCnv_p1::persToTrans(const HIPixelTracklets_p1 *persObj, HIPixelTracklets *transObj, MsgStream &log) {
  transObj->m_eta = persObj->m_eta;
  transObj->m_phi = persObj->m_phi;
  transObj->m_pt = persObj->m_pt;
  transObj->m_etaS = persObj->m_etaS;
  transObj->m_phiS = persObj->m_phiS;
  HepGeom::Point3D<double>* ptmp =  createTransFromPStore(&m_hepPoint3DCnv,persObj->m_vrt, log);
  //transObj->m_vrt = *ptmp;
  delete ptmp;
  std::vector<PixelCluster*>* vtmp = m_clusterVectorCnv.createTransient(&persObj->m_clusters, log );
  //transObj->m_clusters.swap(*vtmp);
  delete vtmp;
}

void HIPixelTrackletsCnv_p1::transToPers(const HIPixelTracklets *transObj, HIPixelTracklets_p1 *persObj, MsgStream &log) {
  log << MSG::INFO << "HIPixelTrackletsCnv_p1:transToPers:starting." << endreq;
  persObj->m_eta = transObj->m_eta;
  log << MSG::INFO << "eta value finished." << endreq;
  persObj->m_phi = transObj->m_phi;
  persObj->m_pt = transObj->m_pt;
  persObj->m_etaS = transObj->m_etaS;
  persObj->m_phiS = transObj->m_phiS;
  log << MSG::INFO << "phiS value finished." << endreq;

  
  //HepGeom::Point3D<double> pointx(transObj->m_vrt.x(),transObj->m_vrt.y(),transObj->m_vrt.z());
  //persObj->m_vrt = HepGeom::Point3D<double>(transObj->m_vrt.x(),transObj->m_vrt.y(),transObj->m_vrt.z());
  //m_hepPoint3DCnv->transToPers(&(transObj->m_vrt),&(persObj->m_vrt),log );
  //persObj->m_vrt = toPersistent(&m_hepPoint3DCnv,&(transObj->m_vrt),log);
  //log << MSG::INFO << "vrt: " << persObj->m_vrt.x() << endreq;
//   log << MSG::INFO << "vertex value finished." << endreq;
//   //log << MSG::INFO << "energy Loss: " << (transObj->m_clusters)[0]->energyLoss() << endreq;
//   //log << MSG::INFO << "m_clusterVectorCnv: " << m_clusterVectorCnv << endreq;
//   m_clusterVectorCnv.transToPers( &(transObj->m_clusters), &(persObj->m_clusters), log);  
//   log << MSG::INFO << "HIPixelTrackletsCnv_p1:transToPers:ending." << endreq;
}
