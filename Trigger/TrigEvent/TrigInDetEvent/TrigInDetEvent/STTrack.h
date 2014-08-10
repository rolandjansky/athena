/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STTRACK_H
#define STTRACK_H

#include <vector>
#include <set>
#include <map>
#include <list>

class TrigSiSpacePoint;



/****************************************************************/
/****************************************************************/
/** class STRoad                                               **/
/****************************************************************/
/****************************************************************/

class STRoad {
  
 public:
  
  STRoad() : lay1(100), lay2(100) {}
 
  //  STRoad() {}
  
  ~STRoad() {}
  
  
 public:

  int lay1, lay2;
  
  std::set<int> lay3;

};



/****************************************************************/
/****************************************************************/
/** class STSeed                                               **/
/****************************************************************/
/****************************************************************/

class STSeed {
  
 public:
  
  STSeed() : deleted(false) {
    for(unsigned int i=0; i<19; i++) {layer[i]=false; phiext[i]=1000;}
  }
  ~STSeed() {}
  
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
  bool layer[19];
  float xext[19];
  float yext[19];
  float zext[19];
  float rext[19];
  float dzext[19];
  float drext[19];
  float phiext[19];
  float dphiext[19];
};



/****************************************************************/
/****************************************************************/
/** class STTriplet                                            **/
/****************************************************************/
/****************************************************************/

class STTriplet {
  
 public:
  
  STTriplet() : deleted(false) {}
  ~STTriplet() {}
  
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
/** class STTrack                                              **/
/****************************************************************/
/****************************************************************/

class STTrack {
  
 public:
  
  STTrack() : deleted(false), merged(false), removed(false) {
    for(unsigned int i=0; i<19; i++) {
      sp[i]=sp2[i]=0;
      tr[i]=tr2[i]=0;
      spDistance[i]=10000; spShared[i]=0;
      spRej[i]=spRej2[i]=0;
      chi2=0;
    }
  }
  ~STTrack() {}
  
 public:
  
  // Track parameters
  float eta, phi;
  float chi2;
  bool deleted, merged, removed;
  
  // Space points
  const TrigSiSpacePoint* sp[19];
  const TrigSiSpacePoint* sp2[19];
  const STTriplet* tr[19];
  const STTriplet* tr2[19];
  int spRej[19];
  int spRej2[19];

  int spShared[19];
  int sumSp;
  int sumSpAll;

  // Distances
  float spDistance[19];
  float sumSpDistance;

 public: 

  void evaluateSum() {
    sumSp=0; sumSpAll=0; sumSpDistance=0.0;
    for(unsigned int i=0; i<19; i++) {
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
    for(unsigned int i=0; i<19; i++) {
      if(sp[i]!=0) {
	total++;
        shared += spShared[i];
      }
    }
    return ((float)shared/total);
  }
  
  
  void resetDistance() {
    for(unsigned int i=0; i<19; i++) {spDistance[i]=0;}
  }
  
  
  friend bool operator<(const STTrack &t1, const STTrack &t2) {

    // Count space points
    int sp1=0, sp2=0;
    int lay1=0, lay2=0;
    for(unsigned int i=0; i<19; i++) {
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
/** class STPattern                                            **/
/****************************************************************/
/****************************************************************/

class STPattern {
  
 public:
  
  STPattern() {}
  ~STPattern() {}
  
 public:
 
  STRoad*               road;
  STSeed*               seed;
  std::list<STTriplet*> triplets;
  STTrack*              track;


  friend bool operator<(const STPattern &t1, const STPattern &t2) {
    return ((*(t1.track))<(*(t2.track)));
  }

};


struct STPatternCompare : public std::binary_function<STPattern*, STPattern*, bool> {
  bool operator()(const STPattern *t1, const STPattern *t2) const {
    return ((*(t1->track))<(*(t2->track)));
  };
};



/****************************************************************/
/****************************************************************/
/** class STTrk                                                **/
/****************************************************************/
/****************************************************************/

class STTrk {
  
 public:
  
  STTrk() : triplet_num(0), deleted(false) {}
  ~STTrk() {}
    
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
  STTrack *track;

  // Processing parameters
  bool deleted;

};

#endif // TrigSiTrack_STTrack_H
