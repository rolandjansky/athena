/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_COMMONGEOMETRYMANAGERTOOL_H
#define MUONALIGNGENTOOLS_COMMONGEOMETRYMANAGERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"
#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "TrkAlignEvent/AlignParList.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignModule.h"

#include "CLHEP/Geometry/Point3D.h"

/**
   @file CommonGeometryManagerTool.h
   @class CommonGeometryManagerTool

   @brief Tool with generic methods for muon geometry manager tools.

   @author 
     Robert Harrington <roberth@cern.ch>

   @date 3/8/2011
*/


class RegionSelectionSvc;

namespace MuonGM {
  class MuonDetectorManager;
  class MuonReadoutElement;
}

namespace MuonCalib {
  class IIdToFixedIdTool;
  class MuonFixedId;
}

namespace Trk {
  class IAlignModuleTool;  
}

namespace Muon {

  class CommonGeometryManagerTool : public AthAlgTool {
    
  public:
    
    enum L1Modules { EndcapA, EndcapC, Barrel, nL1Modules };
    
    CommonGeometryManagerTool(const std::string& type, const std::string& name,
			    const IInterface* parent);
    virtual ~CommonGeometryManagerTool();
    
    StatusCode initialize();
    StatusCode finalize();
   
    void createAlignModuleList();

  protected:


    bool chamberSelected(const MuonGM::MuonReadoutElement* mre);
    bool chamberSelected(const MuonCalib::MuonFixedId& fid);
    bool moduleSelected(std::string name);
    
    void setSigmas(DataVector<Trk::AlignPar>* modPars);

    bool alignTraX() { return m_alignTraX; }
    bool alignTraY() { return m_alignTraY; }
    bool alignTraZ() { return m_alignTraZ; }
    bool alignRotX() { return m_alignRotX; }
    bool alignRotY() { return m_alignRotY; }
    bool alignRotZ() { return m_alignRotZ; }

    double sigmaTraX() { return m_sigmaTraX; }
    double sigmaTraY() { return m_sigmaTraY; }
    double sigmaTraZ() { return m_sigmaTraZ; }
    double sigmaRotX() { return m_sigmaRotX; }
    double sigmaRotY() { return m_sigmaRotY; }
    double sigmaRotZ() { return m_sigmaRotZ; }

    double softcutTraX() { return m_softcutTraX; }
    double softcutTraY() { return m_softcutTraY; }
    double softcutTraZ() { return m_softcutTraZ; }
    double softcutRotX() { return m_softcutRotX; }
    double softcutRotY() { return m_softcutRotY; }
    double softcutRotZ() { return m_softcutRotZ; }

    const MuonGM::MuonDetectorManager* p_muonMgr;
    ToolHandle<MuonCalib::IIdToFixedIdTool> m_idTool;

  private:

    const RegionSelectionSvc* m_regionSvc;

    bool m_alignTraX;
    bool m_alignTraY;
    bool m_alignTraZ;
    bool m_alignRotX;
    bool m_alignRotY;
    bool m_alignRotZ;

    double m_sigmaTraX;
    double m_sigmaTraY;
    double m_sigmaTraZ;
    double m_sigmaRotX;
    double m_sigmaRotY;
    double m_sigmaRotZ;    

    double m_softcutTraX;
    double m_softcutTraY;
    double m_softcutTraZ;
    double m_softcutRotX;
    double m_softcutRotY;
    double m_softcutRotZ;

    bool        m_doModuleSelection;
    std::vector<std::string> m_moduleSelection;

  }; // end class
  
} // end namespace

#endif // MUONALIGNGENTOOLS_COMMONGEOMETRYMANAGERTOOL_H
