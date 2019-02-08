/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_FORWARD_H
#define SCT_SLHC_GeoModel_SCT_FORWARD_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"

#include <vector>

namespace InDetDDSLHC {

class SCT_FwdWheel;
class SCT_ServiceBuilder;

class SCT_Forward : public SCT_UniqueComponentFactory{
  
 public:
  
  SCT_Forward(const std::string & name);
  virtual ~SCT_Forward() = default;

  int    numWheels()    const {return m_numWheels;}
  double innerRadius()  const {return m_innerRadius;} 
  double outerRadius()  const {return m_outerRadius;} 
  double zMax() const {return m_zMax;}
  double zMin() const {return m_zMin;}
  double length() const {return m_length;}
  double zCenter() const {return 0.5*(m_zMin+m_zMax);}
  void   setServiceBuilder(const SCT_ServiceBuilder & builder) {m_serviceBuilder = &builder;}
  
  virtual GeoVPhysVol * build(SCT_Identifier id) const;

 private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  
  // Basic parameters
  int     m_numWheels;
  double  m_innerRadius;
  double  m_intermediateRadius;
  double  m_outerRadius;
  double  m_zMin;
  double  m_zIntermediate;
  double  m_zMax;
  double m_length;
  //Child detector elements
  std::vector<const SCT_FwdWheel* > m_wheels;
  const SCT_ServiceBuilder * m_serviceBuilder;
};
}
#endif //SCT_SLHC_GeoModel_SCT_FWDRING_H

