// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_PHOTON_H
#define EGAMMAEVENT_PHOTON_H 1
/**
  @class Photon
   Class to define the Photon object
  @author 
*/

#include "egammaEvent/egamma.h"

namespace Analysis {

class Photon : public egamma
{
public:

  /** @brief default constructor */
  Photon() :
    egamma() 
  { };

  /** @brief constructor */
  Photon(unsigned int author) : 
    egamma(author)
  { };

  /** @brief destructor */
  ~Photon();

  Photon(const egamma& rhs, bool copyDetails=true, bool copyMinimal=false) : 
    egamma(rhs, copyDetails, copyMinimal)
  { };

  Photon(const Photon& rhs, bool copyDetails=true, bool copyMinimal=false) : 
    egamma(rhs, copyDetails, copyMinimal)
  { };

  Photon& operator=(const Photon& rhs);

  /** @brief create photon from egamma*/
  void makePhotonFromEgamma(const egamma& eg);
};

}

#endif
