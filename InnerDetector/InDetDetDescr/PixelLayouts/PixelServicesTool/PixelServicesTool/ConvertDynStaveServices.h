/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ConvertDynStaveServices_H
#define ConvertDynStaveServices_H

#include "PixelServicesTool/ServiceDynMaterial.h"
#include "PixelServicesTool/DetTypeDyn.h"

#include <string>

class StaveDynServices;

class ConvertDynStaveServices {
public:

  ServiceDynMaterial convertStaveServices( const StaveDynServices& stServ) const;
  
  void addCooling( DetTypeDyn::Type type, DetTypeDyn::Part part, int layerNumber,
		   int nStaves, ServiceDynMaterial& mat);

  std::string constructName( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer) const;

  std::string getHvCableName( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer) const;
  std::string getDcsCableName( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer) const;
  std::string getDataCableName( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer) const;

  std::string getLvConductorName( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer) const;
  std::string getLvInsulatorName( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer) const;

};

#endif
