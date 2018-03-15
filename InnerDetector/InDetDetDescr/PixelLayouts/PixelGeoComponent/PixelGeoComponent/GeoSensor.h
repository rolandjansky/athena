/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoSensor_H
#define InDet_GeoSensor_H

#include "PixelGeoComponent/GeoComponent.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoTransform.h"

#include <vector>
#include <string>

class GeoFullPhysVol;
class GeoPhysVol;

namespace InDet {

  class GeoSensor : public GeoComponent {
  public:

    GeoSensor( GeoFullPhysVol* vol);
    GeoSensor( GeoPhysVol* vol);

    GeoSensor(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int sensorPos);

    virtual ~GeoSensor();

    InDet::GeoSensor* place(const HepGeom::Transform3D& transform, int number, int sensorPos) const;

    const GeoFullPhysVol* getSensorFullPhysVolume() const { return physVolume<GeoFullPhysVol>(); };
    
    int getSensorPos() const { return m_sensorPos; }

  private :
    int m_sensorPos;
  };

} 

#endif
