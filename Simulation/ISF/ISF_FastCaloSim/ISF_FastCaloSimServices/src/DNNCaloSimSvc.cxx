/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DNNCaloSimSvc.cxx, (c) ATLAS Detector software             //
///////////////////////////////////////////////////////////////////

// class header include
#include "DNNCaloSimSvc.h"


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

#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"

#include "TFile.h"
#include <fstream>

using std::abs;
using std::atan2;

/** Constructor **/
ISF::DNNCaloSimSvc::DNNCaloSimSvc(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_theContainer(nullptr),
  m_rndGenSvc("AtRndmGenSvc", name),
  m_randomEngine(nullptr),
  m_caloGeo(nullptr)
{
  declareProperty("ParamsInputFilename"            ,       m_paramsFilename,"twoCritic_nn.json");
  //  declareProperty("ParamsInputObject"              ,       m_paramsObject,"SelPDGID");
  declareProperty("CaloCellsOutputName"            ,       m_caloCellsOutputName) ;
  declareProperty("CaloCellMakerTools_setup"       ,       m_caloCellMakerToolsSetup) ;
  declareProperty("CaloCellMakerTools_release"     ,       m_caloCellMakerToolsRelease) ;
  declareProperty("RandomSvc"                      ,       m_rndGenSvc                );
  declareProperty("RandomStream"                   ,       m_randomEngineName         );
  declareProperty("FastCaloSimCaloExtrapolation"   ,       m_FastCaloSimCaloExtrapolation );
}

ISF::DNNCaloSimSvc::~DNNCaloSimSvc()
{}

/** framework methods */
StatusCode ISF::DNNCaloSimSvc::initialize()
{
  ATH_MSG_INFO("start neural network part");
  // get neural net JSON file as an std::istream object
  std::ifstream input(m_paramsFilename);
  // build the graph
  lwt::LightweightGraph graph(lwt::parse_json_graph(input));

  // fill a map of input nodes
  std::map<std::string, std::map<std::string, double> > inputs;
  for (int in_var = 0; in_var< 300; in_var ++)
   {
    inputs["Z"].insert ( std::pair<std::string,double>(std::to_string(in_var),1.) );
    }
  for (int in_var = 0; in_var< 1; in_var ++)
   {
    inputs["E_true"].insert ( std::pair<std::string,double>(std::to_string(in_var),1.) );
    }
  for (int in_var = 0; in_var< 4; in_var ++)
   {
    inputs["pconfig"].insert ( std::pair<std::string,double>(std::to_string(in_var),1.) );
    }
  for (int in_var = 0; in_var< 2; in_var ++)
   {
    inputs["econfig"].insert ( std::pair<std::string,double>(std::to_string(in_var),1.) );
    }
  for (int in_var = 0; in_var< 2; in_var ++)
   {
    inputs["ripos"].insert ( std::pair<std::string,double>(std::to_string(in_var),1.) );
    }
  // compute the output values
  std::map<std::string, double> outputs = graph.compute(inputs);
  ATH_MSG_INFO("neural network output = "<<outputs);

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

  // FIXME should open the jason file here

  // std::unique_ptr<TFile> paramsFile(TFile::Open( inputFile.c_str(), "READ" ));
  // if (paramsFile == nullptr) {
  //   ATH_MSG_ERROR("file = "<<m_paramsFilename<< " not found");
  //   return StatusCode::FAILURE;
  // }
  // ATH_MSG_INFO("Opened parametrization file = "<<m_paramsFilename);
  // paramsFile->ls();
  // m_param=(TFCSParametrizationBase*)paramsFile->Get(m_paramsObject.c_str());
  // if(!m_param) {
  //   ATH_MSG_WARNING("file = "<<m_paramsFilename<< ", object "<< m_paramsObject<<" not found");
  //   return StatusCode::FAILURE;
  // }
  
  // paramsFile->Close();
  
  // m_param->set_geometry(m_caloGeo);
  // m_param->Print("short");
  // m_param->setLevel(MSG::DEBUG);
  
  // Get FastCaloSimCaloExtrapolation
  if(m_FastCaloSimCaloExtrapolation.retrieve().isFailure())
  {
   ATH_MSG_ERROR("FastCaloSimCaloExtrapolation not found ");
   return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::DNNCaloSimSvc::finalize()
{
  ATH_MSG_INFO(m_screenOutputPrefix << "Finalizing ...");
  return StatusCode::SUCCESS;
}

StatusCode ISF::DNNCaloSimSvc::setupEvent()
{
  ATH_MSG_INFO(m_screenOutputPrefix << "setupEvent NEW EVENT! DRDR");
  
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
    std::string chronoName=this->name()+"_"+ itrTool->name();
    if (m_chrono) m_chrono->chronoStart(chronoName);
    StatusCode sc = (*itrTool)->process(m_theContainer);
    if (m_chrono) {
      m_chrono->chronoStop(chronoName);
      ATH_MSG_DEBUG( m_screenOutputPrefix << "Chrono stop : delta " << m_chrono->chronoDelta (chronoName,IChronoStatSvc::USER) * CLHEP::microsecond / CLHEP::second << " second " );
    }

    if (sc.isFailure())
    {
      ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() );
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ISF::DNNCaloSimSvc::releaseEvent()
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
StatusCode ISF::DNNCaloSimSvc::simulate(const ISF::ISFParticle& isfp)
{

  ATH_MSG_VERBOSE("NEW PARTICLE! DNNCaloSimSvc called with ISFParticle: " << isfp);
 
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

  ATH_MSG_DEBUG(" particle: " << isfp.pdgCode() << " Ekin: " << isfp.ekin() << " position eta: " << particle_position.eta() << " direction eta: " << particle_direction.eta() << " position phi: " << particle_position.phi() << "direction phi: " << particle_direction.phi() );

  TFCSTruthState truth(isfp.momentum().x(),isfp.momentum().y(),isfp.momentum().z(),sqrt(isfp.momentum().mag2()+pow(isfp.mass(),2)),isfp.pdgCode());
  truth.set_vertex(particle_position[Amg::x], particle_position[Amg::y], particle_position[Amg::z]);

  TFCSExtrapolationState extrapol;
  m_FastCaloSimCaloExtrapolation->extrapolate(extrapol,&truth);
  ATH_MSG_DEBUG(" calo surface eta " << extrapol.CaloSurface_eta() <<
		" phi " << extrapol.CaloSurface_phi() );
  extrapol.Print();
 // namespace CaloCell_ID_FCS {
 //   enum CaloSample {
 //     FirstSample=CaloCell_ID::PreSamplerB,
 //     PreSamplerB=CaloCell_ID::PreSamplerB, EMB1=CaloCell_ID::EMB1, EMB2=CaloCell_ID::EMB2, EMB3=CaloCell_ID::EMB3,       // LAr barrel
 //     PreSamplerE=CaloCell_ID::PreSamplerE, EME1=CaloCell_ID::EME1, EME2=CaloCell_ID::EME2, EME3=CaloCell_ID::EME3,       // LAr EM endcap
 //     HEC0=CaloCell_ID::HEC0, HEC1=CaloCell_ID::HEC1, HEC2=CaloCell_ID::HEC2, HEC3=CaloCell_ID::HEC3,      // Hadronic end cap cal.
 //     TileBar0=CaloCell_ID::TileBar0, TileBar1=CaloCell_ID::TileBar1, TileBar2=CaloCell_ID::TileBar2,      // Tile barrel
 //     TileGap1=CaloCell_ID::TileGap1, TileGap2=CaloCell_ID::TileGap2, TileGap3=CaloCell_ID::TileGap3,      // Tile gap (ITC & scint)
 //     TileExt0=CaloCell_ID::TileExt0, TileExt1=CaloCell_ID::TileExt1, TileExt2=CaloCell_ID::TileExt2,      // Tile extended barrel
 //     FCAL0=CaloCell_ID::FCAL0, FCAL1=CaloCell_ID::FCAL1, FCAL2=CaloCell_ID::FCAL2,                        // Forward EM endcap
 
 //     // Beware of MiniFCAL! We don't have it, so different numbers after FCAL2
     
 //     LastSample = CaloCell_ID::FCAL2,
 //     MaxSample = LastSample+1,
 //     noSample = -1
 

  for (int isam=0; isam< CaloCell_ID_FCS::MaxSample ; isam++){
    //enum SUBPOS { SUBPOS_MID = 0, SUBPOS_ENT = 1, SUBPOS_EXT = 2}; //MID=middle, ENT=entrance, EXT=exit of cal layer

    for (int isubpos=0; isubpos< 3 ; isubpos++){
    
      ATH_MSG_DEBUG("EXTRAPO isam=" << isam <<
		    " isubpos=" << isubpos <<
		    " OK="    << extrapol.OK(isam,isubpos) <<
		    " eta="  << extrapol.eta(isam,isubpos) <<
		    " phi="  << extrapol.phi(isam,isubpos) <<
		    " r="  << extrapol.r(isam,isubpos) );

    }
  }

  TFCSSimulationState simulstate(m_randomEngine);


  
  ATH_MSG_DEBUG ("DNNCaloSim only debugging!");
  //  m_param->setLevel(MSG::DEBUG);
  // compute simulstate, simulated cells and a few more goodies
  // FIXME run the DNN HERE
  //  FCSReturnCode status = m_param->simulate(simulstate, &truth, &extrapol);
  // if (status != FCSSuccess) {
  //  return StatusCode::FAILURE;
  // }


  // note that m_theCellContainer has all the calorimeter cells
  //for(const auto& theCell : * m_theCellContainer) {
  int n_sqCuts = 0;
  double vall, valll;
  std::vector<CaloCell> v;
  v.reserve(266);

  CaloCell_ID::CaloSample sampling;
  for(const auto& theCell : * m_theContainer) {
   //theCaloDDE=theCell->caloDDE();
    sampling = theCell->caloDDE()->getSampling();
    if ((theCell->caloDDE()->eta_raw() < 0.2125 + 0.025*4) && (theCell->caloDDE()->eta_raw() > 0.2125 - 0.025*4)) {
      if ((theCell->caloDDE()->phi_raw() < (-1.242524) + 0.02454*6) && (theCell->caloDDE()->phi_raw() > (-1.242524) - 0.02454*6)) {

      }
      else{
        continue;
      }
  }
  else{
    continue;
  }
      
  if ((sampling == 0) || (sampling == 1) ){
    if ((theCell->caloDDE()->eta_raw() < 0.2125 + 0.025*3.5) && (theCell->caloDDE()->eta_raw() > 0.2125 - 0.025*3.5)) {
    n_sqCuts ++;
    v.push_back(*theCell);
    // add to vector
  }
  }
  else if((sampling == 2)) {
    if ((theCell->caloDDE()->phi_raw() < (-1.242524) + (0.0859)) && (theCell->caloDDE()->phi_raw() > (-1.242524) - (0.0859))) {
      if ((theCell->caloDDE()->eta_raw() < 0.2125 + 0.025*3.5) && (theCell->caloDDE()->eta_raw() > 0.2125 - 0.025*3.5)) {
        n_sqCuts ++;
        v.push_back(*theCell);
      }
    }
  }

  else if(sampling == 3){
  if ((theCell->caloDDE()->phi_raw() < (-1.242524) + (0.0859)) && (theCell->caloDDE()->phi_raw() > (-1.242524) - (0.0859))) {
    n_sqCuts ++;
    v.push_back(*theCell);
  }

}
//n_sqCuts ++;
 vall = theCell->caloDDE()->eta_raw();
 valll = theCell->caloDDE()->phi_raw();
    
    //ATH_MSG_DEBUG("NNN particle: " << theCell->eta_raw() << " phi raw" << theCell->caloDDE()->phi_raw() << theCell->caloDDE()->calo_hash() << " Ekin: " << isfp.ekin() << " position eta: " << particle_position.eta() << " direction eta: " << particle_direction.eta() << " position phi: " << particle_position.phi() << "direction phi: " << particle_direction.phi() );
     //break;
    //CaloCell* theCell = (CaloCell*)m_theContainer->findCell(iter.first->calo_hash());
    //theCell->addEnergy(iter.second);
  }

  // std::stable_sort by phi, eta, layer in that order to get 266 in right order


  ATH_MSG_DEBUG("NNN total cells passing cuts     " << n_sqCuts << "        eta phi raw" << vall << "and" << valll<< "sampling" << sampling);
  //ATH_MSG_DEBUG("NNN total cells passing cuts" << n_sqCuts );
  //for(const auto& windowCell : * v) {
  std::vector<CaloCell>::iterator windowCell;
  for ( windowCell = v.begin(); windowCell != v.end(); ++windowCell ) {
    ATH_MSG_DEBUG("NNN eta phi " << windowCell->caloDDE()->eta_raw() << " and " << windowCell->caloDDE()->phi_raw() << " sampling " <<
      windowCell->caloDDE()->getSampling());

  }

  // step 1 : gather all inputs
  // given extrapolation find the impact cell in the middle layer,
  // we need : icalohash, ieta, iphi of the middle layer cell impact
  // we need : the eta and phi impact parameter
  // energy 
  // from icalohash, ieta, iphi we need theconfiguration
   // fill the short array of cell calo_hash, and reset the energy in the short array of cell calo_hash

  // step 2 : run the network
  // generate the latent space
  // run the network => output short vector of energies

  // step 3 finalise
  // add the energies from the short vector to full CaloCellContainer



  // ATH_MSG_DEBUG("Energy returned: " << simulstate.E());
  // ATH_MSG_VERBOSE("Energy fraction for layer: ");
  // for (int s = 0; s < CaloCell_ID_FCS::MaxSample; s++)
  // ATH_MSG_VERBOSE(" Sampling " << s << " energy " << simulstate.E(s));

  // //Now deposit all cell energies into the CaloCellContainer
  // for(const auto& iter : simulstate.cells()) {
  //   CaloCell* theCell = (CaloCell*)m_theContainer->findCell(iter.first->calo_hash());
  //   theCell->addEnergy(iter.second);
  // }


  
  return StatusCode::SUCCESS;
}
