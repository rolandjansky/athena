/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIPLET_MERGING_H__
#define __TRIPLET_MERGING_H__

class TrigSiSpacePointBase;
class TrigInDetTriplet;
class LayeredStorageHelper;

typedef struct SpAmbiguity {
public:
SpAmbiguity(int idx, const TrigSiSpacePointBase* psp) : m_tripletIdx(idx), 
    m_pSP(psp) {};
SpAmbiguity(const SpAmbiguity& sa) : m_tripletIdx(sa.m_tripletIdx), m_pSP(sa.m_pSP) {};
SpAmbiguity() : SpAmbiguity(-1, NULL) {};

  const SpAmbiguity& operator = (const SpAmbiguity& sa) {
    m_tripletIdx = sa.m_tripletIdx;
    m_pSP = sa.m_pSP;
    return *this;
  }

  int m_tripletIdx;
  const TrigSiSpacePointBase* m_pSP;
} SP_AMBIGUITY;

typedef struct MergedTriplets {
  MergedTriplets()=delete;
  MergedTriplets(unsigned int nLayers, int tIdx, const std::shared_ptr<TrigInDetTriplet> p) : m_active(true), m_chi2(p->chi2()), m_nSP(1) {
    m_vSPA[0].resize(nLayers);
    m_vSPA[1].resize(nLayers);
    const TrigSiSpacePointBase* pSP = &p->s3();
    m_vSPA[0][pSP->layer()].m_pSP = pSP;
    m_vSPA[0][pSP->layer()].m_tripletIdx = tIdx;
  }

  MergedTriplets(const MergedTriplets& mt) : m_active(mt.m_active), m_chi2(mt.m_chi2), m_nSP(mt.m_nSP) {
    m_vSPA[0] = std::move(mt.m_vSPA[0]);
    m_vSPA[1] = std::move(mt.m_vSPA[1]);
    //std::copy(mt.m_vSPA[0].begin(), mt.m_vSPA[0].end(), std::back_inserter(m_vSPA[0]));
    //std::copy(mt.m_vSPA[1].begin(), mt.m_vSPA[1].end(), std::back_inserter(m_vSPA[1]));
  }  
  void fill_decision(std::vector<bool>& vD) {
    for(int j=0;j<2;j++) {
      for(unsigned int idx=0;idx<m_vSPA[j].size();idx++) {
        if(m_vSPA[j][idx].m_pSP==NULL) continue;
        vD[m_vSPA[j][idx].m_tripletIdx]=true;
      }
    }
  }

  friend bool operator < (const MergedTriplets& t1, const MergedTriplets& t2) {
    // Count space points
    int sp1=0, sp2=0;
    int lay1=0, lay2=0;
    for(int i=0; i<(int)t1.m_vSPA[0].size(); i++) {
      if(t1.m_vSPA[0][i].m_pSP!=NULL) {sp1++; lay1++;}
      if(t2.m_vSPA[0][i].m_pSP!=NULL) {sp2++; lay2++;}
      if(t1.m_vSPA[1][i].m_pSP!=NULL) sp1++;
      if(t2.m_vSPA[1][i].m_pSP!=NULL) sp2++;
    }
    if(!t1.m_active) lay1=0;
    if(!t2.m_active) lay2=0;
    // Evaluate bool relations
    bool isLayBetter        = (lay1             >  lay2);
    bool isLayEqual         = (lay1             == lay2);
    bool isSpBetter         = (sp1              >  sp2);
    bool isSpEqual          = (sp1              == sp2);
    bool isChi2Better       = (t1.m_chi2          <  t2.m_chi2);
    // Check if first track is better
    return (isLayBetter || (isLayEqual&&isSpBetter)|| (isLayEqual&&isSpEqual&&isChi2Better));
  }

  bool m_active;
  float m_chi2;
  int m_nSP;
  std::vector<SP_AMBIGUITY> m_vSPA[2];
} MERGED_TRIPLETS;

typedef class TripletMerger {
 public:
 TripletMerger(const LayeredStorageHelper& lsh) : m_helper(lsh) {};
  void merge(MERGED_TRIPLETS&, MERGED_TRIPLETS&);
 protected:
  const LayeredStorageHelper& m_helper;
} TRIPLET_MERGER;


#endif
