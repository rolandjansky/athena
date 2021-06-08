/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_SMARTLESSNESSTOOL_H
#define G4ATLASTOOLS_SMARTLESSNESSTOOL_H

// G4Atlas includes
#include "G4AtlasTools/GeometryConfigurationToolBase.h"


/** @class SmartlessnessTool SmartlessnessTool.h "G4AtlasTools/SmartlessnessTool.h"
 *
 *  Class to allow for not-standard material construction
 *
 *  @author Andrea Dell'Acqua
 *  @date   2016-08-16
 */

class SmartlessnessTool : public GeometryConfigurationToolBase
{

public:
  /// Standard constructor
  SmartlessnessTool(const std::string& type, const std::string& name,
                          const IInterface* parent);

  /// Destructor
  virtual ~SmartlessnessTool() = default;

  /// Configure method
  virtual StatusCode preGeometryConfigure() const override final;
  virtual StatusCode postGeometryConfigure() const override final;

private:
  Gaudi::Property<std::map<std::string,double>> m_volumeSmartlessLevel{this, "VolumeSmartlessLevel", {}, "Allow setting of indivial volumes' smartless level"};
};

#endif // G4ATLASTOOLS_SMARTLESSNESSTOOL_H
