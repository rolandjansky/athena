/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtCalibInputSvc_H
#define MdtCalibInputSvc_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCalibStandAloneBase/CalibrationIOTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

#include <string>
#include <map>

namespace MuonCalib {
  class MdtStationT0Container;
  class IRtRelation;
  class BFieldCorFunc;
  class IRtResolution;
}

// interface to enable retrieving of a pointer to the singleton //
const InterfaceID IID_IMdtCalibInputSvc("MdtCalibInputSvc", 1, 0);

/** @class MdtCalibInputSvc
Athena service which read calibration from text files and sorts them by station id
@author rauscher@cern.ch
*/

class MdtCalibInputSvc: public AthService {
 public:
//=============================================================================
  /** Service constructor */
  MdtCalibInputSvc(const std::string &name, ISvcLocator *svc_locator);
  /** destructor */
  virtual ~MdtCalibInputSvc();
  static const InterfaceID &interfaceID() { return IID_IMdtCalibInputSvc; }
  /** just some crazy athena function */	
  virtual StatusCode queryInterface(const InterfaceID &riid, void **ppvUnknown);
  /** service initalizer - reads files */
  virtual StatusCode initialize();

  /** Get t0 container for Station */
  const MuonCalib::MdtStationT0Container* GetT0(const MuonCalib::NtupleStationId &id) const;
  /** Get rt relation container */
  const MuonCalib::IRtRelation* GetRtRelation(const MuonCalib::NtupleStationId &id) const;
  /** Get B field correction */
  const MuonCalib::BFieldCorFunc* GetBCorr(const MuonCalib::NtupleStationId &id) const;
  /** Get Resolution */
  const MuonCalib::IRtResolution* GetResolution(const MuonCalib::NtupleStationId &id) const;
  /** Get rt-Relation for calibration region */
  inline const MuonCalib::IRtRelation* GetRtRelation() const {
    return p_sel_region_rt;
  }
  /** Get b-field correction for calibratino region */
  inline const MuonCalib::BFieldCorFunc* GetBCorr() const {
    if(p_sel_region_b == NULL) 
      p_sel_region_b = GetBCorr(m_mean_station_id);
    return p_sel_region_b;
  }
  /** Get resolution for calibration region */
  inline const MuonCalib::IRtResolution* GetResolution() const {
    return p_sel_region_res;
  }
//==============================================================================
 private:
  //!calibration io tool to be used
  ToolHandle<MuonCalib::CalibrationIOTool> m_calib_input_tool{this,"CalibrationInputTool","MuonCalib::CalibrationDummyIOTool"};
  //! calibration data sorted by station id
  std::map<MuonCalib::NtupleStationId, MuonCalib::MdtStationT0Container *> m_t0;
  std::map<MuonCalib::NtupleStationId, MuonCalib::IRtRelation *> m_rt_relation;
  mutable std::map<MuonCalib::NtupleStationId, MuonCalib::BFieldCorFunc *> m_B_corr;
  std::map<MuonCalib::NtupleStationId, MuonCalib::IRtResolution *> m_spat_res;
  /** pointer to region selection service */
  ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
  /** create the b-field correction */
  inline bool create_b_field_correction(const MuonCalib::NtupleStationId &id) const;
  inline const MuonCalib::BFieldCorFunc* findbfieldfun(const MuonCalib::NtupleStationId &id) const;
  /** create mean rt relations, and resolutions for the selected calibration region */
  inline void create_mean_rts();
  inline StatusCode read_calib_input();
  /** rt relation - resolution - and correction function for the selected region - is average of all matching rt relations*/
  const MuonCalib::IRtRelation* p_sel_region_rt;
  mutable const MuonCalib::BFieldCorFunc *p_sel_region_b;
  const MuonCalib::IRtResolution *p_sel_region_res;
  /** station id for mean rt */
  MuonCalib::NtupleStationId m_mean_station_id;
  /** give warnings about missing calibration only once per chamber */
  mutable std::set<MuonCalib::NtupleStationId> m_t0_warned;
  mutable std::set<MuonCalib::NtupleStationId> m_rt_warned;
		
};
#endif
