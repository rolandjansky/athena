/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include "TRT_TR_Process/TRRegionXMLHandler.h"
#include "TRT_TR_Process/TRTTransitionRadiation.h"
#include "TRT_TR_Process/TRTRadiatorParameters.h"
#include "FadsGeometry/GeometryManager.h"
#include "IdDictDetDescr/IdDictManager.h"


//For Detector store:
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include <iostream>

TRRegionXMLHandler::TRRegionXMLHandler(std::string s,
                                       TRTTransitionRadiation *tr) :
  DescriptionFactory(s), m_theProcess(tr),
  m_storeGate(0),
  m_initialLayoutIdDict(false),
  m_msg("TRRegionXMLHandler")
{}

void TRRegionXMLHandler::BuildDescription()
{
  ISvcLocator * svcLocator = Gaudi::svcLocator(); // from Bootstrap


  if (msgLevel(MSG::DEBUG))
    msg(MSG::DEBUG) << "This is TRRegionXMLHandler. Handler called" << endreq;

  StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
  if( sc.isFailure() ) {
    if (msgLevel(MSG::ERROR))
      msg(MSG::ERROR) << "Unable to locate StoreGate! Stopping!" << endreq;
  }
  StoreGateSvc* detStore = 0;
  sc = svcLocator->service( "DetectorStore", detStore);
  if( sc.isFailure() ) {
    if (msgLevel(MSG::ERROR))
      msg(MSG::ERROR) << "Unable to locate DetectorStore! Leaving!" << endreq;
  }

  const IdDictManager * idDictMgr = 0;
  if (StatusCode::SUCCESS == detStore->retrieve(idDictMgr, "IdDict")) {
    if (idDictMgr) {
      std::string tag = idDictMgr->manager()->tag();
      m_initialLayoutIdDict =
        (tag == "initial_layout" || tag == "destaged_layout");
    }
  } else {
    if (msgLevel(MSG::FATAL))
      msg(MSG::FATAL) << "Could not retrieve geometry layout. TR process is not to be trusted in the following "
                      << endreq;
  }


  FADS::GeometryManager* geomManager=FADS::GeometryManager::GetGeometryManager();
  std::string volName=GetAttributeAsString("RadiatorName");
  double foilThickness=GetAttributeAsDouble("RadiatorFoilThickness");
  double gasThickness=GetAttributeAsDouble("RadiatorGasThickness");
  int regionFlag=GetAttributeAsInt("RadiatorBARRELorENDCAP");
  std::string detectorPart=GetAttributeAsString("DetectorPart");

  unsigned int numberOfVolumes = geomManager->GetVolume(detectorPart,volName).size();

  if( numberOfVolumes == 0 ) {
    // In case of destaged geometry (no C wheels) any missing volume
    // except the radiators corresponding to the C wheels will cause
    // the code to abort
    if ( m_initialLayoutIdDict != 0 ) {
      if  ( ( volName!="TRT::MainRadiatorC" ) &&
            ( volName!="TRT::ThinRadiatorC" )    ) {
        if (msgLevel(MSG::FATAL))
          msg(MSG::FATAL) << " Volume-name " << volName
                          <<" not found! Geometry layout "
                          << m_initialLayoutIdDict << endreq;
        std::abort();
      }
    }
    return;
  }

  for( unsigned int i=0; i<numberOfVolumes; i++) {
    TRTRadiatorParameters rad( geomManager->GetVolume(detectorPart,volName)[i],
                               foilThickness,gasThickness,
                               (BEflag)regionFlag );
    m_theProcess->AddRadiatorParameters(rad);
  }

}
