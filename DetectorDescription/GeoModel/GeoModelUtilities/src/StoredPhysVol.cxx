/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
/**
 ** Constructor(s)
 **/
StoredPhysVol::StoredPhysVol( GeoFullPhysVol * physVol )
  : _physVol(physVol)
{
  physVol->ref();
}

/**
 ** Destructor
 **/
StoredPhysVol::~StoredPhysVol()  {
  _physVol->unref();
}

GeoFullPhysVol * StoredPhysVol::getPhysVol() {
  return _physVol;
}


const GeoFullPhysVol *StoredPhysVol::getPhysVol() const{
  return _physVol;
}

