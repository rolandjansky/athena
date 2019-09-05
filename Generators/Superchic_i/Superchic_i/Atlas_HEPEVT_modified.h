/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
Copied directly from the Tauola_i/Atlas_HEPEVT with minor changes
*/


#ifndef Atlas_HEPEVT_modified_h
#define Atlas_HEPEVT_modified_h

#include "CLIDSvc/CLASS_DEF.h"

extern "C" {
  void* atlas_hepevt_modified_address_(void);
}
/**
@class Atlas_HEPEVT_modified.h

@brief Needed fot interface of Superchic_i to Superchic3.03 generator
to store the umodified HEPEVT common.
This code was directly copied  from Tailola_i package  https://gitlab.cern.ch/atlas/athena/blob/21.6/Generators/Tauola_i/Tauola_i/Atlas_HEPEVT.h
with minor changes, original author list is following. This modified class takes into account the length of the array in hepevt parameteri nmxhep which is set to (nmxhep=4000) instead of HEPEVT standard event common which uses nmxhep=10000


@author Borut Paul Kersevan (BPK), June 2003

HEPEVT standard event common
PARAMETER (NMXHEP=10000)
COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
& JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)

*/

class Atlas_HEPEVT_modified {
public:
  Atlas_HEPEVT_modified();
  ~Atlas_HEPEVT_modified();

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
  static const int s_lenNmxhep = 4000 ;

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

  // s_HEPEVT is needed access  the umodified HEPEVT common block information
  static HEPEVT* s_HEPEVT;

  HEPEVT m_HEPEVT;

};

CLASS_DEF( Atlas_HEPEVT_modified, 9966, 1)

#include "Superchic_i/Atlas_HEPEVT_modified.icc"

#endif
