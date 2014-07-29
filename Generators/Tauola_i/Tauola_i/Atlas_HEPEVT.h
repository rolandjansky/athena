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
  int depthRel() const {return _depthRel;}
  int depthPhep() const {return _depthPhep;}
  int depthVhep() const {return _depthVhep;}
  int lenNmxhep() const {return _lenNmxhep;}

private: 
  // Lengths of array in HEPEVT common
  static const int _depthRel = 2 ; // Relatives -Mother & Daughter
  static const int _depthPhep = 5 ;
  static const int _depthVhep = 4 ;
  static const int _lenNmxhep = 10000 ;

  struct HEPEVT;
  friend struct HEPEVT;

  struct HEPEVT {
    int nevhep;
    int nhep;
    int isthep[_lenNmxhep];
    int idhep[_lenNmxhep];
    int jmohep[_lenNmxhep][_depthRel];
    int jdahep[_lenNmxhep][_depthRel];
    double phep[_lenNmxhep][_depthPhep];
    double vhep[_lenNmxhep][_depthVhep];
  };

  int _dummy;
  double _realdummy;

  static HEPEVT* _atlas_HEPEVT;

  HEPEVT s_atlas_HEPEVT;

};

CLASS_DEF( Atlas_HEPEVT, 6166, 1)

#include "Tauola_i/Atlas_HEPEVT.icc"

#endif
