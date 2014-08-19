/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIPLET_CLUSTER_FITTER_H__
#define __TRIPLET_CLUSTER_FITTER_H__

class TrigInDetTripletCluster;
class LayeredStorageHelper;

typedef class TripletClusterFitter {
 public:

  struct SpComparator {
  SpComparator(double c, double s) : m_c(c), m_s(s) {};
    bool operator()(const TrigSiSpacePointBase* p1, const TrigSiSpacePointBase* p2) {
      double x1 = p1->x()*m_c + p1->y()*m_s;
      double x2 = p2->x()*m_c + p2->y()*m_s;
      return (x1 < x2);
    }
    double m_c, m_s;
  };


 TripletClusterFitter(const LayeredStorageHelper& lsh, double bFieldZ=2.0, bool fieldOn=true, float maxChi2=30.0, bool doEloss=false) :
  m_layerHelper(lsh), m_maxChi2(maxChi2), m_doEloss(doEloss) {
    const double C=0.29997;
    m_fConst = (fieldOn) ? C*bFieldZ : 0.0;
    m_X0 = 0.02;// 2% of rad. length per detector
    m_fracLoss = m_X0*(1-0.5*m_X0);
    m_sigmaMS2 = 0.0;
  };
    
  virtual ~TripletClusterFitter(){};
    
  int fit(const std::vector<std::shared_ptr<TrigInDetTriplet>>&, float& eta, float& z0, float& pt);

 protected:

  void newTrack(const std::vector<const TrigSiSpacePointBase*>&, float, float, float);
  double getChi2(const TrigSiSpacePointBase*);
  void update();
  void applyMaterialEffects();

 private:
  const LayeredStorageHelper& m_layerHelper;
  double m_Rk[5],m_Gk[5][5],m_resid[2],
    m_Bk[5][2],m_Vk[2][2],m_Dk[2][2],
    m_fConst;

  double m_X0;
  double m_cosA, m_sinA;

  double m_maxChi2;
  bool m_doEloss;
  double m_sigmaMS2, m_fracLoss, m_deltaCov;

} TRIPLET_CLUSTER_FITTER;

#endif
