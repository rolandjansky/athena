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
#include "MuonRDO/STGC_RawData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

#include "TRandom3.h"
#include "TTree.h"
#include "TF1.h"

#include <string>
#include <vector>

namespace Muon {

  class NSWCalibTool : virtual public INSWCalibTool, public AthAlgTool {

  public:

    NSWCalibTool(const std::string&, const std::string&, const IInterface*);

    virtual ~NSWCalibTool() = default;
    
    virtual StatusCode calibrateClus(const Muon::MMPrepData* prepData, const Amg::Vector3D& globalPos, std::vector<NSWCalib::CalibratedStrip>& calibClus) const override;
    virtual StatusCode calibrateStrip(const double time, const double charge, const double lorentzAngle, NSWCalib::CalibratedStrip& calibStrip) const override;
    virtual StatusCode calibrateStrip(const Muon::MM_RawData* mmRawData, NSWCalib::CalibratedStrip& calibStrip) const override;
    virtual StatusCode calibrateStrip(const Muon::STGC_RawData* sTGCRawData, NSWCalib::CalibratedStrip& calibStrip) const override;

    double pdoToCharge(const int pdoCounts, const Identifier& stripID) const;
    int chargeToPdo(const float charge, const Identifier& stripID) const;

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    StatusCode mmGasProperties(float &vDrift, float &longDiff, float &transDiff, float &interactionDensityMean, float &interactionDensitySigma, TF1* &lorentzAngleFunction) const override;

  private:

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj"};
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 

    StatusCode initializeGasProperties();

    TF1* m_lorentzAngleFunction;
    
    float m_vDrift;
    float m_timeRes;
    float m_longDiff;
    float m_transDiff;
    float m_interactionDensitySigma;
    float m_interactionDensityMean;
    float m_ionUncertainty;
    double m_timeOffset;

    std::string m_gasMixture;
  };


}  // namespace Muon

#endif
