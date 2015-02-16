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

  int lenIevwt() const {return _lenIevwt;}
  int lenRevwt() const {return _lenRevwt;}

  //PYPEVWT* _pypevwt initialization
  inline void init();
  //______________________________________________________________________
  //private methods and members
 private: 
  
  static const int _lenIevwt = 100;
  static const int _lenRevwt = 100;

  //PYPEVWT common block declaration and members
  struct PYPEVWT;
  friend struct PYPEVWT;

  struct PYPEVWT
  {
    int ievwt[_lenIevwt];
    double revwt[_lenRevwt];
  };

  int _dummy;
  double _realdummy;

  static PYPEVWT* _pypevwt;
};

#include "Pythia_i/Pypevwt.icc"

#endif
