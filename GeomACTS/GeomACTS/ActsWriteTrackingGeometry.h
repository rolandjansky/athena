#ifndef GeomACTS_ActsWriteTrackingGeometry_h
#define GeomACTS_ActsWriteTrackingGeometry_h

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"

// PACKAGE
#include "GeomACTS/ObjWriterTool.h"
#include "GeomACTS/IExCellWriterSvc.h"

// STL
#include <fstream>
#include <memory>
#include <vector>

/////////////////////////////////////////////////////////////////////////////

namespace Acts {
  class TrackingGeometry;
  class ITrackingGeometrySvc;
}

class ActsWriteTrackingGeometry : public AthAlgorithm {
public:
  ActsWriteTrackingGeometry (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

private:
  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;

  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  ToolHandle<Acts::ObjWriterTool> m_objWriterTool{this, "ObjWriterTool", "Acts__ObjWriterTool"};

};

#endif // GeomACTS_ActsWriteTrackingGeometry_h
