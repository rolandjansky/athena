/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_MuonCalibDefaultCalibrationSource
#define MuonCalib_MuonCalibDefaultCalibrationSource

//this
#include "MuonCalibDbOperations/IMuonCalibConditionsSource.h"

//athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//c - c++
#include <vector>
#include <string>
#include <map>

class RegionSelectionSvc;

namespace MuonCalib {

//MuonCalibStandAloneBase
class RegionSelectorBase;

class SamplePoint;
class MuonCalibDefaultCalibrationSource: public AthAlgTool, virtual public IMuonCalibConditionsSource {
 public:
//===============================destructor -- constructor======================
  /** constructor*/
  MuonCalibDefaultCalibrationSource(const std::string& t, const std::string& n, const IInterface* p);
//===============================AlgTool interface =============================
  /** initialize */
  StatusCode initialize();
 private:
//===============================job options====================================
  //regions and t0 values
  std::vector<std::string> m_t0_region_str;
  std::vector<float> m_t0;
  std::vector<bool> m_time_slewing_applied_t0;
  std::vector<bool> m_bfield_applied_t0;
  //regions and rt
  std::vector<std::string> m_rt_region_str;
  std::vector<std::string> m_rt_files;
  std::vector<bool> m_time_slewing_applied_rt;
  std::vector<bool> m_bfield_applied_rt;
//==============================private data members============================
  //creation flags
  std::vector<unsigned int> m_creation_flags_t0;
  std::vector<unsigned int> m_creation_flags_rt;
  //region classes
  std::vector<RegionSelectorBase *> m_t0_regions, m_rt_regions;
  //rtt point vector
  std::vector<std::map<int, SamplePoint> > m_rt_points;
  //region selection service - copy part of the calibration
  const RegionSelectionSvc *p_reg_sel_svc;
//===============================private functions==============================
  //initialize regions
  StatusCode initialize_regions(const std::vector<std::string> &reg_str, std::vector<RegionSelectorBase *> & reg);
  void initialize_creation_flags(const std::vector<bool> & ts_applied, const std::vector<bool> & bf_applied, unsigned int n_regions, std::vector<unsigned int> & flags);
  inline StatusCode load_rt_files();
  inline bool store_t0_fun();
  inline bool store_rt_fun();
 protected:
//===============================IMuonCalibConditionsSource interface ==========
  /** insert calibration */
  bool insert_calibration(bool store_t0, bool store_rt);
};

}//namespace MuonCalib

#endif
