/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHitTest/CSCHitsValAlg.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h" 

///for GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ITHistSvc.h"

#include "Identifier/Identifier.h"

#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h" 
#include "MuonReadoutGeometry/CscReadoutElement.h"
 
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHit.h"

#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonSimEvent/CscHitIdHelper.h"

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

//void bookHistograms();
void bookCSCHistograms(std::string);
void cscbookTree();
void bookCSCChamberStats();

CSCHitsValAlg::CSCHitsValAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  ValAlgInterface(),
  m_pMuonMgr           (0),
  m_pCscIdHelper       (0),			
  log( msgSvc(), name )
{
  // Set users' request
  /**Properties to be modified through the jobOption file
     Short description:
     Key for accessing MC truth
     Option to write out ntuple
     AANtuple file (linked to a specific file name in the jobOption)
     AANtuple directory name for CSC variables
     AANtuple tree name for CSC variables
     If true, enter the main algorithm loop
  */
  declareProperty("McEventKey",           m_key="TruthEvent");
  declareProperty("WriteNtuple",          m_writeNtuple = true);
  declareProperty("NtupleFileName",       m_ntupleFileName = "MuonHits");
  declareProperty("NtupleDirectoryName",  m_ntupleDirName = "CSCHitsValidation");
  declareProperty("NtupleTreeName",       m_ntupleTreeName = "CSCTestHits");
  declareProperty("DoCSCTest",            m_DoCSCTest=true);   
  declareProperty("WriteChamberGraphs",   m_writeChambers = true); 
  declareProperty("LastEvent",		  m_LastEvent=1);
  
  log << MSG::INFO << "++++++++++++  CSCHitsValAlg created  ++++++++++++"
      << endreq;
}

CSCHitsValAlg::~CSCHitsValAlg()
{
  log << MSG::INFO << " deleting CSCHitsValAlg " << endreq;
}

StatusCode CSCHitsValAlg::initialize()
{
  log << MSG::INFO << " initialize CSCHitsValAlg" << endreq;

  /**Locate the StoreGateSvc and initialize our local ptr
     intitialize transient event store
  */

  StatusCode sc = StatusCode::SUCCESS;

  // Store Gate active store
  sc = serviceLocator()->service("StoreGateSvc", m_sgSvc);
  if (sc.isFailure() || 0 == m_sgSvc) {
    log << MSG::ERROR << "CSCHitsValAlg: Could not retrieve StoreGateSvc!" << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Retrieved StoreGateSvc." << endreq;

  // retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ){
    log <<MSG::ERROR << "Cannot get AcitveStoreSvc"<<endreq;
    return sc;
  }

  // Initalize the IdHelper
  StoreGateSvc* detStore=0;
  sc = service( "DetectorStore", detStore );
  if ( sc.isFailure())  {
	    log << MSG::ERROR << "DetectorStore service not found" << endreq;
	    return StatusCode::FAILURE;
  }

  // Retrieve the MuonDetectorMananger
  std::string managerName="Muon";
  sc = detStore->retrieve(m_pMuonMgr , "Muon");
  if ( sc.isFailure() ){
	  log << MSG::ERROR <<"Cannot get MuonDetDescrMgr from detector store" << endreq;
	  return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << " Found the MuonDetectorManager from detector store. " << endreq;

  sc = detStore->retrieve(m_pCscIdHelper,"CSCIDHELPER");
  if (sc.isFailure()) {
      log << MSG::FATAL << "Cannot get CscIdHelper" << endreq;
      return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << " Found the CscIdHelper " << endreq;
 
  //bookCSCHistograms();

  /// ROOT AANtuple Initialization---------------------------------------

  if (m_writeNtuple) {

    std::string cscStreamAndPath = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
    std::string csctreeName = m_ntupleTreeName;
 
    std::string csctreePath = cscStreamAndPath+"/"+csctreeName;

    csctree = new TTree(TString(csctreeName), "Muon CSC Hits output");
       
    StatusCode status;
    status=ToolRootHistSvc()->regTree(csctreePath, csctree);

    if(status.isFailure()) {
      log << MSG::DEBUG << "CSCHitsValAlg:: Unable to register TTreeTuple : " <<
	csctreePath << endreq;
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
       CSC station name (char or int), conventional ATLAS naming scheme. Range: [CSS, CSL]
       CSC station eta. Range: [-1,1] (-1 backward endcap, 1 forward endcap)
       CSC station phi, starts from station 1 and increases with phi. Range: [1,8] 
       CSC chamber layer, a layer of physical chambers, increases with |Z|. Range [1,2] (seems to be always 1 i hits)	
       CSC wire layer, a wire plane inside a chamber, increases with |Z|. Range [1,4] (two gas volumes of two wire layers each)
       CSC measures phi, info not available in G4 hits!
       CSC strip, info not available in G4 hits!    
    */  

    csctree->Branch("m_Validation_CSC_NumberOfHits",  &m_Validation_CSC_NumberOfHits, "m_Validation_CSC_NumberOfHits/I");
    csctree->Branch("m_Validation_CSC_EventNumber",  &m_Validation_CSC_EventNumber, "m_Validation_CSC_EventNumber/I");
    csctree->Branch("m_Validation_CSC_Type",  &m_Validation_CSC_Type, "m_Validation_CSC_Type/I");
    csctree->Branch("m_Validation_CSC_RunNumber",  &m_Validation_CSC_RunNumber, "m_Validation_CSC_RunNumber/I");	 
    csctree->Branch("m_Validation_CSC_HitX",  &m_Validation_CSC_HitX, "m_Validation_CSC_HitX/D"); 
    csctree->Branch("m_Validation_CSC_HitY",  &m_Validation_CSC_HitY, "m_Validation_CSC_HitY/D"); 
    csctree->Branch("m_Validation_CSC_HitZ",  &m_Validation_CSC_HitZ, "m_Validation_CSC_HitZ/D"); 
    csctree->Branch("m_Validation_CSC_HitR",  &m_Validation_CSC_HitR, "m_Validation_CSC_HitR/D");
 
    csctree->Branch("m_Validation_CSC_StationName",  &m_Validation_CSC_StationName, "m_Validation_CSC_StationName/I");
    csctree->Branch("m_Validation_CSC_StationEta",  &m_Validation_CSC_StationEta, "m_Validation_CSC_StationEta/I"); 
    csctree->Branch("m_Validation_CSC_StationPhi",  &m_Validation_CSC_StationPhi, "m_Validation_CSC_StationPhi/I"); 
    csctree->Branch("m_Validation_CSC_ChamberLayer",  &m_Validation_CSC_ChamberLayer, "m_Validation_CSC_ChamberLayer/I");
    csctree->Branch("m_Validation_CSC_WireLayer",  &m_Validation_CSC_WireLayer, "m_Validation_CSC_WireLayer/I");
    csctree->Branch("m_Validation_CSC_MeasuresPhi",  &m_Validation_CSC_MeasuresPhi, "m_Validation_CSC_MeasuresPhi/I"); 
    csctree->Branch("m_Validation_CSC_Strip",  &m_Validation_CSC_Strip, "m_Validation_CSC_Strip/I");  
    
  }

  std::vector<std::string> hardware_name_list;
  hardware_name_list.push_back("XXX");
  event_counter=0;

  log << MSG::INFO << "CSCHitsValAlg:: Initialisation ended     " << endreq;
  return StatusCode::SUCCESS;

}  

StatusCode CSCHitsValAlg::finalize()
{
  log << MSG::DEBUG << " finalize " << endreq;
	
  return StatusCode::SUCCESS;
}

/// Execute - on event by event

StatusCode CSCHitsValAlg::execute()
{

  //StatusCode sc = StatusCode::SUCCESS;

  log << MSG::DEBUG << " CSCHitsValAlg:: execute " << endreq;


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
  m_Validation_CSC_RunNumber = numrun;
	
  /**Access MC truth information*/
  const DataHandle<McEventCollection> mcEvent;

  if(storeGateSvc()->retrieve(mcEvent,m_key).isFailure())
    {
      log << MSG::FATAL << "CSCHitsValAlg: Could not find MCevent" << endreq;
      return StatusCode::SUCCESS;
    }
  else {
    log << MSG::DEBUG << "CSCHitsValAlg: Found MCEvent" << endreq;
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

  //CSC
  /**Get the CSC histograms
     Short description:
     MuonSpectrometer RPC cross section 
     MuonSpectrometer RPC longitudinal 
     Eta, Theta, Phi, z residual (...), phi residual (...) of hits in CSCs
  */ 
  TH2 *cscdet;
  TH2 *csclong_view; 
  TH1 *csctheta;
  TH1 *csceta;
  TH1 *cscphi;
  TH1 *csczResid;
  TH1 *cscphiResid;
  TH1 *cscchamberstat;


  /**Enter the main algorithm loop*/
  if (m_DoCSCTest) {


    const DataHandle<CSCSimHitCollection> p_collection; 
    if (storeGateSvc()->retrieve(p_collection,"CSC_Hits")==StatusCode::FAILURE) 
      {
	log << MSG::FATAL << "No CSCSimHitCollection in StoreGate!" << endreq;
	return StatusCode::FAILURE;
      }
    else if(storeGateSvc()->retrieve(p_collection,"CSC_Hits")==StatusCode::SUCCESS)
      {
		
	/**Enter CSC hits loop, initialize the hits counter*/
	counter_ValHitNumber =0 ;
	for(CSCSimHitCollection::const_iterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit) 
	  {
		
	    /**Increment hits counter*/		
	    ++counter_ValHitNumber;


	    /** Check the Hits identifiers, access the functions that give:
		Station name, station eta, station phi, chamber layer, wire layer.
		The values of these variables are written out to the AANtuple (variable content and range explained in the code section where AANTuple variables are
		registered)		
	    */				
	    HitID cschit= i_hit->CSCid();	          
	    CscHitIdHelper* cschelper = CscHitIdHelper::GetHelper();
	    log << MSG::DEBUG << "csc_stname : " << cschelper->GetStationName(cschit) << endreq;
	    std::string csc_stname = cschelper->GetStationName(cschit);    
	    int		csc_stname_int = convertStrToIntName(csc_stname,"CSC");
	    int         csc_steta  = cschelper->GetZSector(cschit);     
	    int         csc_stphi  = cschelper->GetPhiSector(cschit);
	    int         csc_cl     = cschelper->GetChamberLayer(cschit);
	    int         csc_wl     = cschelper->GetWireLayer(cschit);
	    int         csc_measphi = 1;
	    int         csc_strip =  1;

      
	    /**Create the offline identifiers, fill them with hit info.Ready to be accessed and retrieve info.
	       Currently not used in this code except for some checks.
	    */		            
			    
	    Identifier offid = m_pCscIdHelper->channelID(csc_stname, csc_steta, csc_stphi, csc_cl, csc_wl, csc_measphi, csc_strip);
	    if (offid == 0)
	      {
		log << MSG::FATAL << "CSC: Cannot build a valid Identifier; skip " << endreq;
	      }
	    //log << MSG::INFO << " CSC: Offline Id "<<m_pCscIdHelper->show_to_string(offid) << endreq;

	    const MuonGM::CscReadoutElement* descriptor = m_pMuonMgr->getCscReadoutElement(offid);
	    if (descriptor == NULL)
	      {
		log << MSG::FATAL << "CSC readout element not found for Id = " << m_pCscIdHelper->show_to_string(offid) << endreq;
		HepGeom::Point3D<double>(0,0,0);
	      }

	    // -------- Testing Sim to Offline ID conversion ---------
     
	    Amg::Vector3D csc_gPos   = descriptor->stripPos(offid);
	    double csc_strip_phi   = csc_gPos.phi();
	    if (csc_strip_phi < 0.) csc_strip_phi += 2.*M_PI;
	    //double csc_strip_perp  = csc_gPos.perp();
	    //double csc_strip_z   = csc_gPos.z();
	    //double csc_strip_cot   = 1./tan(csc_gPos.theta());

	    Amg::Vector3D xU;
	    Amg::Vector3D localHit(i_hit->getHitStart().x(),i_hit->getHitStart().y(),i_hit->getHitStart().z());
	    xU = descriptor->localToGlobalCoords(localHit,offid);

		 
	    //Check Hits 		
	    /**For every hit within the event, get the global position HepGeom::Point3D<double> u and then retrieve all releveant info
	       either from the HepGeom::Point3D<double> or from the MC vector (direction)
	    */				
	    GeoCSCHit ghit(*i_hit);
	    if (!ghit) continue;	
	    Amg::Vector3D u = ghit.getGlobalPosition();

	    //event->Fill(evt);
	    //run->Fill(numrun);		
	    double rad=sqrt(u.x()*u.x()+u.y()*u.y());

	    std::string hardware_name=convertChamberName(csc_stname_int,csc_steta,csc_stphi,"CSC") ;

            if(m_writeChambers){

	      std::string m_generic_path_cscdetcxx = "/muonhits/CSC/"+hardware_name+"/cschit_det";
	      m_generic_path_cscdetcxx += hardware_name;

	      std::string m_generic_path_csclongviewcxx = "/muonhits/CSC/"+hardware_name+"/cschit_longview";
	      m_generic_path_csclongviewcxx += hardware_name;

	      std::string m_generic_path_cscthetacxx = "/muonhits/CSC/"+hardware_name+"/cschit_theta";
	      m_generic_path_cscthetacxx += hardware_name;

	      std::string m_generic_path_cscetacxx = "/muonhits/CSC/"+hardware_name+"/cschit_eta";
	      m_generic_path_cscetacxx += hardware_name;

	      std::string m_generic_path_cscphicxx = "/muonhits/CSC/"+hardware_name+"/cschit_phi";
	      m_generic_path_cscphicxx += hardware_name;

	      std::string m_generic_path_csczResidcxx = "/muonhits/CSC/"+hardware_name+"/cschit_zResid";
	      m_generic_path_csczResidcxx += hardware_name;

	      std::string m_generic_path_cscphiResidcxx = "/muonhits/CSC/"+hardware_name+"/cschit_phiResid";
	      m_generic_path_cscphiResidcxx += hardware_name;

	      histo_flag=true;
	      for (std::vector<std::string>::const_iterator iter=hardware_name_list.begin(); iter!=hardware_name_list.end(); iter++){
		  if (hardware_name==*iter){histo_flag=false;}
	      }

	      if (histo_flag){ bookCSCHistograms(hardware_name);
	      hardware_name_list.push_back(hardware_name);
	      }
	      //	log << MSG::INFO << "After booking " << endreq;


	      if(tHistSvc()->getHist(m_generic_path_cscdetcxx,cscdet).isFailure()) return StatusCode::FAILURE;
	      if(tHistSvc()->getHist(m_generic_path_csclongviewcxx,csclong_view).isFailure()) return StatusCode::FAILURE;
	      if(tHistSvc()->getHist(m_generic_path_cscthetacxx,csctheta).isFailure()) return StatusCode::FAILURE;
	      if(tHistSvc()->getHist(m_generic_path_cscetacxx,csceta).isFailure()) return StatusCode::FAILURE;
	      if(tHistSvc()->getHist(m_generic_path_cscphicxx,cscphi).isFailure()) return StatusCode::FAILURE;
	      if(tHistSvc()->getHist(m_generic_path_csczResidcxx,csczResid).isFailure()) return StatusCode::FAILURE;
	      if(tHistSvc()->getHist(m_generic_path_cscphiResidcxx,cscphiResid).isFailure()) return StatusCode::FAILURE;

	    cscdet->Fill(u.x(),u.y());
	    csclong_view->Fill(u.z(),rad);
	    csctheta->Fill(direction.theta());
	    csceta->Fill(direction.eta());
	    cscphi->Fill(direction.phi());
	    csczResid->Fill(u.cross(direction).z());
	    cscphiResid->Fill(u.cross(direction).z()); 

            }//end chamber writing if statement

	    /**AANtuple variables
	       Fill the AANtuple variables with a value obtained for each hit within the event
	    */		
	    if (m_writeNtuple) {
	    m_Validation_CSC_Type = 1; //(1: Hit, 2: Digit, 3: Reco)
	    m_Validation_CSC_NumberOfHits = counter_ValHitNumber;
	    m_Validation_CSC_EventNumber=evt;;
	    m_Validation_CSC_HitX = u.x();
	    m_Validation_CSC_HitY = u.y();
	    m_Validation_CSC_HitZ = u.z();
	    m_Validation_CSC_HitR = rad;
	    m_Validation_CSC_StationName = csc_stname_int;
	    // strcpy(m_Validation_CSC_StationName,(csc_stname).c_str());  // put station name string in AANTUPLE as Val_StName  
	    m_Validation_CSC_StationEta = csc_steta;
	    m_Validation_CSC_StationPhi = csc_stphi;		
	    m_Validation_CSC_ChamberLayer = csc_cl;
	    m_Validation_CSC_WireLayer = csc_wl;
	    m_Validation_CSC_MeasuresPhi = csc_measphi;

	    /**After proccessing each hit, dump the info in the tree*/
	    csctree->Fill();
	    }	//end ntuple writing if statement
    
            map<string,int>::iterator iter_hitsperchamber = hitsperchamber_map.find(hardware_name);
	    if ( iter_hitsperchamber  == hitsperchamber_map.end() ){
	      hitsperchamber_map.insert( make_pair( hardware_name,1 ) );
	    } else {iter_hitsperchamber->second+=1;}

	  }/**Close the CSCSimHitCollection loop*/
		
      }/**Close part where CSCSimHitCollection is accessed from the StoreGate*/
	 
            
  }/**Close part of m_DoCSCTest*/

  ++event_counter;
  //  log << MSG::INFO << "event_counter : " << event_counter << endreq;

  if (event_counter==m_LastEvent) {

    std::string m_generic_path_cscchamberstatcxx = "/muonhits/CSC/cschit_chamberstat";

    /**Writes hits per harware chamber in a txt file*/
    ofstream myfile;
    myfile.open ("cschits.txt");
    myfile << "-------- Hit per Chamber Statistics--------\n";

    map<string,int>::iterator iter;
    bookCSCChamberStats();
    for( iter = hitsperchamber_map.begin(); iter != hitsperchamber_map.end(); iter++ ) {
      //bookCSCChamberStats();
      if(tHistSvc()->getHist(m_generic_path_cscchamberstatcxx,cscchamberstat).isFailure()) return StatusCode::FAILURE;

      const char* chambername_char = iter->first.c_str ();
      /**Fills hits per hardware chamber plot*/
      cscchamberstat->Fill(chambername_char,iter->second);
    
      myfile << iter->first << "            |            " << iter->second << "\n";   
    }
    myfile.close();
  }

  return StatusCode::SUCCESS;    

}/**Close execute*/
/*---------------------------------------------------------*/
ITHistSvc* CSCHitsValAlg::ToolRootHistSvc()
  /*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() ) {
    log << MSG::WARNING << ">>> Unable to locate the CSCHitsValAlg Histogram service" << endreq;
  }
   
  return m_rootsvc;
} 



