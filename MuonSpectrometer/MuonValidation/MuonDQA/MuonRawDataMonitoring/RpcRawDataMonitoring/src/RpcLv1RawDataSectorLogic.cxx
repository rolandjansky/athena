/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
// Package : RpcLv1RawDataSectorLogic
// Authors:  A. Vest (TU Dresden), C. Rudolph (TU Dresden) 
// February 2010
//
// DESCRIPTION:
// Subject: RPCLV1-->Sector Logic Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////
      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "xAODEventInfo/EventInfo.h" 
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "RpcRawDataMonitoring/RpcLv1RawDataSectorLogic.h"

#include <inttypes.h> 

#include <sstream>

using namespace std;


//***********************************************************************************************************************
//***********************************************************************************************************************
RpcLv1RawDataSectorLogic::RpcLv1RawDataSectorLogic( const std::string & type, 
						    const std::string & name, 
						    const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )
{  
  // Declare the properties      
  declareProperty("LumiblockHist" ,      m_lumiblockhist	= false	);   
}

//***********************************************************************************************************************
RpcLv1RawDataSectorLogic::~RpcLv1RawDataSectorLogic()
{
  ATH_MSG_INFO( " Deleting RpcLv1RawDataSectorLogic "  );
}


//***********************************************************************************************************************
StatusCode RpcLv1RawDataSectorLogic::initialize()
{
  // Init message stream
  
  ATH_MSG_INFO( "In initializing 'RpcLv1RawDataSectorLogic': "           );
  ATH_MSG_INFO( "Package version = "      << PACKAGE_VERSION             );

  m_rpc_eventstotal = 0;
  m_rpc_event_inarea = 0; 
  m_nTriggerHits = 0 ;
  m_nTriggerHits_out = 0 ;
  m_nTriggerHitsperSector = 0 ;
  m_Diff_triggerBCid = 0 ;
  m_in_triggerBCid = 0 ;
  m_out_triggerBCid = 0 ;
  m_in_rowinBCid = 0 ;
  m_out_rowinBCid = 0 ;
  m_Tower_out = 0 ;
  m_Tower_in = 0 ;
  m_in_sectorid = 0 ;
  m_sectorLogicContainer = 0 ;
      
  // Retrieve the active store
  ATH_CHECK( serviceLocator() -> service("ActiveStoreSvc", m_activeStore) );

  // Ignore the checking code
  ManagedMonitorToolBase::initialize().ignore();

  return StatusCode::SUCCESS;
}


//***********************************************************************************************************************
StatusCode RpcLv1RawDataSectorLogic::StoreTriggerType() 
{
  const xAOD::EventInfo* eventInfo;
  ATH_CHECK(  evtStore() -> retrieve(eventInfo) );
  ATH_MSG_DEBUG( "RpcLv1RawDataSectorLogic::retrieved eventInfo"  );
  
  m_trigtype = eventInfo->level1TriggerType();
  
  return StatusCode::SUCCESS;
}


//***********************************************************************************************************************
StatusCode RpcLv1RawDataSectorLogic::fillHistograms( )
{
  StatusCode sc = StatusCode::SUCCESS; 
  
  if( m_environment == AthenaMonManager::tier0    || 
      m_environment == AthenaMonManager::tier0Raw || 
      m_environment == AthenaMonManager::online )  {  

    ATH_MSG_DEBUG( "RpcLv1RawDataSectorLogic::RPCLV1SectorLogic RawData Monitoring Histograms being filled"  );
    // Retreive and store trigger type for this event
    if (StoreTriggerType() != StatusCode::SUCCESS) 
      ATH_MSG_ERROR( "Cannot find StoreTriggerType !"  );
    
    ATH_MSG_DEBUG( "GetTriggerType(): "<< GetTriggerType()  );
    
    
    // Path for Sector Logic Monitoring
    std::string generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1SectorLogic";
    
    // Sector Logic histogram paths
    MonGroup rpclv1sl_shift ( this, generic_path_rpclv1monitoring + "/Overview", run, ATTRIB_UNMANAGED );
    
    if(m_lumiblockhist){
      // per lumi block
      MonGroup rpclv1sl_lumi_block ( this, generic_path_rpclv1monitoring + "/lumiblock", lumiBlock, ATTRIB_UNMANAGED )  ;
    
      // get histograms
      // per lumi block
      sc = rpclv1sl_lumi_block.getHist(m_rpclv1_TriggerHitsperEventperTriggerSector_LB,"SL_TriggerHits_per_Event_per_TriggerSector_LB");
      if( sc.isFailure()) ATH_MSG_WARNING( "SL_TriggerHits_per_Event_per_TriggerSector_LB: Histogram could not be associated to MonGroup."  );
      sc = rpclv1sl_lumi_block.getHist(m_rpclv1_Hits_per_TriggerSector_LB,"SL_Hits_per_TriggerSector_LB");	
      if( sc.isFailure() ) ATH_MSG_WARNING( "SL_Hits_per_TriggerSector_LB: Histogram could not be associated to MonGroup."  );
      sc = rpclv1sl_lumi_block.getHist( m_rpclv1_triggerBCid_inout_LB, "SL_triggerBCid_inout_LB") ;
      if( sc.isFailure() ) ATH_MSG_WARNING( "SL_triggerBCid_inout_LB: Histogram could not be associated to MonGroup."  );
    }
   
    // Overview
    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerHitsperEvent,"SL_TriggerHits_per_Event");
    if( sc.isFailure()) ATH_MSG_WARNING( "SL_TriggerHits_per_Event: Histogram could not be associated to MonGroup."  );
 
    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerHitsperEventperTriggerSector,"SL_TriggerHits_per_Event_per_TriggerSector");
    if( sc.isFailure()) ATH_MSG_WARNING( "SL_TriggerHits_per_Event_per_TriggerSector: Histogram could not be associated to MonGroup."  );
 
    sc = rpclv1sl_shift.getHist(m_rpclv1_Hits_per_TriggerSector,"SL_Hits_per_TriggerSector");	
    if( sc.isFailure() ) ATH_MSG_WARNING( "SL_Hits_per_TriggerSector: Histogram could not be associated to MonGroup."  );

    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerSector_vs_Pad_triggerBCid_inout,"SL_TriggerSector_vs_Tower_triggerBCid_inout");	
    if( sc.isFailure() ) ATH_MSG_WARNING( "rpclv1_TriggerSector_vs_Pad_triggerBCid_inout: Histogram could not be associated to MonGroup."  );

    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerSector_vs_Pad,"SL_TriggerSector_vs_Tower");	
    if( sc.isFailure() ) ATH_MSG_WARNING( "_rpclv1_TriggerSector_vs_Pad: Histogram could not be associated to MonGroup."  );
    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerSector_vs_Pad_Pt1,"SL_TriggerSector_vs_Tower_Pt1");	
    if( sc.isFailure() ) ATH_MSG_WARNING( "rpclv1_TriggerSector_vs_Pad_Pt1: Histogram could not be associated to MonGroup."  );
    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerSector_vs_Pad_Pt2,"SL_TriggerSector_vs_Tower_Pt2");	
    if( sc.isFailure() ) ATH_MSG_WARNING( "rpclv1_TriggerSector_vs_Pad_Pt2: Histogram could not be associated to MonGroup."  );
    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerSector_vs_Pad_Pt3,"SL_TriggerSector_vs_Tower_Pt3");	
    if( sc.isFailure() ) ATH_MSG_WARNING( "rpclv1_TriggerSector_vs_Pad_Pt3: Histogram could not be associated to MonGroup."  );
    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerSector_vs_Pad_Pt4,"SL_TriggerSector_vs_Tower_Pt4");	
    if( sc.isFailure() ) ATH_MSG_WARNING( "rpclv1_TriggerSector_vs_Pad_Pt4: Histogram could not be associated to MonGroup."  );
    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerSector_vs_Pad_Pt5,"SL_TriggerSector_vs_Tower_Pt5");	
    if( sc.isFailure() ) ATH_MSG_WARNING( "rpclv1_TriggerSector_vs_Pad_Pt5: Histogram could not be associated to MonGroup."  );
    sc = rpclv1sl_shift.getHist(m_rpclv1_TriggerSector_vs_Pad_Pt6,"SL_TriggerSector_vs_Tower_Pt6");	
    if( sc.isFailure() ) ATH_MSG_WARNING( "rpclv1_TriggerSector_vs_Pad_Pt6: Histogram could not be associated to MonGroup."  );

    sc = rpclv1sl_shift.getHist(m_rpclv1_triggerBCid_inout_vs_TriggerSector,"SL_triggerBCid_inout_vs_TriggerSector");
    if( sc.isFailure() ) ATH_MSG_WARNING( "SL_triggerBCid_inout_vs_TriggerSector: Histogram could not be associated to MonGroup."  );

    sc = rpclv1sl_shift.getHist( m_rpclv1_rowinBCid_vs_TriggerSector, "SL_rowinBCid_vs_TriggerSector") ;
    if( sc.isFailure() ) ATH_MSG_WARNING( "SL_rowinBCid_vs_TriggerSector: Histogram could not be associated to MonGroup."  );
    sc = rpclv1sl_shift.getHist( m_rpclv1_triggerBCid_inout_vs_Tower , "SL_triggerBCid_inout_vs_Tower") ;
    if( sc.isFailure() ) ATH_MSG_WARNING( "SL_triggerBCid_inout_vs_Tower: Histogram could not be associated to MonGroup."  );
    sc = rpclv1sl_shift.getHist( m_rpclv1_ptid_vs_Tower, "SL_ptid_vs_Tower") ;
    if( sc.isFailure() ) ATH_MSG_WARNING( "SL_ptid_vs_Tower: Histogram could not be associated to MonGroup."  );

    sc = rpclv1sl_shift.getHist( m_rpclv1_triggerBCid_inout, "SL_triggerBCid_inout") ;
    if( sc.isFailure() ) ATH_MSG_WARNING( "SL_triggerBCid_inout: Histogram could not be associated to MonGroup."  );


    //---------------------------------------
    // Fill the Sector Logic histograms
    //---------------------------------------
    
    m_nTriggerHits = 0;

    // Retrieve the Sector Logic container
    sc = (*m_activeStore) -> retrieve(m_sectorLogicContainer);     
    
    if (sc.isFailure()) {
      ATH_MSG_INFO( "Cannot retrieve the RpcSectorLogicContainer"  );
      return StatusCode::SUCCESS;
    }
    else {
      ///////////////////////////////////////////
      // Loop over the Sector Logic containers //
      ///////////////////////////////////////////
      RpcSectorLogicContainer::const_iterator it = m_sectorLogicContainer -> begin();
      for ( ; it != m_sectorLogicContainer -> end() ; ++it ) 
	{
	  int i_sectorid = (*it)->sectorId();
	  m_nTriggerHitsperSector = 0;

	  // Loop over the trigger hits of each sector
	  RpcSectorLogic::const_iterator ithit = (*it) -> begin();
	  for ( ; ithit != (*it) -> end() ; ++ithit ) 
          {
            // from RpcSLTriggerHit
            bool b_isInput        = (*ithit) -> isInput();
            int i_rowinBcid      = (*ithit) -> rowinBcid();//readout window BCid
            int i_padid          = (*ithit) -> padId();//tower
            int i_ptid           = (*ithit) -> ptId();//threshold
            int i_roi            = (*ithit) -> roi();//region of interest
            int i_triggerBcid    = (*ithit) -> triggerBcid();

            m_Diff_triggerBCid = 0;
            m_in_triggerBCid = 0;
            m_out_triggerBCid = 0;
            m_in_rowinBCid = 0;
            m_out_rowinBCid = 0;
            m_Tower_in = -1;

            if (b_isInput == true){ 
              m_nTriggerHits++;
              m_nTriggerHitsperSector++;
		
              //Fill hits per trigger sector histogram
              m_rpclv1_Hits_per_TriggerSector -> Fill(float(i_sectorid));
              //per lumi block
              if(m_lumiblockhist)m_rpclv1_Hits_per_TriggerSector_LB -> Fill(float(i_sectorid));

              //Fill Pad vs. tigger sector histograms
              if (i_sectorid < 32) m_rpclv1_TriggerSector_vs_Pad -> Fill(float(-(i_padid + 1)), float(i_sectorid));
              else m_rpclv1_TriggerSector_vs_Pad -> Fill(float((i_padid + 1)), float(i_sectorid - 32));
		
              if (i_ptid == 1){
                if (i_sectorid < 32) m_rpclv1_TriggerSector_vs_Pad_Pt1 -> Fill(float(-(i_padid + 1)), float(i_sectorid));
                else m_rpclv1_TriggerSector_vs_Pad_Pt1 -> Fill(float((i_padid + 1)), float(i_sectorid - 32));
              }
              if (i_ptid == 2){
                if (i_sectorid < 32) m_rpclv1_TriggerSector_vs_Pad_Pt2 -> Fill(float(-(i_padid + 1)), float(i_sectorid));
                else m_rpclv1_TriggerSector_vs_Pad_Pt2 -> Fill(float((i_padid + 1)), float(i_sectorid - 32));
              }
              if (i_ptid == 3){
                if (i_sectorid < 32) m_rpclv1_TriggerSector_vs_Pad_Pt3 -> Fill(float(-(i_padid + 1)), float(i_sectorid));
                else m_rpclv1_TriggerSector_vs_Pad_Pt3 -> Fill(float((i_padid + 1)), float(i_sectorid - 32));
              }
              if (i_ptid == 4){
                if (i_sectorid < 32) m_rpclv1_TriggerSector_vs_Pad_Pt4 -> Fill(float(-(i_padid + 1)), float(i_sectorid));
                else m_rpclv1_TriggerSector_vs_Pad_Pt4 -> Fill(float((i_padid + 1)), float(i_sectorid - 32));
              }
              if (i_ptid == 5){
                if (i_sectorid < 32) m_rpclv1_TriggerSector_vs_Pad_Pt5 -> Fill(float(-(i_padid + 1)), float(i_sectorid));
                else m_rpclv1_TriggerSector_vs_Pad_Pt5 -> Fill(float((i_padid + 1)), float(i_sectorid - 32));
              }
              if (i_ptid == 6){
                if (i_sectorid < 32) m_rpclv1_TriggerSector_vs_Pad_Pt6 -> Fill(float(-(i_padid + 1)), float(i_sectorid));
                else m_rpclv1_TriggerSector_vs_Pad_Pt6 -> Fill(float((i_padid + 1)), float(i_sectorid - 32));
              }
		
              //Fill trigger sector vs. rowin BCid histogram
              m_rpclv1_rowinBCid_vs_TriggerSector -> Fill(float(i_sectorid), float(i_rowinBcid));

              //Fill Threshold vs. Pad(Tower) histogram
              m_rpclv1_ptid_vs_Tower -> Fill(float(i_padid), float(i_ptid));

              //Trigger BCid in
              m_in_triggerBCid = i_triggerBcid;
              //ReadOutWindow BCid in
              m_in_rowinBCid = i_rowinBcid;

              //Tower in
              m_Tower_in = i_padid;
		
              m_Tower_out = -1;
              // Loop over the trigger hits of each sector
              RpcSectorLogic::const_iterator ithiti = (*it) -> begin();
              for ( ; ithiti != (*it) -> end() ; ++ithiti ) 
              {
                bool b_isInput1        = (*ithiti) -> isInput();
                int i_roi1            = (*ithiti) -> roi();//region of interest
                int i_triggerBcid1    = (*ithiti) -> triggerBcid();
                int i_rowinBcid1      = (*ithiti) -> rowinBcid();
                if (b_isInput1 == false){ 

                  //Tower out calculated from ROI out
                  if ( i_roi1 < 4 ) m_Tower_out = 0; 
                  if ( i_roi1 >= 4 && i_roi < 8 ) m_Tower_out = 1; 
                  if ( i_roi1 >= 8 && i_roi < 12 ) m_Tower_out = 2; 
                  if ( i_roi1 >= 12 && i_roi < 16 ) m_Tower_out = 3; 
                  if ( i_roi1 >= 16 && i_roi < 20 ) m_Tower_out = 4; 
                  if ( i_roi1 >= 20 && i_roi < 24 ) m_Tower_out = 5; 
                  if ( i_roi1 >= 24 ) m_Tower_out = 6; 
		      
                  //Trigger BCid out
                  m_out_triggerBCid = i_triggerBcid1;
                  //ReadOutWindow BCid in
                  m_out_rowinBCid = i_rowinBcid1;

                  //Calculate Timing difference in trigger BCid
                  if ((m_Tower_out == m_Tower_in) && (m_out_rowinBCid == m_in_rowinBCid)){
                    if(m_in_triggerBCid - m_out_triggerBCid >= 0) m_Diff_triggerBCid = m_in_triggerBCid - m_out_triggerBCid;
                    else  m_Diff_triggerBCid = (m_in_triggerBCid - m_out_triggerBCid) + 8;
                  }
                }
		    
              }
            }
	      

            //Fill in/out dependent histograms & histograms with timing difference only for IN
            if (b_isInput == true){ 

              m_rpclv1_triggerBCid_inout_vs_Tower -> Fill(float(m_Tower_out), float(m_Diff_triggerBCid));

              // if (i_padid == 0){ 
              m_rpclv1_triggerBCid_inout_vs_TriggerSector -> Fill(float(i_sectorid), float(m_Diff_triggerBCid));
              m_rpclv1_triggerBCid_inout -> Fill(float(m_Diff_triggerBCid));
              //per lumi block
              if(m_lumiblockhist)m_rpclv1_triggerBCid_inout_LB -> Fill(float(m_Diff_triggerBCid));
              // }

              if (i_sectorid < 32) m_rpclv1_TriggerSector_vs_Pad_triggerBCid_inout -> Fill(float(-(i_padid + 1)), float(i_sectorid), float(m_Diff_triggerBCid));
              else m_rpclv1_TriggerSector_vs_Pad_triggerBCid_inout -> Fill(float((i_padid + 1)), float(i_sectorid - 32), float(m_Diff_triggerBCid));
            }
		
          } // End Loop over the trigger hits of each sector	  
	  
	  // Fill the trigger hits per event per sector histogram here
	  if (m_nTriggerHitsperSector > 0) {
	    m_rpclv1_TriggerHitsperEventperTriggerSector -> Fill(float(i_sectorid), float(m_nTriggerHitsperSector));
	    //per lumi block
	    if(m_lumiblockhist)m_rpclv1_TriggerHitsperEventperTriggerSector_LB -> Fill(float(i_sectorid), float(m_nTriggerHitsperSector));
	  }

	} // End Loop over the Sector Logic containers
      
      // Fill the trigger hits per event histogram here
      m_rpclv1_TriggerHitsperEvent -> Fill(float(m_nTriggerHits));

    }
    
  } // AthenaMonManager::tier0 || AthenaMonManager::tier0Raw   
  
  return sc;
} 


//***********************************************************************************************************************
StatusCode RpcLv1RawDataSectorLogic::bookHistogramsRecurrent()
{
  ATH_MSG_DEBUG( "Booking Histograms for RPC RawData Sector Logic Monitoring"  );

  StatusCode sc = StatusCode::SUCCESS; 
  
  if( m_environment == AthenaMonManager::tier0    || 
      m_environment == AthenaMonManager::tier0Raw || 
      m_environment == AthenaMonManager::online )  {     
    
    //Declare the Sector Logic histograms
    std::string generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1SectorLogic";
     
    MonGroup rpclv1sl_shift ( this, generic_path_rpclv1monitoring + "/Overview", run, ATTRIB_UNMANAGED );
    
    if(newLumiBlockFlag() && m_lumiblockhist){
      MonGroup rpclv1sl_lumi_block ( this, generic_path_rpclv1monitoring + "/lumiblock", lumiBlock, ATTRIB_UNMANAGED )  ;
      ATH_MSG_DEBUG( "RUN : "  << run  );
      ATH_MSG_DEBUG( "LumiBlock : "   << lumiBlock  );
      
      //_____________________________________________________________________
      // Sector logic statistics

      std::string rpclv1_Hits_per_TriggerSector_LB_title = "SL_Hits_per_TriggerSector_LB";    
      const char* rpclv1_Hits_per_TriggerSector_LB_title_char = rpclv1_Hits_per_TriggerSector_LB_title.c_str ();

      TH1 *rpclv1_Hits_per_TriggerSector_LB = new TH1I(rpclv1_Hits_per_TriggerSector_LB_title_char, rpclv1_Hits_per_TriggerSector_LB_title_char, 64, 0, 64);

      ATH_CHECK(  rpclv1sl_lumi_block.regHist(rpclv1_Hits_per_TriggerSector_LB) );
      rpclv1_Hits_per_TriggerSector_LB -> SetFillColor(9);
      rpclv1_Hits_per_TriggerSector_LB -> GetXaxis() -> SetTitle("RPC trigger sector");
      rpclv1_Hits_per_TriggerSector_LB -> GetYaxis() -> SetTitle("Counts per trigger sector"); 
      
      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_Hits_per_TriggerSector_LB  );
      
      

      //_____________________________________________________________________
      // Trigger hits per event per Trigger Sector
      std::string rpclv1_TriggerHitsperEventperTriggerSector_LB_title      = "SL_TriggerHits_per_Event_per_TriggerSector_LB";     
      const char* rpclv1_TriggerHitsperEventperTriggerSector_LB_title_char = rpclv1_TriggerHitsperEventperTriggerSector_LB_title.c_str();
      
      // Create Histogram
      TH2 *rpclv1_TriggerHitsperEventperTriggerSector_LB = new TH2I(rpclv1_TriggerHitsperEventperTriggerSector_LB_title_char, rpclv1_TriggerHitsperEventperTriggerSector_LB_title_char, 64, 0, 64, 30, -0.5, 29.5);
 
      ATH_CHECK( rpclv1sl_lumi_block.regHist(rpclv1_TriggerHitsperEventperTriggerSector_LB) );
      rpclv1_TriggerHitsperEventperTriggerSector_LB -> SetOption("COLZ");
      rpclv1_TriggerHitsperEventperTriggerSector_LB -> GetXaxis() -> SetTitle("RPC trigger sector");
      rpclv1_TriggerHitsperEventperTriggerSector_LB -> GetYaxis() -> SetTitle("RPC trigger hits / event");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_TriggerHitsperEventperTriggerSector_LB  );


      //_____________________________________________________________________
      // trigger BCid in vs. out per Lumi Block

      std::string rpclv1_triggerBCid_inout_LB_title = "SL_triggerBCid_inout_LB";
      const char* rpclv1_triggerBCid_inout_LB_title_char = rpclv1_triggerBCid_inout_LB_title.c_str();

      TH1 * rpclv1_triggerBCid_inout_LB = new TH1I(rpclv1_triggerBCid_inout_LB_title_char, rpclv1_triggerBCid_inout_LB_title_char, 8, 0, 8);
 
      ATH_CHECK(  rpclv1sl_lumi_block.regHist(rpclv1_triggerBCid_inout_LB) );
      rpclv1_triggerBCid_inout_LB -> SetFillColor(9);
      rpclv1_triggerBCid_inout_LB -> GetXaxis() -> SetTitle("triggerBCid (in - out)");
      rpclv1_triggerBCid_inout_LB -> GetYaxis() -> SetTitle("Counts");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_triggerBCid_inout_LB << rpclv1_triggerBCid_inout_LB_title.c_str()  );
	
	
    }// isNewLumiBlock
    
    
    if(newRunFlag()) {
      
      ATH_MSG_DEBUG( "RUN : "  << run  );

      //_____________________________________________________________________
      // rowin BCid vs Trigger Sector
      std::string rpclv1_rowinBCid_vs_TriggerSector_title = "SL_rowinBCid_vs_TriggerSector";
      const char* rpclv1_rowinBCid_vs_TriggerSector_title_char = rpclv1_rowinBCid_vs_TriggerSector_title.c_str();
	
      TH2* rpclv1_rowinBCid_vs_TriggerSector = new TH2I ( rpclv1_rowinBCid_vs_TriggerSector_title_char, rpclv1_rowinBCid_vs_TriggerSector_title_char, 64, 0, 64, 8, 0, 8);

      ATH_CHECK(  rpclv1sl_shift.regHist( rpclv1_rowinBCid_vs_TriggerSector ) );
      rpclv1_rowinBCid_vs_TriggerSector -> GetXaxis() -> SetTitle("RPC trigger sector");  
      rpclv1_rowinBCid_vs_TriggerSector -> GetYaxis() -> SetTitle("rowinBCid");
      rpclv1_rowinBCid_vs_TriggerSector -> SetOption("COLZ");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_rowinBCid_vs_TriggerSector << rpclv1_rowinBCid_vs_TriggerSector_title.c_str()  );
	
	


      //_____________________________________________________________________
      // trigger BCid (in - out) vs Tower
      std::string rpclv1_triggerBCid_inout_vs_Tower_title = "SL_triggerBCid_inout_vs_Tower";
      const char* rpclv1_triggerBCid_inout_vs_Tower_title_char = rpclv1_triggerBCid_inout_vs_Tower_title.c_str();
	
      TH2* rpclv1_triggerBCid_inout_vs_Tower = new TH2I ( rpclv1_triggerBCid_inout_vs_Tower_title_char, rpclv1_triggerBCid_inout_vs_Tower_title_char, 8, 0, 8, 8, 0, 8);
      
      ATH_CHECK(  rpclv1sl_shift.regHist( rpclv1_triggerBCid_inout_vs_Tower ) );
      rpclv1_triggerBCid_inout_vs_Tower -> GetXaxis() -> SetTitle("Tower");  
      rpclv1_triggerBCid_inout_vs_Tower -> GetYaxis() -> SetTitle("triggerBCid (in - out)");
      rpclv1_triggerBCid_inout_vs_Tower -> SetOption("COLZ");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " <<  rpclv1_triggerBCid_inout_vs_Tower << rpclv1_triggerBCid_inout_vs_Tower_title.c_str()  );



      //_____________________________________________________________________
      // Sector logic statistics

      std::string rpclv1_Hits_per_TriggerSector_title = "SL_Hits_per_TriggerSector";    
      const char* rpclv1_Hits_per_TriggerSector_title_char = rpclv1_Hits_per_TriggerSector_title.c_str ();

      TH1 *rpclv1_Hits_per_TriggerSector = new TH1I(rpclv1_Hits_per_TriggerSector_title_char, rpclv1_Hits_per_TriggerSector_title_char, 64, 0, 64);

      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_Hits_per_TriggerSector) );
      rpclv1_Hits_per_TriggerSector -> SetFillColor(9);
      rpclv1_Hits_per_TriggerSector -> GetXaxis() -> SetTitle("RPC trigger sector");
      rpclv1_Hits_per_TriggerSector -> GetYaxis() -> SetTitle("Counts per trigger sector"); 
      
      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_Hits_per_TriggerSector  );
    
      //_____________________________________________________________________
      // Trigger hits per event
      std::string rpclv1_TriggerHitsperEvent_title      = "SL_TriggerHits_per_Event";     
      const char* rpclv1_TriggerHitsperEvent_title_char = rpclv1_TriggerHitsperEvent_title.c_str();
      
      // Create Histogram
      TH1 *rpclv1_TriggerHitsperEvent = new TH1I(rpclv1_TriggerHitsperEvent_title_char, rpclv1_TriggerHitsperEvent_title_char, 30, -0.5, 29.5);
 
      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_TriggerHitsperEvent) );
      rpclv1_TriggerHitsperEvent -> SetFillColor(9);
      rpclv1_TriggerHitsperEvent -> GetXaxis() -> SetTitle("RPC trigger hits / event");
      rpclv1_TriggerHitsperEvent -> GetYaxis() -> SetTitle("Counts");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_TriggerHitsperEvent  );
 
 

      //_____________________________________________________________________
      // Trigger hits per event per Trigger Sector
      std::string rpclv1_TriggerHitsperEventperTriggerSector_title      = "SL_TriggerHits_per_Event_per_TriggerSector";     
      const char* rpclv1_TriggerHitsperEventperTriggerSector_title_char = rpclv1_TriggerHitsperEventperTriggerSector_title.c_str();
      
      // Create Histogram
      TH2 *rpclv1_TriggerHitsperEventperTriggerSector = new TH2I(rpclv1_TriggerHitsperEventperTriggerSector_title_char, rpclv1_TriggerHitsperEventperTriggerSector_title_char, 64, 0, 64, 30, -0.5, 29.5);
 
      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_TriggerHitsperEventperTriggerSector) );
      rpclv1_TriggerHitsperEventperTriggerSector -> SetOption("COLZ");
      rpclv1_TriggerHitsperEventperTriggerSector -> GetXaxis() -> SetTitle("RPC trigger sector");
      rpclv1_TriggerHitsperEventperTriggerSector -> GetYaxis() -> SetTitle("RPC trigger hits / event");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_TriggerHitsperEventperTriggerSector  );
 

      //_____________________________________________________________________
      //TriggerSector_vs_Tower

      std::string rpclv1_TriggerSector_vs_Pad_triggerBCid_inout_title = "SL_TriggerSector_vs_Tower_triggerBCid_inout";    
      const char* rpclv1_TriggerSector_vs_Pad_triggerBCid_inout_title_char = rpclv1_TriggerSector_vs_Pad_triggerBCid_inout_title.c_str ();     

      TH2 *rpclv1_TriggerSector_vs_Pad_triggerBCid_inout = new TH2I(rpclv1_TriggerSector_vs_Pad_triggerBCid_inout_title_char,rpclv1_TriggerSector_vs_Pad_triggerBCid_inout_title_char, 17, -8.5, 8.5, 32, 0, 32);

      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_TriggerSector_vs_Pad_triggerBCid_inout) );
      rpclv1_TriggerSector_vs_Pad_triggerBCid_inout -> SetOption("COLZ");
      rpclv1_TriggerSector_vs_Pad_triggerBCid_inout -> GetXaxis() -> SetTitle("Tower");
      rpclv1_TriggerSector_vs_Pad_triggerBCid_inout -> GetYaxis() -> SetTitle("RPC trigger sector");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_TriggerSector_vs_Pad_triggerBCid_inout  );

      //_____________________________________________________________________
      //TriggerSector_vs_Tower

      std::string rpclv1_TriggerSector_vs_Pad_title = "SL_TriggerSector_vs_Tower";    
      const char* rpclv1_TriggerSector_vs_Pad_title_char = rpclv1_TriggerSector_vs_Pad_title.c_str ();     

      TH2 *rpclv1_TriggerSector_vs_Pad = new TH2I(rpclv1_TriggerSector_vs_Pad_title_char,rpclv1_TriggerSector_vs_Pad_title_char, 17, -8.5, 8.5, 32, 0, 32);

      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_TriggerSector_vs_Pad) );
      rpclv1_TriggerSector_vs_Pad -> SetOption("COLZ");
      rpclv1_TriggerSector_vs_Pad -> GetXaxis() -> SetTitle("Tower");
      rpclv1_TriggerSector_vs_Pad -> GetYaxis() -> SetTitle("RPC trigger sector");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_TriggerSector_vs_Pad  );

      //_____________________________________________________________________
      //TriggerSector_vs_Tower_Pt1
      std::string rpclv1_TriggerSector_vs_Pad_Pt1_title = "SL_TriggerSector_vs_Tower_Pt1";    
      const char* rpclv1_TriggerSector_vs_Pad_Pt1_title_char = rpclv1_TriggerSector_vs_Pad_Pt1_title.c_str ();     

      TH2 *rpclv1_TriggerSector_vs_Pad_Pt1 = new TH2I(rpclv1_TriggerSector_vs_Pad_Pt1_title_char,rpclv1_TriggerSector_vs_Pad_Pt1_title_char, 17, -8.5, 8.5, 32, 0, 32);

      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_TriggerSector_vs_Pad_Pt1) );
      rpclv1_TriggerSector_vs_Pad_Pt1 -> SetOption("COLZ");
      rpclv1_TriggerSector_vs_Pad_Pt1 -> GetXaxis() -> SetTitle("Tower");
      rpclv1_TriggerSector_vs_Pad_Pt1 -> GetYaxis() -> SetTitle("RPC trigger sector");

      //TriggerSector_vs_Tower_Pt2
      std::string rpclv1_TriggerSector_vs_Pad_Pt2_title = "SL_TriggerSector_vs_Tower_Pt2";    
      const char* rpclv1_TriggerSector_vs_Pad_Pt2_title_char = rpclv1_TriggerSector_vs_Pad_Pt2_title.c_str ();     

      TH2 *rpclv1_TriggerSector_vs_Pad_Pt2 = new TH2I(rpclv1_TriggerSector_vs_Pad_Pt2_title_char,rpclv1_TriggerSector_vs_Pad_Pt2_title_char, 17, -8.5, 8.5, 32, 0, 32);

      ATH_CHECK( rpclv1sl_shift.regHist(rpclv1_TriggerSector_vs_Pad_Pt2) );
      rpclv1_TriggerSector_vs_Pad_Pt2 -> SetOption("COLZ");
      rpclv1_TriggerSector_vs_Pad_Pt2 -> GetXaxis() -> SetTitle("Tower");
      rpclv1_TriggerSector_vs_Pad_Pt2 -> GetYaxis() -> SetTitle("RPC trigger sector");

      //TriggerSector_vs_Tower_Pt3
      std::string rpclv1_TriggerSector_vs_Pad_Pt3_title = "SL_TriggerSector_vs_Tower_Pt3";    
      const char* rpclv1_TriggerSector_vs_Pad_Pt3_title_char = rpclv1_TriggerSector_vs_Pad_Pt3_title.c_str ();     

      TH2 *rpclv1_TriggerSector_vs_Pad_Pt3 = new TH2I(rpclv1_TriggerSector_vs_Pad_Pt3_title_char,rpclv1_TriggerSector_vs_Pad_Pt3_title_char, 17, -8.5, 8.5, 32, 0, 32);

      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_TriggerSector_vs_Pad_Pt3) );
      rpclv1_TriggerSector_vs_Pad_Pt3 -> SetOption("COLZ");
      rpclv1_TriggerSector_vs_Pad_Pt3 -> GetXaxis() -> SetTitle("Tower");
      rpclv1_TriggerSector_vs_Pad_Pt3 -> GetYaxis() -> SetTitle("RPC trigger sector");

      //TriggerSector_vs_Tower_Pt4
      std::string rpclv1_TriggerSector_vs_Pad_Pt4_title = "SL_TriggerSector_vs_Tower_Pt4";    
      const char* rpclv1_TriggerSector_vs_Pad_Pt4_title_char = rpclv1_TriggerSector_vs_Pad_Pt4_title.c_str ();     

      TH2 *rpclv1_TriggerSector_vs_Pad_Pt4 = new TH2I(rpclv1_TriggerSector_vs_Pad_Pt4_title_char,rpclv1_TriggerSector_vs_Pad_Pt4_title_char, 17, -8.5, 8.5, 32, 0, 32);

      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_TriggerSector_vs_Pad_Pt4) );
      rpclv1_TriggerSector_vs_Pad_Pt4 -> SetOption("COLZ");
      rpclv1_TriggerSector_vs_Pad_Pt4 -> GetXaxis() -> SetTitle("Tower");
      rpclv1_TriggerSector_vs_Pad_Pt4 -> GetYaxis() -> SetTitle("RPC trigger sector");

      //TriggerSector_vs_Tower_Pt5
      std::string rpclv1_TriggerSector_vs_Pad_Pt5_title = "SL_TriggerSector_vs_Tower_Pt5";    
      const char* rpclv1_TriggerSector_vs_Pad_Pt5_title_char = rpclv1_TriggerSector_vs_Pad_Pt5_title.c_str ();     

      TH2 *rpclv1_TriggerSector_vs_Pad_Pt5 = new TH2I(rpclv1_TriggerSector_vs_Pad_Pt5_title_char,rpclv1_TriggerSector_vs_Pad_Pt5_title_char, 17, -8.5, 8.5, 32, 0, 32);

      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_TriggerSector_vs_Pad_Pt5) );
      rpclv1_TriggerSector_vs_Pad_Pt5 -> SetOption("COLZ");
      rpclv1_TriggerSector_vs_Pad_Pt5 -> GetXaxis() -> SetTitle("Tower");
      rpclv1_TriggerSector_vs_Pad_Pt5 -> GetYaxis() -> SetTitle("RPC trigger sector");

      //TriggerSector_vs_Tower_Pt6
      std::string rpclv1_TriggerSector_vs_Pad_Pt6_title = "SL_TriggerSector_vs_Tower_Pt6";    
      const char* rpclv1_TriggerSector_vs_Pad_Pt6_title_char = rpclv1_TriggerSector_vs_Pad_Pt6_title.c_str ();     

      TH2 *rpclv1_TriggerSector_vs_Pad_Pt6 = new TH2I(rpclv1_TriggerSector_vs_Pad_Pt6_title_char,rpclv1_TriggerSector_vs_Pad_Pt6_title_char, 17, -8.5, 8.5, 32, 0, 32);

      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_TriggerSector_vs_Pad_Pt6) );
      rpclv1_TriggerSector_vs_Pad_Pt6 -> SetOption("COLZ");
      rpclv1_TriggerSector_vs_Pad_Pt6 -> GetXaxis() -> SetTitle("Tower");
      rpclv1_TriggerSector_vs_Pad_Pt6 -> GetYaxis() -> SetTitle("RPC trigger sector");



      //_____________________________________________________________________
      //trigger BCid in vs. out

      std::string rpclv1_triggerBCid_inout_vs_TriggerSector_title = "SL_triggerBCid_inout_vs_TriggerSector";    
      const char* rpclv1_triggerBCid_inout_vs_TriggerSector_title_char = rpclv1_triggerBCid_inout_vs_TriggerSector_title.c_str ();     

      TH2 *rpclv1_triggerBCid_inout_vs_TriggerSector = new TH2I(rpclv1_triggerBCid_inout_vs_TriggerSector_title_char,rpclv1_triggerBCid_inout_vs_TriggerSector_title_char, 64, 0, 64, 8, 0, 8);

      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_triggerBCid_inout_vs_TriggerSector) );
      rpclv1_triggerBCid_inout_vs_TriggerSector -> SetOption("COLZ");
      rpclv1_triggerBCid_inout_vs_TriggerSector -> GetXaxis() -> SetTitle("RPC trigger sector");
      rpclv1_triggerBCid_inout_vs_TriggerSector -> GetYaxis() -> SetTitle("triggerBCid (in - out)");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_triggerBCid_inout_vs_TriggerSector  );

	
	
      //_____________________________________________________________________
      // PtID (Threshold)

      std::string rpclv1_ptid_vs_Tower_title = "SL_ptid_vs_Tower";
      const char* rpclv1_ptid_vs_Tower_title_char = rpclv1_ptid_vs_Tower_title.c_str();

      TH2 * rpclv1_ptid_vs_Tower = new TH2I(rpclv1_ptid_vs_Tower_title_char, rpclv1_ptid_vs_Tower_title_char, 8, 0, 8, 6, 1, 7);
 
      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_ptid_vs_Tower) );
      rpclv1_ptid_vs_Tower -> SetOption("COLZ");
      rpclv1_ptid_vs_Tower -> GetXaxis() -> SetTitle("Tower");
      rpclv1_ptid_vs_Tower -> GetYaxis() -> SetTitle("Threshold");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_ptid_vs_Tower << rpclv1_ptid_vs_Tower_title.c_str()  );
	
	
      //_____________________________________________________________________
      // trigger BCid in vs. out

      std::string rpclv1_triggerBCid_inout_title = "SL_triggerBCid_inout";
      const char* rpclv1_triggerBCid_inout_title_char = rpclv1_triggerBCid_inout_title.c_str();

      TH1 * rpclv1_triggerBCid_inout = new TH1I(rpclv1_triggerBCid_inout_title_char, rpclv1_triggerBCid_inout_title_char, 8, 0, 8);
 
      ATH_CHECK(  rpclv1sl_shift.regHist(rpclv1_triggerBCid_inout) );
      rpclv1_triggerBCid_inout -> SetFillColor(9);
      rpclv1_triggerBCid_inout -> GetXaxis() -> SetTitle("triggerBCid (in - out)");
      rpclv1_triggerBCid_inout -> GetYaxis() -> SetTitle("Counts");

      ATH_MSG_DEBUG( "INSIDE bookHistograms successfully: " << rpclv1_triggerBCid_inout << rpclv1_triggerBCid_inout_title.c_str()  );
	
	
 

    } // isNewRun  
    
  } // AthenaMonManager::tier0 || AthenaMonManager::tier0Raw 
  
  return sc;
}



//***********************************************************************************************************************
StatusCode RpcLv1RawDataSectorLogic::procHistograms() 
{

  ATH_MSG_DEBUG( "RpcLv1RawDataSectorLogic finalize()"  );
  return StatusCode::SUCCESS;
}


//***********************************************************************************************************************
StatusCode RpcLv1RawDataSectorLogic::finalize() 
{
  ATH_MSG_DEBUG( "RpcLv1RawDataSectorLogic finalize()"  );
   
  return StatusCode::SUCCESS;
}
