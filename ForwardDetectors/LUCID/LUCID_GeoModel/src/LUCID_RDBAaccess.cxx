/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_RDBAaccess.h"

//#include "CLHEP/Units/SystemOfUnits.h"
#include "GeoModelKernel/Units.h"

#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Bootstrap.h"

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
  
  log << MSG::DEBUG << " Watch-out: VESSELOUTERRADMIN and VESSELOUTERRADMAX are increased of 3mm to remove overlaps. " << endmsg;
  
  IRDBRecordset::const_iterator AccessSvc_iter;
  
  for(AccessSvc_iter = m_lucidParams->begin(); AccessSvc_iter != m_lucidParams->end(); AccessSvc_iter++) {
    
    distanceToIP                  = (*AccessSvc_iter)->getDouble("DISTANCETOIP")*GeoModelKernelUnits::mm;
    VJdistanceToIP                = (*AccessSvc_iter)->getDouble("VJDISTANCETOIP")*GeoModelKernelUnits::mm;
    VJconelength                  = (*AccessSvc_iter)->getDouble("VJCONELENGTH")*GeoModelKernelUnits::mm;
    VJconeRadiusFront             = (*AccessSvc_iter)->getDouble("VJCONERADIUSFRONT")*GeoModelKernelUnits::mm;
    VJconeRadiusBack              = (*AccessSvc_iter)->getDouble("VJCONERADIUSBACK")*GeoModelKernelUnits::mm;
    VJconeThickness               = (*AccessSvc_iter)->getDouble("VJCONETHICKNESS")*GeoModelKernelUnits::mm;
    VJconeFrontRingThickness      = (*AccessSvc_iter)->getDouble("VJCONEFRONTRINGTHICKNESS")*GeoModelKernelUnits::mm;
    VJconeFrontRingLength         = (*AccessSvc_iter)->getDouble("VJCONEFRONTRINGLENGTH")*GeoModelKernelUnits::mm;
    VJconeFrontRingOverlap        = (*AccessSvc_iter)->getDouble("VJCONEFRONTRINGOVERLAP")*GeoModelKernelUnits::mm;
    vesselInnerRadius             = (*AccessSvc_iter)->getDouble("VESSELINNERRADIUS")*GeoModelKernelUnits::mm;
    vesselInnerThickness          = (*AccessSvc_iter)->getDouble("VESSELINNERTHICKNESS")*GeoModelKernelUnits::mm;
    vesselOuterRadMin             = (*AccessSvc_iter)->getDouble("VESSELOUTERRADMIN")*GeoModelKernelUnits::mm + 3*GeoModelKernelUnits::mm;
    vesselOuterRadMax             = (*AccessSvc_iter)->getDouble("VESSELOUTERRADMAX")*GeoModelKernelUnits::mm + 3*GeoModelKernelUnits::mm;
    vesselOuterThickness          = (*AccessSvc_iter)->getDouble("VESSELOUTERTHICKNESS")*GeoModelKernelUnits::mm;
    vesselLength                  = (*AccessSvc_iter)->getDouble("VESSELLENGTH")*GeoModelKernelUnits::mm;
    bulkheadThickness             = (*AccessSvc_iter)->getDouble("BULKHEADTHICKNESS")*GeoModelKernelUnits::mm;
    coolingRadius                 = (*AccessSvc_iter)->getDouble("COOLINGRADIUS")*GeoModelKernelUnits::mm;
    coolingThickness              = (*AccessSvc_iter)->getDouble("COOLINGTHICKNESS")*GeoModelKernelUnits::mm;
    layerRadius1                  = (*AccessSvc_iter)->getDouble("LAYERRADIUS1")*GeoModelKernelUnits::mm;
    layerRadius2                  = (*AccessSvc_iter)->getDouble("LAYERRADIUS2")*GeoModelKernelUnits::mm;
    tubeRadius                    = (*AccessSvc_iter)->getDouble("TUBERADIUS")*GeoModelKernelUnits::mm;
    tubeThickness                 = (*AccessSvc_iter)->getDouble("TUBETHICKNESS")*GeoModelKernelUnits::mm;
    tubeLength                    = (*AccessSvc_iter)->getDouble("TUBELENGTH")*GeoModelKernelUnits::mm;
    pmtThickness                  = (*AccessSvc_iter)->getDouble("PMTTHICKNESS")*GeoModelKernelUnits::mm;
    gasPressure                   = (*AccessSvc_iter)->getDouble("GASPRESSURE")*GeoModelKernelUnits::bar;
    gasDensity                    = (*AccessSvc_iter)->getDouble("GASDENSITY")*GeoModelKernelUnits::gram/GeoModelKernelUnits::cm3;
    gasTemperature                = (*AccessSvc_iter)->getDouble("GASTEMPERATURE")*GeoModelKernelUnits::kelvin;
    quartzDensity                 = (*AccessSvc_iter)->getDouble("QUARTZDENSITY")*GeoModelKernelUnits::gram/GeoModelKernelUnits::cm3;
    tubePolish                    = (*AccessSvc_iter)->getDouble("TUBEPOLISH");
    waveLengthStep                = (*AccessSvc_iter)->getDouble("WAVELENGTHSTEP");
    waveLengthMin                 = (*AccessSvc_iter)->getDouble("WAVELENGTHMIN");
    waveLengthMax                 = (*AccessSvc_iter)->getDouble("WAVELENGTHMAX");
  }
}

void LUCID_RDBAccess::Print() {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_GeoModel::LUCID_RDBAaccess");

  log << MSG::DEBUG << " distanceToIP         [mm]: " << distanceToIP/GeoModelKernelUnits::mm           << endmsg;
  log << MSG::DEBUG << " vesselInnerRadius    [mm]: " << vesselInnerRadius/GeoModelKernelUnits::mm      << endmsg;
  log << MSG::DEBUG << " vesselInnerThickness [mm]: " << vesselInnerThickness/GeoModelKernelUnits::mm   << endmsg;
  log << MSG::DEBUG << " vesselOuterRadMin    [mm]: " << vesselOuterRadMin /GeoModelKernelUnits::mm     << endmsg;
  log << MSG::DEBUG << " vesselOuterRadMax    [mm]: " << vesselOuterRadMax/GeoModelKernelUnits::mm      << endmsg;
  log << MSG::DEBUG << " vesselOuterThickness [mm]: " << vesselOuterThickness/GeoModelKernelUnits::mm   << endmsg;
  log << MSG::DEBUG << " vesselLength         [mm]: " << vesselLength /GeoModelKernelUnits::mm          << endmsg;
  log << MSG::DEBUG << " bulkheadThickness    [mm]: " << bulkheadThickness/GeoModelKernelUnits::mm      << endmsg;
  log << MSG::DEBUG << " coolingRadius        [mm]: " << coolingRadius/GeoModelKernelUnits::mm          << endmsg;
  log << MSG::DEBUG << " coolingThickness     [mm]: " << coolingThickness/GeoModelKernelUnits::mm       << endmsg;
  log << MSG::DEBUG << " layerRadius1         [mm]: " << layerRadius1/GeoModelKernelUnits::mm           << endmsg;
  log << MSG::DEBUG << " layerRadius2         [mm]: " << layerRadius2/GeoModelKernelUnits::mm           << endmsg;
  log << MSG::DEBUG << " tubeRadius           [mm]: " << tubeRadius/GeoModelKernelUnits::mm             << endmsg;
  log << MSG::DEBUG << " tubeThickness        [mm]: " << tubeThickness/GeoModelKernelUnits::mm          << endmsg;
  log << MSG::DEBUG << " tubeLength           [mm]: " << tubeLength/GeoModelKernelUnits::mm             << endmsg;
  log << MSG::DEBUG << " pmtThickness         [mm]: " << pmtThickness/GeoModelKernelUnits::mm           << endmsg;
  log << MSG::DEBUG << " gasPressure         [bar]: " << gasPressure/GeoModelKernelUnits::bar           << endmsg;
  log << MSG::DEBUG << " gasDensity        [g/cm3]: " << gasDensity/(GeoModelKernelUnits::gram/GeoModelKernelUnits::cm3)     << endmsg;
  log << MSG::DEBUG << " gasTempearture   [kelvin]: " << gasTemperature/GeoModelKernelUnits::kelvin     << endmsg;
  log << MSG::DEBUG << " quartzDensity     [g/cm3]: " << quartzDensity/(GeoModelKernelUnits::gram/GeoModelKernelUnits::cm3)  << endmsg;  
  log << MSG::DEBUG << " tubePolish               : " << tubePolish                << endmsg;
  log << MSG::DEBUG << " waveLengthStep           : " << waveLengthStep            << endmsg;
  log << MSG::DEBUG << " waveLengthMin            : " << waveLengthMin             << endmsg;
  log << MSG::DEBUG << " waveLengthMax            : " << waveLengthMax             << endmsg;
}

void LUCID_RDBAccess::CloseDB() {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_GeoModel::LUCID_RDBAaccess");
  
  log << MSG::DEBUG << " LUCID_RDBAaccess::CloseDB "<< endmsg;

  log << MSG::DEBUG << " ending LUCID_GeoModel::LUCID_RDBAaccess " << endmsg;
}
