/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_RDBAaccess.h"

#include "CLHEP/Units/SystemOfUnits.h"

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
    
    distanceToIP                  = (*AccessSvc_iter)->getDouble("DISTANCETOIP")*CLHEP::mm;
    VJdistanceToIP                = (*AccessSvc_iter)->getDouble("VJDISTANCETOIP")*CLHEP::mm;
    VJconelength                  = (*AccessSvc_iter)->getDouble("VJCONELENGTH")*CLHEP::mm;
    VJconeRadiusFront             = (*AccessSvc_iter)->getDouble("VJCONERADIUSFRONT")*CLHEP::mm;
    VJconeRadiusBack              = (*AccessSvc_iter)->getDouble("VJCONERADIUSBACK")*CLHEP::mm;
    VJconeThickness               = (*AccessSvc_iter)->getDouble("VJCONETHICKNESS")*CLHEP::mm;
    VJconeFrontRingThickness      = (*AccessSvc_iter)->getDouble("VJCONEFRONTRINGTHICKNESS")*CLHEP::mm;
    VJconeFrontRingLength         = (*AccessSvc_iter)->getDouble("VJCONEFRONTRINGLENGTH")*CLHEP::mm;
    VJconeFrontRingOverlap        = (*AccessSvc_iter)->getDouble("VJCONEFRONTRINGOVERLAP")*CLHEP::mm;
    vesselInnerRadius             = (*AccessSvc_iter)->getDouble("VESSELINNERRADIUS")*CLHEP::mm;
    vesselInnerThickness          = (*AccessSvc_iter)->getDouble("VESSELINNERTHICKNESS")*CLHEP::mm;
    vesselOuterRadMin             = (*AccessSvc_iter)->getDouble("VESSELOUTERRADMIN")*CLHEP::mm + 3*CLHEP::mm;
    vesselOuterRadMax             = (*AccessSvc_iter)->getDouble("VESSELOUTERRADMAX")*CLHEP::mm + 3*CLHEP::mm;
    vesselOuterThickness          = (*AccessSvc_iter)->getDouble("VESSELOUTERTHICKNESS")*CLHEP::mm;
    vesselLength                  = (*AccessSvc_iter)->getDouble("VESSELLENGTH")*CLHEP::mm;
    bulkheadThickness             = (*AccessSvc_iter)->getDouble("BULKHEADTHICKNESS")*CLHEP::mm;
    coolingRadius                 = (*AccessSvc_iter)->getDouble("COOLINGRADIUS")*CLHEP::mm;
    coolingThickness              = (*AccessSvc_iter)->getDouble("COOLINGTHICKNESS")*CLHEP::mm;
    layerRadius1                  = (*AccessSvc_iter)->getDouble("LAYERRADIUS1")*CLHEP::mm;
    layerRadius2                  = (*AccessSvc_iter)->getDouble("LAYERRADIUS2")*CLHEP::mm;
    tubeRadius                    = (*AccessSvc_iter)->getDouble("TUBERADIUS")*CLHEP::mm;
    tubeThickness                 = (*AccessSvc_iter)->getDouble("TUBETHICKNESS")*CLHEP::mm;
    tubeLength                    = (*AccessSvc_iter)->getDouble("TUBELENGTH")*CLHEP::mm;
    pmtThickness                  = (*AccessSvc_iter)->getDouble("PMTTHICKNESS")*CLHEP::mm;
    gasPressure                   = (*AccessSvc_iter)->getDouble("GASPRESSURE")*CLHEP::bar;
    gasDensity                    = (*AccessSvc_iter)->getDouble("GASDENSITY")*CLHEP::gram/CLHEP::cm3;
    gasTemperature                = (*AccessSvc_iter)->getDouble("GASTEMPERATURE")*CLHEP::kelvin;
    quartzDensity                 = (*AccessSvc_iter)->getDouble("QUARTZDENSITY")*CLHEP::gram/CLHEP::cm3;
    tubePolish                    = (*AccessSvc_iter)->getDouble("TUBEPOLISH");
    waveLengthStep                = (*AccessSvc_iter)->getDouble("WAVELENGTHSTEP");
    waveLengthMin                 = (*AccessSvc_iter)->getDouble("WAVELENGTHMIN");
    waveLengthMax                 = (*AccessSvc_iter)->getDouble("WAVELENGTHMAX");
  }
}

void LUCID_RDBAccess::Print() {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_GeoModel::LUCID_RDBAaccess");

  log << MSG::DEBUG << " distanceToIP         [mm]: " << distanceToIP/CLHEP::mm           << endmsg;
  log << MSG::DEBUG << " vesselInnerRadius    [mm]: " << vesselInnerRadius/CLHEP::mm      << endmsg;
  log << MSG::DEBUG << " vesselInnerThickness [mm]: " << vesselInnerThickness/CLHEP::mm   << endmsg;
  log << MSG::DEBUG << " vesselOuterRadMin    [mm]: " << vesselOuterRadMin /CLHEP::mm     << endmsg;
  log << MSG::DEBUG << " vesselOuterRadMax    [mm]: " << vesselOuterRadMax/CLHEP::mm      << endmsg;
  log << MSG::DEBUG << " vesselOuterThickness [mm]: " << vesselOuterThickness/CLHEP::mm   << endmsg;
  log << MSG::DEBUG << " vesselLength         [mm]: " << vesselLength /CLHEP::mm          << endmsg;
  log << MSG::DEBUG << " bulkheadThickness    [mm]: " << bulkheadThickness/CLHEP::mm      << endmsg;
  log << MSG::DEBUG << " coolingRadius        [mm]: " << coolingRadius/CLHEP::mm          << endmsg;
  log << MSG::DEBUG << " coolingThickness     [mm]: " << coolingThickness/CLHEP::mm       << endmsg;
  log << MSG::DEBUG << " layerRadius1         [mm]: " << layerRadius1/CLHEP::mm           << endmsg;
  log << MSG::DEBUG << " layerRadius2         [mm]: " << layerRadius2/CLHEP::mm           << endmsg;
  log << MSG::DEBUG << " tubeRadius           [mm]: " << tubeRadius/CLHEP::mm             << endmsg;
  log << MSG::DEBUG << " tubeThickness        [mm]: " << tubeThickness/CLHEP::mm          << endmsg;
  log << MSG::DEBUG << " tubeLength           [mm]: " << tubeLength/CLHEP::mm             << endmsg;
  log << MSG::DEBUG << " pmtThickness         [mm]: " << pmtThickness/CLHEP::mm           << endmsg;
  log << MSG::DEBUG << " gasPressure         [bar]: " << gasPressure/CLHEP::bar           << endmsg;
  log << MSG::DEBUG << " gasDensity        [g/cm3]: " << gasDensity/(CLHEP::gram/CLHEP::cm3)     << endmsg;
  log << MSG::DEBUG << " gasTempearture   [kelvin]: " << gasTemperature/CLHEP::kelvin     << endmsg;
  log << MSG::DEBUG << " quartzDensity     [g/cm3]: " << quartzDensity/(CLHEP::gram/CLHEP::cm3)  << endmsg;  
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
