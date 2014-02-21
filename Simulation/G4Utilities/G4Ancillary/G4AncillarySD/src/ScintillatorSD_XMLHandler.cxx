/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AncillarySD/ScintillatorSD_XMLHandler.h"
#include "G4AncillarySD/ScintillatorSD.h"

#include "FadsGeometry/GeometryManager.h"

#include <string>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

//________________________________________________________________________________________
ScintillatorSD_XMLHandler::ScintillatorSD_XMLHandler(std::string s,ScintillatorSD *scinSD)
: DescriptionFactory(s), theScintillatorSD(scinSD) {}

//________________________________________________________________________________________
void ScintillatorSD_XMLHandler::BuildDescription()
{
  FADS::GeometryManager* geomManager=FADS::GeometryManager::GetGeometryManager();
  std::string detectorPart      = GetAttributeAsString("DetectorPart");
  std::string volumeName  = GetAttributeAsString("VolumeName");
  std::string hitCollectionName = GetAttributeAsString("HitCollectionName");
  bool writeHitsWithZeroEnergy = ( GetAttributeAsString("WriteHitsWithZeroEnergy") == "TRUE" );

  unsigned int numberOfVolumes = geomManager->GetVolume(detectorPart,volumeName).size();

  //Get MsgSvc:
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  IMessageSvc  * msgSvc;
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  if (status == StatusCode::SUCCESS) {
    MsgStream log(msgSvc, "ScintillatorSD_XMLHandler");
    log << MSG::DEBUG << "Associating "<<numberOfVolumes<<" log. vol. for "
	<<detectorPart<<" : "<<volumeName<<" with the HC : "<<hitCollectionName
	<< " ("<<(writeHitsWithZeroEnergy?"will":"won't")<<" write hits with Edep=0)"<<endreq;
  };

  for( unsigned int i=0; i<numberOfVolumes; i++) {
    theScintillatorSD->addVolume( geomManager->GetVolume(detectorPart,volumeName)[i] , hitCollectionName, writeHitsWithZeroEnergy);
  };

}
