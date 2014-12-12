/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_FWDMODULE_H
#define SCT_SLHC_GeoModel_SCT_FWDMODULE_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"
#include "SCT_SLHC_GeoModel/SCT_Identifier.h"
#include "SCT_SLHC_GeoModel/SCT_FwdSensor.h"

#include <string>
#include <cmath>


class GeoTransform;
class GeoVPhysVol;
class GeoPhysVol;

namespace InDetDDSLHC {

class SCT_FwdSpine;

class SCT_FwdModule : public SCT_UniqueComponentFactory{
 public:
  
  SCT_FwdModule(const std::string & name, int ring, int doubleSided);
  ~SCT_FwdModule();
  SCT_FwdModule & operator=(const SCT_FwdModule &) = delete;
  SCT_FwdModule(const SCT_FwdModule &) = delete;
  //GeoVPhysVol* build(SCT_Identifier id, GeoTransform * position, GeoPhysVol * world) ;
  virtual GeoVPhysVol* build(SCT_Identifier id) const;
  
  double stereoAngle() const {return m_stereoAngle;}
  double moduleShift() const {return m_moduleShift;}	
  double thickness()   const {return m_thickness;}
  double length()      const {return m_length;}
  double innerRadius() const {return m_innerRadius;}
  double outerRadius() const {return m_outerRadius;}
  double deltaPhi()    const {return m_deltaPhi;}

  
 private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  
  int m_ring; //ring to which this module belongs (there is a one to one corresp.)
  int    m_doubleSided;
  double m_stereoAngle;      // Magnitude of stereo angle
  double m_length;
  double m_innerRadius;
  double m_middleRadius;
  double m_outerRadius;
  double m_innerWidth;
  double m_outerWidth;
  double m_deltaPhi;
  double m_thickness;
  double m_moduleShift;
  double m_interSidesGap;
  // public:	  
  // Child detector elements
  SCT_FwdSensor* m_sensor;
  const SCT_FwdSpine* m_spine;
};
}
#endif //SCT_SLHC_GeoModel_SCT_FWDMODULE_H
