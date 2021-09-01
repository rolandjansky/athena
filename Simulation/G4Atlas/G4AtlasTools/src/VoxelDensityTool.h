/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_VOXELDENSITYTOOL_H
#define G4ATLASTOOLS_VOXELDENSITYTOOL_H

// G4Atlas includes
#include "G4AtlasTools/GeometryConfigurationToolBase.h"


/** @class VoxelDensityTool VoxelDensityTool.h "G4AtlasTools/VoxelDensityTool.h"
 *
 *  Class to allow for not-standard material construction
 *
 *  @author Andrea Dell'Acqua
 *  @date   2016-08-16
 */

class VoxelDensityTool : public GeometryConfigurationToolBase
{

public:
  /// Standard constructor
  VoxelDensityTool(const std::string& type, const std::string& name,
                          const IInterface* parent);

  /// Destructor
  virtual ~VoxelDensityTool() = default;

  /// Configure method
  virtual StatusCode preGeometryConfigure() const override final;
  virtual StatusCode postGeometryConfigure() const override final;

private:
  Gaudi::Property<std::map<std::string,double>> m_volumeVoxelDensityLevel{this, "VolumeVoxellDensityLevel", {}, "Allow setting of indivial volumes' voxel density level"};
};

#endif // G4ATLASTOOLS_VOXELDENSITYTOOL_H
