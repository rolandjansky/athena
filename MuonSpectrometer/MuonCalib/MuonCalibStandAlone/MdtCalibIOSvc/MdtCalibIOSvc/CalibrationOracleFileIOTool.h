/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibrationOracleFileIOTool_H
#define CalibrationOracleFileIOTool_H

// MuonCalibStandAloneBase
#include "AthenaBaseComps/AthAlgTool.h"
#include "MdtCalibUtils/RtDataFromFile.h"
#include "MuonCalibStandAloneBase/ICalibrationIOTool.h"

namespace MuonCalib {

    class RtRelation;

    class CalibrationOracleFileIOTool : public AthAlgTool, virtual public ICalibrationIOTool {
    public:
        /** constructor*/
        CalibrationOracleFileIOTool(const std::string &t, const std::string &n, const IInterface *p);
        /** initialisation */
        /** write out t0 */
        StatusCode WriteT0(MdtTubeFitContainer *t0_output, const NtupleStationId &station_id, int iov_start, int iov_end) override;
        /** write rt*/
        StatusCode WriteRt(const RtCalibrationOutput *rt_relation, std::shared_ptr<const IRtResolution> resolution,
                           const NtupleStationId &station_id, int iov_start, int iov_end, bool /*real_rt*/,
                           bool /*real_resolution*/) override;

        StatusCode LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &, int) override { return StatusCode::FAILURE; }
        StatusCode LoadRt(std::map<NtupleStationId, IRtRelation *> &, std::map<NtupleStationId, IRtResolution *> &, int) override {
            return StatusCode::FAILURE;
        }

    private:
        //! path to calibration directory - job option
        std::string m_calib_dir;
        //! fill rt relation
        inline bool fill_rt(std::unique_ptr<RtDataFromFile::RtRelation> &rt, const std::shared_ptr<const IRtRelation>& new_rt,
                            const std::shared_ptr<const MuonCalib::IRtResolution>& resolut);
    };

}  // namespace MuonCalib
#endif
