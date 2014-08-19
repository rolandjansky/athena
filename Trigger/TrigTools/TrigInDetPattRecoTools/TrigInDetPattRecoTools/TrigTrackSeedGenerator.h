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
    bool operator()(const float& z, const TrigSiSpacePointBase* const& p) const {
      return z < p->z();
    }
  };

  struct smallerThanZ {
    bool operator()(const TrigSiSpacePointBase* const& p, const float& z) const {
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

InternalSoA(int max_size) : m_maxSize(max_size) {
  m_sp = new const TrigSiSpacePointBase* [m_maxSize];
  m_r = new double[m_maxSize];
  m_u = new double[m_maxSize];
  m_v = new double[m_maxSize];
  m_t = new double[m_maxSize];
  m_tCov = new double[m_maxSize];
}

  ~InternalSoA() {
    delete[] m_sp;
    delete[] m_r;
    delete[] m_u;
    delete[] m_v;
    delete[] m_t;
    delete[] m_tCov;
  }

  int m_maxSize;
  const TrigSiSpacePointBase** m_sp;
  double* m_r;
  double* m_u;
  double* m_v;
  double* m_t;
  double* m_tCov;

} INTERNAL_SOA;


typedef std::multimap<float, TrigInDetTriplet*, std::greater<float> > INTERNAL_TRIPLET_BUFFER;

typedef class TrigTrackSeedGenerator {

 public:

  TrigTrackSeedGenerator(const TrigCombinatorialSettings&);
  ~TrigTrackSeedGenerator(); 

  void loadSpacePoints(const std::vector<TrigSiSpacePointBase>&);
  void createSeeds();
  void getSeeds(std::vector<TrigInDetTriplet*>&);

private:

  void createTriplets(const TrigSiSpacePointBase*, int, int, INTERNAL_TRIPLET_BUFFER&);
  void storeTriplets(INTERNAL_TRIPLET_BUFFER&);

  const TrigCombinatorialSettings& m_settings;
  int m_maxSoaSize;
  double m_maxOuterRadius;
  double m_minRadius, m_maxRadius, m_radBinWidth;
  int m_nMaxPhiSlice;
  double m_phiSliceWidth;
  int m_nMaxRadBin;
  double m_minDeltaRadius, m_maxDeltaRadius, m_zTol;
  unsigned int m_maxTripletBufferLength;
  double m_maxD0_mixed;

  std::vector<std::vector<const TrigSiSpacePointBase*> > m_radLayers;
  std::vector<std::vector<const TrigSiSpacePointBase*> > m_phiSlices;

  PHI_R_STORAGE* m_pStore;

  INTERNAL_SOA* m_pSoA;

  double m_CovMS, m_ptCoeff, m_minPt2;

  INTERNAL_TRIPLET_BUFFER m_triplets;

} TRIG_TRACK_SEED_GENERATOR;

#endif
