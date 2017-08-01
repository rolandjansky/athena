/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_BoxEnvelope_H
#define G4ATLASTOOLS_BoxEnvelope_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// Members

// STL library
#include <string>
#include <vector>
#include <map>

/// @class BoxEnvelope
/// @todo TODO needs documentation
class BoxEnvelope final : public DetectorGeometryBase
{
 public:
  /// Basic constructor and destructor
  BoxEnvelope(const std::string& type, const std::string& name, const IInterface *parent);
  ~BoxEnvelope() {}

  /** Athena method. called at initialization time, being customized here */
  //StatusCode initialize() override final;

  /** virtual methods being implemented here */

  virtual void BuildGeometry() override final;

 private:
  double m_dX;
  double m_dY;
  double m_dZ;
  std::string m_materialName;
  unsigned int m_numberOfHoles;
  std::vector<std::string> m_holeNames;
  std::vector<double> m_hole_dX;
  std::vector<double> m_hole_dY;
  std::vector<double> m_hole_dZ;
  std::vector<double> m_holePosX;
  std::vector<double> m_holePosY;
  std::vector<double> m_holePosZ;
};

#endif
