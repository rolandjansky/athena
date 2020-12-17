/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ReadAlignTrans - simple example of algorithm reading AlignableTransforms
// Richard Hawkings, started 19/1/04

#include "ReadAlignTrans.h"
#include "GaudiKernel/ISvcLocator.h"
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include <list>

ReadAlignTrans::ReadAlignTrans(const std::string& name
			       , ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name,pSvcLocator)
  , m_sidetman()
{}

ReadAlignTrans::~ReadAlignTrans(void)
{}

StatusCode ReadAlignTrans::initialize()
{
  ATH_MSG_INFO("ReadAlignTrans::initialize() called");

  // get SiDetectorManagers for checking module positions
  ATH_CHECK(detStore()->retrieve(m_sidetman[0],"Pixel"));
  ATH_CHECK(detStore()->retrieve(m_sidetman[1],"SCT"));

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

StatusCode ReadAlignTrans::readSiPositions(std::vector<Identifier>& idvec,
					   std::vector<Amg::Vector3D >& posvec) {
  // find the identifier and position of all the pixel and SCT modules
  int nmod=0;
  idvec.clear();
  posvec.clear();
  for (int idet=0;idet<2;++idet) {
    if (m_sidetman[idet]==0) return StatusCode::FAILURE;
    for (InDetDD::SiDetectorElementCollection::const_iterator iter=
	 m_sidetman[idet]->getDetectorElementBegin();
         iter!=m_sidetman[idet]->getDetectorElementEnd();++iter) {
      idvec.push_back((*iter)->identify());
      posvec.push_back((*iter)->center());
      ++nmod;
    }
  }
  ATH_MSG_INFO("readSiPositions sees " << nmod << " Si modules");
  return StatusCode::SUCCESS;
}
