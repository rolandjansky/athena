/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FORWARD_H
#define SCT_GEOMODEL_SCT_FORWARD_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

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
  int     m_numModuleTypes;
  int     m_numWheels;
  double  m_innerRadius;
  double  m_outerRadius;
  double  m_zMin;
  double  m_zMax;
  double  m_trtGapPos;
  double  m_coolingPipeRadius;
  int     m_numThermalShieldElements;
  bool    m_cylinderServicesPresent;
 
  // Derived parameters
  double m_length;
  double m_outerRadiusCylinderServices;

  // Child detector elements
  std::vector <SCT_FwdWheel *> m_wheels;

  std::vector <SCT_FwdModule *> m_modules;
  
};

#endif //  SCT_GEOMODEL_SCT_FWDRING_H

