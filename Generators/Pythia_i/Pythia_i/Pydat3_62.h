/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Pydat3_h
#define Pydat3_h

extern "C" {
  void* pydat3_address_(void);
}

/**
@class Pydat3.hh

@brief Description:
      Class definition for Pydat3, which is used
      to modify the Pythia Pydat3 common.

 Environment:
      CDF Run 2

@author       Chris Green,   Purdue University
      
Stan Thompson, Glasgow University    


 Pythia Common access method
*/

class Pydat3 {
public:
  Pydat3();
  ~Pydat3();

  int& mdcy(int kc, int i);
  int& mdme(int idc, int i);
  double& brat(int idc);
  int& kfdp(int idc, int kf);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMdcy() const {return s_lenMdcy;}
  int depthMdcy() const {return s_depthMdcy;}
  int lenMdme() const {return s_lenMdme;}
  int depthMdme() const {return s_depthMdme;}
  int lenBrat() const {return s_lenBrat;}
  int lenKfdp() const {return s_lenKfdp;}
  int depthKfdp() const {return s_depthKfdp;}
private: 

  // Lengths of array in PYDAT2 common
  static const int s_lenMdcy = 500;
  static const int s_depthMdcy = 3;
  static const int s_lenMdme = 8000;
  static const int s_depthMdme = 2;
  static const int s_lenBrat = 8000;
  static const int s_lenKfdp = 8000;
  static const int s_depthKfdp = 5;

  struct PYDAT3;
  friend struct PYDAT3;
  struct PYDAT3 {
    int mdcy[s_depthMdcy][s_lenMdcy];
    int mdme[s_depthMdme][s_lenMdme];
    double brat[s_lenBrat];
    int kfdp[s_depthKfdp][s_lenKfdp];
  };
  int m_dummy;
  double m_realdummy;
  static PYDAT3* s_pydat3;
};

#include "Pythia_i/Pydat3.icc"

#endif
