/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_CSC_DIGITIZER_H
#define MUONDIGITIZATION_CSC_DIGITIZER_H

#include <TString.h>  // for Form

#include <algorithm>
#include <cassert>
#include <map>
#include <vector>

#include "CLHEP/Random/RandomEngine.h"
#include "CscCalibTools/ICscCalibTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonSimEvent/CSCSimHit.h"
#include "MuonSimEvent/CscHitIdHelper.h"

// Author: Ketevi A. Assamagan
// BNL, October 17 2003

// Digitization class for CSC hits

class DigitData {
public:
    double driftTime0;
    double driftTime;
    double stripCharge;
};

class CSC_Digitizer {
public:
    // full constructor
    CSC_Digitizer(CscHitIdHelper* cscHitHelper, const MuonGM::MuonDetectorManager* muonMgr, ICscCalibTool* pcalib);

    // Destructor
    ~CSC_Digitizer() = default;
    // Public methods

    // initialize random number generators
    StatusCode initialize();

    // set the digitization time window
    void setWindow(const double t1, const double t2);

    void set(const double bunchTime);

    void setAmplification(const double amplification) { m_amplification = amplification; }

    void setDebug(int debug) { m_debug = debug; }

    void setDriftVelocity(double v0) { m_driftVelocity = v0; }
    void setElectronEnergy(double e) { m_electronEnergy = e; }  // eV
    void setNInterFixed() { m_NInterFromEnergyLoss = false; }

    // digitize a single hit
    StatusCode digitize_hit(const CSCSimHit* cscHit, std::vector<IdentifierHash>& hashVec,
                            std::map<IdentifierHash, std::pair<double, double> >& data_map, CLHEP::HepRandomEngine* rndmEngine);
    StatusCode digitize_hit(const CSCSimHit* cscHit, std::vector<IdentifierHash>& hashVec,
                            std::map<IdentifierHash, std::vector<float> >& data_SampleMap,
                            std::map<IdentifierHash, std::vector<float> >& data_SampleMapOddPhase, CLHEP::HepRandomEngine* rndmEngine);
    StatusCode digitize_hit(const CSCSimHit* cscHit, std::vector<IdentifierHash>& hashVec,
                            std::map<IdentifierHash, std::vector<float> >& data_SampleMap, CLHEP::HepRandomEngine* rndmEngine);

    // input parameters should be as form of cscHelper returned value...
    IdentifierHash getHashId(const Identifier& input_id, const int stripId, const int measuresPhi) const;

private:
    // private methods need to compute the induce charge on the strip
    double qWire(const int& nElectrons, const double& gammaDist) const;
    double qStripR(const double x) const;
    double qStripR(const double x, const Identifier& id) const;
    double qStripPhi(const double x, const Identifier& id) const;
    double fparamPhi(const double x, const std::array<double, 9>& p) const;
    double getDriftTime(const MuonGM::CscReadoutElement* descriptor, const Amg::Vector3D& pos) const;

    bool outsideWindow(double time) const;  // default +-50...
    void fillMaps(const IdentifierHash hash, const double driftTime, const double stripCharge, std::vector<IdentifierHash>& hashVec,
                  std::map<IdentifierHash, std::pair<double, double> >& data_map);
    void fillSampleMaps(const IdentifierHash hash, const double driftTime, const double stripCharge, std::vector<IdentifierHash>& hashVec,
                        std::map<IdentifierHash, std::vector<float> >& data_map,
                        bool phase = 0);  // new interface trying to provide 4 samples instead of q,t

    Identifier to_identifier(const CSCSimHit* cscHit) const;

    // private data members
    CscHitIdHelper* m_cscHitHelper{nullptr};
    const MuonGM::MuonDetectorManager* m_muonMgr{nullptr};  // cannot use ReadCondHandleKey since no athena component
    const CscIdHelper* m_cscIdHelper{nullptr};
    ICscCalibTool* m_pcalib{nullptr};

    static constexpr int s_maxElectron{21};  // max electron per interaction :not configurable
    bool m_NInterFromEnergyLoss{true};
    double m_electronEnergy{66.};  // unit is eV

    double m_Polia{0.};
    std::array<double, s_maxElectron> m_sprob{0.};
    double m_timeWindowLowerOffset{0.};
    double m_timeWindowUpperOffset{0.};
    double m_bunchTime{0.};
    double m_amplification{0.};
    double m_driftVelocity{60.};  // (1e-6 * 1e9); // 6 cm/microsecond -> mm/ns // 0.06
    int m_debug{0};
    /// Cache the csc id dictionary
    std::map<char, int> m_stationDict{};
};

#endif
