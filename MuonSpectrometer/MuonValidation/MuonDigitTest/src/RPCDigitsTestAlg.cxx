/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// RPCDigitsTestAlg
// --------
// based on Ketevi A. Assamagan's RPCDigitValidation
////////////////////////////////////////////////////////////////////////////////

#include "MuonDigitTest/RPCDigitsTestAlg.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//#include "EventInfo/AtlasMcWeight.h"

///for GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ITHistSvc.h"

///for StoreGate
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"
 
#include "GeoAdaptors/GeoMuonHits.h"
 
#include "MuonReadoutGeometry/MuonDetectorManager.h" 
#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonSimEvent/RpcHitIdHelper.h" 
 
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"

#include "MuonSimValUtils/MuonChambersRange.h"
#include "MuonSimValUtils/MuonChamberNameConverter.h"

#include "GeneratorObjects/McEventCollection.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include <TH2D.h>
#include "TTree.h"
#include <string>

#include "HepMC/GenEvent.h"
 
#include <algorithm>
#include <TMath.h>
#include <fstream>
#include <iostream>


const int max_ValDigitNumber =100 ;

using namespace std;
using namespace MuonGM;

void bookRPCHistograms(std::string);
void bookRPCChamberStats();
////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

RPCDigitsTestAlg::RPCDigitsTestAlg(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator),
  TestAlgInterface(),
  m_pMuonMgr           (0),
  m_pRpcIdHelper       (0),
  log( msgSvc(), name )
{
  declareProperty("WriteNtuple",          m_writeNtuple = true);
  declareProperty("McEventKey",           m_key = "TruthEvent");
  declareProperty("NtupleFileName",       m_ntupleFileName = "MuonDigits");
  declareProperty("NtupleDirectoryName",  m_ntupleDirName  = "RPCDigitsValidation");
  declareProperty("NtupleTreeName",       m_ntupleTreeName = "RPCTestDigits");
  declareProperty("DoRPCTest",            m_DoRPCTest = true);
  declareProperty("ChamberName",	  m_chamberName="XXX");	
  declareProperty("StationSize",          m_StationSize="XXX");
  declareProperty("StationEta",           m_StationEta=-100);
  declareProperty("StationPhi",           m_StationPhi=-100);  
  declareProperty("WriteChamberGraphs",   m_writeChambers = true);
  declareProperty("LastEvent",            m_LastEvent=1); 

  log << MSG::INFO << "++++++++++++  RPCDigitsTestAlg created  ++++++++++++"<< endreq;
}

RPCDigitsTestAlg::~RPCDigitsTestAlg()
{
  log << MSG::INFO << " deleting RPCDigitsTestAlg " << endreq;
}

////////////////////////////////////////////////////////////////////////////////
// member function implementation
////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------*/
StatusCode RPCDigitsTestAlg::initialize()
{
  log << MSG::DEBUG << " initializing RPCDigitsTestAlg " << endreq;
  StatusCode sc = StatusCode::SUCCESS;

  // Locate the StoreGateSvc and initialize our local ptr
  // intitialize transient event store

  sc = service("StoreGateSvc", m_sgSvc);

  if (!sc.isSuccess() || 0 == m_sgSvc)
    {
      log << MSG::ERROR << "RPCDigitsTestAlg: Could not find StoreGateSvc" << endreq;
      return sc;
    }
  log << MSG::DEBUG << "Retrieved StoreGateSvc" << endreq;

  // retrieve the StoreGate Active store Service
  sc = service("ActiveStoreSvc",m_activeStore);
  if (sc.isFailure())
    {
      log<<MSG::ERROR << "Could not retrieve StoreGate ActiveStoreSvc!" <<endreq;
      return sc;
    }	
  log << MSG::DEBUG << "Retrieved StoreGate ActiveStoreSvc." << endreq;
 
  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore );
  if ( sc.isSuccess() )
    {
      sc = detStore->retrieve( m_pMuonMgr );
      if ( sc.isFailure() )
	{
	  log << MSG::ERROR << " RPCDigitsTestAlg::initialize() :  Could not find MuonDetDescrMgr " << endreq;
	  return sc;
	}
      else m_pRpcIdHelper = m_pMuonMgr->rpcIdHelper();
    }
  else log << MSG::ERROR  << " RPCDigitsTestAlg::initialize() : MuonDetDescrMgr not found in DetectorStore " << endreq;
 
  if (sc.isFailure())
    {
      log <<MSG::ERROR << "Could not get DetectorStore"<<endreq;
      return sc;
    }
 
  /// ROOT AANtuple Initialization---------------------------------------
  if (m_writeNtuple)
    {
      std::string rpcStreamAndPath = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
      std::string rpctreeName = m_ntupleTreeName;
      std::string rpctreePath = rpcStreamAndPath+"/"+rpctreeName;

      rpctree = new TTree(TString(rpctreeName), "Muon RPC Digits output");

      StatusCode status;
      status=ToolRootHistSvc()->regTree(rpctreePath, rpctree);

      if(status.isFailure())
	{
	  log << MSG::DEBUG << "RPCDigitsTestAlg:: Unable to register TTreeTuple : "
	      << rpctreePath << endreq;
	  return status;
	}
      //-----------------
      // add items-variables to AANTUPLE

      rpctree->Branch("m_Validation_RPC_NumberOfDigits",&m_Val_RPC_NumberOfDigits,"m_Validation_RPC_NumberOfDigits/I");
      rpctree->Branch("m_Validation_RPC_RunNumber",&m_Val_RPC_RunNumber,"m_Validation_RPC_RunNumber/I");
      rpctree->Branch("m_Validation_RPC_EventNumber",&m_Val_RPC_EventNumber,"m_Validation_RPC_EventNumber/I");
      rpctree->Branch("m_Validation_RPC_StationName",&m_Validation_RPC_StationName,"m_Validation_RPC_StationName/I");
      rpctree->Branch("m_Validation_RPC_StationEta",&m_Validation_RPC_StationEta,"m_Validation_RPC_StationEta/I");
      rpctree->Branch("m_Validation_RPC_StationPhi",&m_Validation_RPC_StationPhi,"m_Validation_RPC_StationPhi/I");
      rpctree->Branch("m_Validation_RPC_DoubletR",&m_Validation_RPC_DoubletR,"m_Validation_RPC_DoubletR/I");
      rpctree->Branch("m_Validation_RPC_DoubletZ",&m_Validation_RPC_DoubletZ,"m_Validation_RPC_DoubletZ/I");
      rpctree->Branch("m_Validation_RPC_DoubletPhi",&m_Validation_RPC_DoubletPhi,"m_Validation_RPC_DoubletPhi/I");
      rpctree->Branch("m_Validation_RPC_DoubletGasGap",&m_Validation_RPC_DoubletGasGap,"m_Validation_RPC_DoubletGasGap/I");
      rpctree->Branch("m_Validation_RPC_MeasuresPhi",&m_Validation_RPC_MeasuresPhi,"m_Validation_RPC_MeasuresPhi/I");
      rpctree->Branch("m_Validation_RPC_Strip",&m_Validation_RPC_Strip,"m_Validation_RPC_Strip/I");
      rpctree->Branch("m_Validation_RPC_StripX",&m_Validation_RPC_StripX,"m_Validation_RPC_StripX/D");
      rpctree->Branch("m_Validation_RPC_StripY",&m_Validation_RPC_StripY,"m_Validation_RPC_StripY/D");
      rpctree->Branch("m_Validation_RPC_StripZ",&m_Validation_RPC_StripZ,"m_Validation_RPC_StripZ/D");
      rpctree->Branch("m_Validation_RPC_Stripdistance",&m_Validation_RPC_Stripdistance,"m_Validation_RPC_Stripdista/D");
    }
  std::vector<std::string> hardware_name_list;
  hardware_name_list.push_back("XXX");
  event_counter=0;
  
  log << MSG::DEBUG << "RPCDigitsTestAlg:: Initialisation ended  " << endreq;
  return sc;
}


/// Execute - on event by event

/*---------------------------------------------------------*/
StatusCode RPCDigitsTestAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;
  log << MSG::DEBUG << " RPCDigitsTestAlg:: execute " << endreq;

  // ---------------------------------------
  // fill event data
  // get EventInfo for run and event number
  const EventInfo* pevt;

  clear_variables();

  if (storeGateSvc()->retrieve(pevt).isFailure())
    {
      log << MSG::FATAL << "RPCDigitsTestAlg::Could not find event" << endreq;
      return sc;
    }
  log << MSG::DEBUG << "RPCDigitsTestAlg::Found EventInfo in SG" << endreq;
 
  if(isVerbose()) log << MSG::VERBOSE <<"RPCDigitsTestAlg::Processing EventInfo event #"<<pevt->event_ID()->event_number() << " run: " << pevt->event_ID()->run_number() << endreq;
 
  m_Val_RPC_RunNumber = pevt->event_ID()->run_number();
  m_Val_RPC_EventNumber= pevt->event_ID()->event_number();

  if(m_DoRPCTest)
    {
      findMuons();
      if( m_theMuons.size() ){ if( do_AANtuple_conversion().isFailure()) return StatusCode::FAILURE; }
      else log << MSG::WARNING <<"could not find any muon in the event, skipping RPCDigitsTestAlgvalidation" << endreq; 
    }

  return sc;
}

/*---------------------------------------------------------*/
StatusCode RPCDigitsTestAlg::finalize()
{
  return StatusCode::SUCCESS; 
}
/*---------------------------------------------------------*/

void RPCDigitsTestAlg::findMuons()
{
  m_theMuons.clear();
  HepGeom::Point3D<double> pvert = HepGeom::Point3D<double>(0,0,0);

  const DataHandle<McEventCollection> mcEvent;

  if(storeGateSvc()->retrieve(mcEvent,m_key).isFailure())
    {
      log << MSG::FATAL << "RPCDigitsTestAlg: Could not find MCevent" << endreq;
      return;
    }
  log << MSG::DEBUG << "RPCDigitsTestAlg: Found MCEvent" << endreq;

  m_nPar = 0;
  mu_phi = 0.;
  mu_eta = 0.;

  for (McEventCollection::const_iterator itr = mcEvent->begin(); itr!=mcEvent->end(); ++itr)
    {
      const HepMC::GenEvent* myEvent = (*itr);
      HepMC::GenEvent::particle_const_iterator Part;
      for (Part = myEvent->particles_begin(); Part!=myEvent->particles_end(); ++Part )
	{
	  if ( (*Part)->pdg_id() == -13 || (*Part)->pdg_id() == 13 || (*Part)->pdg_id() == 999)
	    {
	      HepMC::GenVertex* Vert = (*Part)->production_vertex();
	      if (Vert!=0)
		{
		  pvert.setX((*Part)->momentum().px());
		  pvert.setY((*Part)->momentum().py());
		  pvert.setZ((*Part)->momentum().pz());

		  mu_phi		= (*Part)->momentum().phi();
		  mu_eta		= (*Part)->momentum().eta();
		  m_theMuons.insert(std::map<int, HepGeom::Point3D<double> >::value_type((*Part)->barcode(),
									  pvert));
		  mu_p		=       (*Part)->momentum().e();
		  mu_pT		=       (*Part)->momentum().perp();
		  double	x       =       Vert->position().x();
		  double	y       =       Vert->position().y();
		  //double	z	=	Vert->position().z();
		  double	rho     =       sqrt(x*x+y*y);
		  double	phi     =       (*Part)->momentum().phi();
		  double	theta   =       (*Part)->momentum().theta();
		  pvert = HepGeom::Point3D<double> (rho,theta,phi);

		}
	    }
	  ++m_nPar;
	}
    }
}

/*---------------------------------------------------------*/
StatusCode RPCDigitsTestAlg::do_AANtuple_conversion()
{
  
  /// ROOT histograms ---------------------------------------    
  
  //  TH2 *rpcdet;
  //  TH2 *rpclongview; 
  TH1 *rpcdoubletr;    
  TH1 *rpcgasgap;    
  TH1 *rpcdoubletz;  
  TH1 *rpcdoubletphi;
  TH1 *rpcmeasuresphi;
  TH1 *rpcstrip; 
  //   TH1 *rpcx;  
  //   TH1 *rpcy;
  //   TH1 *rpcz;
  TH1 *rpcchamberstat;  
  
  if (m_DoRPCTest)
    {
      typedef RpcDigitCollection::const_iterator digit_iterator;

      const DataHandle<RpcDigitContainer> rpcCont;
      std::string key = "RPC_DIGITS";
      if (storeGateSvc()->retrieve(rpcCont,key)==StatusCode::FAILURE)
	{
	  log << MSG::FATAL << "No RpcDigitContainer in StoreGate!" << endreq;
	  return StatusCode::FAILURE;
	}
      else if(storeGateSvc()->retrieve(rpcCont,key)==StatusCode::SUCCESS)
	{
	  m_Val_RPC_NumberOfDigits=0;
	  for (RpcDigitContainer::const_iterator it1_coll=rpcCont->begin();
	       it1_coll!=rpcCont->end(); ++it1_coll)
	    { // loop over container

	      const RpcDigitCollection* thecoll=(*it1_coll);
	      digit_iterator it1_dig=thecoll->begin();
	      digit_iterator it2_dig=thecoll->end();
	      // loop over digit collection
	      for (  ; it1_dig!=it2_dig; ++it1_dig)
		{// loop over collection
		  const  RpcDigit* theDigit = *it1_dig;

		  Identifier channelId = theDigit->identify();

		  if (m_pRpcIdHelper->valid(channelId))
		    {
		      const MuonGM::RpcReadoutElement* descriptor = m_pMuonMgr->getRpcReadoutElement(channelId);

 		      Amg::Vector3D digPos  = descriptor->stripPos(channelId);
		      // 		      HepGeom::Point3D<double> globalWire =  descriptor->localToGlobalCoords( localHitOnWire
		      // 									    , channelId );

		      int nstName  	= m_pRpcIdHelper->stationName(channelId);
		      int nstEta  	= m_pRpcIdHelper->stationEta(channelId);
		      int nstPhi  	= m_pRpcIdHelper->stationPhi(channelId);
		      int ndoubletR	= m_pRpcIdHelper->doubletR(channelId);
		      int ndoubletZ	= m_pRpcIdHelper->doubletZ(channelId);
		      int ndoubletPhi	= m_pRpcIdHelper->doubletPhi(channelId);
		      int ngasGap	= m_pRpcIdHelper->gasGap(channelId);
		      int nmeasuresPhi= m_pRpcIdHelper->measuresPhi(channelId);
		      int nstrip	= m_pRpcIdHelper->strip(channelId);
		      float distance = 0.0;

		      //		      double rad=sqrt(digPos.x()*digPos.x()+digPos.y()*digPos.y());
	    	  
		      //		      double rpc_phi = digPos.phi();
		      //		      if (rpc_phi<0.) {rpc_phi += 2.*M_PI;} 
 			  
		      // 		      double rpc_theta = TMath::ATan(rad/globalWire.z());
		      // 	    	      if (rpc_theta<0) {rpc_theta += M_PI;}    	      
		      // 		      double rpc_eta = -(TMath::Log(TMath::Tan((rpc_theta/2.))));
		      
		      
                      if (m_writeNtuple) {
		      m_Val_RPC_NumberOfDigits++;
		      m_Validation_RPC_StationName=nstName;
		      m_Validation_RPC_StationEta=nstEta;
		      m_Validation_RPC_StationPhi=nstPhi;
		      m_Validation_RPC_DoubletR=ndoubletR;
		      m_Validation_RPC_DoubletZ=ndoubletZ;
		      m_Validation_RPC_DoubletPhi=ndoubletPhi;
		      m_Validation_RPC_DoubletGasGap=ngasGap;
		      m_Validation_RPC_MeasuresPhi=nmeasuresPhi;
		      m_Validation_RPC_Strip=nstrip;
		      m_Validation_RPC_StripX=digPos.x();
		      m_Validation_RPC_StripY=digPos.y();
		      m_Validation_RPC_StripZ=digPos.z();
		      m_Validation_RPC_Stripdistance=distance;

		      rpctree->Fill();
		      } //end ntuple writing if statement
		      
	    	      std::string hardware_name=convertChamberName(nstName,nstEta,nstPhi,"RPC") ;		    
 	    	      if (selectChambersRange(hardware_name, m_chamberName, 
					      nstEta, m_StationEta,
					      nstPhi, m_StationPhi, m_StationSize)) {
				    
                       if(m_writeChambers){
				    
	    		std::string m_generic_path_rpcdoubletrcxx = "/muondigits/RPC/"+hardware_name+"/rpcdigit_doubletr";
	    		m_generic_path_rpcdoubletrcxx += hardware_name;

	    		std::string m_generic_path_rpcgasgapcxx = "/muondigits/RPC/"+hardware_name+"/rpcdigit_gasgap";
	    		m_generic_path_rpcgasgapcxx += hardware_name;

	    		std::string m_generic_path_rpcdoubletzcxx = "/muondigits/RPC/"+hardware_name+"/rpcdigit_doubletz";
	    		m_generic_path_rpcdoubletzcxx += hardware_name;				    

	    		std::string m_generic_path_rpcdoubletphicxx = "/muondigits/RPC/"+hardware_name+"/rpcdigit_doubletphi";
	    		m_generic_path_rpcdoubletphicxx += hardware_name;	

	    		std::string m_generic_path_rpcmeasuresphicxx = "/muondigits/RPC/"+hardware_name+"/rpcdigit_measuresphi";
	    		m_generic_path_rpcmeasuresphicxx += hardware_name;
			
	    		std::string m_generic_path_rpcstripcxx = "/muondigits/RPC/"+hardware_name+"/rpcdigit_strip";
	    		m_generic_path_rpcstripcxx += hardware_name;								    


	      		histo_flag=true;
	      		for (std::vector<std::string>::const_iterator iter=hardware_name_list.begin(); iter!=hardware_name_list.end(); iter++){
			  if (hardware_name==*iter){histo_flag=false;}
			}
	      		hardware_name_list.push_back(hardware_name);
	      		if (histo_flag){ bookRPCHistograms(hardware_name); 
	      		}


	      		if(tHistSvc()->getHist(m_generic_path_rpcdoubletrcxx,rpcdoubletr).isFailure()) return StatusCode::FAILURE; 
	      		if(tHistSvc()->getHist(m_generic_path_rpcgasgapcxx,rpcgasgap).isFailure()) return StatusCode::FAILURE; 
	      		if(tHistSvc()->getHist(m_generic_path_rpcdoubletzcxx,rpcdoubletz).isFailure()) return StatusCode::FAILURE; 
	      		if(tHistSvc()->getHist(m_generic_path_rpcdoubletphicxx,rpcdoubletphi).isFailure()) return StatusCode::FAILURE; 
	      		if(tHistSvc()->getHist(m_generic_path_rpcmeasuresphicxx,rpcmeasuresphi).isFailure()) return StatusCode::FAILURE; 
			if(tHistSvc()->getHist(m_generic_path_rpcstripcxx,rpcstrip).isFailure()) return StatusCode::FAILURE; 

			rpcdoubletr->Fill(ndoubletR);
			rpcgasgap->Fill(ngasGap);
			rpcdoubletz->Fill(ndoubletZ);
			rpcdoubletphi->Fill(ndoubletPhi);
			rpcmeasuresphi->Fill(nmeasuresPhi);
			rpcstrip->Fill(nstrip);	    
			
                       }//end chamber writing if statement
				    
	     		map<string,int>::iterator iter_digitsperchamber = digitsperchamber_map.find(hardware_name);
	      		if ( iter_digitsperchamber  == digitsperchamber_map.end() ){ 
			  digitsperchamber_map.insert( make_pair( hardware_name,1 ) );  
	      		} else {iter_digitsperchamber->second+=1;}				    
				    
		      }		    
		    }

		}
	    }
	}
    }

  ++event_counter;

  if (event_counter==m_LastEvent) {
  	
    std::string m_generic_path_rpcchamberstatcxx = "/muondigits/RPC/rpcdigit_chamberstat";
		
    /**Writes digits per harware chamber in a txt file*/
    ofstream myfile;
    myfile.open ("rpcdigits.txt");
    myfile << "-------- Digit per Chamber Statistics--------\n";
	
    map<string,int>::iterator iter;
    bookRPCChamberStats();
    for( iter = digitsperchamber_map.begin(); iter != digitsperchamber_map.end(); iter++ ) {
      //bookRPCChamberStats();	
      if(tHistSvc()->getHist(m_generic_path_rpcchamberstatcxx,rpcchamberstat).isFailure()) return StatusCode::FAILURE; 
	
      const char* chambername_char = iter->first.c_str ();
      /**Fills digits per harware chamber plot*/
      rpcchamberstat->Fill(chambername_char,iter->second);
		
      myfile << iter->first << "            |            " << iter->second << "\n";
    }
    myfile.close();
  }


  return StatusCode::SUCCESS;
}


double RPCDigitsTestAlg::getDistance(HepGeom::Point3D<double> clusPos, HepGeom::Point3D<double> vertex,
				     int measuresPhi) const
{
  MsgStream log(msgSvc(), name());

  double dist;
  double thetaStrip = clusPos.theta();
  double phiStrip   = clusPos.phi();
  double thetaMC    = vertex.y(); // x and y components contains theta and phi
  double phiMC      = vertex.z(); // see method truth_info for details

  if (measuresPhi==0)
    {
      dist = sqrt(clusPos.x()*clusPos.x()+clusPos.y()*clusPos.y())*sin(thetaMC-thetaStrip);
    }
  else
    {
      dist = sqrt(clusPos.x()*clusPos.x()+clusPos.y()*clusPos.y())*sin(phiMC-phiStrip);
    }

  log << MSG::DEBUG <<" Comparing MC Truth to GeoModel Position "
      << " thetaMC = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << thetaMC
      << " phiMC   = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << phiMC
      << " thetaStrip = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << thetaStrip
      << " phiStrip = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << phiStrip
      << " distance from tack (mm) = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << dist
      << endreq;

  return dist;
}

ITHistSvc* RPCDigitsTestAlg::ToolRootHistSvc()
{
  MsgStream log(msgSvc(), name());
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() )
    {
      log << MSG::WARNING << ">>> Unable to locate the RPCDigitsTestAlg Histogram service" << endreq;
    }
  return m_rootsvc;
}



void	RPCDigitsTestAlg::clear_variables()
{
  m_Val_RPC_RunNumber=-1;
  m_Val_RPC_EventNumber=-1;
  m_Val_RPC_NumberOfDigits=-1;
  m_Validation_RPC_StationName=-1;
  m_Validation_RPC_StationEta=-1;
  m_Validation_RPC_StationPhi=-1;
  m_Validation_RPC_DoubletR=-1;
  m_Validation_RPC_DoubletZ=-1;
  m_Validation_RPC_DoubletPhi=-1;
  m_Validation_RPC_DoubletGasGap=-1;
  m_Validation_RPC_MeasuresPhi=-1;
  m_Validation_RPC_Strip=-1;
  m_Validation_RPC_StripX=-1;
  m_Validation_RPC_StripY=-1;
  m_Validation_RPC_StripZ=-1;
  m_Validation_RPC_Stripdistance=-1;
}
