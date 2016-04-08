/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************************
   MuonAlignValidation.cxx

   Author:      Dominique Fortin - Triumf
   created:     Oct 2008
   Modified by: Nektarios Chr. Benekos - Illinois
   Description: Implementation code for the validation of the alignment
	        
*********************************************************************/

// #include "MuonAlignMonitoring/MuonAlignValidation.h"

// //Gaudi
// #include "GaudiKernel/MsgStream.h"
// #include "GaudiKernel/AlgFactory.h"
// #include "GaudiKernel/IToolSvc.h"
// #include "GaudiKernel/ITHistSvc.h"
// #include "GaudiKernel/IChronoStatSvc.h"
// #include "StoreGate/StoreGateSvc.h"

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

// // Building track from segments
// #include "MuonRecToolInterfaces/IMuonSegmentTrackBuilder.h"

// // Track info
// #include "TrkSegment/Segment.h"
// #include "TrkSegment/SegmentCollection.h"
// #include "TrkEventPrimitives/FitQuality.h"
// #include "TrkParameters/TrackParameters.h"
// #include "TrkEventPrimitives/PropDirection.h"
// #include "TrkExInterfaces/IExtrapolator.h"
// #include "TrkGeometry/TrackingGeometry.h"
// #include "TrkGeometry/MagneticFieldProperties.h"
// #include "TrkParameters/TrackParameters.h"
// #include "TrkSurfaces/PlaneSurface.h"

// #include "MagFieldInterfaces/IMagFieldSvc.h"
// #include "AthenaMonitoring/AthenaMonManager.h"

// //std and STL includes
// #include <algorithm>
// #include <math.h>
// #include <functional>
// #include <iostream>
// #include <vector>


// /* **********************************************************
//    Constructor
//    ********************************************************** */
// MuonAlignValidation::MuonAlignValidation( const std::string & type, const std::string & name, const IInterface* parent )
//   : ManagedMonitorToolBase( type, name, parent ), 
//     m_activeStore(0),
//     m_storeGate(0),
//     p_muonMgr(0),
//     p_magFieldProperties(0),
//     p_mdtIdHelper(0),
//     p_cscIdHelper(0),
//     p_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
//     p_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
//     p_trackBuilder("Muon::MuonSegmentTrackBuilder/MuonSegmentTrackBuilder"), 
//     p_IExtrapolator("Trk::Extrapolator/AtlasExtrapolator"),
//     m_thistSvc(0) 
// {

//   /** switches to control the analysis through job options */
//   //  declareProperty("TrackingGeometryName", m_trackingGeometryName);
//   m_histoTool  = ToolHandle<Muon::MuonDQAHistTool>("Muon::MuonDQAHistTool/MuonDQAHistTool");

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
// MuonAlignValidation::~MuonAlignValidation() {}

// /*---------------------------------------------------------*/
// StatusCode MuonAlignValidation::initialize(){
//   ATH_MSG_INFO("Initializing MuonAlignValidation MonTool");

//   StatusCode sc = ManagedMonitorToolBase::initialize();
//   if (!sc.isSuccess()) return sc;

//   // retrieve the active store
//   sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
//   if (sc != StatusCode::SUCCESS ) {
//     ATH_MSG_ERROR(" Cannot get ActiveStoreSvc " );
//     return StatusCode::RECOVERABLE;
//   }

//   /** get a handle of StoreGate for access to the Event Store */
//   sc = service("StoreGateSvc", m_storeGate);
//   if (sc.isFailure()) {
//     ATH_MSG_ERROR( "Unable to retrieve pointer to StoreGateSvc" );
//     return StatusCode::RECOVERABLE;
//   }
 

//   /** Magnetic Field */
//   p_magFieldProperties = new Trk::MagneticFieldProperties(); 

//   /** Detector Store and sub-detector Id helpers */
//   StoreGateSvc* detStore=0;
//   sc = serviceLocator()->service("DetectorStore", detStore);
//   if (sc.isFailure()) {
//     ATH_MSG_ERROR( "DetectorStore service not found." );
//     return StatusCode::RECOVERABLE;
//   }else{
    
//     sc = detStore->retrieve( p_muonMgr );
    
//     if ( sc.isFailure() ) {
//       ATH_MSG_FATAL( " Cannot retrieve MuonDetDescrMgr from detector store" );
//       return StatusCode::FAILURE;
//     }else{
//       sc = detStore->retrieve(p_mdtIdHelper, "MDTIDHELPER");
//       if (sc.isFailure()){
// 	ATH_MSG_FATAL( "Cannot get MdtIdHelper" );
// 	return StatusCode::FAILURE;
//       }
//       sc = detStore->retrieve(p_cscIdHelper, "CSCIDHELPER");
//       if (sc.isFailure()){
// 	ATH_MSG_FATAL( "Cannot get CscIdHelper" );
// 	return StatusCode::FAILURE;
//       }          
//     }
//   }
 
//   /** Helper tool */
//   sc = p_helperTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL( "Failed to retrieve  " << p_helperTool ); 
//     return StatusCode::FAILURE;
//   } 

//   /** Id helpers tools */
//   sc = p_idHelperTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL( "Failed to retrieve tool   " << p_idHelperTool ); 
//     return StatusCode::FAILURE;
//   }    

//   /** Track builder from a pair of segments */
//   sc = p_trackBuilder.retrieve();
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL( "Could not get " << p_trackBuilder ); 
//     return StatusCode::FAILURE;
//   }


//   /** Track extrapolator */
//   sc = p_IExtrapolator.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL("Could not get " << p_IExtrapolator ); 
//     return StatusCode::FAILURE;
//   }

  
//   /** Handle on the NTuple and histogramming service */ 
//   sc = service("THistSvc", m_thistSvc);
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL( "Unable to retrieve pointer to THistSvc" );
//     return StatusCode::FAILURE;
//   }

//   // Retrieve histoTool
//   sc = m_histoTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL("Could not get " << m_histoTool );  
//     return sc;
//   }
  
//   return sc;
// }

// /* ********************************************************** 
//    Book histograms
//    ********************************************************** */
// StatusCode MuonAlignValidation::bookHistograms(){

//   ATH_MSG_VERBOSE("Booking MuonAlignValidation histograms");

//   if (newLumiBlock) ATH_MSG_VERBOSE("newLumiBlock");
//   if (newRun) ATH_MSG_VERBOSE("newRun");
//   StatusCode sc = StatusCode::SUCCESS;

//   bool keepGoing = true;
//   // Production of histograms:
//   if (keepGoing){
//     if(newRun || newLowStatInterval){
//       bookMuonAlignValidationHistograms(/* isNewEventsBlock,*/ newLumiBlock, newRun);
//       if(sc.isFailure()) { 
// 	ATH_MSG_FATAL("Failed to bookMuonAlignValidationHistograms" );
// 	return sc;
//       } 
//       return sc; 
//     } // newRun
 
//   } // keepGoing 

// return StatusCode::SUCCESS;

// } 
// /* ********************************************************** 
//    Book histograms
//    ********************************************************** */
// StatusCode MuonAlignValidation::bookMuonAlignValidationHistograms(/* bool isNewEventsBlock, */bool newLumiBlock, bool newRun) {
//   ATH_MSG_DEBUG( "Muon Alignment Monitoring : begin of run" );

//   StatusCode sc = StatusCode::SUCCESS;
//   if(newLumiBlock) {}
//   if(newRun){
 
//     for (int j=0;j<(int)m_segmCollectionName.size();j++) {
//       if ( m_segmCollectionFlag[j] == 0 ) continue;

//       MgmtAttr_t attr = ATTRIB_MANAGED;
//       std::string m_generic_path_muonalign = "Muon/MuonAlignmentMonitoring";
//       MonGroup al_muonalign_shift ( this, m_generic_path_muonalign+"/MuonGenericAlignMon/"+m_segmCollectionName[j], run, attr );
 
//       // Overall summary plot at once 

//       // The weight gives the average on this 2D plot
//       m_h_muonalign_summary.push_back(m_histoTool->makeAndRegisterTH2F(al_muonalign_shift,"m_h_muonalign_summary", "Delta Z (local): phi-sector vs eta-sector", 16, 0., 16., 16, 0., 16.) );
    
//       // Summary endcap A

//       // small

//       // trackZ_segmentZ (cylindrical) 
	  
//       m_h_endcapAS_trackZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift, "m_h_endcapAS_trackZ_segmentZ", "Endcap A: track_{EI-E0} - segment_{EM}", 101, -10.1, 10.1 ) );
	  
//       // resPt (%)
//       m_h_endcapAS_trackPt_segmentPt.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift, "m_h_endcapAS_trackPt_segmentPt", "Endcap A: track_{EI-E0} - segment_{EM}", 101,-50.5, 50.5 )) ;
	  
//       // Large

//       // trackZ_segmentZ (cylindrical) 
//       m_h_endcapAL_trackZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift,"m_h_endcapAL_trackZ_segmentZ", "Endcap A: track_{EI-E0} - segment_{EM}", 101, -10.1, 10.1 ));
   
//       // resPt (%)
//       m_h_endcapAL_trackPt_segmentPt.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift,"m_h_endcapAL_trackPt_segmentPt", "Endcap A: track_{EI-E0} - segment_{EM}", 101, -50.5, 50.5 ) );
   
//       // Summary plots endcap C

//       // small

//       // trackZ_segmentZ (cylindrical) 
//       m_h_endcapCS_trackZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift,"m_h_endcapCS_trackZ_segmentZ", "Endcap C: track_{EI-E0} - segment_{EM}", 101, -10.1, 10.1) );
 
//       // resPt (%)
//       m_h_endcapCS_trackPt_segmentPt.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift , "m_h_endcapCS_trackPt_segmentPt", "Endcap C: track_{EI-E0} - segment_{EM}",101, -50.5, 50.5 ));
   
//       // Large

//       // trackZ_segmentZ (cylindrical) 
//       m_h_endcapCL_trackZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift, "m_h_endcapCL_trackZ_segmentZ", "Endcap C: track_{EI-E0} - segment_{EM}",101, -10.1, 10.1 ));
 
//       // resPt (%)
//       m_h_endcapCL_trackPt_segmentPt.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift, "m_h_endcapCL_trackPt_segmentPt", "Endcap C: track_{EI-E0} - segment_{EM}", 101, -50.5, 50.5 ));
   
//       /// Summary plots barrel

//       // Small

//       // trackZ_segmentZ
//       m_h_barrelS_trackZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift, "m_h_barrelS_trackZ_segmentZ", "Barrel small: track_{BI-B0} - segment_{BM}", 101, -10.1, 10.1 ));
 
//       // resPt (%)
//       m_h_barrelS_trackPt_segmentPt.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift,"m_h_barrelS_trackPt_segmentPt", "Barrel small: track_{BI-B0} - segment_{BM}", 101, -50.5, 50.5 ));
   
//       // Large

//       // trackZ_segmentZ
//       m_h_barrelL_trackZ_segmentZ.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift,"m_h_barrelL_trackZ_segmentZ", "Barrel large: track_{BI-B0} - segment_{BM}",	101, -10.1, 10.1 ));
 
//       // resPt (%)
//       m_h_barrelL_trackPt_segmentPt.push_back(m_histoTool->makeAndRegisterTH1F(al_muonalign_shift, "m_h_barrelL_trackPt_segmentPt", "Barrel large: track_{BI-B0} - segment_{BM}", 101, -50.5, 50.5 ));
   
 
//     } // end of loop over segment collection names

//     for (int k=0; k<16; ++k) {
//       for (int l=0; l<16; ++l) {
// 	zLocSumMoore[k][l] = 0.;
// 	zLocCountMoore[k][l] = 0;
// 	zLocSumMboy[k][l] = 0.;
// 	zLocCountMboy[k][l] = 0;
//       }
//     }
 
//   }
 
//   return StatusCode::SUCCESS;  
// }		 

// /* **********************************************************
//    Finalize - delete any memory allocation from the heap
//    ********************************************************** */
// StatusCode MuonAlignValidation::procHistograms( /*bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun*/ ) {
  
//   StatusCode sc=StatusCode::SUCCESS;

//   bool keepGoing = false;
//   if ( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) keepGoing = true;

//   if ( !keepGoing ) return StatusCode::SUCCESS;

//   if ( keepGoing ) {
//     if (endOfEventsBlock) {}
//     if (endOfLumiBlock) {}
//     if (endOfRun) {

//       // Add axis label on plots
//       for ( unsigned int ii = 0; ii < m_h_barrelL_trackZ_segmentZ.size(); ++ii ) {

//         m_h_endcapCL_trackZ_segmentZ[ii]->GetXaxis()->SetTitle("Z_{trk} - Z_{seg} (local) [mm]");
// 	m_h_endcapCL_trackPt_segmentPt[ii]->GetXaxis()->SetTitle("P_{T}(trk) - P_{T}(seg) [%]");

//         m_h_endcapAL_trackZ_segmentZ[ii]->GetXaxis()->SetTitle("Z_{trk} - Z_{seg} (local) [mm]");
// 	m_h_endcapAL_trackPt_segmentPt[ii]->GetXaxis()->SetTitle("P_{T}(trk) - P_{T}(seg) [%]");

//         m_h_endcapCS_trackZ_segmentZ[ii]->GetXaxis()->SetTitle("Z_{trk} - Z_{seg} (local) [mm]");
// 	m_h_endcapCS_trackPt_segmentPt[ii]->GetXaxis()->SetTitle("P_{T}(trk) - P_{T}(seg) [%]");

//         m_h_endcapAS_trackZ_segmentZ[ii]->GetXaxis()->SetTitle("Z_{trk} - Z_{seg} (local) [mm]");
// 	m_h_endcapAS_trackPt_segmentPt[ii]->GetXaxis()->SetTitle("P_{T}(trk) - P_{T}(seg) [%]");

//         m_h_barrelS_trackZ_segmentZ[ii]->GetXaxis()->SetTitle("Z_{trk} - Z_{seg} [mm]");
// 	m_h_barrelS_trackPt_segmentPt[ii]->GetXaxis()->SetTitle("P_{T}(trk) - P_{T}(seg) [%]");

// 	m_h_barrelL_trackZ_segmentZ[ii]->GetXaxis()->SetTitle("Z_{trk} - Z_{seg} [mm]");
// 	m_h_barrelL_trackPt_segmentPt[ii]->GetXaxis()->SetTitle("P_{T}(trk) - P_{T}(seg) [%]");

// 	// Fill summary plot
// 	if ( ii < 1 ) {
// 	  for (int k=0; k<16; ++k) {
// 	    for (int l=0; l<16; ++l) {
// 	      float avg = 0.;
// 	      if ( zLocCountMboy[k][l] > 10) avg = zLocSumMboy[k][l] / zLocCountMboy[k][l];
// 	      m_h_muonalign_summary[ii]->Fill( k+0.5, l+0.5, avg );	    
// 	    }
// 	  }
// 	} else if ( ii < 2 ) {
// 	  for (int k=0; k<16; ++k) {
// 	    for (int l=0; l<16; ++l) {
// 	      float avg = 0.;
// 	      if ( zLocCountMoore[k][l] > 10) avg = zLocSumMoore[k][l] / zLocCountMoore[k][l];
// 	      m_h_muonalign_summary[ii]->Fill( k+0.5, l+0.5, avg );	    
// 	    }
// 	  }
// 	}
//         m_h_muonalign_summary[ii]->GetXaxis()->SetTitle("#eta sector");
//         m_h_muonalign_summary[ii]->GetYaxis()->SetTitle("#phi sector");
	
//       } // End loop over segment collection names


//       // Clear stuff - to start a new run

//       m_h_muonalign_summary.clear();    

//       m_h_endcapAS_trackZ_segmentZ.clear();
//       m_h_endcapAS_trackPt_segmentPt.clear();

//       m_h_endcapCS_trackZ_segmentZ.clear();
//       m_h_endcapCS_trackPt_segmentPt.clear();

//       m_h_endcapAL_trackZ_segmentZ.clear();
//       m_h_endcapAL_trackPt_segmentPt.clear();

//       m_h_endcapCL_trackZ_segmentZ.clear();
//       m_h_endcapCL_trackPt_segmentPt.clear();

//       m_h_barrelS_trackZ_segmentZ.clear();
//       m_h_barrelS_trackPt_segmentPt.clear();

//       m_h_barrelL_trackZ_segmentZ.clear();
//       m_h_barrelL_trackPt_segmentPt.clear();


//       // Reset counters for statistical studies

//       for (int k=0; k<16; ++k) {
// 	for (int l=0; l<16; ++l) {
// 	  zLocSumMoore[k][l] = 0.;
// 	  zLocCountMoore[k][l] = 0;
// 	  zLocSumMboy[k][l] = 0.;
// 	  zLocCountMboy[k][l] = 0;
// 	}
//       }
  
//     } // endOfRun
  
//   } // keepGoing 
  
//   return sc;
// }


// /* **********************************************************
//    Execute - on event by event
//    ********************************************************** */
// StatusCode MuonAlignValidation::fillHistograms() {

//   StatusCode sc=StatusCode::SUCCESS;

//   // Chrono services start:
//   // m_chrono->chronoStart("DOMAlignChrono");

//   bool keepGoing = false;
//   if ( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) keepGoing = true;

//   if ( keepGoing ) {

//     int h_index = -1;

//     for ( unsigned j=0; j<m_segmCollectionName.size(); ++j ) {

//       // check if using this collection from steering file
//       if ( m_segmCollectionFlag[j] == 0 ) continue; 

//       ++h_index;


//       sc = getSegments(h_index);
//       if (sc.isFailure()) {
//         msg(MSG::FATAL)  << "Could not fill histograms in MuonAlignValidation::getSegments for collection" << m_segmCollectionName[j] << endreq;
//         return StatusCode::FAILURE;
//       }
//     }
//   } // keepGoing


//   // Chrono services stop:
//   // m_chrono->chronoStop("DOMAlignChrono");


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
// StatusCode MuonAlignValidation::getSegments( int& hIndex ) {


//   /** get a pointer to the segments */

//   const Trk::SegmentCollection* segmentCollection = 0;
//   StatusCode sc = m_storeGate->retrieve(segmentCollection, m_segmCollectionName[hIndex]);
 
//   if ( sc.isFailure() ) {
//     ATH_MSG_WARNING(" MuonAlignValidation: Cannot retrieve Segment Collection : "<< m_segmCollectionName[hIndex] );
//     return StatusCode::SUCCESS;
//   }

	
//   /** get a pointer to the tracks */        

//   const DataVector<Trk::Track> *m_tracks; 
//   sc = m_storeGate->retrieve(m_tracks, m_trackCollectionName[hIndex]);

//   if ( sc.isFailure() ) {
//     ATH_MSG_WARNING(" MuonAlignValidation: Cannot retrieve Track Collection : "<< m_trackCollectionName[hIndex] );
//     return StatusCode::SUCCESS;
//   }




//   Trk::SegmentCollection::const_iterator s;
//   int nSegments = segmentCollection->end() - segmentCollection->begin();


//   // Need at least 3 segments, also require < 22 segments
//   if ( nSegments < 3 || nSegments > 21 ) {
//     return StatusCode::SUCCESS;
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

//       // Look if segment direction agrees between 1 and 2
//       if ( !matchPairSegment( globalDir1, globalDir2 ) ) continue;


//       // ****************************************************** 
//       // Form a track using 2 segments which passed selection
//       // combination in inner and outer stations
//       // ******************************************************
//       const Trk::Track* track = p_trackBuilder->combine( *mSeg, *mSeg2);
//       if ( !track ) {
// 	continue;
//       }

    
//       // Now get a third segment matching this track on a different station, and get residual
//       for ( unsigned k = 0; k < muonMid.size(); ++k ) {
//         Muon::MuonSegment* mSeg3 = muonMid[k];

//         // ignore case where several segments close to one another:
//         if ( hasShoweredMid[k] > 0 ) continue;

//         // Get the identifier (chamber ID)
//         Identifier id3                        = p_helperTool->chamberId( *mSeg3 );
//         bool isEndcap3                        = p_idHelperTool->isEndcap( id3 );
//         bool isSmall3                         = p_idHelperTool->isSmallChamber( id3 );

//         // should be all in barrel / all in endcap
//         if ( isEndcap1 != isEndcap3 ) continue;

//         // Get segment measurements
//         const Amg::Vector3D& globalPos3  = mSeg3->globalPosition();
//         const Amg::Vector3D& globalDir3  = mSeg3->globalDirection();

//         int etaSector3 = p_mdtIdHelper->stationEta( id3 );
//         int phiSector3 = p_mdtIdHelper->stationPhi( id3 );


// 	// Check that inside same pointing tower
// 	if ( etaSector1 != etaSector3 ) {
// 	  if ( etaSector2 != etaSector3 ) continue;  // has to be equal to inner or outer station
// 	}
// 	if ( phiSector1 != phiSector3 ) continue;
// 	if ( isSmall1   != isSmall3 ) continue;


//         // Test if we are in the same phi sector
//         // get the phi sector
//         std::string name3                       = p_idHelperTool->chamberNameString( id3 );

//         // Look if segment direction agrees between 1 and 3, and 2 and 3
//         if ( isEndcap1 != isEndcap3 ) continue;
//         if ( !matchPairSegment( globalDir1, globalDir3 ) ) continue;
//         if ( !matchPairSegment( globalDir2, globalDir3 ) ) continue;


//         // I want to take the track and propagate it to the 3rd segment
//         // Surface on which the 3rd segment is defined
//         const Trk::Surface& segSurface 	     = mSeg3->associatedSurface();
//         const Amg::Vector2D* localPos3  = segSurface.globalToLocal(globalPos3, 1000.);


//         // PROPAGATE TRACK TO 3RD SEGMENT USING THE SURFACE ON WHICH THE SEGMENT IS DEFINED
// 	const Trk::TrackParameters* extraPar = p_IExtrapolator->extrapolate( *track, segSurface, Trk::anyDirection, false, Trk::nonInteracting);
       
//         if ( !extraPar ) {
// 	  continue;
// 	}

//         const Amg::Vector2D& trklocalPos   = extraPar->localPosition();
//         const Amg::Vector3D* trkglobalPos  = segSurface.localToGlobal( trklocalPos );

//         // Get track pt
//         float measPerigee_p_T = 10.0;
//         if ( !p_helperTool->isSLTrack(*track) ) {
//           const  Trk::Perigee *measPerigee = dynamic_cast< const Trk::Perigee *>(track->perigeeParameters());
//           if (!measPerigee) continue;
//           measPerigee_p_T = measPerigee->pT()/1000.; // in GeV
//         }

//         float dX = trkglobalPos->x() - globalPos3.x();
//         float dY = trkglobalPos->y() - globalPos3.y();
//         float dZ = trkglobalPos->z() - globalPos3.z();

// 	// This is the precision direction in local coordinates (better for endcap)
//         float dZlocal = trklocalPos.y() - localPos3->y();

//         // Radial distance between track and segment at surface:
// 	//float dR = sqrt( dX * dX + dY * dY + dZ * dZ );

//         // Test that segment and track are within 10 cm of each other in x, y and z
//         float absDx = dX;
//         if ( absDx < 0. ) absDx = -dX;
//         float absDy = dY;
//         if ( absDy < 0. ) absDy = -dY;
//         float absDz = dZ;
//         if ( absDz < 0. ) absDz = -dZ;

//         if ( absDx > 100. || absDy > 100. || absDz > 100. ) continue;

// 	// haveTrackSegMatch = true;
	
// 	// Compare pt of Moore/Muonboy track with pt of 2-segment track


//         float bestdZloc = 9999.;
//         float muMeasPerigee_p_T = 1.;
//         float deltaPt_pct = 999.;

//         if ( true ) {
//           for (unsigned i=0; i<m_tracks->size(); i++){
//             // Get track
//             const Trk::Track* muTrack=(*m_tracks)[i];
//             if (muTrack == 0) {
//               continue;
//             }

//             // Extrapolate to surface
//             const Trk::TrackParameters* muExtraPar = p_IExtrapolator->extrapolate( *muTrack, segSurface, Trk::anyDirection, false, Trk::nonInteracting);
//             if ( !muExtraPar ) continue;
//             const Amg::Vector2D& muTrklocalPos = muExtraPar->localPosition();

//             // Compare with track from 2 segments
// 	    float tmp_muTrkdZloc = trklocalPos.y() - muTrklocalPos.y();  // The precision direction !!!

//             if ( tmp_muTrkdZloc < 0.) tmp_muTrkdZloc = -tmp_muTrkdZloc;

// 	    if ( tmp_muTrkdZloc > bestdZloc ) continue;
//             bestdZloc = tmp_muTrkdZloc;

//             if ( !p_helperTool->isSLTrack(*muTrack) ) {
//               const  Trk::Perigee *muMeasPerigee = dynamic_cast< const Trk::Perigee *>(muTrack->perigeeParameters());
//               if (!muMeasPerigee) continue;
//               muMeasPerigee_p_T = muMeasPerigee->pT()/1000.;
//             }
            
//             delete muExtraPar; 

//           }

//           if ( muMeasPerigee_p_T > 1.) deltaPt_pct = (measPerigee_p_T / muMeasPerigee_p_T - 1.0) * 100.0; // % difference

//         } // if goodTrkColl


//         // Here, modify phiSector so it matches the 1-16 map instead of 1-8:
//         if ( isSmall3 ) {
//           phiSector3 = 2 * phiSector3;     // Small chambers start at 2
//         }
//         else {
//           phiSector3 = 2 * phiSector3 - 1; // Large chambers start at 1
//         }

//         // Output to log file:  for debug purposes
//         //ATH_MSG_DEBUG("MuonAlignValidationResiduals "  << isEndcap3 << " " << isCSC1 << " " << phiSector3 << " " <<  etaSector3 << " " << dX << " " << dY << " " << dZ << " " << dR << " " << deltaPt_pct );


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

//         /*	
// 	bool dominiqueCheck = false;
// 	if ( dominiqueCheck ) {
//           if (hIndex < 1 ) {
//             std::cout << "MuonAlignMuonBoy "  << isEndcap3 << " " << thisEtaSector << " "  
// 		      << etaSector3 << " " << dX << " " << dY << " " << dZ << " " << dR << " " 
// 		      << measPerigee_p_T - muMeasPerigee_p_T << " " << std::endl;
//           } else {
//             std::cout << "MuonAlignMoore "  << isEndcap3 << " " << thisEtaSector << " "  
// 		      << etaSector3 << " " << dX << " " << dY << " " << dZ << " " << dR << " " 
// 		      << measPerigee_p_T - muMeasPerigee_p_T << " " << std::endl;
//           }
//         }
//         */

// 	if ( dZlocal < 2.0 && dZlocal > -2.0 ) {	  
// 	  if ( thisEtaSector > -1 && thisEtaSector < 16 ) {
// 	    if (phiSector3 > 0 && phiSector3 < 17 ) {
// 	      if ( hIndex < 1) { 
// 	        zLocSumMboy[thisEtaSector][phiSector3-1] += dZlocal;
// 	        zLocCountMboy[thisEtaSector][phiSector3-1]++;
// 	      } else if ( hIndex < 2) {
// 	        zLocSumMoore[thisEtaSector][phiSector3-1] += dZlocal;
// 	        zLocCountMoore[thisEtaSector][phiSector3-1]++;
// 	      } 
// 	    }
// 	  }
//         }

//         // Fill histograms here:

//         // avg in phi sector
//         if ( isEndcap3 ) {
// 	  if ( isSmall3 ) {
// 	    if ( globalPos3.z() < 0. ) {
// 	      m_h_endcapCS_trackZ_segmentZ[hIndex]->Fill( dZlocal );
// 	      m_h_endcapCS_trackPt_segmentPt[hIndex]->Fill( deltaPt_pct );
// 	    } else {
// 	      m_h_endcapAS_trackZ_segmentZ[hIndex]->Fill( dZlocal );
// 	      m_h_endcapAS_trackPt_segmentPt[hIndex]->Fill( deltaPt_pct );
// 	    }
//           } else {
// 	    if ( globalPos3.z() < 0. ) {
// 	      m_h_endcapCL_trackZ_segmentZ[hIndex]->Fill( dZlocal );
// 	      m_h_endcapCL_trackPt_segmentPt[hIndex]->Fill( deltaPt_pct );
// 	    } else {
// 	      m_h_endcapAL_trackZ_segmentZ[hIndex]->Fill( dZlocal );
// 	      m_h_endcapAL_trackPt_segmentPt[hIndex]->Fill( deltaPt_pct );
// 	    }
// 	  }
//         }

// 	if ( isEndcap3 ) continue; // next plots are for barrel only

// 	if ( isSmall3 ) {
// 	  m_h_barrelS_trackZ_segmentZ[hIndex]->Fill( dZlocal );
// 	  m_h_barrelS_trackPt_segmentPt[hIndex]->Fill( deltaPt_pct );
// 	} else {
// 	  m_h_barrelL_trackZ_segmentZ[hIndex]->Fill( dZlocal );
// 	  m_h_barrelL_trackPt_segmentPt[hIndex]->Fill( deltaPt_pct );
// 	}

//         delete extraPar;

//       }  // end of loop on middle station segment list to match to track

//       delete track;

//     }  // end of loop on outer station segment list 

//   }  // end of loop on inner station segment list


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
// bool MuonAlignValidation::goodSegmentQuality(float& chi2, int& nhots, int& nholes, bool& isMDT, bool& isEndcap, int& station) {

//   bool pass = true;

//   // CSC seem to have tight internal constraints anyway...
//   if ( !isMDT ) {
//     return true;
//   }

//   int mdtNhotsMin   = m_bMdtnhotsmin;
//   int mdtNholeMax   = m_bMdtnholemax;
//   float mdtChi2Max = m_bMdtchi2max;


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
//    Track match method
 
//    Test that segment is associated with an existing track
//    This is needed for studying non-pointing tracks which go across several towers
//    This requires way to much CPU...
//    ********************************************************** */
// bool MuonAlignValidation::hasTrackMatch( std::vector<Muon::MuonSegment* > muonSegs, const DataVector<Trk::Track> *m_tracks ) {

//   bool pass = true;

//   // Loop over segment vector
//   for ( unsigned i = 0; i < muonSegs.size(); ++i ) {

//     Muon::MuonSegment* mSeg = muonSegs[i];

//     // Get the identifier (chamber ID)
//     Identifier id1                        = p_helperTool->chamberId( *mSeg );
//     int stationId1                        = p_idHelperTool->stationIndex( id1 );
//     bool isCSC1                           = p_idHelperTool->isCsc( id1 );
//     bool isEndcap1                        = p_idHelperTool->isEndcap( id1 );

//     int type = -1;
//     if ( isCSC1 || stationId1 == 0 || stationId1 == 4) {
//       type = 0;
//       if ( hasShoweredInn[i] > 0 ) {
// 	hasTrackMatchInn.push_back( -1 );
// 	continue;  
//       }
//     }
//     else if ( stationId1 == 1 || stationId1 == 5 ) {
//       type = 1;
//       if ( hasShoweredMid[i] > 0 ) {
// 	hasTrackMatchMid.push_back( -1 );
// 	continue;  
//       }
//     }
//     else {
//       type = 2;
//       if ( hasShoweredOut[i] > 0 ) {
// 	hasTrackMatchOut.push_back( -1 );
// 	continue;  
//       }
//     }

//     // Get segment measurements
//     const Amg::Vector3D& globalPos  = mSeg->globalPosition();

//     // I want to take the track and propagate it to the segment and see if have match
//     const Trk::Surface& segSurface = mSeg->associatedSurface();

//     bool foundMatch = false;
//     int trackId = -1;

//     // Loop over tracks to see if have match
//     for ( unsigned j=0; j<m_tracks->size(); ++j ) {

//       if ( foundMatch ) continue;

//       // Get track
//       const Trk::Track* muTrack=(*m_tracks)[j];
//       if ( muTrack == 0 ) continue;

//       // Extrapolate to surface and get position on surface
//       const Trk::TrackParameters* extraPar = p_IExtrapolator->extrapolate( *muTrack, segSurface, Trk::anyDirection, false, Trk::nonInteracting);
//       if ( !extraPar ) continue;
      
//       const Amg::Vector2D& trklocalPos   = extraPar->localPosition();
//       const Amg::Vector3D* trkglobalPos  = segSurface.localToGlobal( trklocalPos );

//       // Compare with segment
//       float dZ = trkglobalPos->z() - globalPos.z();
//       if ( dZ < 0. ) dZ = -dZ;
//       if ( dZ > 100. ) continue;

//       float twopi = 2. * 3.1416;
//       float Phi1 = trkglobalPos->phi();
//       if (Phi1 < 0. ) Phi1 += twopi;
//       float Phi2 = globalPos.phi();
//       if (Phi2 < 0. ) Phi2 += twopi;
//       float dPhi = Phi1 - Phi2;
//       if ( dPhi < 0. ) dPhi = -dPhi;
//       if ( dPhi > twopi ) dPhi -= twopi;

//       // twopi /16 = 1 chamber width in phi
//       if ( dPhi > twopi / 32. ) continue;  // within half a chamber in phi

//       if ( isEndcap1 ) {
//         // Cylindrical R
//         float Rseg   = sqrt(globalPos.x()*globalPos.x() + globalPos.y()*globalPos.y());
//         float Rtrack = sqrt( trkglobalPos->x()*trkglobalPos->x() + trkglobalPos->y()*trkglobalPos->y() );
//         float dR = Rtrack - Rseg;
// 	if ( dR < 0.) dR = -dR;

// 	if ( dR < 50. ) foundMatch = true;   
// 	trackId = j;
 
//       } else {

//         if ( dZ < 50. ) foundMatch = true;
// 	trackId = j;
//       }

//     }


//     if ( type == 0) {
//       if ( foundMatch ) {
// 	hasTrackMatchInn.push_back( trackId );
//       } else {
//         hasShoweredInn[i] = 999;
//         hasTrackMatchInn.push_back( -1 );
//       }
//     }
   
//     if ( type == 1) {
//       if ( foundMatch ) {
//         hasTrackMatchMid.push_back( trackId );
//       } else {
//         hasShoweredMid[i] = 999;
//         hasTrackMatchMid.push_back( -1 );
//       }
//     }

//     if ( type == 2) {
//       if ( foundMatch ) {
//         hasTrackMatchOut.push_back( trackId );
//       } else {
//         hasShoweredOut[i] = 999;
//         hasTrackMatchOut.push_back( -1 );
//       }
//     }


//   }

//   return pass;

// }


// /* **********************************************************
//    matchPairSegment method
 
//    Test if segment pair is close enough in theta, phi to be considered
//    Use segment direction here
//    ********************************************************** */
// bool MuonAlignValidation::matchPairSegment( const Amg::Vector3D& globalDir1,const Amg::Vector3D& globalDir2 ) {

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

//   if ( dR > m_mdtdelphimax ) return false; // this is saying R ~ phi < phimax

//   return pass;

// }


// /* **********************************************************
//    showeringTest method
 
//    Test if segment showered in this chamber, i.e. 
//    look for abnormal number of hits on tracks 
//    and/or several segments clustered in same chamber
//    ********************************************************** */
// void MuonAlignValidation::showeringTest( std::vector<Muon::MuonSegment* > muonSegs, int stationIdx  ) {

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
// // /* **********************************************************/
// // TH1F* MuonAlignValidation::makeAndRegisterTH1F(MonGroup& mon, const char* hName, std::string hTitle, int nBins, float minX, float maxX) {
// // 	  TH1F* h = TH1F::create(hName,hTitle.c_str(),nBins,minX,maxX);
// // 	  //h->Sumw2();
// // 	  if (mon.regHist(h).isFailure()) {
// // 	    ATH_MSG_WARNING ("Unable to book histogram with name = "+std::string(hName));
// // 	  }
// // 	  return h;
// // 	}
// // 	
// // /* **********************************************************/	
// // 	TH2F* MuonAlignValidation::makeAndRegisterTH2F(MonGroup& mon,
// // 	                                             const char* hName, std::string hTitle,
// // 	                                             int nBinsX, float minX, float maxX,
// // 	                                             int nBinsY, float minY, float maxY) {
// // 	  TH2F* h = TH2F::create(hName,hTitle.c_str(),nBinsX,minX,maxX,nBinsY,minY,maxY);
// // 	  //h->Sumw2();
// // 	  //h->SetOption("colz");
// // 	  if (mon.regHist(h).isFailure()) {
// // 	    ATH_MSG_WARNING ("Unable to book histogram with name = "+std::string(hName));
// // 	  }
// // 	  return h;
// // 	}
// // 
