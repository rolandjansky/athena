/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  ~PolyconicalEnvelope() {}

  /** virtual methods being implemented here */

  virtual void BuildGeometry() override final;

 private:
  std::string m_materialName;
  unsigned int m_surfaceNr;
  std::vector<double> m_innerRadii;
  std::vector<double> m_outerRadii;
  std::vector<double> m_ZSurfaces;
  double m_startphi;
  double m_deltaphi;

};

#endif
