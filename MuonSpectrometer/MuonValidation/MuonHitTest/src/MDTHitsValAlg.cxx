/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHitTest/MDTHitsValAlg.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
 
///for GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ITHistSvc.h"

#include "Identifier/Identifier.h"
 
#include "GeoAdaptors/GeoMuonHits.h"
 
#include "MuonReadoutGeometry/MuonDetectorManager.h" 
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
 
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/MDTSimHit.h"

#include "MuonSimValUtils/MuonChambersRange.h"
#include "MuonSimValUtils/MuonChamberNameConverter.h"
#include "MuonSimValUtils/MuonStrToIntName.h" 

#include "GeneratorObjects/McEventCollection.h"

#include "CLHEP/Vector/LorentzVector.h"

#include <iostream>
#include <fstream>
#include <TH1D.h>
#include <TH2D.h>
#include "TTree.h"
#include <string>
#include <TMath.h>
#include <sstream>

using namespace std;
using namespace MuonGM;

//Function for histogram, AANtuple booking
void bookMDTHistograms(std::string);
void bookMDTChamberStats();
void mdtbookTree();

MDTHitsValAlg::MDTHitsValAlg(const std::string& name, 
			     ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  ValAlgInterface(),
  m_pMuonMgr           (0),
  m_pMdtIdHelper       (0),
  log( msgSvc(), name )
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
  declareProperty("StationSize",	  m_StationSize="XXX");
  declareProperty("StationEta",		  m_StationEta=-100);
  declareProperty("StationPhi",		  m_StationPhi=-100);  
  declareProperty("SpecRegion",		  m_SpecRegion=0);
  declareProperty("LastEvent",		  m_LastEvent=1);  
  declareProperty("WriteChamberGraphs",   m_writeChambers = true);
  declareProperty("GnaMonMode", 	  m_GnaMode = false);  
  
  log << MSG::INFO << "++++++++++++  MDTHitsValAlg created  ++++++++++++" << endreq;
}

MDTHitsValAlg::~MDTHitsValAlg()
{
  log << MSG::INFO << " deleting MDTHitsValAlg " << endreq;
}

StatusCode MDTHitsValAlg::initialize()
{
  log << MSG::DEBUG << " initializing MDTHitsValAlg " << endreq;
 
  /**Locate the StoreGateSvc and initialize our local ptr
     intitialize transient event store
  */
    
  StatusCode sc = StatusCode::SUCCESS;
 
  // Store Gate active store
  sc = serviceLocator()->service("StoreGateSvc", m_sgSvc); 
  if (sc.isFailure() || 0 == m_sgSvc) {
    log << MSG::ERROR << "MDTHitsValAlg: Could not find StoreGateSvc" << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Retrieved StoreGateSvc" << endreq;

  // retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    log << MSG::ERROR << " Cannot get ActiveStoreSvc " << endreq;
    return sc ;
  }

  // Initialize the IdHelper 
  StoreGateSvc* detStore=0;
  sc = service( "DetectorStore", detStore );
  if (sc.isFailure())   {
    log << MSG::ERROR << "DetectorStore service not found" << endreq; 
    return StatusCode::FAILURE;
  }

  // Retrieve the MuonDetectorManager  
  std::string managerName="Muon";
  sc = detStore->retrieve(m_pMuonMgr , "Muon");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Cannot get MuonDetectorManager from detector store    "  << managerName << " ! " << endreq;
    return StatusCode::FAILURE;
  }  
  log << MSG::DEBUG << " Found the MuonDetectorManager from detector store. " << endreq;
     
  sc = detStore->retrieve(m_pMdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure()) {
    log << MSG::FATAL << "Cannot get MdtIdHelper" << endreq;
    return StatusCode::FAILURE;
  }  
 
  log << MSG::DEBUG << " Found the MdtIdHelper " << endreq;
 	
  /// ROOT AANtuple Initialization---------------------------------------

  if (m_writeNtuple) {

    std::string mdtStreamAndPath = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
    std::string mdttreeName = m_ntupleTreeName;

    std::string mdttreePath = mdtStreamAndPath+"/"+mdttreeName;

    mdttree = new TTree(TString(mdttreeName), "Muon MDT Hits output");
      
    StatusCode status;
    status=ToolRootHistSvc()->regTree(mdttreePath, mdttree);

    if(status.isFailure()) {
      log << MSG::DEBUG << "MDTHitsValAlg:: Unable to register TTreeTuple : " << mdttreePath << endreq;
      return status;   
    }

  
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

    mdttree->Branch("m_Validation_MDT_NumberOfHits",  &m_Validation_MDT_NumberOfHits, "m_Validation_MDT_NumberOfHits/I");
    mdttree->Branch("m_Validation_MDT_EventNumber",  &m_Validation_MDT_EventNumber, "m_Validation_MDT_EventNumber/I");
    mdttree->Branch("m_Validation_MDT_Type",  &m_Validation_MDT_Type, "m_Validation_MDTidation_type/I");
    mdttree->Branch("m_Validation_MDT_RunNumber",  &m_Validation_MDT_RunNumber, "m_Validation_MDT_RunNumber/I");
    mdttree->Branch("m_Validation_MDT_HitX",  &m_Validation_MDT_HitX, "m_Validation_MDT_HitX/D");
    mdttree->Branch("m_Validation_MDT_HitY",  &m_Validation_MDT_HitY, "m_Validation_MDT_HitY/D");
    mdttree->Branch("m_Validation_MDT_HitZ",  &m_Validation_MDT_HitZ, "m_Validation_MDT_HitZ/D");
    mdttree->Branch("m_Validation_MDT_HitR",  &m_Validation_MDT_HitR, "m_Validation_MDT_HitR/D");


    mdttree->Branch("m_Validation_MDT_StationName", &m_Validation_MDT_StationName, "m_Validation_MDT_StationName/I");
    mdttree->Branch("m_Validation_MDT_StationEta",  &m_Validation_MDT_StationEta, "m_Validation_MDT_StationEta/I");
    mdttree->Branch("m_Validation_MDT_StationPhi",  &m_Validation_MDT_StationPhi, "m_Validation_MDT_StationPhi/I");		
    mdttree->Branch("m_Validation_MDT_IDMultiLayer",  &m_Validation_MDT_IDMultiLayer, "m_Validation_MDT_IDMultiLayer/I");
    mdttree->Branch("m_Validation_MDT_IDLayer",  &m_Validation_MDT_IDLayer, "m_Validation_MDT_IDLayer/I");   
    mdttree->Branch("m_Validation_MDT_IDTube",  &m_Validation_MDT_IDTube, "m_Validation_MDT_IDTube/I");
	
    //mdttree->Branch("m_Validation_MDT_LocalX",  &m_Validation_MDT_LocalX, "m_Validation_MDT_LocalX/D");
    //mdttree->Branch("m_Validation_MDT_LocalY",  &m_Validation_MDT_LocalY, "m_Validation_MDT_LocalY/D");
    //mdttree->Branch("m_Validation_MDT_LocalZ",  &m_Validation_MDT_LocalZ, "m_Validation_MDT_LocalZ/D");  
    //mdttree->Branch("m_Validation_MDT_LocalR",  &m_Validation_MDT_LocalR, "m_Validation_MDT_LocalR/D");
    



  }
 
  std::vector<std::string> hardware_name_list;
  hardware_name_list.push_back("XXX");
  event_counter=0;
  
  log << MSG::DEBUG << "MDTHitsValAlg:: Initialisation ended  " << endreq;

  return StatusCode::SUCCESS;

}


/// Execute - on event by event

StatusCode MDTHitsValAlg::execute()
{

  //StatusCode sc = StatusCode::SUCCESS;

  log << MSG::DEBUG << " MDTHitsValAlg:: execute " << endreq;  
  
    
  // ---------------------------------------
  // fill event data
  /** get EventInfo, used to obtain run and event number */
  const EventInfo* pevt;
		
	
  if (storeGateSvc()->retrieve(pevt).isFailure()) {
    log << MSG::FATAL << "Could not find event" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Found EventInfo in SG" << endreq;
  }	

  if(isVerbose())
    {
      log << MSG::VERBOSE <<"Processing EventInfo event #"<<pevt->event_ID()->event_number() << " run: " << pevt->event_ID()->run_number() << endreq;
    }


  /**Get number of events processed and run number*/
  //int evt = pevt->event_ID()->event_number();
  int numrun = pevt->event_ID()->run_number();
  m_Validation_MDT_RunNumber = numrun;

  /**Access MC truth information*/
  const DataHandle<McEventCollection> mcEvent;

  if(storeGateSvc()->retrieve(mcEvent,m_key).isFailure())
    {
      log << MSG::FATAL << "MDTHitsValAlg: Could not find MCevent" << endreq;
      return StatusCode::SUCCESS;
    }
  else {
    log << MSG::DEBUG << "MDTHitsValAlg: Found MCEvent" << endreq;
  }


  DataVector<HepMC::GenEvent>::const_iterator e;
  if (mcEvent->size()!=1) return StatusCode::SUCCESS;

  HepGeom::Vector3D<double> direction;
  for (e=mcEvent->begin();e!=mcEvent->end(); e++) {
    for (HepMC::GenEvent::particle_const_iterator p= (**e).particles_begin(); p!= (**e).particles_end(); p++)
      {
	direction.setX((**p).momentum().px());
	direction.setY((**p).momentum().py());
	direction.setZ((**p).momentum().pz());
	direction = direction.unit();
      }
  }



  /// ROOT histograms ---------------------------------------
 
  TH2 *mdtdet;
  TH2 *mdtlongview; 
  TH1 *mdtmultil;    
  TH1 *mdtlayer;    
  TH1 *mdttube;  
  TH1 *mdteta;
  TH1 *mdtphi;
  TH1 *mdtx;  
  TH1 *mdty;
  TH1 *mdtz;
  TH1 *mdtchamberstat;
  TH1 *mdtgnamontube = NULL;    

  /**Enter the main algorithm loop*/
  if (m_DoMDTTest) {
    const DataHandle<MDTSimHitCollection> p_collection;
    if (storeGateSvc()->retrieve(p_collection,"MDT_Hits")==StatusCode::FAILURE)
      {
	log << MSG::FATAL << "No MDTSimHitCollection in StoreGate!" << endreq;
	return StatusCode::FAILURE;
      }
    else if(storeGateSvc()->retrieve(p_collection,"MDT_Hits")==StatusCode::SUCCESS)
      {

	/**Enter MDT hits loop, initialize the hits counter*/
	counter_ValHitNumber =0 ;
	for(MDTSimHitCollection::const_iterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit)
	  {

	    /** Reminder:
		i_hit = pointer to an entry in vector<MDTSimHit>
		*i_hit= an entry in vector<MDTSimHit>, that is, a MDTSimHit */
	    /**Increment hits counter*/
	    ++counter_ValHitNumber;


	    /** Check the Hits identifiers, access the functions that give:
		Station name, station eta, station phi, multilayer ID, layer ID, tube ID.
		The values of these variables are written out to the AANtuple (variable content and range explained in the code section where AANTuple variables are
		registered)		
	    */		
	    HitID mdthit= i_hit->MDTid();
               
	    MdtHitIdHelper* mdthelper = MdtHitIdHelper::GetHelper();
	    log << MSG::DEBUG << "mdt_stname : " << mdthelper->GetStationName(mdthit) << endreq;
 	       
	    std::string mdt_stname = mdthelper->GetStationName(mdthit);
	    /**Function that gives int station name from string station name.Locatedin MuonSimValutils*/                      
	    int		mdt_stname_int = convertStrToIntName(mdt_stname,"MDT"); 
	    int         mdt_steta  = mdthelper->GetZSector(mdthit);
	    int         mdt_stphi  = mdthelper->GetPhiSector(mdthit);
	    int         mdt_ml     = mdthelper->GetMultiLayer(mdthit);
	    int         mdt_tl     = mdthelper->GetLayer(mdthit);
	    int         mdt_tube   = mdthelper->GetTube(mdthit);
	    
	    
	    
	    // 	    if (mdthelper->GetMultiLayer(mdthit)==2){
	    // 		     mdt_tube = mdt_tube + mdthelper->tubeMax(mdthit);
	    // 	    }
	    //	    log << MSG::INFO << "mdt_stname_int : " << mdt_stname_int << endreq;     

	    /**Create the offline identifiers, fill them with hit info.Ready to be accessed and retrieve info.
	       Currently not used in this code except for some checks.
	    */
	    Identifier offid = m_pMdtIdHelper->channelID(mdt_stname, mdt_steta, mdt_stphi,mdt_ml,mdt_tl,mdt_tube);
	    if (offid == 0)
	      {
		log << MSG::FATAL << "MDT: Cannot build a valid Identifier; skip " << endreq;
	      }
   
	    const MuonGM::MdtReadoutElement* descriptor = m_pMuonMgr->getMdtReadoutElement(offid);
	    if (descriptor == NULL)
	      {
		log << MSG::FATAL << "MDT readout element not found for Id = " << m_pMdtIdHelper->show_to_string(offid) << endreq;	 
    return StatusCode::FAILURE;
	      }
	    // -------- Testing Sim to Offline ID conversion ---------
	    Amg::Vector3D mdt_gPos   = descriptor->tubePos(offid);
	    //double mdt_tube_pos = sqrt(mdt_gPos.perp()*mdt_gPos.perp()+mdt_gPos.z()*mdt_gPos.z());
	    double mdt_tube_phi   = mdt_gPos.phi();
	    if (mdt_tube_phi < 0.) mdt_tube_phi += 2.*M_PI;
	    //double mdt_tube_perp  = mdt_gPos.perp();
	    //double mdt_tube_z   = mdt_gPos.z();
	    //double mdt_tube_cot   = 1./tan(mdt_gPos.theta());     
     
	    //Amg::Transform3D xf = descriptor->absTransform();
	    //Amg::Vector3D    xU = xf*Amg::Vector3D(i_hit->localPosition().x(),-i_hit->localPosition().z(), i_hit->localPosition().y());   
	    //Amg::Vector3D    gx = mdt_gPos + xU;
      
	    //Check Hits 
	    /**For every hit within the event, get the global position HepGeom::Point3D<double> u and then retrieve all releveant info
	       either from the HepGeom::Point3D<double> or from the MC vector (direction)
	    */		 
	    GeoMDTHit ghit(*i_hit);
   	
		
	    if (!ghit) continue;	
	    Amg::Vector3D u = ghit.getGlobalPosition();

 	
	    //	    event->Fill(evt);
	    //	    run->Fill(numrun);
	    double rad=sqrt(u.x()*u.x()+u.y()*u.y());
	    
	    //	    double mdt_phi = TMath::ATan(u.y()/u.x());
	    double mdt_phi = u.phi();
	    if (mdt_phi<0.) {mdt_phi += 2.*M_PI;} 
	    double mdt_theta = TMath::ATan(rad/u.z());
	    if (mdt_theta<0) {mdt_theta += M_PI;}
	    double mdt_eta = -(TMath::Log(TMath::Tan((mdt_theta/2.))));
 
	    etaregion=false;
	    if(m_SpecRegion==1) // Barrel region
	      {
		mdt_eta_cutmin=0; 
		mdt_eta_cutmax=0.9;
	      }else if(m_SpecRegion==2) // Overlap region 
		{
		  mdt_eta_cutmin=0.9;
		  mdt_eta_cutmax=1.2;
		}else if (m_SpecRegion==3) // Endcap region
		  {
		    mdt_eta_cutmin=1.2;
		    mdt_eta_cutmax=3.0;
		  }					
	    else if(m_SpecRegion==0) 
	      {
		mdt_eta_cutmin=0;
		mdt_eta_cutmax=3.0;
	      }
	    					    
	    if (fabs(mdt_eta)>mdt_eta_cutmin && fabs(mdt_eta)<mdt_eta_cutmax) {etaregion=true;}
	    
	    /**From function that convert MDT and RPC identifiers to harware names.Located in MuonSimValUtils*/
	    std::string hardware_name=convertChamberName(mdt_stname_int,mdt_steta,mdt_stphi,"MDT") ;

	    /**bool function, selects chamber to be plotted in histo file according to the user's 
	       selection in jobOpt.Located in MuonSimValUtils
	    */
  	    if ((selectChambersRange(hardware_name, m_chamberName, 
				     mdt_steta, m_StationEta,
				     mdt_stphi, m_StationPhi, m_StationSize)) && etaregion) {

	      if (mdt_ml==1){
	    	mdt_tl=mdt_tl+10;
	      } else if (mdt_ml==2){
	    	mdt_tl=mdt_tl+20;
		mdt_tube=mdt_tube+200;		
	      }
	      	      
	      if(m_writeChambers){
	    
	      std::string m_generic_path_mdtdetcxx = "/muonhits/MDT/"+hardware_name+"/mdthit_det";
	      m_generic_path_mdtdetcxx += hardware_name;
       	    
	      std::string m_generic_path_mdtlongviewcxx = "/muonhits/MDT/"+hardware_name+"/mdthit_longview";
	      m_generic_path_mdtlongviewcxx += hardware_name;

	      std::string m_generic_path_mdtmultilcxx = "/muonhits/MDT/"+hardware_name+"/mdthit_multil";
	      m_generic_path_mdtmultilcxx += hardware_name;

	      std::string m_generic_path_mdtlayercxx = "/muonhits/MDT/"+hardware_name+"/mdthit_layer";
	      m_generic_path_mdtlayercxx += hardware_name;

	      std::string m_generic_path_mdttubecxx = "/muonhits/MDT/"+hardware_name+"/mdthit_tube";
	      m_generic_path_mdttubecxx += hardware_name;

	      std::string m_generic_path_mdtetacxx = "/muonhits/MDT/"+hardware_name+"/mdthit_eta";
	      m_generic_path_mdtetacxx += hardware_name;

	      std::string m_generic_path_mdtphicxx = "/muonhits/MDT/"+hardware_name+"/mdthit_phi";
	      m_generic_path_mdtphicxx += hardware_name;

	      std::string m_generic_path_mdtxcxx = "/muonhits/MDT/"+hardware_name+"/mdthit_x";
	      m_generic_path_mdtxcxx += hardware_name;

	      std::string m_generic_path_mdtycxx = "/muonhits/MDT/"+hardware_name+"/mdthit_y";
	      m_generic_path_mdtycxx += hardware_name;

	      std::string m_generic_path_mdtzcxx = "/muonhits/MDT/"+hardware_name+"/mdthit_z";
	      m_generic_path_mdtzcxx += hardware_name;

	      std::string m_generic_path_mdtgnamoncxx = "/muonhits/MDT/"+hardware_name+"/mdthit_gnamon";
	      m_generic_path_mdtgnamoncxx += hardware_name;
	
	      //	log << MSG::INFO << "Before booking " << endreq;	    
	      histo_flag=true;
	      for (std::vector<std::string>::const_iterator iter=hardware_name_list.begin(); iter!=hardware_name_list.end(); iter++){
		if (hardware_name==*iter){histo_flag=false;}
	      }
	      if (histo_flag)
		{ 
		  bookMDTHistograms(hardware_name); 
		  hardware_name_list.push_back(hardware_name);
		}
	      //	log << MSG::INFO << "After booking " << endreq;
 	
	      if(tHistSvc()->getHist(m_generic_path_mdtdetcxx,mdtdet).isFailure()) return StatusCode::FAILURE; 
	      if(tHistSvc()->getHist(m_generic_path_mdtlongviewcxx,mdtlongview).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_mdtmultilcxx,mdtmultil).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_mdtlayercxx,mdtlayer).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_mdttubecxx,mdttube).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_mdtetacxx,mdteta).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_mdtphicxx,mdtphi).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_mdtxcxx,mdtx).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_mdtycxx,mdty).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_mdtzcxx,mdtz).isFailure()) return StatusCode::FAILURE;	    	    
	      if(tHistSvc()->getHist(m_generic_path_mdtgnamoncxx,mdtgnamontube).isFailure()) return StatusCode::FAILURE;
	      // 	    log << MSG::INFO << "u.x : " << u.x() << endreq;
	      // 	    log << MSG::INFO << "u.y : " << u.y() << endreq;
	      // 	    log << MSG::INFO << "u.z : " << u.z() << endreq;
	      // 	    log << MSG::INFO << "eta : " << mdt_eta << endreq;
	      // 	    log << MSG::INFO << "phi : " << mdt_phi << endreq;
	    
	      ///for MDTs that have barrel + endcap section, take only the barrel when plotting the xy-2d plot of the detector
	      if(fabs(mdt_eta)<m_BarrelEtaCut){
		mdtdet->Fill(u.x(),u.y());
	      }	    
	      mdtlongview->Fill(u.z(),rad);
	      mdtmultil->Fill(mdt_ml); 
	      mdtlayer->Fill(mdt_tl);  
	      mdttube->Fill(mdt_tube);  
	      mdteta->Fill(mdt_eta);
	      mdtphi->Fill(mdt_phi);
	      mdtx->Fill(u.x());  
	      mdty->Fill(u.y());
	      mdtz->Fill(u.z());
	      		
	      // 	    zResid->Fill(u.cross(direction).dot(direction.cross(HepGeom::Vector3D<double>(0,0,1)).unit()));
	      // 	    phiResid->Fill(u.cross(direction).z());

	      // 	    mdtzResid->Fill(u.cross(direction).dot(direction.cross(HepGeom::Vector3D<double>(0,0,1)).unit()));
	      // 	    mdtphiResid->Fill(u.cross(direction).z()); 
              }//end chamber writing if statement


	      /**AANtuple variables
		 Fill the AANtuple variables with a value obtained for each hit within the event
	      */
	      if (m_writeNtuple) {
	      m_Validation_MDT_Type=1;
	      //m_Validation_MDT_NumberOfHits=mdthit;
	      m_Validation_MDT_NumberOfHits=counter_ValHitNumber;
	      m_Validation_MDT_EventNumber=event_counter+1;				

	      m_Validation_MDT_HitX=u.x();
	      m_Validation_MDT_HitY=u.y();
	      m_Validation_MDT_HitZ=u.z();
	      m_Validation_MDT_HitR=rad;

	      m_Validation_MDT_StationName=mdt_stname_int;

	      //	      strcpy(m_Validation_MDT_StationName,(mdt_stname).c_str());  // put station name string in AANTUPLE as Val_StName  

	      m_Validation_MDT_StationEta=mdt_steta;
	      m_Validation_MDT_StationPhi=mdt_stphi;
	      m_Validation_MDT_IDTube=mdt_tube;
	      m_Validation_MDT_IDMultiLayer=mdt_ml;
	      m_Validation_MDT_IDLayer=mdt_tl;		


	      //m_Validation_MDT_LocalX=i_hit->localPosition().x();
	      //m_Validation_MDT_LocalY=i_hit->localPosition().y();
	      //m_Validation_MDT_LocalZ=i_hit->localPosition().z();
	      //m_Validation_MDT_LocalR=sqrt((i_hit->localPosition().x())*(i_hit->localPosition().x())+(i_hit->localPosition().y())*(i_hit->localPosition().y()));
 


	      /**After proccessing each hit, dump the info in the tree*/  
	      mdttree->Fill();
	      }	//end ntuple writing if statement	

	      //GnaMonMode histogramm creation

	      if(m_GnaMode) {

	    	  //basic function is tube_hardware = [ tube_software + (layer - 1)*max(number of tubes in layer)
		      //								 + (multilayer - 1)*max(number of tubes in one layer of previous multilayer)
	    	  //                                               * max(number of layers in previous multilayer) ]

	    	  //convert layer numbering from 1->4 to 1->8
	    	  //check if we are in 2nd multilayer
	    	  //then add 4 if large chamber, 3 if small chamber
	    	  int gna_layer = m_pMdtIdHelper->tubeLayer(offid);
	    	  if (m_pMdtIdHelper->multilayer(offid)==2) {
	    	    if ( hardware_name.at(1) == 'I' && hardware_name.at(3) != '8' )
	    	      gna_layer += 4;
	    	    else
	    	      gna_layer += 3;
	    	  }
	    	  int gna_tube= m_pMdtIdHelper->tube(offid) + (gna_layer-1) * m_pMdtIdHelper->tubeMax(offid);
	    	  //Correct for CutOuts
	    	  if(hardware_name.substr(0,4)=="BMS4" || hardware_name.substr(0,4)=="BMS6"){//layer 1-4 tubeId 41-48 cut out
	    	    if(gna_layer==1||gna_layer==2||gna_layer==3) gna_tube = m_pMdtIdHelper->tube(offid) + (gna_layer-1) * 48;
	    	  }
	    	  if(hardware_name.substr(0,3)=="BIR" && gna_layer <= 4){
	    	    if(hardware_name.substr(5,2)=="11"||hardware_name.substr(5,2)=="15") {
	    	      if(hardware_name.substr(3,1)=="1")gna_tube = m_pMdtIdHelper->tube(offid) + 6 + (gna_layer-1)*30;//layer 1-4 tube id 1-6 cut out
	    	      if(hardware_name.substr(3,1)=="2")gna_tube = m_pMdtIdHelper->tube(offid) + (gna_layer-1)*30;//layer 1-4 tube id 28-30 cut out
	    	      if(hardware_name.substr(3,1)=="4")gna_tube = m_pMdtIdHelper->tube(offid) + 3 + (gna_layer-1)*30;//layer 1-4 tube id 1-3 cut out
	    	      if(hardware_name.substr(3,1)=="5")gna_tube = m_pMdtIdHelper->tube(offid) + (gna_layer-1)*24;//layer 1-4 tube id 22-24 cut out
	    	    }
	    	  }

          if (!mdtgnamontube) {
            // Pointer is null - accoring to coverity could happen
            log << MSG::ERROR << "trying to dereference mdtgnamontube when NULL";
            return StatusCode::FAILURE;
          } else {
            mdtgnamontube->Fill(gna_tube);
          }

	      } // end GnaMonMode


	      map<string,int>::iterator iter_hitsperchamber = hitsperchamber_map.find(hardware_name);
	      if ( iter_hitsperchamber  == hitsperchamber_map.end() ){ 
	        hitsperchamber_map.insert( make_pair( hardware_name,1 ) );  
	      } else {iter_hitsperchamber->second+=1;}	  
	    
	    }/**Close chamber name loop*/
	  
	  }/**Close the MDTSimHitCollection loop*/
	
           
      }/**Close part where MDTSimHitCollection is accessed from the StoreGate*/
 
		
  } /**Close part of m_DoMDTTest*/

  ++event_counter;
  //  log << MSG::INFO << "event_counter : " << event_counter << endreq;
  
  if (event_counter==m_LastEvent) {
  	
    std::string m_generic_path_mdtchamberstatcxx = "/muonhits/MDT/mdthit_chamberstat";
		
    /**Writes hits per harware chamber in a txt file*/
    ofstream myfile;
    myfile.open ("mdthits.txt");
    myfile << "-------- Hit per Chamber Statistics--------\n";
	
    map<string,int>::iterator iter;
    bookMDTChamberStats();
    for( iter = hitsperchamber_map.begin(); iter != hitsperchamber_map.end(); iter++ ) {
      //bookMDTChamberStats();	
      if(tHistSvc()->getHist(m_generic_path_mdtchamberstatcxx,mdtchamberstat).isFailure()) return StatusCode::FAILURE;
	
      const char* chambername_char = iter->first.c_str ();
      /**Fills hits per harware chamber plot*/
      mdtchamberstat->Fill(chambername_char,iter->second);
		
      myfile << iter->first << "            |            " << iter->second << "\n";
    }
    myfile.close();
  }
  
  return StatusCode::SUCCESS;
  
}/**Close execute*/

 
StatusCode MDTHitsValAlg::finalize()
{
  log << MSG::DEBUG << " MDTHitsValAlg:: finalize " << endreq;     
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
ITHistSvc* MDTHitsValAlg::ToolRootHistSvc()
  /*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() ) 
    {
      log << MSG::WARNING << ">>> Unable to locate the MDTHitsValAlg Histogram service" << endreq;
    }
   
  return m_rootsvc;
} 

 
