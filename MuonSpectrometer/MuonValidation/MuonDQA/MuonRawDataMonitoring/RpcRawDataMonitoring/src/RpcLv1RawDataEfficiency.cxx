/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
// Package : RpcLv1RawDataEfficiency
// Author: Ulrike Schnoor (ulrike.schnoor@cern.ch) - P. Anger (panger@cern.ch)
//
// DESCRIPTION:
// Subject: RPCLV1--> Efficiency Offline Muon Data Quality
// RPCLv1 Sector Hits vs LB
/////////////////////////////////////////////////////////////////////////

#include "StoreGate/DataHandle.h"
 
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "RpcRawDataMonitoring/RpcLv1RawDataEfficiency.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include <inttypes.h> 

#include <sstream>

//================================================================================================================================
RpcLv1RawDataEfficiency::RpcLv1RawDataEfficiency( const std::string & type, 
						  const std::string & name, 
						  const IInterface* parent ) : ManagedMonitorToolBase( type, name, parent )
{
  declareProperty("isMC", m_isMC=false);
}

//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::initialize()
{
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_MSG_INFO( "In initializing 'RpcLv1RawDataEfficiency'"  );
  ATH_MSG_INFO( "Package version = "<< PACKAGE_VERSION  );
  
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
  
  ATH_CHECK( m_idHelperSvc.retrieve() );
// MuonDetectorManager from the conditions store
  ATH_CHECK(m_DetectorManagerKey.initialize());
  ATH_MSG_DEBUG( "Found the MuonDetectorManager from detector store."  );

  ATH_CHECK(m_rpcCoinKey.initialize());
  ATH_CHECK(m_eventInfo.initialize());
  ATH_CHECK(m_sectorLogicContainerKey.initialize(!m_isMC));
  return StatusCode::SUCCESS;
}

//================================================================================================================================
// Reads RPC coincidence data from container into vectors for use in filling histograms
//================================================================================================================================
StatusCode RpcLv1RawDataEfficiency::readRpcCoinDataContainer()
{
  ATH_MSG_DEBUG( "Reading RpcCoincidenceContainer... "  );
   
  SG::ReadHandle<Muon::RpcCoinDataContainer> rpc_coin_container(m_rpcCoinKey);
  Muon::RpcCoinDataContainer::const_iterator  it_container;
  //Muon::RpcCoinDataCollection::const_iterator it_collection;
  Identifier prdcoll_id;
  const MuonGM::RpcReadoutElement* descriptor_Atl;
  double  x_atlas, y_atlas, z_atlas, phi_atlas, eta_atlas;
  int irpcstationEta; // for the switch of the eta in the coin matrix
    
  // MuonDetectorManager from the conditions store
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
  if(MuonDetMgr==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
    return StatusCode::FAILURE; 
  } 


  for( it_container = rpc_coin_container->begin(); it_container != rpc_coin_container->end(); ++it_container ) {
    for ( Muon::RpcCoinDataCollection::const_iterator it_collection = (*it_container)->begin(); it_collection != (*it_container)->end(); ++it_collection ) { // each collection is a trigger signal
      ATH_MSG_DEBUG( "Original RPC container: " << (*it_collection)->threshold() <<int((*it_collection)->isLowPtCoin()) << int((*it_collection)->isHighPtCoin())  );
      if(int((*it_collection)->threshold())<1 || (*it_collection)->threshold()>3 || (int((*it_collection)->isLowPtCoin())+int((*it_collection)->isHighPtCoin())!=1)) continue; // some thresholds are 99 (lowpt=highpt=0)->skip
      CoincidenceData* coindata = new CoincidenceData;
      coindata->SetThresholdLowHigh(int((*it_collection)->threshold()), int((*it_collection)->isLowPtCoin()), int((*it_collection)->isHighPtCoin()));
      prdcoll_id   = (*it_collection)->identify();
      descriptor_Atl = MuonDetMgr->getRpcReadoutElement( prdcoll_id );
      irpcstationEta = int(m_idHelperSvc->rpcIdHelper().stationEta(prdcoll_id));
      x_atlas = descriptor_Atl->stripPos(prdcoll_id ).x();
      y_atlas = descriptor_Atl->stripPos(prdcoll_id ).y();
      z_atlas = descriptor_Atl->stripPos(prdcoll_id ).z();
      //obtaining phi coordinate:
      if ( x_atlas > 0 ) {
	phi_atlas = std::atan ( y_atlas / x_atlas );
      }
      else if ( x_atlas == 0 ){
	if (y_atlas > 0) phi_atlas =  CLHEP::pi/2;
	else             phi_atlas = -CLHEP::pi/2;
      }
      else{
	if (y_atlas > 0) phi_atlas = std::atan ( y_atlas / x_atlas ) + CLHEP::pi ;
	else             phi_atlas = -CLHEP::pi + std::atan ( y_atlas / x_atlas ) ;
      }
      // obtaining  pseudorapidity coordinate
      if ( z_atlas!=0  ) eta_atlas = -std::log( std::abs( std::tan( 0.5 * std::atan(std::hypot(x_atlas, y_atlas)/ z_atlas )) ) );
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
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo);
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
  if(!m_isMC){
    // Retrieve the Sector Logic container
    SG::ReadHandle<RpcSectorLogicContainer> sectorLogicContainer(m_sectorLogicContainerKey);

    RpcSectorLogicContainer::const_iterator it = sectorLogicContainer -> begin();
    
    for ( ; it != sectorLogicContainer -> end() ; ++it ) 
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
  return StatusCode::SUCCESS;
}
