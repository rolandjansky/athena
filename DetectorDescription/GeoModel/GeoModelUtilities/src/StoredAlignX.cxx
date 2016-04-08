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
  : m_alignX(alignX)
{
  alignX->ref();
}

/**
 ** Destructor
 **/
StoredAlignX::~StoredAlignX()  {
  m_alignX->unref();
}

GeoAlignableTransform * StoredAlignX::getAlignX() {
  return m_alignX;
}


const GeoAlignableTransform *StoredAlignX::getAlignX() const{
  return m_alignX;
}

