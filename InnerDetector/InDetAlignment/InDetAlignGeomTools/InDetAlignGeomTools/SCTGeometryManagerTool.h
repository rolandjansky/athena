/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGEOMTOOLS_SCTGEOMETRYMANAGERTOOL_H
#define INDETALIGNGEOMTOOLS_SCTGEOMETRYMANAGERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthContainers/DataVector.h"

#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModuleList.h"

/**
  @file SCTGeometryManagerTool.h
  @class SCTGeometryManagerTool

  @brief Tool for setting up SCT alignment geometry.

  @author Vicente Lacuesta <Vicente.Lacuesta.Miquel@cern.ch>
  @author Daniel Kollar <daniel.kollar@cern.ch>
  @date 28/08/2009
*/

namespace InDetDD
{
  class SCT_DetectorManager;
}

class SCT_ID;

namespace Trk
{
  class AlignPar;
  class AlignModule;
}

namespace InDet
{
  class SCTGeometryManagerTool : virtual public Trk::IGeometryManagerTool, public AthAlgTool
  {
  public:
    SCTGeometryManagerTool(const std::string& type, const std::string& name, const IInterface * parent);
    virtual ~SCTGeometryManagerTool();

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

    /** builds geometry for SCT alignment */
    void buildGeometry();

    /** adds alignment parameters for the module
     * checks for active parameters and calls setSigmas() */
    void addModuleParameters(Trk::AlignModule * module, DataVector< DataVector<Trk::AlignPar> > * allFullModPars, DataVector< DataVector<Trk::AlignPar> > * allActiveModPars);

    /** check whether the alignment level is correct */
    bool checkAlignLevel();

    /** check whether the Barrel alignment level is correct */
    bool checkAlignLevelBarrel();

    /** check whether the Endcap alignment level is correct */
    bool checkAlignLevelEndcaps();

    void setAlignLevel(int level);


  private:

    /** creates L0 AlignModules for SCT */
    void buildL0();

    /** creates L1 AlignModules for SCT barrel */
    void buildL1Barrel();

    /** creates L1 AlignModules for SCT endcaps */
    void buildL1Endcaps();

    /** creates L2 AlignModules for SCT barrel */
    void buildL2Barrel();

    /** creates L25 AlignModules for SCT barrel */
    void buildL25Barrel();

    /** creates L27 AlignModules for SCT barrel */
    void buildL27Barrel();

    /** creates L2 AlignModules for SCT endcaps */
    void buildL2Endcaps();

    /** creates L25 AlignModules for SCT endcaps */
    void buildL25Endcaps();

    /** creates L2 AlignModules for the full Pixel in the ordering equal to the old code */
    void buildL2Old();

    /** creates L3 AlignModules for SCT barrel */
    void buildL3Barrel();

    /** creates L3 AlignModules for SCT endcaps */
    void buildL3Endcaps();

    /** sets sigmas for modules */
    void setSigmas(Trk::AlignModule * mod, DataVector<Trk::AlignPar> * modPars);

    /** check wheather module is selected for module pointer */
    bool moduleSelected(Trk::AlignModule * mod);

    /** check wheather module is selected for module id */
    bool moduleSelected(unsigned long long modId);

    /** print basic geometry info to screen */
    void dumpGeometry();

    const InDetDD::SCT_DetectorManager * m_detManager; //!< pointer to SCT detector manager

    const SCT_ID * m_idHelper; //!< pointer to SCT detector manager

    ToolHandle<Trk::IAlignModuleTool> m_alignModuleTool;  //!< pointer to AlignModuleTool

    Trk::AlignModuleList m_alignModuleList;
    Trk::AlignModuleList m_idHashToAlignModuleMap;
      
    std::vector<Trk::AlignModuleList *> m_idHashToAlignModuleMaps; 

    DataVector< DataVector<Trk::AlignPar> > * m_alignParList;
    DataVector< DataVector<Trk::AlignPar> > * m_fullAlignParList;

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

    double m_softcutEndcapX;
    double m_softcutEndcapY;
    double m_softcutEndcapZ;
    double m_softcutEndcapRotX;
    double m_softcutEndcapRotY;
    double m_softcutEndcapRotZ;

    bool m_useOldL2;
    bool m_dumpGeometry;

  }; // end class

} // end namespace

#endif // INDETALIGNGEOMTOOLS_SCTGEOMETRYMANAGERTOOL_H
