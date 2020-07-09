/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_CylindricalEnvelope_H
#define G4ATLASTOOLS_CylindricalEnvelope_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

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

  ~CylindricalEnvelope() = default;

  /** virtual methods being implemented here */

  virtual void BuildGeometry();

 private:
  Gaudi::Property<std::string> m_materialName{this, "Material", "Air", "Envelope Material"};
  Gaudi::Property<double> m_innerRadius{this, "InnerRadius", 0., "Envelope inner radius"};
  Gaudi::Property<double> m_outerRadius{this, "OuterRadius", 0., "Envelope outer radius"};
  Gaudi::Property<double> m_dZ{this, "dZ", 0., "Envelope dZ"};
  Gaudi::Property<double> m_startphi{this, "StartPhi", 0., ""};
  Gaudi::Property<double> m_deltaphi{this, "DeltaPhi", 360.*CLHEP::deg, ""};
};

#endif
