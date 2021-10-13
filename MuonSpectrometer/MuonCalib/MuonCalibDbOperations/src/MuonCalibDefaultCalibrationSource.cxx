/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// this
#include "MuonCalibDbOperations/MuonCalibDefaultCalibrationSource.h"

#include "MuonCalibDbOperations/CoolInserter.h"

// MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectorBase.h"

// MuonCalibMath
#include "MuonCalibMath/SamplePoint.h"

// MuonCalibIdentifier//MuonCalibIdentifier
#include "MuonCalibIdentifier/MdtCalibCreationFlags.h"

// c-c++
#include <fstream>
#include <sstream>

namespace MuonCalib {
    MuonCalibDefaultCalibrationSource::~MuonCalibDefaultCalibrationSource() = default;
    MuonCalibDefaultCalibrationSource::MuonCalibDefaultCalibrationSource(const std::string &t, const std::string &n, const IInterface *p) :
        AthAlgTool(t, n, p) {
        declareInterface<IMuonCalibConditionsSource>(this);
        declareProperty("T0Regions", m_t0_region_str);
        declareProperty("T0", m_t0);
        declareProperty("TimeSlewingAppliedT0", m_time_slewing_applied_t0);
        declareProperty("BFieldCorrectionAppliedT0", m_bfield_applied_t0);
        declareProperty("RtRegions", m_rt_region_str);
        declareProperty("RtFiles", m_rt_files);
        declareProperty("TimeSlewingAppliedRt", m_time_slewing_applied_rt);
        declareProperty("BFieldCorrectionAppliedRt", m_bfield_applied_rt);
    }

    StatusCode MuonCalibDefaultCalibrationSource::initialize() {
        // get region selection service
        ATH_CHECK(m_reg_sel_svc.retrieve());
        // check matching lengths
        if (m_t0_region_str.size() != m_t0.size()) {
            ATH_MSG_FATAL("Configuration error: T0Regions and T0 have different size!");
            return StatusCode::FAILURE;
        }
        if (m_rt_region_str.size() != m_rt_files.size()) {
            ATH_MSG_FATAL("Configuration error: RtRegions and RtFiles have different size!");
            return StatusCode::FAILURE;
        }
        // initialize regions
        ATH_CHECK(initialize_regions(m_t0_region_str, m_t0_regions));
        initialize_creation_flags(m_time_slewing_applied_t0, m_bfield_applied_t0, m_t0_regions.size(), m_creation_flags_t0);
        ATH_CHECK(initialize_regions(m_rt_region_str, m_rt_regions));
        initialize_creation_flags(m_time_slewing_applied_rt, m_bfield_applied_rt, m_rt_regions.size(), m_creation_flags_rt);
        // load rt files
        ATH_CHECK(load_rt_files());
        return StatusCode::SUCCESS;
    }

    bool MuonCalibDefaultCalibrationSource::insert_calibration(bool store_t0, bool store_rt) {
        if (store_t0 && !store_t0_fun()) { return false; }
        if (store_rt && !store_rt_fun()) { return false; }
        return true;
    }

    StatusCode MuonCalibDefaultCalibrationSource::initialize_regions(const std::vector<std::string> &reg_str,
                                                                     std::vector<std::unique_ptr<RegionSelectorBase> > &reg_vec) {
        for (const std::string &reg : reg_str) {
            std::unique_ptr<RegionSelectorBase> r = RegionSelectorBase::GetRegion(reg);
            if (!r) {
                ATH_MSG_FATAL("Error in region " << reg);
                return StatusCode::FAILURE;
            }
            reg_vec.emplace_back(std::move(r));
        }
        return StatusCode::SUCCESS;
    }

    void MuonCalibDefaultCalibrationSource::initialize_creation_flags(const std::vector<bool> &ts_applied,
                                                                      const std::vector<bool> &bf_applied, unsigned int n_regions,
                                                                      std::vector<unsigned int> &flags) {
        for (unsigned int i = 0; i < n_regions; i++) {
            unsigned int flag(0);
            if (i < ts_applied.size() && ts_applied[i]) { flag |= MuonCalib::TIME_SLEWING_CORRECTION_APPLIED; }
            if (i < bf_applied.size() && bf_applied[i]) { flag |= MuonCalib::B_FIELD_CORRECTIONS_APPLIED; }
            flags.push_back(flag);
        }
    }

    inline StatusCode MuonCalibDefaultCalibrationSource::load_rt_files() {
        for (std::vector<std::string>::const_iterator it = m_rt_files.begin(); it != m_rt_files.end(); it++) {
            const std::string &fname(*it);
            std::string line;
            std::ifstream rtf(fname.c_str());
            if (!rtf.good()) {
                ATH_MSG_FATAL("Cannot open rt file '" << fname << "'!");
                return StatusCode::FAILURE;
            }
            std::map<int, SamplePoint> pts;
            // read dummy line
            std::getline(rtf, line);
            int i(0);
            while (!rtf.eof()) {
                std::getline(rtf, line);
                std::istringstream ln_str(line);
                double r, t, s;
                ln_str >> r;
                ln_str >> t;
                ln_str >> s;
                if (rtf.eof()) break;
                SamplePoint pt(t, r, s);
                pts[i] = pt;
                i++;
            }
            m_rt_points.push_back(pts);
        }
        return StatusCode::SUCCESS;
    }

    inline bool MuonCalibDefaultCalibrationSource::store_t0_fun() {
        const std::vector<NtupleStationId> &regions(m_reg_sel_svc->GetStationsInRegions());
        for (std::vector<NtupleStationId>::const_iterator it = regions.begin(); it != regions.end(); it++) {
            MuonFixedId fid(it->FixedId());
            for (unsigned int i = 0; i < m_t0_regions.size(); i++) {
                if (m_t0_regions[i]->Result(fid)) {
                    m_inserter->StartT0Chamber(*it);
                    while (m_inserter->AppendT0(m_t0[i], 5, 0.0)) {}
                    m_inserter->StoreT0Chamber(*it, std::string("default"), m_creation_flags_t0[i]);
                    //				break;
                }
            }
        }
        return true;
    }

    inline bool MuonCalibDefaultCalibrationSource::store_rt_fun() {
        const std::vector<NtupleStationId> &regions(m_reg_sel_svc->GetStationsInRegions());
        for (std::vector<NtupleStationId>::const_iterator it = regions.begin(); it != regions.end(); it++) {
            MuonFixedId fid(it->FixedId());
            for (unsigned int i = 0; i < m_rt_regions.size(); i++) {
                if (m_rt_regions[i]->Result(fid)) {
                    m_inserter->StoreRtChamber(*it, m_rt_points[i], std::string("default"), m_creation_flags_rt[i]);
                    break;
                }
            }
        }
        return true;
    }

}  // namespace MuonCalib
