/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TileGeoSectionBuilder.h
 *
 * @brief Definition of TileGeoSectionBuilder class
 *
 * @author Vakho Tsulaia
 *
 */
#ifndef TILEGEOMODEL_TILEGEOSECTIONBUILDER_H
#define TILEGEOMODEL_TILEGEOSECTIONBUILDER_H

#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

// Definition for regions
#define TILE_REGION_CENTRAL 1
#define TILE_REGION_EXTENDED 2
#define TILE_REGION_GAP 3
#define TILE_REGION_MBSCIN 4

class MsgStream;
class GeoPhysVol;
class TileDddbManager;
class TileDetDescriptor;
class TileDetDescrManager;
class GeoMaterial;
class GeoTrd;

class TileGeoSectionBuilder   
{
 public:
  /** Constructor */
  TileGeoSectionBuilder(DataHandle<StoredMaterialManager> & matManager,
                        TileDddbManager* pDbManager,
                        int ushape,
                        int glue,
                        MsgStream * log);

  /** Destructor */
  ~TileGeoSectionBuilder();

  /** Section parameters are the following:
      @param sec_number Number of sector
      @param tile_rmax  Maximal radius 
      @param rminb      Minimal radius
      @param dzglue     Glue gap along dZ
      @param delta_phi  Delta Phi
      @param ModuleNcp  Module index
      @param zlen_itc2  Length for ITC2 only
      @param neg        Used to apply extre transformation to cuts on EBC modules
  */
  void fillSection(GeoPhysVol*&             mother,
                   int                      sec_number,
                   double                   tile_rmax,
                   double                   rminb,
                   double                   dzglue,
                   double                   delta_phi,
                   int                      ModuleNcp = 0,
                   double                   zlen_itc2 = 0.,
		   bool                     neg = false);

  /** Girder parameters are the following:
      @param mother     Pointer to mother volume
      @param tile_rmax  Radius from TILE
      @param tilb_rmax  Radius from TILB 
      @param delta_ph   Delta phi      
      @param thickness  Thickness of Girder 
  */
  void fillGirder(GeoPhysVol*&             mother,
                  double                   tile_rmax,
                  double                   tilb_rmax,
                  double                   tan_delta_phi_2,
                  double                   thickness);

  /** Finger parameters are the following:
      @param mother     Pointer to mother volume
      @param tile_rmax  Radius from TILE
      @param tilb_rmax  Radius from TILB 
      @param delta_ph   Delta phi      
      @param ModuleNcp  Module index
      @param corrected_dz  dZ correction
  */
  void fillFinger(GeoPhysVol*&             mother,
		  int                      sec_number,
                  double                   tile_rmax,
                  double                   tilb_rmax,
                  double                   delta_phi_not_used,
                  int                      ModuleNcp = 0,                  
                  double                   corrected_dz = 0.);

  /** Period parameters are the following:
      @param mother      Pointer to mother volume
      @param thickness   Thickness of period
      @param dzglue      Glue thickness in z direction
      @param delta_ph    Delta phi      
      @param period_type Type of period

  */
  void fillPeriod(GeoPhysVol*&              mother,
                  double                    thickness,
                  double                    dzglue,
                  double                    tan_delta_phi_2,
                  int                       period_type,
                  GeoTrd *                  period=0);

  /** Readout Descriptor parameters are the following:
      @param descriptor  Pointer to descriptor volume
      @param detector    Detector index
      @param side        Side index
      @param testbeam    1 for test beam geometry
      @param addPlates   1 for adding plate
      @param nphi        Phi numbers
      @param zshift      Shift in Z direction [mm]
  */
  void fillDescriptor(TileDetDescriptor*&   descriptor,
		      unsigned int          detector,
		      int                   side,
		      bool                  testbeam,
		      bool                  addPlates,
                      unsigned int          nphi,
                      float                 zshift); // in mm

  /** Cell dimension parameters are the following:
      @param manager    Pointer to TileDetDescrManager
      @param detector   Detector index
      @param addPlates  1 for adding plate
      @param zShiftPos  Positiv shift in Z direction [mm] 
      @param zShiftNeg  Negative shift in Z direction [mm]
  */
  void computeCellDim(TileDetDescrManager*& manager,
		      int                   detector,
                      bool                  addPlates,
                      float                 zShiftPos, // in mm
                      float                 zShiftNeg);// in mm

  /** Calculator of Z pozition given sample in region (detector):
      @param zshift   Z shift
      @param zcenter  Z central position
      @param dz       dZ length
  */
  void calculateZ(int detector,
                  int side,
                  int sample,
                  float zshift,
                  float& zcenter,
                  float& dz);

  /** calculateEta function calculates are the following parameters
      given sample in region (detector)
      @param etamin  Eta minimal
      @param etamax  Eta maximal
      @param deta    Eta delta
      @param neta    Eta number
  */
  void calculateEta(int detector,
		    int side,
		    int sample,
		    float& etamin,
		    float& etamax,
		    float& deta,
		    unsigned int& neta);
 
  /** Function for checking empty volumes:
      @param VolumeName  The volume name
      @param print       printig ON/OFF
      @param level       volume level and printig level
      @param XYZ         checking variables
  */
  void checking(std::string VolumeName, bool print, int level,
                double X1, double X2, double Y1, double Y2, double Z); 

  // These methods should be used when section contents are not built
  // They initialize private data members used for computing cell dimensions
  void setBarrelPeriodThickness(double val);
  void setBarrelGlue(double val);
  void setExtendedPeriodThickness(double val);

 private:

  DataHandle<StoredMaterialManager>     m_theMaterialManager;
  TileDddbManager *                     m_dbManager;
  MsgStream *                           m_log;

  int          m_Ushape;
  int          m_Glue;
  double       m_barrelPeriodThickness;
  double       m_barrelGlue;
  double       m_extendedPeriodThickness;

  /** Flag for activation verbose level for debugging */
  bool         m_verbose;

  GeoMaterial* m_matLArServices;
  GeoMaterial* m_matIronHalfDens;

  /** Makes iron layer a little bit wider to obtain same sampling fraction for simulation without a glue*/
  double       m_AdditionalIronLayer;

  void checktransfunc(double absorber, double period, int np, double center);
  void printdouble(const char * name, double val);

};

#endif // not TILEGEOMODEL_TILEGEOSECTIONBUILDER_H
