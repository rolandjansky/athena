/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_ExtraRegionSelector_h
#define MuonCalib_ExtraRegionSelector_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

//MuonCalibExtraTreeEvent
#include "MuonCalibExtraTreeEvent/EventHandler.h"

class RegionSelectionSvc;

class TFile;
class TH1F;

namespace MuonCalib {

class MuonCalibSegment;

class ExtraRegionSelector : public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  ExtraRegionSelector(const std::string & t, const std::string & n, const IInterface *p);
  inline ~ExtraRegionSelector() {}
//=========================public member functions==============================
  //initialize and finalize
  StatusCode initialize(void);
  StatusCode finalize(void);
  //load event
  void prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments);
 private:
//=========================private data=========================================
  //if set to true, segments wich are not completely in the selected calibration region will be dropped - job options
  bool m_exclusive_segments;
  //if true select muonboy tracks, else moore
  bool m_muonboy;
  //minimum and maximum number of segment hits
  int m_min_hits, m_max_hits;
  //select segment author
  int m_select_seg_author;
  //momentum cut
  float m_momentum_cut;
  //if set to true, an debug root file is created
  bool m_do_debug_file;
  //! pointer to region selection service
  RegionSelectionSvc *p_reg_sel_svc;
  //track author id;
  int m_track_author_id;
  //Event handler for generating ExtraEvents
  EventHandler m_evt_handler;
  inline void process_segment(MuonCalibSegment *seg);				//debug root file
  TFile *m_debug_file;
  TH1F *m_all_p;
  TH1F *m_selected_p_by_track;
  TH1F *m_selected_p_by_segment;
  
};

} //namespace MuonCalib

#endif
