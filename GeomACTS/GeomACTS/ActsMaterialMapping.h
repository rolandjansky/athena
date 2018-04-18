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

// ACTS
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Utilities/BFieldMapUtils.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapper.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"

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
  
  template<typename>
  class ExtrapolationCell;
}

template<typename>
class RootExCellWriter;

class ParticleGun;

class EventContext;

class ITHistSvc;



class ActsMaterialMapping : public AthAlgorithm {
public:
  ActsMaterialMapping (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

  bool isClonable() const override { return true; }
  struct TreeEntry {
    double X0;
    double L0;
    double theta;
    double phi;
    double tTot;
    std::vector<std::array<double, 3>> step_pos;
    std::vector<double> step_X0;
    std::vector<double> step_L0;
    std::vector<double> step_A;
    std::vector<double> step_Z;
    std::vector<double> step_Rho;
    std::vector<double> step_t;
    std::vector<double> step_tX0;
  };
  
  // val tree
  /*static*/ TTree* p_materialTrackTree;
  //static TreeEntry m_treeEntry; 
  //static std::mutex m_treeMutex;

private:



  StatusCode registerOutputTree();

  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;
  ServiceHandle<Acts::IExCellWriterSvc> m_exCellWriterSvc;
  ServiceHandle<ITHistSvc> m_tHistSvc;

  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  ToolHandle<Acts::IExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "Acts__ExtrapolationTool"};

  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>> m_exCells;
  std::shared_ptr<RootExCellWriter<Acts::TrackParameters>> m_rootEccWriter;

  SG::ReadHandleKey<Trk::MaterialStepCollection> m_inputMaterialStepCollection{this, "InputMaterialStepCollection", "StoreGateSvc+MaterialStepRecords"};

  std::unique_ptr<Acts::MaterialMapper> m_materialMapper;


  //Acts::MaterialTrack m_trackRecord;

  double m_treeX0;
  double m_treeL0;
  double m_treeTheta;
  double m_treePhi;
  double m_treeTTot;

  const std::string m_validationTreeName = "/MATTRACKVAL/MaterialTracks";
  std::vector<std::array<double, 3>> m_treeStepPos;
  std::vector<double> m_treeStepX0;
  std::vector<double> m_treeStepL0;
  std::vector<double> m_treeStepA;
  std::vector<double> m_treeStepZ;
  std::vector<double> m_treeStepRho;
  std::vector<double> m_treeStepSteplength;
  std::vector<double> m_treeStepTX0;

};

#endif // GeomACTS_ActsMaterialMapping_h
