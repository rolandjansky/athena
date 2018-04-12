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

  struct compareRless {
    bool operator()(const TrigSiSpacePointBase* p1, const TrigSiSpacePointBase* p2) {
      return p1->r()>p2->r();
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

  struct greaterThanR_i {
    bool operator()(const TrigSiSpacePointBase* const& p, float r) const {
      return r < p->r();
    }
  };

  struct smallerThanR {
    bool operator()(const TrigSiSpacePointBase* const& p, float r) const {
      return p->r() < r;
    }
  };

  struct smallerThanR_i {
    bool operator()(float r, const TrigSiSpacePointBase* const& p) const {
      return p->r() < r;
    }
  };

  //LPhiSector() : m_nSP(0) {m_phiSlices.clear();}
LPhiSector(int nPhiSlices) : m_nSP(0) {
  std::vector<const TrigSiSpacePointBase*> d;
  m_phiSlices.resize(nPhiSlices, d);
  m_phiThreeSlices.resize(nPhiSlices, d);
  if(nPhiSlices == 1) {//special case
    m_threeIndices[1].push_back(0);
    m_threeIndices[0].push_back(-1);
    m_threeIndices[2].push_back(-1);
  }
  if(nPhiSlices == 2) {//special case
    m_threeIndices[1].push_back(0);
    m_threeIndices[0].push_back(-1);
    m_threeIndices[2].push_back(1);
    m_threeIndices[1].push_back(1);
    m_threeIndices[0].push_back(0);
    m_threeIndices[2].push_back(-1);
  }
  if(nPhiSlices > 2) {
    for(int phiIdx=0;phiIdx<nPhiSlices;phiIdx++) {
      m_threeIndices[1].push_back(phiIdx);
      if(phiIdx>0) m_threeIndices[0].push_back(phiIdx-1);
      else m_threeIndices[0].push_back(nPhiSlices-1);
      if(phiIdx<nPhiSlices-1) m_threeIndices[2].push_back(phiIdx+1);
      else m_threeIndices[2].push_back(0);
    }
  }
  //for(int phiIdx=0;phiIdx<nPhiSlices;phiIdx++) {
  //  std::cout<<"PHI SLICE "<<phiIdx<<"[ "<<m_threeIndices[0][phiIdx]<<" "<<m_threeIndices[1][phiIdx]
  //	     <<" "<<m_threeIndices[2][phiIdx]<<"]"<<std::endl;
  //}
}

LPhiSector(const LPhiSector& ps) : m_nSP(ps.m_nSP), m_phiSlices(ps.m_phiSlices), m_phiThreeSlices(ps.m_phiThreeSlices) {
  for(int i=0;i<3;i++) {
    m_threeIndices[i] = ps.m_threeIndices[i];
  }
}

  const LPhiSector& operator = (const LPhiSector& ps) {
    m_nSP = ps.m_nSP;
    m_phiSlices = ps.m_phiSlices;
    m_phiThreeSlices = ps.m_phiThreeSlices;
    for(int i=0;i<3;i++) {
      m_threeIndices[i] = ps.m_threeIndices[i];
    }
    return *this;
  }

  void reset() {
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it = m_phiSlices.begin();it!=m_phiSlices.end();++it) {
      (*it).clear();
    }
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it = m_phiThreeSlices.begin();it!=m_phiThreeSlices.end();++it) {
      (*it).clear();
    }
    m_nSP = 0;
  }

  void addSpacePoint(int phiIndex, const TrigSiSpacePointBase* p) {
    m_nSP++;
    m_phiSlices[phiIndex].push_back(p);
    for(int i=0;i<3;i++) {
      if(m_threeIndices[i][phiIndex]==-1) continue;//to account for special cases nPhiSlices=1,2
      m_phiThreeSlices[m_threeIndices[i][phiIndex]].push_back(p);
    }
  }

  void sortSpacePoints(bool isBarrel) {
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it=m_phiSlices.begin();it!=m_phiSlices.end();++it) {
      if((*it).empty()) continue;
      if(isBarrel) std::sort(it->begin(), it->end(), compareZ());
      else std::sort(it->begin(), it->end(), compareR());
    }
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it=m_phiThreeSlices.begin();it!=m_phiThreeSlices.end();++it) {
      if((*it).empty()) continue;
      if(isBarrel) std::sort(it->begin(), it->end(), compareZ());
      else std::sort(it->begin(), it->end(), compareR());
    }
  }

  void sortSpacePoints(bool isBarrel, bool isPositive) {
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it=m_phiSlices.begin();it!=m_phiSlices.end();++it) {
      if((*it).empty()) continue;
      if(isBarrel) std::sort(it->begin(), it->end(), compareZ());
      else {
	if(isPositive) std::sort(it->begin(), it->end(), compareRless());
	else std::sort(it->begin(), it->end(), compareR());
      }
    }
    for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator it=m_phiThreeSlices.begin();it!=m_phiThreeSlices.end();++it) {
      if((*it).empty()) continue;
      if(isBarrel) std::sort(it->begin(), it->end(), compareZ());
      else {
	if(isPositive) std::sort(it->begin(), it->end(), compareRless());
	else std::sort(it->begin(), it->end(), compareR());
      }
    }
  }

  int size() const { return m_nSP; }
  
  int m_nSP;
  std::vector<std::vector<const TrigSiSpacePointBase*> > m_phiSlices;
  std::vector<std::vector<const TrigSiSpacePointBase*> > m_phiThreeSlices;
  std::vector<int> m_threeIndices[3];

private:
  LPhiSector() {};

} L_PHI_SECTOR;

typedef struct LPhi_Storage {

public:

  LPhi_Storage(int nPhiSectors, int nLayers) {
    m_layers.reserve(nLayers);
    for(int i = 0;i<nLayers;i++) m_layers.push_back(L_PHI_SECTOR(nPhiSectors));
    /*
    for(int i = 0;i<nLayers;i++) {
      std::cout<<"Layer "<<i<<std::endl;
      for(int j=0;j<nPhiSectors;j++) {
	std::cout<<"Phi sector "<<j<<" 3 indices ";
	for(int k=0;k<3;k++) {
	  std::cout<<m_layers[i].m_threeIndices[k][j]<<" ";
	}
	std::cout<<std::endl;
      }
    }
    */
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

  void sortSpacePoints2(const std::vector<TRIG_INDET_SI_LAYER>& layerGeometry) {
    int layerId = 0;
    for(std::vector<L_PHI_SECTOR>::iterator it=m_layers.begin();it!=m_layers.end();++it,layerId++) {
      if((*it).m_nSP==0) continue;
      (*it).sortSpacePoints(layerGeometry[layerId].m_type==0, layerGeometry[layerId].m_refCoord > 0);
    }
  }

  std::vector<L_PHI_SECTOR> m_layers;

} L_PHI_STORAGE;

typedef struct InternalSoA {

public:

InternalSoA() : m_spi(0), m_spo(0), m_r(0), m_u(0), m_v(0), m_t(0), m_ti(0), m_to(0), m_tCov(0), m_sorted_sp(0),
    m_sorted_sp_type(0),
    m_sorted_sp_t(0) {}

  ~InternalSoA() {
  }

  void clear() {
    delete[] m_spi;
    delete[] m_spo;
    delete[] m_r;
    delete[] m_u;
    delete[] m_v;
    delete[] m_t;
    delete[] m_ti;
    delete[] m_to;
    delete[] m_tCov;
    delete[] m_sorted_sp;
    delete[] m_sorted_sp_type;
    delete[] m_sorted_sp_t;
    m_spi = 0;
    m_spo = 0;
    m_r = 0;
    m_u = 0;
    m_v = 0;
    m_t = 0;
    m_ti = 0;
    m_to = 0;
    m_tCov = 0;
    m_sorted_sp = 0;
    m_sorted_sp_type = 0;
    m_sorted_sp_t = 0;
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
    m_ti = new double[spSize];
    m_to = new double[spSize];
    m_tCov = new double[spSize];
    m_sorted_sp = new const TrigSiSpacePointBase*[spSize];
    m_sorted_sp_type = new int[spSize];
    m_sorted_sp_t = new double[spSize];
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
  double* m_ti;
  double* m_to;
  double* m_tCov;
  const TrigSiSpacePointBase** m_sorted_sp;
  int* m_sorted_sp_type;
  double* m_sorted_sp_t;

} INTERNAL_SOA;


typedef std::vector<std::pair<float, TrigInDetTriplet*> > INTERNAL_TRIPLET_BUFFER;

typedef std::pair<std::vector<const TrigSiSpacePointBase*>::const_iterator, std::vector<const TrigSiSpacePointBase*>::const_iterator> SP_RANGE;

typedef class TrigTrackSeedGenerator {

 public:

  TrigTrackSeedGenerator(const TrigCombinatorialSettings&);
  ~TrigTrackSeedGenerator(); 

  void loadSpacePoints(const std::vector<TrigSiSpacePointBase>&);
  void createSeeds();
  void createSeedsZv();
  void getSeeds(std::vector<TrigInDetTriplet*>&);

private:
  bool validateLayerPair(int, int, float, float); 
  bool validateLayerPair(int, int, float, float, float); 
  bool getSpacepointRange(int, const std::vector<const TrigSiSpacePointBase*>&, SP_RANGE&);
  int processSpacepointRange(int, float, float, bool, const SP_RANGE&);
  int processSpacepointRangeZv(float, float, bool, const SP_RANGE&);
  void createTriplets(const TrigSiSpacePointBase*, int, int, INTERNAL_TRIPLET_BUFFER&);
  void createTripletsNew(const TrigSiSpacePointBase*, int, int, INTERNAL_TRIPLET_BUFFER&);
  void storeTriplets(INTERNAL_TRIPLET_BUFFER&);

  const TrigCombinatorialSettings& m_settings;
  double m_phiSliceWidth;
  double m_minDeltaRadius, m_maxDeltaRadius, m_zTol;

  L_PHI_STORAGE* m_pStore;

  INTERNAL_SOA m_SoA;

  double m_CovMS, m_minR_squ, m_dtPreCut;

  INTERNAL_TRIPLET_BUFFER m_triplets;

  float m_zMinus, m_zPlus, m_minCoord, m_maxCoord;

  //bool m_isBarrel;

  int m_nInner, m_nOuter;
  std::vector<int> m_innerMarkers, m_outerMarkers;
} TRIG_TRACK_SEED_GENERATOR;

#endif // not TRIGINDETPATTRECOTOOLS_TRIGTRACKSEEDGENERATOR_H
