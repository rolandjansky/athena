/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibrationOracleFileIOTool_H
#define CalibrationOracleFileIOTool_H


// MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibrationIOTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MdtCalibUtils/RtDataFromFile.h"

namespace MuonCalib {

class RtRelation;

class CalibrationOracleFileIOTool : public AthAlgTool, virtual public CalibrationIOTool {
 public:
  /** constructor*/
  CalibrationOracleFileIOTool(const std::string &t, const std::string &n, const IInterface *p);
  /** initialisation */
  inline StatusCode initialize() {
    return StatusCode::SUCCESS;
  }
  /** write out t0 */
  StatusCode WriteT0(MdtTubeFitContainer *t0_output, const NtupleStationId &station_id, int iov_start, int iov_end);
  /** write rt*/	
  StatusCode WriteRt(const RtCalibrationOutput *rt_relation, const IRtResolution *resolution, const NtupleStationId &station_id, 
		     int iov_start, int iov_end, bool /*real_rt*/, bool /*real_resolution*/);
 private:
  //! path to calibration directory - job option
  std::string m_calib_dir;
  //! fill rt relation
  inline bool fill_rt(RtDataFromFile::RtRelation *rt, const IRtRelation *new_rt, const MuonCalib::IRtResolution *resolut);
};

}
#endif
