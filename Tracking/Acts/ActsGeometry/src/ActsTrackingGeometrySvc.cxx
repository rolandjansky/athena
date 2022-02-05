/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsTrackingGeometrySvc.h"

// ATHENA
#include "GaudiKernel/EventContext.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "StoreGate/StoreGateSvc.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "BeamPipeGeoModel/BeamPipeDetectorManager.h"
#include "GeoModelKernel/GeoTube.h"

// ACTS
#include "Acts/ActsVersion.hpp"
#include "Acts/Geometry/CylinderVolumeBounds.hpp"
#include "Acts/Geometry/CylinderVolumeBuilder.hpp"
#include "Acts/Geometry/CylinderVolumeHelper.hpp"
#include "Acts/Geometry/ITrackingVolumeBuilder.hpp"
#include "Acts/Geometry/LayerArrayCreator.hpp"
#include "Acts/Geometry/LayerCreator.hpp"
#include "Acts/Geometry/SurfaceArrayCreator.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Geometry/TrackingGeometryBuilder.hpp"
#include "Acts/Geometry/TrackingVolume.hpp"
#include "Acts/Geometry/TrackingVolumeArrayCreator.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Geometry/PassiveLayerBuilder.hpp"
#include <Acts/Plugins/Json/JsonMaterialDecorator.hpp>
#include <Acts/Plugins/Json/MaterialMapJsonConverter.hpp>

// PACKAGE
#include "ActsGeometry/ActsAlignmentStore.h"
#include "ActsGeometry/ActsDetectorElement.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ActsLayerBuilder.h"
#include "ActsGeometry/ActsStrawLayerBuilder.h"
#include "ActsInterop/IdentityHelper.h"
#include "ActsInterop/Logger.h"

using namespace Acts::UnitLiterals;

ActsTrackingGeometrySvc::ActsTrackingGeometrySvc(const std::string &name,
                                                 ISvcLocator *svc)
    : base_class(name, svc), m_detStore("StoreGateSvc/DetectorStore", name) {
  m_elementStore = std::make_shared<
      std::vector<std::shared_ptr<const ActsDetectorElement>>>();
}

StatusCode ActsTrackingGeometrySvc::initialize() {
  ATH_MSG_INFO(name() << " is initializing");

  // FIXME: ActsCaloTrackingVolumeBuilder holds ReadHandle to
  // CaloDetDescrManager. Hopefully this service is never called before that
  // object is available.
  m_autoRetrieveTools = false;
  m_checkToolDeps = false;

  ATH_MSG_INFO("ACTS version is: v"
               << Acts::VersionMajor << "." << Acts::VersionMinor << "."
               << Acts::VersionPatch << " [" << Acts::CommitHash << "]");

  // load which subdetectors to build from property
  std::set<std::string> buildSubdet(m_buildSubdetectors.begin(),
                                    m_buildSubdetectors.end());
  ATH_MSG_INFO("Configured to build " << buildSubdet.size()
                                      << " subdetectors:");
  for (const auto &s : buildSubdet) {
    ATH_MSG_INFO(" - " << s);
  }

  ATH_MSG_DEBUG("Loading detector manager(s)");
  if (buildSubdet.find("Pixel") != buildSubdet.end()) {
    ATH_CHECK(m_detStore->retrieve(p_pixelManager, "Pixel"));
  }
  if (buildSubdet.find("SCT") != buildSubdet.end()) {
    ATH_CHECK(m_detStore->retrieve(p_SCTManager, "SCT"));
  }
  if (buildSubdet.find("TRT") != buildSubdet.end()) {
    ATH_CHECK(m_detStore->retrieve(p_TRTManager, "TRT"));
    ATH_CHECK(m_detStore->retrieve(m_TRT_idHelper, "TRT_ID"));
  }
  if (buildSubdet.find("ITkPixel") != buildSubdet.end()) {
    ATH_CHECK(m_detStore->retrieve(p_ITkPixelManager, "ITkPixel"));
  }
  if (buildSubdet.find("ITkStrip") != buildSubdet.end()) {
    ATH_CHECK(m_detStore->retrieve(p_ITkStripManager, "ITkStrip"));
  }

  if(m_buildBeamPipe) {
    ATH_CHECK(m_detStore->retrieve(p_beamPipeMgr, "BeamPipe"));
  }


  ATH_MSG_DEBUG("Setting up ACTS geometry helpers");

  Acts::LayerArrayCreator::Config lacCfg;
  auto layerArrayCreator = std::make_shared<const Acts::LayerArrayCreator>(
      lacCfg, makeActsAthenaLogger(this, "LayArrCrtr", "ActsTGSvc"));

  Acts::TrackingVolumeArrayCreator::Config tvcCfg;
  auto trackingVolumeArrayCreator =
      std::make_shared<const Acts::TrackingVolumeArrayCreator>(
          tvcCfg, makeActsAthenaLogger(this, "TrkVolArrCrtr", "ActsTGSvc"));

  Acts::CylinderVolumeHelper::Config cvhConfig;
  cvhConfig.layerArrayCreator = layerArrayCreator;
  cvhConfig.trackingVolumeArrayCreator = trackingVolumeArrayCreator;

  auto cylinderVolumeHelper =
      std::make_shared<const Acts::CylinderVolumeHelper>(
          cvhConfig, makeActsAthenaLogger(this, "CylVolHlpr", "ActsTGSvc"));

  Acts::TrackingGeometryBuilder::Config tgbConfig;
  tgbConfig.trackingVolumeHelper = cylinderVolumeHelper;

  if (m_useMaterialMap) {
    std::shared_ptr<const Acts::IMaterialDecorator> matDeco = nullptr;
    std::string matFile = m_materialMapInputFile;
    ATH_MSG_INFO("Configured to use material input: " << matFile);
    if (matFile.find(".json") != std::string::npos) {
      // Set up the converter first
      Acts::MaterialMapJsonConverter::Config jsonGeoConvConfig;
      // Set up the json-based decorator
      matDeco = std::make_shared<const Acts::JsonMaterialDecorator>(
          jsonGeoConvConfig, m_materialMapInputFile, Acts::Logging::INFO);
    }
    tgbConfig.materialDecorator = matDeco;
  }

  // default geometry context, this is nominal
  ActsGeometryContext constructionContext;
  constructionContext.construction = true;

  std::pair sctECEnvelopeZ{20_mm, 20_mm};

  try {
    // BeamPipe
    if(m_buildBeamPipe) {
      tgbConfig.trackingVolumeBuilders.push_back([&](const auto &gctx,
                                                     const auto &inner,
                                                     const auto &) {

        Acts::CylinderVolumeBuilder::Config bpvConfig =
          makeBeamPipeConfig(cylinderVolumeHelper);

        Acts::CylinderVolumeBuilder beamPipeVolumeBuilder {
          bpvConfig, makeActsAthenaLogger(this, "BPVolBldr", "ActsTGSvc")};

        return beamPipeVolumeBuilder.trackingVolume(gctx, inner);
      });
    }



    // PIXEL
    if (buildSubdet.count("Pixel") > 0) {
      tgbConfig.trackingVolumeBuilders.push_back([&](const auto &gctx,
                                                     const auto &inner,
                                                     const auto &) {
        auto cfg = makeLayerBuilderConfig(p_pixelManager);
        cfg.mode = ActsLayerBuilder::Mode::Pixel;
        auto lb = std::make_shared<ActsLayerBuilder>(
            cfg, makeActsAthenaLogger(this, "PixelGMSLayBldr", "ActsTGSvc"));
        Acts::CylinderVolumeBuilder::Config cvbConfig;
        cvbConfig.layerEnvelopeR = {3_mm, 3_mm};
        cvbConfig.layerEnvelopeZ = 1_mm;
        cvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
        cvbConfig.volumeSignature = 0;
        cvbConfig.volumeName = "Pixel";
        cvbConfig.layerBuilder = lb;
        cvbConfig.buildToRadiusZero = !m_buildBeamPipe;

        Acts::CylinderVolumeBuilder cvb(
            cvbConfig, makeActsAthenaLogger(this, "CylVolBldr", "ActsTGSvc"));

        return cvb.trackingVolume(gctx, inner);
      });
    }

    // ITK PIXEL
    if (buildSubdet.count("ITkPixel") > 0) {
      tgbConfig.trackingVolumeBuilders.push_back(
          [&](const auto &gctx, const auto &inner, const auto &) {
            auto cfg = makeLayerBuilderConfig(p_ITkPixelManager);
            cfg.mode = ActsLayerBuilder::Mode::ITkPixelInner;
            cfg.objDebugOutput = m_objDebugOutput;
            cfg.doEndcapLayerMerging = true;
            auto lb = std::make_shared<ActsLayerBuilder>(
                cfg, makeActsAthenaLogger(this, "ITkPxInLb", "ActsTGSvc"));

            Acts::CylinderVolumeBuilder::Config cvbConfig;
            cvbConfig.layerEnvelopeR = {5_mm, 5_mm};
            cvbConfig.layerEnvelopeZ = 1_mm;
            cvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
            cvbConfig.volumeSignature = 0;
            cvbConfig.volumeName = "ITkPixelInner";
            cvbConfig.layerBuilder = lb;
            cvbConfig.buildToRadiusZero = !m_buildBeamPipe;

            Acts::CylinderVolumeBuilder cvb(
                cvbConfig,
                makeActsAthenaLogger(this, "CylVolBldr", "ActsTGSvc"));

            return cvb.trackingVolume(gctx, inner);
          });

      tgbConfig.trackingVolumeBuilders.push_back(
          [&](const auto &gctx, const auto &inner, const auto &) {
            auto cfg = makeLayerBuilderConfig(p_ITkPixelManager);
            cfg.mode = ActsLayerBuilder::Mode::ITkPixelOuter;
            cfg.objDebugOutput = m_objDebugOutput;
            cfg.doEndcapLayerMerging = false;
            auto lb = std::make_shared<ActsLayerBuilder>(
                cfg, makeActsAthenaLogger(this, "ITkPxOtLb", "ActsTGSvc"));

            Acts::CylinderVolumeBuilder::Config cvbConfig;
            cvbConfig.layerEnvelopeR = {5_mm, 5_mm};
            cvbConfig.layerEnvelopeZ = 1_mm;
            cvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
            cvbConfig.volumeSignature = 0;
            cvbConfig.volumeName = "ITkPixelOuter";
            cvbConfig.layerBuilder = lb;
            cvbConfig.buildToRadiusZero = false;
            cvbConfig.checkRingLayout = true;
            cvbConfig.ringTolerance = 10_mm;

            Acts::CylinderVolumeBuilder cvb(
                cvbConfig,
                makeActsAthenaLogger(this, "CylVolBldr", "ActsTGSvc"));

            return cvb.trackingVolume(gctx, inner);
          });
    }

    // ITK STRIP
    if (buildSubdet.count("ITkStrip") > 0) {
      tgbConfig.trackingVolumeBuilders.push_back(
          [&](const auto &gctx, const auto &inner, const auto &) {
            auto cfg = makeLayerBuilderConfig(p_ITkStripManager);
            cfg.mode = ActsLayerBuilder::Mode::ITkStrip;
            cfg.objDebugOutput = m_objDebugOutput;
            auto lb = std::make_shared<ActsLayerBuilder>(
                cfg, makeActsAthenaLogger(this, "ITkStripLB", "ActsTGSvc"));

            Acts::CylinderVolumeBuilder::Config cvbConfig;
            cvbConfig.layerEnvelopeR = {5_mm, 5_mm};
            cvbConfig.layerEnvelopeZ = 1_mm;
            cvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
            cvbConfig.volumeSignature = 0;
            cvbConfig.volumeName = "ITkStrip";
            cvbConfig.layerBuilder = lb;
            cvbConfig.buildToRadiusZero = 
              buildSubdet.count("ITkPixel") == 0 && !m_buildBeamPipe;

            Acts::CylinderVolumeBuilder cvb(
                cvbConfig,
                makeActsAthenaLogger(this, "CylVolBldr", "ActsTGSvc"));

            return cvb.trackingVolume(gctx, inner);
          });
    }

    bool buildSCT = buildSubdet.count("SCT") > 0;
    bool buildTRT = buildSubdet.count("TRT") > 0;

    if (buildSCT && buildTRT) {
      // building both we need to take care
      tgbConfig.trackingVolumeBuilders.push_back(
          [&](const auto &gctx, const auto &inner, const auto &) {
            auto cfg = makeLayerBuilderConfig(p_SCTManager);
            cfg.mode = ActsLayerBuilder::Mode::SCT;
            cfg.endcapEnvelopeZ = sctECEnvelopeZ;
            auto sct_lb = std::make_shared<ActsLayerBuilder>(
                cfg, makeActsAthenaLogger(this, "SCTGMSLayBldr", "ActsTGSvc"));

            auto trt_lb = makeStrawLayerBuilder(p_TRTManager);

            return makeSCTTRTAssembly(gctx, *sct_lb, *trt_lb,
                                      *cylinderVolumeHelper, inner);
          });

    } else if (buildSCT) {
      tgbConfig.trackingVolumeBuilders.push_back(
          [&](const auto &gctx, const auto &inner, const auto &) {
            auto lbCfg = makeLayerBuilderConfig(p_SCTManager);
            lbCfg.mode = ActsLayerBuilder::Mode::SCT;
            lbCfg.endcapEnvelopeZ = sctECEnvelopeZ;
            auto lb = std::make_shared<ActsLayerBuilder>(
                lbCfg,
                makeActsAthenaLogger(this, "SCTGMSLayBldr", "ActsTGSvc"));

            Acts::CylinderVolumeBuilder::Config cvbConfig;
            cvbConfig.layerEnvelopeR = {5_mm, 5_mm};
            cvbConfig.layerEnvelopeZ = 2_mm;
            cvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
            cvbConfig.volumeSignature = 0;
            cvbConfig.volumeName = "SCT";
            cvbConfig.layerBuilder = lb;
            cvbConfig.buildToRadiusZero = false;

            Acts::CylinderVolumeBuilder cvb(
                cvbConfig,
                makeActsAthenaLogger(this, "SCTCylVolBldr", "ActsTGSvc"));

            return cvb.trackingVolume(gctx, inner);
          });
    } else if (buildTRT) {
      tgbConfig.trackingVolumeBuilders.push_back(
          [&](const auto &gctx, const auto &inner, const auto &) {
            auto lb = makeStrawLayerBuilder(p_TRTManager);
            Acts::CylinderVolumeBuilder::Config cvbConfig;
            cvbConfig.layerEnvelopeR = {5_mm, 5_mm};
            cvbConfig.layerEnvelopeZ = 2_mm;
            cvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
            cvbConfig.volumeSignature = 0;
            cvbConfig.volumeName = "TRT";
            cvbConfig.layerBuilder = lb;
            cvbConfig.buildToRadiusZero = false;

            Acts::CylinderVolumeBuilder cvb(
                cvbConfig,
                makeActsAthenaLogger(this, "TRTCylVolBldr", "ActsTGSvc"));

            return cvb.trackingVolume(gctx, inner);
          });
    }

    // Calo
    if (buildSubdet.count("Calo") > 0) {
      tgbConfig.trackingVolumeBuilders.push_back(
          [&](const auto &gctx, const auto &inner, const auto &) {
            return m_caloVolumeBuilder->trackingVolume(gctx, inner, nullptr);
          });
    }
  } catch (const std::exception &e) {
    ATH_MSG_ERROR("Encountered error when building Acts tracking geometry");
    ATH_MSG_ERROR(e.what());
    return StatusCode::FAILURE;
  }

  auto trackingGeometryBuilder =
      std::make_shared<const Acts::TrackingGeometryBuilder>(
          tgbConfig, makeActsAthenaLogger(this, "TrkGeomBldr", "ActsTGSvc"));

  ATH_MSG_VERBOSE("Begin building process");
  m_trackingGeometry =
      trackingGeometryBuilder->trackingGeometry(constructionContext.context());
  ATH_MSG_VERBOSE("Building process completed");

  if (!m_trackingGeometry) {
    ATH_MSG_ERROR("No ACTS tracking geometry was built. Cannot proceeed");
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE("Building nominal alignment store");
  ActsAlignmentStore *nominalAlignmentStore = new ActsAlignmentStore();

  populateAlignmentStore(nominalAlignmentStore);

  // manage ownership
  m_nominalAlignmentStore =
      std::unique_ptr<const ActsAlignmentStore>(nominalAlignmentStore);

  ATH_MSG_INFO("Acts TrackingGeometry construction completed");

  return StatusCode::SUCCESS;
}

std::shared_ptr<const Acts::TrackingGeometry>
ActsTrackingGeometrySvc::trackingGeometry() {

  ATH_MSG_VERBOSE("Retrieving tracking geometry");
  return m_trackingGeometry;
}

std::shared_ptr<const Acts::ILayerBuilder>
ActsTrackingGeometrySvc::makeStrawLayerBuilder(
    const InDetDD::InDetDetectorManager *manager) {

  std::string managerName = manager->getName();
  auto matcher = [](const Acts::GeometryContext & /*gctx*/,
                    Acts::BinningValue /*bValue*/, const Acts::Surface * /*aS*/,
                    const Acts::Surface *
                    /*bS*/) -> bool { return false; };

  Acts::SurfaceArrayCreator::Config sacCfg;
  sacCfg.surfaceMatcher = matcher;
  sacCfg.doPhiBinningOptimization = false;

  auto surfaceArrayCreator = std::make_shared<Acts::SurfaceArrayCreator>(
      sacCfg,
      makeActsAthenaLogger(this, managerName + "SrfArrCrtr", "ActsTGSvc"));
  Acts::LayerCreator::Config lcCfg;
  lcCfg.surfaceArrayCreator = surfaceArrayCreator;
  auto layerCreator = std::make_shared<Acts::LayerCreator>(
      lcCfg, makeActsAthenaLogger(this, managerName + "LayCrtr", "ActsTGSvc"));

  ActsStrawLayerBuilder::Config cfg;
  cfg.mng = static_cast<const InDetDD::TRT_DetectorManager *>(manager);
  cfg.elementStore = m_elementStore;
  cfg.layerCreator = layerCreator;
  cfg.idHelper = m_TRT_idHelper;
  return std::make_shared<const ActsStrawLayerBuilder>(
      cfg, makeActsAthenaLogger(this, managerName + "GMSLayBldr", "ActsTGSvc"));
}

ActsLayerBuilder::Config ActsTrackingGeometrySvc::makeLayerBuilderConfig(
    const InDetDD::InDetDetectorManager *manager) {
  std::string managerName = manager->getName();

  std::shared_ptr<const Acts::ILayerBuilder> gmLayerBuilder;
  auto matcher = [](const Acts::GeometryContext & /*gctx*/,
                    Acts::BinningValue bValue, const Acts::Surface *aS,
                    const Acts::Surface *bS) -> bool {
    auto a = dynamic_cast<const ActsDetectorElement *>(
        aS->associatedDetectorElement());
    auto b = dynamic_cast<const ActsDetectorElement *>(
        bS->associatedDetectorElement());
    if ((not a) or (not b)) {
      throw std::runtime_error(
          "Cast of surface associated element to ActsDetectorElement failed "
          "in ActsTrackingGeometrySvc::makeVolumeBuilder");
    }

    IdentityHelper idA = a->identityHelper();
    IdentityHelper idB = b->identityHelper();

    // check if same bec
    // can't be same if not
    if (idA.bec() != idB.bec())
      return false;

    if (bValue == Acts::binPhi) {
      // std::cout << idA.phi_module() << " <-> " << idB.phi_module() <<
      // std::endl;
      return idA.phi_module() == idB.phi_module();
    }

    if (bValue == Acts::binZ) {
      return (idA.eta_module() == idB.eta_module()) &&
             (idA.layer_disk() == idB.layer_disk()) && (idA.bec() == idB.bec());
    }

    if (bValue == Acts::binR) {
      return (idA.eta_module() == idB.eta_module()) &&
             (idA.layer_disk() == idB.layer_disk()) && (idB.bec() == idA.bec());
    }

    return false;
  };

  Acts::SurfaceArrayCreator::Config sacCfg;
  sacCfg.surfaceMatcher = matcher;

  auto surfaceArrayCreator = std::make_shared<Acts::SurfaceArrayCreator>(
      sacCfg,
      makeActsAthenaLogger(this, managerName + "SrfArrCrtr", "ActsTGSvc"));
  Acts::LayerCreator::Config lcCfg;
  lcCfg.surfaceArrayCreator = surfaceArrayCreator;
  auto layerCreator = std::make_shared<Acts::LayerCreator>(
      lcCfg, makeActsAthenaLogger(this, managerName + "LayCrtr", "ActsTGSvc"));

  ActsLayerBuilder::Config cfg;
  cfg.surfaceMatcher = matcher;

  // set bins from configuration
  if (m_barrelMaterialBins.size() != 2) {
    throw std::invalid_argument("Number of barrel material bin counts != 2");
  }
  std::vector<size_t> brlBins(m_barrelMaterialBins);
  cfg.barrelMaterialBins = {brlBins.at(0), brlBins.at(1)};

  if (m_endcapMaterialBins.size() != 2) {
    throw std::invalid_argument("Number of endcap material bin counts != 2");
  }
  std::vector<size_t> ecBins(m_endcapMaterialBins);
  cfg.endcapMaterialBins = {ecBins.at(0), ecBins.at(1)};

  cfg.mng = static_cast<const InDetDD::SiDetectorManager *>(manager);
  // use class member element store
  cfg.elementStore = m_elementStore;
  cfg.layerCreator = layerCreator;

  // gmLayerBuilder = std::make_shared<const ActsLayerBuilder>(
  //     cfg, makeActsAthenaLogger(this, managerName + "GMLayBldr",
  //     "ActsTGSvc"));

  // return gmLayerBuilder;
  return cfg;
}

std::shared_ptr<Acts::TrackingVolume>
ActsTrackingGeometrySvc::makeSCTTRTAssembly(
    const Acts::GeometryContext &gctx, const Acts::ILayerBuilder &sct_lb,
    const Acts::ILayerBuilder &trt_lb, const Acts::CylinderVolumeHelper &cvh,
    const std::shared_ptr<const Acts::TrackingVolume> &pixel) {
  ATH_MSG_VERBOSE("Building SCT+TRT assembly");

  Acts::CylinderVolumeBuilder::Config cvbCfg;
  Acts::CylinderVolumeBuilder cvb(
      cvbCfg, makeActsAthenaLogger(this, "SCTTRTCVB", "ActsTGSvc"));

  ATH_MSG_VERBOSE("Making SCT negative layers: ");
  Acts::VolumeConfig sctNegEC =
      cvb.analyzeContent(gctx, sct_lb.negativeLayers(gctx), {});
  ATH_MSG_VERBOSE("Making SCT positive layers: ");
  Acts::VolumeConfig sctPosEC =
      cvb.analyzeContent(gctx, sct_lb.positiveLayers(gctx), {});
  ATH_MSG_VERBOSE("Making SCT central layers: ");
  Acts::VolumeConfig sctBrl =
      cvb.analyzeContent(gctx, sct_lb.centralLayers(gctx), {});

  ATH_MSG_VERBOSE("Making TRT negative layers: ");
  Acts::VolumeConfig trtNegEC =
      cvb.analyzeContent(gctx, trt_lb.negativeLayers(gctx), {});
  ATH_MSG_VERBOSE("Making TRT positive layers: ");
  Acts::VolumeConfig trtPosEC =
      cvb.analyzeContent(gctx, trt_lb.positiveLayers(gctx), {});
  ATH_MSG_VERBOSE("Making TRT central layers: ");
  Acts::VolumeConfig trtBrl =
      cvb.analyzeContent(gctx, trt_lb.centralLayers(gctx), {});

  // synchronize trt

  double absZMinEC = std::min(std::abs(trtNegEC.zMax), std::abs(trtPosEC.zMin));
  double absZMaxEC = std::max(std::abs(trtNegEC.zMin), std::abs(trtPosEC.zMax));

  trtNegEC.zMin = -absZMaxEC;
  trtNegEC.zMax = -absZMinEC;
  trtPosEC.zMin = absZMinEC;
  trtPosEC.zMax = absZMaxEC;

  using CVBBV = Acts::CylinderVolumeBounds::BoundValues;

  // if pixel is present, shrink SCT volumes in R
  bool isSCTSmallerInZ = false;
  if (pixel) {
    ATH_MSG_VERBOSE("Shrinking SCT in R (and maybe in increase size in Z) to fit around Pixel");
    auto pixelBounds = dynamic_cast<const Acts::CylinderVolumeBounds *>(
        &pixel->volumeBounds());
    double sctNegECzMin = std::min(sctNegEC.zMin, -pixelBounds->get(CVBBV::eHalfLengthZ));
    double sctPosECzMax = std::max(sctPosEC.zMax, pixelBounds->get(CVBBV::eHalfLengthZ));

    ATH_MSG_VERBOSE("- SCT +-EC.rMin: " << sctNegEC.rMin << " -> " << pixelBounds->get(CVBBV::eMaxR));
    ATH_MSG_VERBOSE("- SCT  BRL.rMin: " << sctBrl.rMin << " -> " << pixelBounds->get(CVBBV::eMaxR));
    ATH_MSG_VERBOSE("- SCT EC.zMin: " << sctNegEC.zMin << " -> " << sctNegECzMin);
    ATH_MSG_VERBOSE("- SCT EC.zMax: " << sctPosEC.zMax << " -> " << sctPosECzMax);

    sctNegEC.rMin = pixelBounds->get(CVBBV::eMaxR);
    sctPosEC.rMin = pixelBounds->get(CVBBV::eMaxR);
    sctBrl.rMin = pixelBounds->get(CVBBV::eMaxR);

    isSCTSmallerInZ = sctPosEC.zMax < pixelBounds->get(CVBBV::eHalfLengthZ);

    sctNegEC.zMin = sctNegECzMin;
    sctPosEC.zMax = sctPosECzMax;


  } else {
    ATH_MSG_VERBOSE("Pixel is not configured, not wrapping");
  }

  ATH_MSG_VERBOSE("SCT Volume Configuration:");
  ATH_MSG_VERBOSE("- SCT::NegativeEndcap: " << sctNegEC.layers.size()
                                            << " layers, "
                                            << sctNegEC.toString());
  ATH_MSG_VERBOSE("- SCT::Barrel: " << sctBrl.layers.size() << " layers, "
                                    << sctBrl.toString());
  ATH_MSG_VERBOSE("- SCT::PositiveEncap: " << sctPosEC.layers.size()
                                           << " layers, "
                                           << sctPosEC.toString());

  ATH_MSG_VERBOSE("TRT Volume Configuration:");
  ATH_MSG_VERBOSE("- TRT::NegativeEndcap: " << trtNegEC.layers.size()
                                            << " layers, "
                                            << trtNegEC.toString());
  ATH_MSG_VERBOSE("- TRT::Barrel: " << trtBrl.layers.size() << " layers, "
                                    << trtBrl.toString());
  ATH_MSG_VERBOSE("- TRT::PositiveEncap: " << trtPosEC.layers.size()
                                           << " layers, "
                                           << trtPosEC.toString());

  // harmonize SCT BRL <-> EC, normally the CVB does this, but we're skipping
  // that
  sctBrl.zMax = (sctBrl.zMax + sctPosEC.zMin) / 2.;
  sctBrl.zMin = -sctBrl.zMax;

  // and now harmonize everything
  // inflate TRT Barrel to match SCT
  trtBrl.zMin = sctBrl.zMin;
  trtBrl.zMax = sctBrl.zMax;

  // extend TRT endcaps outwards z so they match SCT
  trtNegEC.zMin = sctNegEC.zMin;
  trtPosEC.zMax = sctPosEC.zMax;

  // extend endcap in z so it touches barrel
  trtNegEC.zMax = trtBrl.zMin;
  sctNegEC.zMax = trtBrl.zMin;
  trtPosEC.zMin = trtBrl.zMax;
  sctPosEC.zMin = trtBrl.zMax;

  // extend SCT in R so they touch TRT barel
  sctBrl.rMax = trtBrl.rMin;
  sctNegEC.rMax = trtNegEC.rMin;
  sctPosEC.rMax = trtPosEC.rMin;

  // extend TRT endcaps in r to that of Barrel
  trtNegEC.rMax = trtBrl.rMax;
  trtPosEC.rMax = trtBrl.rMax;

  ATH_MSG_VERBOSE("Dimensions after synchronization between SCT and TRT");
  ATH_MSG_VERBOSE("SCT Volume Configuration:");
  ATH_MSG_VERBOSE("- SCT::NegativeEndcap: " << sctNegEC.layers.size()
                                            << " layers, "
                                            << sctNegEC.toString());
  ATH_MSG_VERBOSE("- SCT::Barrel: " << sctBrl.layers.size() << " layers, "
                                    << sctBrl.toString());
  ATH_MSG_VERBOSE("- SCT::PositiveEncap: " << sctPosEC.layers.size()
                                           << " layers, "
                                           << sctPosEC.toString());

  ATH_MSG_VERBOSE("TRT Volume Configuration:");
  ATH_MSG_VERBOSE("- TRT::NegativeEndcap: " << trtNegEC.layers.size()
                                            << " layers, "
                                            << trtNegEC.toString());
  ATH_MSG_VERBOSE("- TRT::Barrel: " << trtBrl.layers.size() << " layers, "
                                    << trtBrl.toString());
  ATH_MSG_VERBOSE("- TRT::PositiveEncap: " << trtPosEC.layers.size()
                                           << " layers, "
                                           << trtPosEC.toString());

  auto makeTVol = [&](const auto &vConf, const auto &name) {
    return cvh.createTrackingVolume(gctx, vConf.layers, {},
                                    nullptr, // no material
                                    vConf.rMin, vConf.rMax, vConf.zMin,
                                    vConf.zMax, name);
  };

  // now turn them into actual TrackingVolumes
  auto tvSctNegEC = makeTVol(sctNegEC, "SCT::NegativeEndcap");
  auto tvSctBrl = makeTVol(sctBrl, "SCT::Barrel");
  auto tvSctPosEC = makeTVol(sctPosEC, "SCT::PositiveEndcap");

  auto tvTrtNegEC = makeTVol(trtNegEC, "TRT::NegativeEndcap");
  auto tvTrtBrl = makeTVol(trtBrl, "TRT::Barrel");
  auto tvTrtPosEC = makeTVol(trtPosEC, "TRT::PositiveEndcap");

  // combine the endcaps and the barrels, respetively
  auto negEC =
      cvh.createContainerTrackingVolume(gctx, {tvSctNegEC, tvTrtNegEC});
  auto posEC =
      cvh.createContainerTrackingVolume(gctx, {tvSctPosEC, tvTrtPosEC});
  auto barrel = cvh.createContainerTrackingVolume(gctx, {tvSctBrl, tvTrtBrl});

  // and now combine all of those into one container for the assembly

  auto container =
      cvh.createContainerTrackingVolume(gctx, {negEC, barrel, posEC});

  // if pixel is present, add positive and negative gap volumes so we can wrap
  // it all
  if (pixel) {
    auto containerBounds = dynamic_cast<const Acts::CylinderVolumeBounds *>(
        &container->volumeBounds());
    auto pixelBounds = dynamic_cast<const Acts::CylinderVolumeBounds *>(
        &pixel->volumeBounds());
    std::vector<std::shared_ptr<Acts::TrackingVolume>> noVolumes;

    if(!isSCTSmallerInZ) {
      // pixel is smaller in z, need gap volumes
      auto posGap = cvh.createGapTrackingVolume(
          gctx, noVolumes,
          nullptr, // no material,
          pixelBounds->get(CVBBV::eMinR), pixelBounds->get(CVBBV::eMaxR),
          pixelBounds->get(CVBBV::eHalfLengthZ),
          containerBounds->get(CVBBV::eHalfLengthZ),
          0,    // material layers,
          true, // cylinder
          "Pixel::PositiveGap");
      auto negGap = cvh.createGapTrackingVolume(
          gctx, noVolumes,
          nullptr, // no material,
          pixelBounds->get(CVBBV::eMinR), pixelBounds->get(CVBBV::eMaxR),
          -containerBounds->get(CVBBV::eHalfLengthZ),
          -pixelBounds->get(CVBBV::eHalfLengthZ),
          0,    // material layers,
          true, // cylinder
          "Pixel::NegativeGap");

      auto pixelContainer =
          cvh.createContainerTrackingVolume(gctx, {negGap, pixel, posGap});
      // and now create one container that contains Pixel+SCT+TRT
      container =
          cvh.createContainerTrackingVolume(gctx, {pixelContainer, container});
    }
    else {
      // wrap the pixel directly
      container =
          cvh.createContainerTrackingVolume(gctx, {pixel, container});
    }

  }

  return container;
}

void ActsTrackingGeometrySvc::populateAlignmentStore(
    ActsAlignmentStore *store) const {
  ATH_MSG_DEBUG("Populate the alignment store with all detector elements");
  size_t nElements = 0;
  m_trackingGeometry->visitSurfaces([store,
                                     &nElements](const Acts::Surface *srf) {
    const Acts::DetectorElementBase *detElem = srf->associatedDetectorElement();
    const auto *gmde = dynamic_cast<const ActsDetectorElement *>(detElem);
    gmde->storeTransform(store);
    nElements++;
  });
  ATH_MSG_DEBUG("Populated with " << nElements << " elements");
}

const ActsAlignmentStore *
ActsTrackingGeometrySvc::getNominalAlignmentStore() const {
  return m_nominalAlignmentStore.get();
}

Acts::CylinderVolumeBuilder::Config
ActsTrackingGeometrySvc::makeBeamPipeConfig(
    std::shared_ptr<const Acts::CylinderVolumeHelper> cvh) const {

  // adapted from InnerDetector/InDetDetDescr/InDetTrackingGeometry/src/BeamPipeBuilder.cxx

  PVConstLink beamPipeTopVolume =  p_beamPipeMgr->getTreeTop(0);

  if (p_beamPipeMgr->getNumTreeTops() == 1){ 
    beamPipeTopVolume = p_beamPipeMgr->getTreeTop(0)->getChildVol(0)->getChildVol(0);
  }

  Acts::Transform3 beamPipeTransform;
  beamPipeTransform.setIdentity();

  beamPipeTransform = Acts::Translation3(
      beamPipeTopVolume->getX().translation().x(),
      beamPipeTopVolume->getX().translation().y(),
      beamPipeTopVolume->getX().translation().z()
  );

  double beamPipeRadius = 20;

  const GeoLogVol* beamPipeLogVolume = beamPipeTopVolume->getLogVol();
  const GeoTube* beamPipeTube = nullptr;


  if (beamPipeLogVolume == nullptr) {
    ATH_MSG_ERROR("Beam pip volume has no log volume");
    throw std::runtime_error("Beam pip volume has no log volume");
  }
  // get the geoShape and translate
  beamPipeTube = dynamic_cast<const GeoTube*>(beamPipeLogVolume->getShape());
  if (beamPipeTube == nullptr){
    ATH_MSG_ERROR("BeamPipeLogVolume was not of type GeoTube");
    throw std::runtime_error{"BeamPipeLogVolume was not of type GeoTube"};
  }

  for(unsigned int i=0;i<beamPipeTopVolume->getNChildVols();i++) {

    if(beamPipeTopVolume->getNameOfChildVol(i) == "SectionC03"){

      PVConstLink childTopVolume =  beamPipeTopVolume->getChildVol(i);
      const GeoLogVol* childLogVolume = childTopVolume->getLogVol();
      const GeoTube* childTube = nullptr;

      if (childLogVolume){
        childTube = dynamic_cast<const GeoTube*>(childLogVolume->getShape());
        if (childTube){
          beamPipeRadius = 0.5 * (childTube->getRMax()+childTube->getRMin());
        }
      }

      break; // Exit loop after SectionC03 is found
    }

  } // Loop over child volumes

  ATH_MSG_VERBOSE("BeamPipe constructed from Database: translation (yes) - radius "
      << ( beamPipeTube ? "(yes)" : "(no)") << " - r = " << beamPipeRadius );

  ATH_MSG_VERBOSE("BeamPipe shift estimated as    : " 
      <<  beamPipeTransform.translation().x() << ", "
      <<  beamPipeTransform.translation().y() << ","
      <<  beamPipeTransform.translation().y());

  Acts::CylinderVolumeBuilder::Config cfg;

  Acts::PassiveLayerBuilder::Config bplConfig;
  bplConfig.layerIdentification = "BeamPipe";
  bplConfig.centralLayerRadii = {beamPipeRadius * 1_mm};
  bplConfig.centralLayerHalflengthZ = {3000_mm};
  bplConfig.centralLayerThickness = {1_mm};
  auto beamPipeBuilder = std::make_shared<const Acts::PassiveLayerBuilder>(
      bplConfig, makeActsAthenaLogger(this, "BPLayBldr", "ActsTGSvc"));

  // create the volume for the beam pipe
  cfg.trackingVolumeHelper = cvh;
  cfg.volumeName = "BeamPipe";
  cfg.layerBuilder = beamPipeBuilder;
  cfg.layerEnvelopeR = {1_mm, 1_mm};
  cfg.buildToRadiusZero = true;
  cfg.volumeSignature = 0;

  return cfg;
}
