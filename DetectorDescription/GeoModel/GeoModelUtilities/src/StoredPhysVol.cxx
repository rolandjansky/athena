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
  : m_physVol(physVol)
{
  physVol->ref();
}

/**
 ** Destructor
 **/
StoredPhysVol::~StoredPhysVol()  {
  m_physVol->unref();
}

GeoFullPhysVol * StoredPhysVol::getPhysVol() {
  return m_physVol;
}


const GeoFullPhysVol *StoredPhysVol::getPhysVol() const{
  return m_physVol;
}

