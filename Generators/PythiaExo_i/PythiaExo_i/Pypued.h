/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYPUED_H
#define PYPUED_H

extern "C"{
  void* pypued_address_(void);
}

/**
@class      Pypued.hh

@brief      Class definition for Pypued, which is used
            to modify the Pythia Pypued common block, which holds variables related
            to the Universal Extra Dimensions model.
@author

            L. Mijovic (liza.mijovic@cern.ch), Oct. 2008

*/

class Pypued 
{
  //______________________________________________________________________
  //public methods and members
  public:

  //constructor and destructor
  Pypued();
  ~Pypued();
  
  int& iued(int n);
  double& rued(int n);

  int lenIued() const {return _lenIued;}
  int lenRued() const {return _lenRued;}

  //PYPUED* _pypued initialization
  inline void init();
  //______________________________________________________________________
  //private methods and members
 private: 
  
  static const int _lenIued = 100;
  static const int _lenRued = 100;

  //PYPUED common block declaration and members
  struct PYPUED;
  friend struct PYPUED;

  struct PYPUED
  {
    int iued[_lenIued];
    double rued[_lenRued];
  };

  int _dummy;
  double _realdummy;

  static PYPUED* _pypued;
};

#include "Pythia_i/Pypued.icc"

#endif
