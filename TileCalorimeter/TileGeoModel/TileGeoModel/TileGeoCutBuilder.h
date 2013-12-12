/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TileGeoSectionBuioder.h
 *
 * @brief Definition of TileGeoSectionBuioder class
 *
 * @author Sergey Baranov
 *
 */
#ifndef TileGeoCutBuilder_h
#define TileGeoCutBuilder_h 1

#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

class MsgStream;
class GeoPhysVol;
class TileDddbManager;
class TileDetDescriptor;
class TileDetDescrManager;


class TileGeoCutBuilder   
{
 public:
  /** Constructor */
  TileGeoCutBuilder(DataHandle<StoredMaterialManager> & matManager,
                    TileDddbManager* pDbManager,
                    MsgStream * log);

  /** Destructor */
  ~TileGeoCutBuilder();

  /** Section parameters are the following:
      @param sec_number Number of sector
      @param tile_rmax  Maximal radius 
      @param rminb      Minimal radius 
      @param dzglue     Glue gap along dZ
      @param delta_phi  Delta Phi
      @param zlen_itc2  Length for ITC2 only
  */
  void MakeCut(GeoPhysVol*&             mother,
               int                      number);

  void checking(std::string VolumeName, bool print, int level,
                double X1, double X2, double Y1, double Y2, double Z); 

 private:

  DataHandle<StoredMaterialManager>     theMaterialManager;
  TileDddbManager *                     dbManager;
  MsgStream *                           m_log;

  double       m_barrelPeriodThickness;
  double       m_barrelGlue;
  double       m_extendedPeriodThickness;

  /** Flag for activation verbose level for debugging */
  bool         m_verbose;
};

#endif

