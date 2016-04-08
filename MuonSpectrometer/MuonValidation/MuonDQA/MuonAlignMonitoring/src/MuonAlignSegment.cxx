/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************************
   MuonAlignSegment.cxx

   Author:      Dominique Fortin - Triumf
   created:     Feb 2009
   Description: Implementation code for the validation of the alignment
   using segments only (no track building)
	        
*********************************************************************/

// #include "MuonAlignMonitoring/MuonAlignSegment.h"
 
// // Id-related includes
// #include "Identifier/Range.h" 
// #include "Identifier/IdentifierHash.h"
// #include "IdDictDetDescr/IdDictManager.h"

// // DetDescr includes
// #include "MuonIdHelpers/MuonIdHelper.h"
// #include "MuonIdHelpers/MdtIdHelper.h"
// #include "MuonIdHelpers/CscIdHelper.h"
 
// #include "MuonReadoutGeometry/MuonDetectorManager.h"
// #include "MuonReadoutGeometry/MdtReadoutElement.h"
// #include "MuonReadoutGeometry/CscReadoutElement.h"

// // Muon Helpers
// #include "MuonRecHelperTools/MuonEDMPrinterTool.h"
// #include "MuonRecHelperTools/MuonEDMHelperTool.h"
// #include "MuonIdHelpers/MuonIdHelperTool.h" 
// #include "MuonIdHelpers/MuonStationIndex.h"

// // Muon Segment + Quality
// #include "MuonSegment/MuonSegment.h"
// #include "MuonSegment/MuonSegmentQuality.h"

// // Track info
// #include "TrkSegment/Segment.h"
// #include "TrkSegment/SegmentCollection.h"
// #include "TrkEventPrimitives/FitQuality.h"

// #include <algorithm>
// #include <math.h>
// #include <functional>
// #include <iostream>
// #include <vector>


// /* **********************************************************
//    Constructor
//    ********************************************************** */
// MuonAlignSegment::MuonAlignSegment( const std::string & type, const std::string & name, const IInterface* parent )
//   : ManagedMonitorToolBase( type, name, parent ), 
//     m_activeStore(0),
//     m_storeGate(0),
//     p_muonMgr(0),
//     p_mdtIdHelper(0),
//     p_cscIdHelper(0),
//     p_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
//     p_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
//     m_thistSvc(0)
// {
//   m_histoTool  = ToolHandle<Muon::MuonDQAHistTool>("Muon::MuonDQAHistTool/MuonDQAHistTool");
//   /** switches to control the analysis through job options */
//   declareProperty( "WhichSegmCollections",  m_segmCollectionFlag);
//   declareProperty( "SegmentCollectionName", m_segmCollectionName);
//   declareProperty( "MuonTrackCollections",  m_trackCollectionName);

//   /** segment cuts - default values - to be modified in job options */
//   declareProperty("bMdtSegmentChiSqMax",  m_bMdtchi2max     = 12.0);
//   declareProperty("bMdtSegmentNHolesMax", m_bMdtnholemax    = 2);
//   declareProperty("bMdtSegmentNHotsMin",  m_bMdtnhotsmin    = 5);

//   declareProperty("eMdtSegmentChiSqMax",  m_eMdtchi2max     = 20.0);
//   declareProperty("eMdtSegmentNHolesMax", m_eMdtnholemax    = 2);
//   declareProperty("eMdtSegmentNHotsMin",  m_eMdtnhotsmin    = 5);

//   declareProperty("eCscSegmentChiSqMax",  m_eCscchi2max     = 20.0);
//   declareProperty("eCscSegmentNHotsMin",  m_eCscnhotsmin    = 4);

//   /** Requirements on segment pairs */
//   declareProperty("mdtSegmentDThetaMax",  m_mdtdelthetamax = 0.05);  // Direction in rads
//   declareProperty("mdtSegmentDPhiMax",    m_mdtdelphimax   = 0.10);  // Position  in rads

//   /** Requirement to identify showering muons also use mdtSegmentDPhiMax */
//   declareProperty("mdtSegmentDEtaMax",    m_mdtdeltaEta    = 0.10);   // Delta eta
//   declareProperty("mdtSegmentDZMax",      m_mdtdeltaZ      = 100.0);  // Delta Z (in cm)
//   declareProperty("mdtSegmentDRMax",      m_mdtdeltaR      = 100.0);  // Delta R (in cm)


  
// }


// /* **********************************************************
//    Destructor
//    ********************************************************** */
// MuonAlignSegment::~MuonAlignSegment() {}

// /*---------------------------------------------------------*/
// StatusCode MuonAlignSegment::initialize(){
//   ATH_MSG_INFO ("Initializing MuonAlignSegment MonTool");

//   StatusCode sc = ManagedMonitorToolBase::initialize();
//   if (!sc.isSuccess()) return sc;
 
//   ATH_MSG_DEBUG("MuonAlignSegment::bookHistograms() to setup tools/services" );
  
//   // retrieve the active store
//   sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
//   if (sc != StatusCode::SUCCESS ) {
//     ATH_MSG_ERROR( " Cannot get ActiveStoreSvc " );
//     return sc ;
//   }

//   /** get a handle of StoreGate for access to the Event Store */
//   sc = service("StoreGateSvc", m_storeGate);
//   if (sc.isFailure()) {
//     ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
//     return sc;
//   }else ATH_MSG_DEBUG("Defined StoreGateSvc service" ); 
 


//   /** Detector Store and sub-detector Id helpers */
//   StoreGateSvc* detStore=0;
//   sc = serviceLocator()->service("DetectorStore", detStore);
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL("DetectorStore service not found." );
//     return sc;
//   }else{
//     ATH_MSG_DEBUG("Retrieved tool " << detStore );
  
//     sc = detStore->retrieve( p_muonMgr );
     
//     if ( sc.isFailure() ) {
//       ATH_MSG_ERROR(" Cannot retrieve MuonDetDescrMgr from detector store" );
//       return sc;
//     }else{
//       ATH_MSG_DEBUG("Retrieved MuonDetDescrMgr  " << p_muonMgr );
//       sc = detStore->retrieve(p_mdtIdHelper, "MDTIDHELPER");
//       if (sc.isFailure()){
// 	ATH_MSG_ERROR ("Cannot get MdtIdHelper" );
// 	return sc;
//       } else {
// 	ATH_MSG_DEBUG("mdtIdHelper successfully retrieved. " );
//       }
//       sc = detStore->retrieve(p_cscIdHelper, "CSCIDHELPER");
//       if (sc.isFailure()){
// 	ATH_MSG_ERROR("Cannot get CscIdHelper");
// 	return sc;
//       } else {
// 	ATH_MSG_DEBUG("cscIdHelper successfully retrieved. " );
//       }
          
//     }
//   }

 
//   /** Helper tool */
//   sc = p_helperTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL("Fatal to retrieve - Could not get " << p_helperTool ); 
//     return sc;
//   } else {
//     ATH_MSG_DEBUG( "Retrieved tool" << p_helperTool );
//   } 

//   /** Id helpers tools */
//   sc = p_idHelperTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_ERROR ("Failed to retrieve tool   " << p_idHelperTool ); 
//     return sc;
//   } else ATH_MSG_DEBUG( "Retrieved tool   " << p_idHelperTool );

  
//   /** Handle on the NTuple and histogramming service */ 
//   sc = service("THistSvc", m_thistSvc);
//   if (sc.isFailure()) {
//     ATH_MSG_ERROR ("Unable to retrieve pointer to THistSvc" );
//     return sc;
//   }

//   // Retrieve histoTool
//   sc = m_histoTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL("Could not get " << m_histoTool);  
//     return sc;
//   }
    
//   return sc;
// }

// /* **********************************************************
//    Book histograms
//    ********************************************************** */
// StatusCode MuonAlignSegment::bookHistograms() {

//   ATH_MSG_VERBOSE("Booking MuonAlignSegment histograms");

//   if (newLumiBlock) ATH_MSG_VERBOSE("newLumiBlock");
//   if (newRun) ATH_MSG_VERBOSE("newRun");
//   StatusCode sc = StatusCode::SUCCESS;

//   bool keepGoing = false;
//   if ( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) keepGoing = true;

//   if ( !keepGoing ) {
//     ATH_MSG_FATAL("MuonAlignSegment requires RAW information" );
//     return sc;
//   }

//   // Production of histograms:
   
//   if (keepGoing){

//     // Need to decide when histograms have to be recreated --> right now fill by run:
//     if (newEventsBlock){}
//     if (newLumiBlock){}
//     if (newRun){

//       ATH_MSG_INFO("Muon Alignment Monitoring : begin of run" );

//       for (int j=0;j<(int)m_segmCollectionName.size();j++) {
//         if ( m_segmCollectionFlag[j] == 0 ) continue;

//         MgmtAttr_t attr = ATTRIB_MANAGED;
// 	std::string m_generic_path_muonalign = "Muon/MuonAlignmentMonitoring";
// 	std::string muon_summaryalignsegmmom_path = "unknown";
// 	muon_summaryalignsegmmom_path = m_generic_path_muonalign+"/MuonSegmAlignMon/"+m_segmCollectionName[j];
        
// 	MonGroup al_muonalignsegm_shift ( this, muon_summaryalignsegmmom_path, run, attr );
 
// 	// Overall summary plot at once

// 	// The weight gives the average on this 2D plot
// 	m_h_muonalign_linesummary.push_back(m_histoTool->makeAndRegisterTH2F(al_muonalignsegm_shift,"m_h_muonalign_linesummary", "Delta Z (local): phi-sector vs eta-sector",16, 0., 16., 16, 0., 16.));
  
// 	// Summary endcap A

// 	// small

// 	// trackZ_segmentZ (cylindrical) 
// 	m_h_endcapAS_lineZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_endcapAS_lineZ_segmentZ", "Endcap A: line_{EI-E0} - segment_{EM}",101, -10.1, 10.1 ));
   
// 	// Large

// 	// trackZ_segmentZ (cylindrical) 
// 	m_h_endcapAL_lineZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_endcapAL_lineZ_segmentZ", "Endcap A: line_{EI-E0} - segment_{EM}",101, -10.1, 10.1) );

//         // Summary plots endcap C

// 	// small

// 	// trackZ_segmentZ (cylindrical) 
// 	m_h_endcapCS_lineZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_endcapCS_lineZ_segmentZ", "Endcap C: line_{EI-E0} - segment_{EM}",101, -10.1, 10.1 ));
 
// 	// Large

// 	// trackZ_segmentZ (cylindrical) 
// 	m_h_endcapCL_lineZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_endcapCL_lineZ_segmentZ", "Endcap C: line_{EI-E0} - segment_{EM}",101, -10.1, 10.1 ));
 
//         /// Summary plots barrel

// 	// Small

// 	// trackZ_segmentZ
// 	m_h_barrelS_lineZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrelS_lineZ_segmentZ", "Barrel small: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
 
// 	// Large

// 	// trackZ_segmentZ
// 	m_h_barrelL_lineZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrelL_lineZ_segmentZ", "Barrel large: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
 
//         /// Individual sectors of barrel

// 	m_h_barrel_lineZ_segmentZ1.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ1", "Barrel sec-1: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
// 	m_h_barrel_lineZ_segmentZ2.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ2", "Barrel sec-2: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 )); 
// 	m_h_barrel_lineZ_segmentZ3.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ3", "Barrel sec-3: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 )); 
// 	m_h_barrel_lineZ_segmentZ4.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ4", "Barrel sec-4: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
// 	m_h_barrel_lineZ_segmentZ5.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ5", "Barrel sec-5: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
// 	m_h_barrel_lineZ_segmentZ6.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ6", "Barrel sec-6: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
// 	m_h_barrel_lineZ_segmentZ7.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ7", "Barrel sec-7: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
// 	m_h_barrel_lineZ_segmentZ8.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ8", "Barrel sec-8: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
// 	m_h_barrel_lineZ_segmentZ9.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ9", "Barrel sec-9: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 )); 
// 	m_h_barrel_lineZ_segmentZ10.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ10", "Barrel sec-10: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
// 	m_h_barrel_lineZ_segmentZ11.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ11", "Barrel sec-11: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));	 
// 	m_h_barrel_lineZ_segmentZ12.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ12", "Barrel sec-12: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
//  	m_h_barrel_lineZ_segmentZ13.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ13", "Barrel sec-13: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 )); 
// 	m_h_barrel_lineZ_segmentZ14.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ14", "Barrel sec-14: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
// 	m_h_barrel_lineZ_segmentZ15.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ15", "Barrel sec-15: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));	 
// 	m_h_barrel_lineZ_segmentZ16.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalignsegm_shift,"m_h_barrel_lineZ_segmentZ16", "Barrel sec-16: line_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
//       } // end of loop over segment collection names
 
//       // Initialize counters for statistical studies
//       for (int k=0; k<16; ++k) {
// 	for (int l=0; l<16; ++l) {
// 	  zLocSumMoore[k][l] = 0.;
// 	  zLocCountMoore[k][l] = 0;
// 	  zLocSumMboy[k][l] = 0.;
// 	  zLocCountMboy[k][l] = 0;
// 	}
//       }
  
//     } // isNewRun
  
//   } // keepGoing 

//   return sc;
// }		 


// /* **********************************************************
//    Finalize - delete any memory allocation from the heap
//    ********************************************************** */
// StatusCode MuonAlignSegment::procHistograms( /*bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun*/ ) {
  
//   StatusCode sc=StatusCode::SUCCESS;

//   bool keepGoing = false;
//   if ( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) keepGoing = true;

//   if ( !keepGoing ) return StatusCode::SUCCESS;

//   ATH_MSG_DEBUG( "MuonAliMonitoring finalize()");
//   if ( keepGoing ) {
//     if (endOfEventsBlock) {}
//     if (endOfLumiBlock) {}
//     if (endOfRun) {

//       // Label histogram axes

//       for ( unsigned int ii = 0; ii < m_h_endcapAS_lineZ_segmentZ.size(); ++ii ) {
//         m_h_endcapCL_lineZ_segmentZ[ii]->GetXaxis()->SetTitle("R_{line} - R_{seg} [mm]");
//         m_h_endcapAL_lineZ_segmentZ[ii]->GetXaxis()->SetTitle("R_{line} - R_{seg} [mm]");
//         m_h_endcapCS_lineZ_segmentZ[ii]->GetXaxis()->SetTitle("R_{line} - R_{seg} [mm]");
//         m_h_endcapAS_lineZ_segmentZ[ii]->GetXaxis()->SetTitle("R_{line} - R_{seg} [mm]");
//         m_h_barrelS_lineZ_segmentZ[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrelL_lineZ_segmentZ[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");

// 	m_h_barrel_lineZ_segmentZ1[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ2[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ3[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ4[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ5[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ6[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ7[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ8[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ9[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ10[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ11[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ12[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ13[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ14[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ15[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");
// 	m_h_barrel_lineZ_segmentZ16[ii]->GetXaxis()->SetTitle("Z_{line} - Z_{seg} [mm]");

// 	// Fill summary plot
// 	if ( ii < 1 ) {
// 	  for (int k=0; k<16; ++k) {
// 	    for (int l=0; l<16; ++l) {
// 	      float avg = 0.;
// 	      if ( zLocCountMboy[k][l] > 10) avg = zLocSumMboy[k][l] / zLocCountMboy[k][l];
// 	      m_h_muonalign_linesummary[ii]->Fill( k+0.5, l+0.5, avg );	    
// 	    }
// 	  }
// 	} else if ( ii < 2 ) {
// 	  for (int k=0; k<16; ++k) {
// 	    for (int l=0; l<16; ++l) {
// 	      float avg = 0.;
// 	      if ( zLocCountMoore[k][l] > 10) avg = zLocSumMoore[k][l] / zLocCountMoore[k][l];
// 	      m_h_muonalign_linesummary[ii]->Fill( k+0.5, l+0.5, avg );	    
// 	    }
// 	  }
// 	}
//         m_h_muonalign_linesummary[ii]->GetXaxis()->SetTitle("#eta sector");
//         m_h_muonalign_linesummary[ii]->GetYaxis()->SetTitle("#phi sector");

//       }

//       // Clear stuff - to start a new run

//       m_h_endcapAS_lineZ_segmentZ.clear();
//       m_h_endcapCS_lineZ_segmentZ.clear();
//       m_h_barrelS_lineZ_segmentZ.clear();
//       m_h_endcapAL_lineZ_segmentZ.clear();
//       m_h_endcapCL_lineZ_segmentZ.clear();
//       m_h_barrelL_lineZ_segmentZ.clear();
//       m_h_barrel_lineZ_segmentZ1.clear();
//       m_h_barrel_lineZ_segmentZ2.clear();
//       m_h_barrel_lineZ_segmentZ3.clear();
//       m_h_barrel_lineZ_segmentZ4.clear();
//       m_h_barrel_lineZ_segmentZ5.clear();
//       m_h_barrel_lineZ_segmentZ6.clear();
//       m_h_barrel_lineZ_segmentZ7.clear();
//       m_h_barrel_lineZ_segmentZ8.clear();
//       m_h_barrel_lineZ_segmentZ9.clear();
//       m_h_barrel_lineZ_segmentZ10.clear();
//       m_h_barrel_lineZ_segmentZ11.clear();
//       m_h_barrel_lineZ_segmentZ12.clear();
//       m_h_barrel_lineZ_segmentZ13.clear();
//       m_h_barrel_lineZ_segmentZ14.clear();
//       m_h_barrel_lineZ_segmentZ15.clear();
//       m_h_barrel_lineZ_segmentZ16.clear();
  
//     } // endOfRun
  
//   } // keepGoing 
  
//   return sc;
// }


// /* **********************************************************
//    Execute - on event by event
//    ********************************************************** */
// StatusCode MuonAlignSegment::fillHistograms() {

//   StatusCode sc=StatusCode::SUCCESS;

//   bool keepGoing = false;
//   if ( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) keepGoing = true;

//   if ( keepGoing ) {

//     int h_index = -1;

//     for ( unsigned j=0; j<m_segmCollectionName.size(); ++j ) {
//       ATH_MSG_DEBUG("Segm collection FLAG is " << m_segmCollectionFlag[j]);

//       // check if using this collection from steering file
//       if ( m_segmCollectionFlag[j] == 0 ) continue; 

//       ++h_index;


//       sc = getSegments(h_index);
//       if (sc.isFailure()) {
//         ATH_MSG_WARNING("The method MuonAlignValidation::getSegments failed" );
//         return sc;
//       }
//     }
//   } // keepGoing



//   return sc;
// }


// /* **********************************************************
//    getSegments method
  
//    Require that 1st segment be at inner-most station
//    Require that 2nd segment be at outer-most station
//    Build track using these 2 segments
//    Find a 3rd segment in the middle station
//    Extrapolate track to surface where 3rd segment is defined
//    Compare track with segment position
//    Fill histograms
//    ********************************************************** */
// StatusCode MuonAlignSegment::getSegments( int& hIndex ) {

//   ATH_MSG_DEBUG("in MuonAlignValidation::getSegments()");

//   /** get a pointer to the segments */
//   ATH_MSG_DEBUG("MuonAlignValidation: Segment collection name is " << m_segmCollectionName[hIndex] );

//   const Trk::SegmentCollection* segmentCollection = 0;
//   StatusCode sc = m_storeGate->retrieve(segmentCollection, m_segmCollectionName[hIndex]);
 
//   if ( sc.isFailure() ) {
//     ATH_MSG_WARNING(" MuonAlignValidation: Cannot retrieve Segment Collection : "<< m_segmCollectionName[hIndex] );
//     return sc;
//   }

//   Trk::SegmentCollection::const_iterator s;
//   int nSegments = segmentCollection->end() - segmentCollection->begin();

//   ATH_MSG_DEBUG("MuonAlignValidation " << nSegments << " segment in this event " );


//   // Need at least 3 segments, also require < 13 segments
//   if ( nSegments < 3 || nSegments > 12 ) {
//     return sc;
//   }


//   // Sort segment by station
//   // Also apply quality requirements here
//   std::vector<Muon::MuonSegment* > muonInn;
//   std::vector<Muon::MuonSegment* > muonMid;
//   std::vector<Muon::MuonSegment* > muonOut;

//   for ( s = segmentCollection->begin(); s != segmentCollection->end(); ++s ) {
//     Muon::MuonSegment* mSeg               = dynamic_cast<Muon::MuonSegment*> (*s);
//     if(!mSeg) continue;
//     // Get the identifier (chamber ID)
//     Identifier id1                        = p_helperTool->chamberId( *mSeg );

//     if ( !id1.is_valid() ) {
//       ATH_MSG_WARNING("MuonAlignValidation: Bad identifier: " << id1 );
//       continue;  // skip segment with bad identifier
//     }
//     int stationId1                        = p_idHelperTool->stationIndex( id1 );
//     bool isMDT1                           = p_idHelperTool->isMdt( id1 );
//     bool isCSC1                           = p_idHelperTool->isCsc( id1 );
//     bool isEndcap1                        = p_idHelperTool->isEndcap( id1 );

//     if ( !isCSC1 ) {
//       if ( stationId1 < 0 ) continue;
//       if ( stationId1 > 6 ) continue;
//     }

//     // Moore MDT segments only
//     if ( isMDT1 && hIndex < 1 ) {
  
//       const Muon::MuonSegmentQuality* Q1 = dynamic_cast<const Muon::MuonSegmentQuality*>(mSeg->fitQuality());
//       if(!Q1) continue;    
//       float chisq1  = Q1->chiSquared();
//       int n_dof1     = Q1->numberDoF();
//       int hots1      = n_dof1 +2;
//       int holes1     = Q1->channelsWithoutHit().size();
//       if ( !goodSegmentQuality(chisq1, hots1, holes1, isMDT1, isEndcap1, stationId1) ) continue;
     
//     }

//     if ( isCSC1 || stationId1 == 0 || stationId1 == 4) {
//       muonInn.push_back( mSeg );
//     }
//     else if ( stationId1 == 1 || stationId1 == 5 ) {
//       muonMid.push_back( mSeg );
//     }
//     else {
//       muonOut.push_back( mSeg );
//     }

//   }


//   ATH_MSG_DEBUG("MuonAlignValidation nGoodQuality seg inner/middle/outer " 
// 		<< muonInn.size() << "  " 
// 		<< muonMid.size() << "  " 
// 		<< muonOut.size() ); 

//   // Need segment in all 3 stations, otherwise end
//   if ( muonInn.size() < 1 || muonMid.size() < 1 || muonOut.size() < 1 ) {
//     return sc;
//   }


//   // Clear properties:
//   hasShoweredInn.clear();
//   hasShoweredMid.clear();
//   hasShoweredOut.clear();

//   // Test that segments aren't in a shower
//   // that is, no chamber with multi-segments close to one another
//   // showeringSkeleton( *segmentCollection );
//   showeringTest( muonInn, 0 );
//   showeringTest( muonMid, 1 );
//   showeringTest( muonOut, 2 );


//   // First loop to find matching segment pair
//   for ( unsigned i = 0; i < muonInn.size(); ++i ) {
//     Muon::MuonSegment* mSeg = muonInn[i];

//     // chamber containing several segments close to one another:
//     if ( hasShoweredInn[i] > 0 ) continue;

//     // Get the identifier (chamber ID)
//     Identifier id1                        = p_helperTool->chamberId( *mSeg );

//     // Get information on chamber type, etc
//     bool isMDT1                           = p_idHelperTool->isMdt( id1 );
//     bool isCSC1                           = p_idHelperTool->isCsc( id1 );
//     bool isEndcap1                        = p_idHelperTool->isEndcap( id1 );
//     bool isSmall1                         = p_idHelperTool->isSmallChamber( id1 );

//     int phiSector1 = 99;
//     int etaSector1 = 99;

//     if ( isMDT1 ) {
//       etaSector1                         = p_mdtIdHelper->stationEta( id1 );
//       phiSector1                         = p_mdtIdHelper->stationPhi( id1 );
//     }
//     if ( isCSC1 ) {
//       // Dealing with CSC ..
//       etaSector1                         = p_cscIdHelper->stationEta( id1 );
//       phiSector1                         = p_cscIdHelper->stationPhi( id1 );
//     }


//     // Get segment measurements  
//     const Amg::Vector3D& globalDir1 = mSeg->globalDirection();
//     const Amg::Vector3D& globalPos1 = mSeg->globalPosition();


//     // To speed up algorithm
//     // bool haveTrackSegMatch = false;      


//     // 2nd loop on segment collection to form a pair:
//     for ( unsigned j = 0; j < muonOut.size(); ++j ) {
//       Muon::MuonSegment* mSeg2 = muonOut[j];

//       // Check for showering here to speed up !!!
//       if ( hasShoweredOut[j] > 0 ) continue;

//       // Get the identifier (chamber ID)
//       Identifier id2                          = p_helperTool->chamberId( *mSeg2 );
//       bool isEndcap2                          = p_idHelperTool->isEndcap( id2 );
//       bool isSmall2                           = p_idHelperTool->isSmallChamber( id2 );

//       // make sure both segments are in same region (endcap or barrel)
//       if ( isEndcap1 != isEndcap2 ) continue;

//       int etaSector2                          = p_mdtIdHelper->stationEta( id2 );
//       int phiSector2                          = p_mdtIdHelper->stationPhi( id2 );

//       // Check that inside same pointing tower
//       if ( abs(etaSector1 - etaSector2) > 1 ) continue; // allow to shift by one eta tower over 3 stations
//       if ( phiSector1 != phiSector2 ) continue;
//       if ( isSmall1   != isSmall2 ) continue;


//       // Get segment measurements  
//       const Amg::Vector3D& globalDir2  = mSeg2->globalDirection();
//       const Amg::Vector3D& globalPos2  = mSeg2->globalPosition();

//       // Look if segment direction agrees between 1 and 2
//       if ( isEndcap1 != isEndcap2 ) continue;
//       if ( !matchPairSegment( globalDir1, globalDir2 ) ) continue;

//       // ****************************************************** 
//       // Form a pseudo track using 2 segments which passed selection
//       // combination in inner and outer stations
//       // ******************************************************

    
//       // Now get a third segment matching this pseudo-track on a different station
//       for ( unsigned k = 0; k < muonMid.size(); ++k ) {
//         Muon::MuonSegment* mSeg3 = muonMid[k];

//         // ignore case where several segments close to one another:
//         if ( hasShoweredMid[k] > 0 ) continue;

//         // Get the identifier (chamber ID)
//         Identifier id3                        = p_helperTool->chamberId( *mSeg3 );
//         bool isEndcap3                        = p_idHelperTool->isEndcap( id3 );
//         bool isSmall3                         = p_idHelperTool->isSmallChamber( id3 );

//         // should be all in barrel / all in endcap
//         if ( isEndcap2 != isEndcap3 ) continue;

//         int etaSector3                          = p_mdtIdHelper->stationEta( id3 );
//         int phiSector3                          = p_mdtIdHelper->stationPhi( id3 );

// 	// Check that inside same pointing tower
// 	if ( etaSector1 != etaSector3 ) {
// 	  if ( etaSector2 != etaSector3 ) continue;  // has to be equal to inner or outer station
// 	}
// 	if ( phiSector1 != phiSector3 ) continue;
// 	if ( isSmall1   != isSmall3 ) continue;


//         // Get segment measurements
//         const Amg::Vector3D& globalPos3  = mSeg3->globalPosition();
//         const Amg::Vector3D& globalDir3  = mSeg3->globalDirection();

//         // Test if we are in the same phi sector
//         // get the phi sector
//         std::string name3                       = p_idHelperTool->chamberNameString( id3 );

//         // Look if segment direction agrees between 1 and 3, and 2 and 3
//         if ( isEndcap1 != isEndcap3 ) continue;
//         if ( !matchPairSegment( globalDir1, globalDir3 ) ) continue;
//         if ( !matchPairSegment( globalDir2, globalDir3 ) ) continue;


//         // I want to take the track and propagate it to the 3rd segment
//         // Surface on which the 3rd segment is defined
// 	float resid = 999.;

// 	float extraR = 0.;
// 	float extraZ = 0.;
// 	float check  = 0.;

//         float tdX = globalPos2.x() - globalPos1.x();
//         float tdY = globalPos2.y() - globalPos1.y();
//         float tdR = sqrt(tdX*tdX + tdY*tdY);
//         float tdZ = globalPos2.z() - globalPos1.z();

// 	if ( isEndcap3 ) {
// 	  // For endcap, use z from segment
// 	  extraZ = globalPos3.z();

// 	  // Then compute extrapolated position at this Z
// 	  float r1 = sqrt( globalPos1.x()*globalPos1.x() + globalPos1.y()*globalPos1.y() );
// 	  float r2 = sqrt( globalPos2.x()*globalPos2.x() + globalPos2.y()*globalPos2.y() );
// 	  float r3 = sqrt( globalPos3.x()*globalPos3.x() + globalPos3.y()*globalPos3.y() );

// 	  extraR = tdR / tdZ * ( globalPos3.z() - globalPos1.z() ) + r1;
// 	  resid = r3 - extraR;

// 	  // Check should be zero !
// 	  check = tdR / tdZ  * ( globalPos2.z() - globalPos1.z() ) + r1 - r2;

// 	} else {

// 	  // For barrel, use R from segment
// 	  extraR = sqrt( globalPos3.x()*globalPos3.x() + globalPos3.y()*globalPos3.y() );
// 	  float r1 = sqrt( globalPos1.x()*globalPos1.x() + globalPos1.y()*globalPos1.y() );
// 	  float r2 = sqrt( globalPos2.x()*globalPos2.x() + globalPos2.y()*globalPos2.y() );

// 	  extraZ = tdZ / tdR * ( extraR - r1 ) + globalPos1.z();
// 	  resid = globalPos3.z() - extraZ;

// 	  // Check should be zero !
// 	  check = tdZ / tdR * ( r2 - r1 ) + globalPos1.z() - globalPos2.z();
// 	}

// 	// Make sure that the extrapolation is computed properly
// 	// For some reason, it's not always the case, so use a tolerence of 1mm
// 	if ( check < 0. ) check = -check;
// 	if ( check > 1. ) continue;

//         // Test that segment and track are within 10 cm of each other in x, y and z
//         float absResid = resid;
//         if ( absResid < 0. ) absResid = -resid;
//         if ( absResid > 100. ) continue;

// 	// haveTrackSegMatch = true;

	
//         // Here, modify phiSector so it matches the 1-16 map instead of 1-8:
//         if ( isSmall3 ) {
//           phiSector3 = 2 * phiSector3;     // Small chambers start at 2
//         }
//         else {
//           phiSector3 = 2 * phiSector3 - 1; // Large chambers start at 1
//         }
      
//         /*
// 	bool dominiqueCheck = false;
//         if ( dominiqueCheck ) {
//           if (hIndex > 0 ) {
//             std::cout << "PositionE inner: "  << globalPos1.x() << " " << globalPos1.y() << " " << globalPos1.z() << std::endl;  
//             std::cout << "PositionE outer: "  << globalPos2.x() << " " << globalPos2.y() << " " << globalPos2.z() << std::endl;  
//             std::cout << "PositionE middl: "  << globalPos3.x() << " " << globalPos3.y() << " " << globalPos3.z() << std::endl;  
//             std::cout << "PositionE extra: "  << extraR << " " << resid << std::endl;
//           } else {
//             std::cout << "PositionB inner: "  << globalPos1.x() << " " << globalPos1.y() << " " << globalPos1.z() << std::endl;  
//             std::cout << "PositionB outer: "  << globalPos2.x() << " " << globalPos2.y() << " " << globalPos2.z() << std::endl;  
//             std::cout << "PositionB middl: "  << globalPos3.x() << " " << globalPos3.y() << " " << globalPos3.z() << std::endl;  
//             std::cout << "PositionB extra: "  << extraZ << " " << resid << std::endl;
//           }
//         }
//         */

//         // Fill histograms here:

// 	int thisEtaSector = -1;
// 	if ( globalPos3.z() < 0. ) {
// 	  // In C side
// 	  if ( isEndcap3 ) {
// 	    if ( isCSC1 ) { 
// 	      thisEtaSector = 0;
// 	    } else {
// 	      thisEtaSector = 1;
// 	    }
// 	  } else { 
// 	    // barrel eta sector goes from -6, to -1  --> want it to go from 2 to 7
// 	    thisEtaSector = 8 + etaSector3; 
// 	  }
// 	} else {
// 	  // In A side
// 	  if ( isEndcap3 ) {
// 	    if ( isCSC1 ) { 
// 	      thisEtaSector = 15;
// 	    } else {
// 	      thisEtaSector = 14;
// 	    }
// 	  } else { 
// 	    // barrel eta sector goes from 1, to 6  --> want it to go from 8 to 13
// 	    thisEtaSector = etaSector3 + 7; 
// 	  }
// 	}	  
              
//         if ( resid < 2.0 && resid > -2.0 ) {
// 	  // Input for summary plot:
// 	  if ( thisEtaSector > -1 && thisEtaSector < 16 ) {
// 	    if (phiSector3 > 0 && phiSector3 < 17 ) {
// 	      if ( hIndex < 1) {
//   	        zLocSumMboy[thisEtaSector][phiSector3-1] += resid;
// 	        zLocCountMboy[thisEtaSector][phiSector3-1]++;
// 	      } else if ( hIndex < 2) {
//  	        zLocSumMoore[thisEtaSector][phiSector3-1] += resid;
// 	        zLocCountMoore[thisEtaSector][phiSector3-1]++;
// 	      }
// 	    } 
// 	  }
// 	}



//         // avg in phi sector
//         if ( isEndcap3 ) {
//           if ( globalPos3.z() < 0. ) {
//   	    if ( isSmall3  ) m_h_endcapCS_lineZ_segmentZ[hIndex]->Fill( resid );
// 	    if ( !isSmall3 ) m_h_endcapCL_lineZ_segmentZ[hIndex]->Fill( resid );
//           } else {
//  	    if ( isSmall3  ) m_h_endcapAS_lineZ_segmentZ[hIndex]->Fill( resid );
// 	    if ( !isSmall3 ) m_h_endcapAL_lineZ_segmentZ[hIndex]->Fill( resid );
//           }
//         }

// 	if ( isEndcap3 ) continue; // next plots are for barrel only

// 	if ( isSmall3  ) m_h_barrelS_lineZ_segmentZ[hIndex]->Fill( resid );
// 	if ( !isSmall3 ) m_h_barrelL_lineZ_segmentZ[hIndex]->Fill( resid );

// 	if ( phiSector3 == 1 )  m_h_barrel_lineZ_segmentZ1[hIndex]->Fill(  resid );
// 	if ( phiSector3 == 2 )  m_h_barrel_lineZ_segmentZ2[hIndex]->Fill(  resid );
// 	if ( phiSector3 == 3 )  m_h_barrel_lineZ_segmentZ3[hIndex]->Fill(  resid );
// 	if ( phiSector3 == 4 )  m_h_barrel_lineZ_segmentZ4[hIndex]->Fill(  resid );
// 	if ( phiSector3 == 5 )  m_h_barrel_lineZ_segmentZ5[hIndex]->Fill(  resid );
// 	if ( phiSector3 == 6 )  m_h_barrel_lineZ_segmentZ6[hIndex]->Fill(  resid );
// 	if ( phiSector3 == 7 )  m_h_barrel_lineZ_segmentZ7[hIndex]->Fill(  resid );
// 	if ( phiSector3 == 8 )  m_h_barrel_lineZ_segmentZ8[hIndex]->Fill(  resid );
// 	if ( phiSector3 == 9 )  m_h_barrel_lineZ_segmentZ9[hIndex]->Fill(  resid );
// 	if ( phiSector3 == 10 ) m_h_barrel_lineZ_segmentZ10[hIndex]->Fill( resid );
// 	if ( phiSector3 == 11 ) m_h_barrel_lineZ_segmentZ11[hIndex]->Fill( resid );
// 	if ( phiSector3 == 12 ) m_h_barrel_lineZ_segmentZ12[hIndex]->Fill( resid );
// 	if ( phiSector3 == 13 ) m_h_barrel_lineZ_segmentZ13[hIndex]->Fill( resid );
// 	if ( phiSector3 == 14 ) m_h_barrel_lineZ_segmentZ14[hIndex]->Fill( resid );
// 	if ( phiSector3 == 15 ) m_h_barrel_lineZ_segmentZ15[hIndex]->Fill( resid );
// 	if ( phiSector3 == 16 ) m_h_barrel_lineZ_segmentZ16[hIndex]->Fill( resid );


//       }  // end of loop on middle station segment list to match to track

//     }  // end of loop on outer station segment list 

//   }  // end of loop on inner station segment list

//   ATH_MSG_DEBUG("MuonAlignValidation::getSegments() succeeded" );

//   muonInn.clear();
//   muonMid.clear();
//   muonOut.clear();
  		
//   return sc;
// }



// /* **********************************************************
//    goodSegmentQuality method
 
//    Test quality of segments:
//    chi^2, # of hits on tracks, # of holes
//    ********************************************************** */
// bool MuonAlignSegment::goodSegmentQuality(float& chi2, int& nhots, int& nholes, bool& isMDT, bool& isEndcap, int& station) {

//   bool pass = true;

//   // CSC seem to have tight internal constraints anyway...
//   if ( !isMDT ) {
//     return true;
//   }

//   int mdtNhotsMin   = m_bMdtnhotsmin;
//   int mdtNholeMax   = m_bMdtnholemax;
//   float mdtChi2Max  = m_bMdtchi2max;


//   // BI station has 8 layers instead of 6.
//   // Adjust accordingly
//   if ( station == 0 ) {
//     mdtNhotsMin += 1;     
//     mdtNholeMax += 1; 
//   }


//   if (isEndcap ) {
//     mdtNhotsMin = m_eMdtnhotsmin;
//     mdtNholeMax = m_eMdtnholemax;
//     mdtChi2Max  = m_eMdtchi2max;

//     // EI station has 8 layers instead of 6.
//     // Adjust accordingly
//     if ( station == 4 ) {
//       mdtNhotsMin += 1;     
//       mdtNholeMax += 1;
//     }
//   }


//   // Cuts for case where dealing with MDT
//   if ( isMDT ) {
//     if ( chi2   > mdtChi2Max )  return false;
//     if ( nholes > mdtNholeMax ) return false;
//     if ( nhots  < mdtNhotsMin ) return false;
//   } 
//   // Otherwise, dealing with CSC
//   else {
//     return true;
//   }

//   return pass;

// }


// /* **********************************************************
//    matchPairSegment method
 
//    Test if segment pair is close enough in theta, phi to be considered
//    Use segment direction here
//    ********************************************************** */
// bool MuonAlignSegment::matchPairSegment( const Amg::Vector3D& globalDir1, const Amg::Vector3D& globalDir2 ) {

//   bool pass = true;

//   float dtheta = globalDir1.theta()-globalDir2.theta();
//   if ( dtheta < 0. ) dtheta = -dtheta;
//   if ( dtheta > m_mdtdelthetamax ) return false;  


//   float dphi = globalDir1.phi()-globalDir2.phi();
//   if ( dphi < 0.) dphi = -dphi;
//   if ( dphi > m_mdtdelphimax ) return false;   


//   float deta = globalDir1.eta()-globalDir2.eta();
//   if ( deta < 0. ) deta = -deta;
//   float dR = sqrt ( dphi*dphi + deta*deta );

//   if ( dR > m_mdtdelphimax ) return false;  // this is saying R ~ phi < phimax

//   return pass;

// }


// /* **********************************************************
//    showeringTest method
 
//    Test if segment showered in this chamber, i.e. 
//    look for abnormal number of hits on tracks 
//    and/or several segments clustered in same chamber
//    ********************************************************** */
// void MuonAlignSegment::showeringTest( std::vector<Muon::MuonSegment* > muonSegs, int stationIdx  ) {

//   for ( unsigned i = 0; i < muonSegs.size(); ++i ) {
//     Muon::MuonSegment* mSeg = muonSegs[i];

//     // Get the identifier (chamber ID)
//     Identifier id1                        = p_helperTool->chamberId( *mSeg );

//     // Get segment measurements
//     const Amg::Vector3D& globalPos  = mSeg->globalPosition();

//     // Segment 1 position
//     float phi1    = globalPos.phi();
//     float eta1    = globalPos.eta();
//     float x1      = globalPos.x();
//     float y1      = globalPos.y();
//     float z1      = globalPos.z();

//     int busyChamber = 0;

//     // 2nd loop on segment collection to see if more than 1 segment in this chamber:

//     for ( unsigned j = 0; j < muonSegs.size(); ++j ) {

//       if ( i == j ) continue; // same segment

//       Muon::MuonSegment* mSeg2 = muonSegs[j];

//       // Get the identifier (chamber ID)
//       Identifier id2                          = p_helperTool->chamberId( *mSeg2 );

//       if ( id2 != id1 ) continue;

//       // Station index (a string)
//       bool isEndcap2                          = p_idHelperTool->isEndcap( id2 );

//       // Get segment measurements
//       const Amg::Vector3D& globalPos2   = mSeg2->globalPosition();

//       // Segment 2 position
//       float phi2    = globalPos2.phi();
//       float eta2    = globalPos2.eta();

//       // Are segments very close to one another ?
//       // (as opposed to 2 distinct tracks well separated in the chamber)

//       // A) First test on Eta:
//       float deltaE = eta2 - eta1;
//       if ( deltaE < 0. ) deltaE = -deltaE;
//       if ( deltaE >= m_mdtdeltaEta  ) continue;

//       // B) Second test on phi
//       double PI = 3.1415927;
//       double TwoPI = 2.0 * PI;
//       float delPhi = phi2 - phi1;
//       if ( delPhi < 0. ) delPhi = -delPhi;
//       if ( delPhi > TwoPI -0.1 ) delPhi = delPhi - TwoPI;
//       if ( delPhi < 0. ) delPhi = -delPhi;
//       if ( delPhi >= m_mdtdelphimax ) continue;

//       float x2      = globalPos2.x();
//       float y2      = globalPos2.y();
//       float z2      = globalPos2.z();

//       // case where dealing in barrel
//       if ( !isEndcap2 ) {
//         float deltaZ = z2 - z1;
//         if ( deltaZ < 0. ) deltaZ = -deltaZ;
//         if ( deltaZ < m_mdtdeltaZ  ) busyChamber++;
//       }
//       // instead dealing with endcap
//       else {
//         float deltaR = sqrt(x2*x2 + y2*y2) - sqrt(x1*x1 + y1*y1);
//         if ( deltaR < 0. ) deltaR = -deltaR;
//         if ( deltaR < m_mdtdeltaR  ) busyChamber++;
//       }
//     }

//     if ( stationIdx == 0 ) hasShoweredInn.push_back( busyChamber );
//     if ( stationIdx == 1 ) hasShoweredMid.push_back( busyChamber );
//     if ( stationIdx == 2 ) hasShoweredOut.push_back( busyChamber );
//   }

//   return;
// }
