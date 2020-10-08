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

  Acts::JsonGeometryConverter::Config m_cfg;

  Gaudi::Property<std::string> m_filePath{this, "OutputFile", "material-maps.json", "Output json file for the Material Map"};
  Gaudi::Property<bool> m_processSensitives{this, "processSensitives", true, "Write sensitive surface to the json file"};
  Gaudi::Property<bool> m_processApproaches{this, "processApproaches", true, "Write approche surface to the json file"};
  Gaudi::Property<bool> m_processRepresenting{this, "processRepresenting", true, "Write representing surface to the json file"};
  Gaudi::Property<bool> m_processBoundaries{this, "processBoundaries", true, "Write boundary surface to the json file"};
  Gaudi::Property<bool> m_processVolumes{this, "processVolumes", true, "Write volume to the json file"};
  Gaudi::Property<bool> m_processDenseVolumes{this, "processDenseVolumes", false, "Write dense volume to the json file"};
  Gaudi::Property<bool> m_processnonmaterial{this, "processnonmaterial", false, "Add proto material to all surfaces and volumes"};
};

#endif
