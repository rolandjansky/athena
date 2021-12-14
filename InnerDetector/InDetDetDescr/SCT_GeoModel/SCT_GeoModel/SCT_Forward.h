/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FORWARD_H
#define SCT_GEOMODEL_SCT_FORWARD_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <memory>
#include <vector>

class SCT_FwdModule;
class SCT_FwdWheel;

class SCT_Forward : public SCT_UniqueComponentFactory
{

public:
  
  SCT_Forward(const std::string & name, int ec,
              InDetDD::SCT_DetectorManager* detectorManager,
              const SCT_GeometryManager* geometryManager,
              SCT_MaterialManager* materials);
  ~SCT_Forward();

  //
  // Methods to return basic and derived parameters. 
  //
  // Retrieve basic parameters
  //
  
  int    numModuleTypes() const {return m_numModuleTypes;}
  int    numWheels()    const {return m_numWheels;}
  double innerRadius()  const {return m_innerRadius;} 
  double outerRadius()  const {return m_outerRadius;} 
  double zMax() const {return m_zMax;}
  double zMin() const {return m_zMin;}
  double trtGapPos() const {return m_trtGapPos;}

  //
  // Retrieve derived parameters
  //
  double length() const {return m_length;}
  double zCenter() const {return 0.5*(m_zMin+m_zMax);}

  virtual GeoVPhysVol * build(SCT_Identifier id);


 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  // Basic parameters
  int     m_endcap;
  int     m_numModuleTypes = 0;
  int     m_numWheels = 0;
  double  m_innerRadius = 0.0;
  double  m_outerRadius = 0.0;
  double  m_zMin = 0.0;
  double  m_zMax = 0.0;
  double  m_trtGapPos = 0.0;
  double  m_coolingPipeRadius = 0.0;
  int     m_numThermalShieldElements = 0;
  bool    m_cylinderServicesPresent = false;
 
  // Derived parameters
  double m_length = 0.0;
  double m_outerRadiusCylinderServices = 0.0;

  // Child detector elements
  std::vector<std::unique_ptr<SCT_FwdWheel>> m_wheels;

  std::vector<std::unique_ptr<SCT_FwdModule>> m_modules;
  
};

#endif //  SCT_GEOMODEL_SCT_FWDRING_H

