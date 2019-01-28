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
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

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
#include "CaloIdentifier/CaloIdManager.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"

#include "PathResolver/PathResolver.h"

#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"

#include "TFile.h"
#include <fstream>
#include "CLHEP/Random/RandGauss.h"

using std::abs;
using std::atan2;

/** Constructor **/
ISF::DNNCaloSimSvc::DNNCaloSimSvc(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_theContainer(nullptr),
  m_rndGenSvc("AtRndmGenSvc", name),
  m_randomEngine(nullptr),
  m_caloDetDescrManager(nullptr),
  m_caloGeo(nullptr)
{
  declareProperty("ParamsInputFilename"            ,       m_paramsFilename,"DNNCaloSim/DNNCaloSim_GAN_nn_v0.json");
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
  ATH_MSG_INFO(m_screenOutputPrefix << "Initializing ...");

  ATH_CHECK(m_rndGenSvc.retrieve());
  m_randomEngine = m_rndGenSvc->GetEngine( m_randomEngineName);
  if(!m_randomEngine)
  {
   ATH_MSG_ERROR("Could not get random number engine from RandomNumberService. Abort.");
   return StatusCode::FAILURE;
  }
  
  m_caloDetDescrManager  = CaloDetDescrManager::instance();
  const FCALDetectorManager * fcalManager=NULL;
  ATH_CHECK(detStore()->retrieve(fcalManager));

  const CaloIdManager* caloId_mgr = m_caloDetDescrManager->getCalo_Mgr();
  m_emID = caloId_mgr->getEM_ID();
  
  m_caloGeo = new CaloGeometryFromCaloDDM();
  m_caloGeo->LoadGeometryFromCaloDDM(m_caloDetDescrManager);
  if(!m_caloGeo->LoadFCalChannelMapFromFCalDDM(fcalManager) )ATH_MSG_FATAL("Found inconsistency between FCal_Channel map and GEO file. Please, check if they are configured properly.");


  // FIXME should open the json file here


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
bool compCellsForDNNSortMirror(const CaloCell* a, const CaloCell* b)
{
    if ((a->caloDDE()->getSampling()) < (b->caloDDE()->getSampling()))
        return true;
    else if ((a->caloDDE()->getSampling()) > (b->caloDDE()->getSampling()))
        return false;
      // reverse sort in eta for left half of detector
    if ((a->caloDDE()->eta_raw()) < (b->caloDDE()->eta_raw()))
        return false;
    else if ((a->caloDDE()->eta_raw()) > (b->caloDDE()->eta_raw()))
        return true;

    if (((a->caloDDE()->phi_raw()) > (b->caloDDE()->phi_raw())))
        return false;

    return true;
}

bool compCellsForDNNSort(const CaloCell* a, const CaloCell* b)
{
    if ((a->caloDDE()->getSampling()) < (b->caloDDE()->getSampling()))
        return true;
    else if ((a->caloDDE()->getSampling()) > (b->caloDDE()->getSampling()))
        return false;

    if ((a->caloDDE()->eta_raw()) < (b->caloDDE()->eta_raw()))
        return true;
    else if ((a->caloDDE()->eta_raw()) > (b->caloDDE()->eta_raw()))
        return false;

    if (((a->caloDDE()->phi_raw()) > (b->caloDDE()->phi_raw())))
        return false;

    return true;
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
  //FIXME this is extrapolating to many layers, when we only need middle layer middle surface
  //FIXME could have dedicated extrapolation to save time 
  //FIXME check which surface was used to generate the training file
  m_FastCaloSimCaloExtrapolation->extrapolate(extrapol,&truth);
  ATH_MSG_DEBUG(" calo surface eta " << extrapol.CaloSurface_eta() <<
		" phi " << extrapol.CaloSurface_phi() );
  //  extrapol.Print();
 

  if (false)
    {
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
    }

  //FIXME deal with endcap as well 
  int isam=CaloCell_ID_FCS::EMB2;
  int isubpos=SUBPOS_ENT;
  double etaExtrap=-999.;
  double phiExtrap=-999.;
  if (extrapol.eta(isam,isubpos)) {
    etaExtrap=extrapol.eta(isam,isubpos);
    phiExtrap=extrapol.phi(isam,isubpos);
  }

  ATH_MSG_DEBUG("Will use isam=" << isam <<
		" isubpos=" << isubpos <<
		" eta="  << etaExtrap << 
		" phi="  << phiExtrap );

  //now find the cell it corresponds to  
  //FIXME this is barrel should also look in endcap 
  // (note that is really looking up eta, phi, not raw eta phi
  const CaloDetDescrElement* impactCellDDE=m_caloDetDescrManager->get_element(CaloCell_ID::EMB2,etaExtrap,phiExtrap);
  double caloHashImpactCell=-999;
  double etaImpactCell=-999;
  double phiImpactCell=-999;
  double etaRawImpactCell=-999;
  double phiRawImpactCell=-999;
  double trueEnergy = -999;

  trueEnergy = isfp.ekin();

  if (impactCellDDE!=nullptr){
    caloHashImpactCell=impactCellDDE->calo_hash();
    etaImpactCell=impactCellDDE->eta();
    phiImpactCell=impactCellDDE->phi();
    etaRawImpactCell=impactCellDDE->eta_raw();
    phiRawImpactCell=impactCellDDE->phi_raw();

  }

  ATH_MSG_DEBUG("impact cell calohash=" << caloHashImpactCell <<
		" eta="  << etaImpactCell << 
		" phi="  << phiImpactCell <<
		" eta raw="  << etaRawImpactCell << 
		" phi raw="  << phiRawImpactCell <<
    " true energy=" << trueEnergy  );

  int impactEtaIndex = m_emID->eta(impactCellDDE->identify());
  int impactPhiIndex = m_emID->phi(impactCellDDE->identify());

    ATH_MSG_DEBUG("rrr impact eta_index " << m_emID->eta(impactCellDDE->identify()) << " phi_index " << m_emID->phi(impactCellDDE->identify()) <<
    " sampling " << m_emID->sampling(impactCellDDE->identify()));



  //FIXME remove ?
  TFCSSimulationState simulstate(m_randomEngine);


  // note that m_theCellContainer has all the calorimeter cells

  //for(const auto& theCell : * m_theCellContainer) {
  int n_sqCuts = 0;
  double vall, valll;
  std::vector<CaloCell*> windowCells;
  windowCells.reserve(266);



  CaloCell_ID::CaloSample sampling;
  for(const auto& theCell : * m_theContainer) {
   //theCaloDDE=theCell->caloDDE();
    sampling = theCell->caloDDE()->getSampling();
    if ((theCell->caloDDE()->eta_raw() < etaRawImpactCell + m_EtaRawBackCut_const) && (theCell->caloDDE()->eta_raw() > etaRawImpactCell - m_EtaRawBackCut_const)) {
      if ((theCell->caloDDE()->phi_raw() < phiRawImpactCell + m_PhiRawStripCut_const) && (theCell->caloDDE()->phi_raw() > phiRawImpactCell - m_PhiRawStripCut_const)) {

      }
      else{
        continue;
      }
  }
  else{
    continue;
  }
      
  if ((sampling == 0) || (sampling == 1) ){
    if ((theCell->caloDDE()->eta_raw() < etaRawImpactCell + m_EtaRawMiddleCut_const) && (theCell->caloDDE()->eta_raw() > etaRawImpactCell - m_EtaRawMiddleCut_const)) {
    n_sqCuts ++;
    windowCells.push_back(theCell);
    // add to vector
  }
  }
  else if((sampling == 2)) {
    if ((theCell->caloDDE()->phi_raw() < phiRawImpactCell + m_PhiRawMiddleCut_const) && (theCell->caloDDE()->phi_raw() > phiRawImpactCell - m_PhiRawMiddleCut_const)) {
      if ((theCell->caloDDE()->eta_raw() < etaRawImpactCell + m_EtaRawMiddleCut_const) && (theCell->caloDDE()->eta_raw() > etaRawImpactCell - m_EtaRawMiddleCut_const)) {
        n_sqCuts ++;
        windowCells.push_back(theCell);
      }
    }
  }

  else if(sampling == 3){
  if ((theCell->caloDDE()->phi_raw() < phiRawImpactCell + m_PhiRawMiddleCut_const) && (theCell->caloDDE()->phi_raw() > phiRawImpactCell - m_PhiRawMiddleCut_const)) {
    n_sqCuts ++;
    windowCells.push_back(theCell);
  }

}
 vall = theCell->caloDDE()->eta_raw();
 valll = theCell->caloDDE()->phi_raw();
  }

  // FIXME std::stable_sort by phi, eta, layer in that order to get 266 in right order

  ATH_MSG_DEBUG("NNN total cells passing cuts     " << n_sqCuts << "        eta phi raw " << vall << " and " << valll<< " sampling " << sampling);


  ATH_MSG_INFO("start neural network part");

  // FIXME do once, what can be done once
  // get neural net JSON file as an std::istream object
  std::string inputFile=PathResolverFindCalibFile(m_paramsFilename);
  if (inputFile==""){
    ATH_MSG_ERROR("Could not find json file " << m_paramsFilename );
    return StatusCode::FAILURE;
  } 

  ATH_MSG_DEBUG("Using json file " << m_paramsFilename );
  

  std::ifstream input(inputFile);
  // build the graph
  lwt::LightweightGraph graph(lwt::parse_json_graph(input));

  // fill a map of input nodes
  std::map<std::string, std::map<std::string, double> > inputs;
  double riImpactEta;
  double riImpactPhi;
  double randGaussz = 0.;
  double sum = 0.;
  double sqsum = 0.;
  double sd = 0.;

  int pconf = impactPhiIndex % 4 ;
  int econf = (impactEtaIndex + 1) % 2 ; // ofset corresponds to difference in index calculated for neural net preprocessing
    
    riImpactEta = ((etaExtrap - etaRawImpactCell) - m_riImpactEtaMean_const)/m_riImpactEtaScale_const; // ??? or imact - extrap?
    riImpactPhi = ((phiExtrap - phiRawImpactCell) - m_riImpactPhiMean_const);
    // keep phi in -pi to pi
    if (riImpactPhi > CLHEP::pi){
      riImpactPhi -= 2 * CLHEP::pi;
    }
    else if (riImpactPhi < - CLHEP::pi){
      riImpactPhi += 2 * CLHEP::pi;
    }
    riImpactPhi = riImpactPhi/m_riImpactPhiScale_const;

  for (int in_var = 0; in_var< 300; in_var ++)
   {
    randGaussz = CLHEP::RandGauss::shoot(simulstate.randomEngine(), 0., 1.);
    inputs["Z"].insert ( std::pair<std::string,double>(std::to_string(in_var), randGaussz) );
    sum+= randGaussz;
    sqsum += std::pow(randGaussz,2);

    }
    sum /=300.;
    sqsum /= 300.;
    sd = std::sqrt(sqsum - std::pow(sum, 2));
    ATH_MSG_DEBUG("mean std of gaussian = "<<sum << " and " << sd);


  for (int in_var = 0; in_var< 1; in_var ++)
   {
    inputs["E_true"].insert ( std::pair<std::string,double>(std::to_string(in_var), (std::log(trueEnergy) - m_logTrueEnergyMean_const)/m_logTrueEnergyScale_const) );
    }
  for (int in_var = 0; in_var< 4; in_var ++)
   {
    if (in_var == pconf){
      inputs["pconfig"].insert ( std::pair<std::string,double>(std::to_string(in_var),1.) );
    }
    else{
      inputs["pconfig"].insert ( std::pair<std::string,double>(std::to_string(in_var),0.) );
    }
    }
  for (int in_var = 0; in_var< 2; in_var ++){
    if (in_var == econf){
      inputs["econfig"].insert ( std::pair<std::string,double>(std::to_string(in_var),1.) );
    }
    else{
      inputs["econfig"].insert ( std::pair<std::string,double>(std::to_string(in_var),0.) );
    }
  }

    inputs["ripos"].insert ( std::pair<std::string,double>("0", riImpactEta) ); 
    inputs["ripos"].insert ( std::pair<std::string,double>("1", riImpactPhi ) );

  // compute the output values
  std::map<std::string, double> outputs = graph.compute(inputs);
  ATH_MSG_DEBUG("neural network output = "<<outputs);


  std::vector<CaloCell*>::iterator windowCell;
  int i = 0;
  double myPhiIndex = -999.;
  double myEtaIndex = -999.;

  if (etaRawImpactCell < 0){
    std::sort(windowCells.begin(), windowCells.end(), &compCellsForDNNSortMirror);
  }
  else{
    std::sort(windowCells.begin(), windowCells.end(), &compCellsForDNNSort);
  }
  

  for ( windowCell = windowCells.begin(); windowCell != windowCells.end(); ++windowCell ) {
    (*windowCell)->addEnergy(trueEnergy * outputs[std::to_string(i)]);

      ATH_MSG_DEBUG("NNN ImactEtaRaw" << etaRawImpactCell << " cell eta_raw " << (*windowCell)->caloDDE()->eta_raw() << " phi_raw " << (*windowCell)->caloDDE()->phi_raw() << " sampling " <<
              (*windowCell)->caloDDE()->getSampling() << "energy " << (*windowCell)->energy());
     myPhiIndex = (*windowCell)->caloDDE()->phi_raw() - (-3.1323888); // phi_right
     //myPhiIndex = (*windowCell)->caloDDE()->phi_raw() - (-3.126253); // phi_left

     //myEtaIndex = (*windowCell)->caloDDE()->eta_raw() - (-1.4375); // eta_left
     myEtaIndex = (*windowCell)->caloDDE()->eta_raw() - (1.4375); // eta_right

     if (myPhiIndex > CLHEP::pi){
      myPhiIndex -= 2 * CLHEP::pi;
     }
     myPhiIndex /= m_MiddleCellWidthPhi_const;
     myEtaIndex /= m_MiddleCellWidthEta_const;
    //if (((*windowCell)->caloDDE()->eta_raw() < 0) && (*windowCell)->caloDDE()->getSampling() == 2) {
    // if (((*windowCell)->caloDDE()->eta_raw() > 0) && (*windowCell)->caloDDE()->getSampling() == 2) {
      ATH_MSG_DEBUG("lll cell eta_index " << m_emID->eta((*windowCell)->caloDDE()->identify()) << " phi_index " << m_emID->phi((*windowCell)->caloDDE()->identify()) <<
        "  sampling " << m_emID->sampling((*windowCell)->caloDDE()->identify()) << 
        "  myPhiIndex(right) " << myPhiIndex <<
      "  myEtaIndex(right) " << myEtaIndex);
     // }
      
      i++;
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
