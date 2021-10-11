/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// this
#include "MuonCalibDbOperations/MuonCalibDbCalibrationSource.h"

#include "MuonCalibDbOperations/CalibDbConnection.h"
#include "MuonCalibDbOperations/CalibHeadOperations.h"
#include "MuonCalibDbOperations/CalibRtDbOperations.h"
#include "MuonCalibDbOperations/CalibT0DbOperations.h"
#include "MuonCalibDbOperations/CoolInserter.h"

// coral
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

// MuonCalibIdentifier//MuonCalibIdentifier
#include "MuonCalibIdentifier/MdtCalibCreationFlags.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

// MuonCalibMath
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonCalibMath/SamplePoint.h"
// MuonCalibStandALoneBase
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectorBase.h"
// c - c++
#include "exception"
#include "iostream"
#include "sstream"

namespace MuonCalib {
    MuonCalibDbCalibrationSource::~MuonCalibDbCalibrationSource() = default;
    MuonCalibDbCalibrationSource::MuonCalibDbCalibrationSource(const std::string& t, const std::string& n, const IInterface* p) :
        IMuonCalibConditionsSource(), IConditionsStorage(), AthAlgTool(t, n, p) {
        declareInterface<IMuonCalibConditionsSource>(this);
        declareProperty("SiteName", m_site_name);
        declareProperty("HeadId", m_head_id);
        declareProperty("ConnectionString", m_calib_connection_string);
        declareProperty("WorkingSchema", m_calib_working_schema);
        declareProperty("UserName", m_username);
        declareProperty("Password", m_password);
        declareProperty("Region", m_region_str);
        m_mdt_tube_cols.push_back(std::string("TUBE_ID"));
        m_mdt_tube_cols.push_back(std::string("P4"));
        m_mdt_tube_cols.push_back(std::string("VALIDFLAG"));
        m_mdt_tube_cols.push_back(std::string("ADC_1"));
        m_store_t0 = true;
        m_store_rt = true;
        declareProperty("StoreT0", m_store_t0);
        declareProperty("StoreRT", m_store_rt);
        m_t0_offset = 0.0;
        declareProperty("DriftTimeOffsetsVsR", m_drift_time_offsets);
        declareProperty("T0Offset", m_t0_offset);
        m_time_slewing_applied = false;
        declareProperty("TimeSlewingApplied", m_time_slewing_applied);
        m_b_field_correction_applied = false;
        declareProperty("BFieldCorrectionApplied", m_b_field_correction_applied);
        m_creation_flags = 0;
    }

    StatusCode MuonCalibDbCalibrationSource::initialize() {
        // process region
        m_region = RegionSelectorBase::GetRegion(m_region_str);
        if (!m_region) {
            ATH_MSG_FATAL("Error initializing RegionSelectorBase::GetRegion with region string: '" << m_region_str);
            return StatusCode::FAILURE;
        }
        // open conneciton to calibration db
        m_connection = std::make_unique<CalibDbConnection>(m_calib_connection_string, m_calib_working_schema);
        m_connection->SetLogin(m_username, m_password);
        if (!m_connection->OpenConnection()) {
            ATH_MSG_FATAL("Cannot open connection to calibration database!");
            return StatusCode::FAILURE;
        }
        m_head_ops = std::make_unique<CalibHeadOperations>(*m_connection);
        int lowrun, uprun, lowtime, uptime;
        if (!m_head_ops->GetHeadInfo(m_head_id, lowrun, uprun, lowtime, uptime)) {
            ATH_MSG_FATAL("Cannot get header " << m_head_id << "from calib datbase");
            return StatusCode::FAILURE;
        }
        m_iov_start = lowrun;
        m_iov_end = uprun;
        m_iov_end++;
        m_data_connection.reset(m_head_ops->GetDataConnection(m_head_id, false));
        if (!m_data_connection) {
            ATH_MSG_FATAL("Cannot open data connection!");
            return StatusCode::FAILURE;
        }
        if (!m_data_connection->OpenConnection()) {
            ATH_MSG_FATAL("Cannot open data connection!");
            return StatusCode::FAILURE;
        }
        m_creation_flags = 0;
        if (m_time_slewing_applied) m_creation_flags |= MuonCalib::TIME_SLEWING_CORRECTION_APPLIED;
        if (m_b_field_correction_applied) m_creation_flags |= MuonCalib::B_FIELD_CORRECTIONS_APPLIED;
        return StatusCode::SUCCESS;
    }

    bool MuonCalibDbCalibrationSource::StoreT0Chamber(const int& chamber, const std::map<TubeId, coral::AttributeList>& rows) {
        MuonFixedId id(chamber);
        if (!m_region->Result(id)) { return true; }
        NtupleStationId sid(id);
        sid.SetMultilayer(0);
        std::string data_string;
        std::ostringstream f;
        f << chamber << m_site_name << m_head_id;
        if (!m_inserter->StartT0Chamber(sid)) {
            ATH_MSG_WARNING("Cannot insert chamber " << chamber);
            return true;
        }
        for (std::map<TubeId, coral::AttributeList>::const_iterator it = rows.begin(); it != rows.end(); it++) {
            if (!m_inserter->AppendT0(it->second["P4"].data<float>() + m_t0_offset, it->second["VALIDFLAG"].data<short>(),
                                      it->second["ADC_1"].data<float>())) {
                ATH_MSG_WARNING("Wrong number of tubes in database for " << sid.regionId() << "!");
                break;
            }
        }
        m_inserter->StoreT0Chamber(sid, f.str(), m_creation_flags);
        return true;
    }

    bool MuonCalibDbCalibrationSource::StoreRtChamber(const int& chamber, const std::map<int, SamplePoint>& points) {
        MuonFixedId id(chamber);
        if (!m_region->Result(id)) { return true; }
        NtupleStationId sid(id);
        sid.SetMultilayer(0);
        std::ostringstream f;
        f << chamber << m_site_name << m_head_id;
        if (m_drift_time_offsets.size()) {
            float slice_width = 14.6 / static_cast<float>(m_drift_time_offsets.size());
            std::map<int, SamplePoint> points_cp(points);
            for (std::map<int, SamplePoint>::iterator it = points_cp.begin(); it != points_cp.end(); it++) {
                int slice_number = static_cast<int>(std::floor(it->second.x2() / slice_width));
                if (slice_number < 0) slice_number = 0;
                if (slice_number >= static_cast<int>(m_drift_time_offsets.size()))
                    slice_number = static_cast<int>(m_drift_time_offsets.size()) - 1;
                it->second.set_x1(it->second.x1() + m_drift_time_offsets[slice_number]);
            }
            m_inserter->StoreRtChamber(sid, points_cp, f.str(), m_creation_flags);
        } else {
            m_inserter->StoreRtChamber(sid, points, f.str(), m_creation_flags);
        }
        return true;
    }

    bool MuonCalibDbCalibrationSource::insert_calibration(bool store_t0, bool store_rt) {
        try {
            if (store_t0 && m_store_t0) {
                CalibT0DbOperations t0_op(*m_data_connection);
                if (!t0_op.ReadForConditions(m_site_name, m_head_id, *this)) {
                    ATH_MSG_WARNING("T0 insert failed!");
                    return false;
                }
            }
            if (store_rt && m_store_rt) {
                CalibRtDbOperations rt_op(*m_data_connection);
                if (!rt_op.ReadForConditions(m_site_name, m_head_id, *this)) {
                    ATH_MSG_WARNING("RT insert failed!");
                    return false;
                }
            }
        }  // try
        catch (const std::exception& e) {
            ATH_MSG_FATAL("Exception: " << e.what());
            return false;
        }
        return true;
    }

}  // namespace MuonCalib
