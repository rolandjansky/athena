/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGEOMTOOLS_SIGEOMETRYMANAGERTOOL_H
#define INDETALIGNGEOMTOOLS_SIGEOMETRYMANAGERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthContainers/DataVector.h"

#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModuleList.h"

#include <TGeoManager.h>


/**
  @file SiGeometryManagerTool.h
  @class SiGeometryManagerTool

  @brief Tool for setting up Silicon alignment geometry.

  @author Vicente Lacuesta <Vicente.Lacuesta.Miquel@cern.ch>
  @author Daniel Kollar <daniel.kollar@cern.ch>
  @date 28/08/2009
*/

namespace InDetDD
{
  class SCT_DetectorManager;
  class PixelDetectorManager;
}

class SCT_ID;
class PixelID;
class SiliconID;

namespace Trk
{
  class AlignPar;
  class AlignModule;
}

namespace InDet
{
  class SiGeometryManagerTool : virtual public Trk::IGeometryManagerTool, public AthAlgTool
  {
  public:
    SiGeometryManagerTool(const std::string& type, const std::string& name, const IInterface * parent);
    virtual ~SiGeometryManagerTool();

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

    /** sets the output stream for the logfile */
    void setLogStream(std::ostream * os);

  private:

    /** creates L0 AlignModules for Silicon */
    void buildL0();

    /** sets sigmas for modules */
    void setSigmas(Trk::AlignModule * mod, DataVector<Trk::AlignPar> * modPars);

    /** check wheather module is selected for module pointer */
    bool moduleSelected(Trk::AlignModule * mod);

    /** check wheather module is selected for module id */
    bool moduleSelected(unsigned long long modId);

    /** print basic geometry info to screen */
    void dumpGeometry();

    /** check whether the module is of a single detector type */
    bool isOneDetOnly(const Trk::AlignModule * mod, Trk::AlignModule::DetectorType dettype) const;

    const InDetDD::PixelDetectorManager * m_pixelDetManager; //!< pointer to PIX detector manager
    const InDetDD::SCT_DetectorManager  * m_sctDetManager;   //!< pointer to SCT detector manager

    const PixelID * m_pixHelper;  //!< pointer to PIX detector manager
    const SCT_ID * m_sctHelper;   //!< pointer to SCT detector manager
    const SiliconID * m_idHelper; //!< pointer to Silicon detector manager

    ToolHandle<Trk::IGeometryManagerTool> m_pixelGeoManager; //!< pointer to Pixel geometry manager
    ToolHandle<Trk::IGeometryManagerTool> m_sctGeoManager;   //!< pointer to SCT geometry manager

    ToolHandle<Trk::IAlignModuleTool> m_alignModuleTool;     //!< pointer to AlignModuleTool
      
    Trk::AlignModuleList m_alignModuleList;
    Trk::AlignModuleList m_idHashToAlignModuleMap;
      
    std::vector<Trk::AlignModuleList *> m_idHashToAlignModuleMaps; 

    DataVector< DataVector<Trk::AlignPar> > * m_alignParList;
    DataVector< DataVector<Trk::AlignPar> > * m_fullAlignParList;

    // alignment transforms
    bool m_alignX;
    bool m_alignY;
    bool m_alignZ;
    bool m_alignRotX;
    bool m_alignRotY;
    bool m_alignRotZ;

    double m_sigmaX;
    double m_sigmaY;
    double m_sigmaZ;
    double m_sigmaRotX;
    double m_sigmaRotY;
    double m_sigmaRotZ;

    // Silicon joboptions
    bool m_alignPixel;
    bool m_alignSCT;

    bool m_doModuleSelection;
    std::vector<unsigned long long> m_moduleSelection;

    bool m_dumpGeometry;
    bool m_actualGeom;

  }; // end class

} // end namespace

#endif // INDETALIGNGEOMTOOLS_SIGEOMETRYMANAGERTOOL_H
