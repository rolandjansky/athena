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
  int lenMsub() const {return _lenMsub;}
  int lenCkin() const {return _lenCkin;}
  int lenKfin() const {return _lenKfin;}
  int depthKfin() const {return _depthKfin;}
private: 

  // Lengths of array in PYSUBS common
  static const int _lenMsub = 500;
  static const int _lenCkin = 200;
  static const int _lenKfin = 81;
  static const int _depthKfin = 2;

  struct PYSUBS;
  friend struct PYSUBS;
  struct PYSUBS {
  int msel;
  int padding;
  int msub[_lenMsub];
  int kfin[_lenKfin][_depthKfin];
  double ckin[_lenCkin];
  };
  int _dummy;
  double _realdummy;
  static PYSUBS* _pysubs;
};

#include "Pythia_i/Pysubs.icc"

#endif
