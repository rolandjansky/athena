/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHitTest/RPCHitsValAlg.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h" 

///for GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ITHistSvc.h"

#include "Identifier/Identifier.h"

#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h" 
#include "MuonReadoutGeometry/RpcReadoutElement.h"
 
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/RPCSimHit.h"
 
#include "MuonSimEvent/RpcHitIdHelper.h" 
#include "MuonIdHelpers/RpcIdHelper.h"

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

void bookRPCHistograms(std::string);
void bookRPCChamberStats();
void rpcbookTree();

RPCHitsValAlg::RPCHitsValAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  ValAlgInterface(),
  m_pMuonMgr           (0),
  m_pRpcIdHelper       (0),	
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
     ChamberName,StationSize,StationEta/Phi are used to define a specific chamber/se of chambers to be plotted in
     the histo file  
  */
  declareProperty("McEventKey",     	  m_key="TruthEvent");
  declareProperty("WriteNtuple",          m_writeNtuple = true);
  declareProperty("NtupleFileName",       m_ntupleFileName = "MuonHits");
  declareProperty("NtupleDirectoryName",  m_ntupleDirName = "RPCHitsValidation");
  declareProperty("NtupleTreeName",       m_ntupleTreeName = "RPCTestHits");
  declareProperty("DoRPCTest",            m_DoRPCTest=true);
  declareProperty("ChamberName",	  m_chamberName="XXX");	
  declareProperty("StationSize",          m_StationSize="XXX");
  declareProperty("StationEta",           m_StationEta=-100);
  declareProperty("StationPhi",           m_StationPhi=-100);  
  declareProperty("LastEvent",            m_LastEvent=1);          	
  declareProperty("WriteChamberGraphs",   m_writeChambers = true);
  

  log << MSG::INFO << "++++++++++++  RPCHitsValAlg created  ++++++++++++" << endreq;
}

RPCHitsValAlg::~RPCHitsValAlg()
{
  log << MSG::INFO << " deleting RPCHitsValAlg " << endreq;
}

StatusCode RPCHitsValAlg::initialize()
{
  log << MSG::DEBUG << " initialize " << endreq;

  /**Locate the StoreGateSvc and initialize our local ptr
     intitialize transient event store
  */
  StatusCode sc= service("StoreGateSvc", m_sgSvc);

  if (!sc.isSuccess() || 0 == m_sgSvc) {
    log << MSG::FATAL << "RPCHitsValAlg: Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Retrieved StoreGateSvc." << endreq;
  }          			

  StoreGateSvc* detStore;
  sc = serviceLocator()->service( "DetectorStore", detStore );
  if (sc.isFailure())   {
    log << MSG::ERROR << "Cannot locate the DetectorStore" << endreq; 
    return sc;
  }
 
  sc = detStore->retrieve( m_pMuonMgr ); 
  if (sc.isFailure()){
    log << MSG::ERROR << "Cannot retrieve MuonDetectorManager" << endreq;
    return sc;
  }

  sc = detStore->retrieve(m_pRpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure()){
    log << MSG::ERROR << "Cannot retrieve m_rpcId" << endreq;
    return sc;
  }  


  /// ROOT AANtuple Initialization---------------------------------------

  if (m_writeNtuple) {

    std::string rpcStreamAndPath = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
    std::string rpctreeName = m_ntupleTreeName;

    std::string rpctreePath = rpcStreamAndPath+"/"+rpctreeName;

    rpctree = new TTree(TString(rpctreeName), "Muon RPC Hits output");
      
    StatusCode status;
    status=ToolRootHistSvc()->regTree(rpctreePath, rpctree);

    if(status.isFailure()) {
      log << MSG::DEBUG << "RPCHitsValAlg:: Unable to register TTreeTuple : " <<
	rpctreePath << endreq;
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
       RPC station name (char or int), conventional ATLAS naming scheme. Range: [BIL, BIR, BIS, BEE, BML, BMS, BMF, BOL, BOS, BOF, BOG, BOH]
       RPC station eta, increases with Z. Range: [-8,8] 
       RPC station phi, starts from sector 1 and increases with phi. Range: [1,8] (each phi unit incorporates two MuonSpectrometer barrel sectors)
       RPC Doublet R, number of chambers attached to a MDT, increases with R. Range: [1,2] (depends on the station type, two for middle stations - one for outer)
       RPC Doublet Z, one out of the two Z segments of an RPC plate, increases with |Z|. Range: [1,2]
       RPC Doublet Phi, one out of the two phi segments of an RPC plate, increases with phi. Range: [1,2]
       RPC Doublet GasGap, the gas volume between the RPC plates,increases with R. Range: [1,2]
       RPC Doublet MeasuresPhi, gives the orientation of the strips. Range: [0,1] (0 for strips that measure Z, 1 for strips that measure Phi)
       RPC Strip, info not available in G4 hits!	
    */

    rpctree->Branch("m_Validation_RPC_NumberOfHits",  &m_Validation_RPC_NumberOfHits, "m_Validation_RPC_NumberOfHits/I");
    rpctree->Branch("m_Validation_RPC_EventNumber",  &m_Validation_RPC_EventNumber, "m_Validation_RPC_EventNumber/I");
    rpctree->Branch("m_Validation_RPC_Type",  &m_Validation_RPC_Type, "m_Validation_RPC_Type/I");
    rpctree->Branch("m_Validation_RPC_RunNumber",  &m_Validation_RPC_RunNumber, "m_Validation_RPC_RunNumber/I");	 
    rpctree->Branch("m_Validation_RPC_HitX",  &m_Validation_RPC_HitX, "m_Validation_RPC_HitX/D"); 
    rpctree->Branch("m_Validation_RPC_HitY",  &m_Validation_RPC_HitY, "m_Validation_RPC_HitY/D"); 
    rpctree->Branch("m_Validation_RPC_HitZ",  &m_Validation_RPC_HitZ, "m_Validation_RPC_HitZ/D"); 
    rpctree->Branch("m_Validation_RPC_HitR",  &m_Validation_RPC_HitR, "m_Validation_RPC_HitR/D");
 
    rpctree->Branch("m_Validation_RPC_StationName",  &m_Validation_RPC_StationName, "m_Validation_RPC_StationName/I");
    rpctree->Branch("m_Validation_RPC_StationEta",  &m_Validation_RPC_StationEta, "m_Validation_RPC_StationEta/I"); 
    rpctree->Branch("m_Validation_RPC_StationPhi",  &m_Validation_RPC_StationPhi, "m_Validation_RPC_StationPhi/I"); 

    rpctree->Branch("m_Validation_RPC_DoubletR",  &m_Validation_RPC_DoubletR, "m_Validation_RPC_DoubletR/I");  
    rpctree->Branch("m_Validation_RPC_DoubletZ",  &m_Validation_RPC_DoubletZ, "m_Validation_RPC_DoubletZ/I");  
    rpctree->Branch("m_Validation_RPC_DoubletPhi",  &m_Validation_RPC_DoubletPhi, "m_Validation_RPC_DoubletPhi/I");  
    rpctree->Branch("m_Validation_RPC_DoubletGasGap",  &m_Validation_RPC_DoubletGasGap, "m_Validation_RPC_DoubletGasGap/I");   
    rpctree->Branch("m_Validation_RPC_MeasuresPhi",  &m_Validation_RPC_MeasuresPhi, "m_Validation_RPC_MeasuresPhi/I");
    rpctree->Branch("m_Validation_RPC_Strip",  &m_Validation_RPC_Strip, "m_Validation_RPC_Strip/I");


  }
  
  std::vector<std::string> hardware_name_list;
  hardware_name_list.push_back("XXX");
  event_counter=0; 
  
  log << MSG::DEBUG << "RPCHitsValAlg:: Initialisation ended     " << endreq;
  return StatusCode::SUCCESS;

} 

StatusCode RPCHitsValAlg::finalize()
{
  log << MSG::DEBUG << " finalize " << endreq;
	
  return StatusCode::SUCCESS;
}


/// Execute - on event by event

StatusCode RPCHitsValAlg::execute()
{

  log << MSG::DEBUG << " RPCHitsValAlg:: execute " << endreq;
  // StatusCode sc = StatusCode::SUCCESS;

  // ---------------------------------------
  // fill event data
  /** get EventInfo, used to obtain run and event number */
  const EventInfo* pevt;
	
  if (storeGateSvc()->retrieve(pevt).isFailure()) {
    log << MSG::FATAL << "Could not find event" << endreq;
    return(StatusCode::FAILURE);
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
  //int numrun = pevt->event_ID()->run_number();
  m_Validation_RPC_RunNumber = pevt->event_ID()->run_number(); 	

  /**Access MC truth information*/
  const DataHandle<McEventCollection> mcEvent;
  
  if(storeGateSvc()->retrieve(mcEvent,m_key).isFailure())
    {
      log << MSG::FATAL << "RPCHitsValAlg: Could not find MCevent" << endreq;
      return StatusCode::SUCCESS;
    }
  else {
    log << MSG::DEBUG << "RPCHitsValAlg: Found MCEvent" << endreq;
  }
	

  DataVector<HepMC::GenEvent>::const_iterator e;
  if (mcEvent->size()!=1) return StatusCode::SUCCESS;

  HepGeom::Vector3D<double> direction;
  for (e=mcEvent->begin();e!=mcEvent->end(); e++) {
    for (HepMC::GenEvent::particle_const_iterator p= (**e).particles_begin(); p!= (**e).particles_end(); p++) 
      {
	direction.setX((**p).momentum().px());
	direction.setX((**p).momentum().px());
	direction.setX((**p).momentum().px());
	direction = direction.unit();
      }	
  }



  /// ROOT histograms ---------------------------------------

  TH2 *rpcdet;
  TH2 *rpclongview; 
  TH1 *rpceta;
  TH1 *rpcphi;
  TH1 *rpcdoubletr;
  TH1 *rpcdoubletz;		
  TH1 *rpcdoubletphi;
  //TH1 *rpcdoubleteta;
  TH1 *rpcgasgap;    
  TH1 *rpcx;  
  TH1 *rpcy;
  TH1 *rpcz;
  TH1 *rpcchamberstat;  
  


  /**Enter the main algorithm loop*/
  if (m_DoRPCTest) {


    const DataHandle<RPCSimHitCollection> p_collection; 
    if (storeGateSvc()->retrieve(p_collection,"RPC_Hits")==StatusCode::FAILURE) 
      {
	log << MSG::FATAL << "No RPCSimHitCollection in StoreGate!" << endreq;
	return StatusCode::FAILURE;
      }
    else if(storeGateSvc()->retrieve(p_collection,"RPC_Hits")==StatusCode::SUCCESS)
      {
	
	/**Enter RPC hits loop, initialize the hits counter*/	
	counter_ValHitNumber =0 ;
	for(RPCSimHitCollection::const_iterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit) 
	  {
		
	    /**Increment hits counter*/
	    ++counter_ValHitNumber;


	    /** Check the Hits identifiers, access the functions that give:
		Station name, station eta, station phi, doublet Z, doublet phi, doublet R, GasGap, Measures Phi.
		The values of these variables are written out to the AANtuple (variable content and range explained in the code section where AANTuple variables are
		registered)		
	    */		       
	    HitID rpchit= i_hit->RPCid();	          
	    RpcHitIdHelper* rpchelper = RpcHitIdHelper::GetHelper();
	    std::string rpc_stname = rpchelper->GetStationName(rpchit);
	    /**Function that gives int station name from string station name.Locatedin MuonSimValutils*/                      
	    int		rpc_stname_int = convertStrToIntName(rpc_stname,"RPC"); 	    
	    int         rpc_steta  = rpchelper->GetZSector(rpchit);
	    int         rpc_stphi  = rpchelper->GetPhiSector(rpchit);
	    int         rpc_dbr    = rpchelper->GetDoubletR(rpchit);
	    int         rpc_dbz    = rpchelper->GetDoubletZ(rpchit);
	    int         rpc_dbp    = rpchelper->GetDoubletPhi(rpchit);
	    int         rpc_gg     = rpchelper->GetGasGapLayer(rpchit);
	    int         rpc_mfi    = rpchelper->GetMeasuresPhi(rpchit);
	    int         rpc_strip  = 1;//rpchelper->GetStrip(rpchit);
    

	    /**Create the offline identifiers, fill them with hit info.Ready to be accessed and retrieve info.
	       Currently not used in this code except for some checks.
	    */

	    Identifier offid = m_pRpcIdHelper->channelID(rpc_stname, rpc_steta, rpc_stphi, rpc_dbr, rpc_dbz, rpc_dbp, rpc_gg, rpc_mfi, rpc_strip);
	    if (offid == 0)
	      {
		log << MSG::FATAL << "RPC : Cannot build a valid Identifier; skip " << endreq;
	      }
   
	    const MuonGM::RpcReadoutElement* descriptor = m_pMuonMgr->getRpcReadoutElement(offid);
	    if (descriptor != NULL)
	      {
		Amg::Vector3D xU;
		Amg::Vector3D localHit(i_hit->localPosition().x(),i_hit->localPosition().y(),i_hit->localPosition().z());
		xU = descriptor->localToGlobalCoords(localHit,offid);
	      }
	    else
	      {
		log << MSG::FATAL << "RPC readout element not found for Id = " << m_pRpcIdHelper->show_to_string(offid) << endreq;	 
		HepGeom::Point3D<double>(0,0,0);     
	      }
	    // -------- Testing Sim to Offline ID conversion ---------
     
	    Amg::Vector3D rpc_gPos   = descriptor->stripPos(offid);
	    double rpc_strip_phi   = rpc_gPos.phi();
	    if (rpc_strip_phi < 0.) rpc_strip_phi += 2.*M_PI;
	    //double rpc_strip_perp  = rpc_gPos.perp();
	    //double rpc_strip_z   = rpc_gPos.z();
     
     
	    //Check Hits     		
	    /**For every hit within the event, get the global position HepGeom::Point3D<double> u and then retrieve all releveant info
	       either from the HepGeom::Point3D<double> or from the MC vector (direction)
	    */
	    GeoRPCHit ghit(*i_hit);
	                         
	    if (!ghit) continue;	
	    Amg::Vector3D u = ghit.getGlobalPosition();
		
	    //	    event->Fill(evt);
	    //	    run->Fill(numrun);						
	    double rad=sqrt(u.x()*u.x()+u.y()*u.y());

//	    double rpc_phi = TMath::ATan(u.y()/u.x());
	    double rpc_phi = u.phi();
	    if (rpc_phi<0.) {rpc_phi += 2.*M_PI;} 
	    double rpc_theta = TMath::ATan(rad/u.z());
	    if (rpc_theta<0) {rpc_theta += M_PI;}
	    double rpc_eta = -(TMath::Log(TMath::Tan((rpc_theta/2.))));


	    /**From function that convert MDT and RPC identifiers to harware names.Located in MuonSimValUtils*/
	    std::string hardware_name=convertChamberName(rpc_stname_int,rpc_steta,rpc_stphi,"RPC") ;

	    /**bool function, selects chamber to be plotted in histo file according to the user's 
	       selection in jobOpt.Located in MuonSimValUtils
	    */				
  	    if (selectChambersRange(hardware_name, m_chamberName, 
				    rpc_steta, m_StationEta,
				    rpc_stphi, m_StationPhi, m_StationSize)) {
  
	      if(m_writeChambers){
	      std::string m_generic_path_rpcdetcxx = "/muonhits/RPC/"+hardware_name+"/rpchit_det";
	      m_generic_path_rpcdetcxx += hardware_name;
       	    
	      std::string m_generic_path_rpclongviewcxx = "/muonhits/RPC/"+hardware_name+"/rpchit_longview";
	      m_generic_path_rpclongviewcxx += hardware_name;

	      std::string m_generic_path_rpcgasgapcxx = "/muonhits/RPC/"+hardware_name+"/rpchit_gasgap";
	      m_generic_path_rpcgasgapcxx += hardware_name;

	      std::string m_generic_path_rpcdoubletrcxx = "/muonhits/RPC/"+hardware_name+"/rpchit_doubletr";
	      m_generic_path_rpcdoubletrcxx += hardware_name;

	      std::string m_generic_path_rpcdoubletzcxx = "/muonhits/RPC/"+hardware_name+"/rpchit_doubletz";
	      m_generic_path_rpcdoubletzcxx += hardware_name;

	      //        	    std::string m_generic_path_rpcdoubletetacxx = "/muonhits/RPC/"+hardware_name+"/rpchit_doubleteta";
	      //  	    m_generic_path_rpcdoubletrcxx += hardware_name;

	      std::string m_generic_path_rpcdoubletphicxx = "/muonhits/RPC/"+hardware_name+"/rpchit_doubletphi";
	      m_generic_path_rpcdoubletphicxx += hardware_name;

	      std::string m_generic_path_rpcetacxx = "/muonhits/RPC/"+hardware_name+"/rpchit_eta";
	      m_generic_path_rpcetacxx += hardware_name;

	      std::string m_generic_path_rpcphicxx = "/muonhits/RPC/"+hardware_name+"/rpchit_phi";
	      m_generic_path_rpcphicxx += hardware_name;

	      std::string m_generic_path_rpcxcxx = "/muonhits/RPC/"+hardware_name+"/rpchit_x";
	      m_generic_path_rpcxcxx += hardware_name;

	      std::string m_generic_path_rpcycxx = "/muonhits/RPC/"+hardware_name+"/rpchit_y";
	      m_generic_path_rpcycxx += hardware_name;

	      std::string m_generic_path_rpczcxx = "/muonhits/RPC/"+hardware_name+"/rpchit_z";
	      m_generic_path_rpczcxx += hardware_name;		
	    
	      histo_flag=true;
	      for (std::vector<std::string>::const_iterator iter=hardware_name_list.begin(); iter!=hardware_name_list.end(); iter++){
		if (hardware_name==*iter){histo_flag=false;}
	      }
	      hardware_name_list.push_back(hardware_name);
	      if (histo_flag){ bookRPCHistograms(hardware_name); 
	      }	
 	
	      if(tHistSvc()->getHist(m_generic_path_rpcdetcxx,rpcdet).isFailure()) return StatusCode::FAILURE;	
	      if(tHistSvc()->getHist(m_generic_path_rpclongviewcxx,rpclongview).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_rpcgasgapcxx,rpcgasgap).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_rpcdoubletrcxx,rpcdoubletr).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_rpcdoubletzcxx,rpcdoubletz).isFailure()) return StatusCode::FAILURE;
	      // 	    if(tHistSvc()->getHist(m_generic_path_rpcdoubletetacxx,rpcdoubleteta).isFailure()) return StatusCode::FAILURE;
	      if(tHistSvc()->getHist(m_generic_path_rpcdoubletphicxx,rpcdoubletphi).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_rpcetacxx,rpceta).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_rpcphicxx,rpcphi).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_rpcxcxx,rpcx).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_rpcycxx,rpcy).isFailure()) return StatusCode::FAILURE;	    
	      if(tHistSvc()->getHist(m_generic_path_rpczcxx,rpcz).isFailure()) return StatusCode::FAILURE;	    
	    
	      rpcdet->Fill(u.x(),u.y());    
	      rpclongview->Fill(u.z(),rad);
	      rpcgasgap->Fill(rpc_gg);
	      rpcdoubletr->Fill(rpc_dbr);
	      rpcdoubletz->Fill(rpc_dbz);
	      //	    rpcdoubleteta->Fill(rpc_dbe);
	      rpcdoubletphi->Fill(rpc_dbp);
	      rpceta->Fill(rpc_eta);
	      rpcphi->Fill(rpc_phi);
	      rpcx->Fill(u.x());  
	      rpcy->Fill(u.y());
	      rpcz->Fill(u.z());	
              }//end chamber writing if statement
		
	      /**AANtuple variables
		 Fill the AANtuple variables with a value obtained for each hit within the event
	      */
	      if (m_writeNtuple) {
	      m_Validation_RPC_Type = 1; //(1: Hit, 2: Digit, 3: Reco)
	      m_Validation_RPC_NumberOfHits = counter_ValHitNumber;
	      m_Validation_RPC_EventNumber=evt;
	      m_Validation_RPC_HitX = u.x();
	      m_Validation_RPC_HitY = u.y();
	      m_Validation_RPC_HitZ = u.z();
	      m_Validation_RPC_HitR = rad;

	      m_Validation_RPC_StationName = rpc_stname_int;
		
//	      strcpy(m_Validation_RPC_StationName,(rpc_stname).c_str());  // put station name string in AANTUPLE as Val_StName  
	      m_Validation_RPC_StationEta = rpc_steta;
	      m_Validation_RPC_StationPhi = rpc_stphi;		

	      m_Validation_RPC_DoubletR=rpc_dbr;  
	      m_Validation_RPC_DoubletZ=rpc_dbz;  
	      m_Validation_RPC_DoubletPhi=rpc_dbp;  
	      m_Validation_RPC_DoubletGasGap=rpc_gg;   
	      m_Validation_RPC_MeasuresPhi=rpc_mfi;  

	      /**After proccessing each hit, dump the info in the tree*/
	      rpctree->Fill();		
	      }	//end ntuple writing if statement
	      
	      map<string,int>::iterator iter_hitsperchamber = hitsperchamber_map.find(hardware_name);
	      if ( iter_hitsperchamber  == hitsperchamber_map.end() ){ 
	        hitsperchamber_map.insert( make_pair( hardware_name,1 ) );  
	      } else {iter_hitsperchamber->second+=1;}	    
	    
	    }/**Close chamber name loop*/	 
	 
	  }/**Close the RPCSimHitCollection loop*/
	
	
      }/**Close part where RPCSimHitCollection is accessed from the StoreGate*/


  }/**Close part of m_DoRPCTest*/

  ++event_counter;  
  
  if (event_counter==m_LastEvent) {
  	
    std::string m_generic_path_rpcchamberstatcxx = "/muonhits/RPC/rpchit_chamberstat";
    //	log << MSG::INFO << "Histo path cxx: " << m_generic_path_rpcchamberstatcxx << endreq;	
	
    /**Writes hits per harware chamber in a txt file*/
    ofstream myfile;
    myfile.open ("rpchits.txt");
    myfile << "-------- Hit per Chamber Statistics--------\n";
	
    map<string,int>::iterator iter;
    bookRPCChamberStats();
    for( iter = hitsperchamber_map.begin(); iter != hitsperchamber_map.end(); iter++ ) {
      //bookRPCChamberStats();	
      if(tHistSvc()->getHist(m_generic_path_rpcchamberstatcxx,rpcchamberstat).isFailure()) return StatusCode::FAILURE;
	
      const char* chambername_char = iter->first.c_str ();
      /**Fills hits per harware chamber plot*/
      rpcchamberstat->Fill(chambername_char,iter->second);
		
      myfile << iter->first << "            |            " << iter->second << "\n";
    }
    myfile.close();
  }

  return StatusCode::SUCCESS;
	
}/**Close execute*/

/*---------------------------------------------------------*/
ITHistSvc* RPCHitsValAlg::ToolRootHistSvc()
  /*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() ) {
    log << MSG::WARNING << ">>> Unable to locate the RPCHitsValAlg Histogram service" << endreq;
  }
   
  return m_rootsvc;
} 

