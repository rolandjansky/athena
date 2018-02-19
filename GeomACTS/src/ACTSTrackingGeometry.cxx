/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeomACTS/ACTSTrackingGeometry.h"

#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "TrkSurfaces/TrapezoidBounds.h"

#include <Eigen/Dense>

// @todo: re-check this
#define ACTS_CORE_IDENTIFIER_PLUGIN "Identifier/Identifier.h"

#include "ACTS/Detector/TrackingGeometry.hpp"
//#include "ACTS/Plugins/GeoModelPlugins/GeoModelDetectorElement.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/LayerCreator.hpp"
#include "ACTS/Tools/SurfaceArrayCreator.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/LayerCreator.hpp"
#include "ACTS/Tools/PassiveLayerBuilder.hpp"
#include "ACTS/Tools/TrackingGeometryBuilder.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Tools/ITrackingVolumeBuilder.hpp"
#include "ACTS/Tools/ILayerBuilder.hpp"

#include "ACTS/Utilities/Units.hpp"

// TEMPORARY
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Utilities/BFieldMapUtils.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"
#include "ACTS/MagneticField/concept/AnyFieldLookup.hpp"

#include "GeomACTS/Extrapolation/RootExCellWriter.hpp"
#include "GeomACTS/Extrapolation/ObjExCellWriter.hpp"


#include "GeomACTS/GeoModelLayerBuilder.hpp"
#include "GeomACTS/GeoModelStrawLayerBuilder.hpp"
#include "GeomACTS/obj/ObjSurfaceWriter.hpp"
#include "GeomACTS/obj/ObjTrackingGeometryWriter.hpp"
#include "GeomACTS/IdentityHelper.hpp"

#include "GeomACTS/Extrapolation/ParticleGun.hpp"

#include "GeoModelKernel/GeoPrintGraphAction.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <random>
#include <atomic>
#include <algorithm>
#include <thread>


namespace {
const Amg::Vector3D origin(0., 0., 0.);
}


/////////////////////////////////////////////////////////////////////////////
//
// Prints out SiDetectorElement transform

/////////////////////////////////////////////////////////////////////////////

ACTSTrackingGeometry::ACTSTrackingGeometry(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_firstEvent(true),
      m_geoModelSvc("GeoModelSvc", name) {
  // Get parameter values from jobOptions file
  declareProperty("ModulesOnly", m_modulesOnly = true,
                  "Print transforms of modules");
  declareProperty("ExpandId", m_expandId = true, "Print fields of identifier");
  declareProperty("NominalPosition", m_nominal = true,
                  "Print out nominal geometry");
  declareProperty("AlignedPosition", m_aligned = false,
                  "Print out aligned geometry");
  declareProperty("FullRotationMatrix", m_fullRotationMatrix = false,
                  "If true prints the 9 elements of the rotation matrix");
  declareProperty("OutputFile", m_outputFileName = "geometry.dat",
                  "Output file name");
  declareProperty("GeoModelSvc", m_geoModelSvc);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode ACTSTrackingGeometry::initialize() {
  // Get GeoModelSvc
  const IGeoModelSvc* geoModel;
  ATH_CHECK(service("GeoModelSvc", geoModel));
  m_fileout.open(m_outputFileName.c_str());
  ATH_MSG_DEBUG("Opening output file " << m_outputFileName);
  if (!m_fileout) {
    ATH_MSG_ERROR("Could not open file " << m_outputFileName);
    return StatusCode::FAILURE;
  }
  // Print version infomration
  m_fileout << "# ATLAS tag: " << geoModel->atlasVersion() << std::endl;
  m_fileout << "# InDet tag: " << geoModel->inDetVersionOverride() << std::endl;
  m_fileout << "# Pixel tag: " << geoModel->pixelVersionOverride() << std::endl;
  m_fileout << "# SCT   tag: " << geoModel->SCT_VersionOverride() << std::endl;
  m_fileout << "# TRT   tag: " << geoModel->TRT_VersionOverride() << std::endl;

  // prepare element store
  m_elementStore = std::make_shared<std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>>();

  return StatusCode::SUCCESS;
}

StatusCode ACTSTrackingGeometry::buildTrackingGeometry() {
  //const InDetDD::SiDetectorManager* siDetManager;
  //ATH_CHECK(detStore()->retrieve(siDetManager, managerName));
      //if ( detStore()->retrieve(m_TRTGeoManager, "TRT").isFailure()) {

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

  std::vector<std::string> siDetectors = {
    "Pixel",
    "SCT",
  };
  for(const auto& managerName : siDetectors) {
    const InDetDD::SiDetectorManager* manager;
    ATH_CHECK(detStore()->retrieve(manager, managerName));
    volumeBuilders.push_back(
        makeVolumeBuilder(manager, cylinderVolumeHelper, managerName == "Pixel"));
  }

  // TRT
  const InDetDD::TRT_DetectorManager* trtMng;
  ATH_CHECK(detStore()->retrieve(trtMng, "TRT"));
  volumeBuilders.push_back(makeVolumeBuilder(trtMng, cylinderVolumeHelper));


  Acts::TrackingGeometryBuilder::Config tgbConfig;
  tgbConfig.trackingVolumeHelper   = cylinderVolumeHelper;
  tgbConfig.trackingVolumeBuilders = volumeBuilders;
  auto trackingGeometryBuilder
      = std::make_shared<const Acts::TrackingGeometryBuilder>(tgbConfig);
  

  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = trackingGeometryBuilder->trackingGeometry();


  writeTrackingGeometry(*trackingGeometry);

  //using BField_t = Acts::ConstantBField;
  //std::shared_ptr<Acts::ConstantBField> bField
    //= std::make_shared<Acts::ConstantBField>(
        //0 * Acts::units::_T,
        //0 * Acts::units::_T,
        //5 * Acts::units::_T);
    
  
  // ATLAS FIELD
  using BField_t = Acts::InterpolatedBFieldMap;
  Acts::concept::AnyFieldLookup<> mapper = bfield();
  Acts::InterpolatedBFieldMap::Config config;
  config.scale  = 1.;
  config.mapper = std::move(mapper);
  std::shared_ptr<Acts::InterpolatedBFieldMap> bField
    = std::make_shared<Acts::InterpolatedBFieldMap>(std::move(config));
  

  Acts::Logging::Level extrapLogLevel = Acts::Logging::INFO;

  // (a) RungeKuttaPropagtator
  using RKEngine = Acts::RungeKuttaEngine<BField_t>;
  typename RKEngine::Config propConfig;
  propConfig.fieldService = bField;
  auto propEngine         = std::make_shared<RKEngine>(propConfig);
  propEngine->setLogger(Acts::getDefaultLogger("RungeKuttaEngine", extrapLogLevel));
  // (b) MaterialEffectsEngine
  Acts::MaterialEffectsEngine::Config matConfig;
  auto                                materialEngine
    = std::make_shared<Acts::MaterialEffectsEngine>(matConfig);
  materialEngine->setLogger(
      Acts::getDefaultLogger("MaterialEffectsEngine", extrapLogLevel));
  // (c) StaticNavigationEngine
  Acts::StaticNavigationEngine::Config navConfig;
  navConfig.propagationEngine     = propEngine;
  navConfig.materialEffectsEngine = materialEngine;
  navConfig.trackingGeometry      = trackingGeometry;
  auto navEngine = std::make_shared<Acts::StaticNavigationEngine>(navConfig);
  navEngine->setLogger(Acts::getDefaultLogger("NavigationEngine", extrapLogLevel));
  // (d) the StaticEngine
  Acts::StaticEngine::Config statConfig;
  statConfig.propagationEngine     = propEngine;
  statConfig.navigationEngine      = navEngine;
  statConfig.materialEffectsEngine = materialEngine;
  auto statEngine = std::make_shared<Acts::StaticEngine>(statConfig);
  statEngine->setLogger(Acts::getDefaultLogger("StaticEngine", extrapLogLevel));
  // (e) the material engine
  Acts::ExtrapolationEngine::Config exEngineConfig;
  exEngineConfig.trackingGeometry     = trackingGeometry;
  exEngineConfig.propagationEngine    = propEngine;
  exEngineConfig.navigationEngine     = navEngine;
  exEngineConfig.extrapolationEngines = {statEngine};
  auto exEngine = std::make_unique<Acts::ExtrapolationEngine>(exEngineConfig);
  exEngine->setLogger(Acts::getDefaultLogger("ExtrapolationEngine", extrapLogLevel));


  RootExCellWriter<Acts::TrackParameters>::Config reccWriterConfig;
  reccWriterConfig.filePath       = "excells_charged.root";
  reccWriterConfig.treeName       = "extrapolation_charged";
  reccWriterConfig.writeBoundary  = true;
  reccWriterConfig.writeMaterial  = true;
  reccWriterConfig.writeSensitive = true;
  reccWriterConfig.writePassive   = true;
  auto rootEccWriter
      = std::make_shared<RootExCellWriter<Acts::TrackParameters>>(
          reccWriterConfig);

  auto ofs = std::make_shared<std::ofstream>();
  ofs->open("extrapolation_charged.obj");
  ObjExCellWriter<Acts::TrackParameters>::Config objeccWriterConfig;
  objeccWriterConfig.outputStream = ofs;
  auto objEccWriter
      = std::make_shared<ObjExCellWriter<Acts::TrackParameters>>(
          objeccWriterConfig);



  //std::mt19937 rng;
  ParticleGun::Config pgCfg;
  pgCfg.nParticles = 120;
  pgCfg.pID = 11;
  pgCfg.mass = 0.51099891 * Acts::units::_MeV;
  pgCfg.charge = -1.;
  //pgCfg.etaRange = {-1.16, -1};
  pgCfg.etaRange = {-3, 3};
  auto rng = std::make_shared<std::mt19937>();
  pgCfg.rng = rng;

  ParticleGun pg(pgCfg, Acts::getDefaultLogger("ParticleGun", Acts::Logging::VERBOSE));

  std::vector<Acts::ProcessVertex> vertices = pg.generate();


  std::cout << "Processing particles:" << std::endl;
  std::atomic<size_t> nProcessed(0);
  
  
  size_t nthreads = std::thread::hardware_concurrency();
  nthreads = std::max(size_t(1), nthreads);
  std::vector<std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>> results(nthreads);

  std::cout << "using " << nthreads << " threads" << std::endl;

  //for (const auto &pv : vertices) {
  for(size_t n=0;n<vertices.size();n++) {
    

    const Acts::ProcessVertex &pv = vertices.at(n);

    const Acts::Vector3D &pos = pv.position();
    std::cout << "OUTGOING: at " << pos.x() << " " << pos.y() << " " << pos.z() << std::endl;
    Acts::PerigeeSurface surface(pv.position());


    #pragma omp parallel num_threads(nthreads)
    {

      #pragma omp for
      for(size_t pi=0;pi<pv.outgoingParticles().size();++pi) {
        const auto &particle = pv.outgoingParticles().at(pi);
      //for(const auto &particle : pv.outgoingParticles()) {
        double pt = particle.momentum().perp();
        double pz = particle.momentum().z();

        //std::cout << "pt = " << pt << " pz = " << pz << std::endl;

        // prepare this particle for extrapolation
        double d0    = 0.;
        double z0    = 0.;
        double phi   = particle.momentum().phi();
        double theta = particle.momentum().theta();
        // treat differently for neutral particles
        double qop = particle.charge() != 0
            ? particle.charge() / particle.momentum().mag()
            : 1. / particle.momentum().mag();
        // parameters
        Acts::ActsVectorD<5> pars;
        pars << d0, z0, phi, theta, qop;
        std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;

        if (particle.charge()) {
          // charged extrapolation - with hit recording
          Acts::BoundParameters startParameters(
              std::move(cov), std::move(pars), surface);
          Acts::ExtrapolationCell<Acts::TrackParameters> ecc(startParameters);
          ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
          ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
          //executeTestT<Acts::TrackParameters>(startParameters, particle.barcode(), cCells);
          ecc.searchMode                       = 1;
          ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectSensitive);
          ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectPassive);
          ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectBoundary);
          ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);
          //eTestConfig.collectSensitive                 = true;
          //eTestConfig.collectPassive                   = true;
          //eTestConfig.collectBoundary                  = true;
          //eTestConfig.collectMaterial                  = true;
          //eTestConfig.sensitiveCurvilinear             = false;
          //eTestConfig.pathLimit                        = -1.;
    
          Acts::ExtrapolationCode eCode = exEngine->extrapolate(ecc);

          int tid = omp_get_thread_num();
          // where do we push?
          results.at(tid).push_back(std::move(ecc));
          
          //cCells.push_back(std::move(ecc));
          //cCells.at(n) = std::move(ecc);
   

          int nP = nProcessed;
          nProcessed++;
          if (nP % (pgCfg.nParticles / 20) == 0) {
            std::cout << "processed " << nP << " / " << pgCfg.nParticles << " : ";
            std::cout << std::fixed << std::setprecision(2);
            std::cout << ((nP / double(pgCfg.nParticles))*100) << "%" << std::endl;
          }

        }
      }
    } 
  }

  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>   cCells;
  cCells.reserve(pgCfg.nParticles);
  for(auto &res : results) {
    //std::copy(std::make_move_iterator(res.begin()), std::make_move_iterator(res.end()), std::back_inserter(cCells));
    for(auto &c : res) {
      cCells.push_back(std::move(c));
    }
  }

  // write extrap cells
  rootEccWriter->write(cCells);
  rootEccWriter->endRun();

  objEccWriter->write(cCells);


  ofs->close();


  return StatusCode::SUCCESS;
}

Acts::InterpolatedBFieldMap::FieldMapper<3, 3> ACTSTrackingGeometry::bfield() const
{
  std::function<size_t(std::array<size_t, 3> binsXYZ,
                       std::array<size_t, 3> nBinsXYZ)> localToGlobalBin = 
      [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
      return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2))
          + binsXYZ.at(1) * nBinsXYZ.at(2)
          + binsXYZ.at(2));
      };

  std::string fieldMapFile = "../../acts-data/MagneticField/ATLAS/ATLASBField_xyz.root";
  std::string treeName = "bField";
  double      lengthUnit = Acts::units::_mm;
  double      BFieldUnit = Acts::units::_T;
  bool        firstOctant = false;




  /// [1] Read in field map file
  // Grid position points in x, y and z
  std::vector<double> xPos;
  std::vector<double> yPos;
  std::vector<double> zPos;
  // components of magnetic field on grid points
  std::vector<Acts::Vector3D> bField;
  // [1] Read in file and fill values
  TFile* inputFile = TFile::Open(fieldMapFile.c_str());
  TTree* tree      = (TTree*)inputFile->Get(treeName.c_str());
  Int_t  entries   = tree->GetEntries();

  double x, y, z;
  double Bx, By, Bz;

  tree->SetBranchAddress("x", &x);
  tree->SetBranchAddress("y", &y);
  tree->SetBranchAddress("z", &z);

  tree->SetBranchAddress("Bx", &Bx);
  tree->SetBranchAddress("By", &By);
  tree->SetBranchAddress("Bz", &Bz);

  // reserve size
  xPos.reserve(entries);
  yPos.reserve(entries);
  zPos.reserve(entries);
  bField.reserve(entries);

  for (int i = 0; i < entries; i++) {
    tree->GetEvent(i);
    xPos.push_back(x);
    yPos.push_back(y);
    zPos.push_back(z);
    bField.push_back(Acts::Vector3D(Bx, By, Bz));
  }
  inputFile->Close();

  return Acts::fieldMapperXYZ(localToGlobalBin,
                              xPos,
                              yPos,
                              zPos,
                              bField,
                              lengthUnit,
                              BFieldUnit,
                              firstOctant);
}


std::shared_ptr<const Acts::ITrackingVolumeBuilder> 
ACTSTrackingGeometry::makeVolumeBuilder(const InDetDD::InDetDetectorManager* manager, std::shared_ptr<const Acts::CylinderVolumeHelper> cvh, bool toBeamline)
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

void 
ACTSTrackingGeometry::writeTrackingGeometry(const Acts::TrackingGeometry& trackingGeometry)
{
  std::vector<std::string> subDetectors
      = {"Pixel", "SCT", "TRT"};

  std::vector<std::shared_ptr<ObjSurfaceWriter>> subWriters;
  std::vector<std::shared_ptr<std::ofstream>>           subStreams;

  for (const auto& sdet : subDetectors) {
    auto        sdStream = std::shared_ptr<std::ofstream>(new std::ofstream);
    std::string sdOutputName = sdet + std::string(".obj");
    sdStream->open(sdOutputName);
    // object surface writers
    ObjSurfaceWriter::Config sdObjWriterConfig(sdet,
        Acts::Logging::INFO);
    sdObjWriterConfig.filePrefix         = "";
    sdObjWriterConfig.outputPhiSegments  = 10;
    sdObjWriterConfig.outputPrecision    = 6;
    sdObjWriterConfig.outputScalor       = 1.;
    sdObjWriterConfig.outputThickness    = 1.;
    sdObjWriterConfig.outputSensitive    = true;
    sdObjWriterConfig.outputLayerSurface = false;
    sdObjWriterConfig.outputStream       = sdStream;
    auto sdObjWriter
      = std::make_shared<ObjSurfaceWriter>(sdObjWriterConfig);
    // push back
    subWriters.push_back(sdObjWriter);
    subStreams.push_back(sdStream);
  }

  // configure the tracking geometry writer
  ObjTrackingGeometryWriter::Config tgObjWriterConfig(
      "ObjTrackingGeometryWriter", Acts::Logging::INFO);
  tgObjWriterConfig.surfaceWriters       = subWriters;
  tgObjWriterConfig.filePrefix           = "";
  tgObjWriterConfig.sensitiveGroupPrefix = "";
  tgObjWriterConfig.layerPrefix          = "";
  // the tracking geometry writers
  auto tgObjWriter
    = std::make_shared<ObjTrackingGeometryWriter>(tgObjWriterConfig);

  // write the tracking geometry object
  tgObjWriter->write(trackingGeometry);

}

StatusCode ACTSTrackingGeometry::execute() {
  if (m_firstEvent) {
    m_firstEvent = false;
    //ATH_CHECK(printElements("Pixel"));
     //ATH_CHECK( printElements("SCT") );
    ATH_CHECK( buildTrackingGeometry() );
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode ACTSTrackingGeometry::finalize() {
  m_fileout.close();
  return StatusCode::SUCCESS;
}

std::string ACTSTrackingGeometry::printTransform(
    const Amg::Transform3D& trans) const {
  Amg::Vector3D xyz = trans * origin;
  std::ostringstream ostr;
  ostr << xyz.x() << " " << xyz.y() << " " << xyz.z() << " ";
  if (m_fullRotationMatrix) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        ostr << trans(i, j) << " ";
      }
    }
  } else {
    double alpha = 0, beta = 0, gamma = 0;
    extractAlphaBetaGamma(trans, alpha, beta, gamma);
    ostr << alpha << " " << beta << " " << gamma;
  }
  // CLHEP::HepRotation rot = trans.getRotation();
  // ostr << std::endl << rot;
  return ostr.str();
}

void ACTSTrackingGeometry::extractAlphaBetaGamma(const Amg::Transform3D& trans,
                                            double& alpha, double& beta,
                                            double& gamma) const {
  double siny = trans(0, 2);
  beta = asin(siny);
  // Check if cosy = 0. This requires special treatment.
  // can check either element (1,2),(2,2) both equal zero
  // or (0,1) and (0,0)
  if ((trans(1, 2) == 0) && (trans(2, 2) == 0)) {
    // alpha and gamma are degenerate. We arbitrarily choose
    // gamma = 0.
    gamma = 0;
    alpha = atan2(trans(1, 1), trans(2, 1));
  } else {
    alpha = atan2(-trans(1, 2), trans(2, 2));
    gamma = atan2(-trans(0, 1), trans(0, 0));
    if (alpha == 0) alpha = 0;  // convert -0 to 0
    if (gamma == 0) gamma = 0;  // convert -0 to 0
  }
}
