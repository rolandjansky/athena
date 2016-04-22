/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_ExtraRegionSelectorFast_h
#define MuonCalib_ExtraRegionSelectorFast_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

#include "set"
#include "list"

class RegionSelectionSvc;

class TFile;
class TH1F;
class TH2F;

namespace MuonCalib {

class MuonCalibSegment;
class MuonFixedId;

class ExtraRegionSelectorFast : public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  ExtraRegionSelectorFast(const std::string &t, const std::string &n, const IInterface *p);
  inline ~ExtraRegionSelectorFast() {}
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
  //minimum and maximum number of segment hits
  int m_min_hits, m_max_hits;
  //select segment author
  std::set<int> m_track_author;
  std::vector<int> m_track_author_vec;
  int m_select_seg_author;
  //momentum cut
  float m_momentum_cut;
  //minimum number of segment hits on track
  int m_min_hits_on_track;
  //maximum number of hits not on track
  int m_max_hits_not_on_track;	
  //if set to true, an debug root file is created
  bool m_do_debug_file;
  //! pointer to region selection service
  RegionSelectionSvc *p_reg_sel_svc;
  //track author id;
  int m_track_author_id;
  //Event handler for generating ExtraEvents
  inline void process_segment(MuonCalibSegment *seg);				//debug root file
  TFile *m_debug_file;
  TH1F *m_all_p;
  TH1F *m_selected_p_by_track;
  TH1F *m_selected_p_by_segment;
  TH2F *m_n_match_vs_missing;			
  inline bool isOnTrack(const MuonCalibSegment &seg, const std::list<std::set<MuonFixedId> > &track_hits);
};

} //namespace MuonCalib

#endif
