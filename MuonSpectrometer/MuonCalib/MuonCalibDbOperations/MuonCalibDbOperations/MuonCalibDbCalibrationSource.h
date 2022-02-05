/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_MuonCalibDbCalibrationSource
#define MuonCalib_MuonCalibDbCalibrationSource

// this
#include "MuonCalibDbOperations/IConditionsStorage.h"
#include "MuonCalibDbOperations/IMuonCalibConditionsSource.h"
// athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace MuonCalib {

    class CalibDbConnection;
    class RegionSelectorBase;
    class CalibHeadOperations;

    class MuonCalibDbCalibrationSource : public AthAlgTool, virtual public IMuonCalibConditionsSource, virtual public IConditionsStorage {
    public:
        //===============================destructor -- constructor======================
        /** constructor*/
        MuonCalibDbCalibrationSource(const std::string& t, const std::string& n, const IInterface* p);

        virtual ~MuonCalibDbCalibrationSource();
        //===============================AlgTool interface =============================
        /** initialize */
        StatusCode initialize();

        //===============================IConditionsStorage interface===================
        /** call back for t0 */
        bool StoreT0Chamber(const int& chamber, const std::map<TubeId, coral::AttributeList>& rows);
        // call back for rt
        bool StoreRtChamber(const int& chamber, const std::map<int, SamplePoint>& points);

    private:
        inline const std::vector<std::string>& RowsFromMdtTubeTable() const { return m_mdt_tube_cols; }
        inline const std::vector<std::string>& RowsFromMdtTubeVTable() const { return m_mdt_tube_v_cols; }
        //===============================IMuonCalibConditionsSource interface ==========
        inline void SetIOV(int& start, int& end) const {
            if (start < 0) { start = m_iov_start; }
            if (end < 0) { end = m_iov_end; }
        }
        //===============================job options====================================
        // head id and site name - job options
        std::string m_site_name;
        int m_head_id = 0;
        // overwirte iov - job-option
        int m_iov_start = 0, m_iov_end = 0;
        // connection string for calibration db  - job option
        std::string m_calib_connection_string, m_calib_working_schema;
        // username and password for calib-db connection
        std::string m_username, m_password;
        // calibratino region, for which the calibration is copied
        std::string m_region_str;
        // drift time offsets per radius - job options
        std::vector<float> m_drift_time_offsets;
        // store t0 or rt
        bool m_store_t0, m_store_rt;
        // global t0 offset - added to the t0 values
        float m_t0_offset;
        // creation flags - job options
        bool m_time_slewing_applied, m_b_field_correction_applied;
        unsigned int m_creation_flags;
        //===============================private data==================================
        // selected columns
        std::vector<std::string> m_mdt_tube_cols, m_mdt_tube_v_cols;
        std::unique_ptr<CalibDbConnection> m_connection;
        std::unique_ptr<CalibDbConnection> m_data_connection;
        std::unique_ptr<CalibHeadOperations> m_head_ops;
        std::unique_ptr<RegionSelectorBase> m_region;

    protected:
        //===============================IMuonCalibConditionsSource interface ==========
        /** insert calibration */
        bool insert_calibration(bool store_t0, bool store_rt);
    };

}  // namespace MuonCalib

#endif
