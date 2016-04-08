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
  ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
   p_geomodelsvc(0)
{}

ReadAlignTrans::~ReadAlignTrans(void)
{}

StatusCode ReadAlignTrans::initialize()
{
  ATH_MSG_INFO("ReadAlignTrans::initialize() called");

  // get ptr to GeoModelSvc to be able to register callback
  if (StatusCode::SUCCESS!=service("GeoModelSvc",p_geomodelsvc)) {
    ATH_MSG_ERROR("Cannot get ptr to GeoModelSvc");
    return StatusCode::FAILURE;
  }

  // get SiDetectorManagers for checking module positions
  if (StatusCode::SUCCESS!=detStore()->retrieve(p_sidetman[0],"Pixel")) {
    ATH_MSG_ERROR("Cannot get Pixel SiDetectorManager");
    return StatusCode::FAILURE;
  }
  if (StatusCode::SUCCESS!=detStore()->retrieve(p_sidetman[1],"SCT")) {
    ATH_MSG_ERROR("Cannot get SCT SiDetectorManager");
    return StatusCode::FAILURE;
  }

  // register callbacks on IGeoModelSvc::align to be called back after
  // alignment has been recalculated

  if (StatusCode::SUCCESS==detStore()->regFcn(&IGeoModelSvc::align,
	    p_geomodelsvc,&ReadAlignTrans::testCallBack,this)) {
    ATH_MSG_INFO("Registered callback for IGeoModelSvc");
  } else {
    ATH_MSG_ERROR("Register callback failed");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode ReadAlignTrans::execute() {
  ATH_MSG_INFO("In ReadAlignTrans::execute");
  std::vector<Identifier> ev_ident;
  std::vector<Amg::Vector3D > ev_pos;
  if (StatusCode::SUCCESS!=readSiPositions(ev_ident,ev_pos)) {
    ATH_MSG_ERROR("Cannot read silicon detector posiitons from GeoModel");
    return StatusCode::FAILURE;
  }
  // loop over positions, compare event and cache
  std::vector<Amg::Vector3D >::const_iterator iposcache=m_poscache.begin();
  std::vector<Amg::Vector3D >::const_iterator iposev=ev_pos.begin();
  int iseq=0;
  ATH_MSG_INFO("Printing positions of first 10 from cache and event lookup");
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
  ATH_MSG_INFO("In ReadAlignTrans::finalize");
  return StatusCode::SUCCESS;
}

StatusCode ReadAlignTrans::testCallBack( IOVSVC_CALLBACK_ARGS_P( /* I */ ,
								 keys ) ) {
  // print out the keys we were given (for info)
  // could do nothing if key is not /Indet/Align to save time
  std::string keylist;
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end(); ++itr) keylist+=" "+*itr;
  ATH_MSG_INFO("ReadAlignTrans callback invoked for keys:"+keylist);
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
  ATH_MSG_INFO("readSiPositions sees " << nmod << " Si modules");
  return StatusCode::SUCCESS;
}
