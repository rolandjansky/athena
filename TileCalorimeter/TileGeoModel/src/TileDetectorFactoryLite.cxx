/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch> , Aug 2021
 */

#include "TileDetectorFactoryLite.h"
#include "TileGeoSectionBuilder.h"
#include "TileGeoCutBuilder.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileDddbManager.h"
#include "TileDetDescr/TileDetDescriptor.h"

#include "CaloIdentifier/TileID.h"

#include "GeoModelKernel/GeoVolumeCursor.h"

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm> 
#include <cmath>
#include <string>


// Constructor:
TileDetectorFactoryLite::TileDetectorFactoryLite(StoreGateSvc *pDetStore,
                                   TileDetDescrManager *manager,
						           GeoModelIO::ReadGeoModel* sqliteReader, 
                                   const TileSwitches & switches,
                                   MsgStream *log,
                                   bool fullGeo)
      : m_detectorStore(pDetStore)
      , m_detectorManager(manager)
      , m_sqliteReader(sqliteReader)
      , m_switches(switches)
      , m_log(log)
      //, m_addPlatesToCellVolume(addPlates)
      //, m_uShape(ushape)
      //, m_glue(glue)
      //, m_csTube(cstube)
      //, m_testbeamGeometry(false)
      , m_verbose(log->level()<=MSG::VERBOSE) 
      , m_fullGeo(fullGeo)
{
  m_switches.testBeam = false;
}
      
// Destructor: 
TileDetectorFactoryLite::~TileDetectorFactoryLite(){}
      
// Creation of geometry:
void TileDetectorFactoryLite::create(GeoPhysVol *world)  
{ 
      (*m_log) << MSG::INFO <<" Entering TileDetectorFactoryLite::create()" << endmsg;
       

      // Build Readout geometry
      // ... TO-DO...
      //


      // Set geometry Tree Tops
      GeoVolumeCursor cursor(world);
      while(!cursor.atEnd()) {
          std::string volName = cursor.getName();
          if(volName.compare(0,3,"Tile")==0) {
              m_detectorManager->addTreeTop(GeoPVLink(cursor.getVolume().operator->()));
          }
          cursor.next();
      }
}


