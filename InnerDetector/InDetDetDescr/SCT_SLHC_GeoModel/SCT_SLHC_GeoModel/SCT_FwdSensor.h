/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_FWDSENSOR_H
#define SCT_SLHC_GeoModel_SCT_FWDSENSOR_H


#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoPhysVol;

namespace InDetDD{class SiDetectorDesign;}

namespace InDetDDSLHC {

class SCT_FwdSensor : public SCT_UniqueComponentFactory{
 public:
  SCT_FwdSensor(const std::string & name, int ring);
  
  // Thickness of sensor
  double thickness() const {return  m_thickness;}

  // Length of physical sensor
  double length() const {return m_length;} 
  //radius of the sensor (equivalent to the radius of the ring)
  double middleRadius()     const {return m_middleRadius;}
  double innerRadius()      const {return m_innerRadius;}
  double outerRadius()      const {return m_outerRadius;}
  double innerWidth()      const {return m_innerWidth;}
  double outerWidth()      const {return m_outerWidth;}
  double deltaPhi()         const {return m_deltaPhi;}
  double sensorOffset() const {return m_sensorOffset;}

  
  virtual GeoVPhysVol * build(SCT_Identifier id) const;


private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  void makeDesign(); 

  int m_ring; //ring to which this sensor belongs (there is a one to one corresp.)
  double m_thickness;     // Thickness of sensor
  double m_deltaPhi;    // Width of sensor at inner radius
  double m_length;        // Length (radially).
  double m_innerRadius;
  double m_middleRadius;
  double m_outerRadius;
  double m_innerWidth;
  double m_outerWidth;
  double m_activeHalfLength; // Active half length
  
  double m_sensorOffset; // Offset of det element center.
                         // Differs from  m_sensorRadius for truncated middle detector

  const GeoMaterial* m_materialSensor;
  
  int m_chargeCarrier; 

  InDetDD::SiDetectorDesign * m_design;
};
}
#endif //SCT_SLHC_GeoModel_SCT_FWDSENSOR_H
