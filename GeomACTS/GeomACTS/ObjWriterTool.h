#ifndef GEOMACTS_ACTS_OBJWRITERTOOL_H
#define GEOMACTS_ACTS_OBJWRITERTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"


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
  
  std::string m_outputDirectory;
  std::vector<std::string> m_subDetectors;

  std::shared_ptr<ObjTrackingGeometryWriter> m_tgObjWriter;


};

}


#endif
