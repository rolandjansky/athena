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

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"

#include "PathResolver/PathResolver.h"

#include "lwtnn/parse_json.hh"

#include "TFile.h"
#include <fstream>
#include "CLHEP/Random/RandGauss.h"

using std::abs;
using std::atan2;

/** Constructor **/
ISF::DNNCaloSimSvc::DNNCaloSimSvc(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_graph(nullptr),
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


  // initialize DNN 
  if (initializeNetwork().isFailure())
    {
      ATH_MSG_ERROR("Could not initialize network ");
      return StatusCode::FAILURE;

    }

  
  // Get FastCaloSimCaloExtrapolation
  if(m_FastCaloSimCaloExtrapolation.retrieve().isFailure())
  {
   ATH_MSG_ERROR("FastCaloSimCaloExtrapolation not found ");
   return StatusCode::FAILURE;
  }

  m_windowCells.reserve(m_numberOfCellsForDNN_const);
  
  return StatusCode::SUCCESS;
}

// initialize lwtnn network 
StatusCode ISF::DNNCaloSimSvc::initializeNetwork()
{

  // get neural net JSON file as an std::istream object
  std::string inputFile=PathResolverFindCalibFile(m_paramsFilename);
  if (inputFile==""){
    ATH_MSG_ERROR("Could not find json file " << m_paramsFilename );
    return StatusCode::FAILURE;
  } 

  ATH_MSG_DEBUG("Using json file " << m_paramsFilename );
  std::ifstream input(inputFile);
  // build the graph
  m_graph=new lwt::LightweightGraph(lwt::parse_json_graph(input));
  if (m_graph==nullptr){
    ATH_MSG_ERROR("Could not create LightWeightGraph from  " << m_paramsFilename );
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

    if (((a->caloDDE()->phi_raw()) > (b->caloDDE()->phi_raw()))){
    	// check for pi -pi discontinuity
    	if ((((a->caloDDE()->phi_raw()) - (b->caloDDE()->phi_raw()))) > CLHEP::pi )
    		return true;
    	else
        	return false;
    }
    // check for pi -pi discontinuity
    else if ((((b->caloDDE()->phi_raw()) - (a->caloDDE()->phi_raw()))) > CLHEP::pi )
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

    if (((a->caloDDE()->phi_raw()) > (b->caloDDE()->phi_raw()))){
    	// check for pi -pi discontinuity
    	if ((((a->caloDDE()->phi_raw()) - (b->caloDDE()->phi_raw()))) > CLHEP::pi )
    		return true;
    	else
        	return false;
    }
    // check for pi -pi discontinuity
    else if ((((b->caloDDE()->phi_raw()) - (a->caloDDE()->phi_raw()))) > CLHEP::pi )
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


  TFCSTruthState truth(isfp.momentum().x(),isfp.momentum().y(),isfp.momentum().z(),sqrt(isfp.momentum().mag2()+pow(isfp.mass(),2)),isfp.pdgCode());
  truth.set_vertex(particle_position[Amg::x], particle_position[Amg::y], particle_position[Amg::z]);

  TFCSExtrapolationState extrapol;
  //FIXME this is extrapolating to many layers, when we only need middle layer middle surface
  //FIXME could have dedicated extrapolation to save time 
  m_FastCaloSimCaloExtrapolation->extrapolate(extrapol,&truth);
  //  extrapol.Print();
 

  if (false)
    {
      for (int isam=0; isam< CaloCell_ID_FCS::MaxSample ; isam++){
	//enum SUBPOS { SUBPOS_MID = 0, SUBPOS_ENT = 1, SUBPOS_EXT = 2}; //MID=middle, ENT=entrance, EXT=exit of cal layer

	for (int isubpos=0; isubpos< 3 ; isubpos++){
      
	  ATH_MSG_VERBOSE("EXTRAPO isam=" << isam <<
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

  ATH_MSG_VERBOSE("Will use isam=" << isam <<
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

  ATH_MSG_VERBOSE("impact cell calohash=" << caloHashImpactCell <<
		" eta="  << etaImpactCell << 
		" phi="  << phiImpactCell <<
		" eta raw="  << etaRawImpactCell << 
		" phi raw="  << phiRawImpactCell <<
    " true energy=" << trueEnergy  );

  int impactEtaIndex = m_emID->eta(impactCellDDE->identify());
  int impactPhiIndex = m_emID->phi(impactCellDDE->identify());

  ATH_MSG_VERBOSE("impact eta_index " << m_emID->eta(impactCellDDE->identify()) 
		  << " phi_index " << m_emID->phi(impactCellDDE->identify()) 
		  << " sampling " << m_emID->sampling(impactCellDDE->identify()));


  //FIXME move to initialize?
  TFCSSimulationState simulstate(m_randomEngine);

  int n_sqCuts = 0;


  // select the cells DNN will simulate 
  // note that m_theCellContainer has all the calorimeter cells
  CaloCell_ID::CaloSample sampling;
  for(const auto& theCell : * m_theContainer) {
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
	// add to vector
	m_windowCells.push_back(theCell);
	
      }
    }
    else if((sampling == 2)) {
      if ((theCell->caloDDE()->phi_raw() < phiRawImpactCell + m_PhiRawMiddleCut_const) && (theCell->caloDDE()->phi_raw() > phiRawImpactCell - m_PhiRawMiddleCut_const)) {
	if ((theCell->caloDDE()->eta_raw() < etaRawImpactCell + m_EtaRawMiddleCut_const) && (theCell->caloDDE()->eta_raw() > etaRawImpactCell - m_EtaRawMiddleCut_const)) {
	  n_sqCuts ++;
	  m_windowCells.push_back(theCell);
	}
      }
    }

    else if(sampling == 3){
      if ((theCell->caloDDE()->phi_raw() < phiRawImpactCell + m_PhiRawMiddleCut_const) && (theCell->caloDDE()->phi_raw() > phiRawImpactCell - m_PhiRawMiddleCut_const)) {
	n_sqCuts ++;
	m_windowCells.push_back(theCell);
      }

    }
  }

  if (n_sqCuts != m_numberOfCellsForDNN_const){
  	ATH_MSG_ERROR("Total cells passing DNN selection is " << n_sqCuts << " but should be " << m_numberOfCellsForDNN_const );
  	// FIXME at this point bail out
  	return StatusCode::FAILURE;

  }


  // start neural network part
  // fill a map of input nodes
  std::map<std::string, std::map<std::string, double> > inputs;
  double riImpactEta;
  double riImpactPhi;
  double randGaussz = 0.;

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

  // fill randomize latent space
  for (int in_var = 0; in_var< m_GANLatentSize_const; in_var ++)
    {
      randGaussz = CLHEP::RandGauss::shoot(simulstate.randomEngine(), 0., 1.);
      inputs["Z"].insert ( std::pair<std::string,double>(std::to_string(in_var), randGaussz) );

    }

  // fill preprocessed true energy
  for (int in_var = 0; in_var< 1; in_var ++)
    {
      inputs["E_true"].insert ( std::pair<std::string,double>(std::to_string(in_var), (std::log(trueEnergy) - m_logTrueEnergyMean_const)/m_logTrueEnergyScale_const) );
    }
  // fill p,e configurations multi-hot vector
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
  // fill position of extrap particle in impact cell
  inputs["ripos"].insert ( std::pair<std::string,double>("0", riImpactEta) ); 
  inputs["ripos"].insert ( std::pair<std::string,double>("1", riImpactPhi ) );

  // compute the output values
  std::map<std::string, double> outputs = m_graph->compute(inputs);
  ATH_MSG_VERBOSE("neural network output = "<<outputs);

 // sort cells within the cluster like they are fed to DNN
  if (etaRawImpactCell < 0){
    std::sort(m_windowCells.begin(), m_windowCells.end(), &compCellsForDNNSortMirror);
  }
  else{
    std::sort(m_windowCells.begin(), m_windowCells.end(), &compCellsForDNNSort);
  }

  std::vector<CaloCell*>::iterator windowCell;
  int itr = 0;
  // double preprocessedPhiIndex = -999.;
  // double preprocessedEtaIndex = -999.;

  for ( windowCell = m_windowCells.begin(); windowCell != m_windowCells.end(); ++windowCell ) {
    (*windowCell)->addEnergy(trueEnergy * outputs[std::to_string(itr)]);
    itr++;

    // ATH_MSG_VERBOSE("ImpactEtaRaw" << etaRawImpactCell << " cell eta_raw " << (*windowCell)->caloDDE()->eta_raw() << " phi_raw " << (*windowCell)->caloDDE()->phi_raw() << " sampling " <<
		  // (*windowCell)->caloDDE()->getSampling() << "energy " << (*windowCell)->energy());
    // preprocessedPhiIndex = (*windowCell)->caloDDE()->phi_raw() - (-3.1323888); // phi_right
    // //preprocessedPhiIndex = (*windowCell)->caloDDE()->phi_raw() - (-3.126253); // phi_left

    // //preprocessedEtaIndex = (*windowCell)->caloDDE()->eta_raw() - (-1.4375); // eta_left
    // preprocessedEtaIndex = (*windowCell)->caloDDE()->eta_raw() - (1.4375); // eta_right

    // if (preprocessedPhiIndex > CLHEP::pi){
    //   preprocessedPhiIndex -= 2 * CLHEP::pi;
    // }
    // preprocessedPhiIndex /= m_MiddleCellWidthPhi_const;
    // preprocessedEtaIndex /= m_MiddleCellWidthEta_const;
    // ATH_MSG_VERBOSE("cell eta_index " << m_emID->eta((*windowCell)->caloDDE()->identify()) << " phi_index " << m_emID->phi((*windowCell)->caloDDE()->identify()) <<
		  // "  sampling " << m_emID->sampling((*windowCell)->caloDDE()->identify()) << 
		  // "  preprocessedPhiIndex(right) " << preprocessedPhiIndex <<
		  // "  preprocessedEtaIndex(right) " << preprocessedEtaIndex);
  }
  m_windowCells.clear();


  
  return StatusCode::SUCCESS;
}
