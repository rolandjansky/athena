/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LAYERED_SPACEPOINT_STORAGE_H__
#define __LAYERED_SPACEPOINT_STORAGE_H__

#include<vector>
#include<algorithm>
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigInDetTracklet.h"

typedef struct SortedSP {
public:
SortedSP(float c, const TrigSiSpacePointBase* p) : m_coordinate(c), m_pSP(p) {};
  float m_coordinate;
  const TrigSiSpacePointBase* m_pSP;
} SORTED_SP;

typedef std::pair<std::vector<SORTED_SP>::const_iterator, std::vector<SORTED_SP>::const_iterator > SP_RANGE;

typedef class LayeredStorageBase {

  friend class TrigCombinatorialTrackFinding;

  struct compareSP {
    bool operator()(const SORTED_SP& s1, const SORTED_SP& s2) {
      return s1.m_coordinate<s2.m_coordinate;
    }
  };

 public:

 LayeredStorageBase(int l) : m_layerId(l) {
    m_refCoord[0] = 0.0;
    m_refCoord[1] = 0.0;
    m_vpSP[0].clear();
    m_vpSP[1].clear();
  }
  virtual ~LayeredStorageBase() { m_vpSP[0].clear();m_vpSP[1].clear();}

  virtual SP_RANGE getSpacePoints(const TrigInDetDoublet*, float, bool debug = false) = 0;
  virtual void addSpacePoint(const TrigSiSpacePointBase*) = 0;
 
  void sort() {
    for(int sgnId=0;sgnId<2;sgnId++) {
      if(m_vpSP[sgnId].empty()) continue;
      m_refCoord[sgnId]/=m_vpSP[sgnId].size();
      std::sort(m_vpSP[sgnId].begin(),m_vpSP[sgnId].end(), LayeredStorageBase::compareSP());
    }
  }

  inline int layerId() const {
    return m_layerId;
  }
  inline unsigned int nSpacePoints() const {
    return m_vpSP[0].size()+m_vpSP[1].size();
  }
  inline bool isEmpty() const {
    return m_vpSP[0].empty() && m_vpSP[1].empty();
  }

  inline bool checkBarcode(int b) {
    if(isEmpty()) return false;
    for(int s=0;s<2;s++) {
      for(auto sp : m_vpSP[s]) {
	if(sp.m_pSP->barCode()==b) return true;
      }
    }
    return false;
  }

 protected:

  SP_RANGE findRange(const SORTED_SP s1, const SORTED_SP s2, int sideId=1) {
    auto it1 = std::lower_bound(m_vpSP[sideId].begin(), m_vpSP[sideId].end(), s1, LayeredStorageBase::compareSP());
    auto it2 = std::upper_bound(m_vpSP[sideId].begin(), m_vpSP[sideId].end(), s2, LayeredStorageBase::compareSP());
    return SP_RANGE(it1, it2);
  } 

  int m_layerId;
  float m_refCoord[2];
  std::vector<SORTED_SP> m_vpSP[2];

} LAYERED_STORAGE_BASE;

typedef class LayeredStorageBarrel : public LayeredStorageBase {
  
 public:
 LayeredStorageBarrel(int l) : LayeredStorageBase(l) {};
  ~LayeredStorageBarrel(){};

  SP_RANGE getSpacePoints(const TrigInDetDoublet* d, float cut, bool debug) {
    float zs = d->s1().z() + d->tau()*(m_refCoord[1]-d->s1().r());//using only halfId = 1 for barrel 
    float cs = cut*d->cs();
    SORTED_SP spMin(zs-cs,NULL), spMax(zs+cs,NULL);
    if(debug) {
      std::cout<<"BR: boundaries min="<<zs-cs<<" max="<<zs+cs<<std::endl;
      std::cout<<"Spacepoint list:"<<std::endl;
      for(auto spp : m_vpSP[1]) {
	std::cout<<"coord ="<<spp.m_coordinate<<" barcode="<<spp.m_pSP->barCode()<<std::endl;
      }
    }
    return findRange(spMin,spMax);
  }

  void addSpacePoint(const TrigSiSpacePointBase* p) {
    m_refCoord[1]+=p->r();
    m_vpSP[1].push_back(SORTED_SP(p->z(), p));//using only halfId = 1 for barrel 
  }

} LAYERED_STORAGE_BARREL;

typedef class LayeredStorageEndcap : public LayeredStorageBase {

 public:
 LayeredStorageEndcap(int l) : LayeredStorageBase(l) {};
  virtual ~LayeredStorageEndcap(){};

  SP_RANGE getSpacePoints(const TrigInDetDoublet* d, float cut, bool debug) {
    float dz = d->s2().z()-d->s1().z();
    if(fabs(dz)<1.0) return SP_RANGE(m_vpSP[0].end(), m_vpSP[0].end());
    int halfId = (dz>0) ? 1 : 0;

    float rs = d->s1().r() + (m_refCoord[halfId]-d->s1().z())/d->tau();
    float cs_inv = cut*d->cs_inv();

    SORTED_SP spMin(rs-cs_inv,NULL), spMax(rs+cs_inv,NULL);

    if(debug) {
      std::cout<<"SP1 r="<<d->s1().r()<<" SP2 r="<<d->s2().r()<<std::endl;
      std::cout<<"SP1 z="<<d->s1().z()<<" SP2 z="<<d->s2().z()<<std::endl;
      std::cout<<"expected hit point r="<<rs<<std::endl;
      std::cout<<"EC: boundaries min="<<rs-cs_inv<<" max="<<rs+cs_inv<<" refCoord="<<m_refCoord[halfId]<<std::endl;
      std::cout<<"Spacepoint list:"<<std::endl;
      for(auto spp : m_vpSP[halfId]) {
	std::cout<<"coord ="<<spp.m_coordinate<<" z="<<spp.m_pSP->z()<<" barcode="<<spp.m_pSP->barCode()<<std::endl;
      }
    }
    return findRange(spMin,spMax,halfId);
  }

  void addSpacePoint(const TrigSiSpacePointBase* p) {
    int sgnId = (p->z()>0) ? 1 : 0;
    m_refCoord[sgnId]+=p->z();
    m_vpSP[sgnId].push_back(SORTED_SP(p->r(), p));
  }

} LAYERED_STORAGE_ENDCAP;

typedef class LayeredStorageHelper {
 public:
 LayeredStorageHelper(int pix1, int pix2, int sct, int max) : m_barrelPix(pix1), m_endcapPix(pix2), m_encapSct(sct), m_maxLayer(max) {}; 
  inline bool isBarrel(int l) const {
    return (l<m_endcapPix);
  }
  inline bool isPixel(int l) const {
    return (l<m_barrelPix) || ((l>=m_endcapPix) && (l<m_encapSct));
  } 
  inline bool isValid(int l) const {
    return (l>=0) && (l<m_maxLayer);
  }
  inline int getNumberOfLayers() const {
    return m_maxLayer;
  }
 protected:
  int m_barrelPix, m_endcapPix, m_encapSct, m_maxLayer;
 private:
  LayeredStorageHelper(){};
} LAYERED_STORAGE_HELPER;



#endif
