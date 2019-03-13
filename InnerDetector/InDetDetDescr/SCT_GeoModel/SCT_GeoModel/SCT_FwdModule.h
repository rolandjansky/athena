/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDMODULE_H
#define SCT_GEOMODEL_SCT_FWDMODULE_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include "SCT_GeoModel/SCT_FwdSensor.h"

#include <string>

class SCT_Identifier;
class SCT_FwdModuleConnector;
class SCT_FwdHybrid;
class SCT_FwdSpine;
class SCT_FwdSubSpine;

class GeoTransform;
class GeoVPhysVol;
class GeoPhysVol;


class SCT_FwdModule : public SCT_UniqueComponentFactory 
{
public:

  SCT_FwdModule(const std::string & name, int ringType,
                InDetDD::SCT_DetectorManager* detectorManager,
                const SCT_GeometryManager* geometryManager,
                SCT_MaterialManager* materials);
  ~SCT_FwdModule();
  //explicitly disallow copy, assignment to appease coverity
  SCT_FwdModule(const SCT_FwdModule &) = delete;
  SCT_FwdModule & operator=(const SCT_FwdModule &) = delete;
  
  // Ring type
  int ringType() const {return m_ringType;}

  virtual GeoVPhysVol* build(SCT_Identifier id);

  double stereoAngle() const {return m_stereoAngle;}

  double moduleShift()      const {return m_moduleShift;}	

  double length()     const {return m_length;}
  double outerWidth() const {return m_widthOuter;}
  double innerWidth() const {return m_widthInner;}
  double thickness()  const {return m_thickness;}

  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 

  double centerRadius() const {return 0.5 * (m_innerRadius + m_outerRadius);}
  double sensorCenterRadius() const {return m_sensor->centerRadius();}
  double powerTapeStart() const {return m_powerTapeStart;}

  double mainMountPointRadius() const {return m_mainMountPoint;}
  double secMountPointRadius() const {return m_secMountPoint;}
  double endModuleRadius() const {return m_endLocator;}

private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

//  const std::string & m_name;
  int    m_ringType;
  
  double m_stereoAngle;      // Magnitude of stereo angle
  double m_length;
  double m_widthOuter;
  double m_widthInner;
  double m_thickness;
  double m_moduleShift;
  double m_glueThickness;
  double m_distBtwMountPoints;
  double m_mountPointToCenter;
  bool   m_hybridIsOnInnerEdge;
  int    m_upperSide;
  
  double m_innerRadius;
  double m_outerRadius;
  double m_powerTapeStart;

  double m_mainMountPoint;
  double m_secMountPoint;
  double m_endLocator;

  bool   m_connectorPresent;

public:	  
  // Child detector elements
  SCT_FwdSensor    * m_sensor;
  SCT_FwdModuleConnector    * m_connector;
  SCT_FwdHybrid    * m_hybrid;
  SCT_FwdSpine     * m_spine;
  SCT_FwdSubSpine  * m_subspineL;
  SCT_FwdSubSpine  * m_subspineR;
};
    
#endif //  SCT_GEOMODEL_SCT_FWDMODULE_H
