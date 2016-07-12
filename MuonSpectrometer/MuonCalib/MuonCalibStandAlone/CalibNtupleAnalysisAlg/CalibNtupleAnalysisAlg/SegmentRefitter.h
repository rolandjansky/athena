/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_SegmentRefitter_h
#define MuonCalib_SegmentRefitter_h

//Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

class MdtCalibInputSvc;

namespace MuonCalib {

class IMdtPatRecFitter;
class T0Refinement;

class SegmentRefitter : public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  SegmentRefitter(const std::string &t, const std::string &n, const IInterface *p);
  inline ~SegmentRefitter() {}
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
  //! if set to true, curved segments are fitted - jo
  bool m_curved;
  //! if set to true fitting with t0-refinement is activated - jo
  bool m_t0_refine;
  //! if set to true,the hit selection will be refined.
  bool m_refine_hit_selection;
  //! road width for segment fitting - jo
  double m_road_width;	
  //! abort fitting if it takes longer that m_time_out seconds
  double m_time_out;
  //! minimum and maximum number of hits after the segment refit
  int m_min_hits, m_max_hits;
  //! fitter class (no t0 refinement)
  IMdtPatRecFitter *p_qfitter;
  //! t0 refinement
  T0Refinement *p_t0_refinement;
  //! calibration input service	
  ServiceHandle<MdtCalibInputSvc> m_calib_input_svc;
  //refine t0
  inline bool refine_t0(MuonCalibSegment *&segment);
		
};

} //namespace MuonCalib
#endif
