/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_SegmentRecalibration_h
#define MuonCalib_SegmentRecalibration_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

class MdtCalibInputSvc;
namespace MuonCalib {
class IMdtSlewCorFunc;

class SegmentRecalibration : public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  SegmentRecalibration(const std::string &t, const std::string  &n, const IInterface *p);
  inline ~SegmentRecalibration() {}
//=========================public member functions==============================
  //initialize and finalize
  StatusCode initialize(void);
  inline StatusCode finalize(void) {
    return StatusCode :: SUCCESS;
  }
  //load event
  void prepareSegments(const MuonCalibEvent *& /*event*/, std::map<NtupleStationId, MuonCalibSegment *> &segments);
 private:
//=========================private data=========================================
  //! switches for tube by tube correction
  //! LEAVE: Take it as it is in calibraition file
  //! UNAPPLY: Add correction which is stored in the ntuple 
  //! FILE: Read correction from file or recalculate - this entails UNAPPLY
  std::string m_t0_switch;
  std::string m_B_corr_switch;
  std::string m_time_slwing_corr_switch;
  std::string m_rt_scaling_switch;
  //! numeric values for the switches
  //! 0=LEAVE
  //! 1=UNAPPLY
  //! 2=FILE
  int m_t0_numswitch, m_B_corr_numswitch, m_time_slwing_numswitch, m_rt_scaling_numswitch;
  //! switches for recalculation of r and dr OFF|ON
  bool m_recalc_r_switch;
  //! replace drift radius by true radius (mc only) - jo
  bool m_use_true_r;
  //! undo segmentwise fitted t0
  //! <0: Automatic
  //! 0: Don't
  //! 1: Do
  int m_undo_refitted_t0;
  float m_prop_factor;
  //! calibration input service
  ServiceHandle<MdtCalibInputSvc> m_calib_input_svc;
  IMdtSlewCorFunc *p_ts_corr_func;
  //convert text switches to numerical values	
  inline bool convert_switch(const std::string &s_switch, int &num_switch, const char *switchname) const;	
};	

} //namespace MuonCalib

#endif
