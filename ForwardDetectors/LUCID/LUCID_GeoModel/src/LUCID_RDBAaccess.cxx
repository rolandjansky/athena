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
  
  log << MSG::DEBUG << " Watch-out: VESSELOUTERRADMIN and VESSELOUTERRADMAX are increased of 3mm to remove overlaps. " << endmsg;
  
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
    vesselInnerRadius             = (*AccessSvc_iter)->getDouble("VESSELINNERRADIUS")*Gaudi::Units::mm;
    vesselInnerThickness          = (*AccessSvc_iter)->getDouble("VESSELINNERTHICKNESS")*Gaudi::Units::mm;
    vesselOuterRadMin             = (*AccessSvc_iter)->getDouble("VESSELOUTERRADMIN")*Gaudi::Units::mm + 3*Gaudi::Units::mm;
    vesselOuterRadMax             = (*AccessSvc_iter)->getDouble("VESSELOUTERRADMAX")*Gaudi::Units::mm + 3*Gaudi::Units::mm;
    vesselOuterThickness          = (*AccessSvc_iter)->getDouble("VESSELOUTERTHICKNESS")*Gaudi::Units::mm;
    vesselLength                  = (*AccessSvc_iter)->getDouble("VESSELLENGTH")*Gaudi::Units::mm;
    bulkheadThickness             = (*AccessSvc_iter)->getDouble("BULKHEADTHICKNESS")*Gaudi::Units::mm;
    coolingRadius                 = (*AccessSvc_iter)->getDouble("COOLINGRADIUS")*Gaudi::Units::mm;
    coolingThickness              = (*AccessSvc_iter)->getDouble("COOLINGTHICKNESS")*Gaudi::Units::mm;
    layerRadius1                  = (*AccessSvc_iter)->getDouble("LAYERRADIUS1")*Gaudi::Units::mm;
    layerRadius2                  = (*AccessSvc_iter)->getDouble("LAYERRADIUS2")*Gaudi::Units::mm;
    tubeRadius                    = (*AccessSvc_iter)->getDouble("TUBERADIUS")*Gaudi::Units::mm;
    tubeThickness                 = (*AccessSvc_iter)->getDouble("TUBETHICKNESS")*Gaudi::Units::mm;
    tubeLength                    = (*AccessSvc_iter)->getDouble("TUBELENGTH")*Gaudi::Units::mm;
    pmtThickness                  = (*AccessSvc_iter)->getDouble("PMTTHICKNESS")*Gaudi::Units::mm;
    gasPressure                   = (*AccessSvc_iter)->getDouble("GASPRESSURE")*Gaudi::Units::bar;
    gasDensity                    = (*AccessSvc_iter)->getDouble("GASDENSITY")*GeoModelKernelUnits::gram/Gaudi::Units::cm3;
    gasTemperature                = (*AccessSvc_iter)->getDouble("GASTEMPERATURE")*Gaudi::Units::kelvin;
    quartzDensity                 = (*AccessSvc_iter)->getDouble("QUARTZDENSITY")*GeoModelKernelUnits::gram/Gaudi::Units::cm3;
    tubePolish                    = (*AccessSvc_iter)->getDouble("TUBEPOLISH");
    waveLengthStep                = (*AccessSvc_iter)->getDouble("WAVELENGTHSTEP");
    waveLengthMin                 = (*AccessSvc_iter)->getDouble("WAVELENGTHMIN");
    waveLengthMax                 = (*AccessSvc_iter)->getDouble("WAVELENGTHMAX");
  }
}

void LUCID_RDBAccess::Print() {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_GeoModel::LUCID_RDBAaccess");

  log << MSG::DEBUG << " distanceToIP         [mm]: " << distanceToIP/Gaudi::Units::mm           << endmsg;
  log << MSG::DEBUG << " vesselInnerRadius    [mm]: " << vesselInnerRadius/Gaudi::Units::mm      << endmsg;
  log << MSG::DEBUG << " vesselInnerThickness [mm]: " << vesselInnerThickness/Gaudi::Units::mm   << endmsg;
  log << MSG::DEBUG << " vesselOuterRadMin    [mm]: " << vesselOuterRadMin /Gaudi::Units::mm     << endmsg;
  log << MSG::DEBUG << " vesselOuterRadMax    [mm]: " << vesselOuterRadMax/Gaudi::Units::mm      << endmsg;
  log << MSG::DEBUG << " vesselOuterThickness [mm]: " << vesselOuterThickness/Gaudi::Units::mm   << endmsg;
  log << MSG::DEBUG << " vesselLength         [mm]: " << vesselLength /Gaudi::Units::mm          << endmsg;
  log << MSG::DEBUG << " bulkheadThickness    [mm]: " << bulkheadThickness/Gaudi::Units::mm      << endmsg;
  log << MSG::DEBUG << " coolingRadius        [mm]: " << coolingRadius/Gaudi::Units::mm          << endmsg;
  log << MSG::DEBUG << " coolingThickness     [mm]: " << coolingThickness/Gaudi::Units::mm       << endmsg;
  log << MSG::DEBUG << " layerRadius1         [mm]: " << layerRadius1/Gaudi::Units::mm           << endmsg;
  log << MSG::DEBUG << " layerRadius2         [mm]: " << layerRadius2/Gaudi::Units::mm           << endmsg;
  log << MSG::DEBUG << " tubeRadius           [mm]: " << tubeRadius/Gaudi::Units::mm             << endmsg;
  log << MSG::DEBUG << " tubeThickness        [mm]: " << tubeThickness/Gaudi::Units::mm          << endmsg;
  log << MSG::DEBUG << " tubeLength           [mm]: " << tubeLength/Gaudi::Units::mm             << endmsg;
  log << MSG::DEBUG << " pmtThickness         [mm]: " << pmtThickness/Gaudi::Units::mm           << endmsg;
  log << MSG::DEBUG << " gasPressure         [bar]: " << gasPressure/Gaudi::Units::bar           << endmsg;
  log << MSG::DEBUG << " gasDensity        [g/cm3]: " << gasDensity/(GeoModelKernelUnits::gram/Gaudi::Units::cm3)     << endmsg;
  log << MSG::DEBUG << " gasTempearture   [kelvin]: " << gasTemperature/Gaudi::Units::kelvin     << endmsg;
  log << MSG::DEBUG << " quartzDensity     [g/cm3]: " << quartzDensity/(GeoModelKernelUnits::gram/Gaudi::Units::cm3)  << endmsg;  
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
