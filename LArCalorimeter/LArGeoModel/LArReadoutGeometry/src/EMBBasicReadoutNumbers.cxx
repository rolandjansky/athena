/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"


#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h" 
#include "LArReadoutGeometry/EMBBasicReadoutNumbers.h"

EMBBasicReadoutNumbers::EMBBasicReadoutNumbers()
{


  // The EMB gets and managers certain arrays needed to build descriptors.  Here is that:
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IRDBAccessSvc* rdbAccess;
  IGeoModelSvc * geoModel;
  
  if (svcLocator->service ("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error in LArDetectorFactoryH62002, cannot access GeoModelSvc");

  if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in LArDetectorFactoryH62002, cannot access RDBAccessSvc");

  DecodeVersionKey larVersionKey(geoModel, "LAr");

  IRDBRecordset_ptr barrelLongDiv       = rdbAccess->getRecordsetPtr("BarrelLongDiv", larVersionKey.tag(),larVersionKey.node());
  if (barrelLongDiv->size()==0)   throw std::runtime_error("Error getting BarrelLongDiv table");


  IRDBRecordset_ptr barrelGeometry       = rdbAccess->getRecordsetPtr("BarrelGeometry", larVersionKey.tag(),larVersionKey.node());
  if (barrelGeometry->size()==0)   throw std::runtime_error("Error getting BarrelGeometry table");


  IRDBRecordset_ptr presamplerGeometry       = rdbAccess->getRecordsetPtr("PresamplerGeometry", larVersionKey.tag(),larVersionKey.node());
  if (presamplerGeometry->size()==0)   throw std::runtime_error("Error getting PresamplerGeometry table");

 
  IRDBRecordset_ptr embSampSep = rdbAccess->getRecordsetPtr("EMBSampSep", larVersionKey.tag(),larVersionKey.node());
  if (embSampSep->size()==0) {
    embSampSep = rdbAccess->getRecordsetPtr("EMBSampSep", "EMBSampSep-00");
    if (embSampSep->size()==0) {
      throw std::runtime_error("Error getting EMBSampSep table");
    }
  }
 
  m_presamplerRadius = (*presamplerGeometry)[0]->getDouble("RACTIVE")*Gaudi::Units::cm;
  m_rInAc            = (*barrelGeometry)[0]->getDouble("RIN_AC")*Gaudi::Units::cm;
  m_rOutAc           = (*barrelGeometry)[0]->getDouble("ROUT_AC")*Gaudi::Units::cm;
  for (int i=0;i<8;i++) m_EE.push_back((*barrelLongDiv)[0]->getDouble("EE",i));
  for (int i=0;i<8;i++) m_RMX12.push_back((*barrelLongDiv)[0]->getDouble("RMX12",i)*Gaudi::Units::cm);
  for (int i=0;i<53;i++) m_RMX23.push_back((*barrelLongDiv)[0]->getDouble("RMX23",i)*Gaudi::Units::cm);

  int sampSepSize = embSampSep->size();
  m_EMBSamplingSepInnerRMax.resize(sampSepSize,0.0);
  for (int i{0}; i<sampSepSize; ++i) {
    m_EMBSamplingSepInnerRMax.at((*embSampSep)[i]->getInt("IND"))=(*embSampSep)[i]->getDouble("RMAX");
  }
}

EMBBasicReadoutNumbers::~EMBBasicReadoutNumbers()
{
}


