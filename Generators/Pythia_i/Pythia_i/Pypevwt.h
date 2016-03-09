/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYPEVWT_H
#define PYPEVWT_H

extern "C"{
  void* pypevwt_address_(void);
}

/**
@class      Pypevwt.h

@brief      Class definition for Pypevwt, which is used
            to modify the Pythia Pypevwt common block that holds variables related
            to event weighting.
        
@author     Eric Feng (Chicago):  Eric.Feng@cern.ch,   March 2010

*/

class Pypevwt 
{
  //______________________________________________________________________
  //public methods and members
  public:

  //constructor and destructor
  Pypevwt();
  ~Pypevwt();
  
  int& ievwt(int n);
  double& revwt(int n);

  int lenIevwt() const {return s_lenIevwt;}
  int lenRevwt() const {return s_lenRevwt;}

  //PYPEVWT* _pypevwt initialization
  inline void init();
  //______________________________________________________________________
  //private methods and members
 private: 
  
  static const int s_lenIevwt = 100;
  static const int s_lenRevwt = 100;

  //PYPEVWT common block declaration and members
  struct PYPEVWT;
  friend struct PYPEVWT;

  struct PYPEVWT
  {
    int ievwt[s_lenIevwt];
    double revwt[s_lenRevwt];
  };

  int m_dummy;
  double m_realdummy;

  static PYPEVWT* s_pypevwt;
};

#include "Pythia_i/Pypevwt.icc"

#endif
