/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TileDetectorFactoryLite.h
 *
 * @class TileDetectorFactoryLite
 *
 * @brief Definition of the TileDetectorFactoryLite class. 
 * The TileDetectorFactoryLite is invoked by the TileDetectorTool when the GeoModel 
 * description of Tile calorimeter is built from the SQLite database
 * It also builds readout geometry (coming soon).
 *
 * @author Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch> , Aug 2021
 *
 * Updates: 
 * - Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>, Oct 2021
 * - Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>, Dec 2021
 *   Moved code to use Switches, for instance for 'addPlatesToCell' or 'testBeam' 
 * - Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>, Jan 2022
 *   Added the build of the Readout geometry
 */

#ifndef TileDetectorFactoryLite_h
#define TileDetectorFactoryLite_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileGeoModel/TileSwitches.h"

// FWD Declarations
class MsgStream;
class StoreGateSvc;
class IRDBAccessSvc;
namespace GeoModelIO {
      class ReadGeoModel;
}


class TileDetectorFactoryLite : public GeoVDetectorFactory
{
public:
  
  /** Constructor */
  TileDetectorFactoryLite(StoreGateSvc *pDetStore, 
                          TileDetDescrManager *manager, 
                          GeoModelIO::ReadGeoModel* sqliteReader,
                          IRDBAccessSvc* rdbaccess,
                          const TileSwitches & switches,
                          MsgStream *log, 
                          bool fullGeo);
  
  /** Destructor */
  ~TileDetectorFactoryLite();
  
  /** Creation of Tile geometry */
  virtual void create(GeoPhysVol *world);

  /** Access function to TileDetDescr geometry data */
  virtual const TileDetDescrManager * getDetectorManager() const { return m_detectorManager; }


private:  
  
  /** Detector pointer to Store Gate service */
  StoreGateSvc              *m_detectorStore;

  /** Detector pointer to TileDetDescrManager */  
  TileDetDescrManager       *m_detectorManager;

  /** Pointer to an instance of the GeoModel I/O class */
  GeoModelIO::ReadGeoModel* m_sqliteReader;

  /** Pointer to an instance of the RDBAccessSvc to get parameters from the SQLite file */
  IRDBAccessSvc* m_paramsSvc;

  /** all switches */
  TileSwitches m_switches;
  
  /** Get message SVC */
  MsgStream                 *m_log;

  /** Flag for activation verbose level for debugging */
  bool                       m_verbose;

  /** Geometry configuration: FULL, RECO */
  bool                       m_fullGeo;
};

#endif

