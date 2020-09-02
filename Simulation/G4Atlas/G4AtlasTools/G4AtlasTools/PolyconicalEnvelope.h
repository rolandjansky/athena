/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_PolyconicalEnvelope_H
#define G4ATLASTOOLS_PolyconicalEnvelope_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// STL library
#include <string>
#include <vector>

/// @class PolyconicalEnvelope
/// @todo TODO needs documentation
class PolyconicalEnvelope final : public DetectorGeometryBase
{
 public:
  // Basic constructor and destructor
  PolyconicalEnvelope(const std::string& type, const std::string& name, const IInterface *parent);
  ~PolyconicalEnvelope() = default;

  /** virtual methods being implemented here */

  virtual void BuildGeometry() override final;

 private:
  Gaudi::Property<std::string> m_materialName{this, "Material", "Air", "Envelope Material"};
  Gaudi::Property<unsigned int> m_surfaceNr{this, "NSurfaces", 0, "Number of surfaces in Z"};
  Gaudi::Property<std::vector<double> > m_innerRadii{this, "InnerRadii", {}, "Envelope inner radii"};
  Gaudi::Property<std::vector<double> > m_outerRadii{this, "OuterRadii", {}, "Envelope outer radii"};
  Gaudi::Property<std::vector<double> > m_ZSurfaces{this, "ZSurfaces", {}, "Envelope surface Zs"};
  Gaudi::Property<double> m_startphi{this, "StartPhi", 0., ""};
  Gaudi::Property<double> m_deltaphi{this, "DeltaPhi", 360.*CLHEP::deg, ""};
};

#endif
