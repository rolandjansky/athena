/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibrationDbIOTool_H
#define CalibrationDbIOTool_H

class MdtIdHelper;
namespace MuonGM {
  class MuonDetectorManager;
}

// MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibrationIOTool.h"

#include "MdtCalibUtils/RtDataFromFile.h"

#include "AthenaBaseComps/AthAlgTool.h"

class RegionSelectionSvc;

namespace MuonCalib {

class CalibDbConnection;
class IRtRelation;
class SamplePoint;
class CalibHeadOperations;

class CalibrationDbIOTool : public AthAlgTool, virtual public CalibrationIOTool {
 public:
  /** constructor*/
  CalibrationDbIOTool(const std::string& t, const std::string& n, const IInterface* p);
  /** initialisation */
  StatusCode initialize();
  /** finalisation */
  StatusCode finalize();
  /** write out t0 */
  StatusCode WriteT0(MdtTubeFitContainer* t0_output, const NtupleStationId & station_id, int iov_start, int iov_end);
  /** load t0s*/
  StatusCode LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int /*iov_id*/);
  /** write rt*/
  StatusCode WriteRt(const RtCalibrationOutput *rt_relation, const IRtResolution * resolution, const NtupleStationId & station_id, int iov_start, int iov_end, bool /*real_rt*/, bool /*real resolution*/);
  /** load rts */
  StatusCode LoadRt(std::map<NtupleStationId, IRtRelation *> & rts, std::map<NtupleStationId, IRtResolution *> &res, int /*iov_id*/);	
  
 private:
  //! path to calibration directory - job option
  std::string m_calib_dir;
  //! create rt relation as lookup table if set tot true - job option
  bool m_rt_lookup;
  //! if true use validated - jo
  bool m_use_validated_t0, m_use_validated_rt;
  // url of oracle data-base
  std::string m_db_ConnectionString;
  std::string m_writer_connection_string;
  std::string m_db_WorkingSchema;
  //! account data
  std::string m_reader_account, m_reader_password;
  std::string m_writer_account, m_writer_password;
  //username and password - job options
  int m_max_rt_points;
  int m_headid;
  std::string m_sitename;
  /** pointer to region selection service */
  RegionSelectionSvc *p_reg_sel_svc;
  std::vector<MuonCalib::NtupleStationId> region_ids;
  /** pointer to db connection */
  CalibDbConnection * m_connection;
  CalibDbConnection * m_data_connections[2];
  CalibHeadOperations * m_head_ops;
  /** convert rt and resolution to points */
  void fillRtPoints(const IRtRelation*  rt_relation, std::vector<SamplePoint> & points);
  void fillResPoints(const IRtResolution *rt_resolution, std::vector<SamplePoint> &  points);
  CalibDbConnection * get_connection(int write);
};

}
#endif
