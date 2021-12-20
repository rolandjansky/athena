/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDCYLINDERSERVICES_H
#define SCT_GEOMODEL_SCT_FWDCYLINDERSERVICES_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <vector>

class SCT_FwdCylinderServices : public SCT_SharedComponentFactory
{

public:
  
  SCT_FwdCylinderServices(const std::string & name, double rmin, double rmax, double length,
                          InDetDD::SCT_DetectorManager* detectorManager,
                          const SCT_GeometryManager* geometryManager,
                          SCT_MaterialManager* materials);
  ~SCT_FwdCylinderServices();

  //
  // Methods to return basic and derived parameters. 
  double innerRadius()  const {return m_innerRadius;} 
  double outerRadius()  const {return m_outerRadius;} 
  double length() const {return m_length;}
 
private:
  void getParameters();
  virtual GeoVPhysVol * build();

  // Basic parameters
  double  m_innerRadius;
  double  m_outerRadius;
  double  m_length;

  // Parameters of daughters 
  std::vector<double> m_coolingLocAngle;
  std::vector<double> m_lmtLocAngle;
  std::vector<double> m_lmtCoolingLocAngle;
  std::vector<double> m_fibreLocAngle;
  std::vector<double> m_nPipeLocAngle;
  std::vector<double> m_railLocAngle;

  double  m_coolingDeltaR = 0.0;
  double  m_lmtDeltaR = 0.0;
  double  m_lmtCoolingDeltaR = 0.0;
  double  m_fibreDeltaR = 0.0;
  double  m_nPipeDeltaR = 0.0;
  double  m_railDeltaR = 0.0;

  double  m_coolingRPhi = 0.0;
  double  m_lmtRPhi = 0.0;
  double  m_lmtCoolingRPhi = 0.0;
  double  m_fibreRPhi = 0.0;
  double  m_nPipeRPhi = 0.0;
  double  m_railRPhi = 0.0;

  std::string m_coolingMaterialName;
  std::string m_lmtMaterialName;
  std::string m_lmtCoolingMaterialName;
  std::string m_fibreMaterialName;
  std::string m_nPipeMaterialName;
  std::string m_railMaterialName;

  double  m_lmtCoolingZOffset = 0.0;

};

#endif //  SCT_GEOMODEL_SCT_FWDCYLINDERSERVICES_H

