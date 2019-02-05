/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDAlignMonPVBiases.cxx
// AUTHORS: Ambrosius  Vermeulen, Pierfrancesco Butti
// **********************************************************************

#include <sstream>
#include <math.h>
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TMath.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"


//#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "xAODEventInfo/EventInfo.h"

//#include "AthenaMonitoring/AthenaMonManager.h"
#include "IDAlignMonPVBiases.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"
#include "TrackSelectionTool.h"


// *********************************************************************
// Public Methods
// *********************************************************************

IDAlignMonPVBiases::IDAlignMonPVBiases( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
  m_events(0),
  m_histosBooked(0),
  m_tracksName("ExtendedTracks"),
  m_triggerChainName("NoTriggerSelection")
  //m_TreeFolder("/PVbiases/PVbiases"),
  //m_Tree(0),
  //m_TreeName("PVbiases")
{
  m_trackSelection = ToolHandle< InDetAlignMon::TrackSelectionTool >("InDetAlignMon::TrackSelectionTool");

  InitializeHistograms();

  declareProperty("tracksName"           , m_tracksName);
  declareProperty("CheckRate"            , m_checkrate=1000);
  declareProperty("triggerChainName"     , m_triggerChainName);
  declareProperty("trackSelection"       , m_trackSelection);
  declareProperty("VxPrimContainerName"  , m_VxPrimContainerName);
 
}


IDAlignMonPVBiases::~IDAlignMonPVBiases() { }

void IDAlignMonPVBiases::InitializeHistograms() {

	/////////////////////////////////////////////////
    	//////Initialize histo's 400MeV until 600MeV/////
    	/////////////////////////////////////////////////
    	m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive = 0;
    	m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative = 0;

    	m_trkd0_wrtPV_vs_phi_400MeV_600MeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_phi_400MeV_600MeV_negative 	= 0;

	m_trkd0_wrtPV_vs_eta_400MeV_600MeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_eta_400MeV_600MeV_negative 	= 0;
 
	/////////////////////////////////////////////////
    	///////Initialize histo's 600MeV until 1GeV//////
    	/////////////////////////////////////////////////
	m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive = 0;
    	m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative = 0;

	m_trkd0_wrtPV_vs_phi_600MeV_1GeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_phi_600MeV_1GeV_negative 	= 0;

	m_trkd0_wrtPV_vs_eta_600MeV_1GeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_eta_600MeV_1GeV_negative 	= 0;
  
	/////////////////////////////////////////////////
    	////////Initialize histo's 1GeV until 2GeV///////
    	/////////////////////////////////////////////////
	m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive	= 0;
    	m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative	= 0;

	m_trkd0_wrtPV_vs_phi_1GeV_2GeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_phi_1GeV_2GeV_negative 	= 0;

	m_trkd0_wrtPV_vs_eta_1GeV_2GeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_eta_1GeV_2GeV_negative 	= 0;
  
	/////////////////////////////////////////////////
    	////////Initialize histo's 2GeV until 5GeV///////
    	/////////////////////////////////////////////////
	m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive 	= 0;
    	m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative 	= 0;

	m_trkd0_wrtPV_vs_phi_2GeV_5GeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_phi_2GeV_5GeV_negative 	= 0;

	m_trkd0_wrtPV_vs_eta_2GeV_5GeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_eta_2GeV_5GeV_negative 	= 0;
  
	/////////////////////////////////////////////////
    	///////Initialize histo's 5GeV until 10GeV///////
    	/////////////////////////////////////////////////
	m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive = 0;
    	m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative = 0;

	m_trkd0_wrtPV_vs_phi_5GeV_10GeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_phi_5GeV_10GeV_negative 	= 0;

	m_trkd0_wrtPV_vs_eta_5GeV_10GeV_positive 	= 0;
  	m_trkd0_wrtPV_vs_eta_5GeV_10GeV_negative 	= 0;
  
	/////////////////////////////////////////////////
    	///////Initialize histo's larger than 10GeV//////
    	/////////////////////////////////////////////////
	m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_positive 	= 0;
    	m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_negative 	= 0;

	m_trkd0_wrtPV_vs_phi_10GeV_positive 		= 0;
  	m_trkd0_wrtPV_vs_phi_10GeV_negative 		= 0;

	m_trkd0_wrtPV_vs_eta_10GeV_positive 		= 0;
  	m_trkd0_wrtPV_vs_eta_10GeV_negative 		= 0;
} 


StatusCode IDAlignMonPVBiases::initialize()
{
  StatusCode sc;
  m_events=0; 
  m_histosBooked=0; 

  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess())
    return StatusCode::SUCCESS;
   
  if ( m_trackSelection.retrieve().isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to retrieve tool " << m_trackSelection << endmsg;
    return StatusCode::SUCCESS;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSelection << endmsg;
  }

  ATH_CHECK( m_vertexKey.initialize() );
/*
  //create tree and branches
  if(m_Tree == 0) {

    m_Tree = new TTree(m_TreeName.c_str(), "Tree");

    m_Tree->Branch("run_number"      ,  &m_runNumber,  "runNumber/I");
    m_Tree->Branch("event_number"    ,  &m_evtNumber,  "eventNumber/I");
    m_Tree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");

    m_Tree->Branch("charge",  &m_charge,  "Charge/D");
    m_Tree->Branch("pt",  &m_pt,  "pt/D");
    m_Tree->Branch("eta", &m_eta, "eta/D");
    m_Tree->Branch("phi", &m_phi, "phi/D");
    m_Tree->Branch("z0",  &m_z0,  "z0/D");
    m_Tree->Branch("d0",  &m_d0,  "d0/D");
    m_Tree->Branch("z0_err",  &m_z0_err,  "z0_err/D");
    m_Tree->Branch("d0_err",  &m_d0_err,  "d0_err/D");
    m_Tree->Branch("vertex_x",  &m_vertex_x,  "vertex_x/D");
    m_Tree->Branch("vertex_y",  &m_vertex_y,  "vertex_y/D");
    m_Tree->Branch("vertex_z",  &m_vertex_z,  "vertex_z/D");
  }

  //register the tree
  ITHistSvc* tHistSvc = 0;
  if (service("THistSvc",tHistSvc).isFailure()){
    ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !");
    //m_validationMode = false;
  }

  if ((tHistSvc->regTree(m_TreeFolder, m_Tree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_Tree; m_Tree = 0;
    //m_validationMode = false;
  }
*/

  return StatusCode::SUCCESS;
}

StatusCode IDAlignMonPVBiases::bookHistograms()
{
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Running in online mode "<<std::endl;
  }
  
  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Running in cosmic mode "<<std::endl;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Running in collision mode "<<std::endl;
  }

  std::string outputDirName = "IDAlignMon/" + m_tracksName + "_" + m_triggerChainName + "/GenericTracks";
  MonGroup al_mon ( this, outputDirName, run );
  MonGroup al_mon_ls ( this, outputDirName, lowStat );
  
  ITHistSvc* tHistSvc = 0;
  if (service("THistSvc",tHistSvc).isFailure()){
    ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !");
    //m_validationMode = false;
  }

  //if ( newLowStatFlag() ) {    }
  //if ( newLumiBlockFlag() ) {    }
  if ( newRunFlag() ) {  

    	//if user environment specified we don't want to book new histograms at every run boundary
    	//we instead want one histogram per job
    	if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) return StatusCode::SUCCESS;

    	const int nd0Bins = 10000;
    	double maxD0 = 5;
   
	const int nphiBins = 50;
	const int nphiBinsMap = 20;
        double maxPhi = 3.1;

	const int netaBins = 50; 
	const int netaBinsMap = 20;
        double maxEta = 2.5; 

 	/////////////////////////////////////////////////
    	///Define+register histo's 400MeV until 600MeV///
    	/////////////////////////////////////////////////
	//TH3D for maps 
	m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive"	, "d0 vs phi vs eta 400MeV-600MeV positive; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);
	m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative"	, "d0 vs phi vs eta 400MeV-600MeV negative; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative);

	//TH2D vs phi
	m_trkd0_wrtPV_vs_phi_400MeV_600MeV_positive	= new TH2F("trk_d0_wrtPV_vs_phi_400MeV_600MeV_positive"		, "d0 vs phi 400MeV-600MeV positive; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_phi_400MeV_600MeV_negative	= new TH2F("trk_d0_wrtPV_vs_phi_400MeV_600MeV_negative"		, "d0 vs phi 400MeV-600MeV negative; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_400MeV_600MeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_400MeV_600MeV_negative	);

	//TH2D vs eta
	m_trkd0_wrtPV_vs_eta_400MeV_600MeV_positive	= new TH2F("trk_d0_wrtPV_vs_eta_400MeV_600MeV_positive"		, "d0 vs eta 400MeV-600MeV positive; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_eta_400MeV_600MeV_negative	= new TH2F("trk_d0_wrtPV_vs_eta_400MeV_600MeV_negative"		, "d0 vs eta 400MeV-600MeV negative; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_400MeV_600MeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_400MeV_600MeV_negative	);
	
	/////////////////////////////////////////////////
    	////Define+register histo's 600MeV until 1GeV////
    	/////////////////////////////////////////////////
 	//TH3D for maps 
	m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive"	, "d0 vs phi vs eta 600MeV-1GeV positive; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);
	m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative"	, "d0 vs phi vs eta 600MeV-1GeV negative; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative);

	//TH2D vs phi
	m_trkd0_wrtPV_vs_phi_600MeV_1GeV_positive	= new TH2F("trk_d0_wrtPV_vs_phi_600MeV_1GeV_positive"		, "d0 vs phi 600MeV-1GeV positive; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_phi_600MeV_1GeV_negative	= new TH2F("trk_d0_wrtPV_vs_phi_600MeV_1GeV_negative"		, "d0 vs phi 600MeV-1GeV negative; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );
	
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_600MeV_1GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_600MeV_1GeV_negative	);

	//TH2D vs eta
	m_trkd0_wrtPV_vs_eta_600MeV_1GeV_positive	= new TH2F("trk_d0_wrtPV_vs_eta_600MeV_1GeV_positive"		, "d0 vs eta 600MeV-1GeV positive; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_eta_600MeV_1GeV_negative	= new TH2F("trk_d0_wrtPV_vs_eta_600MeV_1GeV_negative"		, "d0 vs eta 600MeV-1GeV negative; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_600MeV_1GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_600MeV_1GeV_negative	);
	
	/////////////////////////////////////////////////
    	/////Define+register histo's 1GeV until 2GeV/////
    	/////////////////////////////////////////////////
 	//TH3D for maps 
	m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive"	, "d0 vs phi vs eta 1GeV-2GeV positive; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);
	m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative"	, "d0 vs phi vs eta 1GeV-2GeV negative; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative);

	//TH2D vs phi
	m_trkd0_wrtPV_vs_phi_1GeV_2GeV_positive	= new TH2F("trk_d0_wrtPV_vs_phi_1GeV_2GeV_positive"		, "d0 vs phi 1GeV-2GeV positive; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_phi_1GeV_2GeV_negative	= new TH2F("trk_d0_wrtPV_vs_phi_1GeV_2GeV_negative"		, "d0 vs phi 1GeV-2GeV negative; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_1GeV_2GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_1GeV_2GeV_negative	);

    	//TH2D vs eta
	m_trkd0_wrtPV_vs_eta_1GeV_2GeV_positive	= new TH2F("trk_d0_wrtPV_vs_eta_1GeV_2GeV_positive"		, "d0 vs eta 1GeV-2GeV positive; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_eta_1GeV_2GeV_negative	= new TH2F("trk_d0_wrtPV_vs_eta_1GeV_2GeV_negative"		, "d0 vs eta 1GeV-2GeV negative; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_1GeV_2GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_1GeV_2GeV_negative	);

	/////////////////////////////////////////////////
    	/////Define+register histo's 2GeV until 5GeV/////
    	/////////////////////////////////////////////////
	//TH3D for maps 
	m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive"	, "d0 vs phi vs eta 2GeV-5GeV positive; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);
	m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative"	, "d0 vs phi vs eta 2GeV-5GeV negative; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative);

	//TH2D vs phi
	m_trkd0_wrtPV_vs_phi_2GeV_5GeV_positive	= new TH2F("trk_d0_wrtPV_vs_phi_2GeV_5GeV_positive"		, "d0 vs phi 2GeV-5GeV positive; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_phi_2GeV_5GeV_negative	= new TH2F("trk_d0_wrtPV_vs_phi_2GeV_5GeV_negative"		, "d0 vs phi 2GeV-5GeV negative; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_2GeV_5GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_2GeV_5GeV_negative	);

	//TH2D vs eta
	m_trkd0_wrtPV_vs_eta_2GeV_5GeV_positive	= new TH2F("trk_d0_wrtPV_vs_eta_2GeV_5GeV_positive"		, "d0 vs eta 2GeV-5GeV positive; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_eta_2GeV_5GeV_negative	= new TH2F("trk_d0_wrtPV_vs_eta_2GeV_5GeV_negative"		, "d0 vs eta 2GeV-5GeV negative; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_2GeV_5GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_2GeV_5GeV_negative	);

	/////////////////////////////////////////////////
    	/////Define+register histo's 5GeV until 10GeV////
    	/////////////////////////////////////////////////
    	//TH3D for maps 
	m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive"	, "d0 vs phi vs eta 5GeV-10GeV positive; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);
	m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative"	, "d0 vs phi vs eta 5GeV-10GeV negative; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative);

	//TH2D vs phi
	m_trkd0_wrtPV_vs_phi_5GeV_10GeV_positive	= new TH2F("trk_d0_wrtPV_vs_phi_5GeV_10GeV_positive"		, "d0 vs phi 5GeV-10GeV positive; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_phi_5GeV_10GeV_negative	= new TH2F("trk_d0_wrtPV_vs_phi_5GeV_10GeV_negative"		, "d0 vs phi 5GeV-10GeV negative; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_5GeV_10GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_5GeV_10GeV_negative	);

	//TH2D vs eta
	m_trkd0_wrtPV_vs_eta_5GeV_10GeV_positive	= new TH2F("trk_d0_wrtPV_vs_eta_5GeV_10GeV_positive"		, "d0 vs eta 5GeV-10GeV positive; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_eta_5GeV_10GeV_negative	= new TH2F("trk_d0_wrtPV_vs_eta_5GeV_10GeV_negative"		, "d0 vs eta 5GeV-10GeV negative; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );
	
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_5GeV_10GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_5GeV_10GeV_negative	);
	
	/////////////////////////////////////////////////
    	////Define+register histo's larger than 10GeV////
    	/////////////////////////////////////////////////
    	//TH3D for maps 
	m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_positive 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_10GeV_positive"	, "d0 vs phi vs eta >10GeV positive; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);
	m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_negative 	= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_10GeV_negative"	, "d0 vs phi vs eta >10GeV negative; #phi; #eta; d0 [mm]", nphiBinsMap, -maxPhi, maxPhi, netaBinsMap, -maxEta, maxEta, nd0Bins, -maxD0, maxD0);

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_positive);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_negative);

    	//TH2D vs phi
	m_trkd0_wrtPV_vs_phi_10GeV_positive	= new TH2F("trk_d0_wrtPV_vs_phi_10GeV_positive"		, "d0 vs phi >10GeV positive; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_phi_10GeV_negative	= new TH2F("trk_d0_wrtPV_vs_phi_10GeV_negative"		, "d0 vs phi >10GeV negative; #phi; d0 [mm]"		, nphiBins, -maxPhi, maxPhi, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_10GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_phi_10GeV_negative	);

	//TH2D vs eta
	m_trkd0_wrtPV_vs_eta_10GeV_positive	= new TH2F("trk_d0_wrtPV_vs_eta_10GeV_positive"		, "d0 vs eta >10GeV positive; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );
	m_trkd0_wrtPV_vs_eta_10GeV_negative	= new TH2F("trk_d0_wrtPV_vs_eta_10GeV_negative"		, "d0 vs eta >10GeV negative; #eta; d0 [mm]"		, netaBins, -maxEta, maxEta, nd0Bins, -maxD0, maxD0 );

	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_10GeV_positive	);
	RegisterHisto(al_mon, m_trkd0_wrtPV_vs_eta_10GeV_negative	);
	
/*
    /////////////////////////////////////////////////
    //Histo's from IDAMonGenericTracks.cxx///////////
    /////////////////////////////////////////////////
      const float m_Pi = 3.14156;
  
      m_d0_pt         = new TH2F("d0_pt"       , "d0 vs pt"           , nD0Bins,d0bin,NpTbins,ptBin);

      m_d0_pt = new TProfile("prof", "d0 vs pT;pT (GeV);d0 (mm)", NpTbins, ptBin);
      m_trk_d0_wrtPV_vs_phi_vs_eta         = new TH3F("trk_d0_wrtPV_vs_phi_vs_eta"       , "d0 vs phi vs eta"           , 100, -3., 3.,  40, 0, 2*m_Pi,  100, -0.5, 0.5 );
      m_trk_d0_wrtPV_vs_phi_vs_eta_barrel  = new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_barrel", "d0 vs phi vs eta (Barrel)"  , 100, -3., 3.,  40, 0, 2*m_Pi,  100, -0.5, 0.5 );
      m_trk_d0_wrtPV_vs_phi_vs_eta_ecc     = new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_ecc"   , "d0 vs phi vs eta (Endcap C)", 100, -3., 3.,  40, 0, 2*m_Pi,  100, -0.5, 0.5 );
      m_trk_d0_wrtPV_vs_phi_vs_eta_eca     = new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_eca"   , "d0 vs phi vs eta (Endcap A)", 100, -3., 3.,  40, 0, 2*m_Pi,  100, -0.5, 0.5 );
  
      m_trk_z0_wrtPV_vs_phi_vs_eta         = new TH3F("trk_z0_wrtPV_vs_phi_vs_eta"       , "d0 vs phi vs eta"           , 100, -3., 3.,  40, 0, 2*m_Pi,  100, -1, 1 );
      m_trk_z0_wrtPV_vs_phi_vs_eta_barrel  = new TH3F("trk_z0_wrtPV_vs_phi_vs_eta_barrel", "d0 vs phi vs eta (Barrel)"  , 100, -3., 3.,  40, 0, 2*m_Pi,  100, -1, 1 );
      m_trk_z0_wrtPV_vs_phi_vs_eta_ecc     = new TH3F("trk_z0_wrtPV_vs_phi_vs_eta_ecc"   , "d0 vs phi vs eta (Endcap C)", 100, -3., 3.,  40, 0, 2*m_Pi,  100, -1, 1 );
      m_trk_z0_wrtPV_vs_phi_vs_eta_eca     = new TH3F("trk_z0_wrtPV_vs_phi_vs_eta_eca"   , "d0 vs phi vs eta (Endcap A)", 100, -3., 3.,  40, 0, 2*m_Pi,  100, -1, 1 );

      RegisterHisto(al_mon, m_d0_pt );

      RegisterHisto(al_mon, m_trk_d0_wrtPV_vs_phi_vs_eta        );
      RegisterHisto(al_mon, m_trk_d0_wrtPV_vs_phi_vs_eta_barrel );
      RegisterHisto(al_mon, m_trk_d0_wrtPV_vs_phi_vs_eta_ecc    );
      RegisterHisto(al_mon, m_trk_d0_wrtPV_vs_phi_vs_eta_eca    );
      
      RegisterHisto(al_mon, m_trk_z0_wrtPV_vs_phi_vs_eta        );
      RegisterHisto(al_mon, m_trk_z0_wrtPV_vs_phi_vs_eta_barrel );
      RegisterHisto(al_mon, m_trk_z0_wrtPV_vs_phi_vs_eta_ecc    );
      RegisterHisto(al_mon, m_trk_z0_wrtPV_vs_phi_vs_eta_eca    );
    */
	

    m_histosBooked++;

   } 
    return StatusCode::SUCCESS;
}

/*void IDAlignMonPVBiases::RegisterHisto(MonGroup& mon, TH1F_LW* histo) {
  
  //histo->Sumw2(); this uses a lot of memory and isn't needed!
  //histo->SetOption("e");
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH1F_LW Histogram:" << endmsg;
  }
}*/


void IDAlignMonPVBiases::RegisterHisto(MonGroup& mon, TH1* histo) {

  //histo->Sumw2();
  histo->SetOption("e");
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH1 Histogram:" << endmsg;
  }
}

void IDAlignMonPVBiases::RegisterHisto(MonGroup& mon, TProfile* histo) {

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TProfile Histogram:" << endmsg;
  }

}

void IDAlignMonPVBiases::RegisterHisto(MonGroup& mon, TH2* histo) {
  
  //histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH2 Histogram:" << endmsg;
  }
}

void IDAlignMonPVBiases::RegisterHisto(MonGroup& mon, TProfile2D* histo) {

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TProfile2D Histogram:" << endmsg;
  }

}

StatusCode IDAlignMonPVBiases::fillHistograms()
{
  m_events++;

  /******************************************************************
  ** Event Information
  *******************************************************************/
  ATH_MSG_DEBUG("Retrieving event info.");
  const xAOD::EventInfo * eventInfo;
  if (evtStore()->retrieve(eventInfo).isFailure())
    ATH_MSG_ERROR("Could not retrieve event info.");
  else
  {
    m_runNumber = eventInfo->runNumber();
    m_evtNumber = eventInfo->eventNumber();
    m_lumi_block = eventInfo->lumiBlock();
    ATH_MSG_DEBUG(" Execute() starting on --> Run: " << m_runNumber << "  event: " << m_evtNumber);
  }

  /******************************************************************
  ** Retrieve Trackparticles
  ******************************************************************/
  const xAOD::TrackParticleContainer* tracks = 0;
  if( ! evtStore()->retrieve( tracks, "InDetTrackParticles").isSuccess() ){
	msg(MSG::WARNING) << "Failed to retrieve event info collection. Exiting." << endmsg;
	return StatusCode::FAILURE;
  }

  /**
   * Retrieve Vertices
   */

  SG::ReadHandle<xAOD::VertexContainer> vertices { m_vertexKey };
  if (!vertices.isValid()) {
    ATH_MSG_WARNING( "Failed to retrieve vertex container with key " << m_vertexKey.key() );
    return StatusCode::FAILURE;
  }

  xAOD::TrackParticleContainer::const_iterator track_itr = tracks->begin();
  xAOD::TrackParticleContainer::const_iterator track_itrE = tracks->end();
	
  /******************************************************************
  ** Trackparticle Loop
  *******************************************************************/
  for (; track_itr!=track_itrE; ++track_itr) {
    const xAOD::Vertex* foundVertex = nullptr;
    for (const auto& vx : *vertices)
    {
      for (const auto& tpLink : vx->trackParticleLinks())
      {
	if (*tpLink == *track_itr)
	{
	  foundVertex = vx;
	  break;
	}
	if (foundVertex) break;
      }
    }       
       	// require having vertex
	if(!foundVertex) continue;
        // require associated with primary vertex
   	if(foundVertex->vertexType() != 1) continue;
	// require at least 10 tracks associated
        if(foundVertex->nTrackParticles() < 10) continue;

        const Trk::ImpactParametersAndSigma* myIPandSigma(NULL);
        myIPandSigma = m_trackToVertexIPEstimator->estimate(*track_itr, foundVertex, true);

        // require d0_pv to be smaller than 4
        if(myIPandSigma->IPd0 > 4.0) continue;
        if(myIPandSigma->IPd0 < -4.0) continue;
        
	/******************************************************************
  	** Fill TTree
  	*******************************************************************/
	m_d0=myIPandSigma->IPd0;
        m_z0=myIPandSigma->IPz0;
        m_z0_err = myIPandSigma->sigmaz0;
    	m_d0_err= myIPandSigma->sigmad0;

        m_charge = (*track_itr)->charge();
 	m_pt = (*track_itr)->pt();
        m_eta = (*track_itr)->eta();
        m_phi = (*track_itr)->phi();
        m_vertex_x = foundVertex->x();
        m_vertex_y = foundVertex->y();
        m_vertex_z = foundVertex->z();
  
	//m_Tree->Fill();

 	/******************************************************************
  	** Fill Histograms
  	*******************************************************************/
        double pt = (*track_itr)->pt()*0.001;
	
	if(pt>0.4&&pt<0.6) {
		//Fill TH3D for positive and negative vs phi vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_positive->Fill(m_phi,m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_vs_eta_400MeV_600MeV_negative->Fill(m_phi,m_eta,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs phi
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_400MeV_600MeV_positive->Fill(m_phi,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_400MeV_600MeV_negative->Fill(m_phi,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_eta_400MeV_600MeV_positive->Fill(m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_eta_400MeV_600MeV_negative->Fill(m_eta,myIPandSigma->IPd0);
	}

      	if(pt>0.6&&pt<1) {
		//Fill TH3D for positive and negative vs phi vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_positive->Fill(m_phi,m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_vs_eta_600MeV_1GeV_negative->Fill(m_phi,m_eta,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs phi
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_600MeV_1GeV_positive->Fill(m_phi,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_600MeV_1GeV_negative->Fill(m_phi,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_eta_600MeV_1GeV_positive->Fill(m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_eta_600MeV_1GeV_negative->Fill(m_eta,myIPandSigma->IPd0);
	}

      	if(pt>1&&pt<2) {
		//Fill TH3D for positive and negative vs phi vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_positive->Fill(m_phi,m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_vs_eta_1GeV_2GeV_negative->Fill(m_phi,m_eta,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs phi
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_1GeV_2GeV_positive->Fill(m_phi,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_1GeV_2GeV_negative->Fill(m_phi,myIPandSigma->IPd0);
	
		//Fill TH2D for positive and negative vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_eta_1GeV_2GeV_positive->Fill(m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_eta_1GeV_2GeV_negative->Fill(m_eta,myIPandSigma->IPd0);
	}

      	if(pt>2&&pt<5) {
		//Fill TH3D for positive and negative vs phi vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_positive->Fill(m_phi,m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_vs_eta_2GeV_5GeV_negative->Fill(m_phi,m_eta,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs phi
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_2GeV_5GeV_positive->Fill(m_phi,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_2GeV_5GeV_negative->Fill(m_phi,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_eta_2GeV_5GeV_positive->Fill(m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_eta_2GeV_5GeV_negative->Fill(m_eta,myIPandSigma->IPd0);
	}

      	if(pt>5&&pt<10) {
		//Fill TH3D for positive and negative vs phi vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_positive->Fill(m_phi,m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_vs_eta_5GeV_10GeV_negative->Fill(m_phi,m_eta,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs phi
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_5GeV_10GeV_positive->Fill(m_phi,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_5GeV_10GeV_negative->Fill(m_phi,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_eta_5GeV_10GeV_positive->Fill(m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_eta_5GeV_10GeV_negative->Fill(m_eta,myIPandSigma->IPd0);
	}

      	if(pt>10) {
		//Fill TH3D for positive and negative vs phi vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_positive->Fill(m_phi,m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_vs_eta_10GeV_negative->Fill(m_phi,m_eta,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs phi
		if(m_charge==1) m_trkd0_wrtPV_vs_phi_10GeV_positive->Fill(m_phi,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_phi_10GeV_negative->Fill(m_phi,myIPandSigma->IPd0);

		//Fill TH2D for positive and negative vs eta
		if(m_charge==1) m_trkd0_wrtPV_vs_eta_10GeV_positive->Fill(m_eta,myIPandSigma->IPd0);
		if(m_charge==-1) m_trkd0_wrtPV_vs_eta_10GeV_negative->Fill(m_eta,myIPandSigma->IPd0);
	}

	/******************************************************************
  	** Divide in barrel, eca, ecc
  	*******************************************************************/
    	/*if((*track_itr)->track()) {
        	
		int nhpixB=0, nhpixECA=0, nhpixECC=0, nhsctB=0, nhsctECA=0, nhsctECC=0, nhtrtB=0, nhtrtECA=0, nhtrtECC=0;
    		// loop over all hits on track
    	
        	const Trk::Track* track_itr2 = (*track_itr)->track();
		const DataVector<const Trk::TrackStateOnSurface>* TSOS;

    		TSOS = track_itr2->trackStateOnSurfaces();
	
		DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItr  = TSOS->begin();
    		DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItrE = TSOS->end();

    		if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"starting to loop over TSOS"<<endmsg;
 	
    		for (; TSOSItr != TSOSItrE; ++TSOSItr) {
	
      			//check that we have track parameters defined for the surface (pointer is not null)
      			if(!((*TSOSItr)->trackParameters())) {
        			if (msgLvl(MSG::DEBUG)) msg() << "hit skipped because no associated track parameters" << endmsg;
        			continue;
      			}
      
      			Identifier surfaceID;
      			const Trk::MeasurementBase* mesb=(*TSOSItr)->measurementOnTrack();
      			if (mesb != 0 && mesb->associatedSurface().associatedDetectorElement()!=NULL) {
				surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
			}else{ 
      				// hits, outliers
				continue;
			}

      			if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) ){
   
        			// --- pixel
        			if (m_idHelper->is_pixel(surfaceID)){
          				if(m_pixelID->barrel_ec(surfaceID)      ==  0){
            					nhpixB++;
          				}
          				else if(m_pixelID->barrel_ec(surfaceID) ==  2)  nhpixECA++;
          				else if(m_pixelID->barrel_ec(surfaceID) == -2) nhpixECC++;
        			}
        			// --- sct
        			else if (m_idHelper->is_sct(surfaceID)){
          				if(m_sctID->barrel_ec(surfaceID)      ==  0){
            					nhsctB++;
          				}
          				else if(m_sctID->barrel_ec(surfaceID) ==  2) nhsctECA++;
          				else if(m_sctID->barrel_ec(surfaceID) == -2) nhsctECC++;
        			}
        			// --- trt
        			if (m_idHelper->is_trt(surfaceID)){
          				int m_barrel_ec      = m_trtID->barrel_ec(surfaceID);
          				if(m_barrel_ec == 1 || m_barrel_ec == -1 ) {
            					nhtrtB++;
          				}
          				else if(m_barrel_ec ==  2){
            					nhtrtECA++;
          				}else if(m_barrel_ec == -2){
            					nhtrtECC++;
          				}
        			}	
      			}
    		}
 		
		int nhpix=nhpixB+nhpixECA+nhpixECC;
    		int nhsct=nhsctB+nhsctECA+nhsctECC;
    		int nhtrt=nhtrtB+nhtrtECA+nhtrtECC;
    		int nhits=nhpix+nhsct+nhtrt;

        	// set EC or not
		bool hasECAhits = false;
        	if(nhpixECA+nhsctECA+nhtrtECA > 0) hasECAhits = true;
        	bool hasECChits = false;
        	if(nhpixECC+nhsctECC+nhtrtECC > 0) hasECChits = true;

        	if (!hasECAhits && !hasECChits) { //filling barrel histograms
        		m_trk_d0_wrtPV_vs_phi_vs_eta_barrel->Fill(m_eta, m_phi, myIPandSigma->IPd0);
        		m_trk_z0_wrtPV_vs_phi_vs_eta_barrel->Fill(m_eta, m_phi, myIPandSigma->IPz0);
		}else if (hasECAhits) {//filling endcap A histograms
        		m_trk_d0_wrtPV_vs_phi_vs_eta_eca -> Fill(m_eta, m_phi, myIPandSigma->IPd0);
        		m_trk_z0_wrtPV_vs_phi_vs_eta_eca -> Fill(m_eta, m_phi, myIPandSigma->IPz0);
 		}else if (hasECChits) {//filling endcap C histograms
        		m_trk_d0_wrtPV_vs_phi_vs_eta_ecc -> Fill(m_eta, m_phi, myIPandSigma->IPd0);
        		m_trk_z0_wrtPV_vs_phi_vs_eta_ecc -> Fill(m_eta, m_phi, myIPandSigma->IPz0);
 		}
	}*/
   	
 } // End of track selection loop

 return StatusCode::SUCCESS;
}

StatusCode IDAlignMonPVBiases::procHistograms()
{
  //if( endOfLowStatFlag() ) {  }
  //if( endOfLumiBlockFlag() ) {  }
  //if( endOfRunFlag() ) {  }
 
  return StatusCode::SUCCESS;
}

StatusCode IDAlignMonPVBiases::finalize()
{
  return StatusCode::SUCCESS; 
}
