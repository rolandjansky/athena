#include "ActsGeometry/TrackingGeometrySvc.h"

// Athena
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "StoreGate/StoreGateSvc.h"

// ACTS
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Tools/ITrackingVolumeBuilder.hpp"
#include "Acts/Tools/LayerArrayCreator.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Tools/TrackingVolumeArrayCreator.hpp"
#include "Acts/Tools/CylinderVolumeHelper.hpp"
#include "Acts/Tools/TrackingGeometryBuilder.hpp"
#include "Acts/Tools/CylinderVolumeBuilder.hpp"

#include "Acts/Plugins/ProtobufPlugin/ProtobufMaterialMapReader.hpp"

// PACKAGE
#include "ActsGeometry/GeoModelLayerBuilder.hpp"
#include "ActsGeometry/GeoModelStrawLayerBuilder.hpp"
#include "ActsGeometry/GeoModelDetectorElement.hpp"
#include "ActsInterop/IdentityHelper.h"
#include "ActsInterop/Logger.h"

Acts::TrackingGeometrySvc::TrackingGeometrySvc(const std::string& name, ISvcLocator* svc)
   : base_class(name,svc),
   m_detStore("StoreGateSvc/DetectorStore", name)
{
  m_elementStore = std::make_shared<std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>>();
}

StatusCode
Acts::TrackingGeometrySvc::initialize()
{
  ATH_CHECK ( m_detStore->retrieve(p_pixelManager, "Pixel") );
  ATH_CHECK ( m_detStore->retrieve(p_SCTManager, "SCT") );
  ATH_CHECK ( m_detStore->retrieve(p_TRTManager, "TRT") );

  
  std::list<std::shared_ptr<const Acts::ITrackingVolumeBuilder>> volumeBuilders;

  auto layerArrayCreator = std::make_shared<const Acts::LayerArrayCreator>(
      ACTS_ATH_LOGGER("LayArrayCreator"));

  auto trackingVolumeArrayCreator
      = std::make_shared<const Acts::TrackingVolumeArrayCreator>(
          ACTS_ATH_LOGGER("TrkVolArrayCreator"));

  Acts::CylinderVolumeHelper::Config cvhConfig;
  cvhConfig.layerArrayCreator          = layerArrayCreator;
  cvhConfig.trackingVolumeArrayCreator = trackingVolumeArrayCreator;

  auto cylinderVolumeHelper
    = std::make_shared<const Acts::CylinderVolumeHelper>(
        cvhConfig, ACTS_ATH_LOGGER("CylVolHelper"));

  try {
    // PIXEL
    volumeBuilders.push_back(
          makeVolumeBuilder(p_pixelManager, cylinderVolumeHelper, true));
    
    // SCT
    volumeBuilders.push_back(
          makeVolumeBuilder(p_SCTManager, cylinderVolumeHelper));

    // TRT
    volumeBuilders.push_back(makeVolumeBuilder(p_TRTManager, cylinderVolumeHelper));
  }
  catch (const std::invalid_argument& e) {
    ATH_MSG_ERROR(e.what());
    return StatusCode::FAILURE;
  }


  Acts::TrackingGeometryBuilder::Config tgbConfig;
  tgbConfig.trackingVolumeHelper   = cylinderVolumeHelper;
  tgbConfig.trackingVolumeBuilders = volumeBuilders;

  if(m_useMaterialMap) {
    ATH_MSG_INFO("Configured to use material map from " << std::string(m_materialMapInputFile));
    Acts::ProtobufMaterialMapReader::Config cfg;
    cfg.infile = m_materialMapInputFile;
    Acts::ProtobufMaterialMapReader reader(std::move(cfg));

    tgbConfig.inputMaterialMap = reader.read();
  }



  auto trackingGeometryBuilder
      = std::make_shared<const Acts::TrackingGeometryBuilder>(tgbConfig,
          ACTS_ATH_LOGGER("TrkGeomBldr"));
  
  m_trackingGeometry = trackingGeometryBuilder->trackingGeometry();
  
  return StatusCode::SUCCESS;
}

std::shared_ptr<const Acts::TrackingGeometry>
Acts::TrackingGeometrySvc::trackingGeometry() {
  return m_trackingGeometry;
}

std::shared_ptr<const Acts::ITrackingVolumeBuilder> 
Acts::TrackingGeometrySvc::makeVolumeBuilder(const InDetDD::InDetDetectorManager* manager, std::shared_ptr<const Acts::CylinderVolumeHelper> cvh, bool toBeamline)
{
  std::string managerName = manager->getName();

  Eigen::Vector3d ctrAvg(0, 0, 0);

  using GMLB = Acts::GeoModelLayerBuilder;


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
        ACTS_ATH_LOGGER("SurfaceArrayCreator"));
    Acts::LayerCreator::Config lcCfg;
    lcCfg.surfaceArrayCreator = surfaceArrayCreator;
    auto layerCreator = std::make_shared<Acts::LayerCreator>(
        lcCfg, ACTS_ATH_LOGGER("LayerCreator"));

    Acts::GeoModelStrawLayerBuilder::Config cfg;
    cfg.mng = static_cast<const InDetDD::TRT_DetectorManager*>(manager);
    cfg.elementStore = m_elementStore;
    cfg.layerCreator = layerCreator;
    gmLayerBuilder = std::make_shared<const Acts::GeoModelStrawLayerBuilder>(cfg,
      ACTS_ATH_LOGGER("GMSLayBldr"));

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
        ACTS_ATH_LOGGER("SurfaceArrayCreator"));
    Acts::LayerCreator::Config lcCfg;
    lcCfg.surfaceArrayCreator = surfaceArrayCreator;
    auto layerCreator = std::make_shared<Acts::LayerCreator>(
        lcCfg, ACTS_ATH_LOGGER("LayerCreator"));



    GMLB::Config cfg;
    
    if(managerName == "Pixel") {
      cfg.subdetector = Acts::GeoModelDetectorElement::Subdetector::Pixel;
    }
    else {
      cfg.subdetector = Acts::GeoModelDetectorElement::Subdetector::SCT;
    }

    // set bins from configuration
    if (m_barrelMaterialBins.size() != 2) {
      throw std::invalid_argument("Number of barrel material bin counts != 2");
    }
    std::vector<size_t> brlBins(m_barrelMaterialBins);
    cfg.barrelMaterialBins = {brlBins.at(0), 
                              brlBins.at(1)};

    if (m_endcapMaterialBins.size() != 2) {
      throw std::invalid_argument("Number of endcap material bin counts != 2");
    }
    std::vector<size_t> ecBins(m_endcapMaterialBins);
    cfg.endcapMaterialBins = {ecBins.at(0), 
                              ecBins.at(1)};

    cfg.mng = static_cast<const InDetDD::SiDetectorManager*>(manager);
    // use class member element store
    cfg.elementStore = m_elementStore;
    cfg.layerCreator = layerCreator;

    gmLayerBuilder = std::make_shared<const GMLB>(cfg,
      ACTS_ATH_LOGGER("GMLayBldr"));
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
        ACTS_ATH_LOGGER("CylinderVolumeBuilder"));

  return cylinderVolumeBuilder;
}
