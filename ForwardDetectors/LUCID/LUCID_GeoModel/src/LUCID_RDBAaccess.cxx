/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_RDBAaccess.h"

#include "GeoModelKernel/Units.h"

#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "AthenaKernel/getMessageSvc.h"

#include "GaudiKernel/ISvcLocator.h"

LUCID_RDBAccess::LUCID_RDBAccess() {

  ReadDB();
  SetParameters();
  Print();
  CloseDB();
}

LUCID_RDBAccess::~LUCID_RDBAccess() {}

void LUCID_RDBAccess::ReadDB() {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_GeoModel::LUCID_RDBAaccess");

  log << MSG::DEBUG << " Starting LUCID_GeoModel::LUCID_RDBAaccess " << endmsg;
  log << MSG::DEBUG << " LUCID_RDBAaccess::ReadDB "<< endmsg;  

  m_svcLocator = Gaudi::svcLocator();
  IRDBAccessSvc* iAccessSvc = NULL;
  
  StatusCode result = m_svcLocator->service("RDBAccessSvc", iAccessSvc);

  if (result.isFailure() || iAccessSvc == NULL) {

    log << MSG::FATAL << " Could not initialize RDBAccessSvc! " << endmsg;
    
    throw GaudiException(" Could not initalize RDBAccessSvc ", "LUCID_GeoModel", StatusCode::FAILURE);
  }

  DecodeVersionKey atlasVersion("ATLAS");
  std::string AtlasVersion = atlasVersion.tag();
  
  m_lucidParams = iAccessSvc->getRecordsetPtr("LucidParams", AtlasVersion, "ATLAS");
  
  if (!m_lucidParams->size()) std::cerr << " ERROR: Unable to retrieve LucidParams data "<< std::endl;
  
  log << MSG::INFO << " LUCID data corresponding to " << AtlasVersion << " fetched " << endmsg;
}

void LUCID_RDBAccess::SetParameters() {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_GeoModel::LUCID_RDBAaccess");
  
  log << MSG::DEBUG << " LUCID_RDBAaccess::SetParameters "<< endmsg;

  log << MSG::DEBUG << " Starting LUCID_GeoModel::LUCID_RDBAaccess " << endmsg;
    
  IRDBRecordset::const_iterator AccessSvc_iter;
  
  for(AccessSvc_iter = m_lucidParams->begin(); AccessSvc_iter != m_lucidParams->end(); AccessSvc_iter++) {
    
    distanceToIP                  = (*AccessSvc_iter)->getDouble("DISTANCETOIP")*Gaudi::Units::mm;
    VJdistanceToIP                = (*AccessSvc_iter)->getDouble("VJDISTANCETOIP")*Gaudi::Units::mm;
    VJconelength                  = (*AccessSvc_iter)->getDouble("VJCONELENGTH")*Gaudi::Units::mm;
    VJconeRadiusFront             = (*AccessSvc_iter)->getDouble("VJCONERADIUSFRONT")*Gaudi::Units::mm;
    VJconeRadiusBack              = (*AccessSvc_iter)->getDouble("VJCONERADIUSBACK")*Gaudi::Units::mm;
    VJconeThickness               = (*AccessSvc_iter)->getDouble("VJCONETHICKNESS")*Gaudi::Units::mm;
    VJconeFrontRingThickness      = (*AccessSvc_iter)->getDouble("VJCONEFRONTRINGTHICKNESS")*Gaudi::Units::mm;
    VJconeFrontRingLength         = (*AccessSvc_iter)->getDouble("VJCONEFRONTRINGLENGTH")*Gaudi::Units::mm;
    VJconeFrontRingOverlap        = (*AccessSvc_iter)->getDouble("VJCONEFRONTRINGOVERLAP")*Gaudi::Units::mm;
    cylinderLength                = (*AccessSvc_iter)->getDouble("VESSELLENGTH")*Gaudi::Units::mm;
    cylinderRadius                = (*AccessSvc_iter)->getDouble("COOLINGRADIUS")*Gaudi::Units::mm; 
    cylinderThickness             = (*AccessSvc_iter)->getDouble("COOLINGTHICKNESS")*Gaudi::Units::mm;
    pmtPosition                   = (*AccessSvc_iter)->getDouble("LAYERRADIUS1")*Gaudi::Units::mm;
    pmtRadius                     = (*AccessSvc_iter)->getDouble("TUBERADIUS")*Gaudi::Units::mm;
    pmtThickness                  = (*AccessSvc_iter)->getDouble("PMTTHICKNESS")*Gaudi::Units::mm;
    quartzDensity                 = (*AccessSvc_iter)->getDouble("QUARTZDENSITY")*GeoModelKernelUnits::gram/Gaudi::Units::cm3;
    waveLengthStep                = (*AccessSvc_iter)->getDouble("WAVELENGTHSTEP");
    waveLengthMin                 = (*AccessSvc_iter)->getDouble("WAVELENGTHMIN");
    waveLengthMax                 = (*AccessSvc_iter)->getDouble("WAVELENGTHMAX");
  }
}

void LUCID_RDBAccess::Print() {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_GeoModel::LUCID_RDBAaccess");

  log << MSG::INFO  << " distanceToIP            [mm]: " << distanceToIP/Gaudi::Units::mm                 << endmsg;
  log << MSG::INFO  << " VJdistanceToIP          [mm]: " << VJdistanceToIP/Gaudi::Units::mm               << endmsg;
  log << MSG::INFO  << " VJconelength            [mm]: " << VJconelength/Gaudi::Units::mm                 << endmsg;
  log << MSG::INFO  << " VJconeRadiusFront       [mm]: " << VJconeRadiusFront/Gaudi::Units::mm            << endmsg;
  log << MSG::INFO  << " VJconeRadiusBack        [mm]: " << VJconeRadiusBack/Gaudi::Units::mm             << endmsg;
  log << MSG::INFO  << " VJconeThickness         [mm]: " << VJconeThickness/Gaudi::Units::mm              << endmsg;
  log << MSG::INFO  << " VJconeFrontRingThickness[mm]: " << VJconeFrontRingThickness/Gaudi::Units::mm     << endmsg;
  log << MSG::INFO  << " VJconeFrontRingLength   [mm]: " << VJconeFrontRingLength/Gaudi::Units::mm        << endmsg;
  log << MSG::INFO  << " VJconeFrontRingOverlap  [mm]: " << VJconeFrontRingOverlap/Gaudi::Units::mm       << endmsg;
  log << MSG::INFO  << " cylinderLength          [mm]: " << cylinderLength/Gaudi::Units::mm               << endmsg;
  log << MSG::INFO  << " cylinderRadius          [mm]: " << cylinderRadius/Gaudi::Units::mm               << endmsg;
  log << MSG::INFO  << " cylinderThickness       [mm]: " << cylinderThickness/Gaudi::Units::mm            << endmsg;
  log << MSG::INFO  << " pmtPosition             [mm]: " << pmtPosition/Gaudi::Units::mm                  << endmsg;
  log << MSG::INFO  << " pmtRadius               [mm]: " << pmtRadius/Gaudi::Units::mm                    << endmsg;
  log << MSG::INFO  << " pmtThickness            [mm]: " << pmtThickness/Gaudi::Units::mm                 << endmsg;
  log << MSG::INFO  << " quartzDensity        [g/cm3]: " << quartzDensity/(GeoModelKernelUnits::gram/Gaudi::Units::cm3) << endmsg;
  log << MSG::INFO  << " waveLengthStep          [nm]: " << waveLengthStep                         << endmsg;
  log << MSG::INFO  << " waveLengthMin           [nm]: " << waveLengthMin                          << endmsg;
  log << MSG::INFO  << " waveLengthMax           [nm]: " << waveLengthMax                          << endmsg;
}

void LUCID_RDBAccess::CloseDB() {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_GeoModel::LUCID_RDBAaccess");
  
  log << MSG::DEBUG << " LUCID_RDBAaccess::CloseDB "<< endmsg;

  log << MSG::DEBUG << " ending LUCID_GeoModel::LUCID_RDBAaccess " << endmsg;
}
