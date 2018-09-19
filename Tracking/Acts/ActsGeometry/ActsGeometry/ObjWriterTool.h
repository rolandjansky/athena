/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_OBJWRITERTOOL_H
#define ACTSGEOMETRY_OBJWRITERTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/


namespace Acts {

class TrackingGeometry;
class ObjTrackingGeometryWriter;

static const InterfaceID IID_ACTSObjWriterTool("ACTSObjWriterTool", 1, 0);

class ObjWriterTool : public AthAlgTool
{

public:

  static const InterfaceID& interfaceID(){return IID_ACTSObjWriterTool;}

  StatusCode initialize() override;

  ObjWriterTool(const std::string& type, const std::string& name,
    const IInterface* parent);
  
  void
  write(const Acts::TrackingGeometry&);

private:
  
  Gaudi::Property<std::string> m_outputDirectory{this, "OutputDirectory", ".", ""};
  Gaudi::Property<std::vector<std::string>> m_subDetectors{this, "SubDetectors", {}, ""};

  std::shared_ptr<ObjTrackingGeometryWriter> m_tgObjWriter;


};

}


#endif
