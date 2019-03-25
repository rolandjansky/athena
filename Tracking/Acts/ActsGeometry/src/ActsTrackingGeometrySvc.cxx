/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsTrackingGeometrySvc.h"

// ATHENA
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/EventContext.h"
#include "InDetIdentifier/TRT_ID.h"

// ACTS
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Tools/ITrackingVolumeBuilder.hpp"
#include "Acts/Tools/LayerArrayCreator.hpp"
#include "Acts/Tools/SurfaceArrayCreator.hpp"
#include "Acts/Tools/LayerCreator.hpp"
#include "Acts/Tools/TrackingVolumeArrayCreator.hpp"
#include "Acts/Tools/CylinderVolumeHelper.hpp"
#include "Acts/Tools/TrackingGeometryBuilder.hpp"
#include "Acts/Tools/CylinderVolumeBuilder.hpp"

// PACKAGE
#include "ActsGeometry/ActsLayerBuilder.h"
#include "ActsGeometry/ActsStrawLayerBuilder.h"
#include "ActsGeometry/ActsDetectorElement.h"
#include "ActsInterop/IdentityHelper.h"
#include "ActsInterop/Logger.h"
#include "ActsGeometry/ActsAlignmentStore.h"


ActsTrackingGeometrySvc::ActsTrackingGeometrySvc(const std::string& name, ISvcLocator* svc)
   : base_class(name,svc),
   m_detStore("StoreGateSvc/DetectorStore", name)
{
  m_elementStore = std::make_shared<std::vector<std::shared_ptr<const ActsDetectorElement>>>();
}

StatusCode
ActsTrackingGeometrySvc::initialize()
{
  ATH_MSG_INFO(name() << " is initializing");
  
  ATH_CHECK ( m_detStore->retrieve(p_pixelManager, "Pixel") );
  ATH_CHECK ( m_detStore->retrieve(p_SCTManager, "SCT") );
  ATH_CHECK ( m_detStore->retrieve(p_TRTManager, "TRT") );

  if (m_detStore->retrieve(m_TRT_idHelper, "TRT_ID").isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve TRT ID Helper" << endmsg;
  }
  

  auto layerArrayCreator = std::make_shared<const Acts::LayerArrayCreator>(
      makeActsAthenaLogger(this, "LayArrCrtr", "ActsTGSvc"));

  auto trackingVolumeArrayCreator
      = std::make_shared<const Acts::TrackingVolumeArrayCreator>(
          makeActsAthenaLogger(this, "TrkVolArrCrtr", "ActsTGSvc"));

  Acts::CylinderVolumeHelper::Config cvhConfig;
  cvhConfig.layerArrayCreator          = layerArrayCreator;
  cvhConfig.trackingVolumeArrayCreator = trackingVolumeArrayCreator;

  auto cylinderVolumeHelper
    = std::make_shared<const Acts::CylinderVolumeHelper>(
        cvhConfig, makeActsAthenaLogger(this, "CylVolHlpr", "ActsTGSvc"));

  Acts::TrackingGeometryBuilder::Config tgbConfig;
  tgbConfig.trackingVolumeHelper   = cylinderVolumeHelper;
  try {
    // PIXEL
    tgbConfig.trackingVolumeBuilders.push_back([this, cylinderVolumeHelper](
          const auto& inner, const auto&) {
        auto tv =  makeVolumeBuilder(p_pixelManager, cylinderVolumeHelper, true);
        return tv->trackingVolume(inner);
    });
    
    // SCT
    tgbConfig.trackingVolumeBuilders.push_back([this, cylinderVolumeHelper](
          const auto& inner, const auto&) {
        auto tv = makeVolumeBuilder(p_SCTManager, cylinderVolumeHelper);
        return tv->trackingVolume(inner);
    });

    // TRT
    tgbConfig.trackingVolumeBuilders.push_back([this, cylinderVolumeHelper](
          const auto& inner, const auto&) {
        auto tv = makeVolumeBuilder(p_TRTManager, cylinderVolumeHelper);
        return tv->trackingVolume(inner);
    });
  }
  catch (const std::invalid_argument& e) {
    ATH_MSG_ERROR(e.what());
    return StatusCode::FAILURE;
  }


  auto trackingGeometryBuilder
      = std::make_shared<const Acts::TrackingGeometryBuilder>(tgbConfig,
          makeActsAthenaLogger(this, "TrkGeomBldr", "ActsTGSvc"));
  
  m_trackingGeometry = trackingGeometryBuilder->trackingGeometry();
  
  //const Acts::TrackingVolume* = m_trackingGeometry->highestTrackingVolume();


  ATH_MSG_INFO("Acts TrackingGeometry construction completed");
  
  return StatusCode::SUCCESS;
}

std::shared_ptr<const Acts::TrackingGeometry>
ActsTrackingGeometrySvc::trackingGeometry() {

  ATH_MSG_VERBOSE("Retrieving tracking geometry");
  return m_trackingGeometry;
}

std::shared_ptr<const Acts::ITrackingVolumeBuilder> 
ActsTrackingGeometrySvc::makeVolumeBuilder(const InDetDD::InDetDetectorManager* manager, std::shared_ptr<const Acts::CylinderVolumeHelper> cvh, bool toBeamline)
{
  std::string managerName = manager->getName();

  Eigen::Vector3d ctrAvg(0, 0, 0);

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
        makeActsAthenaLogger(this, "SrfArrCrtr", "ActsTGSvc"));
    Acts::LayerCreator::Config lcCfg;
    lcCfg.surfaceArrayCreator = surfaceArrayCreator;
    auto layerCreator = std::make_shared<Acts::LayerCreator>(
        lcCfg, makeActsAthenaLogger(this, "LayCrtr", "ActsTGSvc"));

    ActsStrawLayerBuilder::Config cfg;
    cfg.mng = static_cast<const InDetDD::TRT_DetectorManager*>(manager);
    cfg.elementStore = m_elementStore;
    cfg.layerCreator = layerCreator;
    cfg.trackingGeometrySvc = this;
    cfg.idHelper = m_TRT_idHelper;
    gmLayerBuilder = std::make_shared<const ActsStrawLayerBuilder>(cfg,
      makeActsAthenaLogger(this, "GMSLayBldr", "ActsTGSvc"));

    //gmLayerBuilder->centralLayers();
    //gmLayerBuilder->negativeLayers();
    //gmLayerBuilder->positiveLayers();
  }
  else {
    auto matcher = [](Acts::BinningValue bValue, const Acts::Surface* aS,
                      const Acts::Surface* bS) -> bool {

      auto a = dynamic_cast<const ActsDetectorElement*>(aS->associatedDetectorElement());
      auto b = dynamic_cast<const ActsDetectorElement*>(bS->associatedDetectorElement());
      if ((not a) or (not b)){
        throw std::runtime_error("Cast of surface associated element to ActsDetectorElement failed in ActsTrackingGeometrySvc::makeVolumeBuilder");
      }
    
      IdentityHelper idA = a->identityHelper();
      IdentityHelper idB = b->identityHelper();

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
        makeActsAthenaLogger(this, "SrfArrCrtr", "ActsTGSvc"));
    Acts::LayerCreator::Config lcCfg;
    lcCfg.surfaceArrayCreator = surfaceArrayCreator;
    auto layerCreator = std::make_shared<Acts::LayerCreator>(
        lcCfg, makeActsAthenaLogger(this, "LayCrtr", "ActsTGSvc"));



    ActsLayerBuilder::Config cfg;
    
    if(managerName == "Pixel") {
      cfg.subdetector = ActsDetectorElement::Subdetector::Pixel;
    }
    else {
      cfg.subdetector = ActsDetectorElement::Subdetector::SCT;
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
    cfg.trackingGeometrySvc = this;

    gmLayerBuilder = std::make_shared<const ActsLayerBuilder>(cfg,
      makeActsAthenaLogger(this, "GMLayBldr", "ActsTGSvc"));
  }



  Acts::CylinderVolumeBuilder::Config cvbConfig;
  cvbConfig.layerEnvelopeR = {0, 0};
  cvbConfig.layerEnvelopeZ       = 2;
  cvbConfig.trackingVolumeHelper = cvh;
  cvbConfig.volumeSignature      = 0;
  cvbConfig.volumeName           = managerName;
  cvbConfig.layerBuilder         = gmLayerBuilder;
  cvbConfig.buildToRadiusZero = toBeamline;

  auto cylinderVolumeBuilder
    = std::make_shared<const Acts::CylinderVolumeBuilder>(
        cvbConfig,
        makeActsAthenaLogger(this, "CylVolBldr", "ActsTGSvc"));

  return cylinderVolumeBuilder;
}

void
ActsTrackingGeometrySvc::setAlignmentStore(const ActsAlignmentStore* gas, const EventContext& ctx) 
{
  std::lock_guard<std::mutex> lock(m_gasMapMutex);
  m_gasMap[ctx.slot()] = gas;
}

const ActsAlignmentStore*
ActsTrackingGeometrySvc::getAlignmentStore(const EventContext& ctx) const
{
  std::lock_guard<std::mutex> lock(m_gasMapMutex);
  if (m_gasMap.find(ctx.slot()) == m_gasMap.end()) return nullptr;
  return m_gasMap[ctx.slot()];
}
