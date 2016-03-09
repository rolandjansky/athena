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
  int lenMstp() const {return s_lenMstp;}
  int lenParp() const {return s_lenParp;}
  int lenMsti() const {return s_lenMsti;}
  int lenPari() const {return s_lenPari;}
private: 

  // Lengths of MSTU and PARU COMMONS
  static const int s_lenMstp = 200;
  static const int s_lenParp = 200;
  static const int s_lenMsti = 200;
  static const int s_lenPari = 200;

  struct PYPARS;
  friend struct PYPARS;
  struct PYPARS {
  int mstp[s_lenMstp];
  double parp[s_lenParp];
  int msti[s_lenMsti];
  double pari[s_lenPari];
  };
  int m_dummy;
  double m_realdummy;
  static PYPARS* s_pypars;
};

#include "Pythia_i/Pypars.icc"

#endif
