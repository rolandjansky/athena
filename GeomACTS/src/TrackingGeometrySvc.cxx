
#include "GeomACTS/TrackingGeometrySvc.h"
#include "GeomACTS/GeoModelLayerBuilder.hpp"
#include "GeomACTS/GeoModelStrawLayerBuilder.hpp"
#include "GeomACTS/GeoModelDetectorElement.hpp"
#include "GeomACTS/IdentityHelper.hpp"

// ACTS
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Tools/ITrackingVolumeBuilder.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/TrackingGeometryBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"

// Athena
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "StoreGate/StoreGateSvc.h"

Acts::TrackingGeometrySvc::TrackingGeometrySvc(const std::string& name, ISvcLocator* svc)
   : base_class(name,svc),
   m_detStore("StoreGateSvc/DetectorStore", name)
{
  m_elementStore = std::make_shared<std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>>();
}

StatusCode
Acts::TrackingGeometrySvc::initialize()
{
  
  return StatusCode::SUCCESS;
}

std::shared_ptr<const Acts::TrackingGeometry>
Acts::TrackingGeometrySvc::trackingGeometry() {

  if ( !m_detStore->retrieve(p_pixelManager, "Pixel").isSuccess() ) {
    ATH_MSG_FATAL("Unable to retrieve manager for Pixel");
    return nullptr;
  }
  if ( !m_detStore->retrieve(p_SCTManager, "SCT").isSuccess() ) {
    ATH_MSG_FATAL("Unable to retrieve manager for SCT");
    return nullptr;
  }
  if ( !m_detStore->retrieve(p_TRTManager, "TRT").isSuccess() ) {
    ATH_MSG_FATAL("Unable to retrieve manager for TRT");
    return nullptr;
  }


  Acts::Logging::Level loggingLevel = Acts::Logging::VERBOSE;
  
  std::list<std::shared_ptr<const Acts::ITrackingVolumeBuilder>> volumeBuilders;

  auto layerArrayCreator = std::make_shared<const Acts::LayerArrayCreator>(
      Acts::getDefaultLogger("LayArrayCreator", loggingLevel));

  auto trackingVolumeArrayCreator
      = std::make_shared<const Acts::TrackingVolumeArrayCreator>(
          Acts::getDefaultLogger("TrkVolArrayCreator", loggingLevel));

  Acts::CylinderVolumeHelper::Config cvhConfig;
  cvhConfig.layerArrayCreator          = layerArrayCreator;
  cvhConfig.trackingVolumeArrayCreator = trackingVolumeArrayCreator;

  auto cylinderVolumeHelper
    = std::make_shared<const Acts::CylinderVolumeHelper>(
        cvhConfig, Acts::getDefaultLogger("CylVolHelper", loggingLevel));


  // PIXEL and SCT

  //std::vector<std::string> siDetectors = {
    //"Pixel",
    //"SCT",
  //};

  //for(const auto& managerName : siDetectors) {
    //const InDetDD::SiDetectorManager* manager;
    //volumeBuilders.push_back(
        //makeVolumeBuilder(manager, cylinderVolumeHelper, managerName == "Pixel"));
  //}
    
  volumeBuilders.push_back(
        makeVolumeBuilder(p_pixelManager, cylinderVolumeHelper, true));
  
  volumeBuilders.push_back(
        makeVolumeBuilder(p_SCTManager, cylinderVolumeHelper));

  // TRT
  volumeBuilders.push_back(makeVolumeBuilder(p_TRTManager, cylinderVolumeHelper));


  Acts::TrackingGeometryBuilder::Config tgbConfig;
  tgbConfig.trackingVolumeHelper   = cylinderVolumeHelper;
  tgbConfig.trackingVolumeBuilders = volumeBuilders;
  auto trackingGeometryBuilder
      = std::make_shared<const Acts::TrackingGeometryBuilder>(tgbConfig);
  

  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = trackingGeometryBuilder->trackingGeometry();


  return trackingGeometry;
}

std::shared_ptr<const Acts::ITrackingVolumeBuilder> 
Acts::TrackingGeometrySvc::makeVolumeBuilder(const InDetDD::InDetDetectorManager* manager, std::shared_ptr<const Acts::CylinderVolumeHelper> cvh, bool toBeamline)
{
  std::string managerName = manager->getName();

  Eigen::Vector3d ctrAvg(0, 0, 0);
  size_t nElem = 0;

  using GMLB = Acts::GeoModelLayerBuilder;

  Acts::Logging::Level loggingLevel = Acts::Logging::VERBOSE;

  // auto gmLayerBuilder = GMLB(cfg);
  //GMLB gmLayerBuilder(cfg);
  std::shared_ptr<const Acts::ILayerBuilder> gmLayerBuilder;
  if (manager->getName() == "TRT") {
    auto matcher = [](Acts::BinningValue /*bValue*/, const Acts::Surface* /*aS*/,
                      const Acts::Surface* /*bS*/) -> bool {
      return false;
    };

    Acts::SurfaceArrayCreator::Config sacCfg;
    sacCfg.surfaceMatcher = matcher;
    sacCfg.doPhiBinningOptimization = false;

    auto surfaceArrayCreator = std::make_shared<Acts::SurfaceArrayCreator>(
        sacCfg,
        Acts::getDefaultLogger("SurfaceArrayCreator", Acts::Logging::VERBOSE));
    Acts::LayerCreator::Config lcCfg;
    lcCfg.surfaceArrayCreator = surfaceArrayCreator;
    auto layerCreator = std::make_shared<Acts::LayerCreator>(
        lcCfg, Acts::getDefaultLogger("LayerCreator", Acts::Logging::VERBOSE));

    Acts::GeoModelStrawLayerBuilder::Config cfg;
    cfg.mng = static_cast<const InDetDD::TRT_DetectorManager*>(manager);
    cfg.elementStore = m_elementStore;
    cfg.layerCreator = layerCreator;
    gmLayerBuilder = std::make_shared<const Acts::GeoModelStrawLayerBuilder>(cfg,
      Acts::getDefaultLogger("GMSLayBldr", Acts::Logging::VERBOSE));

    //gmLayerBuilder->centralLayers();
    //gmLayerBuilder->negativeLayers();
    //gmLayerBuilder->positiveLayers();
  }
  else {
    auto matcher = [](Acts::BinningValue bValue, const Acts::Surface* aS,
                      const Acts::Surface* bS) -> bool {

      auto a = dynamic_cast<const Acts::GeoModelDetectorElement*>(
          aS->associatedDetectorElement());
      auto b = dynamic_cast<const Acts::GeoModelDetectorElement*>(
          bS->associatedDetectorElement());


      //auto id_a = a->identify();
      //auto id_b = b->identify();
    
      Acts::IdentityHelper idA = a->identityHelper();
      Acts::IdentityHelper idB = b->identityHelper();

      // check if same bec
      // can't be same if not
      if(idA.bec() != idB.bec()) return false;

      if (bValue == Acts::binPhi) {
        //std::cout << idA.phi_module() << " <-> " << idB.phi_module() << std::endl;
        return idA.phi_module() == idB.phi_module();
      }

      if (bValue == Acts::binZ) {
        return (idA.eta_module() == idB.eta_module())
               && (idA.layer_disk() == idB.layer_disk())
               && (idA.bec() == idB.bec());
      }

      if (bValue == Acts::binR) {
        return (idA.eta_module() == idB.eta_module()) 
               && (idA.layer_disk() == idB.layer_disk())
               && (idB.bec() == idA.bec());
      }

      return false;
    };

    Acts::SurfaceArrayCreator::Config sacCfg;
    sacCfg.surfaceMatcher = matcher;

    auto surfaceArrayCreator = std::make_shared<Acts::SurfaceArrayCreator>(
        sacCfg,
        Acts::getDefaultLogger("SurfaceArrayCreator", Acts::Logging::VERBOSE));
    Acts::LayerCreator::Config lcCfg;
    lcCfg.surfaceArrayCreator = surfaceArrayCreator;
    auto layerCreator = std::make_shared<Acts::LayerCreator>(
        lcCfg, Acts::getDefaultLogger("LayerCreator", Acts::Logging::VERBOSE));



    GMLB::Config cfg;
    
    if(managerName == "Pixel") {
      cfg.subdetector = Acts::GeoModelDetectorElement::Subdetector::Pixel;
    }
    else {
      cfg.subdetector = Acts::GeoModelDetectorElement::Subdetector::SCT;
    }

    cfg.mng = static_cast<const InDetDD::SiDetectorManager*>(manager);
    // use class member element store
    cfg.elementStore = m_elementStore;
    cfg.layerCreator = layerCreator;
    gmLayerBuilder = std::make_shared<const GMLB>(cfg,
      Acts::getDefaultLogger("GMLayBldr", Acts::Logging::VERBOSE));
  }



  Acts::CylinderVolumeBuilder::Config cvbConfig;
  cvbConfig.layerEnvelopeR = {0, 0};
  cvbConfig.layerEnvelopeZ       = 2;
  cvbConfig.trackingVolumeHelper = cvh;
  cvbConfig.volumeSignature      = 0;
  cvbConfig.volumeName           = managerName;
  //cvbConfig.volumeMaterial       = volumeMaterial;
  cvbConfig.layerBuilder         = gmLayerBuilder;
  cvbConfig.buildToRadiusZero = toBeamline;

  auto cylinderVolumeBuilder
    = std::make_shared<const Acts::CylinderVolumeBuilder>(
        cvbConfig,
        Acts::getDefaultLogger("CylinderVolumeBuilder", loggingLevel));

  return cylinderVolumeBuilder;
}
