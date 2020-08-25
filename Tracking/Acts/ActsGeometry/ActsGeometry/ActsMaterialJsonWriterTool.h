/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSMATERIALJSONWRITERTOOL_H
#define ACTSGEOMETRY_ACTSMATERIALJSONWRITERTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/

// PACKAGE
#include "ActsGeometryInterfaces/IActsMaterialJsonWriterTool.h"

// ACTS
#include "Acts/Plugins/Json/JsonGeometryConverter.hpp"

namespace Acts {
  class TrackingGeometry;
}

class ActsMaterialJsonWriterTool : public extends<AthAlgTool, IActsMaterialJsonWriterTool>
{

public:

  virtual StatusCode initialize() override;

  ActsMaterialJsonWriterTool(const std::string& type, const std::string& name,
                                const IInterface* parent);

  ~ActsMaterialJsonWriterTool();

  virtual
  void
  write(const Acts::JsonGeometryConverter::DetectorMaterialMaps& detMaterial) const override;

  virtual
  void
  write(const Acts::TrackingGeometry& tGeometry) const override;


private:

  Gaudi::Property<std::string> m_filePath{this, "FilePath", "material-maps.json", "Output json file for the Material Map"};

};
#endif
