/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SITRACK_TRIPLET_MERGER_H__
#define __TRIG_L2_SITRACK_TRIPLET_MERGER_H__

#include "TrigInDetEvent/TrigSiSpacePointBase.h"

class TrigL2TrackCandidate;
class TrigL2SiTrackTripletMergingCuts;

#include <algorithm>
#include <cstring>
#include <iostream>
#include <iomanip>

#define MAX_SITRACK_TRIPLET_NUM 100


class TripletTableCell {
 public:
  TripletTableCell() {
    m_spIndices[0]=0;m_spIndices[1]=0;
  }
  char operator[](unsigned int idx) const {
    if(idx>1) return 0;
    return m_spIndices[idx];
  }
  void operator()(unsigned int idx, char spIndex) {
    if(idx>1) return;
    m_spIndices[idx]=spIndex;
  }
 protected:
  char m_spIndices[2];
};

class TripletTableRow {
 public:
  friend class TripletTable;
  friend class TrigL2SiTrackTripletMerger;

  struct compareTriplets {
    bool operator()(const TripletTableRow* p1, const TripletTableRow* p2) {
      if((*p1)==(*p2)) {
	return (p1->m_chi2<p2->m_chi2);
      }
      else return (*p1)<(*p2);
    }
  };

void add(unsigned int idx, char spIdx, unsigned int layerIdx, float chi2) {
    m_tripletIdx = idx;
    memset(&m_triplets[0],0,sizeof(m_triplets));
    m_triplets[layerIdx](0,spIdx);
    m_nSP=3;//assuming that the first two are common for all the triplets
    m_nLay=3;
    m_chi2=chi2;
  }

  friend bool operator < (const TripletTableRow&, const TripletTableRow&);
  friend bool operator == (const TripletTableRow&, const TripletTableRow&);

  unsigned int m_tripletIdx;
  TripletTableCell m_triplets[MAX_SILICON_LAYER_NUM];
  int m_nSP;
  int m_nLay;
  float m_chi2;
};

class TripletTable {
 public:
  friend class TrigL2SiTrackTripletMerger;
  //  friend std::ostream& operator << (std::ostream&,TripletTable&); 

  TripletTable(TrigL2TrackCandidate& t) : m_tree(t), m_size(0) {

  }

  void addSpacePoint(const TrigSiSpacePointBase* sp ) {
    m_rows[m_size].add(m_size,m_size+2,sp->layer(),m_tree.tripletParameters(m_size).m_chi2);
    m_deleted[m_size]=false;
    m_size++;
  }

  void mergeRows(std::pair<float, std::pair<unsigned int, unsigned int> >);
  bool compareRows(unsigned int, unsigned int);

  void getMergedTriplets(std::vector<const TripletTableRow*>& rV) {
    for(unsigned int i = 0; i<m_size;i++) {
      if(m_deleted[i]) continue;
      rV.push_back(&m_rows[i]);
    }
  }

 protected:
  TrigL2TrackCandidate& m_tree;
  unsigned int m_size;
  TripletTableRow m_rows[MAX_SITRACK_TRIPLET_NUM];
  bool m_deleted[MAX_SITRACK_TRIPLET_NUM];
};
/*
std::ostream& operator << (std::ostream& o, TripletTable& t) {
  for(unsigned int i=0;i<t.m_size;i++) {
    if(t.m_deleted[i]) continue;
    for(unsigned int j=0;j<MAX_SILICON_LAYER_NUM;j++) {
      const TrigSiSpacePointBase* pSP=NULL;
      if(j==t.m_tree.seed_sp1()->layer()) {
	pSP=t.m_tree.seed_sp1();
      }
      else if(j==t.m_tree.seed_sp2()->layer()) {
	pSP=t.m_tree.seed_sp2();
      }
      else {
	if(t.m_rows[i].m_triplets[j][0]!=0) {
	  unsigned int spIdx=t.m_rows[i].m_triplets[j][0]-2;
	  pSP = t.m_tree.triplet_sp3(spIdx);
	  if(pSP->layer()!=j) {
	    std::cout<<"0: LAYER INCONSISTENT !!!"<<std::endl;
	  }
	}
	else o<<"[    ]";
      }
      if(pSP!=NULL) 
	o<<"["<<std::setw(4)<<pSP->index()<<"]";
    }
    o<<" chi2="<<t.m_rows[i].m_chi2*1000.0;
    o<<std::endl;
    
    for(unsigned int j=0;j<MAX_SILICON_LAYER_NUM;j++) {
      const TrigSiSpacePointBase* pSP=NULL;
      if(t.m_rows[i].m_triplets[j][1]!=0) {
	unsigned int spIdx=t.m_rows[i].m_triplets[j][1]-2;
	pSP = t.m_tree.triplet_sp3(spIdx); 
	if(pSP->layer()!=j) {
	  std::cout<<"1: LAYER INCONSISTENT !!!"<<std::endl;
	}
      }
      else o<<"[    ]";
      if(pSP!=NULL) 
	o<<"["<<std::setw(4)<<pSP->index()<<"]";
    }
    o<<std::endl;
    
  }
  return o;
}
*/

class TrigL2SiTrackTripletMerger {
 public:
  TrigL2SiTrackTripletMerger(const TrigL2SiTrackTripletMergingCuts& c, TrigL2TrackCandidate* p, unsigned int& trackNumber) : 
    m_cuts(c), m_full_trks(p), m_full_trks_num(trackNumber) {
    m_full_trks_num = 0;
  }
  virtual ~TrigL2SiTrackTripletMerger() {};

  bool operator()(TrigL2TrackCandidate&);
 protected:

  float tripletDistance(TrigL2TrackCandidate&, unsigned int, unsigned int);

  const TrigL2SiTrackTripletMergingCuts m_cuts;
  TrigL2TrackCandidate* m_full_trks;
  unsigned int& m_full_trks_num;

};




#endif
