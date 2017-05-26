/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
// Package : RpcLv1RawDataEfficiency
// Author: Ulrike Schnoor (ulrike.schnoor@cern.ch) - P. Anger (panger@cern.ch)
//
// DESCRIPTION:
// Subject: RPCLV1--> Efficiency Offline Muon Data Quality
// RPCLv1 Sector Hits vs LB
/////////////////////////////////////////////////////////////////////////
      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "StoreGate/DataHandle.h"
 
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "RpcRawDataMonitoring/RpcLv1RawDataEfficiency.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"

#include "xAODEventInfo/EventInfo.h"
#include <inttypes.h> 

#include <sstream>

using namespace std;



//================================================================================================================================
RpcLv1RawDataEfficiency::RpcLv1RawDataEfficiency( const std::string & type, 
						  const std::string & name, 
						  const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )
{}

//================================================================================================================================
RpcLv1RawDataEfficiency::~RpcLv1RawDataEfficiency()
{
  ATH_MSG_INFO( " Deleting RpcLv1RawDataEfficiency "  );
}

//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::initialize()
{
  ATH_MSG_INFO( "In initializing 'RpcLv1RawDataEfficiency'"  );
  ATH_MSG_INFO( "Package version = "<< PACKAGE_VERSION  );
  
  m_activeStore  = 0 ;
  m_rpcIdHelper  = 0 ;
  m_sectorLogicContainer  = 0 ;
  m_muonMgr  = 0 ;
  m_trigtype  = 0 ;
  m_event  = 0 ;
  m_lumiblock  = 0 ;
  m_BCID  = 0 ;
  m_rpclv1_sectorhits_A[0]  = 0 ;
  m_rpclv1_sectorhits_C[0]  = 0 ;  	
  m_rpclv1_sectorhits_all[0]= 0 ;
  m_rpclv1_sectorhits_A[1]  = 0 ;
  m_rpclv1_sectorhits_C[1]  = 0 ;  	
  m_rpclv1_sectorhits_all[1]= 0 ;
  m_rpclv1_sectorhits_A[2]  = 0 ;
  m_rpclv1_sectorhits_C[2]  = 0 ;  	
  m_rpclv1_sectorhits_all[2]= 0 ;
  m_rpclv1_sectorhits_A[3]  = 0 ;
  m_rpclv1_sectorhits_C[3]  = 0 ;  	
  m_rpclv1_sectorhits_all[3]= 0 ;
  m_rpclv1_sectorhits_A[4]  = 0 ;
  m_rpclv1_sectorhits_C[4]  = 0 ;  	
  m_rpclv1_sectorhits_all[4]= 0 ;
  m_rpclv1_sectorhits_A[5]  = 0 ;
  m_rpclv1_sectorhits_C[5]  = 0 ;  	
  m_rpclv1_sectorhits_all[5]= 0 ;
  
  // Retrieve the active store where all the information is being read from
  ATH_CHECK(  serviceLocator()->service("ActiveStoreSvc", m_activeStore) );

  ATH_CHECK(  detStore()->retrieve(m_rpcIdHelper,"RPCIDHELPER") );
  ATH_CHECK(  detStore()->retrieve(m_muonMgr) );
  ATH_MSG_DEBUG( "Found the MuonDetectorManager from detector store."  );

  // Ignore the checking code
  ManagedMonitorToolBase::initialize().ignore();
  
  return StatusCode::SUCCESS;
}


//================================================================================================================================
// Reads the offline muon container into vectors for use in filling histograms
//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::readOfflineMuonContainer( std::string key )
{
  ATH_MSG_VERBOSE( "Reading OfflineMuonContainer... "  );

  // read the container the first time or again, so delete the old one
  m_OfflineMuons.clear();
  
  const float ptcut = 1.0;
  const float etamin = -1.0;
  const float etamax = 1.0;
 
  const Analysis::MuonContainer* muonCont;
  
  // retrieve the offline muon container
  StatusCode sc = (*m_activeStore)->retrieve(muonCont, key);
  if(sc.isFailure()) {
    ATH_MSG_WARNING( "Container of muon particle with key " << key << " not found in ActiveStore"   );
    return StatusCode::SUCCESS;
  }

  float pt,eta;

  int pixHits;
  int sctHits;
  int trtHits;            
  int pixHoles; 
  int sctHoles; 
  int trtOL;             
  int trtOLfrac;
  bool trt;
  float matchChi2;

  Analysis::MuonContainer::const_iterator it  = muonCont->begin();
  Analysis::MuonContainer::const_iterator ite = muonCont->end();
  for (;  it != ite; it++ ) {
    pt  = (*it)->pt();
    eta = (*it)->eta();
    if( fabs(pt) < ptcut || 
	fabs(eta) < etamin ||
	fabs(eta) > etamax ) continue;
    
    pixHits  = (*it)->numberOfPixelHits();
    sctHits  = (*it)->numberOfSCTHits();
    trtHits  = (*it)->numberOfTRTHits();
    pixHoles = (*it)->numberOfPixelHoles();
    sctHoles = (*it)->numberOfSCTHoles();
    trtOL    = (*it)->numberOfTRTOutliers();
    
    trtOLfrac = 0;
    if( trtHits + trtOL > 0 ) trtOLfrac = trtOL/(trtHits + trtOL);
    
    trt=false;
    if( fabs(eta) < 1.9 ) 
      trt = ( (trtHits > 5) && (trtOLfrac < 0.9) );
    else{
      if( trtHits > 5 )
	trt = ( trtOLfrac < 0.9 );
      else
	trt=true;
    }

    matchChi2 = (*it)->matchChi2();

    //Muid MCP except phi hits
    if( key == "MuidMuonCollection" &&
	!( (*it)->combinedMuonTrackParticle() &&
	   sctHits >= 6 &&
	   pixHits >= 2 &&
	   pixHoles+sctHoles <=1 &&
           trt 
	   ) ) continue;
    
    //Staco MCP
    if( key == "StacoMuonCollection" &&
	! ( (*it)->combinedMuonTrackParticle() &&
            sctHits >= 6 &&
	    pixHits >= 2 &&
	    pixHoles+sctHoles <=1 &&
	    trt &&
	    matchChi2 < 50 ) ) continue;

    OfflineMuon offmu;
    offmu.SetPtEtaPhiQ( pt/CLHEP::GeV, eta, (*it)->phi(), (*it)->charge() );
    m_OfflineMuons.push_back(offmu);
  }
  ATH_MSG_VERBOSE( "Number of offline muons before dR check : " << m_OfflineMuons.size()  );

  // check if no track is too close to another track
  bool bTooClose = false;
  float trackdr;
  const float trackdr_min = .8;

  vector<OfflineMuon>::const_iterator it1 = m_OfflineMuons.begin();
  vector<OfflineMuon>::const_iterator it2;
  while( it1 != m_OfflineMuons.end() ) {
    it2 = it1+1;
    while( it2 != m_OfflineMuons.end() ) {
      trackdr = (*it1).DeltaR( (*it2) );
      if( trackdr < trackdr_min ) bTooClose = true;
      ++it2;}
    ++it1; }
  if(bTooClose) m_OfflineMuons.clear();

  ATH_MSG_VERBOSE( "Finished reading OfflineMuonContainer... "  );
  return StatusCode::SUCCESS;
}



//================================================================================================================================
// Reads RPC coincidence data from container into vectors for use in filling histograms
//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::readRpcCoinDataContainer()
{
  ATH_MSG_DEBUG( "Reading RpcCoincidenceContainer... "  );
   
  const Muon::RpcCoinDataContainer* rpc_coin_container;
  ATH_CHECK(  (*m_activeStore)->retrieve(rpc_coin_container, "RPC_triggerHits" ) );
  Muon::RpcCoinDataContainer::const_iterator  it_container;
  //Muon::RpcCoinDataCollection::const_iterator it_collection;
  Identifier prdcoll_id;
  const MuonGM::RpcReadoutElement* descriptor_Atl;
  double  x_atlas, y_atlas, z_atlas, phi_atlas, eta_atlas;
  int irpcstationEta; // for the switch of the eta in the coin matrix
    


  for( it_container = rpc_coin_container->begin(); it_container != rpc_coin_container->end(); ++it_container ) {
    for ( Muon::RpcCoinDataCollection::const_iterator it_collection = (*it_container)->begin(); it_collection != (*it_container)->end(); ++it_collection ) { // each collection is a trigger signal
      ATH_MSG_DEBUG( "Original RPC container: " << (*it_collection)->threshold() <<int((*it_collection)->isLowPtCoin()) << int((*it_collection)->isHighPtCoin())  );
      if(int((*it_collection)->threshold())<1 || (*it_collection)->threshold()>3 || (int((*it_collection)->isLowPtCoin())+int((*it_collection)->isHighPtCoin())!=1)) continue; // some thresholds are 99 (lowpt=highpt=0)->skip
      CoincidenceData* coindata = new CoincidenceData;
      coindata->SetThresholdLowHigh(int((*it_collection)->threshold()), int((*it_collection)->isLowPtCoin()), int((*it_collection)->isHighPtCoin()));
      prdcoll_id   = (*it_collection)->identify();
      descriptor_Atl = m_muonMgr->getRpcReadoutElement( prdcoll_id );
      irpcstationEta = int(m_rpcIdHelper->stationEta(prdcoll_id));
      x_atlas = descriptor_Atl->stripPos(prdcoll_id ).x();
      y_atlas = descriptor_Atl->stripPos(prdcoll_id ).y();
      z_atlas = descriptor_Atl->stripPos(prdcoll_id ).z();
      //obtaining phi coordinate:
      if ( x_atlas > 0 ) {
	phi_atlas = atan ( y_atlas / x_atlas );
      }
      else if ( x_atlas == 0 ){
	if (y_atlas > 0) phi_atlas =  CLHEP::pi/2;
	else             phi_atlas = -CLHEP::pi/2;
      }
      else{
	if (y_atlas > 0) phi_atlas = atan ( y_atlas / x_atlas ) + CLHEP::pi ;
	else             phi_atlas = -CLHEP::pi + atan ( y_atlas / x_atlas ) ;
      }
      // obtaining  pseudorapidity coordinate
      if ( z_atlas!=0  ) eta_atlas = -log( abs( tan( 0.5 * atan(sqrt(pow(x_atlas,2.)+pow(y_atlas,2.))/ z_atlas )) ) );
      else eta_atlas = 0;
      if ( irpcstationEta<0 ) eta_atlas = -eta_atlas;
      coindata->SetEtaPhi( eta_atlas, phi_atlas );

      m_CoincidenceData.push_back( coindata );
    }
  }
  ATH_MSG_DEBUG( "Finished with reading the RpcCoinDataContainer... "  );
  return StatusCode::SUCCESS;
}

//================================================================================================================================
// Reads the trigger type in memory
//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::StoreTriggerType() 
{
  ATH_MSG_DEBUG( "Storing Trigger Type... "  );
  const xAOD::EventInfo* eventInfo;
  ATH_CHECK( evtStore() -> retrieve(eventInfo) );
  ATH_MSG_DEBUG( "RpcLv1RawDataEfficiency::retrieved eventInfo"  );
  
  // Protection against simulated cosmics when the trigger_info() of the event_info is not filled and returns a null pointer. 
  m_trigtype = eventInfo->level1TriggerType();  

  // Get event index variables
  m_lumiblock = eventInfo->lumiBlock() ;
  m_event     = eventInfo->eventNumber() ;
  m_BCID      = eventInfo->bcid() ;
  
  return StatusCode::SUCCESS;
}

//================================================================================================================================
// Book the histogramms for nominator and denominator
//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::bookHistogramsRecurrent()
{
  ATH_MSG_INFO( "Booking Sector Hits histograms for RPCLV1RawDataEfficiency... "  );

  // not used yet, but commenting them out leads to "unused variables" warnings since they are passed as arguments to the function  

  if(newRunFlag()){ //book all histograms per new run
    std::string generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1Efficiency";
    MonGroup MG_SectorHits(this, generic_path_rpclv1monitoring + "/SectorHits", run, ATTRIB_UNMANAGED ); 

    // Sector hits
    for (unsigned int iMuThreshold = 0; iMuThreshold < 6; iMuThreshold++) {  // threshold       

      m_ss.str(""); m_ss << "SectorHits" <<   "PT" <<iMuThreshold+1 << "_C";
      m_rpclv1_sectorhits_C[iMuThreshold] = new TH2I(m_ss.str().c_str(), (m_ss.str() + ";LB;Sector").c_str() , 500, 0., 1500., 32, 0., 32.); 
      ATH_CHECK( MG_SectorHits.regHist(m_rpclv1_sectorhits_C[iMuThreshold]) );

      m_ss.str(""); m_ss << "SectorHits" <<  "PT" <<iMuThreshold+1 << "_A";
      m_rpclv1_sectorhits_A[iMuThreshold] = new TH2I(m_ss.str().c_str(), (m_ss.str() + ";LB;Sector").c_str() , 500, 0., 1500., 32, 32., 64.); 
      ATH_CHECK(  MG_SectorHits.regHist(m_rpclv1_sectorhits_A[iMuThreshold]) );

      m_ss.str(""); m_ss << "SectorHits" <<  "PT" <<iMuThreshold+1 << "_all";
      m_rpclv1_sectorhits_all[iMuThreshold] = new TH2I(m_ss.str().c_str(), (m_ss.str() + ";LB;Sector").c_str() , 500, 0., 1500., 64, 0., 64.); 
      ATH_CHECK(  MG_SectorHits.regHist(m_rpclv1_sectorhits_all[iMuThreshold]) );
    }
  }

  
  return StatusCode::SUCCESS;
}


//================================================================================================================================
// Fills the histograms for nominator and denominator
//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::fillHistograms( )
{
  ATH_MSG_DEBUG( "Filling histograms for RPCLv1RawDataEfficiency... "  );

  
  // Retreive and store trigger type for this event                                                                                                          
  if (StoreTriggerType() != StatusCode::SUCCESS)
    ATH_MSG_ERROR( "Cannot find StoreTriggerType! "  );

  // == Filling the Histograms                                                                                                                               
  //--------------------Sector Hits---------------------------------
  // Retrieve the Sector Logic container
  if ( (*m_activeStore) -> retrieve(m_sectorLogicContainer).isFailure() ) {
    ATH_MSG_INFO( "Cannot retrieve the RpcSectorLogicContainer"  );
    return StatusCode::FAILURE;
  }
  else {
		      
    RpcSectorLogicContainer::const_iterator it = m_sectorLogicContainer -> begin();

    for ( ; it != m_sectorLogicContainer -> end() ; ++it ) 
      {
	int i_sectorid = (*it)->sectorId();
	// Loop over the trigger hits of each sector
	RpcSectorLogic::const_iterator ithit = (*it) -> begin();
	for ( ; ithit != (*it) -> end() ; ++ithit ) 
	  {
	    bool b_isInput        = (*ithit) -> isInput();
	    int i_ptid   = (*ithit) -> ptId();
	    if (b_isInput == true) // fill histograms only if there was a trigger hit
	      {
		for( int iThresholdIndex=0; iThresholdIndex < 6; iThresholdIndex++) {
		  if (i_ptid == (iThresholdIndex + 1))
		    {
		      m_rpclv1_sectorhits_all[iThresholdIndex]->Fill(m_lumiblock,i_sectorid);
		      m_rpclv1_sectorhits_A[iThresholdIndex]->Fill(m_lumiblock,i_sectorid);
		      m_rpclv1_sectorhits_C[iThresholdIndex]->Fill(m_lumiblock,i_sectorid);
		    }
		}
	      }
	  }
      }
  }
  
  return StatusCode::SUCCESS;;
}

//================================================================================================================================
// Proc histograms
//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::procHistograms()
{
  ATH_MSG_INFO( "RpcLv1RawDataEfficiency finalize()"  );

  return StatusCode::SUCCESS;
}



//================================================================================================================================
// Finalizing
//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::finalize() 
{
  ATH_MSG_INFO( "RpcLv1RawDataEfficiency finalize()"  );
  return StatusCode::SUCCESS;
}
