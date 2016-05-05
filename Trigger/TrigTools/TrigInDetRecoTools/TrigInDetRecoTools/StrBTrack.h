/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRBTRACK_H
#define STRBTRACK_H

#include <vector>
#include <set>
#include <map>
#include <list>
#include <functional>

class TrigSiSpacePoint;



/****************************************************************/
/****************************************************************/
/** class StrBRoad                                               **/
/****************************************************************/
/****************************************************************/

class StrBRoad {
  
 public:
  
  StrBRoad() : lay1(100), lay2(100) {}
 
  //  StrBRoad() {}
  
  ~StrBRoad() {}
  
  
 public:

  int lay1, lay2;
  
  std::set<int> lay3;

};



/****************************************************************/
/****************************************************************/
/** class StrBSeed                                               **/
/****************************************************************/
/****************************************************************/

class StrBSeed {
  
 public:
  
  StrBSeed() : deleted(false) {
    for(unsigned int i=0; i<20; i++) {layer[i]=false; phiext[i]=1000;}
  }
  ~StrBSeed() {}
  
 public:
  
  // Track parameters
  float eta, phi, d02, z;
  bool deleted;

  // Space points
  const TrigSiSpacePoint *sp1, *sp2;
  
  // Seed cut quantities
  float dr;    // m_seedingPreCutRZ
  float z0;    // m_vertexingCutRZ
  float z12;
  float dphi;  // m_seedingPreCutPhi
  float phi0;  // m_seedingPreCutRPhi

  // Cached selection information
  float costheta;
  float deltax12;
  float deltay12;
  float deltaz12;
  float deltar12;

  // Extrapolation information
  bool layer[20];
  float xext[20];
  float yext[20];
  float zext[20];
  float rext[20];
  float dzext[20];
  float drext[20];
  float phiext[20];
  float dphiext[20];
};



/****************************************************************/
/****************************************************************/
/** class StrBTriplet                                            **/
/****************************************************************/
/****************************************************************/

class StrBTriplet {
  
 public:
  
  StrBTriplet() : deleted(false) {}
  ~StrBTriplet() {}
  
 public:
  
  // Track parameters
  float eta, phi, d0, pt, z0;
  float chi2;
  bool deleted;
  
  // Space points
  const TrigSiSpacePoint *sp1, *sp2, *sp3;
  
  // Seed cut quantities
  float dr;    // m_seedingPreCutRZ
  float dphi;  // m_seedingPreCutPhi
  float phi0;  // m_seedingPreCutRPhi
  float drz;
  float drp; 
};



/****************************************************************/
/****************************************************************/
/** class StrBTrack                                              **/
/****************************************************************/
/****************************************************************/

class StrBTrack {
  
 public:
  
  StrBTrack() : deleted(false), merged(false), removed(false) {
    sp.resize(m_maxLay,0);
    sp2.resize(m_maxLay,0);
    tr.resize(m_maxLay,0);
    tr2.resize(m_maxLay,0);
    spRej.resize(m_maxLay,0);
    spRej2.resize(m_maxLay,0);
    spShared.resize(m_maxLay,0);
    spDistance.resize(m_maxLay,1000);
    chi2=0;
  }

  ~StrBTrack() {
    sp.clear();
    sp2.clear();
    tr.clear();
    tr2.clear();
    spRej.clear();
    spRej2.clear();
    spShared.clear();
    spDistance.clear();
  }
  
 public:
  
  // Track parameters
  float eta, phi;
  float chi2;
  bool deleted, merged, removed;
  
  // Space points
  std::vector<const TrigSiSpacePoint*> sp;
  std::vector<const TrigSiSpacePoint*> sp2;
  std::vector<const StrBTriplet*> tr;
  std::vector<const StrBTriplet*> tr2;
  std::vector<int> spRej;
  std::vector<int> spRej2;

  std::vector<int> spShared;
  int sumSp;
  int sumSpAll;

  // Distances
  std::vector<float> spDistance;
  float sumSpDistance;

 private:

  static int m_maxLay;

 public:

  static void setMaxLay(int maxLay) {m_maxLay=maxLay;}
  int getMaxLay() const {return m_maxLay;}

 public: 

  void evaluateSum() {
    sumSp=0; sumSpAll=0; sumSpDistance=0.0;
    for(int i=0; i<m_maxLay; i++) {
      if(sp[i]!=0) {
	sumSp++;
	sumSpAll++;
	sumSpDistance += spDistance[i];
      }
      if(sp2[i]!=0) {
	sumSpAll++;
      }
    }
  }
  
  
  float evaluateSharedFraction() {
    int total=0, shared=0;
    for(int i=0; i<m_maxLay; i++) {
      if(sp[i]!=0) {
	total++;
        shared += spShared[i];
      }
    }
    return ((float)shared/total);
  }
  
  
  void resetDistance() {
    for(int i=0; i<m_maxLay; i++) {spDistance[i]=0;}
  }
  
  
  friend bool operator<(const StrBTrack &t1, const StrBTrack &t2) {

    // Count space points
    int sp1=0, sp2=0;
    int lay1=0, lay2=0;
    for(int i=0; i<t1.getMaxLay(); i++) {
      if(t1.sp[i]!=0) {sp1++; lay1++;}
      if(t2.sp[i]!=0) {sp2++; lay2++;}
      if(t1.sp2[i]!=0) sp1++;
      if(t2.sp2[i]!=0) sp2++;
    }
    if(t1.deleted) lay1=0;
    if(t2.deleted) lay2=0;

    // Evaluate bool relations
    bool isLayBetter        = (lay1             >  lay2);
    bool isLayEqual         = (lay1             == lay2);
    bool isSpBetter         = (sp1              >  sp2);
    bool isSpEqual          = (sp1              == sp2);
    bool isChi2Better       = (t1.chi2          <  t2.chi2);

    // Check if first track is better
    return (isLayBetter || (isLayEqual&&isSpBetter)|| (isLayEqual&&isSpEqual&&isChi2Better));
  }

};





/****************************************************************/
/****************************************************************/
/** class StrBPattern                                            **/
/****************************************************************/
/****************************************************************/

class StrBPattern {
  
 public:
  
  StrBPattern() {}
  ~StrBPattern() {}
  
 public:
 
  StrBRoad*               road;
  StrBSeed*               seed;
  std::list<StrBTriplet*> triplets;
  StrBTrack*              track;


  friend bool operator<(const StrBPattern &t1, const StrBPattern &t2) {
    return ((*(t1.track))<(*(t2.track)));
  }

};


struct StrBPatternCompare : public std::binary_function<StrBPattern*, StrBPattern*, bool> {
  bool operator()(const StrBPattern *t1, const StrBPattern *t2) const {
    return ((*(t1->track))<(*(t2->track)));
  };
};



/****************************************************************/
/****************************************************************/
/** class StrBTrk                                                **/
/****************************************************************/
/****************************************************************/

class StrBTrk {
  
 public:
  
  StrBTrk() : triplet_num(0), deleted(false) {}
  ~StrBTrk() {}
    
 public:
  
  // General parameters
  float z0;

  // Seed parameters
  const TrigSiSpacePoint *seed_sp1, *seed_sp2;
  float seed_z0, seed_eta, seed_phi;

  // Triplet parameters
  unsigned int triplet_num;
  std::vector<const TrigSiSpacePoint*> triplet_sp3;
  std::vector<float> triplet_eta;
  std::vector<float> triplet_phi;
  std::vector<float> triplet_chi2;
  std::vector<float> triplet_pt;
  std::vector<float> triplet_d0;

  // Track parameters
  StrBTrack *track;

  // Processing parameters
  bool deleted;

};

#endif // TrigSiTrack_StrBTrack_H
