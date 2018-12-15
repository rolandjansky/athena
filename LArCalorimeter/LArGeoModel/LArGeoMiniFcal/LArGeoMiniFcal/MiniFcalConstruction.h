/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEOMINIFCAL_MINIFCALCONSTRUCTION_H
#define LARGEOMINIFCAL_MINIFCALCONSTRUCTION_H

#include "GeoModelKernel/GeoDefinitions.h"
class GeoFullPhysVol;

namespace LArGeo {

  class MiniFcalConstruction 
  {
  public:
    
    MiniFcalConstruction(bool posZSide=true);
    ~MiniFcalConstruction();
    
    GeoFullPhysVol*        GetEnvelope();
    const GeoTrf::Transform3D&  GetTopTransform();

  private:

    GeoFullPhysVol*      m_physiMiniFcal;
    GeoTrf::Transform3D  m_transform;
    bool                 m_posZSide;
    
  };

} // namespace LArGeo

#endif 
