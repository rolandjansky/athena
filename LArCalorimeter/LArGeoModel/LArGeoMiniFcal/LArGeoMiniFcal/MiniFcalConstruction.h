/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MiniFcalConstruction_H__
#define __MiniFcalConstruction_H__

#include "CLHEP/Geometry/Transform3D.h"
class GeoFullPhysVol;

namespace LArGeo {

  class MiniFcalConstruction 
  {
  public:
    
    MiniFcalConstruction(bool posZSide=true);
    ~MiniFcalConstruction();
    
    GeoFullPhysVol*        GetEnvelope();
    const HepGeom::Transform3D&  GetTopTransform();

  private:

    GeoFullPhysVol* m_physiMiniFcal;
    HepGeom::Transform3D  m_transform;
    bool            m_posZSide;
    
  };

} // namespace LArGeo

#endif // __MiniFcalConstruction_H__
