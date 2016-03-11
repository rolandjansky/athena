/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Pysubs_h
#define Pysubs_h

extern "C" {
  void* pysubs_address_(void);
}

/**
@class      Pysubs.hh

@brief      Class definition for Pysubs, which is used
      to modify the Pythia Pysubs common.

 Environment:
     CDF Run 2

@author      Chris Green,   Purdue University
      Stan Thompson, Glasgow University    

 Pythia Common access method
*/

class Pysubs {
public:
  Pysubs();
  ~Pysubs();
  int& msel(); 
  int& msub(int n);
  int& kfin(int iside, int jflav);
  double& ckin(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMsub() const {return s_lenMsub;}
  int lenCkin() const {return s_lenCkin;}
  int lenKfin() const {return s_lenKfin;}
  int depthKfin() const {return s_depthKfin;}
private: 

  // Lengths of array in PYSUBS common
  static const int s_lenMsub = 500;
  static const int s_lenCkin = 200;
  static const int s_lenKfin = 81;
  static const int s_depthKfin = 2;

  struct PYSUBS;
  friend struct PYSUBS;
  struct PYSUBS {
  int msel;
  int padding;
  int msub[s_lenMsub];
  int kfin[s_lenKfin][s_depthKfin];
  double ckin[s_lenCkin];
  };
  int m_dummy;
  double m_realdummy;
  static PYSUBS* s_pysubs;
};

#include "PythiaExo_i/Pysubs.icc"

#endif
