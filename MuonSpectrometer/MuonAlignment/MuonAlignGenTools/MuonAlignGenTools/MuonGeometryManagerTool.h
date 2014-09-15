/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_MUONGEOMETRYMANAGERTOOL_H
#define MUONALIGNGENTOOLS_MUONGEOMETRYMANAGERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"
#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "MuonAlignGenTools/CommonGeometryManagerTool.h"
#include "TrkAlignEvent/AlignParList.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignModule.h"

#include "CLHEP/Geometry/Point3D.h"

/**
   @file MuonGeometryManagerTool.h
   @class MuonGeometryManagerTool

   @brief Tool for creating AlignModules for MDT alignment.

   @author 
     Robert Harrington <roberth@bu.edu>, Steffen Kaiser <Steffen.Kaiser@cern.ch>, Yair Mahalalel <Yair.Mahalalel@cern.ch>

   @date 10/14/2008
*/

namespace Trk {
  class IAlignModuleTool;  
}

namespace Muon {

  class MuonGeometryManagerTool : public CommonGeometryManagerTool, virtual public Trk::IGeometryManagerTool {
    
  public:
    
    MuonGeometryManagerTool(const std::string& type, const std::string& name,
			    const IInterface* parent);
    virtual ~MuonGeometryManagerTool();
    
    StatusCode initialize();
    StatusCode finalize();
    
    /** Returns number of alignment parameters */
    int ReadGeometry(int);

    void buildGeometry();

    void createAlignModuleList();

    void PrintModPositions() {}
    void StoreModPositions(std::string) {}

  protected:
    
    void addModuleParameters(Trk::AlignModule * module, 
			     DataVector<DataVector<Trk::AlignPar> >* allFullModPars, 
			     DataVector<DataVector<Trk::AlignPar> >* allActiveModPars);

  private:

    bool isOneDetOnly(const Trk::AlignModule* mod, 
		      Trk::AlignModule::DetectorType& detType) const;


    ToolHandle<Trk::IAlignModuleTool> m_alignModuleTool; //!< Pointer to AlignModuleTool

    ToolHandle<Trk::IGeometryManagerTool> m_mdtGeoManager; //!< Pointer to MdtGeometryManagerTool
    ToolHandle<Trk::IGeometryManagerTool> m_cscGeoManager; //!< Pointer to MdtGeometryManagerTool
    ToolHandle<Trk::IGeometryManagerTool> m_tgcGeoManager; //!< Pointer to MdtGeometryManagerTool


    Trk::AlignModuleList  m_alignModuleList;    

    std::vector<Trk::AlignModuleList*> m_idHashToAlignModuleMaps; // uses det element id hash

    DataVector<DataVector<Trk::AlignPar> >* m_alignParList;
    DataVector<DataVector<Trk::AlignPar> >* m_fullAlignParList;

    bool m_alignMdt;
    bool m_alignTgc;
    bool m_alignCsc;

  }; // end class
  
} // end namespace

#endif // MUONALIGNGENTOOLS_MUONGEOMETRYMANAGERTOOL_H
