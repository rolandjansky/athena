/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_IN_DET_TRACKLET_H__
#define __TRIG_IN_DET_TRACKLET_H__

#include<cmath>
#include<memory>
#include<set>
#include<algorithm>

#include "TrigInDetEvent/TrigSiSpacePointBase.h"


class TrigInDetTracklet {

 public:
   TrigInDetTracklet() = delete; //to prevent creation w/o initialization
  
 TrigInDetTracklet(float eta, float phi, float z0, float d0, float pt, float chi2) : m_eta(eta), m_phi(phi), m_z0(z0), m_d0(d0), m_pt(pt), m_chi2(chi2) {}
  TrigInDetTracklet(const TrigInDetTracklet& t) : m_eta(t.m_eta), m_phi(t.m_phi), m_z0(t.m_z0), m_d0(t.m_d0), m_pt(t.m_pt), m_chi2(t.m_chi2) {}
  TrigInDetTracklet& operator = (const TrigInDetTracklet& t) {
    m_eta   = t.m_eta;
    m_phi   = t.m_phi;
    m_z0    = t.m_z0;
    m_d0    = t.m_d0;
    m_pt    = t.m_pt;
    m_chi2  = t.m_chi2;
    return *this;
  }

  void eta(float eta)   {m_eta  = eta;}
  void phi(float phi)   {m_phi  = phi;}
  void z0(float z0)     {m_z0   = z0;}
  void d0(float d0)     {m_d0   = d0;}
  void pt(float pt)     {m_pt   = pt;}
  void chi2(float chi2) {m_chi2 = chi2;}

  float eta()  const {return m_eta;}
  float phi()  const {return m_phi;}
  float z0()   const {return m_z0;}
  float d0()   const {return m_d0;}
  float pt()   const {return m_pt;}
  float chi2() const {return m_chi2;}

 protected:

  float m_eta, m_phi, m_z0, m_d0, m_pt, m_chi2;

 private:


};


class TrigInDetDoublet : public TrigInDetTracklet {

 public:

 TrigInDetDoublet(const TrigSiSpacePointBase& s1, const TrigSiSpacePointBase& s2, float z0, float tau) : 
  TrigInDetTracklet(0.0, 0.0, z0, 0.0, 0.0, 0.0),
    m_tau(tau),
    m_s1(s1), 
    m_s2(s2) {
      m_cs = sqrt(tau*tau+1.0);
      if (tau!=0.0) {//only possible for barrel doublets
        m_cs_inv = sqrt(1.0/(tau*tau)+1.0);
      }
      else {
        m_cs_inv = 0;
      }
    };

 TrigInDetDoublet(const TrigSiSpacePointBase& s1, const TrigSiSpacePointBase& s2, float eta, float phi, float z0, float d0, float pt, float chi2=0.0) : 
  TrigInDetTracklet(eta, phi, z0, d0, pt, chi2), m_s1(s1), m_s2(s2) {
      float dz = s2.z()-s1.z();
      float dr = s2.r()-s1.r();
      m_tau = dz/dr;
      m_cs = sqrt(m_tau*m_tau+1.0);
      m_cs_inv = sqrt(1.0/(m_tau*m_tau)+1.0);
  };
  
 TrigInDetDoublet(const TrigInDetDoublet& d) : TrigInDetTracklet(d.m_eta, d.m_phi, d.m_z0, d.m_z0, d.m_pt, d.m_chi2),
    m_tau(d.m_tau),
    m_s1(d.m_s1),
    m_s2(d.m_s2), m_cs(d.m_cs), m_cs_inv(d.m_cs_inv) {};

  const TrigSiSpacePointBase& s1() const {return m_s1;}
  const TrigSiSpacePointBase& s2() const {return m_s2;}
  float tau() const {return m_tau;}
  float cs() const {return m_cs;}
  float cs_inv() const {return m_cs_inv;}

 protected:

  float m_tau;//cot(theta)
  const TrigSiSpacePointBase& m_s1;
  const TrigSiSpacePointBase& m_s2;
  float m_cs;//used for getSpacePoint calculation in LayeredStorage (barrel)
  float m_cs_inv;//used for getSpacePoint calculation in LayeredStorage (endcap)

};


class TrigInDetTriplet : public TrigInDetTracklet {

 public:

 TrigInDetTriplet(const TrigSiSpacePointBase& s1, const TrigSiSpacePointBase& s2, const TrigSiSpacePointBase& s3, float z0) : 
  TrigInDetTracklet(0.0, 0.0, z0, 0.0, 0.0, 0.0), 
    m_s1(s1), m_s2(s2), m_s3(s3), m_V(0.0), m_tag(-1) {
      m_weight = 1.0;
    };

 TrigInDetTriplet(const TrigSiSpacePointBase& s1, const TrigSiSpacePointBase& s2, const TrigSiSpacePointBase& s3, 
		  float eta, float phi, float z0, float d0, float pt, float chi2=0.0, float V=0.0) :
  TrigInDetTracklet(eta, phi, z0, d0, pt, chi2), 
    m_s1(s1), m_s2(s2), m_s3(s3), m_V(V), m_tag(-1) {
      m_weight = std::exp(-0.5*chi2);
    };

 TrigInDetTriplet(const TrigInDetTriplet& t) : TrigInDetTracklet(t.m_eta, t.m_phi, t.m_z0, t.m_d0, t.m_pt, t.m_chi2),
    m_s1(t.m_s1), m_s2(t.m_s2), m_s3(t.m_s3), m_V(t.m_V), m_tag(t.m_tag), m_weight(t.m_weight) {};
 TrigInDetTriplet(TrigInDetTriplet* t) : TrigInDetTracklet(t->m_eta, t->m_phi, t->m_z0, t->m_d0, t->m_pt, t->m_chi2),
    m_s1(t->m_s1), m_s2(t->m_s2), m_s3(t->m_s3), m_V(t->m_V), m_tag(t->m_tag), m_weight(t->m_weight) {};

  const TrigSiSpacePointBase& s1() const {return m_s1;}
  const TrigSiSpacePointBase& s2() const {return m_s2;}
  const TrigSiSpacePointBase& s3() const {return m_s3;}
  float weight() const {return m_weight;}
  float V() const {return m_V;}
  int tag() const {return m_tag;}
  void updateTag(int t) {m_tag=t;}

 protected:

  const TrigSiSpacePointBase& m_s1;
  const TrigSiSpacePointBase& m_s2;
  const TrigSiSpacePointBase& m_s3;
  float m_V;//variance of r-z residual
  int m_tag;//for clone identification and merging
  float m_weight;//weight=std::exp(-0.5*chi^2), cached
};


class TrigInDetTripletCluster : public TrigInDetTracklet {

 public:

 TrigInDetTripletCluster(std::vector<std::shared_ptr<TrigInDetTriplet>>& triplets,
			 float eta, float phi, float z0, float d0, float pt, float chi2=0.0, float llr=0.0) :
  TrigInDetTracklet(eta, phi, z0, d0, pt, chi2),
    m_triplets(triplets), m_llr(llr) {};

 TrigInDetTripletCluster(std::shared_ptr<TrigInDetTriplet> triplet) :
  TrigInDetTracklet(triplet->eta(), triplet->phi(), triplet->z0(), triplet->d0(), triplet->pt(), triplet->chi2()),
    m_triplets(std::vector<std::shared_ptr<TrigInDetTriplet>>{triplet}), 
    m_llr(0.0) {};
  
 TrigInDetTripletCluster(const TrigInDetTripletCluster& t) : TrigInDetTracklet(t.m_eta, t.m_phi, t.m_z0, t.m_d0, t.m_pt, t.m_chi2),
    m_triplets(t.m_triplets), m_llr(t.m_llr) {};

  ~TrigInDetTripletCluster() {
    m_triplets.clear();
  }

  void resetTriplets() {m_triplets.clear();}

  const std::vector<std::shared_ptr<TrigInDetTriplet>>& triplets() const {return m_triplets;}
  void addTriplet(TrigInDetTriplet* t) {m_triplets.push_back(std::make_shared<TrigInDetTriplet>(t));}
  float llr() const {return m_llr;}

 protected:

  std::vector<std::shared_ptr<TrigInDetTriplet>> m_triplets;
  float m_llr;// log-likelihood ratio
};


#endif // __TRIG_IN_DET_TRACKLET__
