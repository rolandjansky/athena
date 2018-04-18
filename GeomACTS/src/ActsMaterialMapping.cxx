// @todo: re-check this
#define ACTS_CORE_IDENTIFIER_PLUGIN "Identifier/Identifier.h"

#include "GeomACTS/ActsMaterialMapping.h"

// ACTS
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapper.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Material/MaterialProperties.hpp"

// ROOT
#include "TTree.h"
#include "TFile.h"

// ATHENA
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"

// PACKAGE
#include "GeomACTS/Extrapolation/ParticleGun.hpp"
#include "GeomACTS/ITrackingGeometrySvc.h"
#include "GeomACTS/IExtrapolationTool.h"
#include "GeomACTS/IExCellWriterSvc.h"
#include "GeomACTS/Logger.h"
//#include "GeomACTS/Extrapolation/ObjExCellWriter.hpp"

// STL
#include <string>
#include <mutex>

//TTree* ActsMaterialMapping::p_materialTrackTree = nullptr;
//ActsMaterialMapping::TreeEntry ActsMaterialMapping::m_treeEntry;
//std::mutex ActsMaterialMapping::m_treeMutex;


ActsMaterialMapping::ActsMaterialMapping(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_trackingGeometrySvc("TrackingGeometrySvc", name),
      m_exCellWriterSvc("ExCellWriterSvc", name),
      m_tHistSvc("THistSvc", name)
{;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode ActsMaterialMapping::initialize() {

  ATH_MSG_INFO("ACTS material mapping is initializing");

  ATH_CHECK( m_trackingGeometrySvc.retrieve() );
  ATH_CHECK( m_exCellWriterSvc.retrieve() );
  ATH_CHECK( m_tHistSvc.retrieve() );
  
  ATH_CHECK( m_extrapolationTool.retrieve() );
  m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();

  ATH_CHECK( m_inputMaterialStepCollection.initialize() );

  Acts::MaterialMapper::Config matMapCfg;
  matMapCfg.etaCutoff = 6;
  matMapCfg.extrapolationEngine = m_extrapolationTool->extrapolationEngine();

  m_materialMapper = std::make_unique<Acts::MaterialMapper>(matMapCfg);
  m_materialMapper->setLogger(ACTS_ATH_LOGGER("MaterialMapper"));

  //auto ctx = Gaudi::Hive::currentContext();
  //if(ctx.slot() == 0) {
  //ATH_MSG_ALWAYS("Index " << index());
  //if(index() == 0) {
    ATH_CHECK( registerOutputTree() );
  //}

  return StatusCode::SUCCESS;
}

StatusCode ActsMaterialMapping::finalize() {

  auto ctx = Gaudi::Hive::currentContext();
  ATH_MSG_ALWAYS("finalize, slot: " << ctx.slot());
  ATH_MSG_ALWAYS("Index " << index());
  if(index() == 0) {
  //if(ctx.slot() == 0) {
    //ATH_MSG_INFO("Writing validation tree");
    //auto file = std::make_unique<TFile>("MaterialTracks.root", "RECREATE");
    //file->cd();
    //p_materialTrackTree->Write();
    //file->Close();
    //delete p_materialTrackTree;
    //m_tHistSvc->merge(m_validationTreeName);
  }

  return StatusCode::SUCCESS;
}

StatusCode ActsMaterialMapping::registerOutputTree() {
  ATH_MSG_INFO("Registering output tree");
  
  p_materialTrackTree = new TTree("MaterialTracks", "MaterialTracks");
  //auto tree = std::make_unique<TTree>("MaterialTracks", "MaterialTracks");

  //TTree *tree;
  //ATH_CHECK( m_tHistSvc->regTree(m_validationTreeName) );
  //ATH_CHECK( m_tHistSvc->getTree(m_validationTreeName, tree) );

  //tree->Branch("X0", &m_treeTX0);
  //tree->Branch("L0", &m_treeTL0);
  //tree->Branch("phi", &m_treePhi);
  //tree->Branch("theta", &m_treeTheta);
  //tree->Branch("tTot", &m_treeTTot);

  //tree->Branch("step_X0", &m_treeStepX0);
  //tree->Branch("step_L0", &m_treeStepL0);
  //tree->Branch("step_A", &m_treeStepA);
  //tree->Branch("step_Z", &m_treeStepZ);
  //tree->Branch("step_rho", &m_treeStepRho);
  //tree->Branch("step_steplength", &m_treeStepSteplength);

    
  //p_materialTrackTree->Branch("MaterialTrack", &m_trackRecord);
  p_materialTrackTree->Branch("X0", &m_treeX0);
  p_materialTrackTree->Branch("L0", &m_treeL0);
  p_materialTrackTree->Branch("phi", &m_treePhi);
  p_materialTrackTree->Branch("theta", &m_treeTheta);

  p_materialTrackTree->Branch("step_X0", &m_treeStepX0);
  p_materialTrackTree->Branch("step_L0", &m_treeStepL0);
  p_materialTrackTree->Branch("step_A", &m_treeStepA);
  p_materialTrackTree->Branch("step_Z", &m_treeStepZ);
  p_materialTrackTree->Branch("step_rho", &m_treeStepRho);
  p_materialTrackTree->Branch("step_steplength", &m_treeStepSteplength);

  //ATH_CHECK( m_tHistSvc->regTree(m_validationTreeName, std::move(tree)) );
  ATH_CHECK( m_tHistSvc->regTree(m_validationTreeName, p_materialTrackTree) );

  //p_materialTrackTree = tree;
  

  //p_materialTrackTree = new TTree("MaterialTracks", "MaterialTracks");
  ////p_treeEntry =
  //p_materialTrackTree->Branch("X0", &m_treeEntry.X0);
  //p_materialTrackTree->Branch("L0", &m_treeEntry.L0);
  //p_materialTrackTree->Branch("theta", &m_treeEntry.theta);
  //p_materialTrackTree->Branch("phi", &m_treeEntry.phi);
  //p_materialTrackTree->Branch("tTot", &m_treeEntry.tTot);
  //p_materialTrackTree->Branch("step_X0", &m_treeEntry.step_X0);
  //p_materialTrackTree->Branch("step_L0", &m_treeEntry.step_L0);
  //p_materialTrackTree->Branch("step_A", &m_treeEntry.step_A);
  //p_materialTrackTree->Branch("step_Z", &m_treeEntry.step_Z);
  //p_materialTrackTree->Branch("step_Rho", &m_treeEntry.step_Rho);
  //p_materialTrackTree->Branch("step_t", &m_treeEntry.step_t);
  //p_materialTrackTree->Branch("step_tX0", &m_treeEntry.step_tX0);

  //ATH_CHECK( m_tHistSvc->regTree(m_validationTreeName, p_materialTrackTree) );

  return StatusCode::SUCCESS;
}

StatusCode ActsMaterialMapping::execute()
{

  Acts::MaterialMapper::Cache mCache
    = m_materialMapper->materialMappingCache(*m_trackingGeometrySvc->trackingGeometry());


  SG::ReadHandle<Trk::MaterialStepCollection> materialStepCollection(m_inputMaterialStepCollection);



  if (materialStepCollection.isValid() && materialStepCollection->size()){
    size_t nMaterialSteps = materialStepCollection->size();
    ATH_MSG_DEBUG("[+] Successfully read  "<< nMaterialSteps << " geantino steps");

    // create a direction out of the last material step
    double dirx = (*materialStepCollection)[nMaterialSteps-1]->hitX();
    double diry = (*materialStepCollection)[nMaterialSteps-1]->hitY();
    double dirz = (*materialStepCollection)[nMaterialSteps-1]->hitZ();
    Acts::Vector3D direction = Acts::Vector3D(dirx,diry,dirz).unit();

    // MaterialTrack from MaterialStepCollection
    Acts::MaterialStep::Position startPos(0, 0, 0);
    double theta = direction.theta();
    double phi = direction.phi();

    std::vector<Acts::MaterialStep> materialSteps;
    materialSteps.reserve(nMaterialSteps);

    double X0 = 0;
    double L0 = 0;

    m_treeStepPos.clear();
    m_treeStepPos.reserve(nMaterialSteps);
    m_treeStepX0.clear();
    m_treeStepX0.reserve(nMaterialSteps);
    m_treeStepL0.clear();
    m_treeStepL0.reserve(nMaterialSteps);
    m_treeStepA.clear();
    m_treeStepA.reserve(nMaterialSteps);
    m_treeStepZ.clear();
    m_treeStepZ.reserve(nMaterialSteps);
    m_treeStepRho.clear();
    m_treeStepRho.reserve(nMaterialSteps);
    m_treeStepSteplength.clear();
    m_treeStepSteplength.reserve(nMaterialSteps);
    m_treeStepTX0.clear();
    m_treeStepTX0.reserve(nMaterialSteps);

    m_treeTTot = 0;

    auto matStepIt = (*materialStepCollection).begin();
    for(; matStepIt != (*materialStepCollection).end(); ++matStepIt) {
      const Trk::MaterialStep* athMatStep = *matStepIt;
      //ATH_MSG_DEBUG(" -> Converting MaterialStep at " 
          //<< athMatStep->hitX() << ", " << athMatStep->hitY() << ", " << athMatStep->hitZ());

      Acts::MaterialStep::Position pos(athMatStep->hitX(), 
                                       athMatStep->hitY(), 
                                       athMatStep->hitZ());

      std::array<double, 3> posArr({athMatStep->hitX(), athMatStep->hitY(), athMatStep->hitZ()});
      m_treeStepPos.push_back(posArr);

      m_treeStepX0.push_back(athMatStep->x0());
      m_treeStepL0.push_back(athMatStep->l0());
      m_treeStepA.push_back(athMatStep->A());
      m_treeStepZ.push_back(athMatStep->Z());
      m_treeStepRho.push_back(athMatStep->rho());
      m_treeStepSteplength.push_back(athMatStep->steplength());

      // @TODO: thickness == steplength?
      Acts::MaterialProperties mat(athMatStep->x0(), 
                                   athMatStep->l0(), 
                                   athMatStep->A(), 
                                   athMatStep->Z(), 
                                   athMatStep->rho(), 
                                   athMatStep->steplength());

      X0 += athMatStep->x0();
      L0 += athMatStep->l0();
      m_treeTTot += athMatStep->steplength();

      materialSteps.emplace_back(mat, pos);
    }


    Acts::MaterialTrack inputTrack(startPos, theta, phi, materialSteps, X0, L0);

    ATH_MSG_VERBOSE("Read MaterialTrack from file, it has "
                                       << inputTrack.materialSteps().size()
                                       << " steps.");

    // some screen output to know what is going on
    ATH_MSG_VERBOSE("These will be mapped onto "
                 << mCache.surfaceMaterialRecords.size()
                 << " surfaces.");

        // perform the mapping
    Acts::MaterialTrack mappedTrack
        = m_materialMapper->mapMaterialTrack(mCache, inputTrack);

    ATH_MSG_VERBOSE("Adding mapped track to validation tree");
    //m_trackRecord = mappedTrack;
    
    m_treeX0 = X0;
    m_treeL0 = L0;
    m_treeTheta = theta;
    m_treePhi = phi;
    p_materialTrackTree->Fill();

    //{
      //std::lock_guard<std::mutex> guard(m_treeMutex);
      //// copy over
      //m_treeEntry.X0 = m_treeX0;
      //m_treeEntry.L0 = m_treeL0;
      //m_treeEntry.theta = m_treeTheta;
      //m_treeEntry.phi = m_treePhi;
      //m_treeEntry.tTot = m_treeTTot;
      //p_materialTrackTree->Fill();
    //}

  }

  return StatusCode::SUCCESS;
}


