/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/HECDetDescr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

HECDetDescr::HECDetDescr (const HECDetectorManager *detManager
			  , unsigned int sampling
			  , unsigned int region
			  , const CellBinning &phiBinning
			  , bool isTestBeam)
  : m_manager(detManager)
  , m_samplingIndex(sampling)
  , m_regionIndex(region)
  , m_phiBinning(phiBinning)
  , m_outerRadSegNumber(region==0 ? 9: 3)
  , m_etaBinning(0,0,1,0)
  , m_numBlocks(sampling==0 ? 1:2)
  , m_firstBlock(sampling==0 ? 0 : 2*sampling-1)
{
  // This will soon be unnecessary (when the wheels are divided!
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IRDBAccessSvc* rdbAccess;
  IGeoModelSvc * geoModel;
  
  if (svcLocator->service ("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error in HECDetDescr, cannot access GeoModelSvc");

  if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in HECDetDescr, cannot access RDBAccessSvc");

  DecodeVersionKey larVersionKey(geoModel, "LAr");
  IRDBRecordset_ptr hadronicEndcap        = rdbAccess->getRecordsetPtr("HadronicEndcap", larVersionKey.tag(), larVersionKey.node());

  if (hadronicEndcap->size()==0)       throw std::runtime_error("Error getting HadronicEndcap table");
  // End of soon-to-be-unnecessary part.

  if(!isTestBeam) {
     m_etaBinning = CellBinning( region==0 ? (sampling<2 ? 1.5: (sampling<3 ? 1.6: 1.7)) : 2.5, 
			      region==0 ? 2.5 : (sampling==0 || sampling==3) ? 3.3 : 3.1, 
			      region==0 ? (sampling<2 ? 10: (sampling<3 ? 9:8)) : (sampling==0 || sampling==3) ? 4:3,
			      region==0 ? (sampling<2 ? 0: (sampling<3 ? 1:2)):0);
  } else {
     m_etaBinning = CellBinning( region==0 ? (sampling<2 ? 2.1: 2.3) : 2.5, 
				      region==0 ? 2.5 : (sampling==0 ) ? 3.3 : 3.1, 
				      region==0 ? (sampling<2 ? 4: 2) : (sampling==0) ? 4:3,
				      region==0 ? (sampling<2 ? 6: 8):0);
  }

  
  unsigned int startBlock = m_samplingIndex <2 ?  0 : 3; 
  if(isTestBeam) startBlock = 0;
  {
    double pos=0.0;
    for (unsigned int b=startBlock;b<m_manager->getNumBlocks();b++) {
      if (b>=m_firstBlock && b< (m_firstBlock+m_numBlocks)) {
	double front = pos;
	double back  = pos+m_manager->getBlock(b)->getDepth();
	m_zMin.push_back(front);
	m_zMax.push_back(back);
      }
      pos += m_manager->getBlock(b)->getDepth();
      if(isTestBeam && b==2) pos += (*hadronicEndcap)[0]->getDouble("GAPWHL")*Gaudi::Units::cm;
    }
  }
}


HECDetDescr::~HECDetDescr()
{
}

