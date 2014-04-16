/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// TGCDigitsTestAlg
// --------
// based on Ketevi A. Assamagan's TGCDigitValidation
////////////////////////////////////////////////////////////////////////////////

#include "MuonDigitTest/TGCDigitsTestAlg.h"
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
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonSimEvent/TgcHitIdHelper.h" 
 
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
   
#include "GeneratorObjects/McEventCollection.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"

#include <TH2D.h>
#include "TTree.h"
#include <string>
  
#include "HepMC/GenEvent.h"
 
#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include <algorithm>
#include <cmath>
#include <fstream>


const int maxHits = 200;

using namespace MuonGM;
 
////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

TGCDigitsTestAlg::TGCDigitsTestAlg(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator),
  TestAlgInterface(),
  m_pMuonMgr           (0),
  m_pTgcIdHelper       (0),
  log( msgSvc(), name )
{
  declareProperty("WriteNtuple",          m_writeNtuple = true);
  declareProperty("McEventKey",           m_key = "TruthEvent");
  declareProperty("NtupleFileName",       m_ntupleFileName = "MuonDigits");
  declareProperty("NtupleDirectoryName",  m_ntupleDirName  = "TGCDigitsValidation");
  declareProperty("NtupleTreeName",       m_ntupleTreeName = "TGCTestDigits");	
  declareProperty("DoTGCTest",            m_DoTGCTest = true);
  m_event = 0;

  log << MSG::INFO << "++++++++++++  TGCDigitsTestAlg created  ++++++++++++"
      << endreq;

}

TGCDigitsTestAlg::~TGCDigitsTestAlg()
{
  log << MSG::INFO << " deleting TGCDigitsTestAlg " << endreq;
}
   
////////////////////////////////////////////////////////////////////////////////
// member function implementation
////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------*/
StatusCode TGCDigitsTestAlg::initialize()
{
  log << MSG::INFO << " initializing TGCDigitsTestAlg " << endreq;
 
  // Locate the StoreGateSvc and initialize our local ptr
  // intitialize transient event store 

  StatusCode sc = service("StoreGateSvc", m_sgSvc);

  if (!sc.isSuccess() || 0 == m_sgSvc) {
    log << MSG::ERROR << "TGCDigitsTestAlg: Could not find StoreGateSvc" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Retrieved StoreGateSvc" << endreq;
  } 
      

  // retrieve the StoreGate Active store Service
  
  sc = service("ActiveStoreSvc",m_activeStore);
  if (sc.isFailure()) {
    log<<MSG::ERROR << "Could not retrieve StoreGate ActiveStoreSvc!" <<endreq;
  } else {
    log<<MSG::DEBUG<< "Retrieved StoreGate ActiveStoreSvc." << endreq;
  }



  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore );
  if ( sc.isSuccess() ) 
    {
      sc = detStore->retrieve( m_pMuonMgr );
      if ( sc.isFailure() ) 
	{
	  log << MSG::ERROR 
	      << " TGCDigitsTestAlg::initialize() :  Could not find MuonDetDescrMgr "
	      << endreq;
	} 
      else 
	{ 
	  m_pTgcIdHelper = m_pMuonMgr->tgcIdHelper();
	}
    } 
  else 
    {
      log << MSG::ERROR 
	  << " TGCDigitsTestAlg::initialize() : MuonDetDescrMgr not found in DetectorStore " 
	  << endreq;
    }    
      
      
  if (sc.isFailure()){
    log <<MSG::ERROR << "Could not get DetectorStore"<<endreq;
    return( StatusCode::FAILURE );
  }
 	
  /// ROOT AANtuple Initialization---------------------------------------

  if (m_writeNtuple) {

/*    m_Validation_TGC_StationName	= new std::vector<int>();
    m_Validation_TGC_StationEta	= new std::vector<int>();
    m_Validation_TGC_StationPhi	= new std::vector<int>();
    m_Validation_TGC_GasGap		= new std::vector<int>();
    m_Validation_TGC_IsStrip	= new std::vector<int>();
    m_Validation_TGC_Channel	= new std::vector<int>();
    m_Validation_TGC_ChannelPosX	= new std::vector<float>();
    m_Validation_TGC_ChannelPosY	= new std::vector<float>();
    m_Validation_TGC_ChannelPosZ	= new std::vector<float>();*/


    std::string tgcStreamAndPath = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
    std::string tgctreeName = m_ntupleTreeName;

    std::string tgctreePath = tgcStreamAndPath+"/"+tgctreeName;

    tgctree = new TTree(TString(tgctreeName), "Muon TGC Digits output");

    StatusCode status;
    status=ToolRootHistSvc()->regTree(tgctreePath, tgctree);

    tgctree->Branch("m_Validation_TGC_NumberOfDigits",&m_Val_TGC_NumberOfDigits,"m_Validation_TGC_NumberOfDigits/I");
    tgctree->Branch("m_Validation_TGC_RunNumber",&m_Val_TGC_RunNumber,"m_Validation_TGC_RunNumber/I");
    tgctree->Branch("m_Validation_TGC_EventNumber",&m_Val_TGC_EventNumber,"m_Validation_TGC_EventNumber/I");
    tgctree->Branch("m_Validation_TGC_StationName",&m_Validation_TGC_StationName,"m_Validation_TGC_StationName/I");
    tgctree->Branch("m_Validation_TGC_StationEta",&m_Validation_TGC_StationEta,"m_Validation_TGC_StationEta/I");
    tgctree->Branch("m_Validation_TGC_StationPhi",&m_Validation_TGC_StationPhi,"m_Validation_TGC_StationPhi/I");
    tgctree->Branch("m_Validation_TGC_GasGap",&m_Validation_TGC_GasGap,"m_Validation_TGC_GasGap/I");
    tgctree->Branch("m_Validation_TGC_IsStrip",&m_Validation_TGC_IsStrip,"m_Validation_TGC_IsStrip/I");
    tgctree->Branch("m_Validation_TGC_Channel",&m_Validation_TGC_Channel,"m_Validation_TGC_Channel/I");
    tgctree->Branch("m_Validation_TGC_ChannelPosX",&m_Validation_TGC_ChannelPosX,"m_Validation_TGC_ChannelPosX/D");
    tgctree->Branch("m_Validation_TGC_ChannelPosY",&m_Validation_TGC_ChannelPosY,"m_Validation_TGC_ChannelPosY/D");
    tgctree->Branch("m_Validation_TGC_ChannelPosZ",&m_Validation_TGC_ChannelPosZ,"m_Validation_TGC_ChannelPosZ/D");

    if(status.isFailure()) {
      log << MSG::DEBUG << "TGCDigitsTestAlg:: Unable to register TTreeTuple : " << tgctreePath << endreq;
      return status;   
    }
    //-----------------
    // add items-variables to AANTUPLE

    //m_nMu = new std::vector<int>();
    //tgctree->Branch("m_nMu",  &m_nMu, "Number_ofMuons/I");


  }

  log << MSG::INFO << "TGCDigitsTestAlg:: Initialisation ended  " << endreq;
  return StatusCode::SUCCESS;

}


/// Execute - on event by event

/*---------------------------------------------------------*/ 
StatusCode TGCDigitsTestAlg::execute()
{

  StatusCode sc; 
  log << MSG::DEBUG << " TGCDigitsTestAlg:: execute " << endreq;

  // ---------------------------------------
  // fill event data 
  // get EventInfo for run and event number
  const EventInfo* pevt;

	
  if (storeGateSvc()->retrieve(pevt).isFailure()) {
    log << MSG::FATAL << "TGCDigitsTestAlg::Could not find event" << endreq;
    return(StatusCode::FAILURE);
  }
  else {
    log << MSG::DEBUG << "TGCDigitsTestAlg::Found EventInfo in SG" << endreq;
  }

  if(isVerbose()) 
    {
      log << MSG::VERBOSE <<"TGCDigitsTestAlg::Processing EventInfo event #"<<pevt->event_ID()->event_number() << " run: " << pevt->event_ID()->run_number() << endreq;
    }
  
       
  m_Val_TGC_EventNumber = pevt->event_ID()->event_number();
  m_Val_TGC_RunNumber = pevt->event_ID()->run_number();

  if(m_DoTGCTest)
    {
      findMuons();
 
      if( m_theMuons.size() )
	{
	  sc = do_validation();
	}
      else log<<MSG::WARNING<<"could not find any muon in the event, skipping TGCDigitsTestAlg validation"<<endreq;  
    }
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TGCDigitsTestAlg::finalize()
{
  //log << MSG::ERROR << " TGCDigitsTestAlg :: !!!! in finalize()" << endreq;

  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
void TGCDigitsTestAlg::findMuons()
{   
  m_theMuons.clear();
  HepGeom::Point3D<double> pvert = HepGeom::Point3D<double>(0,0,0);

  const DataHandle<McEventCollection> mcEvent;
  
  if(storeGateSvc()->retrieve(mcEvent,m_key).isFailure())
    {
      log << MSG::FATAL << "TGCDigitsTestAlg: Could not find MCevent" << endreq;
      return;
    }
  else {
    log << MSG::DEBUG << "TGCDigitsTestAlg: Found MCEvent" << endreq;
  }	   
  m_nPar = 0;
  mu_phi = 0.;
  mu_eta = 0.;    

  for (McEventCollection::const_iterator itr = mcEvent->begin(); itr!=mcEvent->end(); ++itr) {
    const HepMC::GenEvent* myEvent = (*itr);
      
    HepMC::GenEvent::particle_const_iterator Part;
    for (Part = myEvent->particles_begin(); Part!=myEvent->particles_end(); ++Part ) {
      if ( (*Part)->pdg_id() == -13 || (*Part)->pdg_id() == 13 || (*Part)->pdg_id() == 999) {
	HepMC::GenVertex* Vert = (*Part)->production_vertex();
	if (Vert!=0) 
	  {
	    pvert.setX((*Part)->momentum().px());
	    pvert.setY((*Part)->momentum().py());
	    pvert.setZ((*Part)->momentum().pz());

	    mu_phi = (*Part)->momentum().phi();
	    mu_eta = (*Part)->momentum().eta();
	    m_theMuons.insert(std::map<int, HepGeom::Point3D<double> >::value_type((*Part)->barcode(), pvert));
            mu_p     =       (*Part)->momentum().e();
            mu_pT    =       (*Part)->momentum().perp();
	    double              x       =       Vert->position().x();
	    double              y       =       Vert->position().y();
	    //double		z	=	Vert->position().z();
            double              rho     =       sqrt(x*x+y*y);
            double              phi     =       (*Part)->momentum().phi();
            double              theta   =       (*Part)->momentum().theta();	    
            pvert = HepGeom::Point3D<double> (rho,theta,phi);
	    //std::cout << "The momentum is " << pt << " CLHEP::MeV" << std::endl;
	    //std::cout << "The vertex is (CLHEP::mm)" << x << " " << y << " " << z << std::endl;
	  }
      }
      ++m_nPar;
    }
      
  }    
      
}

/*---------------------------------------------------------*/
StatusCode TGCDigitsTestAlg::do_validation()
{
  StatusCode sc;

  if (m_DoTGCTest)
    {
      const DataHandle<TgcDigitContainer> tgcCont;
      std::string key = "TGC_DIGITS";
      if (storeGateSvc()->retrieve(tgcCont,key)==StatusCode::FAILURE)
	{
	  log << MSG::FATAL << "No TgcDigitContainer in StoreGate!" << endreq;
	  return StatusCode::FAILURE;
	}

      std::string  sdoKey= "TGC_SDO"; 
      const MuonSimDataCollection* sdoContainer;
      sc = m_sgSvc->retrieve(sdoContainer,sdoKey);
      if (!sc.isSuccess())
	{
	  log << MSG::ERROR << "No Sdo Container found" << endreq;
	  return false;
	}


      m_nHits=0;
      //int mucount=0;
      m_Val_TGC_NumberOfDigits=0;
      for (TgcDigitContainer::const_iterator it1_coll=tgcCont->begin();it1_coll!=tgcCont->end(); ++it1_coll)
	{ // loop over container
	  const TgcDigitCollection* thecoll=(*it1_coll);
	  for(TgcDigitCollection::const_iterator it1_dig=thecoll->begin(); it1_dig!=thecoll->end();++it1_dig)
	    {// loop over digitcollection
	      const  TgcDigit* theDigit = *it1_dig;
	      Identifier channelId = theDigit->identify();
	      if (m_pTgcIdHelper->valid(channelId))
		{
		  const TgcReadoutElement *descriptor=m_pMuonMgr->getTgcReadoutElement(channelId);
		  //const MuonGM::TgcChannelElement* descriptor = m_pMuonMgr->getTgcChannelElement(channelId);
		  //Amg::Vector3D localPos  = descriptor->localChannelPos(channelId);
		  Amg::Vector3D globalPos = descriptor->channelPos(channelId);

		  int stname =m_pTgcIdHelper->stationName(channelId);
		  int steta  =m_pTgcIdHelper->stationEta(channelId);
		  int stphi  =m_pTgcIdHelper->stationPhi(channelId);
		  int isStrip=m_pTgcIdHelper->isStrip(channelId);
		  int gasGap =m_pTgcIdHelper->gasGap(channelId);
		  int channel=m_pTgcIdHelper->channel(channelId);

                  if (m_writeNtuple) {
		  m_Validation_TGC_StationName=stname;
		  m_Validation_TGC_StationEta=steta;
		  m_Validation_TGC_StationPhi=stphi;
		  m_Validation_TGC_GasGap=gasGap;
		  m_Validation_TGC_IsStrip=isStrip;
		  m_Validation_TGC_Channel=channel;
		  m_Validation_TGC_ChannelPosX=globalPos.x();
		  m_Validation_TGC_ChannelPosY=globalPos.y();
		  m_Validation_TGC_ChannelPosZ=globalPos.z();
		  m_Val_TGC_NumberOfDigits++;
		  tgctree->Fill();
                  } //end ntuple writing if statement
		}
	    }
	}

    }

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
const MuonSimData* TGCDigitsTestAlg::getMuonSimData(const Identifier& id, const MuonSimDataCollection& sdoContainer)
{
  MuonSimDataCollection::const_iterator it = sdoContainer.find(id);
  if( it == sdoContainer.end() )
    {
      return 0;
    }
  else
    {
      return &it->second;
    }
}
 
/*---------------------------------------------------------*/
ITHistSvc* TGCDigitsTestAlg::ToolRootHistSvc()
  /*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() )
    {
      log << MSG::WARNING << ">>> Unable to locate the TGCDigitsTestAlg Histogram service" << endreq;
    }
  return m_rootsvc;
}

void	TGCDigitsTestAlg::clear_variables()
{
  m_Val_TGC_RunNumber=-1;
  m_Val_TGC_EventNumber=-1;
  m_Val_TGC_NumberOfDigits=-1;
  m_Validation_TGC_StationName=-1;
  m_Validation_TGC_StationEta=-1;
  m_Validation_TGC_StationPhi=-1;
  m_Validation_TGC_GasGap=-1;
  m_Validation_TGC_IsStrip=-1;
  m_Validation_TGC_Channel=-1;
  m_Validation_TGC_ChannelPosX=-1;
  m_Validation_TGC_ChannelPosY=-1;
  m_Validation_TGC_ChannelPosZ=-1;
}


