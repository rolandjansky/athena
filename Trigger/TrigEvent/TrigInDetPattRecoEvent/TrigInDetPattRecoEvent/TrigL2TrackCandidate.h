/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGL2_TRACK_CANDIDATE_H__
#define __TRIGL2_TRACK_CANDIDATE_H__

#include <vector>
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackParameters.h"
#include <iostream>

class TrigL2TrackCandidate {
 public: 

  TrigL2TrackCandidate(unsigned int nSP=2, float d0 = 0.0, float z0 = 0.0, float phi0 = 0.0, float eta = 0.0, float pT = 0.0) : 
    m_params(TrigL2TrackParameters(d0,z0,phi0,eta,pT)){

    m_vpSpacePoints.clear();
    if(nSP!=0) m_vpSpacePoints.assign(nSP,NULL);//placeholders for seed sp1 and sp2
    for(int i=0;i<MAX_SILICON_LAYER_NUM;i++) {
      m_spIndices[i].clear();
    }
    deleted = false;
    triplet_num = 0;
  }

  ~TrigL2TrackCandidate() {};

  inline const std::vector<const TrigSiSpacePointBase*>& spacePoints() const {
    return m_vpSpacePoints;
  }

  inline void reset() {
    m_vpSpacePoints.clear();
    for(int i=0;i<MAX_SILICON_LAYER_NUM;i++) {
      m_spIndices[i].clear();
    } 
    triplet_num = 0;
    deleted = false;
  }

  inline std::vector<const TrigSiSpacePointBase*>& spacePoints() {
    return m_vpSpacePoints;
  }

  inline void addSpacePoint(const TrigSiSpacePointBase* p) {
    if(p==NULL) return;
    if(p->layer()>=MAX_SILICON_LAYER_NUM) {
      return;
    }
    m_spIndices[p->layer()].push_back(m_vpSpacePoints.size());
    m_vpSpacePoints.push_back(p);
  }

  inline const TrigSiSpacePointBase* sp(unsigned int i) const {
    if (i>=MAX_SILICON_LAYER_NUM) return NULL;
    if(m_spIndices[i].empty()) return NULL;
    return m_vpSpacePoints[(m_spIndices[i])[0]];
  }
  
  inline const TrigSiSpacePointBase* sp2(unsigned int i) const {
    if(i>=MAX_SILICON_LAYER_NUM) return NULL;
    if(m_spIndices[i].size()<2) return NULL;
    return m_vpSpacePoints[(m_spIndices[i])[1]];
  }


  inline const TrigSiSpacePointBase*& seed_sp1() {//seed SP1
    return m_vpSpacePoints[0];
  }
  
  inline const TrigSiSpacePointBase*& seed_sp2() {//seed SP2
    return m_vpSpacePoints[1];
  }

  inline const TrigSiSpacePointBase* triplet_sp3(unsigned int idx) const {//accessor for the third spacepoints;
    if(idx>=triplet_num) return NULL;
    return m_vpSpacePoints[idx+2];
  }

  inline void triplet_clear() {
    triplet_num=0;
    m_tripletParams.clear();
    m_vpSpacePoints.erase(m_vpSpacePoints.begin()+2,m_vpSpacePoints.end()); 
  }

  inline void addThirdSpacePoint(const TrigSiSpacePointBase* p, 
				 float d0 = 0.0, 
				 float z0 = 0.0,
				 float phi0 = 0.0, 
				 float eta = 0.0, 
				 float pt = 0.0, 
				 float chi2 = 1e8) {
    if(p==NULL) return;
    m_vpSpacePoints.push_back(p);
    m_tripletParams.push_back(TrigL2TrackParameters(d0,z0,phi0,eta,pt,chi2));
    triplet_num++;
  }

  //setters and getters

  inline float seed_z0() const {
    return m_params.m_z0;
  }

  inline float seed_eta() const {
    return m_params.m_eta;
  }

  inline float seed_phi() const {
    return m_params.m_phi0;
  }

  inline void seed_z0(float v) {
    m_params.m_z0 = v;
  }

  inline void seed_eta(float v) {
    m_params.m_eta = v;
  }

  inline void seed_phi(float v)  {
    m_params.m_phi0 = v;
  }

  // getter for triplet parameters

  inline TrigL2TrackParameters& tripletParameters(unsigned int idx) {
    if(idx>=triplet_num) return m_params;
    return m_tripletParams[idx];
  }

  void merge(const TrigL2TrackCandidate* p) {

    // Insert second track space points in first track

    for(int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
    
      // Add first SP in module overlap
      
      if(sp(i)==0 && p->sp(i)!=0) {
	addSpacePoint(p->sp(i));
      }
      // Add second SP in module overlap
      
      if(sp(i)!=0 && sp2(i)==0 && p->sp(i)!=0) {
	float dr = p->sp(i)->r()-sp(i)->r();
	float dz = p->sp(i)->z()-sp(i)->z();
	if(p->sp(i)->z()<0) dz=-dz;

	if(i<OffsetEndcapPixels) {
	  if(dr>0.5) {
	    addSpacePoint(p->sp(i));
	  }
	  if(dr<-0.5) {
	    m_spIndices[i].push_back(m_spIndices[i][0]);
	    m_spIndices[i][0]=m_vpSpacePoints.size();
	    m_vpSpacePoints.push_back(p->sp(i));
	  }
	}
	else {
	  if(dz>0.5) {
	    addSpacePoint(p->sp(i));
	  }
	  if(dz<-0.5) {
	    m_spIndices[i].push_back(m_spIndices[i][0]);
	    m_spIndices[i][0]=m_vpSpacePoints.size();
	    m_vpSpacePoints.push_back(p->sp(i));
	  }
	}
      }
    }
  }


  TrigL2TrackParameters m_params;

  bool deleted;
  unsigned int triplet_num;
  
 protected:

  std::vector<const TrigSiSpacePointBase*> m_vpSpacePoints;//the main storage
  //for Strategy B use case the storage is organized as follows
  // [seed_sp1 | seed_sp2 | triplet_sp3[0] | triplet_sp3[1] | triplet_sp3[2] | ...]

  std::vector<int> m_spIndices[MAX_SILICON_LAYER_NUM];//layer-based SP indices in the storage
  std::vector<TrigL2TrackParameters> m_tripletParams;

};

struct compareTrigL2TrackCandidatesPt {
  bool operator()(const TrigL2TrackCandidate* p1, const TrigL2TrackCandidate* p2) {
  return (fabs(p1->m_params.m_pT)<fabs(p2->m_params.m_pT));
  }
};

#endif
