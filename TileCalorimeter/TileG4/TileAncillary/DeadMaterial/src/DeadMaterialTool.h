/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DEADMATERIAL_DEADMATERIALTOOL_H
#define DEADMATERIAL_DEADMATERIALTOOL_H

// Base class header
#include "G4AtlasTools/DetectorGeometryBase.h"

// STL library
#include <string>

/** @class DeadMaterialTool DeadMaterialTool.h "DeadMaterial/DeadMaterialTool.h"
*
*  Tool for building the DeadMaterial detector.
*/

class DeadMaterialTool final : public DetectorGeometryBase
{
public:
  // Basic constructor and destructor
  DeadMaterialTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~DeadMaterialTool();

  /** virtual methods being implemented here */
  virtual void BuildGeometry() override final;

private:
  double m_zLength;
  double m_yLength;
  double m_xLength;
};

#endif //DEADMATERIAL_DEADMATERIALTOOL_H
