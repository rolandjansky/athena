/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDMODULE_H
#define SCT_GEOMODEL_SCT_FWDMODULE_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include "SCT_GeoModel/SCT_FwdSensor.h"

#include <memory>
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
  
  double m_stereoAngle = 0.0;      // Magnitude of stereo angle
  double m_length = 0.0;
  double m_widthOuter = 0.0;
  double m_widthInner = 0.0;
  double m_thickness = 0.0;
  double m_moduleShift = 0.0;
  double m_glueThickness = 0.0;
  double m_distBtwMountPoints = 0.0;
  double m_mountPointToCenter = 0.0;
  bool   m_hybridIsOnInnerEdge = false;
  int    m_upperSide = 0;
  
  double m_innerRadius = 0.0;
  double m_outerRadius = 0.0;
  double m_powerTapeStart = 0.0;

  double m_mainMountPoint = 0.0;
  double m_secMountPoint = 0.0;
  double m_endLocator = 0.0;

  bool   m_connectorPresent = false;

public:	  
  // Child detector elements
  std::unique_ptr<SCT_FwdSensor> m_sensor;
  std::unique_ptr<SCT_FwdModuleConnector> m_connector;
  std::unique_ptr<SCT_FwdHybrid> m_hybrid;
  std::unique_ptr<SCT_FwdSpine> m_spine;
  std::unique_ptr<SCT_FwdSubSpine> m_subspineL;
  std::unique_ptr<SCT_FwdSubSpine> m_subspineR;
};
    
#endif //  SCT_GEOMODEL_SCT_FWDMODULE_H
