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


#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "PathResolver/PathResolver.h"

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
  m_caloGeo = new CaloGeometryFromCaloDDM();
  m_caloGeo->LoadGeometryFromCaloDDM(calo_dd_man);
  TString path_to_fcal_geo_files = "/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/";
  m_caloGeo->LoadFCalGeometryFromFiles(path_to_fcal_geo_files + "FCal1-electrodes.sorted.HV.09Nov2007.dat", path_to_fcal_geo_files + "FCal2-electrodes.sorted.HV.April2011.dat", path_to_fcal_geo_files + "FCal3-electrodes.sorted.HV.09Nov2007.dat");
  
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
  ToolHandleArray<ICaloCellMakerTool>::iterator itrTool = m_caloCellMakerToolsSetup.begin();
  ToolHandleArray<ICaloCellMakerTool>::iterator endTool = m_caloCellMakerToolsSetup.end();
  for (; itrTool != endTool; ++itrTool)
  {
    StatusCode sc = (*itrTool)->process(m_theContainer);
    if (sc.isFailure())
    {
      ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() );
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
StatusCode ISF::FastCaloSimSvcV2::simulate(const ISF::ISFParticle& isfp)
{

  ATH_MSG_VERBOSE("NEW PARTICLE! FastCaloSimSvcV2 called with ISFParticle: " << isfp);

  int pdgid = fabs(isfp.pdgCode());
  
  //int barcode=isfp.barcode(); // isfp barcode, eta and phi: in case we need them
  // float eta_isfp = particle_position.eta();  
  // float phi_isfp = particle_position.phi();  

  Amg::Vector3D particle_position =  isfp.position();  
  

  if(!(pdgid==22 || pdgid==11))
  {
    ATH_MSG_VERBOSE("ISF particle has pdgid "<<pdgid<<", that's not supported yet");
    return StatusCode::SUCCESS; 
  } 

  TFCSTruthState truth(isfp.momentum().x(),isfp.momentum().y(),isfp.momentum().z(),sqrt(pow(isfp.ekin(),2)+pow(isfp.mass(),2)),isfp.pdgCode());
  truth.set_vertex(particle_position[Amg::x], particle_position[Amg::y], particle_position[Amg::z]);

  TFCSExtrapolationState extrapol;
  m_FastCaloSimCaloExtrapolation->extrapolate(extrapol,&truth);
  TFCSSimulationState simulstate;

  FCSReturnCode status = m_param->simulate(simulstate, &truth, &extrapol);
  if (status != FCSSuccess) {
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Energy returned: " << simulstate.E());
  ATH_MSG_DEBUG("Energy fraction for layer: ");
  for (int s = 0; s < CaloCell_ID_FCS::MaxSample; s++)
  ATH_MSG_DEBUG(" Sampling " << s << " energy " << simulstate.E(s));

  //Now deposit all cell energies into the CaloCellContainer
  for(const auto& iter : simulstate.cells()) {
    CaloCell* theCell = (CaloCell*)m_theContainer->findCell(iter.first->calo_hash());
    theCell->addEnergy(iter.second);
  }
  
  return StatusCode::SUCCESS;
}
