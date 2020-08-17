/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSOBJWRITERTOOL_H
#define ACTSGEOMETRY_ACTSOBJWRITERTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/


namespace Acts {

class TrackingGeometry;
class ObjTrackingGeometryWriter;

}

static const InterfaceID IID_ActsObjWriterTool("ActsObjWriterTool", 1, 0);

class ActsObjWriterTool : public AthAlgTool
{

public:

  static const InterfaceID& interfaceID(){return IID_ActsObjWriterTool;}

  StatusCode initialize() override;

  ActsObjWriterTool(const std::string& type, const std::string& name,
    const IInterface* parent);

  void
  write(const ActsGeometryContext& gctx, const Acts::TrackingGeometry&) const;

private:

  Gaudi::Property<std::string> m_outputDirectory{this, "OutputDirectory", ".", ""};
  Gaudi::Property<std::vector<std::string>> m_subDetectors{this, "SubDetectors", {}, ""};

  std::shared_ptr<Acts::ObjTrackingGeometryWriter> m_tgObjWriter;


};

#endif
