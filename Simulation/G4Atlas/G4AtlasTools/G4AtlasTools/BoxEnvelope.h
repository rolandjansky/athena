/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  ~BoxEnvelope() = default;

  /** virtual methods being implemented here */

  virtual void BuildGeometry() override final;

 private:
  Gaudi::Property<double> m_dX{this, "dX", 0., "Envelope dX"};
  Gaudi::Property<double> m_dY{this, "dY", 0., "Envelope dY"};
  Gaudi::Property<double> m_dZ{this, "dZ", 0., "Envelope dZ"};
  Gaudi::Property<std::string> m_materialName{this, "Material", "Air", "Envelope Material"};
  Gaudi::Property<unsigned int> m_numberOfHoles{this, "NumberOfHoles", 0, "Number of hole volumes to subtract from the initial envelope volume."};
  Gaudi::Property<std::vector<std::string> > m_holeNames{this, "HoleNames", {} };
  Gaudi::Property<std::vector<double> > m_hole_dX{this, "Hole_dX", {} };
  Gaudi::Property<std::vector<double> > m_hole_dY{this, "Hole_dY", {} };
  Gaudi::Property<std::vector<double> > m_hole_dZ{this, "Hole_dZ", {} };
  Gaudi::Property<std::vector<double> > m_holePosX{this, "HolePosX", {} };
  Gaudi::Property<std::vector<double> > m_holePosY{this, "HolePosY", {} };
  Gaudi::Property<std::vector<double> > m_holePosZ{this, "HolePosZ", {} };
};

#endif
