/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_SKIAUX_H
#define SCT_GEOMODEL_SCT_SKIAUX_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class SCT_Ski;
// 14th Aug 2005 S.Mima modified.
class SCT_Bracket;
class SCT_Harness;
class SCT_SkiPowerTape;


class SCT_SkiAux : public SCT_SharedComponentFactory
{

public:


  SCT_SkiAux(const std::string & name,
             SCT_Ski * ski,
             SCT_Bracket * bracket,
             SCT_Harness * harness,
             SCT_SkiPowerTape * skiPowerTape,
             double innerRadius,
             double bracketPhiOffset, 
             double powerTapePhiOffset,
             double divisionAngle,
             InDetDD::SCT_DetectorManager* detectorManager,
             const SCT_GeometryManager* geometryManager,
             SCT_MaterialManager* materials);
  
  //
  // Retrieve basic/derived parameters
  //
  double innerRadius()      const {return m_innerRadius;}
  double outerRadius()      const {return m_outerRadius;}
  double length()           const {return m_length;}
  double sectorStartAngle() const {return m_sectorStartAngle;}
  double sectorAngle()      const {return m_sectorAngle;}

  double bracketPhiOffset()   const {return m_bracketPhiOffset;} 
  double powerTapePhiOffset() const {return m_powerTapePhiOffset;} 


  // Retrieve child elements
  const SCT_Ski *         ski()          const {return m_ski;}
  // 14th Aug 2005 S.Mima modified.
  //  const SCT_Bracket *     bracket()      const {return m_bracket;}
  const SCT_Bracket *     bracket()      const {return m_bracket;}
  const SCT_Harness *     harness()      const {return m_harness;}
  const SCT_SkiPowerTape* skiPowerTape() const {return m_skiPowerTape;}


private:  
  void getParameters();
  virtual GeoVPhysVol * build();
 
  // Basic/derived  parameters
  double m_innerRadius;
  double m_outerRadius;
  double m_length;
  double m_bracketPhiOffset;
  double m_powerTapePhiOffset;
  double m_sectorStartAngle;
  double m_sectorAngle;
 
  // Child detector elements
  SCT_Ski * m_ski;
  SCT_Bracket * m_bracket;
  SCT_Harness * m_harness;
  SCT_SkiPowerTape * m_skiPowerTape;
 
};

#endif // SCT_GEOMODEL_SCT_SKIAUX_H

