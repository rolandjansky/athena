/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTPRDValAlg.h"

#include "xAODEventInfo/EventInfo.h"
 
#include "GaudiKernel/ITHistSvc.h"

#include "Identifier/Identifier.h"

#include "HepMC/GenParticle.h"
#include "TrackRecord/TrackRecordCollection.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h" 
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonSimEvent/MdtHitIdHelper.h"

#include "GeneratorObjects/McEventCollection.h"

#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/MDTSimHit.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/MuonSimDataCollection.h"


#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkGeometry/MagneticFieldProperties.h"

#include "StoreGate/StoreGateSvc.h"

#include "TTree.h"
#include <TString.h> // for Form
#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;
using namespace MuonGM;


MDTPRDValAlg::MDTPRDValAlg(const std::string& name, 
			     ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  mdttree(0),
  event_counter(0),
  histo_flag(false),
  descriptor(0),
  m_pMuonMgr(0),
  m_mdtIdHelper(0),
  m_log(0),
  m_debug(false),
  m_verbose(false),
  m_sgSvc(0),

  counter_ValHitNumber(-99),
  m_Validation_MDT_Type(-99),
  m_Validation_MDT_NumberOfHits(-99),
  m_Validation_MDT_EventNumber(-99),
  m_Validation_MDT_RunNumber(-99),
  m_Validation_MDT_pdg(0),
  m_Validation_MDT_phi(-99999.),
  m_Validation_MDT_theta(-99999.),
  m_Validation_MDT_eta(-99999.),
  m_Validation_MDT_qp(-99999.),
  m_Validation_MDT_MEX(-99999.),
  m_Validation_MDT_MEY(-99999.),
  m_Validation_MDT_MEZ(-99999.),
  m_Validation_MDT_Exit_qp(-99999.),
  m_Validation_MDT_ExitX(-99999.),
  m_Validation_MDT_ExitY(-99999.),
  m_Validation_MDT_ExitZ(-99999.),
  m_Validation_MDT_HitX(-99999.),
  m_Validation_MDT_HitY(-99999.),
  m_Validation_MDT_HitZ(-99999.),
  m_Validation_MDT_HitR(-99999.),
  m_Validation_MDT_ExX(-99999.),
  m_Validation_MDT_ExY(-99999.),
  m_Validation_MDT_ExZ(-99999.),
  m_Validation_MDT_ExR(-99999.),
  m_Validation_MDT_StationEta(-99),
  m_Validation_MDT_StationPhi(-99),
  m_Validation_MDT_IDTube(-99),
  m_Validation_MDT_IDMultiLayer(-99),
  m_Validation_MDT_IDLayer(-99),
  m_Validation_MDT_GeoSign(-99),
  m_Validation_MDT_BESL(-99),

  m_Validation_MDT_LocalX(-99999.),
  m_Validation_MDT_LocalY(-99999.),
  m_Validation_MDT_LocalZ(-99999.),
  m_Validation_MDT_LocalR(-99999.),
  m_Validation_MDT_Res_LocalX(-99999.),
  m_Validation_MDT_Res_LocalY(-99999.),
  m_Validation_MDT_ResEloss_LocalX(-99999.),
  m_Validation_MDT_ResEloss_LocalY(-99999.),
  m_Validation_MDT_dtheta_dLocX(-99999.),
  m_Validation_MDT_dtheta_dLocY(-99999.),
  m_Validation_MDT_dphi_dLocX(-99999.),
  m_Validation_MDT_dphi_dLocY(-99999.),
  m_Validation_MDT_dtheta(-99999.),
  m_Validation_MDT_3DdistME(-99999.),
  m_Validation_MDT_ploss(-99999.),
  m_Validation_MDT_3DdistExit(-99999.),
  m_Validation_MDT_last(-99),

  m_Validation_MDT_ExNomWireR(-99999.),
  m_Validation_MDT_ExNomWireZ(-99999.),
  m_Validation_MDT_ExSagWireR(-99999.),
  m_Validation_MDT_ExSagWireZ(-99999.),
  m_Validation_MDT_ExSagRotR(-99999.),
  m_Validation_MDT_ExSagRotZ(-99999.),
  m_Validation_MDT_SimRadius(-99999.),
  m_Validation_MDT_SdoRadius(-99999.),
  m_Validation_MDT_RotRadius(-99999.),
  m_Validation_MDT_WireLen(-99999.),

  m_rootsvc(0),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_rotCreator("Muon::MdtDriftCircleOnTrackCreator/SaggingMdtDriftCircleOnTrackCreator")
{
  // Set users' request
  /**Properties to be modified through the jobOption file
     Short description:
     Key for accessing MC truth
     Option to write out ntuple
     AANtuple file (linked to a specific file name in the jobOption)
     AANtuple directory name for MDT variables
     AANtuple tree name for MDT variables
     If true, enter the main algorithm loop
     Eta cut, serves for rejecting endcap hits ONLY when plotting MDT barrel cross section (gives cleaner plot)
     ChamberName,StationSize,StationEta/Phi are used to define a specific chamber/se of chambers to be plotted in
     the histo file  
  */
  declareProperty("McEventKey",           m_key="TruthEvent");
  declareProperty("WriteNtuple",          m_writeNtuple = true);
  declareProperty("NtupleFileName",       m_ntupleFileName = "MuonHits");
  declareProperty("NtupleDirectoryName",  m_ntupleDirName = "MDTHitsValidation");
  declareProperty("NtupleTreeName",       m_ntupleTreeName = "MDTTestHits");
  declareProperty("DoMDTTest",            m_DoMDTTest=true);
  declareProperty("BarrelEtaCut",         m_BarrelEtaCut=1.);
  declareProperty("ChamberName",	  m_chamberName="XXX");	
  declareProperty("StationSize",m_StationSize="XXX");
  declareProperty("StationEta",m_StationEta=-100);
  declareProperty("StationPhi",m_StationPhi=-100);  
  declareProperty("LastEvent",m_LastEvent=1);  
  declareProperty("Extrapolator",m_extrapolator);
  declareProperty("MDTRotCreator",m_rotCreator);
  declareProperty("UseEntryRecord",m_useEntryLayer = true );
  
}

MDTPRDValAlg::~MDTPRDValAlg()
{
}

StatusCode MDTPRDValAlg::initialize()
{
  m_log     = new MsgStream( msgSvc(), name());
  m_debug   = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;
  *m_log << MSG::INFO << " initializing MDTPRDValAlg " << endreq;
 
  /**Locate the StoreGateSvc and initialize our local ptr
     intitialize transient event store
  */
  StatusCode sc = service("StoreGateSvc", m_sgSvc);

  if (!sc.isSuccess() || 0 == m_sgSvc) {
    *m_log << MSG::ERROR << "MDTPRDValAlg: Could not find StoreGateSvc" << endreq;
    return StatusCode::FAILURE;
  }
  else { 
    *m_log << MSG::DEBUG << "Retrieved StoreGateSvc" << endreq;
  } 
      
  StoreGateSvc* detStore;
  sc = serviceLocator()->service( "DetectorStore", detStore );
  if (sc.isFailure())   {
    *m_log << MSG::ERROR << "Cannot locate the DetectorStore" << endreq; 
    return sc;
  }
 
  sc = detStore->retrieve( m_pMuonMgr ); 
  if (sc.isFailure()){
    *m_log << MSG::ERROR << "Cannot retrieve MuonDetectorManager" << endreq;
    return sc;
  }

  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure()){
    *m_log << MSG::ERROR << "Cannot retrieve m_mdtId" << endreq;
    return sc;
  }  

	
  /// ROOT AANtuple Initialization---------------------------------------

    if (m_writeNtuple) {

      std::string mdtStreamAndPath = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
      std::string mdttreeName = m_ntupleTreeName;

      std::string mdttreePath = mdtStreamAndPath+"/"+mdttreeName;

      mdttree = new TTree(TString(mdttreeName), "Muon MDT Hits output");
      
      StatusCode status;
      ITHistSvc* hSvc=0;
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      sc = svcLocator->service("THistSvc", hSvc);
      if (sc.isFailure()) {
	*m_log << MSG::WARNING << " could not get THistSvc " << endreq;
	return sc;
      }
      
      status=ToolRootHistSvc()->regTree(mdttreePath, mdttree);

      if(status.isFailure()) {
	*m_log << MSG::DEBUG << "MDTPRDValAlg:: Unable to register TTreeTuple : " << mdttreePath << endreq;
	return status;   
      }
      
      *m_log << MSG::INFO << " Creating new ntuple " << mdttreePath << endreq;
  
      //-----------------------------------------------//
      /**add items-variables to the validation AANTUPLE
	 Short description:
	 Total number of G4 hits in AANTUPLE
	 Total number of events processed
	 Type 1=Hits , 2=Digits , 3=Prepraw
	 Run number, event specific ID
	 Global X, Y, Z, R of hit (with respect to the interaction point)
	 MDT station name (char or int), conventional ATLAS naming scheme. Range: [BIL, BIR, BIS, BEE, BML, BMS, BMF, BOL, BOS, BOF, BOG, BOH, EIL, EIS, EES, EEL, EML, EMS, EOL, EOS]
	 MDT station eta, increases with Z in the barrel region (|n|<1) and with R in the endcap regions (|n|>1). Range: [-8,8] ([-8,8] for the barrel, [-6,-1] for the backward endcap with -1
	 closest to the beam, [1,6] for the forward endcap with 1 closest to the beam)
	 MDT station phi, starts from sector 1 and increases with phi. Range: [1,8] (each phi unit incorporates two MuonSpectrometer barrel sectors)
	 MDT Multilayer, a stack of two or three tube layers, two multilayer per MDT chamber. Range: [1,2] (In barrel increases with R, in endcaps increases with |Z|)
	 MDT Tube Layer, a stack of tubes within a multilayer, four layers per multilayer for inner stations and three per multilayer for middle and outer stations. Range: [1,4] (In barrel
	 increases with R, in endcaps increases |Z|)
	 MDT Tube, a single MDT tube. Range: [1,72] (In barrel increases with |Z|, in endcaps increases with R)
	 Local X, Y, Z is the respective coordinate of the hit with reference point the middle of the wire, R the drift radius of the hit.
      */

      mdttree->Branch("MDT_NumberOfHits",  &m_Validation_MDT_NumberOfHits, "MDT_NumberOfHits/I");
      mdttree->Branch("MDT_EventNumber",  &m_Validation_MDT_EventNumber, "MDT_EventNumber/I");
      mdttree->Branch("MDT_Type",  &m_Validation_MDT_Type, "MDT_type/I");
      mdttree->Branch("MDT_RunNumber",  &m_Validation_MDT_RunNumber, "MDT_RunNumber/I");
      mdttree->Branch("MDT_pdg",  &m_Validation_MDT_pdg, "MDT_pdg/I");

      mdttree->Branch("MDT_phi",  &m_Validation_MDT_phi, "MDT_phi/D");
      mdttree->Branch("MDT_theta",  &m_Validation_MDT_theta, "MDT_theta/D");
      mdttree->Branch("MDT_eta",  &m_Validation_MDT_eta, "MDT_eta/D");
      mdttree->Branch("MDT_qp",  &m_Validation_MDT_qp, "MDT_qp/D");
      mdttree->Branch("MDT_MEX",  &m_Validation_MDT_MEX, "MDT_MEX/D");
      mdttree->Branch("MDT_MEY",  &m_Validation_MDT_MEY, "MDT_MEY/D");
      mdttree->Branch("MDT_MEZ",  &m_Validation_MDT_MEZ, "MDT_MEZ/D");
      mdttree->Branch("MDT_Exit_qp",  &m_Validation_MDT_Exit_qp, "MDT_Exit_qp/D");
      mdttree->Branch("MDT_ExitX",  &m_Validation_MDT_ExitX, "MDT_ExitX/D");
      mdttree->Branch("MDT_ExitY",  &m_Validation_MDT_ExitY, "MDT_ExitY/D");
      mdttree->Branch("MDT_ExitZ",  &m_Validation_MDT_ExitZ, "MDT_ExitZ/D");

      mdttree->Branch("MDT_HitX",  &m_Validation_MDT_HitX, "MDT_HitX/D");
      mdttree->Branch("MDT_HitY",  &m_Validation_MDT_HitY, "MDT_HitY/D");
      mdttree->Branch("MDT_HitZ",  &m_Validation_MDT_HitZ, "MDT_HitZ/D");
      mdttree->Branch("MDT_HitR",  &m_Validation_MDT_HitR, "MDT_HitR/D");

      mdttree->Branch("MDT_ExX",  &m_Validation_MDT_ExX, "MDT_ExX/D");
      mdttree->Branch("MDT_ExY",  &m_Validation_MDT_ExY, "MDT_ExY/D");
      mdttree->Branch("MDT_ExZ",  &m_Validation_MDT_ExZ, "MDT_ExZ/D");
      mdttree->Branch("MDT_ExR",  &m_Validation_MDT_ExR, "MDT_ExR/D");

      mdttree->Branch("MDT_StationName",  m_Validation_MDT_StationName, "MDT_StationName/C");
      mdttree->Branch("MDT_StationEta",  &m_Validation_MDT_StationEta, "MDT_StationEta/I");
      mdttree->Branch("MDT_StationPhi",  &m_Validation_MDT_StationPhi, "MDT_StationPhi/I");		
      mdttree->Branch("MDT_IDMultiLayer",  &m_Validation_MDT_IDMultiLayer, "MDT_IDMultiLayer/I");
      mdttree->Branch("MDT_IDLayer",  &m_Validation_MDT_IDLayer, "MDT_IDLayer/I");   
      mdttree->Branch("MDT_IDTube",  &m_Validation_MDT_IDTube, "MDT_IDTube/I");
      mdttree->Branch("MDT_GeoSign",  &m_Validation_MDT_GeoSign, "MDT_GeoSign/I");
      mdttree->Branch("MDT_BESL",  &m_Validation_MDT_BESL, "MDT_BESL/I");
	
      mdttree->Branch("MDT_LocalX",  &m_Validation_MDT_LocalX, "MDT_LocalX/D");
      mdttree->Branch("MDT_LocalY",  &m_Validation_MDT_LocalY, "MDT_LocalY/D");
      mdttree->Branch("MDT_LocalZ",  &m_Validation_MDT_LocalZ, "MDT_LocalZ/D");  
      mdttree->Branch("MDT_LocalR",  &m_Validation_MDT_LocalR, "MDT_LocalR/D");
      mdttree->Branch("MDT_Res_LocalX",  &m_Validation_MDT_Res_LocalX, "MDT_Res_LocalX/D");
      mdttree->Branch("MDT_Res_LocalY",  &m_Validation_MDT_Res_LocalY, "MDT_Res_LocalY/D");
      mdttree->Branch("MDT_ResEloss_LocalX",  &m_Validation_MDT_ResEloss_LocalX, "MDT_ResEloss_LocalX/D");
      mdttree->Branch("MDT_ResEloss_LocalY",  &m_Validation_MDT_ResEloss_LocalY, "MDT_ResEloss_LocalY/D");
      mdttree->Branch("MDT_dtheta_dLocX",  &m_Validation_MDT_dtheta_dLocX, "MDT_dtheta_dLocX/D");
      mdttree->Branch("MDT_dtheta_dLocY",  &m_Validation_MDT_dtheta_dLocY, "MDT_dtheta_dLocY/D");
      mdttree->Branch("MDT_dphi_dLocX",  &m_Validation_MDT_dphi_dLocX, "MDT_dphi_dLocX/D");
      mdttree->Branch("MDT_dphi_dLocY",  &m_Validation_MDT_dphi_dLocY, "MDT_dphi_dLocY/D");
      mdttree->Branch("MDT_dtheta",  &m_Validation_MDT_dtheta, "MDT_dtheta/D");
      mdttree->Branch("MDT_3DdistME",  &m_Validation_MDT_3DdistME, "MDT_3DdistME/D");
      mdttree->Branch("MDT_ploss",  &m_Validation_MDT_ploss, "MDT_ploss/D");
      mdttree->Branch("MDT_3DdistExit",  &m_Validation_MDT_3DdistExit, "MDT_3DdistExit/D");
      mdttree->Branch("MDT_last",  &m_Validation_MDT_last, "MDT_last/I");
 
      mdttree->Branch("MDT_ExNomWireR",  &m_Validation_MDT_ExNomWireR, "MDT_ExNomWireR/D");
      mdttree->Branch("MDT_ExNomWireZ",  &m_Validation_MDT_ExNomWireZ, "MDT_ExNomWireZ/D");
      mdttree->Branch("MDT_ExSagWireR",  &m_Validation_MDT_ExSagWireR,  "MDT_ExSagWireR/D");
      mdttree->Branch("MDT_ExSagWireZ",  &m_Validation_MDT_ExSagWireZ,  "MDT_ExSagWireZ/D");
      mdttree->Branch("MDT_ExSagRotR",   &m_Validation_MDT_ExSagRotR,  "MDT_ExSagRotR/D");
      mdttree->Branch("MDT_ExSagRotZ",   &m_Validation_MDT_ExSagRotZ,  "MDT_ExSagRotZ/D");
      mdttree->Branch("MDT_SimRadius",   &m_Validation_MDT_SimRadius,  "MDT_SimRadius/D");
      mdttree->Branch("MDT_SdoRadius",   &m_Validation_MDT_SdoRadius,  "MDT_SdoRadius/D");
      mdttree->Branch("MDT_RotRadius",   &m_Validation_MDT_RotRadius,  "MDT_RotRadius/D");
      mdttree->Branch("MDT_WireLen",     &m_Validation_MDT_WireLen,    "MDT_WireLen/D");



    }

    sc = m_extrapolator.retrieve();
    if( sc.isSuccess() ){
      *m_log<<MSG::INFO << "Retrieved " << m_extrapolator << endreq;
    }else{
      *m_log<<MSG::FATAL<<"Could not get " << m_extrapolator <<endreq; 
      return sc;
    }
    sc = m_rotCreator.retrieve();
    if( sc.isSuccess() ){
      *m_log<<MSG::INFO << "Retrieved " << m_rotCreator << endreq;
    }else{
      *m_log<<MSG::FATAL<<"Could not get " << m_rotCreator <<endreq; 
      return sc;
    }

    event_counter=0;
  
    *m_log << MSG::INFO << "MDTPRDValAlg:: Initialisation ended  " << endreq;
    return StatusCode::SUCCESS;

}


/// Execute - on event by event

StatusCode MDTPRDValAlg::execute()
{

  /**
     This algorithm performs a hit reallocation check for SimHits, Mdt SDO and MdtPrepData objects in the event.
     The reallocation is performed by extrapolating the muon truth parameters at the Vertex or the MuonEntryRecord
     to all MdtPrepData objects in the event that belong to a muon in the event.
     
  */
  if( m_debug ) *m_log << MSG::DEBUG << " MDTPRDValAlg:: execute " << endreq;  
    
  TruthMap truthMap;
  MuonMdtHitMap muonMdtHitMap;

  // add MC event collection
  addMcEventCollection( truthMap );

  *m_log << MSG::INFO << " addMcEventCollection truth particles found " << truthMap.size() << endreq;

  // add muon entry record
  addMuonRecord( truthMap, false );
  *m_log << MSG::INFO << " addMuonEntryRecord truth particles found " << truthMap.size() << endreq;
  // add exit record
  addMuonRecord( truthMap, true );
  *m_log << MSG::INFO << " addMuonExitRecord truth particles found " << truthMap.size() << endreq;


  // check whether we found truth particles
  if( truthMap.empty() ){
    *m_log << MSG::WARNING << "No truth particles found, cannot perform test" << endreq;
    return StatusCode::SUCCESS;
  }
  
  // add simhits
  addSimHits( muonMdtHitMap, truthMap );

  // add sdo's
  addSimData( muonMdtHitMap, truthMap );

  // check whether we found truth hits
  if( muonMdtHitMap.empty() ){
    if( m_debug ) *m_log << MSG::DEBUG << "No truth hits found, cannot perform test" << endreq;
    return StatusCode::SUCCESS;
  }

  // add prep data
  addPrepData( muonMdtHitMap );

  // analyse data
  analyseHits( muonMdtHitMap, truthMap );
  return StatusCode::SUCCESS;
  
}/**Close execute*/

 
StatusCode MDTPRDValAlg::finalize()
{
  *m_log << MSG::INFO << " MDTPRDValAlg:: finalize " << endreq;

  delete m_log;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
ITHistSvc* MDTPRDValAlg::ToolRootHistSvc()
  /*---------------------------------------------------------*/
{
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() ) 
    {
      *m_log << MSG::WARNING << ">>> Unable to locate the MDTPRDValAlg Histogram service" << endreq;
    }
   
  return m_rootsvc;
} 



void MDTPRDValAlg::addMcEventCollection( MDTPRDValAlg::TruthMap& truthMap ) const {


  /**Access MC truth information*/
  const DataHandle<McEventCollection> mcEvent;
  if(!m_sgSvc->contains<McEventCollection>(m_key)) {
    if( m_debug ) *m_log << MSG::DEBUG << "MDTPRDValAlg: Could not find MCevent" << endreq;    
    return;
  }
  if(m_sgSvc->retrieve(mcEvent,m_key).isFailure()){
      *m_log << MSG::WARNING << "MDTPRDValAlg: Could not retrieve MCevent" << endreq;
      return;
  }else{
    if( m_debug ) *m_log << MSG::DEBUG << "MDTPRDValAlg: Found MCEvent" << endreq;
  }


  DataVector<HepMC::GenEvent>::const_iterator e;
  if (mcEvent->size()!=1) {
    *m_log << MSG::WARNING << " MC event size larger than one: exit algorithm " << endreq;
  }
  if( m_verbose ) *m_log << MSG::VERBOSE << " looping over MC particles " << endreq;
  for (e=mcEvent->begin();e!=mcEvent->end(); e++) {
    for (HepMC::GenEvent::particle_const_iterator p= (**e).particles_begin(); p!= (**e).particles_end(); p++) {

      int pdg = (**p).pdg_id();
      if( abs(pdg) != 13 && pdg !=0 && abs(pdg) != 998) continue;
      if( !(**p).production_vertex() ) continue;

      int barcode = (**p).barcode();
      *m_log << MSG::VERBOSE << " pdg " << pdg << " barcode " <<  barcode << endreq;
      
      TruthInfo* info; 

      // look up barcode
      TruthIt pos = truthMap.find(barcode);
      if( pos == truthMap.end() ){
	// not present, add
	info = new TruthInfo();
	info->barcode = barcode;
	info->pdg = pdg;
	truthMap.insert( std::make_pair(barcode,info) );
        *m_log << MSG::VERBOSE << "  truthMap.size()  " << truthMap.size() << endreq;
      } else{
	info = pos->second;
	if( info->pdg != pdg || info->barcode != barcode ){
	  *m_log << MSG::WARNING << " TruthInfo inconsistent " << endreq;
	  continue;	
	}
	if( info->vertex ){
	  *m_log << MSG::WARNING << " TruthInfo already has vertex?!?!? " << endreq;
	  continue;
	}
      }
      
      ParticleParameters* particle = new ParticleParameters();
      particle->charge   = pdg < 0 ? 1. : -1.;
//  Fix geantino
      if(pdg==-998)  particle->charge   = -1.; 
      if(pdg==0)  particle->charge   = 1.; 
      particle->momentum = Amg::Vector3D( (**p).momentum().px(),(**p).momentum().py(),(**p).momentum().pz());
      particle->position = Amg::Vector3D( (**p).production_vertex()->position().x(),
				       (**p).production_vertex()->position().y(),
				       (**p).production_vertex()->position().z() );
      
      particle->pars= new Trk::Perigee( particle->position, particle->momentum, particle->charge, Amg::Vector3D(0.,0.,0.) );

      info->vertex = particle;

      if( m_debug ) *m_log << MSG::DEBUG << " New muon at vertex " << barcode << "  pos " << particle->position
			   << " phi " << particle->momentum.phi() << " theta " << particle->momentum.theta()
			   << " q*p " << particle->charge*particle->momentum.mag() << endreq;
    }
  }
  if( m_debug ) {
    if( truthMap.empty() ) *m_log << MSG::DEBUG << "MDTPRDValAlg: No Muons in MC event " << endreq;
    else                   *m_log << MSG::DEBUG << "MDTPRDValAlg: Found Muons: " << truthMap.size() << " in MC event" << endreq;
  }    
}


void MDTPRDValAlg::addMuonRecord( TruthMap& truthMap, bool exit ) const {

  const TrackRecordCollection* truthCollection = 0;
  std::string location = "MuonEntryLayer"; // Was "MuonEntryLayer"  
  if(exit) location = "MuonExitLayer";
  if ( m_sgSvc->contains<TrackRecordCollection>(location) ) {
    if(m_sgSvc->retrieve(truthCollection,location ).isFailure()){
      *m_log << MSG::WARNING << " Could not retrieve " << location << endreq;
    }
  }    
  
  if( !truthCollection ){
    location ="MuonEntryLayerFilter";
    if(exit) location = "MuonExitLayerFilter";
    if( m_sgSvc->contains<TrackRecordCollection>(location) ){
      if(m_sgSvc->retrieve(truthCollection,location ).isFailure()){
	*m_log << MSG::WARNING << " Could not retrieve " << location << endreq;
      }
    }
  }

  if( !truthCollection ){
    location ="MuonEntryRecord";
    if(exit) location = "MuonExitRecord";
    if( m_sgSvc->contains<TrackRecordCollection>(location) ){
      if(m_sgSvc->retrieve(truthCollection,location ).isFailure()){
	*m_log << MSG::WARNING << " Could not retrieve " << location << endreq;
      }
    }
  }

  if( !truthCollection ){
    *m_log << MSG::WARNING << "MDTPRDValAlg: Could not retrieve TrackRecordCollection " << endreq;
    return;
  }
      
  if( m_debug ) *m_log << MSG::DEBUG << "retrieved TrackRecordCollection "  << truthCollection->size() << " at " << location << endreq;
  TrackRecordConstIterator tr_it = truthCollection->begin();
  TrackRecordConstIterator tr_it_end = truthCollection->end();
  for(;tr_it!=tr_it_end; ++tr_it){

    int pdg = (*tr_it).GetPDGCode();
    // only use muons
    if( abs(pdg) != 13 && pdg !=0 && abs(pdg) != 998) continue;
    
    int barcode = (*tr_it).GetBarCode();
    
    TruthInfo* info; 

    // look up barcode
    TruthIt pos = truthMap.find(barcode);
    if( pos == truthMap.end() ){
      // not present, add
      info = new TruthInfo();
      info->barcode = barcode;
      info->pdg = pdg;
      truthMap.insert( std::make_pair(barcode,info) );
    }else{
      info = pos->second;
      if( info->pdg != pdg || info->barcode != barcode ){
	*m_log << MSG::WARNING << " TruthInfo inconsistent " << endreq;
	continue;	
      }
      if( info->muonEntry && !exit){
	*m_log << MSG::WARNING << " TruthInfo already has vertex?!?!? " << endreq;
	continue;
      }
    }
    
    ParticleParameters* particle = new ParticleParameters();
    particle->charge   = pdg < 0 ? 1. : -1.;
    if(pdg==-998)  particle->charge   = -1.; 
    if(pdg==0)  particle->charge   = 1.; 
    particle->position = Amg::Vector3D( (*tr_it).GetPosition().x(), (*tr_it).GetPosition().y(), (*tr_it).GetPosition().z() );
    particle->momentum = Amg::Vector3D( (*tr_it).GetMomentum().x(), (*tr_it).GetMomentum().y(), (*tr_it).GetMomentum().z() );
      
//    particle->pars= new Trk::Perigee( particle->position, particle->momentum, particle->charge, Amg::Vector3D(0.,0.,0.) );
    particle->pars= new Trk::Perigee( particle->position, particle->momentum, particle->charge, particle->position );

    if(!exit) info->muonEntry = particle;
    if(exit) info->muonExit = particle;
    double e = (*tr_it).GetEnergy();
    double mass = e*e - particle->momentum.mag()*particle->momentum.mag();
    if(mass>0) mass = sqrt(mass); 
    if( m_debug ) *m_log << MSG::DEBUG << " New muon at " << location << " barcode  " << barcode <<  " pdgid " << pdg
                         << " pos " << particle->position 
                         << " pergee pos " << particle->pars->position()
			 << " phi " << particle->momentum.phi() << " theta " << particle->momentum.theta()
			 << " q*p " << particle->charge*particle->momentum.mag() << " energy " << (*tr_it).GetEnergy() 
                         << " mass " << mass << " volumeName " << (*tr_it).GetVolName() << endreq;
      
  }
  
  if( m_debug ) {
    if( truthMap.empty() ) *m_log << MSG::DEBUG << "MDTPRDValAlg: No Muons in TrackRecord event " << endreq;
    else                   *m_log << MSG::DEBUG << "MDTPRDValAlg: Found Muons: " << truthMap.size() << " in Track Record" << endreq;
  }
} 


void MDTPRDValAlg::addSimHits( MDTPRDValAlg::MuonMdtHitMap& muonMdtHitMap, MDTPRDValAlg::TruthMap& truthMap ) const {

  
  std::string location = "MDT_Hits";
  if ( !m_sgSvc->contains<MDTSimHitCollection>(location) ) {
    if( m_debug ) *m_log << MSG::DEBUG << " No SimHits found at " << location << endreq;
    return;
  }
  
  const DataHandle<MDTSimHitCollection> p_collection;
  if (m_sgSvc->retrieve(p_collection,location).isFailure()) {
    *m_log << MSG::WARNING << "No MDTSimHitCollection in StoreGate!" << endreq;
    return;
  }

  
  MdtHitIdHelper* mdthelper = MdtHitIdHelper::GetHelper(m_mdtIdHelper->tubeMax());
  
  for(MDTSimHitCollection::const_iterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit) {

    const MDTSimHit* simHit = &(*i_hit);
    if( !simHit ) continue;

    int barcode = simHit->particleLink().barcode();
    
    // skip barcodes not in the truth map
    if( !truthMap.count(barcode) ) continue;

    /** Check the Hits identifiers, access the functions that give:
	Station name, station eta, station phi, multilayer ID, layer ID, tube ID.
    */		
    HitID mdthit= simHit->MDTid();
 	       
    std::string mdt_stname = mdthelper->GetStationName(mdthit);
    /**Function that gives int station name from string station name.Locatedin MuonValutils*/                      
    int         mdt_steta  = mdthelper->GetZSector(mdthit);
    int         mdt_stphi  = mdthelper->GetPhiSector(mdthit);
    int         mdt_ml     = mdthelper->GetMultiLayer(mdthit);
    int         mdt_tl     = mdthelper->GetLayer(mdthit);
    int         mdt_tube   = mdthelper->GetTube(mdthit);

    /**Create the offline identifiers, fill them with hit info.Ready to be accessed and retrieve info.
       Currently not used in this code except for some checks.
    */
    Identifier offid = m_mdtIdHelper->channelID(mdt_stname, mdt_steta, mdt_stphi,mdt_ml,mdt_tl,mdt_tube);
    if (offid == 0){
      *m_log << MSG::WARNING << "MDT: Cannot build a valid Identifier; skip " << endreq;
      continue;
    }
    if( m_verbose ) *m_log << MSG::VERBOSE << " SimHit: barcode " << barcode << "  " << m_mdtIdHelper->print_to_string(offid) << endreq;


    MdtHitMap& mdtHitMap = muonMdtHitMap[barcode];
    MdtHitIt pos = mdtHitMap.find(offid);
    if( pos == mdtHitMap.end() ){
      MdtHitData* mdtHitData = new MdtHitData();
      mdtHitData->barcode = barcode;
      mdtHitData->simHit = simHit;
      mdtHitMap.insert( std::make_pair(offid,mdtHitData) );
    }else{
      MdtHitData* mdtHitData = pos->second;
      if( mdtHitData->barcode != barcode ){
	*m_log << MSG::WARNING << " MdtHitData barcode inconsistent " << endreq;
	continue;	
      }
      if( mdtHitData->simHit ){
	*m_log << MSG::WARNING << " MdtHitData for barcode " << barcode << " already has sim hit " << endreq;
	continue;
      }
      mdtHitData->simHit = simHit;
    }
  }
  if( m_debug ) *m_log << MSG::DEBUG << " filled simhit into map " << muonMdtHitMap.size() << endreq;
}


void MDTPRDValAlg::addSimData( MDTPRDValAlg::MuonMdtHitMap& muonMdtHitMap, MDTPRDValAlg::TruthMap& truthMap ) const {

  //Retrieving MDT truth hits from MDT_SDO container 
  std::string  location = "MDT_SDO";
  const MuonSimDataCollection* sdoContainer = 0;
  if ( !m_sgSvc->contains<MuonSimDataCollection>(location) ) {
    if( m_debug ) *m_log << MSG::DEBUG << " No SimData found at " << location << endreq;
    return;
  }
  
  if (m_sgSvc->retrieve(sdoContainer,location).isFailure()) {
    *m_log << MSG::WARNING << "No MDT Sdo Container found" << endreq;
    return;
  }
  if( m_debug ) *m_log << MSG::DEBUG << "MDT Sdo Container found " << sdoContainer->size() << endreq;
  

  MuonSimDataCollection::const_iterator sit = sdoContainer->begin();
  MuonSimDataCollection::const_iterator sit_end = sdoContainer->end();
  for( ;sit!=sit_end;++sit ){
    
    const Identifier&  id      = sit->first;
    const MuonSimData& simData = sit->second;
    
    if( simData.getdeposits().empty() ) {
      *m_log << MSG::WARNING << "MDT Sdo without deposits " << m_mdtIdHelper->print_to_string(id) << endreq;
      continue;
    }
    int barcode = simData.getdeposits().front().first.barcode();
    if( m_debug ) *m_log << MSG::DEBUG << " SDO barcode " << barcode << endreq;
   
    if(barcode==0) {
//      barcode = 10001;
      *m_log << MSG::WARNING << " barcode == 0 fixed to " << barcode << endreq; 
    } 
 
    if( !truthMap.count(barcode) ) continue;

    MdtHitMap& mdtHitMap = muonMdtHitMap[barcode];
    MdtHitIt pos = mdtHitMap.find(id);
    if( pos == mdtHitMap.end() ){
      MdtHitData* mdtHitData = new MdtHitData();
      mdtHitData->barcode = barcode;
      mdtHitData->sdo = &simData;
      mdtHitMap.insert( std::make_pair(id,mdtHitData) );
    }else{
      MdtHitData* mdtHitData = pos->second;
      if( mdtHitData->barcode != barcode ){
	*m_log << MSG::WARNING << " MdtHitData barcode inconsistent " << endreq;
	continue;	
      }
      if( mdtHitData->sdo ){
	*m_log << MSG::WARNING << " MdtHitData for barcode " << barcode << " already has sim hit " << endreq;
	continue;
      }

      if( m_verbose ) *m_log << MSG::VERBOSE << "New SDO: barcode  " << barcode << "  " 
			     << m_mdtIdHelper->print_to_string(id) << endreq;
      mdtHitData->sdo = &simData;
    }
  }
}


void MDTPRDValAlg::addPrepData( MDTPRDValAlg::MuonMdtHitMap& muonMdtHitMap ) const{

  //MDT raw hits....
  const Muon::MdtPrepDataContainer* mdtPrds = 0;      
  std::string  location = "MDT_DriftCircles";
  if ( !m_sgSvc->contains<Muon::MdtPrepDataContainer>(location) ) {
    if( m_debug ) *m_log << MSG::DEBUG << " No MdtPrepData found at " << location << endreq;
    return;
  }
  
  if( m_sgSvc->retrieve( mdtPrds,location ).isFailure() ) {
    *m_log << MSG::WARNING << "MdtPrepDataContainer not found at " << location << endreq;
    return;
  }

  if( m_debug ) *m_log << MSG::DEBUG << "MdtPrepDataContainer found at " << location << endreq;
      
  Muon::MdtPrepDataContainer::const_iterator p_mdt_it = mdtPrds->begin();
  Muon::MdtPrepDataContainer::const_iterator p_mdt_it_end = mdtPrds->end();
  for( ; p_mdt_it!=p_mdt_it_end; ++p_mdt_it ){  
    Muon::MdtPrepDataCollection::const_iterator mdt_it = (*p_mdt_it)->begin();
    Muon::MdtPrepDataCollection::const_iterator mdt_it_end = (*p_mdt_it)->end();
    for( ; mdt_it!=mdt_it_end; ++ mdt_it){

      // get values of MdtPrepData
      const Muon::MdtPrepData& mdt = **mdt_it;
      
      const Identifier& id = mdt.identify();
      
      MdtHitData* mdtHitData = findMdtHitData( id, muonMdtHitMap );
      if( !mdtHitData ) continue;

      if( mdtHitData->prd ){
	*m_log << MSG::WARNING << " MdtHitData already has prd " << endreq;
	continue;
      }

      if( m_verbose ) *m_log << MSG::VERBOSE << " Muon PRD: barcode " << mdtHitData->barcode 
			     << "  " << m_mdtIdHelper->print_to_string(id) << endreq;

      mdtHitData->prd = &mdt;
    }
  }
}

MDTPRDValAlg::MdtHitData* MDTPRDValAlg::findMdtHitData( const Identifier& id, MDTPRDValAlg::MuonMdtHitMap& muonMdtHitMap ) const {
 
  // loop over muons
  MuonMdtHitIt it = muonMdtHitMap.begin();
  MuonMdtHitIt it_end = muonMdtHitMap.end();
  for( ;it!=it_end;++it ){
    MdtHitIt pos = it->second.find(id);
    if( pos!=it->second.end() ) return pos->second;
  }
  return 0;
}


void MDTPRDValAlg::analyseHits( MuonMdtHitMap& muonMdtHitMap, TruthMap& truthMap ) {


  // ---------------------------------------
  // fill event data
  /** get EventInfo, used to obtain run and event number */
  const xAOD::EventInfo* pevt;
		
	
  if (m_sgSvc->retrieve(pevt).isFailure()) {
    *m_log << MSG::WARNING << "Could not find event" << endreq;
    return;
  }else {
    if( m_debug ) *m_log << MSG::DEBUG << "Found EventInfo in SG" << endreq;
  }	

  if(m_verbose) {
    *m_log << MSG::VERBOSE <<"Processing EventInfo event #"<<pevt->eventNumber() << " run: " << pevt->runNumber() << endreq;
  }


  /**Get number of events processed and run number*/
  int evt = pevt->eventNumber();
  int numrun = pevt->runNumber();
  m_Validation_MDT_RunNumber = numrun;

  /**Enter MDT hits loop, initialize the hits counter*/
  counter_ValHitNumber =0 ;
  
  if( m_debug ) *m_log << MSG::DEBUG << " Event number: " <<  evt << endreq;
  if( m_debug ) *m_log << MSG::DEBUG << "Looping over muons: " << muonMdtHitMap.size() << endreq;
      
  MuonMdtHitIt mit = muonMdtHitMap.begin();
  MuonMdtHitIt mit_end = muonMdtHitMap.end();
  for( ;mit!=mit_end;++mit ){

    int barcode = mit->first;
    
    if( m_debug ) *m_log << MSG::DEBUG << "Muon with bar code: " << barcode << "  associated hits " << mit->second.size() << endreq;

    TruthIt tit = truthMap.find(barcode);
    if( tit == truthMap.end() ){
      *m_log << MSG::WARNING << " barcode not found " << endreq;
      continue;
    }
    
    ParticleParameters* vertex = tit->second->vertex;
    ParticleParameters* muonEntry = tit->second->muonEntry;
    ParticleParameters* muonExit = tit->second->muonExit;
    
    ParticleParameters* muon = m_useEntryLayer ? muonEntry : vertex;
    int pdgcode = tit->second->pdg;
    if( !muon ){
      // if no entry record but vertex, use vertex
      if( m_useEntryLayer && vertex ){
	*m_log << MSG::WARNING << " No MuonEntry parameters, using vertex parameters instead " << endreq;
	muon = vertex;
      }else if( !m_useEntryLayer && muonEntry ){
	*m_log << MSG::WARNING << " No vertex parameters, using MuonEntry parameters instead " << endreq;
	muon = vertex;
      }else{
	*m_log << MSG::WARNING << " No parameters cannot perform extrapolation " << endreq;
	continue;
      }
    }
    
    if( !muon || !muon->pars ) {
      *m_log << MSG::WARNING << " No track parameters cannot perform extrapolation " << endreq;
      continue;
    }

    if( m_debug ) *m_log << MSG::DEBUG << " muon entry " << barcode << "  pos " << muon->position
			 << " phi " << muon->momentum.phi() << " theta " << muon->momentum.theta()
			 << " q*p " << muon->charge*muon->momentum.mag() << endreq;
    if( m_debug && muonExit) *m_log << MSG::DEBUG << " muon Exit " << barcode << "  pos " << muonExit->position
			 << " phi " << muonExit->momentum.phi() << " theta " << muonExit->momentum.theta()
			 << " q*p " << muonExit->charge*muonExit->momentum.mag() << endreq;


    
    const Trk::TrackParameters& muonPerigee = *muon->pars;
  
    if( m_debug ) *m_log << MSG::DEBUG << "  muonPerigee position " << muonPerigee.position() << " momentum " << muonPerigee.momentum() << endreq; 
    // loop over MdtHitData
    MdtHitIt hit = mit->second.begin();
    MdtHitIt hit_end = mit->second.end();
    unsigned int ih = 0;
    for( ;hit!=hit_end;++hit ){
      ih++;
      bool last = false;
      if(ih==mit->second.size()) last = true;
      const Identifier& id = hit->first;
      
      MdtHitData& hitData = *hit->second;
      
      // get pointers to data
      const Muon::MdtPrepData* mdt     = hitData.prd;
      const MuonSimData*       sdo     = hitData.sdo;
      const MDTSimHit*         simHit  = hitData.simHit;


      if( !mdt ) {
	*m_log << MSG::WARNING << " MdtHitData without MdtPrepData!!! " << endreq;
	continue;
      }

      if( mdt->identify() != id ){
	*m_log << MSG::WARNING << " Identifier of MdtHitData not consisted with Identifier of MdtPrepData!!! " << endreq;
	continue;
      }

      const MuonGM::MdtReadoutElement* detEl = mdt->detectorElement();
      if (!detEl) throw std::runtime_error(Form("File: %s, Line: %d\nMDTPRDValAlg::analyseHits() - no associated detectorElement", __FILE__, __LINE__));

      // transform to global coords
      Amg::Vector3D simHitPosLoc(simHit->localPosition().x(), simHit->localPosition().y(), simHit->localPosition().z());
      Amg::Vector3D simHitPos(0., 0., 0.);
      double simRadius = 0.;
      double simDistRO = 0.;
      if( simHit ){
	simHitPos = detEl->localToGlobalCoords(simHitPosLoc , id );
	simRadius = simHit->driftRadius();
	simDistRO = simHit->localPosition().z();
      }

      double sdoRadius = 0.;
      double sdoDistRO = 0.;
      Amg::Vector3D sdoHitPos(0., 0., 0.);
      if( sdo ){
	std::vector< MuonSimData::Deposit >::const_iterator deposits_it = sdo->getdeposits().begin() ;
	std::vector< MuonSimData::Deposit >::const_iterator deposits_it_end = sdo->getdeposits().end() ;
	for( ; deposits_it!= deposits_it_end; ++deposits_it) {
	  sdoRadius = (*deposits_it).second.firstEntry();
	  sdoDistRO = (*deposits_it).second.secondEntry();
          if(m_debug) {
// check geometry
            Amg::Vector3D lpos0(0.,0.,0.);
	    sdoHitPos = detEl->localToGlobalCoords(lpos0, id );
            if(m_debug) *m_log << MSG::DEBUG << " sdoHitPos 0 0 0 " << sdoHitPos << endreq;
            Amg::Vector3D lpos1(100.,0.,0.);
	    sdoHitPos = detEl->localToGlobalCoords(lpos1, id );
            if(m_debug) *m_log << MSG::DEBUG << " sdoHitPos 100 0 0 " << sdoHitPos << endreq;
            Amg::Vector3D lpos2(0.,100.,0.);
	    sdoHitPos = detEl->localToGlobalCoords(lpos2, id );
            if(m_debug) *m_log << MSG::DEBUG << " sdoHitPos 0 100 0 " << sdoHitPos << endreq;
            Amg::Vector3D lpos3(0.,0.,100.);
	    sdoHitPos = detEl->localToGlobalCoords(lpos3, id );
            if(m_debug) *m_log << MSG::DEBUG << " sdoHitPos 0 0 100 " << sdoHitPos << endreq;
          } 
// position along wire  = locZ in DetEl frame ( != Tracking frame...)
          Amg::Vector3D lpos(0.,0.,sdoDistRO);
	  sdoHitPos = detEl->localToGlobalCoords(lpos, id );
          if(m_debug) *m_log << MSG::DEBUG << " sdoHitPos " << sdoHitPos << " phi " << sdoHitPos.phi() << endreq;
	}

// simHit = sdoHit = Wire position in global coordinates 
        if(!simHit) simHitPos = sdoHitPos;
      }
// in tracking frame locX == MDT radius locY along wire  
      Amg::Vector2D sdoLocalHitPos(sdoRadius,sdoDistRO);

      const Trk::SaggedLineSurface& sagSurf = detEl->surface(id);
	
      const Trk::TrackParameters* expar = m_extrapolator->extrapolateDirectly(muonPerigee,sagSurf,
								  Trk::alongMomentum,false);
      if( !expar ){
	*m_log << MSG::INFO << " extrapolation failed " << endreq;
	continue;
      }
     if(m_debug) *m_log << MSG::DEBUG << " extrapolated position " << expar->position() << " local position X " << expar->parameters()[Trk::locX] << " local position Y " << expar->parameters()[Trk::locY] << " position phi " << expar->position().phi() << " global phi direction " << expar->parameters()[Trk::phi] << endreq;  
     if(m_debug) *m_log << MSG::DEBUG << " SL surface position  " << sagSurf.center() << endreq;  
     // calculate local position of extrapolated track parameter
     Amg::Vector2D lexpos, lsimpos;
     bool trf_ok=sagSurf.globalToLocal( expar->position(), expar->momentum(), lexpos);
     if(trf_ok) trf_ok=sagSurf.globalToLocal( expar->position(), expar->momentum() , lsimpos);
      
      if( !trf_ok ){
	*m_log << MSG::INFO << " global to local failed " << endreq;
	delete expar;
	continue;
      }
//
//  Perform a full extrapolation including material and Eloss
//
      const Trk::TrackParameters* exparEloss = m_extrapolator->extrapolate(muonPerigee,sagSurf,Trk::alongMomentum,false);
      if( !exparEloss ){
	*m_log << MSG::INFO << " extrapolation failed " << endreq;
	delete expar;
	continue;
      }
      Amg::Vector2D lexposEloss(0., 0.);
      if( sagSurf.globalToLocal( exparEloss->position(), exparEloss->momentum() , lexposEloss) ){
	*m_log << MSG::INFO << " global to local failed lexposEloss " << endreq;
	delete expar;
	delete exparEloss;
	continue;
      }

      const Trk::StraightLineSurface* saggedSurface = sagSurf.correctedSurface(lexpos);
      if( !saggedSurface ){
	*m_log << MSG::INFO << "  could to create sagged surface  " << endreq;
	delete expar;
	delete exparEloss;
	continue;
      }
      const Trk::TrackParameters* exparSag = m_extrapolator->extrapolateDirectly(muonPerigee,*saggedSurface,
								     Trk::alongMomentum,false);
      if( !exparSag ){
	*m_log << MSG::INFO << " extrapolation failed " << endreq;
	delete saggedSurface;
	delete expar;
	delete exparEloss;
	continue;
      }

      // calculate local position of extrapolated track parameter 
      Amg::Vector2D lexposSag(0., 0.);
      if( saggedSurface->globalToLocal( exparSag->position(), expar->momentum() ,  lexposSag)){
	*m_log << MSG::INFO << " global to local failed " << endreq;
	delete saggedSurface;
	delete expar;
	delete exparEloss;
	delete exparSag;
	continue;
      }

	
      // check ROTs
      Amg::Vector3D hitPos = simHit ? simHitPos : expar->position();
      if(sdo&&!simHit) hitPos = sdoHitPos; 
      const Muon::MdtDriftCircleOnTrack* rot = m_rotCreator->createRIO_OnTrack( *mdt, hitPos );
      if( !rot ){
	*m_log << MSG::INFO << " rot creation failed " << endreq;
	delete saggedSurface;
	delete expar;
	delete exparEloss;
	delete exparSag;
	continue;
      }
     
      const Trk::TrackParameters* exparSagRot = m_extrapolator->extrapolateDirectly(muonPerigee,rot->associatedSurface(),
									Trk::alongMomentum,false);
      if( !exparSagRot ){
	*m_log << MSG::INFO << " extrapolation failed " << endreq;
        delete rot;
	delete saggedSurface;
	delete expar;
	delete exparEloss;
	delete exparSag;
	continue;
      }
      const Trk::StraightLineSurface* slSurf = dynamic_cast<const Trk::StraightLineSurface*>(&rot->associatedSurface());
      // calculate local position of extrapolated track parameter 
      Amg::Vector2D lexposSagRot(0., 0.);
      if( slSurf ) {
	trf_ok = slSurf->globalToLocal( exparSagRot->position(), exparSagRot->momentum(), lexposSagRot );
      }else{
	trf_ok = rot->associatedSurface().globalToLocal( exparSagRot->position(), Amg::Vector3D(0., 0., 0.), lexposSagRot );
      }
      if(m_debug) *m_log << MSG::DEBUG << " extrapolated position ito ROT " << exparSagRot->position() << endreq;  
      if(m_debug) *m_log << MSG::DEBUG << " SL surface position ROT  " << slSurf->center() << endreq;  

      if( trf_ok ){
	*m_log << MSG::INFO << " global to local failed " << endreq;
        delete rot;
	delete saggedSurface;
	delete expar;
	delete exparEloss;
	delete exparSag;
	delete exparSagRot;
	continue;
      }

      // increase hit counter
      ++counter_ValHitNumber;
      /**AANtuple variables
	 Fill the AANtuple variables with a value obtained for each hit within the event
      */
      m_Validation_MDT_Type=1;
      m_Validation_MDT_NumberOfHits=counter_ValHitNumber;
      m_Validation_MDT_EventNumber=evt;			
      m_Validation_MDT_pdg =  pdgcode;

      m_Validation_MDT_phi =  muon->momentum.phi();
      m_Validation_MDT_theta =  muon->momentum.theta(); 
      m_Validation_MDT_eta =  muon->momentum.eta(); 
      m_Validation_MDT_qp =  muon->charge*muon->momentum.mag();
      m_Validation_MDT_MEX =  muon->position.x();
      m_Validation_MDT_MEY =  muon->position.y();
      m_Validation_MDT_MEZ =  muon->position.z();
      m_Validation_MDT_Exit_qp =  0;
      m_Validation_MDT_ExitX =  0;
      m_Validation_MDT_ExitY =  0;
      m_Validation_MDT_ExitZ =  0;
      if(muonExit) {
        m_Validation_MDT_Exit_qp =  muonExit->charge*muonExit->momentum.mag();
        m_Validation_MDT_ExitX =  muonExit->position.x();
        m_Validation_MDT_ExitY =  muonExit->position.y();
        m_Validation_MDT_ExitZ =  muonExit->position.z();
      } 

// put station name string in AANTUPLE as Val_StName  
      if( m_mdtIdHelper->stationNameString( m_mdtIdHelper->stationName(id) ).size() == 3 )
        strcpy(m_Validation_MDT_StationName,m_mdtIdHelper->stationNameString( m_mdtIdHelper->stationName(id) ).c_str() );
      else
        strcpy(m_Validation_MDT_StationName,"ERR" );

      std::string stName = m_mdtIdHelper->stationNameString( m_mdtIdHelper->stationName(id));
      int geoSign = 1;
      int codeBESL = 0;
      if(stName[0]=='B') {
        if(m_mdtIdHelper->stationEta(id)>0) geoSign = -1;
        if(stName[2]=='L') codeBESL = 1;
        if(stName[2]=='S') codeBESL = 2;
        if(stName[2]=='F') codeBESL = 2;
        if(stName[2]=='G') codeBESL = 2;
        if(stName[2]=='H') codeBESL = 2;
// BIM/BIR are large 
        if(stName[2]=='M') codeBESL = 1;
        if(stName[2]=='R') codeBESL = 1;
// BEE
        if(stName[2]=='E') codeBESL = 3;
      } else if (stName[0]=='E') {
        if(m_mdtIdHelper->stationEta(id)<0) geoSign = -1;
        if(stName[2]=='L') codeBESL = 11;
        if(stName[2]=='S') codeBESL = 12;
// EE
        if(stName[1]=='E') codeBESL = 13;
      }

      m_Validation_MDT_StationEta=m_mdtIdHelper->stationEta(id);
      m_Validation_MDT_StationPhi=m_mdtIdHelper->stationPhi(id);
      m_Validation_MDT_IDTube=m_mdtIdHelper->tube(id);
      m_Validation_MDT_IDMultiLayer=m_mdtIdHelper->multilayer(id);
      m_Validation_MDT_IDLayer=m_mdtIdHelper->tubeLayer(id);
      m_Validation_MDT_GeoSign = geoSign;
      m_Validation_MDT_BESL = codeBESL;


      m_Validation_MDT_HitX = simHitPos.x();
      m_Validation_MDT_HitY = simHitPos.y();
      m_Validation_MDT_HitZ = simHitPos.z();
      m_Validation_MDT_HitR = simHitPos.perp();

      m_Validation_MDT_ExX = expar->position().x();
      m_Validation_MDT_ExY = expar->position().y();
      m_Validation_MDT_ExZ = expar->position().z();
      m_Validation_MDT_ExR = expar->position().perp();

      m_Validation_MDT_LocalX= simHit ? simHit->localPosition().x() : 0.;
      m_Validation_MDT_LocalY= simHit ? simHit->localPosition().y() : 0.;
      m_Validation_MDT_LocalZ= simHit ? simHit->localPosition().z() : 0.;
      m_Validation_MDT_LocalR= simHit ? simHit->localPosition().perp() : 0.;

      if(!simHit&&sdo) {
        m_Validation_MDT_LocalX= 0.;
        m_Validation_MDT_LocalY= 0.;
        m_Validation_MDT_LocalZ= sdoLocalHitPos[Trk::locZ];
        m_Validation_MDT_LocalR= sdoLocalHitPos[Trk::locR];
      }

// For this surface type:
// sdoLocalHitPos[Trk::locX] = sdoLocalHitPos[Trk::locR]; 
// sdoLocalHitPos[Trk::locY] = sdoLocalHitPos[Trk::locZ]

      m_Validation_MDT_Res_LocalX =  (lexpos)[Trk::locX] - sdoLocalHitPos[Trk::locX]; 
      m_Validation_MDT_Res_LocalY =  (lexpos)[Trk::locY] - sdoLocalHitPos[Trk::locY]; 
      m_Validation_MDT_ResEloss_LocalX =  (lexposEloss)[Trk::locX] - sdoLocalHitPos[Trk::locX]; 
      m_Validation_MDT_ResEloss_LocalY =  (lexposEloss)[Trk::locY] - sdoLocalHitPos[Trk::locY]; 

      Amg::Transform3D gtoLocal = sagSurf.transform().inverse();
      Amg::Vector3D locTube = gtoLocal*sagSurf.center();
      Amg::Vector3D lexpos3 = gtoLocal*expar->position();
      Amg::Vector3D ldifference = lexpos3 - locTube;
      ldifference[2]=0.; 
      int sign = expar->parameters()[Trk::locR] < 0 ? -1 : 1;
      ldifference = sign*ldifference.unit();      
      Amg::Vector3D gpos = sagSurf.transform()*lexpos3;
      Amg::Vector3D sum = lexpos3+ldifference;
      Amg::Vector3D gposX1mm = sagSurf.transform()*sum;
      sum = lexpos3+Amg::Vector3D(0.,0.,1.);
      Amg::Vector3D gposY1mm = sagSurf.transform()*sum;
      m_Validation_MDT_dtheta_dLocX  = gposX1mm.theta() - gpos.theta();
      m_Validation_MDT_dtheta_dLocY  = gposY1mm.theta() - gpos.theta();
      m_Validation_MDT_dphi_dLocY = gposY1mm.phi() - gpos.phi();
      m_Validation_MDT_dphi_dLocX = gposX1mm.phi() - gpos.phi();

      m_Validation_MDT_dtheta =  expar->momentum().theta() - muon->momentum.theta(); 

      Amg::Vector3D dpos = muon->position - sdoHitPos;
      m_Validation_MDT_3DdistME =  dpos.mag(); 

      m_Validation_MDT_ploss = muon->momentum.mag() - exparEloss->momentum().mag();
      m_Validation_MDT_3DdistExit = 0.; 
      if(muonExit) {
        dpos = muon->position -  muonExit->position;
        m_Validation_MDT_3DdistExit =  dpos.mag(); 
      }
      m_Validation_MDT_last = 0;
      if(last) m_Validation_MDT_last = 1;
      m_Validation_MDT_ExNomWireR = (lexpos)[Trk::locX];
      m_Validation_MDT_ExNomWireZ = (lexpos)[Trk::locY];
      m_Validation_MDT_ExSagWireR = (lexposSag)[Trk::locX];
      m_Validation_MDT_ExSagWireZ = (lexposSag)[Trk::locY];
      m_Validation_MDT_SimRadius = simRadius;
      m_Validation_MDT_SdoRadius = sdoRadius;
      m_Validation_MDT_WireLen = detEl->getWireLength(m_mdtIdHelper->tubeLayer(id),m_mdtIdHelper->tube(id) );

      m_Validation_MDT_ExSagRotR = (lexposSagRot)[Trk::locX];
      m_Validation_MDT_ExSagRotZ = (lexposSagRot)[Trk::locY];
      m_Validation_MDT_RotRadius = rot->driftRadius();
      
      if( m_debug ){
	double epr = (lexpos)[Trk::locX];
	double epz = (lexpos)[Trk::locY];
	double epsr = (lexposSag)[Trk::locX];
	double epsz = (lexposSag)[Trk::locY];
	double eprr = (lexposSagRot)[Trk::locX];
	double eprz = (lexposSagRot)[Trk::locY];
	double res_epr_sim  = simRadius - fabs(epr);
	double res_epsr_sim = simRadius - fabs(epsr);
	double res_eprr_sim = simRadius - fabs(eprr);
	double res_simr_sim  = simRadius - fabs(simRadius);
	double res_sdor_sim  = simRadius - fabs(sdoRadius);
	double res_epr_sdo  = sdoRadius - epr;
	double res_epsr_sdo = sdoRadius - epsr;
	double res_eprr_sdo = sdoRadius - eprr;
	double res_simr_sdo  = fabs(sdoRadius) - simRadius;
	double res_sdor_sdo  = sdoRadius - sdoRadius;

	double res_epz_sim  = simDistRO - epz;
	double res_epsz_sim = simDistRO - epsz;
	double res_eprz_sim = simDistRO - eprz;
	double res_simz_sim  = simDistRO - simDistRO;
	double res_sdoz_sim  = simDistRO - sdoDistRO;
	double res_epz_sdo  = sdoDistRO - epz;
	double res_epsz_sdo = sdoDistRO - epsz;
	double res_eprz_sdo = sdoDistRO - eprz;
	double res_simz_sdo  = sdoDistRO - simDistRO;
	double res_sdoz_sdo  = sdoDistRO - sdoDistRO;
	*m_log << MSG::DEBUG << "   new PRD " << m_mdtIdHelper->print_to_string( id ) << std::endl << std::setprecision(4) 
		  << std::setw(15) << " " << std::setw(15) << "| expos " << std::setw(15) << "| expos sag " << std::setw(15) << "| expos ROT " 
		  << std::setw(15) <<  "| sim "  << std::setw(15) << "| sdo " << "|" << std::endl
		  << std::setw(15) << " r " << std::setw(15) << epr       << std::setw(15) << epsr          << std::setw(15)  << eprr 
		  << std::setw(15) << simRadius << std::setw(15) << sdoRadius << std::endl
		  << std::setw(15) << " res sim " << std::setw(15) << res_epr_sim << std::setw(15) << res_epsr_sim << std::setw(15) << res_eprr_sim 
		  << std::setw(15) << res_simr_sim << std::setw(15) << res_sdor_sim << std::endl
		  << std::setw(15) << " res sdo " << std::setw(15) << res_epr_sdo << std::setw(15) << res_epsr_sdo << std::setw(15) << res_eprr_sdo 
		  << std::setw(15) << res_simr_sdo << std::setw(15) << res_sdor_sdo << std::endl
		  << std::setw(15) << " z " << std::setw(15) << epz       << std::setw(15) << epsz          << std::setw(15)  << eprz 
		  << std::setw(15) << simDistRO << std::setw(15) << sdoDistRO << std::endl
		  << std::setw(15) << " res sim " << std::setw(15) << res_epz_sim << std::setw(15) << res_epsz_sim << std::setw(15) << res_eprz_sim 
		  << std::setw(15) << res_simz_sim << std::setw(15) << res_sdoz_sim << std::endl
		  << std::setw(15) << " res sdo " << std::setw(15) << res_epz_sdo << std::setw(15) << res_epsz_sdo << std::setw(15) << res_eprz_sdo 
		  << std::setw(15) << res_simz_sdo << std::setw(15) << res_sdoz_sdo << endreq;
      }

      delete rot;
      delete saggedSurface;
      delete expar;
      delete exparEloss;
      delete exparSag;
      delete exparSagRot;


      /**After proccessing each hit, dump the info in the tree*/  
      mdttree->Fill();		
      
    }
  }
}


void MDTPRDValAlg::cleanUp( MDTPRDValAlg::MuonMdtHitMap& muonMdtHitMap ) const {

  MuonMdtHitIt mit = muonMdtHitMap.begin();
  MuonMdtHitIt mit_end = muonMdtHitMap.end();
  for( ;mit!=mit_end;++mit ){
    // loop over MdtHitData
    MdtHitIt hit = mit->second.begin();
    MdtHitIt hit_end = mit->second.end();
    for( ;hit!=hit_end;++hit ){
      delete hit->second;
    }
  }
}


void MDTPRDValAlg::cleanUp( MDTPRDValAlg::TruthMap& truthMap ) const {
  TruthIt it = truthMap.begin();
  TruthIt it_end = truthMap.end();
  for( ;it!=it_end;++it ) delete it->second;
}
