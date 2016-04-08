/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibTrackSegmentBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

namespace MuonCalib {

MuonCalibTrackSegmentBranch::MuonCalibTrackSegmentBranch(std::string branchName):
  m_branchName(branchName),
  index(0) {
  for(int i=0; i<blockSize; i++) {
    seg_index[i]=-1;
    trk_index[i]=-1;
  }
}
	
bool MuonCalibTrackSegmentBranch::fillBranch(const std::vector<unsigned int> &seg_idx, const int track_index) {
  for(std::vector<unsigned int>::const_iterator it=seg_idx.begin(); it!=seg_idx.end(); it++) {
    if(index>=blockSize) return false;
    seg_index[index]=*it;
    trk_index[index]=track_index;
    index++;
  }
  return true;
}  // end MuonCalibTrackSegmentBranch::fillBranch

/*bool  MuonCalibTrackSegmentBranch::fillBranch(const std::vector<unsigned int> &seg_idx, const int track_index) {
  m_trk_index.push_back(track_index);
  std::vector< UShort_t > id(seg_idx.size());
  for(unsigned int i=0; i<seg_idx.size(); i++) {
  id[i]=seg_idx[i];
  }
  m_seg_index.push_back(id);
  return true;
}*/
	
bool  MuonCalibTrackSegmentBranch::createBranch(TTree* tree) {
  if(!tree) return false;
  NtupleBranchCreator branchCreator(m_branchName);
  std::string index_name ="nTrkSegs";
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );
  branchCreator.createBranch( tree, index_name, &index, "/I");
  branchCreator.createBranch( tree, "trkIndex",  &trk_index,  array_size + "/s" );
  branchCreator.createBranch( tree, "segIndex",  &seg_index,  array_size + "/s" );
  reset();
  return true;
}  // end MuonCalibTrackSegmentBranch::createBranch

/*bool  MuonCalibTrackSegmentBranch::createBranch(TTree* tree) {
  if(!tree) return false;
  tree->Branch((m_branchName + "segIndex").c_str(), "std::vector< std::vector< UShort_t > >", &m_seg_index);
  tree->Branch((m_branchName + "trkIndex").c_str(), "std::vector< UShort_t >", &m_trk_index);
}*/

}  //namespace MuonCalib
