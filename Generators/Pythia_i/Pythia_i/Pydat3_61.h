/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Pydat3_h
#define Pydat3_h

extern "C" {
  void* pydat3_address_(void);
}

/**

@class      Pydat3.hh

@brief      Class definition for Pydat3, which is used
      to modify the Pythia Pydat3 common.

 Environment:
      CDF Run 2

@author      Chris Green,   Purdue University
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
  int lenMdcy() const {return _lenMdcy;}
  int depthMdcy() const {return _depthMdcy;}
  int lenMdme() const {return _lenMdme;}
  int depthMdme() const {return _depthMdme;}
  int lenBrat() const {return _lenBrat;}
  int lenKfdp() const {return _lenKfdp;}
  int depthKfdp() const {return _depthKfdp;}
private: 

  // Lengths of array in PYDAT2 common
  static const int _lenMdcy = 500;
  static const int _depthMdcy = 3;
  static const int _lenMdme = 4000;
  static const int _depthMdme = 2;
  static const int _lenBrat = 4000;
  static const int _lenKfdp = 4000;
  static const int _depthKfdp = 5;

  struct PYDAT3;
  friend struct PYDAT3;
  struct PYDAT3 {
    int mdcy[_depthMdcy][_lenMdcy];
    int mdme[_depthMdme][_lenMdme];
    double brat[_lenBrat];
    int kfdp[_depthKfdp][_lenKfdp];
  };
  int _dummy;
  double _realdummy;
  static PYDAT3* _pydat3;
};

#include "Pythia_i/Pydat3.icc"

#endif
