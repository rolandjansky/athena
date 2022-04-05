/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaEvent/Photon.h"
#include "AthenaKernel/BaseInfo.h"


namespace Analysis {


Photon::~Photon()
= default;


Photon& Photon::operator=(const Photon& rhs) { 
  if ( this != &rhs ) {
    egamma::operator=( rhs );
  }
  return *this; 
}

/** @brief create photon from egamma*/
void Photon::makePhotonFromEgamma(const egamma& eg)
{
  if ( this != &eg ) {
    egamma::operator=(eg);
  }
}


} // namespace Analysis


SG_ADD_BASE (Analysis::Photon, SG_VIRTUAL (IAthenaBarCode));
