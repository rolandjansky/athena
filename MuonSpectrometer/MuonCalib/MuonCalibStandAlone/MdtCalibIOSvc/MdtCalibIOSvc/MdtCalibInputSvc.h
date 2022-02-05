/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtCalibInputSvc_H
#define MdtCalibInputSvc_H

#include <map>
#include <string>

#include "AthenaBaseComps/AthService.h"
#include "CxxUtils/CachedUniquePtr.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MdtCalibData/BFieldCorFunc.h"
#include "MuonCalibStandAloneBase/ICalibrationIOTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"
namespace MuonCalib {
    class MdtStationT0Container;
    class IRtRelation;
    class IRtResolution;
}  // namespace MuonCalib

/** @class MdtCalibInputSvc
Athena service which read calibration from text files and sorts them by station id
@author rauscher@cern.ch
*/

class MdtCalibInputSvc : public AthService {
public:
    //=============================================================================
    /** Service constructor */
    MdtCalibInputSvc(const std::string &name, ISvcLocator *svc_locator);
    /** destructor */
    virtual ~MdtCalibInputSvc();
    static const InterfaceID &interfaceID() {
        // interface to enable retrieving of a pointer to the singleton //
        static const InterfaceID IID_IMdtCalibInputSvc("MdtCalibInputSvc", 1, 0);
        return IID_IMdtCalibInputSvc;
    }
    /** just some crazy athena function */
    StatusCode queryInterface(const InterfaceID &riid, void **ppvUnknown) override;
    /** service initalizer - reads files */
    StatusCode initialize() override;

    /** Get t0 container for Station */
    const MuonCalib::MdtStationT0Container *GetT0(const MuonCalib::NtupleStationId &id) const;
    /** Get rt relation container */
    const MuonCalib::IRtRelation *GetRtRelation(const MuonCalib::NtupleStationId &id) const;
    /** Get B field correction */
    const MuonCalib::BFieldCorFunc *GetBCorr(const MuonCalib::NtupleStationId &id) const;
    /** Get Resolution */
    const MuonCalib::IRtResolution *GetResolution(const MuonCalib::NtupleStationId &id) const;
    /** Get rt-Relation for calibration region */
    const MuonCalib::IRtRelation *GetRtRelation() const { return m_sel_region_rt; }
    /** Get b-field correction for calibratino region */
    const MuonCalib::BFieldCorFunc *GetBCorr() const {
        if (!m_sel_region_b) m_sel_region_b.store(std::unique_ptr<const MuonCalib::BFieldCorFunc>{GetBCorr(m_mean_station_id)});
        return m_sel_region_b.get();
    }
    /** Get resolution for calibration region */
    const MuonCalib::IRtResolution *GetResolution() const { return m_sel_region_res; }
    //==============================================================================
private:
    //! calibration io tool to be used
    ToolHandle<MuonCalib::ICalibrationIOTool> m_calib_input_tool{this, "CalibrationInputTool", "MuonCalib::CalibrationDummyIOTool"};
    //! calibration data sorted by station id
    std::map<MuonCalib::NtupleStationId, MuonCalib::MdtStationT0Container *> m_t0;
    std::map<MuonCalib::NtupleStationId, MuonCalib::IRtRelation *> m_rt_relation;
    mutable std::map<MuonCalib::NtupleStationId, MuonCalib::BFieldCorFunc *> m_B_corr;
    std::map<MuonCalib::NtupleStationId, MuonCalib::IRtResolution *> m_spat_res;
    /** pointer to region selection service */
    ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
    /** create the b-field correction */
    bool create_b_field_correction(const MuonCalib::NtupleStationId &id) const;
    const MuonCalib::BFieldCorFunc *findbfieldfun(const MuonCalib::NtupleStationId &id) const;
    /** create mean rt relations, and resolutions for the selected calibration region */
    void create_mean_rts();
    StatusCode read_calib_input();
    /** rt relation - resolution - and correction function for the selected region - is average of all matching rt relations*/
    const MuonCalib::IRtRelation *m_sel_region_rt;
    mutable CxxUtils::CachedUniquePtr<const MuonCalib::BFieldCorFunc> m_sel_region_b;
    const MuonCalib::IRtResolution *m_sel_region_res;
    /** station id for mean rt */
    MuonCalib::NtupleStationId m_mean_station_id;
    /** give warnings about missing calibration only once per chamber */
    mutable std::set<MuonCalib::NtupleStationId> m_t0_warned;
    mutable std::set<MuonCalib::NtupleStationId> m_rt_warned;
};
#endif
