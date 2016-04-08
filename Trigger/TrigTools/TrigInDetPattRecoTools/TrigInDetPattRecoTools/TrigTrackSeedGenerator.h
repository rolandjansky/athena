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

typedef struct LPhiSector {

public:
  
  struct compareZ {
    bool operator()(const TrigSiSpacePointBase* p1, const TrigSiSpacePointBase* p2) {
      return p1->z()<p2->z();
    }
  };

  struct compareR {
    bool operator()(const TrigSiSpacePointBase* p1, const TrigSiSpacePointBase* p2) {
      return p1->r()<p2->r();
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

  struct greaterThanR {
    bool operator()(float r, const TrigSiSpacePointBase* const& p) const {
      return r < p->r();
    }
  };

  struct smallerThanR {
    bool operator()(const TrigSiSpacePointBase* const& p, float r) const {
      return p->r() < r;
    }
  };

  //LPhiSector() : m_nSP(0) {m_phiSlices.clear();}
LPhiSector(int nPhiSlices) : m_nSP(0) {
  std::vector<const TrigSiSpacePointBase*> d;
  m_phiSlices.resize(nPhiSlices, d);
}

LPhiSector(const LPhiSector& ps) : m_nSP(ps.m_nSP), m_phiSlices(ps.m_phiSlices) {};

  const LPhiSector& operator = (const LPhiSector& ps) {
    m_nSP = ps.m_nSP;
    m_phiSlices = ps.m_phiSlices;
    return *this;
  }

  void reset() {
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it = m_phiSlices.begin();it!=m_phiSlices.end();++it) {
      (*it).clear();
    }
    m_nSP = 0;
  }

  void addSpacePoint(int phiIndex, const TrigSiSpacePointBase* p) {
    m_nSP++;
    m_phiSlices[phiIndex].push_back(p);
  }

  void sortSpacePoints(bool isBarrel) {
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it=m_phiSlices.begin();it!=m_phiSlices.end();++it) {
      if((*it).empty()) continue;
      if(isBarrel) std::sort(it->begin(), it->end(), compareZ());
      else std::sort(it->begin(), it->end(), compareR());
    }
    
  }

  int size() const { return m_nSP; }
  
  int m_nSP;
  std::vector<std::vector<const TrigSiSpacePointBase*> > m_phiSlices;

private:
  LPhiSector() {};

} L_PHI_SECTOR;

typedef struct LPhi_Storage {

public:

  LPhi_Storage(int nPhiSectors, int nLayers) {
    m_layers.reserve(nLayers);
    for(int i = 0;i<nLayers;i++) m_layers.push_back(L_PHI_SECTOR(nPhiSectors));
  }

  void addSpacePoint(int phiIdx, int layerId, const TrigSiSpacePointBase* p) {
    m_layers[layerId].addSpacePoint(phiIdx, p);
  }

  void reset() {
    for(std::vector<L_PHI_SECTOR>::iterator it=m_layers.begin();it!=m_layers.end();++it) {
      if((*it).m_nSP==0) continue;
      (*it).reset();
    }
  }
   
  void sortSpacePoints(const std::vector<TRIG_INDET_SI_LAYER>& layerGeometry) {
    int layerId = 0;
    for(std::vector<L_PHI_SECTOR>::iterator it=m_layers.begin();it!=m_layers.end();++it,layerId++) {
      if((*it).m_nSP==0) continue;
      (*it).sortSpacePoints(layerGeometry[layerId].m_type==0);
    }
  }
  std::vector<L_PHI_SECTOR> m_layers;

} L_PHI_STORAGE;

typedef struct InternalSoA {

public:

InternalSoA() : m_spi(0), m_spo(0), m_r(0), m_u(0), m_v(0), m_t(0), m_tCov(0){ }

  ~InternalSoA() {
  }

  void clear() {
    delete[] m_spi;
    delete[] m_spo;
    delete[] m_r;
    delete[] m_u;
    delete[] m_v;
    delete[] m_t;
    delete[] m_tCov;
    m_spi = 0;
    m_spo = 0;
    m_r = 0;
    m_u = 0;
    m_v = 0;
    m_t = 0;
    m_tCov = 0;
  }
  
  void resize(const int spSize) {
    /*
    m_spi.resize(spSize,0);
    m_spo.resize(spSize,0);
    m_r.resize(spSize);
    m_u.resize(spSize);
    m_v.resize(spSize);
    m_t.resize(spSize);
    m_tCov.resize(spSize);
    */

    m_spi = new const TrigSiSpacePointBase*[spSize];
    m_spo = new const TrigSiSpacePointBase*[spSize];
    m_r = new double[spSize];
    m_u = new double[spSize];
    m_v = new double[spSize];
    m_t = new double[spSize];
    m_tCov = new double[spSize];

  }
  /*
  void reserveSpacePoints(const int spSize) {
    m_spi.reserve(spSize);
    m_spo.reserve(spSize);
  }

  void resizeComponents() {
    size_t size = m_spi.size() + m_spo.size();
    m_r.resize(size);
    m_u.resize(size);
    m_v.resize(size);
    m_t.resize(size);
    m_tCov.resize(size);
  }

  void clear() {
    m_spi.clear();
    m_spo.clear();
    m_r.clear();
    m_u.clear();
    m_v.clear();
    m_t.clear();
    m_tCov.clear();
  }
  */

  /*
  std::vector<const TrigSiSpacePointBase*> m_spi;
  std::vector<const TrigSiSpacePointBase*> m_spo;
  std::vector<double> m_r;
  std::vector<double> m_u;
  std::vector<double> m_v;
  std::vector<double> m_t;
  std::vector<double> m_tCov;
  */

  const TrigSiSpacePointBase** m_spi;
  const TrigSiSpacePointBase** m_spo;
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
  //double m_minRadius, m_maxRadius, m_radBinWidth;
  double m_phiSliceWidth;
  //int m_nMaxRadBin;
  double m_minDeltaRadius, m_maxDeltaRadius, m_zTol;

  L_PHI_STORAGE* m_pStore;

  INTERNAL_SOA m_SoA;

  double m_CovMS, m_minR_squ;

  INTERNAL_TRIPLET_BUFFER m_triplets;

} TRIG_TRACK_SEED_GENERATOR;

#endif
