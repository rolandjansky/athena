/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * author: Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch> , Aug 2021
 *
 * updates:
 * - 2022 Jan, Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>
 *   Added Readout geometry
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
#include "AthenaKernel/getMessageSvc.h" // needed for 'Athena::getMessageSvc()'

#include "GaudiKernel/SystemOfUnits.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm> 
#include <cmath>
#include <string>

#define MLOG(x)   if (m_log->level()<=MSG::x) *m_log << MSG::x

// Constructor:
TileDetectorFactoryLite::TileDetectorFactoryLite(StoreGateSvc *pDetStore,
                                   TileDetDescrManager *manager,
                                   GeoModelIO::ReadGeoModel* sqliteReader, 
                                   IRDBAccessSvc* paramsSvc,
                                   const TileSwitches & switches,
                                   MsgStream *log,
                                   bool fullGeo)
      : m_detectorStore(pDetStore)
      , m_detectorManager(manager)
      , m_sqliteReader(sqliteReader)
      , m_paramsSvc(paramsSvc)
      , m_switches(switches)
      , m_log(log)
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
    StoredMaterialManager* theMaterialManager = nullptr; // dummy matManager, only used to instantiate a 'sectionBuilder'

    // -------- -------- DB MANAGER -------- ----------
    TileDddbManager* dbManager = m_detectorManager->getDbManager();
    
    // -------- -------- SECTION BUILDER  -------- ----------
    TileGeoSectionBuilder* sectionBuilder = new TileGeoSectionBuilder(theMaterialManager,dbManager,m_switches,m_log);


    // --- Get data from SQLite
    (*m_log) << MSG::INFO << "Getting auxiliary data from SQLite..." << endmsg;
    // get table
    IRDBRecordset_ptr tileReadoutDataRecordSet = m_paramsSvc->getRecordsetPtr("TileReadoutData", "", "");
    (*m_log) << MSG::DEBUG << "data rows in the 'TileReadoutData' RecordSet: " << tileReadoutDataRecordSet->size() << endmsg;
    // get data
    double barrelPeriodThickness = 0., extendedPeriodThickness = 0.;
    if (tileReadoutDataRecordSet->size()>0) {
         if ( (*tileReadoutDataRecordSet)[0]->getString("NAME") == "barrelPeriodThickness" ) {
             barrelPeriodThickness = (*tileReadoutDataRecordSet)[0]->getDouble("VALUE");
         } else {
             std::string errMsg = "barrelPeriodThickness not found in position [0] in the TileReadoutData' RecordSet!!!";
             (*m_log) << MSG::ERROR << errMsg << endmsg;
             throw std::runtime_error(errMsg);
         }
         if ( (*tileReadoutDataRecordSet)[1]->getString("NAME") == "extendedPeriodThickness" ) {
             extendedPeriodThickness = (*tileReadoutDataRecordSet)[1]->getDouble("VALUE");
         } else {
             std::string errMsg = "extendedPeriodThickness not found in position [1] in the TileReadoutData' RecordSet!!!";
             (*m_log) << MSG::ERROR << errMsg << endmsg;
             throw std::runtime_error(errMsg);
         }
    } else {
        std::string errorMsg = "The 'TileReadoutData' recordSet is empty!!!";
        (*m_log) << MSG::FATAL << errorMsg << endmsg;
        throw std::runtime_error(errorMsg);
    }
    (*m_log) << MSG::DEBUG << "barrelPeriodThickness: " << barrelPeriodThickness << ", extendedPeriodThickness: " << extendedPeriodThickness << endmsg;

    // set data used to build the readout geometry
    sectionBuilder->setBarrelPeriodThickness(barrelPeriodThickness);
    sectionBuilder->setExtendedPeriodThickness(extendedPeriodThickness);
    
    
     //
     // creating Descriptors and CaloDetDescrElements 
     // 

     int nModulesInSection[6] = {0,0,0,0,0,0};
     double zShiftInSection[6] = {0.0,0.0,0.0,0.0,0.0,0.0,};

     int NumberOfEnv = dbManager->GetNumberOfEnv();
     MLOG(DEBUG) << "NumberOfEnv: " << NumberOfEnv << endmsg;

     for(int EnvCounter = 0; EnvCounter < NumberOfEnv; ++EnvCounter){ //Loop over Envelopes

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

   (*m_log) << MSG::DEBUG << "Loop over Tile detector regions, and call computeCellDim() when needed..." << endmsg; 
    for (int ii=0; ii<6; ++ii) {

        (*m_log) << MSG::DEBUG << "ii: " << ii << ", region: " << dete[ii] << endmsg; 
        
        if (ii%2 == 0) {
            (*m_log) << MSG::DEBUG << "ii: " << ii << ", region: " << dete[ii] << " --> calling computeCellDim()..." << endmsg; 
            sectionBuilder->computeCellDim(m_detectorManager, dete[ii],
                    m_switches.addPlatesToCell,
                    zShiftInSection[ii+1], // zShiftPos
                    zShiftInSection[ii]);  // zShiftNeg
        }

        (*m_log) << MSG::DEBUG << "Get TileDetDescriptor and call fillDescriptor()..." << endmsg; 
        TileDetDescriptor* descriptor = new TileDetDescriptor();
        sectionBuilder->fillDescriptor(descriptor, dete[ii], side[ii],
                m_switches.testBeam,      // set to false - ATLAS geometry
                m_switches.addPlatesToCell, // add front/end plates to cell volume
                nModulesInSection[ii],   // 0-64 modules
                zShiftInSection[ii]);    // Z-shift

        (*m_log) << MSG::DEBUG << "Get an Identifier for the region and add it to the detectorManager..." << endmsg; 
        Identifier idRegion = tileID->region_id(ii);
        descriptor->set(idRegion);
        m_detectorManager->add(descriptor); 
        m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor)); 
    }


    // Set geometry Tree Tops
    GeoVolumeCursor cursor(world);
    while(!cursor.atEnd()) {
        std::string volName = cursor.getName();
        //std::cout << "volName: " << volName << "\n";
        if(volName.compare(0,4,"Tile")==0) {
            (*m_log) << MSG::DEBUG << "Adding vol '" << volName << "' to detManager" << endmsg;
            m_detectorManager->addTreeTop(GeoPVLink(cursor.getVolume().operator->()));
        }
        cursor.next();
    }

    (*m_log) << MSG::INFO << "Tile geometry from SQLite has been created." << endmsg;

}


