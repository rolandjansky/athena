/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef Ffphot_h
#define Ffphot_h

extern "C" {
  void* ffphot_address_(void);
}

/**

@class  Ffphot.h

@brief      Class definition for ffphot, which is used
      to modify the Tauola FFPHOT common.

@author          B. P. Kersevan

 Ffphot Common access method

*/

class Ffphot {
public:
  Ffphot();
  ~Ffphot();
  int& ifpmode();
  float& ffphcut();
  float& ffalpha();
  int& ifinter();
  int& ifsec();
  int& ifitre();
  int& ifiexp();
  int& iftops();

  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
 
private: 

  struct FFPHOT;
  friend struct FFPHOT;

  struct FFPHOT {
    int ifpmode;
    float ffphcut;
    float ffalpha;
    int ifinter;
    int ifsec;
    int ifitre;
    int ifiexp;
    int iftops;
  };

  static FFPHOT* s_ffphot;
};

#include "Photos_i/Ffphot.icc"

#endif
