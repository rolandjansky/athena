/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Atlas_HEPEVT_h
#define Atlas_HEPEVT_h

#include "CLIDSvc/CLASS_DEF.h"

extern "C" {
  void* atlas_hepevt_address_(void);
}
/**
@class Atlas_HEPEVT.h

@brief      Needed fot interface of Tauola/Photos to Herwig 6.5 and Pythia 6.2+
      to store the umodified HEPEVT common.

@author Borut Paul Kersevan (BPK), June 2003

 HEPEVT standard event common
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     & JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)

*/

class Atlas_HEPEVT {
public:
  Atlas_HEPEVT();
  ~Atlas_HEPEVT();

  void fill();
  void spill();
  int& nevhep();
  int& nhep();
  int& isthep(int nh);
  int& idhep(int nh);
  int& jmohep(int i, int nh);
  int& jdahep(int i, int nh);
  double& phep(int j, int nh);
  double& vhep(int k, int nh);

  inline void init(); // inlined for speed of access (small function)

  // return common array lengths
  int depthRel() const {return s_depthRel;}
  int depthPhep() const {return s_depthPhep;}
  int depthVhep() const {return s_depthVhep;}
  int lenNmxhep() const {return s_lenNmxhep;}

private: 
  // Lengths of array in HEPEVT common
  static const int s_depthRel = 2 ; // Relatives -Mother & Daughter
  static const int s_depthPhep = 5 ;
  static const int s_depthVhep = 4 ;
  static const int s_lenNmxhep = 10000 ;

  struct HEPEVT;
  friend struct HEPEVT;

  struct HEPEVT {
    int nevhep;
    int nhep;
    int isthep[s_lenNmxhep];
    int idhep[s_lenNmxhep];
    int jmohep[s_lenNmxhep][s_depthRel];
    int jdahep[s_lenNmxhep][s_depthRel];
    double phep[s_lenNmxhep][s_depthPhep];
    double vhep[s_lenNmxhep][s_depthVhep];
  };

  int m_dummy;
  double m_realdummy;

  static HEPEVT* s_atlas_HEPEVT;

  HEPEVT m_atlas_HEPEVT;

};

CLASS_DEF( Atlas_HEPEVT, 6166, 1)

#include "Tauola_i/Atlas_HEPEVT.icc"

#endif
