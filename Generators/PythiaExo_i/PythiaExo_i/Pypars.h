/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Pypars_h
#define Pypars_h

extern "C" {
  void* pypars_address_(void);
}
/**
@class      Pypars.hh

@brief      Class definition for Pypars, which is used
      to modify the Pythia Pypars common.

 Environment:
      CDF Run 2

@author      Chris Green,   Purdue University
      Stan Thompson, Glasgow University    

 Pythia Common access method
*/

class Pypars {
public:
  Pypars();
  ~Pypars();
  int& mstp(int n);
  double& parp(int n);
  int& msti(int n);
  double& pari(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMstp() const {return _lenMstp;}
  int lenParp() const {return _lenParp;}
  int lenMsti() const {return _lenMsti;}
  int lenPari() const {return _lenPari;}
private: 

  // Lengths of MSTU and PARU COMMONS
  static const int _lenMstp = 200;
  static const int _lenParp = 200;
  static const int _lenMsti = 200;
  static const int _lenPari = 200;

  struct PYPARS;
  friend struct PYPARS;
  struct PYPARS {
  int mstp[_lenMstp];
  double parp[_lenParp];
  int msti[_lenMsti];
  double pari[_lenPari];
  };
  int _dummy;
  double _realdummy;
  static PYPARS* _pypars;
};

#include "PythiaExo_i/Pypars.icc"

#endif
