/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_MuonCalibTrackSegmentBranch_h
#define MuonCalib_MuonCalibTrackSegmentBranch_h

#include "TTree.h"

#include <vector>
#include <string>

namespace MuonCalib {

class MuonCalibTrackSegmentBranch {
 public:
  MuonCalibTrackSegmentBranch(std::string branchName = "trkSeg_");
  bool  fillBranch(const std::vector<unsigned int> &seg_index, const int track_index);
  bool  createBranch(TTree* tree);
  void reset() { m_index = 0; } 
//		void reset() { m_seg_index.clear(); m_trk_index.clear(); } 
  int getBranchEntries() { return m_index; }
 private:
  std::string m_branchName;
  static const  int s_blockSize=3000;
  Int_t m_index;
  UShort_t m_seg_index[s_blockSize];
  UShort_t m_trk_index[s_blockSize];
//		std::vector< std::vector< UShort_t > > m_seg_index;
//		std::vector< UShort_t > m_trk_index;
};

}  // namespace MuonCalib
#endif
