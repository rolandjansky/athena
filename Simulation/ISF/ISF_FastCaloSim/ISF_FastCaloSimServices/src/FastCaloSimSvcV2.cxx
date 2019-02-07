/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastCaloSimSvcV2.cxx, (c) ATLAS Detector software             //
///////////////////////////////////////////////////////////////////

// class header include
#include "FastCaloSimSvcV2.h"


// FastCaloSim includes
#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "ISF_FastCaloSimParametrization/CaloGeometryFromCaloDDM.h"

//!
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictParser/IdDictParser.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/TileID.h"
//!

// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"

#include "PathResolver/PathResolver.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TFile.h"
#include <fstream>

using std::abs;
using std::atan2;

/** Constructor **/
ISF::FastCaloSimSvcV2::FastCaloSimSvcV2(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_param(nullptr),
  m_theContainer(nullptr),
  m_rndGenSvc("AtRndmGenSvc", name),
  m_randomEngine(nullptr),
  m_caloGeo(nullptr)
{
  declareProperty("ParamsInputFilename"            ,       m_paramsFilename,"TFCSparam.root");
  declareProperty("ParamsInputObject"              ,       m_paramsObject,"SelPDGID");
  declareProperty("CaloCellsOutputName"            ,       m_caloCellsOutputName) ;
  declareProperty("CaloCellMakerTools_setup"       ,       m_caloCellMakerToolsSetup) ;
  declareProperty("CaloCellMakerTools_release"     ,       m_caloCellMakerToolsRelease) ;
  declareProperty("RandomSvc"                      ,       m_rndGenSvc                );
  declareProperty("RandomStream"                   ,       m_randomEngineName         );
  declareProperty("FastCaloSimCaloExtrapolation"   ,       m_FastCaloSimCaloExtrapolation );
}

ISF::FastCaloSimSvcV2::~FastCaloSimSvcV2()
{}

/** framework methods */
StatusCode ISF::FastCaloSimSvcV2::initialize()
{

  ATH_MSG_INFO(m_screenOutputPrefix << "Initializing ...");

  ATH_CHECK(m_rndGenSvc.retrieve());
  m_randomEngine = m_rndGenSvc->GetEngine( m_randomEngineName);
  if(!m_randomEngine)
  {
   ATH_MSG_ERROR("Could not get random number engine from RandomNumberService. Abort.");
   return StatusCode::FAILURE;
  }
  
  const CaloDetDescrManager* calo_dd_man  = CaloDetDescrManager::instance();
  const FCALDetectorManager * fcalManager=NULL;
  ATH_CHECK(detStore()->retrieve(fcalManager));
  
  m_caloGeo = new CaloGeometryFromCaloDDM();
  m_caloGeo->LoadGeometryFromCaloDDM(calo_dd_man);
  if(!m_caloGeo->LoadFCalChannelMapFromFCalDDM(fcalManager) )ATH_MSG_FATAL("Found inconsistency between FCal_Channel map and GEO file. Please, check if they are configured properly.");
  
  const std::string fileName = m_paramsFilename;
  std::string inputFile=PathResolverFindCalibFile(fileName);
  std::unique_ptr<TFile> paramsFile(TFile::Open( inputFile.c_str(), "READ" ));
  if (paramsFile == nullptr) {
    ATH_MSG_ERROR("file = "<<m_paramsFilename<< " not found");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Opened parametrization file = "<<m_paramsFilename);
  paramsFile->ls();
  m_param=(TFCSParametrizationBase*)paramsFile->Get(m_paramsObject.c_str());
  if(!m_param) {
    ATH_MSG_WARNING("file = "<<m_paramsFilename<< ", object "<< m_paramsObject<<" not found");
    return StatusCode::FAILURE;
  }
  
  paramsFile->Close();
  
  m_param->set_geometry(m_caloGeo);
  m_param->Print("short");
  m_param->setLevel(MSG::DEBUG);
  
  // Get FastCaloSimCaloExtrapolation
  if(m_FastCaloSimCaloExtrapolation.retrieve().isFailure())
  {
   ATH_MSG_ERROR("FastCaloSimCaloExtrapolation not found ");
   return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::FastCaloSimSvcV2::finalize()
{
  ATH_MSG_INFO(m_screenOutputPrefix << "Finalizing ...");
  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcV2::setupEvent()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_MSG_VERBOSE(m_screenOutputPrefix << "setupEvent NEW EVENT!");
  
  m_theContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);

  StatusCode sc = evtStore()->record(m_theContainer, m_caloCellsOutputName);
  if (sc.isFailure())
  {
    ATH_MSG_FATAL( m_screenOutputPrefix << "cannot record CaloCellContainer " << m_caloCellsOutputName );
    return StatusCode::FAILURE;
  }

  CHECK( m_caloCellMakerToolsSetup.retrieve() );
  ATH_MSG_DEBUG( "Successfully retrieve CaloCellMakerTools: " << m_caloCellMakerToolsSetup );
  for (const ToolHandle<ICaloCellMakerTool>& tool : m_caloCellMakerToolsSetup)
  {
    std::string chronoName=this->name()+"_"+ tool.name();
    if (m_chrono) m_chrono->chronoStart(chronoName);
    StatusCode sc = tool->process(m_theContainer, ctx);
    if (m_chrono) {
      m_chrono->chronoStop(chronoName);
      ATH_MSG_DEBUG( m_screenOutputPrefix << "Chrono stop : delta " << m_chrono->chronoDelta (chronoName,IChronoStatSvc::USER) * CLHEP::microsecond / CLHEP::second << " second " );
    }

    if (sc.isFailure())
    {
      ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << tool.name() );
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcV2::releaseEvent()
{
 
 ATH_MSG_VERBOSE(m_screenOutputPrefix << "release Event");
 
 CHECK( m_caloCellMakerToolsRelease.retrieve() );
 
 //run release tools in a loop
 ToolHandleArray<ICaloCellMakerTool>::iterator itrTool = m_caloCellMakerToolsRelease.begin();
 ToolHandleArray<ICaloCellMakerTool>::iterator endTool = m_caloCellMakerToolsRelease.end();
 for (; itrTool != endTool; ++itrTool)
 {
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Calling tool " << itrTool->name() );
  
  StatusCode sc = (*itrTool)->process(m_theContainer);
  
  if (sc.isFailure())
  {
   ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() );
  }
 }
 
 return StatusCode::SUCCESS;
 
}

/** Simulation Call */
StatusCode ISF::FastCaloSimSvcV2::simulate(const ISF::ISFParticle& isfp, McEventCollection*)
{

  ATH_MSG_VERBOSE("NEW PARTICLE! FastCaloSimSvcV2 called with ISFParticle: " << isfp);
 
  Amg::Vector3D particle_position =  isfp.position();  
  Amg::Vector3D particle_direction(isfp.momentum().x(),isfp.momentum().y(),isfp.momentum().z());
  
   //int barcode=isfp.barcode(); // isfp barcode, eta and phi: in case we need them
  // float eta_isfp = particle_position.eta();  
  // float phi_isfp = particle_position.phi(); 

  //Don't simulate particles with total energy below 10 MeV
  if(isfp.ekin() < 10) {
    ATH_MSG_VERBOSE("Skipping particle with Ekin: " << isfp.ekin() <<" MeV. Below the 10 MeV threshold.");
    return StatusCode::SUCCESS;
  }


  /// for anti protons and anti neutrons the kinetic energy should be 
  /// calculated as Ekin = E() + M() instead of E() - M()
  /// this is achieved by constructing the TFCSTruthState with negative mass 
  float isfp_mass = isfp.mass(); 
  if(isfp.pdgCode() == -2212 or isfp.pdgCode() == -2112) {
    isfp_mass = - isfp_mass;
    ATH_MSG_VERBOSE("Found anti-proton/neutron, negative mass is used for TFCSTruthState "); 
  }


  TFCSTruthState truth;
  truth.SetPtEtaPhiM(particle_direction.perp(), particle_direction.eta(), particle_direction.phi(), isfp_mass);
  truth.set_pdgid(isfp.pdgCode());  
  truth.set_vertex(particle_position[Amg::x], particle_position[Amg::y], particle_position[Amg::z]);

  TFCSExtrapolationState extrapol;
  m_FastCaloSimCaloExtrapolation->extrapolate(extrapol,&truth);
  TFCSSimulationState simulstate(m_randomEngine);

  ATH_MSG_DEBUG(" particle: " << isfp.pdgCode() << " Ekin: " << isfp.ekin() << " position eta: " << particle_position.eta() << " direction eta: " << particle_direction.eta() << " position phi: " << particle_position.phi() << " direction phi: " << particle_direction.phi());
  m_param->setLevel(MSG::DEBUG);

  FCSReturnCode status = m_param->simulate(simulstate, &truth, &extrapol);
  if (status != FCSSuccess) {
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Energy returned: " << simulstate.E());
  ATH_MSG_VERBOSE("Energy fraction for layer: ");
  for (int s = 0; s < CaloCell_ID_FCS::MaxSample; s++)
  ATH_MSG_VERBOSE(" Sampling " << s << " energy " << simulstate.E(s));

  //Now deposit all cell energies into the CaloCellContainer
  for(const auto& iter : simulstate.cells()) {
    CaloCell* theCell = (CaloCell*)m_theContainer->findCell(iter.first->calo_hash());
    theCell->addEnergy(iter.second);
  }
  
  return StatusCode::SUCCESS;
}
