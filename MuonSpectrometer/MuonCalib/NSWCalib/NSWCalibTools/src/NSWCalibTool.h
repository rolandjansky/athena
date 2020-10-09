/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef NSWCalibTool_h
#define NSWCalibTool_h

#include <map>
#include <string>

#include "NSWCalibTools/INSWCalibTool.h"


#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonRDO/MM_RawData.h"
#include "MuonRDO/STGC_RawData.h"

#include "TRandom3.h"
#include "TTree.h"

namespace Muon {

  class NSWCalibTool : virtual public INSWCalibTool, public AthAlgTool {

  public:

    NSWCalibTool(const std::string&, const std::string&, const IInterface*);

    virtual ~NSWCalibTool() = default;
    
    StatusCode calibrateClus(const Muon::MMPrepData* prepData, const Amg::Vector3D& globalPos, std::vector<NSWCalib::CalibratedStrip>& calibClus) const;
    StatusCode calibrateStrip(const double time,  const double charge, const double lorentzAngle, NSWCalib::CalibratedStrip& calibStrip) const;
    
    StatusCode calibrateStrip(const Muon::MM_RawData* mmRawData, NSWCalib::CalibratedStrip& calibStrip) const;
    StatusCode calibrateStrip(const Muon::STGC_RawData* sTGCRawData, NSWCalib::CalibratedStrip& calibStrip) const;

    double pdoToCharge(const int pdoCounts, const Identifier& stripID) const;
    int chargeToPdo(const float charge, const Identifier& stripID) const;

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    StatusCode mmGasProperties(float &vDrift, float &longDiff, float &transDiff, float &interactionDensityMean, float &interactionDensitySigma, TF1* &lorentzAngleFunction) const override;

  private:

    ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
    const MuonGM::MuonDetectorManager * m_muonMgr;
    
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

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
