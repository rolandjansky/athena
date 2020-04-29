/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef NSWCalibTool_h
#define NSWCalibTool_h

#include "NSWCalibTools/INSWCalibTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonRDO/MM_RawData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

#include "TRandom3.h"
#include "TTree.h"
#include "TF1.h"
#include <vector>

namespace Muon {

  class NSWCalibTool : virtual public INSWCalibTool, public AthAlgTool {

  public:

    NSWCalibTool(const std::string&, const std::string&, const IInterface*);

    virtual ~NSWCalibTool() = default;
    
    StatusCode calibrate( const Muon::MM_RawData* mmRawData, const Amg::Vector3D& globalPos, NSWCalib::CalibratedStrip& calibStrip) const;

    virtual StatusCode initialize();
    virtual StatusCode finalize();

  private:

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj"};

    TF1* m_lorentzAngleFunction;
     
    float m_vDrift;
    float m_timeRes;
    float m_longDiff;
    float m_transDiff;
    float m_ionUncertainty;
    double m_timeOffset;
  };
  
}

#endif
