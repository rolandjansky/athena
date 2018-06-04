// @todo: re-check this
#define ACTS_CORE_IDENTIFIER_PLUGIN "Identifier/Identifier.h"

#include "ActsGeometry/ActsMaterialMapping.h"

// ACTS
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Plugins/MaterialPlugins/MaterialMapper.hpp"
#include "Acts/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "Acts/Material/MaterialProperties.hpp"
#include "Acts/Utilities/GeometryID.hpp"

#include "Acts/Plugins/ProtobufPlugin/ProtobufMaterialMapWriter.hpp"

// ROOT
#include "TTree.h"
#include "TFile.h"

// ATHENA
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"

// PACKAGE
#include "ActsGeometry/Extrapolation/ParticleGun.hpp"
#include "ActsGeometry/ITrackingGeometrySvc.h"
#include "ActsGeometry/IExtrapolationTool.h"
#include "ActsGeometry/IExCellWriterSvc.h"
#include "ActsInterop/Logger.h"
#include "ActsGeometry/IMaterialTrackWriterSvc.h"
//#include "ActsGeometry/Extrapolation/ObjExCellWriter.hpp"
#include "ActsGeometry/RootIndexedMaterialWriter.hpp"

// STL
#include <string>
#include <mutex>

//TTree* ActsMaterialMapping::p_materialTrackTree = nullptr;
//ActsMaterialMapping::TreeEntry ActsMaterialMapping::m_treeEntry;
//std::mutex ActsMaterialMapping::m_treeMutex;


ActsMaterialMapping::ActsMaterialMapping(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
      m_trackingGeometrySvc("TrackingGeometrySvc", name),
      m_exCellWriterSvc("ExCellWriterSvc", name),
      m_materialTrackWriterSvc("MaterialTrackWriterSvc", name)
      //m_tHistSvc("THistSvc", name)
{;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool
ActsMaterialMapping::stepFilter(const Acts::MaterialStep& mStep) const
{
    const Acts::Vector3D& pos = mStep.position();
    if (abs(pos.z()) > 800) {
      if (pos.perp() > 620) {
        return false;
      }
    }
    else {
      if (pos.perp() > 540) {
        return false;
      }
    }
    
    return true;
}

StatusCode ActsMaterialMapping::initialize() {

  ATH_MSG_INFO("ACTS material mapping is initializing");

  ATH_CHECK( m_trackingGeometrySvc.retrieve() );
  ATH_CHECK( m_exCellWriterSvc.retrieve() );
  //ATH_CHECK( m_tHistSvc.retrieve() );
  ATH_CHECK( m_materialTrackWriterSvc.retrieve() );
  
  ATH_CHECK( m_extrapolationTool.retrieve() );
  m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();

  ATH_CHECK( m_inputMaterialStepCollection.initialize() );

  Acts::MaterialMapper::Config matMapCfg;
  matMapCfg.etaCutoff = 6;
  matMapCfg.extrapolationEngine = m_extrapolationTool->extrapolationEngine();

  // attempt to cut out TRT material
  // @TODO: remove this
  matMapCfg.stepFilter = [this](const Acts::MaterialStep& mStep) -> bool {
    return stepFilter(mStep);
  };

  m_materialMapper = std::make_unique<Acts::MaterialMapper>(matMapCfg);
  m_materialMapper->setLogger(ACTS_ATH_LOGGER("MaterialMapper"));
    

  // use this as the prototype, will be copied on first access
  //auto mapCache = m_materialMapper->materialMappingCache(*(m_trackingGeometrySvc->trackingGeometry()));
  //m_ctxSpecificMapperCache = Gaudi::Hive::ContextSpecificData<Acts::MaterialMapper::Cache>(mapCache);

  //m_mapCache = m_materialMapper->materialMappingCache(*(m_trackingGeometrySvc->trackingGeometry()));

  //auto ctx = Gaudi::Hive::currentContext();
  //if(ctx.slot() == 0) {
  //ATH_MSG_ALWAYS("Index " << index());
  //if(index() == 0) {
    //ATH_CHECK( registerOutputTree() );
  //}

  return StatusCode::SUCCESS;
}

StatusCode ActsMaterialMapping::finalize() {

  //auto ctx = Gaudi::Hive::currentContext();
  //ATH_MSG_ALWAYS("finalize, slot: " << ctx.slot());
  //ATH_MSG_ALWAYS("Index " << index());
  //if(index() == 0) {
  //if(ctx.slot() == 0) {
    //ATH_MSG_INFO("Writing validation tree");
    //auto file = std::make_unique<TFile>("MaterialTracks.root", "RECREATE");
    //file->cd();
    //p_materialTrackTree->Write();
    //file->Close();
    //delete p_materialTrackTree;
    //m_tHistSvc->merge(m_validationTreeName);
  //}

  std::vector<Acts::MaterialMapper::Cache> caches;
  auto collector = [this, &caches](const auto& mapCache) {
    //ATH_MSG_ALWAYS("mMatTracks: " << mapCache.materialTrackCounter);
    caches.push_back(std::move(mapCache));
  };
  m_ctxSpecificMapperCache.for_each(collector);

  ATH_MSG_DEBUG("Collected " << caches.size() << " mapping caches. Now merging");

  std::map<Acts::GeometryID, Acts::SurfaceMaterialRecord> merged_map;
  size_t materialTrackCounter = 0;

  for(const auto& cache : caches) {
    std::map<Acts::GeometryID, Acts::SurfaceMaterialRecord> smr_map
      = cache.surfaceMaterialRecords;
    materialTrackCounter += cache.materialTrackCounter;

    for(const auto& item : smr_map) {
      const Acts::GeometryID& geoID = item.first;
      const Acts::SurfaceMaterialRecord& smr = item.second;

      if(merged_map.count(geoID) == 0) {
        // we don't have this geo object yet, just copy
        // this can probably move
        merged_map[geoID] = smr;
      }
      else {
        // we already have an SMR for this geo object
        // merge with what we have
        merged_map[geoID].add(smr);
      }

    }
  }
  ATH_MSG_DEBUG("Merging complete");

  Acts::MaterialMapper::Cache mergedCache(std::move(merged_map));
  mergedCache.materialTrackCounter = materialTrackCounter;
  
  ATH_MSG_DEBUG("Creating Surface Materials");
  std::map<Acts::GeometryID, Acts::SurfaceMaterial*> sMaterialMaps
    = m_materialMapper->createSurfaceMaterial(mergedCache);

  ATH_MSG_DEBUG("Write Surface Material histograms");
  Acts::RootIndexedMaterialWriter::Config wCfg;
  wCfg.name = "MaterialWriter";
  wCfg.logger = ACTS_ATH_LOGGER(wCfg.name);
  wCfg.fileName = "SurfaceMaterials.root";
  Acts::RootIndexedMaterialWriter mWriter(wCfg);

  for(const auto& ism : sMaterialMaps) {
    mWriter.write(ism);
  }

  
  ATH_MSG_DEBUG("Writing to protobuf file");
  Acts::ProtobufMaterialMapWriter::Config matMapWriterCfg;
  matMapWriterCfg.outfile = "MaterialMaps.pb";
  Acts::ProtobufMaterialMapWriter matMapWriter(std::move(matMapWriterCfg), 
                                               ACTS_ATH_LOGGER("ProtobufMaterialMapWriter"));
  matMapWriter.write(sMaterialMaps);



  return StatusCode::SUCCESS;
}

Acts::MaterialTrack
ActsMaterialMapping::makeInputTrack(const Trk::MaterialStepCollection& materialStepCollection) const
{
    size_t nMaterialSteps = materialStepCollection.size();
    ATH_MSG_DEBUG("[+] Successfully read  "<< nMaterialSteps << " geantino steps");

    // create a direction out of the last material step
    double dirx = materialStepCollection[nMaterialSteps-1]->hitX();
    double diry = materialStepCollection[nMaterialSteps-1]->hitY();
    double dirz = materialStepCollection[nMaterialSteps-1]->hitZ();
    Acts::Vector3D direction = Acts::Vector3D(dirx,diry,dirz).unit();

    // MaterialTrack from MaterialStepCollection
    Acts::MaterialStep::Position startPos(0, 0, 0);
    double theta = direction.theta();
    double phi = direction.phi();

    std::vector<Acts::MaterialStep> materialSteps;
    materialSteps.reserve(nMaterialSteps);

    double totDInX0 = 0;
    double totDInL0 = 0;

    auto matStepIt = materialStepCollection.begin();
    for(; matStepIt != materialStepCollection.end(); ++matStepIt) {
      const Trk::MaterialStep* athMatStep = *matStepIt;

      Acts::MaterialStep::Position pos(athMatStep->hitX(), 
                                       athMatStep->hitY(), 
                                       athMatStep->hitZ());

      std::array<double, 3> posArr({athMatStep->hitX(), athMatStep->hitY(), athMatStep->hitZ()});

      Acts::MaterialProperties mat(athMatStep->x0(), 
                                   athMatStep->l0(), 
                                   athMatStep->A(), 
                                   athMatStep->Z(), 
                                   athMatStep->rho(), 
                                   athMatStep->steplength());

      double thickness = athMatStep->steplength();
      double X0 = athMatStep->x0();
      double L0 = athMatStep->l0();
      double dInX0 = (X0 * X0 > 10e-10) ? thickness / X0 : 0.;
      double dInL0 = (L0 * L0 > 10e-10) ? thickness / L0 : 0.;

      Acts::MaterialStep mStep(mat, pos);

      if (stepFilter(mStep)) {
        totDInX0 += dInX0;
        totDInL0 += dInL0;

        //materialSteps.emplace_back(mat, pos);
        materialSteps.push_back(std::move(mStep));
      }
    }

    Acts::MaterialTrack inputTrack(startPos, theta, phi, materialSteps, totDInX0, totDInL0);
    return inputTrack;
}

StatusCode ActsMaterialMapping::execute_r(const EventContext& ctx) const
{

  Acts::MaterialMapper::Cache& mapCache = m_ctxSpecificMapperCache;
  
  if(mapCache.surfaceMaterialRecords.empty()) {
    // this is the first event for this cache, initialize
    mapCache = m_materialMapper->materialMappingCache(*(m_trackingGeometrySvc->trackingGeometry()));
  }
  

  //auto mapCache = m_materialMapper->materialMappingCache(*(m_trackingGeometrySvc->trackingGeometry()));

  SG::ReadHandle<Trk::MaterialStepCollection> materialStepCollection(m_inputMaterialStepCollection);

  if (materialStepCollection.isValid() && materialStepCollection->size()){

    Acts::MaterialTrack inputTrack = makeInputTrack(*materialStepCollection);

    ATH_MSG_VERBOSE("Read MaterialTrack from file, it has "
                                       << inputTrack.materialSteps().size()
                                       << " steps.");

    // some screen output to know what is going on
    ATH_MSG_VERBOSE("These will be mapped onto "
                 << mapCache.surfaceMaterialRecords.size()
                 << " surfaces.");

    // perform the mapping
    Acts::MaterialTrack mappedTrack
        = m_materialMapper->mapMaterialTrack(mapCache, inputTrack);

    ATH_MSG_VERBOSE("Write input track");
    m_materialTrackWriterSvc->write(inputTrack);

  }

  return StatusCode::SUCCESS;
}


