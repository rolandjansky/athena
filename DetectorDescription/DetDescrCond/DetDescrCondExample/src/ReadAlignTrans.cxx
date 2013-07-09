/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReadAlignTrans - simple example of algorithm reading AlignableTransforms
// Richard Hawkings, started 19/1/04

#include <list>
#include "GaudiKernel/ISvcLocator.h"
#include "Identifier/Identifier.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/StoreGateSvc.h"

#include "DetDescrCondExample/ReadAlignTrans.h"

ReadAlignTrans::ReadAlignTrans(const std::string& name, 
  ISvcLocator* pSvcLocator) :Algorithm(name,pSvcLocator),
   m_log(msgSvc(),name),
   p_detstore(0),
   p_geomodelsvc(0)
{}

ReadAlignTrans::~ReadAlignTrans(void)
{}

StatusCode ReadAlignTrans::initialize()
{
  m_log << MSG::INFO << "ReadAlignTrans::initialize() called" << endreq;

  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    m_log << MSG::FATAL << "Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }
  // get ptr to GeoModelSvc to be able to register callback
  if (StatusCode::SUCCESS!=service("GeoModelSvc",p_geomodelsvc)) {
    m_log << MSG::ERROR << "Cannot get ptr to GeoModelSvc" << endreq;
    return StatusCode::FAILURE;
  }

  // get SiDetectorManagers for checking module positions
  if (StatusCode::SUCCESS!=p_detstore->retrieve(p_sidetman[0],"Pixel")) {
    m_log << MSG::ERROR << "Cannot get Pixel SiDetectorManager" << endreq;
    return StatusCode::FAILURE;
  }
  if (StatusCode::SUCCESS!=p_detstore->retrieve(p_sidetman[1],"SCT")) {
    m_log << MSG::ERROR << "Cannot get SCT SiDetectorManager" << endreq;
    return StatusCode::FAILURE;
  }

  // register callbacks on IGeoModelSvc::align to be called back after
  // alignment has been recalculated

  if (StatusCode::SUCCESS==p_detstore->regFcn(&IGeoModelSvc::align,
	    p_geomodelsvc,&ReadAlignTrans::testCallBack,this)) {
    m_log << MSG::INFO << "Registered callback for IGeoModelSvc" << endreq;
  } else {
    m_log << MSG::ERROR << "Register callback failed" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode ReadAlignTrans::execute() {
  m_log << MSG::INFO << "In ReadAlignTrans::execute" << endreq;
  std::vector<Identifier> ev_ident;
  std::vector<Amg::Vector3D > ev_pos;
  if (StatusCode::SUCCESS!=readSiPositions(ev_ident,ev_pos)) {
    m_log << "Cannot read silicon detector posiitons from GeoModel"
	  << endreq;
    return StatusCode::FAILURE;
  }
  // loop over positions, compare event and cache
  std::vector<Amg::Vector3D >::const_iterator iposcache=m_poscache.begin();
  std::vector<Amg::Vector3D >::const_iterator iposev=ev_pos.begin();
  int iseq=0;
  m_log << "Printing positions of first 10 from cache and event lookup"
	<< endreq;
  for (std::vector<Identifier>::const_iterator itr=m_identcache.begin();
       itr!=m_identcache.end();++itr,++iposcache,++iposev,++iseq) {
    // print out the first 10 modules unless positions differ
    bool lprint=(iseq<10);
    if ((iposev->x()!=iposcache->x()) || (iposev->y()!=iposcache->y()) ||
	(iposev->z()!=iposcache->z())) {
      lprint=true;
      std::cout  << MSG::ERROR << "Module positions differ!" << std::endl;
    }
    if (lprint) {
      std::cout << "Module [";
      itr->show();
      std::cout << "] eventpos=(" << 
      	iposev->x() << "," << iposev->y() << "," << iposev->z() << 
      	") cachepos= (" << 
      	iposcache->x() << "," << iposcache->y() << "," << iposcache->z() 
      		<< ")" << std::endl;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode ReadAlignTrans::finalize() {
  m_log << MSG::INFO << "In ReadAlignTrans::finalize" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode ReadAlignTrans::testCallBack( IOVSVC_CALLBACK_ARGS_P( /* I */ ,
								 keys ) ) {
  // print out the keys we were given (for info)
  // could do nothing if key is not /Indet/Align to save time
  m_log << MSG::INFO << "ReadAlignTrans callback invoked for keys:";
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end(); ++itr) {
    m_log << *itr << " ";
  }
  m_log << endreq;
  return readSiPositions(m_identcache,m_poscache);
}

StatusCode ReadAlignTrans::readSiPositions(std::vector<Identifier>& idvec,
					   std::vector<Amg::Vector3D >& posvec) {
  // find the identifier and position of all the pixel and SCT modules
  int nmod=0;
  idvec.clear();
  posvec.clear();
  for (int idet=0;idet<2;++idet) {
    if (p_sidetman[idet]==0) return StatusCode::FAILURE;
    for (InDetDD::SiDetectorElementCollection::const_iterator iter=
	 p_sidetman[idet]->getDetectorElementBegin();
         iter!=p_sidetman[idet]->getDetectorElementEnd();++iter) {
      idvec.push_back((*iter)->identify());
      posvec.push_back((*iter)->center());
      ++nmod;
    }
  }
  m_log << "readSiPositions sees " << nmod << " Si modules" << endreq;
  return StatusCode::SUCCESS;
}
