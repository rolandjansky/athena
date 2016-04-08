/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.7%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.7%.codegen_version

//## begin module%4457CB630376.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4457CB630376.CLHEP::cm

//## begin module%4457CB630376.cp preserve=no
//## end module%4457CB630376.cp

//## Module: HECDetDescr%4457CB630376; Pseudo Package body
//## Source file: /home/atlas/TEST-12.0.2-00/LArCalorimeter/LArGeoModel/LArReadoutGeometry/src/HECDetDescr.cxx

//## begin module%4457CB630376.additionalIncludes preserve=no
//## end module%4457CB630376.additionalIncludes

//## begin module%4457CB630376.includes preserve=yes
//## end module%4457CB630376.includes

// HECDetDescr
#include "LArReadoutGeometry/HECDetDescr.h"
//## begin module%4457CB630376.additionalDeclarations preserve=yes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
//## end module%4457CB630376.additionalDeclarations


// Class HECDetDescr 

HECDetDescr::HECDetDescr (const HECDetectorManager *detManager, unsigned int sampling, unsigned int region, const CellBinning &phiBinning, bool isTestBeam)
  //## begin HECDetDescr::HECDetDescr%445A11A300C0.hasinit preserve=no
  //## end HECDetDescr::HECDetDescr%445A11A300C0.hasinit
  //## begin HECDetDescr::HECDetDescr%445A11A300C0.initialization preserve=yes
  // We use here cell width to disambiguate inner HEC and Outer HEC.  Inner HEC has wider cells (0.2) than Outer hec (0.1)
  :
                          m_manager(detManager),
			  m_samplingIndex(sampling),
			  m_regionIndex(region),
			  m_phiBinning(phiBinning),
			  m_outerRadSegNumber(region==0 ? 9: 3),
                          m_etaBinning(0,0,1,0),
			  m_numBlocks(sampling==0 ? 1:2),
			  m_firstBlock(sampling==0 ? 0 : 2*sampling-1)
  
  //## end HECDetDescr::HECDetDescr%445A11A300C0.initialization
{
  //## begin HECDetDescr::HECDetDescr%445A11A300C0.body preserve=yes

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
      if(isTestBeam && b==2) pos += (*hadronicEndcap)[0]->getDouble("GAPWHL")*CLHEP::cm;
    }
  }
  //## end HECDetDescr::HECDetDescr%445A11A300C0.body
}


HECDetDescr::~HECDetDescr()
{
  //## begin HECDetDescr::~HECDetDescr%4457CB630376_dest.body preserve=yes
  //## end HECDetDescr::~HECDetDescr%4457CB630376_dest.body
}


// Additional Declarations
  //## begin HECDetDescr%4457CB630376.declarations preserve=yes
  //## end HECDetDescr%4457CB630376.declarations

//## begin module%4457CB630376.epilog preserve=yes
//## end module%4457CB630376.epilog
