/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TileAtlasFactory.h
 *
 * @brief Definition of TileAtlasFactory class
 *
 * @author Sergey Baranov & Vakho Tsulaia
 *
 */

#ifndef TileAtlasFactory_h
#define TileAtlasFactory_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "TileDetDescr/TileDetDescrManager.h"

class MsgStream;
class StoreGateSvc;

class TileAtlasFactory : public GeoVDetectorFactory
{
public:
  
  /** Constructor */
  TileAtlasFactory(StoreGateSvc *pDetStore, TileDetDescrManager *manager, 
                   bool addPlates, int Ushape, int glue, MsgStream *log, bool fullGeo);
  
  /** Destructor */
  ~TileAtlasFactory();
  
  /** Creation of Tile geometry */
  virtual void create(GeoPhysVol *world);

  /** Access function to TileDetDescr geometry data */
  virtual const TileDetDescrManager * getDetectorManager() const { return m_detectorManager; }

  /** Function for checking empty volumes:
      @param VolumeName  The volume name
      @param print       printig ON/OFF
      @param level       volume level and printig level
      @param X1,X2,Y1,Y2,DZ - checking variables
  */
  void checking(std::string VolumeName, bool print, int level,
                double X1, double X2, double Y1, double Y2, double Z); 

private:  
  
  /** Detector pointer to Store Gate service */
  StoreGateSvc              *m_detectorStore;

  /** Detector pointer to TileDetDescrManager */  
  TileDetDescrManager       *m_detectorManager;

  /** Get message SVC */
  MsgStream                 *m_log;

  /** Add plates to cell volume */
  bool                       m_addPlatesToCellVolume;

  /** U-shape version used */
  int                        m_Ushape;

  /** 0: glue layer is removed and replaced by air, 
      1: simulation with glue, 
      2: glue is replaced by air + width of iron is modified in order to get the same sampling fraction      */
  int                        m_Glue;

  /** Flag for using test beam geometry */
  bool                       m_testbeamGeometry; 

  /** Flag for activation verbose level for debugging */
  bool                       m_verbose;

  /** Geometry configuration: FULL, RECO */
  bool                       m_fullGeo;
};

#endif

