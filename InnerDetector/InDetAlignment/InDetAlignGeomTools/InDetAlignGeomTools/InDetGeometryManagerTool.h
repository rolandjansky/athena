/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGEOMTOOLS_INDETGEOMETRYMANAGERTOOL_H
#define INDETALIGNGEOMTOOLS_INDETGEOMETRYMANAGERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthContainers/DataVector.h"

#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModuleList.h"

/**
  @file InDetGeometryManagerTool.h
  @class InDetGeometryManagerTool

  @brief Tool for setting up Inner Detector alignment geometry.

  @author Daniel Kollar <daniel.kollar@cern.ch>
  @date 04/09/2009
*/

namespace InDetDD
{
  class SCT_DetectorManager;
  class PixelDetectorManager;
  class SiGeometryManagerTool;
}

class SCT_ID;
class PixelID;
class SiliconID;
class TRT_ID;

namespace Trk
{
  class AlignPar;
  class AlignModule;
}

namespace InDet
{
  class InDetGeometryManagerTool : virtual public Trk::IGeometryManagerTool, public AthAlgTool
  {
  public:
    InDetGeometryManagerTool(const std::string& type, const std::string& name, const IInterface * parent);
    virtual ~InDetGeometryManagerTool();

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

    /** check whether the module is Silicon only */
    bool isSiOnly(const Trk::AlignModule * mod) const;

    const InDetDD::PixelDetectorManager * m_pixelDetManager; //!< pointer to PIX detector manager
    const InDetDD::SCT_DetectorManager  * m_sctDetManager;   //!< pointer to SCT detector manager
    const InDetDD::SCT_DetectorManager  * m_siDetManager;   //!< pointer to SCT detector manager
    const InDetDD::TRT_DetectorManager  * m_trtDetManager;   //!< pointer to TRT detector manager

    const PixelID   * m_pixHelper; //!< pointer to PIX detector manager
    const SCT_ID    * m_sctHelper; //!< pointer to SCT detector manager
    const SiliconID * m_siHelper;  //!< pointer to Silicon detector manager
    const TRT_ID    * m_trtHelper; //!< pointer to TRT detector manager

    ToolHandle<Trk::IGeometryManagerTool> m_siGeoManager;    //!< pointer to Silicon geometry manager
    ToolHandle<Trk::IGeometryManagerTool> m_trtGeoManager;   //!< pointer to TRT geometry manager

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
    bool m_alignSi;
    bool m_alignTRT;

    bool m_doModuleSelection;
    std::vector<unsigned long long> m_moduleSelection;

    bool m_dumpGeometry;

  }; // end class

} // end namespace

#endif // INDETALIGNGEOMTOOLS_INDETGEOMETRYMANAGERTOOL_H
