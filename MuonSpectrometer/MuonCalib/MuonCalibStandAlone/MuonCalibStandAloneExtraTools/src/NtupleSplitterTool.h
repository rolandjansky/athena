/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _NtupleSplitterTool_H
#define _NtupleSplitterTool_H

//c - c++
#include "map"

//Gaudi
#include "GaudiKernel/ToolHandle.h" 
#include "AthenaBaseComps/AthAlgTool.h"
//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"

//root
class TFile;

//MuonCalibStandAloneBase
class RegionSelectionSvc;

namespace MuonCalib {

class MuonCalibBranchNtuple;
class MuonCalibTruthCollection;
class MuonCalibRawMdtHit;
class MuonCalibRawRpcHit;
class MuonCalibRawTgcHit;
class MuonCalibMdtTruthHit;
class MuonCalibRpcTruthHit;
class MuonCalibTgcTruthHit;
class MuonCalibCscTruthHit;
class MuonCalibHit_EBranch;
class MuonCalibTrack_EBranch;
class MuonCalibEvent_E;

/** @class NtupleSplitterTool
Scans a run and checks for present chambers and number of hits
@author rauscher@cern
*/

class NtupleSplitterTool: public AthAlgTool, virtual public NtupleCalibrationTool {
 public:
//==============================================================================
  /** Tool Constructor*/
  NtupleSplitterTool(const std::string &t, const std::string &n, const IInterface *p);
  /** tool initialization */
  StatusCode initialize();
  /** tool finalization */
  StatusCode finalize();
  /**analyse event - fill hits into specra*/
  StatusCode handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
  /** we dont need to keep the segments*/
  bool KeepSegments() const {
    return m_calib_tool_handle->KeepSegments();
  }
  inline StatusCode analyseSegments(const std::vector<MuonCalibSegment *> &segments) {
    return  m_calib_tool_handle->analyseSegments(segments);
  }		
  inline const IMdtCalibrationOutput* getResults() const {
    return m_calib_tool_handle->getResults();
  }
  inline const IRtResolution* getResolution() const {
    return m_calib_tool_handle->getResolution();
  }
		
//==============================================================================
 private:
  /** vector with ntuples*/
  std::map<NtupleStationId, MuonCalibBranchNtuple *> m_ntuples;
  std::map<NtupleStationId, MuonCalibHit_EBranch *> m_track_hit_branch;
  std::map<NtupleStationId, MuonCalibTrack_EBranch *> m_track_branch;
  /** file split by level: <1 do not split file
      1 split by station
      2 split by station and phi
      >2 split by station and phi and eta*/
  int m_split_level;
  /** ntuple split level - minimum is split by station*/
  int m_ntuple_split_level;
  /** create new files after given number of patterns */
  int m_max_patterns;
  /** store truth information - jo*/
  bool m_store_truth;
  /** store rawdata - jo */
  bool m_store_mdt_rawdata;
  /** store rpc rawdata */
  bool m_store_rpc_rawdata;
  bool m_store_tgc_rawdata;
  /** store rpc data only for stations with segments */
  bool m_trigger_only_segment_stations;
  /** store raw trigger data */
  bool m_store_raw_triger;	
  /** fill track branches - job-option */
  std::vector<int> m_track_authors;
  std::set<int> m_track_authors_set;
  bool m_all_fill_tracks;
  double m_track_p_cut;
  int m_adc_cut;	
  /** filename prefix **/
  std::string m_filename_prefix;
  /** calibration tool to run while splitting */
  ToolHandle<NtupleCalibrationTool> m_calib_tool_handle;
  //book keeping
  std::map<NtupleStationId, MuonCalibBranchNtuple *> m_truth_ntuples;
  std::set<NtupleStationId> m_ntuples_initialized;
  //rawdata collections
  std::map<NtupleStationId, MuonCalibRawHitCollection *> m_rawdata_collections;
  //stations with segment hits
  std::set<NtupleStationId> m_stations_with_seg_hits;	
  /** current file number and number of stored patterns */
  // calibration region //
  RegionSelectionSvc *p_reg_sel_svc;
  int m_file_number;
  int m_stored_patterns;
  inline MuonCalibBranchNtuple* get_ntuple(const NtupleStationId &id);
  inline NtupleStationId ntuple_region_id(const MuonFixedId &id);
  inline void add_truth(const MuonCalibTruthCollection *truth, MuonCalibBranchNtuple *ntpl);
  inline MuonCalibBranchNtuple* get_ntuple_for_truth(const NtupleStationId &id, const MuonCalibEvent &event);
		
  template<typename T, typename Tit>
    void process_rawdata(Tit  begin, Tit  end);
  inline bool rawdata_selector(const MuonCalibRawMdtHit &hit);
  inline bool rawdata_selector(const MuonCalibRawRpcHit &hit);
  inline bool rawdata_selector(const MuonCalibRawTgcHit &hit);
		
  template<typename Tit>
    void fill_truth(Tit begin, Tit end, const MuonCalibEvent &event);
  inline void fill_truth_hit(const MuonCalibMdtTruthHit &it, MuonCalibBranchNtuple *ntpl);
  inline void fill_truth_hit(const MuonCalibRpcTruthHit &it, MuonCalibBranchNtuple *ntpl);
  inline void fill_truth_hit(const MuonCalibTgcTruthHit &it, MuonCalibBranchNtuple *ntpl);
  inline void fill_truth_hit(const MuonCalibCscTruthHit &it, MuonCalibBranchNtuple *ntpl);
  inline void fillTrackNtuple(const MuonCalibEvent_E &extra_event);
};

}  //namespace MuonCalib

#endif
