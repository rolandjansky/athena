#ifndef GeomACTS_ActsMaterialMapping_h
#define GeomACTS_ActsMaterialMapping_h

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkGeometry/MaterialStepCollection.h"

#include "GaudiKernel/ContextSpecificPtr.h"

// ACTS
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Utilities/BFieldMapUtils.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapper.hpp"

// ROOT
#include "TTree.h"

// PACKAGE
#include "GeomACTS/ObjWriterTool.h"
#include "GeomACTS/IExCellWriterSvc.h"

// STL
#include <memory>
#include <vector>
#include <mutex>

/////////////////////////////////////////////////////////////////////////////

namespace Acts {
  class TrackingGeometry;
  class ITrackingGeometrySvc;
  class IExtrapolationTool;
  
  class IMaterialTrackWriterSvc;

  template<typename>
  class ExtrapolationCell;
}

template<typename>
class RootExCellWriter;

class ParticleGun;

class EventContext;

class ITHistSvc;



class ActsMaterialMapping : public AthReentrantAlgorithm {
public:
  ActsMaterialMapping (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute_r(const EventContext& ctx) const override;
  StatusCode finalize() override;

  //bool isClonable() const override { return true; }

private:

  Acts::MaterialTrack makeInputTrack(const Trk::MaterialStepCollection& materialStepCollection) const;

  // thread-local state object
  //Acts::MaterialMapper::Cache m_mapCache;

  Gaudi::Hive::ContextSpecificData<Acts::MaterialMapper::Cache> m_ctxSpecificMapperCache;


  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;
  ServiceHandle<Acts::IExCellWriterSvc> m_exCellWriterSvc;
  ServiceHandle<Acts::IMaterialTrackWriterSvc> m_materialTrackWriterSvc;

  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  ToolHandle<Acts::IExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "Acts__ExtrapolationTool"};

  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>> m_exCells;
  std::shared_ptr<RootExCellWriter<Acts::TrackParameters>> m_rootEccWriter;

  SG::ReadHandleKey<Trk::MaterialStepCollection> m_inputMaterialStepCollection{this, "InputMaterialStepCollection", "StoreGateSvc+MaterialStepRecords"};

  std::unique_ptr<Acts::MaterialMapper> m_materialMapper;
};

#endif // GeomACTS_ActsMaterialMapping_h
