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
#include "MuonCondData/NswCalibDbTimeChargeData.h"

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
    
    using TimeCalibType = NswCalibDbTimeChargeData::CalibDataType;
    using TimeCalibConst = NswCalibDbTimeChargeData::CalibConstants;
    
    StatusCode calibrateClus(const EventContext& ctx, const Muon::MMPrepData* prepData, const Amg::Vector3D& globalPos, std::vector<NSWCalib::CalibratedStrip>& calibClus) const override;
    StatusCode distToTime(const EventContext& ctx, const Muon::MMPrepData* prepData, const Amg::Vector3D& globalPos, const std::vector<double>& driftDistances, std::vector<double>& driftTimes) const override;
    StatusCode calibrateStrip(const Identifier& id, const double time, const double charge, const double lorentzAngle, NSWCalib::CalibratedStrip& calibStrip) const override;
    StatusCode calibrateStrip(const EventContext& ctx, const Muon::MM_RawData* mmRawData, NSWCalib::CalibratedStrip& calibStrip) const override;
    StatusCode calibrateStrip(const EventContext& ctx, const Muon::STGC_RawData* sTGCRawData, NSWCalib::CalibratedStrip& calibStrip) const override;
    
    
    bool tdoToTime  (const EventContext& ctx, const bool inCounts, const int tdo, const Identifier& chnlId, double& time, const int relBCID) const override;
    bool timeToTdo  (const EventContext& ctx, const double time, const Identifier& chnlId, int& tdo, int& relBCID) const override;
    bool chargeToPdo(const EventContext& ctx, const double charge, const Identifier& chnlId, int& pdo) const override;
    bool pdoToCharge(const EventContext& ctx, const bool inCounts, const int pdo, const Identifier& chnlId, double& charge) const override;

    virtual StatusCode initialize() override;  

    NSWCalib::MicroMegaGas mmGasProperties() const override;

    inline double mmPeakTime  () const override {return m_mmPeakTime;  }
    inline double stgcPeakTime() const override {return m_stgcPeakTime;}

  private:
    const NswCalibDbTimeChargeData* getCalibData(const EventContext& ctx) const;
    bool loadMagneticField(const EventContext& ctx, MagField::AtlasFieldCache& fieldCache ) const;

    bool timeToTdoMM(const NswCalibDbTimeChargeData* tdoPdoData, const double time, const Identifier& chnlId, int& tdo, int& relBCID) const;
    bool timeToTdoSTGC(const NswCalibDbTimeChargeData* tdoPdoData, const double time, const Identifier& chnlId, int& tdo, int& relBCID) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj"};
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 
    SG::ReadCondHandleKey<NswCalibDbTimeChargeData> m_condTdoPdoKey {this, "condTdoPdoKey", "NswCalibDbTimeChargeData", "Key of NswCalibDbTimeChargeData object containing calibration data (TDO and PDO)"};

    Gaudi::Property<bool> m_isData{this, "isData", false, "Processing data"};

    StatusCode initializeGasProperties();
    
    Gaudi::Property<double> m_vDrift{this, "DriftVelocity",0.047, "Drift velocity"};
    Gaudi::Property<double> m_timeRes{this, "TimeResolution", 25., "Time resolution"};
    Gaudi::Property<double> m_longDiff{this, "longDiff", 0.019}; // mm/ mm
    Gaudi::Property<double> m_transDiff{this, "transDiff", 0.036};
    Gaudi::Property<double> m_ionUncertainty{this,"ionUncertainty", 4.0}; //ns
    Gaudi::Property<double> m_mmPeakTime{this, "mmPeakTime", 200.}; //ns
    Gaudi::Property<double> m_stgcPeakTime{this, "sTgcPeakTime", 0.}; // ns
    Gaudi::Property<std::string> m_gasMixture{this, "GasMixture",  "ArCo2_937"};

    // these values should go into the conditions database, but introduce them as properties for now
    // they describe the shift of the time to get it into the VMM time window
    Gaudi::Property<double> m_mmLatencyMC{this,"mmLatencyMC",25};
    Gaudi::Property<double> m_mmLatencyData{this,"mmLatencyData",50}; //this is temporary, need to align with what we find in data
    
    Gaudi::Property<double> m_stgcLatencyMC{this,"stgcLatencyMC",-50};
    Gaudi::Property<double> m_stgcLatencyData{this,"stgcLatencyData",-50}; //this is temporary, need to align with what we find in data

    double m_interactionDensitySigma{0.0F};
    double m_interactionDensityMean{0.0F};
    std::unique_ptr<TF1> m_lorentzAngleFunction{nullptr};

    bool localStripPosition(const Identifier& id, Amg::Vector2D &locPos) const;

  };


}  // namespace Muon

#endif
