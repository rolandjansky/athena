/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/StoredAlignX.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
/**
 ** Constructor(s)
 **/
StoredAlignX::StoredAlignX( GeoAlignableTransform * alignX )
  : _alignX(alignX)
{
  alignX->ref();
}

/**
 ** Destructor
 **/
StoredAlignX::~StoredAlignX()  {
  _alignX->unref();
}

GeoAlignableTransform * StoredAlignX::getAlignX() {
  return _alignX;
}


const GeoAlignableTransform *StoredAlignX::getAlignX() const{
  return _alignX;
}

