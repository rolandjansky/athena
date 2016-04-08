/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCRATE_LARCRATETOOL_H
#define LARCRATE_LARCRATETOOL_H

// Base class header
#include "G4AtlasTools/DetectorGeometryBase.h"

// STL library
#include <string>

/** @class LArCrateTool LArCrateTool.h "LarCrate/LArCrateTool.h"
*
*  Tool for building the LArCrate detector.
*/

class LArCrateTool final : public DetectorGeometryBase
{
public:
  // Basic constructor and destructor
  LArCrateTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~LArCrateTool() {}

  /** virtual methods being implemented here */
  virtual void BuildGeometry() override final;

private:
  double m_zLength;
  double m_yLength;
  double m_xLength;
};

#endif //LARCRATE_LARCRATETOOL_H
