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

  double  m_coolingDeltaR;
  double  m_lmtDeltaR;
  double  m_lmtCoolingDeltaR;
  double  m_fibreDeltaR;
  double  m_nPipeDeltaR;
  double  m_railDeltaR;

  double  m_coolingRPhi;
  double  m_lmtRPhi;
  double  m_lmtCoolingRPhi;
  double  m_fibreRPhi;
  double  m_nPipeRPhi;
  double  m_railRPhi;

  std::string m_coolingMaterialName;
  std::string m_lmtMaterialName;
  std::string m_lmtCoolingMaterialName;
  std::string m_fibreMaterialName;
  std::string m_nPipeMaterialName;
  std::string m_railMaterialName;

  double  m_lmtCoolingZOffset;

};

#endif //  SCT_GEOMODEL_SCT_FWDCYLINDERSERVICES_H

