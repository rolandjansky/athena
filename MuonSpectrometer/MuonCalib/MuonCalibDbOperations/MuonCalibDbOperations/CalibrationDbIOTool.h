/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibrationDbIOTool_H
#define CalibrationDbIOTool_H

class MdtIdHelper;
namespace MuonGM {
    class MuonDetectorManager;
}

// MuonCalibStandAloneBase
#include <array>
#include <memory>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MdtCalibUtils/RtDataFromFile.h"
#include "MuonCalibDbOperations/CalibDbConnection.h"
#include "MuonCalibDbOperations/CalibHeadOperations.h"
#include "MuonCalibMath/SamplePoint.h"
#include "MuonCalibStandAloneBase/ICalibrationIOTool.h"
class RegionSelectionSvc;

namespace MuonCalib {

    // class CalibDbConnection;
    class IRtRelation;

    class CalibrationDbIOTool : public AthAlgTool, virtual public ICalibrationIOTool {
    public:
        /** constructor*/
        CalibrationDbIOTool(const std::string &t, const std::string &n, const IInterface *p);
        virtual ~CalibrationDbIOTool();
        /** initialisation */
        StatusCode initialize() override;
        /** write out t0 */
        StatusCode WriteT0(MdtTubeFitContainer *t0_output, const NtupleStationId &station_id, int iov_start, int iov_end) override;
        /** load t0s*/
        StatusCode LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int /*iov_id*/) override;
        /** write rt*/
        StatusCode WriteRt(const RtCalibrationOutput *rt_relation, std::shared_ptr<const IRtResolution> resolution,
                           const NtupleStationId &station_id, int iov_start, int iov_end, bool /*real_rt*/,
                           bool /*real resolution*/) override;
        /** load rts */
        StatusCode LoadRt(std::map<NtupleStationId, IRtRelation *> &rts, std::map<NtupleStationId, IRtResolution *> &res,
                          int /*iov_id*/) override;

    private:
        //! path to calibration directory - job option
        std::string m_calib_dir;
        //! if true use validated - jo
        bool m_use_validated_t0, m_use_validated_rt = false;
        // url of oracle data-base
        std::string m_db_ConnectionString;
        std::string m_writer_connection_string;
        std::string m_db_WorkingSchema;
        //! account data
        std::string m_reader_account, m_reader_password;
        std::string m_writer_account, m_writer_password;
        // username and password - job options
        int m_max_rt_points;
        int m_headid;
        std::string m_sitename;
        /** pointer to region selection service */
        RegionSelectionSvc *p_reg_sel_svc = nullptr;
        std::vector<MuonCalib::NtupleStationId> m_region_ids;
        /** pointer to db connection */
        std::unique_ptr<CalibDbConnection> m_connection;
        std::array<std::unique_ptr<CalibDbConnection>, 2> m_data_connections;
        std::unique_ptr<CalibHeadOperations> m_head_ops;
        /** convert rt and resolution to points */
        void fillRtPoints(const std::shared_ptr<const IRtRelation>& rt_relation, std::vector<SamplePoint> &points);
        void fillResPoints(const std::shared_ptr<const IRtResolution>& rt_resolution, std::vector<SamplePoint> &points);
        CalibDbConnection *get_connection(int write);
    };

}  // namespace MuonCalib
#endif
