/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_MaterialDescriptionTool_H
#define G4ATLASTOOLS_MaterialDescriptionTool_H

// G4Atlas includes
#include "G4AtlasTools/GeometryConfigurationToolBase.h"


/** @class MaterialDescriptionTool MaterialDescriptionTool.h "G4AtlasTools/MaterialDescriptionTool.h"
 *
 *  Class to allow for not-standard material construction
 *
 *  @author Andrea Dell'Acqua
 *  @date   2016-08-16
 */

class MaterialDescriptionTool : public GeometryConfigurationToolBase
{

public:
  /// Standard constructor
  MaterialDescriptionTool(const std::string& type, const std::string& name,
                          const IInterface* parent);

  /// Destructor
  virtual ~MaterialDescriptionTool(){}


  /// Configure method
  virtual StatusCode preGeometryConfigure() const override final;
  virtual StatusCode postGeometryConfigure() const override final;

};

#endif // G4ATLASTOOLS_MaterialDescriptionTool_H
