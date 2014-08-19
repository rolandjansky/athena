/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_COMBINATORIAL_TRACK_FINDING_H__
#define __TRIG_COMBINATORIAL_TRACK_FINDING_H__

#include <cmath>
#include <memory>
#include "TrigInDetPattRecoTools/LayeredSpacePointStorage.h"
#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"

class TrigSiSpacePointBase;
class TrigInDetTracklet;
class TrigInDetDoublet;
class TrigInDetTriplet;
class TrigInDetTripletCluster;
class TrigInDetTrack;
class LayeredStorageBase;
struct TrigL2LayerSetLUT;

class TrigCombinatorialTrackFinding {
  
 public: 

  TrigCombinatorialTrackFinding(const TrigCombinatorialSettings&);
  ~TrigCombinatorialTrackFinding(); 

  StatusCode findDoublets(const std::vector<const TrigSiSpacePointBase*>&, 
			  const std::vector<const TrigSiSpacePointBase*>&, 
			  std::vector<TrigInDetDoublet*>&);

  StatusCode findTripletClusters(const std::vector<TrigInDetDoublet*>& doublets, 
			  const std::vector<const TrigSiSpacePointBase*>& sp3,
			  std::vector<TrigInDetTripletCluster*>& tripletClusters,
			  const TrigL2LayerSetLUT* pLUT = NULL);

  void setSpacePointsUnused(std::vector<TrigSiSpacePointBase>& vSP);
  void processClusters(std::vector<TrigInDetTripletCluster*>& );
  void mergeClones(std::vector<TrigInDetTripletCluster*>&, std::vector<TrigInDetTripletCluster*>&);

  void collectTriplets(const std::vector<TrigInDetTripletCluster*>&, std::vector<std::shared_ptr<TrigInDetTriplet>>&);
  void collectBestTriplets(const std::vector<TrigInDetTripletCluster*>&, std::vector<std::shared_ptr<TrigInDetTriplet>>&);
  void collectBestTriplets(const TrigInDetTripletCluster*, std::vector<std::shared_ptr<TrigInDetTriplet>>&);

  // Triplet finding code
  StatusCode createTriplets(const TrigInDetDoublet& doublet, const std::vector<LayeredStorageBase*>& , std::vector<std::shared_ptr<TrigInDetTriplet>>& triplets);
  void filterTriplets(const LayeredStorageHelper&, const std::vector<std::shared_ptr<TrigInDetTriplet>>&, std::vector<std::shared_ptr<TrigInDetTriplet>>&);

  //Helper function for phi correction
  static inline void correct_phi(float& phi) {
    if (phi<-M_PI) phi+=2*M_PI;
    else if (phi>M_PI) phi-=2*M_PI;
  }
  static inline void correct_phi(double& phi) {
    if (phi<-M_PI) phi+=2*M_PI;
    else if (phi>M_PI) phi-=2*M_PI;
  }

  void setTestBarCodes(const std::vector<int>& vbc) {
    m_barCodes.clear();
    std::copy(vbc.begin(), vbc.end(), std::back_inserter(m_barCodes));
  }

 protected:
  
  void processClusters(LayeredStorageHelper&, std::vector<TrigInDetTripletCluster*>& );
  float calculateTripletClusterQuality(const TrigInDetTripletCluster*);
  bool checkClones(const TrigInDetTripletCluster*, const TrigInDetTripletCluster*);

 private:

  /************************************/
  /** PROPERTIES **********************/
  /************************************/

  const TrigCombinatorialSettings& m_settings;

  float m_cut_rz[20];
  float m_mindzdr;
  float m_maxdzdr;
  std::vector<int> m_barCodes;
  LayeredStorageHelper m_lsh;

  /************************************/
  /** FITTING HELPER METHODS **********/
  /************************************/
  
  //IDSCAN Hit Filter style d0 calculation
  inline float get_d0(const TrigSiSpacePointBase& s0, const TrigSiSpacePointBase& s1, const TrigSiSpacePointBase& s2) {
    float phi0 = s0.phi();
    float phi1 = s1.phi();
    float phi2 = s2.phi();
    float r0 = s0.r();
    float r1 = s1.r();
    float r2 = s2.r();
    //Note: calculation with ratios of radii is actually slightly slower
    //std::cout << "Old-style d0: " << r0*r1*r2*(phi0*r1 - phi0*r2 - phi1*r0 + phi1*r2 + phi2*r0 - phi2*r1)/(r0*r0*r1 - r0*r0*r2 - r0*r1*r1 + r0*r2*r2 + r1*r1*r2 - r1*r2*r2) << std::endl;
    //std::cout << "New-style d0: " << (phi0*r1 - phi0*r2 - phi1*r0 + phi1*r2 + phi2*r0 - phi2*r1)/(((r0-r1)/r2 + (r2-r0)/r1 + (r1-r2)/r0)) << std::endl;
    return r0*r1*r2*(phi0*r1 - phi0*r2 - phi1*r0 + phi1*r2 + phi2*r0 - phi2*r1)/(r0*r0*r1 - r0*r0*r2 - r0*r1*r1 + r0*r2*r2 + r1*r1*r2 - r1*r2*r2);
  }
  
  //IDSCAN Hit Filter style alpha calculation
  //Parametrise triplet as quadratic a + t*x + (alpha/2)*x**2
  inline float get_alpha(const TrigSiSpacePointBase& s0, const TrigSiSpacePointBase& s1, const TrigSiSpacePointBase& s2) {
    float dx1 = s1.x()-s0.x();
    float dy1 = s1.y()-s0.y();

    float dx2 = s2.x()-s0.x();
    float dy2 = s2.y()-s0.y();

    //Transform into line with s1 and s2 on x-axis, s1 at (0,0)
    float x1prime = sqrt(dx1*dx1+dy1*dy1);
    float x2prime = (dx2*dx1 + dy2*dy1)/x1prime;
    float y2prime = (dy2*dx1 - dx2*dy1)/x1prime;
    return 2*y2prime/(x2prime*(x2prime-x1prime));
  }
  
  //IDSCAN Hit Filter style pT calculation
  inline float get_pT(const TrigSiSpacePointBase& s0, const TrigSiSpacePointBase& s1, const TrigSiSpacePointBase& s2) {
    //pT = 0.3*B*R
    float a = get_alpha(s0, s1, s2);
    if (a==0.0) {
      return 1e9;
    }
    return 0.3*m_settings.m_magFieldZ/a;
  }
  
  //IDSCAN Hit Filter style phi calculation
  inline float get_phi(const TrigSiSpacePointBase& s0, const TrigSiSpacePointBase& s1, const TrigSiSpacePointBase& s2) {
    float x0 = s0.x();
    float x1 = s1.x();
    float y0 = s0.y();
    float y1 = s1.y();
    
    //Transform into coordinate system rotated so that y0'=y1'=0, at angle phi
    float phi = std::atan2(y1-y0,x1-x0);	
    float L = std::fabs(x0*cos(phi) - y0*sin(phi));//Distance to 0th spacepoint from origin
    //y = (alpha/2)*x**2 + t*x to describe parabola
    float alpha = get_alpha(s0,s1,s2);
    //Distance between 0th and 1st spaceppoint
    float x1prime = std::sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
    float t   = -0.5*alpha*x1prime;
    //t' = phi0' in small angle approximation
    float phi0prime = t - alpha*L;
    
    float phi_prime = phi + phi0prime;
    if(phi_prime<-M_PI) phi_prime+=2*M_PI;
    if(phi_prime>M_PI)  phi_prime-=2*M_PI;
    return phi_prime;
  }

};


#endif


 
