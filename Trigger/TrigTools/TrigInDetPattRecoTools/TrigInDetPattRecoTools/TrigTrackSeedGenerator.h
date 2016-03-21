/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_TRACK_SEED_GENERATOR_H__
#define __TRIG_TRACK_SEED_GENERATOR_H__

#include<vector>
#include<algorithm>

#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"

class TrigSiSpacePointBase;
class TrigInDetTriplet;


typedef struct PhiSector {

public:
  
  struct compareZ {
    bool operator()(const TrigSiSpacePointBase* p1, const TrigSiSpacePointBase* p2) {
      return p1->z()<p2->z();
    }
  };

  struct greaterThanZ {
    bool operator()(float z, const TrigSiSpacePointBase* const& p) const {
      return z < p->z();
    }
  };

  struct smallerThanZ {
    bool operator()(const TrigSiSpacePointBase* const& p, float z) const {
      return p->z() < z;
    }
  };

PhiSector() : m_nSP(0) {m_radBins.clear();}
PhiSector(int nBins) : m_nSP(0) {
  m_radBins.resize(nBins);
}
PhiSector(const PhiSector& ps) : m_nSP(ps.m_nSP), m_radBins(ps.m_radBins) {};

  const PhiSector& operator = (const PhiSector& ps) {
    m_nSP = ps.m_nSP;
    m_radBins = ps.m_radBins;
    return *this;
  }

  void reset() {
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it=m_radBins.begin();it!=m_radBins.end();++it) {
      (*it).clear();
    }
  }

  void addSpacePoint(int rIdx, const TrigSiSpacePointBase* p) {
    m_nSP++;
    m_radBins[rIdx].push_back(p);
  }

  void sortSpacePoints() {
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it=m_radBins.begin();it!=m_radBins.end();++it) {
      if((*it).empty()) continue;
      std::sort(it->begin(), it->end(), compareZ());
    }
    
  }

  int size() const { return m_nSP; }
  
  int m_nSP;
  std::vector<std::vector<const TrigSiSpacePointBase*> > m_radBins;

} PHI_SECTOR;


typedef struct PhiR_Storage {

public:

  PhiR_Storage(int nPhiSectors, int nRBins) {
    m_phiSectors.reserve(nPhiSectors);
    for(int i = 0;i<nPhiSectors;i++) m_phiSectors.push_back(PHI_SECTOR(nRBins));
  }

  void addSpacePoint(int phiIdx, int rIdx, const TrigSiSpacePointBase* p) {
    m_phiSectors[phiIdx].addSpacePoint(rIdx, p);
  }

  void reset() {
    for(std::vector<PHI_SECTOR>::iterator it=m_phiSectors.begin();it!=m_phiSectors.end();++it) {
      if((*it).m_nSP==0) continue;
      (*it).reset();
    }
  }
   
  void sortSpacePoints() {
    for(std::vector<PHI_SECTOR>::iterator it=m_phiSectors.begin();it!=m_phiSectors.end();++it) {
      if((*it).m_nSP==0) continue;
      (*it).sortSpacePoints();
    }
  }

  std::vector<PHI_SECTOR> m_phiSectors;

} PHI_R_STORAGE;

typedef struct InternalSoA {

public:

  InternalSoA() {
  }

  ~InternalSoA() {
  }
  
  void reserveSpacePoints(const int spSize) {
    m_sp.reserve(spSize);
  }

  void resizeComponents() {
    size_t size = m_sp.size();
    m_r.resize(size);
    m_u.resize(size);
    m_v.resize(size);
    m_t.resize(size);
    m_tCov.resize(size);
  }

  void clear() {
    m_sp.clear();
    m_r.clear();
    m_u.clear();
    m_v.clear();
    m_t.clear();
    m_tCov.clear();
  }

  std::vector<const TrigSiSpacePointBase*> m_sp;
  std::vector<double> m_r;
  std::vector<double> m_u;
  std::vector<double> m_v;
  std::vector<double> m_t;
  std::vector<double> m_tCov;

} INTERNAL_SOA;


typedef std::multimap<float, TrigInDetTriplet, std::greater<float> > INTERNAL_TRIPLET_BUFFER;

typedef class TrigTrackSeedGenerator {

 public:

  TrigTrackSeedGenerator(const TrigCombinatorialSettings&);
  ~TrigTrackSeedGenerator(); 

  void loadSpacePoints(const std::vector<TrigSiSpacePointBase>&);
  void createSeeds();
  void getSeeds(std::vector<TrigInDetTriplet>&);

private:

  void createTriplets(const TrigSiSpacePointBase*, int, int, INTERNAL_TRIPLET_BUFFER&);
  void storeTriplets(INTERNAL_TRIPLET_BUFFER&);

  const TrigCombinatorialSettings& m_settings;
  int m_maxSoaSize;
  double m_maxOuterRadius;
  double m_minRadius, m_maxRadius, m_radBinWidth;
  double m_phiSliceWidth;
  int m_nMaxRadBin;
  double m_minDeltaRadius, m_maxDeltaRadius, m_zTol;

  PHI_R_STORAGE* m_pStore;

  INTERNAL_SOA m_SoA;

  double m_CovMS, m_minR_squ;

  INTERNAL_TRIPLET_BUFFER m_triplets;

} TRIG_TRACK_SEED_GENERATOR;

#endif
