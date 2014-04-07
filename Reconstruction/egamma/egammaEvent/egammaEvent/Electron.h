// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_ELECTRON_H
#define EGAMMAEVENT_ELECTRON_H 1
/**
  @class Electron
   Class to define the Electron object
  @author 
*/

#include "egammaEvent/egamma.h"

namespace Analysis {

class Electron : public egamma
{
public:
  /** @brief default constructor */
  Electron() : 
    egamma() 
  { };

  /** @brief constructor */
  Electron(unsigned int author) : 
    egamma(author)
  { };

  /** @brief destructor */
  ~Electron();

  Electron(const egamma& rhs, bool copyDetails=true, bool copyMinimal=false) : 
    egamma(rhs, copyDetails, copyMinimal)
  { };

  Electron(const Electron& rhs, bool copyDetails=true, bool copyMinimal=false) : 
    egamma(rhs, copyDetails, copyMinimal)
  { };

  Electron& operator=(const Electron& rhs) ;

  /** @brief create photon from egamma*/
  void makeElectronFromEgamma(const egamma& eg);
};

}

#endif
