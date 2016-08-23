/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGEOMTOOLS_TRTGEOMETRYMANAGERTOOL_H
#define INDETALIGNGEOMTOOLS_TRTGEOMETRYMANAGERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthContainers/DataVector.h"

#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModuleList.h"

/** 
  @file TRTGeometryManagerTool.h
  @class TRTGeometryManagerTool

  @brief Tool for setting up TRT alignment geometry.

  @author Daniel Kollar <daniel.kollar@cern.ch> 
  @date 22/01/2009
*/

namespace InDetDD
{
   class TRT_DetectorManager;
}

class TRT_ID;

namespace Trk
{
   class AlignPar;
   class AlignModule;
}

namespace InDet
{
   class TRTGeometryManagerTool : virtual public Trk::IGeometryManagerTool, public AthAlgTool
   {
      public:
         TRTGeometryManagerTool(const std::string& type, const std::string& name, const IInterface * parent);
         virtual ~TRTGeometryManagerTool();

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

         /** builds geometry for TRT alignment */
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

         /** builds geometry for L1 TRT alignment
           * Creates 1 module - the whole TRT */
         void buildL0();

         /** builds geometry for L1 TRT Barrel alignment
           * Creates single module for barrel */
         void buildL1Barrel();

         /** builds geometry for L1 TRT Endcap alignment
           * Creates 2 modules, one for each endcap */
         void buildL1Endcaps();

         /** builds geometry for L2 TRT Barrel alignment
           * Creates 96 modules - 3 layers with 32 modules per layer */
         void buildL2Barrel();

         /** builds geometry for L2 TRT Endcapalignment
           * Creates 80 modules - 40 rings per endcap */
         void buildL2Endcaps();

         /** builds geometry for L2 TRT Endcapalignment
           * Creates 28 modules - 14 wheels per endcap */
         void buildL2EndcapsOLD();

         /** builds geometry for L3 TRT Barrel alignment
           * Creates many modules - one module per straw */
         void buildL3Barrel();

         /** builds geometry for L3 TRT Endcap alignment
           * Creates many modules - one module per straw */
         void buildL3Endcap();

         /** sets sigmas for modules */
         void setSigmas(Trk::AlignModule * mod, DataVector<Trk::AlignPar> * modPars);

         /** check wheather module is selected for module pointer */
         bool moduleSelected(Trk::AlignModule * mod);

         /** check wheather module is selected for module id */
         bool moduleSelected(unsigned long long modId);

         /** check wheather barrel module is selected by layer and phi */
         bool moduleSelectedBarrel(unsigned int iLayer, unsigned int iPhi);

         /** check wheather we are aligning the different endcaps and sectors */
         bool moduleSelectedEndcap(unsigned int iwheel, unsigned int iphi);

         /** print basic geometry info to screen */
         void dumpGeometry();

         const InDetDD::TRT_DetectorManager * m_trtDetManager; //!< pointer to TRT detector manager

         const TRT_ID * m_idHelper;

         ToolHandle<Trk::IAlignModuleTool> m_alignModuleTool;  //!< pointer to AlignModuleTool

         Trk::AlignModuleList m_alignModuleList;
         Trk::AlignModuleList m_idHashToAlignModuleMap;

         std::vector<Trk::AlignModuleList*> m_idHashToAlignModuleMaps; 

         DataVector< DataVector<Trk::AlignPar> > * m_alignParList;
         DataVector< DataVector<Trk::AlignPar> > * m_fullAlignParList;

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

         std::vector<int> m_barrelPhiSectors;   //!< list of barrel phi sectors to align (if empty, align everything)
         std::vector<int> m_barrelLayers;       //!< list of barrel layers to align (if empty, align everything)

         int m_endcap;                          //!< endcap_id of the endcap to align (-2 or 2 for endcap A or C, 0 for both)
         std::vector<int> m_endcapPhiSectors;   //!< list of endcap phi sectors to align (if empty, align everything)
         std::vector<int> m_endcapWheels;       //!< list of endcap wheels to align (if empty, align everything)

         bool m_doModuleSelection;
         std::vector<unsigned long long> m_moduleSelection;

         bool m_dumpGeometry;
   }; // end class

} // end namespace

#endif // INDETALIGNGEOMTOOLS_TRTGEOMETRYMANAGERTOOL_H
