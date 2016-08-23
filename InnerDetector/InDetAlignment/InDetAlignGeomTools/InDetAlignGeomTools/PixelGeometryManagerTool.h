/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGEOMTOOLS_PIXELGEOMETRYMANAGERTOOL_H
#define INDETALIGNGEOMTOOLS_PIXELGEOMETRYMANAGERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthContainers/DataVector.h"

#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModuleList.h"

/**
  @file PixelGeometryManagerTool.h
  @class PixelGeometryManagerTool

  @brief Tool for setting up Pixel alignment geometry.

  @author Vicente Lacuesta <Vicente.Lacuesta.Miquel@cern.ch>
  @author Daniel Kollar <daniel.kollar@cern.ch>
  @date 28/08/2009
*/

namespace InDetDD
{
  class PixelDetectorManager;
}

class PixelID;

namespace Trk
{
  class AlignPar;
  class AlignModule;
}

class IRDBAccessSvc;
class IGeoModelSvc;

namespace InDet
{
  class PixelGeometryManagerTool : virtual public Trk::IGeometryManagerTool, public AthAlgTool
  {

  public:
    PixelGeometryManagerTool(const std::string& type, const std::string& name, const IInterface * parent);
    virtual ~PixelGeometryManagerTool();

    StatusCode initialize();
    StatusCode finalize();

    /** read the geometry
     * Method is called from the main AlignAlg to build the geometry
     * based on the requested level of alignment. It calls the other
     * build() methods based on that. */
    int ReadGeometry(int solveLevel);

    // the following two methods need to be overloaded but we don't need them (for the moment)
    void PrintModPositions() {};
    void StoreModPositions(std::string) {};

    /** builds geometry for Silicon alignment */
    void buildGeometry();

    /** adds alignment parameters for the module
     * checks for active parameters and calls setSigmas() */
    void addModuleParameters(Trk::AlignModule * module, DataVector< DataVector<Trk::AlignPar> > * allFullModPars, DataVector< DataVector<Trk::AlignPar> > * allActiveModPars);

    /** check whether the alignment level is correct */
    bool checkAlignLevel();

    /** check whether the alignment level for the barrel is correct */
    bool checkAlignLevelBarrel();

    /** check whether the alignment level for the endcaps is correct */
    bool checkAlignLevelEndcaps();

    void setAlignLevel(int level);

   /** get sipeta value from geometry DB */
    int getNoSkipEtaValueFromGeometry() const;


  private:
      
    /** eta correction value needed when IBL is present*/
    int etaCorrection;
    
    /** creates L1 AlignModules for Pixel : full Pixel*/
    void buildL1();

    /** creates L11 IBL as 1 module and old-pixel as 1 module **/
    void buildL11();
    
    /** creates L12 AlignModules for Pixel Barrel : whole barrel as 1 module */
    void buildL12Barrel();

    /** creates L12 AlignModules for Pixel Barrel : 2 endcaps */
    void buildL12Endcaps();

    /** creates L15 AlignModules for Pixel Barrel : 2 half-shells */
    void buildL15Barrel();

    /** creates L16 AlignModules for Pixel Barrel :  IBL staves , remainder of barrel 1 Module*/
    void buildL16Barrel();

    /** creates L2 AlignModules for Pixel Barrel : 3 layers*/
    void buildL2Barrel();

    /** creates L2 AlignModules for Pixel Endcaps : 2 x 3 disks */
    void buildL2Endcaps();

    /** creates L22 AlignModules for Pixel Barrel : 6 half-layers */
    void buildL22Barrel();

    /** creates L26 AlignModules for Pixel Barrel : IBL staves remainder at L2 */
    void buildL26Barrel();

    /** creates L27 AlignModules for Pixel Barrel : 112 staves (ladders) */
    void buildL27Barrel();

    /** creates L3 AlignModules for Pixel Barrel : module (wafer) level */
    void buildL3Barrel();

    /** creates L3 AlignModules for Pixel Endcaps : module (wafer) level */
    void buildL3Endcaps();
    
    /** creates L1 AlignModules for Pixel DBM : 2 endcaps */
    void buildL1DBM();

    /** creates L2 AlignModules for Pixel DBM : 8 telescopes */
    void buildL2DBM();
    
    /** creates L2 AlignModules for Pixel DBM : 24 modules */
    void buildL3DBM();
    
    /** sets sigmas for modules */
    void setSigmas(Trk::AlignModule * mod, DataVector<Trk::AlignPar> * modPars);

    /** check wheather module is selected for module pointer */
    bool moduleSelected(Trk::AlignModule * mod);

    /** check wheather module is selected for module id */
    bool moduleSelected(unsigned long long modId);

    /** print basic geometry info to screen */
    void dumpGeometry();

    /** Checks whether the given phi module in a given barrel layer belongs to top or bottom half */
    bool isTopHalf(unsigned int phi, unsigned int layer) const;

    void printTransform(const Amg::Transform3D & tr) const;

    const InDetDD::PixelDetectorManager * m_detManager; //!< pointer to PIX detector manager

    const PixelID * m_idHelper; //!< pointer to PIX detector manager

    ToolHandle<Trk::IAlignModuleTool> m_alignModuleTool;  //!< pointer to AlignModuleTool

    Trk::AlignModuleList m_alignModuleList;
    Trk::AlignModuleList m_idHashToAlignModuleMap;

    std::vector<Trk::AlignModuleList *> m_idHashToAlignModuleMaps; 

    DataVector< DataVector<Trk::AlignPar> > * m_alignParList;
    DataVector< DataVector<Trk::AlignPar> > * m_fullAlignParList;

    ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
    ServiceHandle< IGeoModelSvc > m_geoModelSvc;

    bool m_doModuleSelection;
    std::vector<unsigned long long> m_moduleSelection;

    bool m_alignBarrel;
    bool m_alignEndcaps;

    bool m_alignBarrelX;
    bool m_alignBarrelY;
    bool m_alignBarrelZ;
    bool m_alignBarrelRotX;
    bool m_alignBarrelRotY;
    bool m_alignBarrelRotZ;
    bool m_alignBarrelBowX;


    bool m_alignEndcapX;
    bool m_alignEndcapY;
    bool m_alignEndcapZ;
    bool m_alignEndcapRotX;
    bool m_alignEndcapRotY;
    bool m_alignEndcapRotZ;

    double m_sigmaBarrelX;
    double m_sigmaBarrelY;
    double m_sigmaBarrelZ;
    double m_sigmaBarrelRotX;
    double m_sigmaBarrelRotY;
    double m_sigmaBarrelRotZ;
    double m_sigmaBarrelBowX;

    double m_sigmaEndcapX;
    double m_sigmaEndcapY;
    double m_sigmaEndcapZ;
    double m_sigmaEndcapRotX;
    double m_sigmaEndcapRotY;
    double m_sigmaEndcapRotZ;

    double m_softcutBarrelX;
    double m_softcutBarrelY;
    double m_softcutBarrelZ;
    double m_softcutBarrelRotX;
    double m_softcutBarrelRotY;
    double m_softcutBarrelRotZ;
    double m_softcutBarrelBowX;

    double m_softcutEndcapX;
    double m_softcutEndcapY;
    double m_softcutEndcapZ;
    double m_softcutEndcapRotX;
    double m_softcutEndcapRotY;
    double m_softcutEndcapRotZ;

    bool m_dumpGeometry;
    bool m_alignDBM;
    
  }; // end class

} // end namespace

#endif // INDETALIGNGEOMTOOLS_PIXELGEOMETRYMANAGERTOOL_H

