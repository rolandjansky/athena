/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_CylindricalEnvelope_H
#define G4ATLASTOOLS_CylindricalEnvelope_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// STL library
#include <string>
#include <vector>

/// @class CylindricalEnvelope
/// @todo TODO needs documentation
class CylindricalEnvelope final : public DetectorGeometryBase
{
 public:
  /// Basic constructor and destructor
  CylindricalEnvelope(const std::string& type, const std::string& name, const IInterface *parent);

  ~CylindricalEnvelope() {}

  /** virtual methods being implemented here */

  virtual void BuildGeometry();

 private:
  std::string m_materialName;
  double m_innerRadius;
  double m_outerRadius;
  double m_dZ;
  double m_startphi;
  double m_deltaphi;

};

#endif
