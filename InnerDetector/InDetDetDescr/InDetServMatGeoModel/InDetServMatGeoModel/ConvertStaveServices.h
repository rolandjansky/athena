/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ConvertStaveServices_H
#define ConvertStaveServices_H

#include "InDetServMatGeoModel/ServiceMaterial.h"
#include "InDetServMatGeoModel/DetType.h"

#include <string>

class StaveServices;

class ConvertStaveServices {
public:

  ServiceMaterial convertStaveServices( const StaveServices& stServ) const;
  
  void addCooling( DetType::Type type, DetType::Part part, int layerNumber,
		   int nStaves, ServiceMaterial& mat);

  std::string constructName( DetType::Type type, DetType::Part part, int layer) const;

  std::string getHvCableName( DetType::Type type, DetType::Part part, int layer) const;
  std::string getDcsCableName( DetType::Type type, DetType::Part part, int layer) const;
  std::string getDataCableName( DetType::Type type, DetType::Part part, int layer) const;

  std::string getLvConductorName( DetType::Type type, DetType::Part part, int layer) const;
  std::string getLvInsulatorName( DetType::Type type, DetType::Part part, int layer) const;

};

#endif
