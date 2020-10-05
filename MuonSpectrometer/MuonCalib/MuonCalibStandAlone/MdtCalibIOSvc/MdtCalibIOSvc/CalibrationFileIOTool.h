/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibrationFileIOTool_H
#define CalibrationFileIOTool_H


// MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibrationIOTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MdtCalibUtils/RtDataFromFile.h"

namespace MuonCalib {

class RtRelation;

class CalibrationFileIOTool : public AthAlgTool, virtual public CalibrationIOTool {
 public:
  /** constructor*/
  CalibrationFileIOTool(const std::string &t, const std::string &n, const IInterface *p);
  /** initialisation */
  inline StatusCode initialize() {
    return StatusCode::SUCCESS;
  }
  /** write out t0 */
  StatusCode WriteT0(MdtTubeFitContainer *t0_output, const NtupleStationId &station_id, int /*iov_start*/, int /*iov_end*/);
  /** write rt*/	
  StatusCode WriteRt(const RtCalibrationOutput *rt_relation, const IRtResolution *resolution, const NtupleStationId &station_id, int /*iov_start*/, int /*iov_end*/, bool /*real_rt*/, bool /*real resolution*/);
  /** load t0s*/
  StatusCode LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int /*iov_id*/);
  /** load rts */
  StatusCode LoadRt(std::map<NtupleStationId, IRtRelation *> &rts, std::map<NtupleStationId, IRtResolution *> &res, int /*iov_id*/);	
 private:
  //! path to calibration directory - job option
  std::string m_calib_dir;
  //! use fixed identifier if set tot true - job option
  bool m_use_fixed_id;
  //! create rt relation as lookup table if set tot true - job option
  bool m_rt_lookup;
  //! fill rt relation
  inline bool fill_rt(RtDataFromFile::RtRelation *rt, const IRtRelation *new_rt, const MuonCalib::IRtResolution *resolut);
  //! extract station identifier from file name
  inline bool interpret_chamber_name(const std::string &nm, const char *prefix, std::string &station, int &eta, int &phi, int &ml) const;
  //! create the rt relation and resolution
  inline  void read_rt_relation(const std::string &fname, std::map<NtupleStationId, IRtRelation *> &rts, std::map<NtupleStationId, IRtResolution *> &res,const MuonCalib::NtupleStationId &id);		
};

}
#endif
