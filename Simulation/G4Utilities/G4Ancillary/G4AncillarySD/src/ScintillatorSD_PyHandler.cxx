/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AncillarySD/ScintillatorSD_PyHandler.h"
#include "G4AncillarySD/ScintillatorSD.h"

#include "FadsGeometry/GeometryManager.h"

#include <string>
#include <iostream>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"


ScintillatorSD_PyHandler::ScintillatorSD_PyHandler() : theScintillatorSD(0) {}

//______________________________________________________________________________
void ScintillatorSD_PyHandler::BuildDescription( std::string detectorPart,
						 std::string volumeName, std::string hitCollectionName,
						 bool writeHitsWithZeroEnergy) {
  FADS::GeometryManager* geomManager=FADS::GeometryManager::GetGeometryManager();
  unsigned int numberOfVolumes = geomManager->GetVolume(detectorPart,volumeName).size();

  //Get MsgSvc:
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  IMessageSvc  * msgSvc;
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  if (status == StatusCode::SUCCESS) {
    MsgStream log(msgSvc, "ScintillatorSD_PyHandler");
    log << MSG::DEBUG << "Associating " << numberOfVolumes << " log. vol. for "
	<< detectorPart << " : " << volumeName << " with the HC : " << hitCollectionName
	<< " (" << ( writeHitsWithZeroEnergy ? "will" : "won't" )
        << " write hits with Edep=0)" << endreq;
   } else {
     std::cerr << "ScintillatorSD_PyHandler::BuildDescription ERROR Couldn't retrieve MessageSvc !!" << std::endl;
     msgSvc = NULL;
   };

  for( unsigned int i = 0; i < numberOfVolumes; ++i) {
    ScintillatorSD::addVolumeDelayed( geomManager->GetVolume(detectorPart,volumeName)[i],
				      hitCollectionName,
				      writeHitsWithZeroEnergy );
    if (msgSvc) {
      MsgStream log(msgSvc, "ScintillatorSD_PyHandler");
      log << MSG::DEBUG <<"Added " << geomManager->GetVolume(detectorPart,volumeName)[i] <<endreq;
     };
  };
}

