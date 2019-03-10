/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_GeneralParameters_H
#define SCT_GeoModel_SCT_GeneralParameters_H

#include "GeoModelKernel/GeoDefinitions.h"

#include <map>
#include <string>

class SCT_DataBase;
class TopLevelPlacements;

class SCT_GeneralParameters {

public:

  SCT_GeneralParameters(SCT_DataBase* rdb);
  ~SCT_GeneralParameters();
  //Explicitly disallow copy, assignment to appease coverity
  SCT_GeneralParameters(const SCT_GeneralParameters &) = delete;
  SCT_GeneralParameters & operator=(const SCT_GeneralParameters &) = delete;

  // General
  double safety() const;

  //Default conditions.
  double temperature() const;
  double biasVoltage() const;
  double depletionVoltage() const;

  const GeoTrf::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;
  
private:

  SCT_DataBase * m_rdb;
  TopLevelPlacements * m_placements;
    
};


#endif // SCT_GeoModel_SCT_GeneralParameters_H
