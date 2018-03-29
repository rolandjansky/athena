/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_GeometryConfigurationToolBase_H
#define G4ATLASTOOLS_GeometryConfigurationToolBase_H

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// G4Atlas includes
#include "G4AtlasInterfaces/IG4GeometryConfigurationTool.h"


/** @class GeometryConfigurationToolBase GeometryConfigurationToolBase.h "G4AtlasTools/GeometryConfigurationToolBase.h"
 *
 *  Tool base class to allow for geometry customization
 *
 *  @author Andrea Dell'Acqua
 *  @date   2016-08-16
 */

class GeometryConfigurationToolBase : public extends<AthAlgTool, IG4GeometryConfigurationTool>
{

public:
  /// Standard constructor
  GeometryConfigurationToolBase(const std::string& type, const std::string& name,
                                const IInterface* parent);

  /// Destructor
  virtual ~GeometryConfigurationToolBase(){}

  /// Initialize method
  virtual StatusCode initialize( ) override;

  /// Configure methods
  virtual StatusCode preGeometryConfigure() const override;
  virtual StatusCode postGeometryConfigure() const override;

};

#endif // G4ATLASTOOLS_GeometryConfigurationTToolBase_H
