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

    // -------- -------- MATERIAL MANAGER -------- ----------
    // TODO: fix this, doesn't get access at runtime!!
    StoredMaterialManager* theMaterialManager = nullptr; // dummy matManager, only used to instantiate a sectionBuilder
    /*if (StatusCode::SUCCESS != m_detectorStore->retrieve(theMaterialManager, "MATERIALS")) 
    {  
        (*m_log) << MSG::ERROR << "Could not find Material Manager MATERIALS" << endmsg; 
        return; 
    }*/

    // -------- -------- MATERIAL MANAGER -------- ----------
    TileDddbManager* dbManager = m_detectorManager->getDbManager();
    
    // -------- -------- SECTION BUILDER  -------- ----------
    TileGeoSectionBuilder* sectionBuilder = new TileGeoSectionBuilder(theMaterialManager,dbManager,m_switches,m_log);


     //
     // creating Descriptors and CaloDetDescrElements 
     // 

     int nModulesInSection[6] = {0,0,0,0,0,0};
     double zShiftInSection[6] = {0.0,0.0,0.0,0.0,0.0,0.0,};

     // int NumberOfEnv = dbManager->GetNumberOfEnv();


     for(int EnvCounter = 0; EnvCounter < dbManager->GetNumberOfEnv(); ++EnvCounter){ //Loop over Envelopes

       dbManager->SetCurrentEnvByIndex(EnvCounter);
       int EnvType = dbManager->GetEnvType();
       int NumberOfMod = dbManager->GetEnvNModules();
       double Zshift = dbManager->GetEnvZShift()*Gaudi::Units::cm;

       if(m_log->level()<=MSG::DEBUG)
     (*m_log) << MSG::DEBUG 
          << " EnvCounter is " << EnvCounter
          << " EnvType is " << EnvType
          << " Zshift is " << Zshift*(1./Gaudi::Units::cm) << " cm"
          << endmsg;

       // Central barrel 
       if(EnvType == 1 || EnvType == 0) { 
         nModulesInSection[0] = nModulesInSection[1] = NumberOfMod;
         zShiftInSection[0] = zShiftInSection[1] = Zshift;
       } else if(EnvType < 6) {
         nModulesInSection[EnvType] = NumberOfMod;
         zShiftInSection[EnvType] = Zshift;
       }
     }


    // Build Regions
    const TileID* tileID = m_detectorManager->get_id();

    unsigned int dete[6] = {TILE_REGION_CENTRAL,TILE_REGION_CENTRAL,
                            TILE_REGION_EXTENDED,TILE_REGION_EXTENDED,
                            TILE_REGION_GAP,TILE_REGION_GAP};
    int side[6] = {0,1,0,1,0,1};

    for (int ii=0; ii<6; ++ii) {
        if (ii%2 == 0) {
            sectionBuilder->computeCellDim(m_detectorManager, dete[ii],
                    m_switches.addPlatesToCell,
                    zShiftInSection[ii+1], // zShiftPos
                    zShiftInSection[ii]);  // zShiftNeg
        }

        TileDetDescriptor* descriptor = new TileDetDescriptor();
        sectionBuilder->fillDescriptor(descriptor, dete[ii], side[ii],
                m_switches.testBeam,      // set to false - ATLAS geometry
                m_switches.addPlatesToCell, // add front/end plates to cell volume
                nModulesInSection[ii],   // 0-64 modules
                zShiftInSection[ii]);    // Z-shift

        Identifier idRegion = tileID->region_id(ii);
        descriptor->set(idRegion);
        m_detectorManager->add(descriptor); 
        m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor)); 
    }


    // Set geometry Tree Tops
    GeoVolumeCursor cursor(world);
    while(!cursor.atEnd()) {
        std::string volName = cursor.getName();
        std::cout << "volName: " << volName << "\n";
        if(volName.compare(0,4,"Tile")==0) {
            (*m_log) << MSG::DEBUG << "Adding vol '" << volName << "' to detManager" << endmsg;
            m_detectorManager->addTreeTop(GeoPVLink(cursor.getVolume().operator->()));
        }
        cursor.next();
    }
}


