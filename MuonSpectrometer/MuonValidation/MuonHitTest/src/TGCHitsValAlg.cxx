/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHitTest/TGCHitsValAlg.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ITHistSvc.h"

#include "Identifier/Identifier.h"

#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h" 
#include "MuonReadoutGeometry/TgcReadoutElement.h"
 
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHit.h"

#include "MuonSimEvent/TgcHitIdHelper.h" 
#include "MuonIdHelpers/TgcIdHelper.h"

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

void bookTGCHistograms(std::string);
void tgcbookTree();
void bookTGCChamberStats();

TGCHitsValAlg::TGCHitsValAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  ValAlgInterface(),
  m_pMuonMgr           (0),
  m_pTgcIdHelper       (0),				
  log( msgSvc(), name )
{
  // Set users' request
  /**Properties to be modified through the jobOption file
     Short description:
     Key for accessing MC truth
     Option to write out ntuple
     AANtuple file (linked to a specific file name in the jobOption)
     AANtuple directory name for RPC variables
     AANtuple tree name for RPC variables
     If true, enter the main algorithm loop
  */
  declareProperty("McEventKey",           m_key="TruthEvent");
  declareProperty("WriteNtuple",          m_writeNtuple = true);
  declareProperty("NtupleFileName",       m_ntupleFileName = "MuonHits");
  declareProperty("NtupleDirectoryName",  m_ntupleDirName = "TGCHitsValidation");
  declareProperty("NtupleTreeName",       m_ntupleTreeName = "TGCTestHits");
  declareProperty("DoTGCTest",            m_DoTGCTest=true);    
  declareProperty("WriteChamberGraphs",   m_writeChambers = true);
  declareProperty("LastEvent",		  m_LastEvent=1);
      	
  log << MSG::INFO << "++++++++++++  TGCHitsValAlg created  ++++++++++++"
      << endreq;
}

TGCHitsValAlg::~TGCHitsValAlg()
{
  log << MSG::INFO << " deleting TGCHitsValAlg " << endreq;
}

StatusCode TGCHitsValAlg::initialize()
{
  log << MSG::INFO << " initialize TGCHitsValAlg " << endreq;

  /**Locate the StoreGateSvc and initialize our local ptr
     intitialize transient event store
  */

  StatusCode sc = StatusCode::SUCCESS;
  
  // Store Gate active store
  sc = serviceLocator()->service("StoreGateSvc", m_sgSvc);
  if (sc.isFailure()||0==m_sgSvc) {
    log << MSG::ERROR << "TGCHitsValAlg: Could not find StoreGateSvc!" << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Retrieved StoreGateSvc." << endreq;

  //retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc",m_activeStore);
  if(sc!=StatusCode::SUCCESS){
	  log<<MSG::ERROR<<"Cannot get ActiveStoreSvc"<<endreq;
	  return sc;
  }

  //Initialize the IdHelper          			
  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore );
  if (sc.isFailure()){
    log <<MSG::ERROR << "Could not get DetectorStore"<<endreq;
    return StatusCode::FAILURE;
  }

  //Retrieve the MuonDetectorManager
  std::string managerName="Muon";
  sc = detStore->retrieve(m_pMuonMgr, "Muon");
  if ( sc.isFailure() ){
	  log << MSG::ERROR << " TGCHitsValAlg::initialize() : MuonDetDescrMgr not found in DetectorStore " << endreq;
  	  return StatusCode::FAILURE;
  }
  log<<MSG::DEBUG<< " Found the MuonDetDescrMgr in DetectorStore"<<endreq;

  sc=detStore->retrieve(m_pTgcIdHelper,"TGCIDHELPER");
  if (sc.isFailure() ){
	  log<<MSG::FATAL<<"Cannot get TgcIdHelper"<<endreq;
	  return StatusCode::FAILURE;
  }

  log<<MSG::DEBUG<<"Found the TgcIdHelper"<<endreq;
 
  //bookTGCHistograms();

  /// ROOT AANtuple Initialization---------------------------------------

  if (m_writeNtuple) {

    std::string tgcStreamAndPath = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
    std::string tgctreeName = m_ntupleTreeName;
 
    std::string tgctreePath = tgcStreamAndPath+"/"+tgctreeName;

    tgctree = new TTree(TString(tgctreeName), "Muon TGC Hits output");
       
    StatusCode status;
    status=ToolRootHistSvc()->regTree(tgctreePath, tgctree);

    if(status.isFailure()) {
      log << MSG::DEBUG << "TGCHitsValAlg:: Unable to register TTreeTuple : " <<
	tgctreePath << endreq;
      return status;   
    }

  
    //-----------------
    /**add items-variables to the validation AANTUPLE
       Short description:
       Total number of G4 hits in AANTUPLE
       Total number of events processed
       Type 1=Hits , 2=Digits , 3=Prepraw
       Run number, event specific ID
       Global X, Y, Z, R of hit (with respect to the interaction point)
       TGC station name (char or int), conventional ATLAS naming scheme. Range: [T1E, T2E, T3E, T4E, T1F, T2F, T3F, T4F]
       TGC station eta, increases with Z. Range: [-5,5] ([-5,-1] backward endcap with -1 at lowest R, [1,5] forward endcap with 1 at lowest R)
       TGC station phi, increases with phi. Range: [1,48] ([1,48] for endcap, [1,24] for forward)
       TGC GasGap, the gas volume within a TGC chamber, increases with |Z|. Range: [1,3] ([1,2] for Doublets, [1,3] for Triplets)
       TGC Doublet MeasuresPhi, info not available in G4 hits!
       TGC IsStrip, info not available in G4 hits!	
    */

    tgctree->Branch("m_Validation_TGC_NumberOfHits",  &m_Validation_TGC_NumberOfHits, "m_Validation_TGC_NumberOfHits/I");
    tgctree->Branch("m_Validation_TGC_EventNumber",  &m_Validation_TGC_EventNumber, "m_Validation_TGC_EventNumber/I");
    tgctree->Branch("m_Validation_TGC_Type",  &m_Validation_TGC_Type, "m_Validation_TGC_Type/I");
    tgctree->Branch("m_Validation_TGC_RunNumber",  &m_Validation_TGC_RunNumber, "m_Validation_TGC_RunNumber/I");	 
    tgctree->Branch("m_Validation_TGC_HitX",  &m_Validation_TGC_HitX, "m_Validation_TGC_HitX/D"); 
    tgctree->Branch("m_Validation_TGC_HitY",  &m_Validation_TGC_HitY, "m_Validation_TGC_HitY/D"); 
    tgctree->Branch("m_Validation_TGC_HitZ",  &m_Validation_TGC_HitZ, "m_Validation_TGC_HitZ/D"); 
    tgctree->Branch("m_Validation_TGC_HitR",  &m_Validation_TGC_HitR, "m_Validation_TGC_HitR/D");
 
    tgctree->Branch("m_Validation_TGC_StationName",  &m_Validation_TGC_StName, "m_Validation_TGC_StName/I");
    tgctree->Branch("m_Validation_TGC_StationEta",  &m_Validation_TGC_StationEta, "m_Validation_TGC_StationEta/I"); 
    tgctree->Branch("m_Validation_TGC_StationPhi",  &m_Validation_TGC_StationPhi, "m_Validation_TGC_StationPhi/I"); 
    tgctree->Branch("m_Validation_TGC_GasGap",  &m_Validation_TGC_GasGap, "m_Validation_TGC_GasGap/I");
    tgctree->Branch("m_Validation_TGC_IsStrip",  &m_Validation_TGC_IsStrip, "m_Validation_TGC_IsStrip/I");
    tgctree->Branch("m_Validation_TGC_Channel",  &m_Validation_TGC_Channel, "m_Validation_TGC_Channel/I");
 
    //tgctree->Branch("m_Validation_TGC_LocalX",  &m_Validation_TGC_LocalX, "m_Validation_TGC_LocalX/D");
    //tgctree->Branch("m_Validation_TGC_LocalY",  &m_Validation_TGC_LocalY, "m_Validation_TGC_LocalY/D");
    //tgctree->Branch("m_Validation_TGC_LocalZ",  &m_Validation_TGC_LocalZ, "m_Validation_TGC_LocalZ/D");
    //tgctree->Branch("m_Validation_TGC_LocalR",  &m_Validation_TGC_LocalR, "m_Validation_TGC_LocalR/D");
 
  }

  std::vector<std::string> hardware_name_list;
  hardware_name_list.push_back("XXX");
  event_counter=0;

  log << MSG::INFO << "Initialisation ended     " << endreq;
  return StatusCode::SUCCESS;

} 

StatusCode TGCHitsValAlg::finalize()
{
  log << MSG::DEBUG << " finalize " << endreq;
	
  return StatusCode::SUCCESS;
}


/// Execute - on event by event

StatusCode TGCHitsValAlg::execute()
{

  //StatusCode sc= StatusCode::SUCCESS;

  log << MSG::DEBUG << " TGCHitsValAlg:: execute " << endreq;

	
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
  int evt = pevt->event_ID()->event_number();
  int numrun = pevt->event_ID()->run_number();
  m_Validation_TGC_RunNumber = numrun;	

  /**Access MC truth information*/
  const DataHandle<McEventCollection> mcEvent;
  
  if(storeGateSvc()->retrieve(mcEvent,m_key).isFailure())
    {
      log << MSG::FATAL << "TGCHitsValAlg: Could not find MCevent" << endreq;
      return StatusCode::SUCCESS;
    }
  else {
    log << MSG::DEBUG << "TGCHitsValAlg: Found MCEvent" << endreq;
  }	   
  

  DataVector<HepMC::GenEvent>::const_iterator e;
  if (mcEvent->size()!=1) return StatusCode::SUCCESS;
 
  Amg::Vector3D direction;
  for (e=mcEvent->begin();e!=mcEvent->end(); e++) {
    for (HepMC::GenEvent::particle_const_iterator p= (**e).particles_begin(); p!= (**e).particles_end(); p++) 
      {
	direction.x() = (**p).momentum().px();
	direction.y() = (**p).momentum().py();
	direction.z() = (**p).momentum().pz();
	direction.normalize();
      }	
  }




  /// ROOT histograms ---------------------------------------	

  TH2 *tgcdet;
  TH2 *tgclong_view; 
  TH1 *tgctheta;
  TH1 *tgceta;
  TH1 *tgcphi;
  TH1 *tgczResid;
  TH1 *tgcphiResid;
  TH1 *tgcchamberstat;
		
  /**Enter the main algorithm loop*/
  if (m_DoTGCTest) {

				
    const DataHandle<TGCSimHitCollection> p_collection; 
    if (storeGateSvc()->retrieve(p_collection,"TGC_Hits")==StatusCode::FAILURE) 
      {
	log << MSG::FATAL << "No TGCSimHitCollection in StoreGate!" << endreq;
	return StatusCode::FAILURE;
      }
    else if(storeGateSvc()->retrieve(p_collection,"TGC_Hits")==StatusCode::SUCCESS)
      {

	/**Enter TGC hits loop, initialize the hits counter*/	
	counter_ValHitNumber =0 ;
	for(TGCSimHitCollection::const_iterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit) 
	  {

	    /**Increment hits counter*/
	    ++counter_ValHitNumber;
		

	    /** Check the Hits identifiers, access the functions that give:
		Station name, station eta, station phi, doublet Z, doublet phi, doublet R, GasGap, Measures Phi.
		The values of these variables are written out to the AANtuple (variable content and range explained in the code section where AANTuple variables are
		registered)		
	    */			
	    HitID tgchit= i_hit->TGCid();	          
	    TgcHitIdHelper* tgchelper = TgcHitIdHelper::GetHelper();
	    log << MSG::DEBUG << "tgc_stname : " << tgchelper->GetStationName(tgchit) << endreq;
	    
	    std::string tgc_stname = tgchelper->GetStationName(tgchit);
	    /**Function that gives int station name from string station name.Locatedin MuonSimValutils*/
	    int		tgc_stname_int = convertStrToIntName(tgc_stname,"TGC");
	    int         tgc_steta  = tgchelper->GetStationEta(tgchit);
	    int         tgc_stphi  = tgchelper->GetStationPhi(tgchit);
	    int         tgc_gg     = tgchelper->GetGasGap(tgchit); 
	    int         tgc_strip  = 1;//tgchelper->GetIsStrip(tgchit);
	    int         tgc_channel = 1;//tgchelper->GetChannel(tgchit);
     
 
	    /**Create the offline identifiers, fill them with hit info.Ready to be accessed and retrieve info.
	       Currently not used in this code except for some checks.
	    */
		            
	    Identifier offid = m_pTgcIdHelper->channelID(tgc_stname, tgc_steta, tgc_stphi, tgc_gg, tgc_strip, tgc_channel);
	    if (offid == 0)
	      {
		log << MSG::FATAL << "TGC: Cannot build a valid Identifier; skip " << endreq;
	      }
	    log << MSG::DEBUG << " TGC: Offline Id "<<m_pTgcIdHelper->show_to_string(offid) << endreq;
    
	    const MuonGM::TgcReadoutElement* descriptor = m_pMuonMgr->getTgcReadoutElement(offid);
	    if (descriptor == NULL)
	      {
		log << MSG::FATAL << "TGC readout element not found for Id = " << m_pTgcIdHelper->show_to_string(offid) << endreq;	 
		HepGeom::Point3D<double>(0,0,0);     
	      }
	    // -------- Testing Sim to Offline ID conversion ---------
     
	    Amg::Vector3D tgc_gPos   = descriptor->channelPos(offid);
	    double tgc_strip_phi   = tgc_gPos.phi();
	    if (tgc_strip_phi < 0.) tgc_strip_phi += 2.*M_PI;
	    //double tgc_strip_perp  = tgc_gPos.perp();
	    //double tgc_strip_z   = tgc_gPos.z();
	    //double tgc_strip_cot   = 1./tan(tgc_gPos.theta());

	    Amg::Vector3D xU;
	    Amg::Vector3D localHit(i_hit->localPosition().x(),i_hit->localPosition().y(),i_hit->localPosition().z());
	    xU = descriptor->localToGlobalCoords(localHit,offid);
		 
	    //Check Hits 
	    /**For every hit within the event, get the global position HepGeom::Point3D<double> u and then retrieve all releveant info
	       either from the HepGeom::Point3D<double> or from the MC vector (direction)
	    */
	    GeoTGCHit ghit(*i_hit);
	    if (!ghit) continue;	
	    Amg::Vector3D u = ghit.getGlobalPosition();
		
	    //event->Fill(evt);
	    //run->Fill(numrun);
	    double rad=sqrt(u.x()*u.x()+u.y()*u.y());
	    std::string hardware_name=convertChamberName(tgc_stname_int,tgc_steta,tgc_stphi,"TGC") ;

            if(m_writeChambers){

	      std::string m_generic_path_tgcdetcxx = "/muonhits/TGC/"+hardware_name+"/tgchit_det";
	      m_generic_path_tgcdetcxx += hardware_name;

	      std::string m_generic_path_tgclongviewcxx = "/muonhits/TGC/"+hardware_name+"/tgchit_longview";
	      m_generic_path_tgclongviewcxx += hardware_name;

	      std::string m_generic_path_tgcthetacxx = "/muonhits/TGC/"+hardware_name+"/tgchit_theta";
	      m_generic_path_tgcthetacxx += hardware_name;

	      std::string m_generic_path_tgcetacxx = "/muonhits/TGC/"+hardware_name+"/tgchit_eta";
	      m_generic_path_tgcetacxx += hardware_name;

	      std::string m_generic_path_tgcphicxx = "/muonhits/TGC/"+hardware_name+"/tgchit_phi";
	      m_generic_path_tgcphicxx += hardware_name;

	      std::string m_generic_path_tgczResidcxx = "/muonhits/TGC/"+hardware_name+"/tgchit_zResid";
	      m_generic_path_tgczResidcxx += hardware_name;

	      std::string m_generic_path_tgcphiResidcxx = "/muonhits/TGC/"+hardware_name+"/tgchit_phiResid";
	      m_generic_path_tgcphiResidcxx += hardware_name;

	    histo_flag=true;
	    for (std::vector<std::string>::const_iterator iter=hardware_name_list.begin(); iter!=hardware_name_list.end(); iter++){
	    if (hardware_name==*iter){histo_flag=false;}
	    }

	    if (histo_flag){ bookTGCHistograms(hardware_name);
	    hardware_name_list.push_back(hardware_name);
	    }

	    if(tHistSvc()->getHist(m_generic_path_tgcdetcxx,tgcdet).isFailure()) return StatusCode::FAILURE;
	    if(tHistSvc()->getHist(m_generic_path_tgclongviewcxx,tgclong_view).isFailure()) return StatusCode::FAILURE;
	    if(tHistSvc()->getHist(m_generic_path_tgcthetacxx,tgctheta).isFailure()) return StatusCode::FAILURE;
	    if(tHistSvc()->getHist(m_generic_path_tgcetacxx,tgceta).isFailure()) return StatusCode::FAILURE;
	    if(tHistSvc()->getHist(m_generic_path_tgcphicxx,tgcphi).isFailure()) return StatusCode::FAILURE;
	    if(tHistSvc()->getHist(m_generic_path_tgczResidcxx,tgczResid).isFailure()) return StatusCode::FAILURE;
	    if(tHistSvc()->getHist(m_generic_path_tgcphiResidcxx,tgcphiResid).isFailure()) return StatusCode::FAILURE;

	    tgcdet->Fill(u.x(),u.y());
	    tgclong_view->Fill(u.z(),rad);
	    tgctheta->Fill(direction.theta());
	    tgceta->Fill(direction.eta());
	    tgcphi->Fill(direction.phi());
	    tgczResid->Fill(u.cross(direction).dot(direction.cross(Amg::Vector3D(0,0,1)).normalized()));
	    tgcphiResid->Fill(u.cross(direction).z()); 		
	    }//end chamber writing if statement

	    /**AANtuple variables
	       Fill the AANtuple variables with a value obtained for each hit within the event
	    */
	    if (m_writeNtuple) {
	    m_Validation_TGC_Type = 1; //(1: Hit, 2: Digit, 3: Reco)
	    m_Validation_TGC_NumberOfHits = counter_ValHitNumber;
	    m_Validation_TGC_EventNumber = evt;
	    m_Validation_TGC_HitX = u.x();
	    m_Validation_TGC_HitY = u.y();
	    m_Validation_TGC_HitZ = u.z();
	    m_Validation_TGC_HitR = rad;
	    m_Validation_TGC_StName = tgc_stname_int;
	    //strcpy(m_Validation_TGC_StName,(tgc_stname).c_str());  // put station name string in AANTUPLE as Val_StName  
	    m_Validation_TGC_StationEta = tgc_steta;
	    m_Validation_TGC_StationPhi = tgc_stphi;		
	    m_Validation_TGC_GasGap = tgc_gg;
	    m_Validation_TGC_IsStrip = tgc_strip;
	    m_Validation_TGC_Channel = tgc_channel;

	    //m_Validation_TGC_LocalX=i_hit->localPosition().x();
	    //m_Validation_TGC_LocalY=i_hit->localPosition().y();
	    //m_Validation_TGC_LocalZ=i_hit->localPosition().z();
	    //m_Validation_TGC_LocalR=sqrt((i_hit->localPosition().x())*(i_hit->localPosition().x())+(i_hit->localPosition().y())*(i_hit->localPosition().y()));

	    /**After proccessing each hit, dump the info in the tree*/
	    tgctree->Fill();		
	    }	//end ntuple writing if statement
	    
	    map<string,int>::iterator iter_hitsperchamber = hitsperchamber_map.find(hardware_name);
	    if ( iter_hitsperchamber  == hitsperchamber_map.end() ){
	       hitsperchamber_map.insert( make_pair( hardware_name,1 ) );
	    } else {iter_hitsperchamber->second+=1;}

	  }/**Close the TGCSimHitCollection loop*/
	
    
      }/**Close part where TGCSimHitCollection is accessed from the StoreGate*/	
    
  }/**Close part of m_DoRPCTest*/

  ++event_counter;

  if (event_counter==m_LastEvent) {

    std::string m_generic_path_tgcchamberstatcxx = "/muonhits/TGC/tgchit_chamberstat";

    /**Writes hits per harware chamber in a txt file*/
    ofstream myfile;
    myfile.open ("tgchits.txt");
    myfile << "-------- Hit per Chamber Statistics--------\n";

    map<string,int>::iterator iter;
    bookTGCChamberStats();
    for( iter = hitsperchamber_map.begin(); iter != hitsperchamber_map.end(); iter++ ) {
      //bookTGCChamberStats();
      if(tHistSvc()->getHist(m_generic_path_tgcchamberstatcxx,tgcchamberstat).isFailure()) return StatusCode::FAILURE;

      const char* chambername_char = iter->first.c_str ();
      /**Fills hits per hardware chamber plot*/
      tgcchamberstat->Fill(chambername_char,iter->second);
    
      myfile << iter->first << "            |            " << iter->second << "\n";
    }
    myfile.close();
  }

  return StatusCode::SUCCESS;
	
}/**Close execute*/
/*---------------------------------------------------------*/
ITHistSvc* TGCHitsValAlg::ToolRootHistSvc()
  /*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() ) {
    log << MSG::WARNING << ">>> Unable to locate the TGCHitsValAlg Histogram service" << endreq;
  }
   
  return m_rootsvc;
} 



