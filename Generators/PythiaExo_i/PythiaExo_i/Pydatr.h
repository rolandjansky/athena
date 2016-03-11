/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Pydatr_h
#define Pydatr_h

extern "C" {
  void* pydatr_address_(void);
}

/**
@class     Pydatr.hh

@brief Description:
      Class definition for Pydatr, which is used
      to modify the Pythia Pydatr common.

 Environment:
      CDF Run 2

@author
      Chris Green,   Purdue University
      Stan Thompson, Glasgow University    

 Pythia Common access method
*/
class Pydatr {
public:
  Pydatr();
  ~Pydatr();
  int& mrpy(int n);
  double& rrpy(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMrpy() const {return s_lenMrpy;}
  int lenRrpy() const {return s_lenRrpy;}
private: 

  // Lengths of array in PYDATR common
  static const int s_lenMrpy = 6;
  static const int s_lenRrpy = 100;
  struct PYDATR;
  friend struct PYDATR;
  struct PYDATR {
  int mrpy[s_lenMrpy];
  double rrpy[s_lenRrpy];
  };
  int m_dummy;
  double m_realdummy;
  static PYDATR* s_pydatr;
};

#include "PythiaExo_i/Pydatr.icc"

#endif
