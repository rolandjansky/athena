/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef NSWCalibTool_h
#define NSWCalibTool_h

#include "NSWCalibTools/INSWCalibTool.h"


#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonRDO/MM_RawData.h"

#include "TRandom3.h"
#include "TTree.h"
#include "TF1.h"
#include <vector>

class MM_RawDataCollection;

namespace Muon {
  class IMuonRawDataProviderTool;

  class NSWCalibTool : virtual public INSWCalibTool, public AthAlgTool {

  public:

    NSWCalibTool(const std::string&, const std::string&, const IInterface*);

    virtual ~NSWCalibTool() = default;
    
    StatusCode calibrate( const Muon::MM_RawData* mmRawData, const Amg::Vector3D& globalPos, double& dist_drift, double& distRes_drift, double& calib_charge);
    virtual StatusCode initialize();
    virtual StatusCode finalize();

  private:

    ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
    
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;
   

    TF1* m_lorentzAngleFunction;
 
    float m_mmBFieldX;
    float m_mmBFieldY;
    float m_mmBFieldZ;
    
    float m_vDrift;
    float m_timeRes;
  };
  
}

#endif
