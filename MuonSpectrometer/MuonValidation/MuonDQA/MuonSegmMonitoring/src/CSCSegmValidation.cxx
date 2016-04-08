/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ===========================================================================
   CSCSegmValidation.cxx
   ------------------------------

   AUTHORS:     I. Christidi
   Modified:    N. Benekos (Illinois) 
   created:     January 2008
   description: Implementation code for the CSCSegmValidation
   =========================================================================== */

// #include "MuonSegmMonitoring/CSCSegmValidation.h"
// #include "MuonRecHelperTools/MuonEDMPrinterTool.h"
// #include "MuonRecHelperTools/MuonEDMHelperTool.h"
// #include "MuonIdHelpers/MuonIdHelperTool.h"  
// #include "MuonDQAUtils/MuonChamberNameConverter.h"
// #include "MuonDQAUtils/MuonDQAFitFunc.h"

// // Track
// #include "TrkExInterfaces/IPropagator.h"
// #include "TrkToolInterfaces/IResidualPullCalculator.h"

// #include "TrkEventPrimitives/FitQualityOnSurface.h"
// #include "EventPrimitives/EventPrimitives.h"
// #include "TrkEventPrimitives/FitQuality.h"
// #include "TrkMeasurementBase/MeasurementBase.h"
// #include "TrkSurfaces/BoundaryCheck.h"

// #include "TrkParameters/TrackParameters.h"
// #include "TrkGeometry/MagneticFieldProperties.h"

// #include "TrkTrack/TrackCollection.h"
// #include "TrkEventPrimitives/ResidualPull.h"
// #include "TrkEventPrimitives/PropDirection.h"

// #include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
// #include "MuonRIO_OnTrack/CscClusterOnTrack.h"

// // Gaudi Tools
// #include "GaudiKernel/SmartDataPtr.h"
// #include "GaudiKernel/INTupleSvc.h"
// #include "AthenaMonitoring/AthenaMonManager.h"

// // Storegate
// #include "StoreGate/StoreGate.h"

// // ROOT Classes
// #include <TH1F.h>
// #include <TH2F.h>

// // C++ 
// #include <TMath.h>
// #include <sstream>


// const int s_maxEvents     = 100000;
// const int s_maxComponents = 50;
// const int s_maxSoS        = 80;

// namespace CscBins {
//   void BinLabels(TH1 *h, int m_side) {
//     h->GetXaxis()->SetTitle("");
//     h->GetXaxis()->SetLabelSize(0.03);
//     if(m_side == -1) {
//       for(size_t j=6; j<86; j++) {
//         if( j%5 != 0 ) {
//           float xmid = h->GetBinLowEdge(j) + h->GetBinWidth(j);
//           xmid -= 1.0; xmid *= -1.0;
//           int seclay = int(ceil(xmid*10)); // get sector/layer
//           int sec = seclay/10;
//           int lay = (seclay%10)/2 ;
//           lay = (4 - lay) + 1;
//           h->GetXaxis()->SetBinLabel(j,Form("%c%02d:%d",(sec%2==0?'S':'L'),sec,lay));
//         } // end for
//       } // end if
//     } else if (m_side == 1) {
//       for(size_t j=6; j<86; j++) {
//         if( j%5 != 0 ) {
//           float xmid = h->GetBinLowEdge(j) + h->GetBinWidth(j);
//           int seclay = int(ceil(xmid*10)); // get sector/layer
//           int sec = seclay/10;
//           int lay = (seclay%10)/2 ;
//           h->GetXaxis()->SetBinLabel(j,Form("%c%02d:%d",(sec%2==0?'S':'L'),sec,lay));
//         }
//       } // end for
//     } // end else

//   } // BinLabels 

// } // end namespace CscBins


// // *********************************************************************
// // Public Methods
// // *********************************************************************

// CSCSegmValidation::CSCSegmValidation( const std::string & type, const std::string & name, const IInterface* parent )
//  :ManagedMonitorToolBase( type, name, parent ), 
//   m_storeGate(0),
//   m_activeStore(NULL),
//   m_muonMgr(NULL),
//   m_cscIdHelper(NULL),
//   m_tgcIdHelper(NULL),
//   m_segms(NULL),
//   m_eventCounter(0),
//   m_eventNumber(-1),
//   m_rec_nSoS(-1),
//   m_debuglevel(false),
//   b_status(false),
//   cscsegm_detailEA_Moore(NULL),
//   cscsegm_detailEA_MuBoy(NULL),
//   cscsegm_detailEC_Moore(NULL),
//   cscsegm_detailEC_MuBoy(NULL),
//   cscsegm_oviewEA_Moore(NULL),
//   cscsegm_oviewEA_MuBoy(NULL),
//   cscsegm_oviewEC_Moore(NULL),
//   cscsegm_oviewEC_MuBoy(NULL)
// {
//   m_pullCalculator = ToolHandle<Trk::IResidualPullCalculator>("Trk::ResidualPullCalculator/ResidualPullCalculator");
//   m_helperTool     = ToolHandle<Muon::MuonEDMHelperTool>("Muon::MuonEDMHelperTool/MuonEDMHelperTool");
//   m_printer        = ToolHandle<Muon::MuonEDMPrinterTool>("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool");
//   m_idHelperTool   = ToolHandle<Muon::MuonIdHelperTool>("Muon::MuonIdHelperTool/MuonIdHelperTool");
//   m_propagator     = ToolHandle<Trk::IPropagator>("Trk::StraightLinePropagator/MuonStraightLinePropagator");
//   m_histoTool      = ToolHandle<Muon::MuonDQAHistTool>("Muon::MuonDQAHistTool/MuonDQAHistTool");
//   m_TrackHistoTool = ToolHandle<Muon::MuonDQATrackHistTool>("Muon::MuonDQATrackHistTool/MuonDQATrackHistTool");
//   m_muondqafitfunc = ToolHandle<Muon::MuonDQAFitFunc>("Muon::MuonDQAFitFunc/MuonDQAFitFunc");

//   declareProperty( "WhichSegmCollections", m_segmCollectionFlag);
//   declareProperty( "MuonSegmCollections",  m_segmCollectionName);
//   declareProperty( "doMDT",  m_doMDT);
//   declareProperty( "doRPC",  m_doRPC);
//   declareProperty( "doTGC",  m_doTGC);
//   declareProperty( "doCSC",  m_doCSC);
// }

// //
// //---------------------------------------------------------------------------------------
// //

// CSCSegmValidation::~CSCSegmValidation()
// {
//   ATH_MSG_INFO( " deleting CSCSegmValidation " );
// }


// //==================================================================//
// /**   Initialize  */
// //==================================================================//

// StatusCode CSCSegmValidation::initialize()
// {
//   StatusCode sc = ManagedMonitorToolBase::initialize();
//   if(!sc.isSuccess()) return sc;

//   ATH_MSG_INFO( "in initialize() in SegmMonitoring" );

//   //initializing tools
//   // Retrieve the StoreGate service

//   sc = service( "StoreGateSvc", m_storeGate );
//   if ( sc.isFailure() ){
//     ATH_MSG_FATAL( "Unable to retrieve the StoreGate service... Exiting!" );
//     return sc;
//   }
//   if (m_debuglevel) ATH_MSG_DEBUG(  "Defined detector service" );

//   // retrieve the active store
//   sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL( " Cannot get ActiveStoreSvc " );
//     return sc ;
//   }
//   if (m_debuglevel) ATH_MSG_DEBUG(  "Defined ActiveStoreSvc" );

//   // Initialize the IdHelper
//   StoreGateSvc* detStore = 0;
//   sc = service("DetectorStore", detStore);
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL( "DetectorStore service not found !" );
//     return sc;
//   }   
//   if (m_debuglevel) ATH_MSG_DEBUG(  "Defined DetectorStore" );

//   // Retrieve the MuonDetectorManager  
//   sc = detStore->retrieve(m_muonMgr);
//   if (sc.isFailure()) {
//     ATH_MSG_DEBUG( "Cannot get MuonDetectorManager from detector store" );
//     return sc;
//   }
//   if (m_debuglevel) ATH_MSG_DEBUG(  " Found the MuonDetectorManager from detector store. " );

//   sc = detStore->retrieve(m_cscIdHelper,"CSCIDHELPER");
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL( "Cannot get CscIdHelper" );
//     return sc;
//   }  
//   if (m_debuglevel) ATH_MSG_DEBUG( " Found the CscIdHelper " );

//   sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL( "Can't retrieve TgcIdHelper" );
//     return sc;
//   }	   
//   if (m_debuglevel) ATH_MSG_DEBUG( " Found the TgcIdHelper " );

//   // Retrieve pull calculator
//   sc = m_pullCalculator.retrieve();
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL("Can't retrieve " << m_pullCalculator );
//     return sc;
//   }	   
//   if (m_debuglevel) ATH_MSG_FATAL(" Found the " << m_pullCalculator );

//   // Retrieve helper tools
//   sc = m_helperTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL("Could not get " << m_helperTool ); 
//     return sc;
//   }
//   if (m_debuglevel) ATH_MSG_DEBUG( "Retrieved " << m_helperTool );

//   sc = m_idHelperTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL( "Could not get " << m_idHelperTool ); 
//     return sc;
//   }
//   if (m_debuglevel) ATH_MSG_DEBUG( "Retrieved " << m_idHelperTool );

//   // Retrieve printer
//   sc = m_printer.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL( "Could not get " << m_printer ); 
//     return sc;
//   }
//   if (m_debuglevel) ATH_MSG_DEBUG( "Retrieved " << m_printer );

//   // Retrieve histoTool
//   sc = m_histoTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL( "Could not get " << m_histoTool ); 
//     return sc;
//   }
//   if (m_debuglevel) ATH_MSG_DEBUG(  "Retrieved " << m_histoTool );

//   // Retrieve TrackHistoTool
//   sc = m_TrackHistoTool.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL( "Could not get " << m_TrackHistoTool ); 
//     return sc;
//   }
//   if (m_debuglevel) ATH_MSG_DEBUG(  "Retrieved " << m_TrackHistoTool );

//   // Retrieve propagators
//   sc = m_propagator.retrieve();
//   if( sc.isFailure() ){
//     ATH_MSG_FATAL( "Could not get " << m_propagator ); 
//     return sc;
//   }
//   if (m_debuglevel) ATH_MSG_INFO( "Retrieved " << m_propagator );

//   // Retrieve fitting tool
//   sc = m_muondqafitfunc.retrieve();
//   if (sc.isFailure()){
//     ATH_MSG_FATAL( "Could not get " << m_muondqafitfunc ); 
//     return sc;
//   }
//   if (m_debuglevel)  ATH_MSG_INFO( "Retrieved " << m_muondqafitfunc );

//   for(int collection = 0; collection < 2; collection++){
//     for(int endcap = 0; endcap < 2; endcap++){
//       h_nsegm[collection][endcap] = 0;
//       h_nsegm_oview[collection][endcap] = 0;
//       for(int sector = 0; sector < 16; sector++){
//         ATH_MSG_DEBUG( " initializing = " << endcap << " sector = " << sector + 1 );
//         h_nsegm_sect[collection][endcap][sector] = 0;
//       } // sector
//     } // endcap
//   } // collection

//   ecap.clear(); coll.clear();
//   ecap.push_back("ECA");
//   ecap.push_back("ECC");
//   coll.push_back("Moore");
//   coll.push_back("MuBoy");

//   return sc;
// }  

// //
// //---------------------------------------------------------------------------------------
// //

// void CSCSegmValidation::bookSegmentHistograms() {

//   // Clear MonGroups
//   //m_cscSegmExpert.clear();
//   //m_cscSegmShift.clear();
//   m_cscSegmDetailEA_Moore.clear();
//   m_cscSegmDetailEA_MuBoy.clear();
//   m_cscSegmDetailEC_Moore.clear();
//   m_cscSegmDetailEC_MuBoy.clear();
//   m_cscSegmOviewEA_Moore.clear();
//   m_cscSegmOviewEA_MuBoy.clear();
//   m_cscSegmOviewEC_Moore.clear();
//   m_cscSegmOviewEC_MuBoy.clear();

//   // Book histograms
//   //h_nclust_per_segm = new TH1F("csc_nclust_per_segm","Number of clusters on CSC segments",4,0,4);
//   //chi2_histo = new TH1F("csc_chi2","Chi2 of csc segments",100,0,100);

//   //std::string ecap[2]= {"ECA", "ECC"  };
//   //std::string coll[2]= {"Moore", "MuBoy" };
//   for(int collection = 0; collection < 2; collection++){
//     for(int endcap = 0; endcap < 2; endcap++){
//       h_nsegm[collection][endcap] = new TH1F(Form("%s_%s_csc_nsegm",coll[collection].c_str(),ecap[endcap].c_str()),
// 					     Form("Number of %s segments at %s;#segments;entries",coll[collection].c_str(),ecap[endcap].c_str()),20,0,20);
//       ATH_MSG_DEBUG( " booked histo for " << coll[collection] << " collection endcap = " << ecap[endcap]  );
//       h_nsegm_oview[collection][endcap] = new TH1F(Form("%s_%s_csc_nsegm",coll[collection].c_str(),ecap[endcap].c_str()),
// 						   Form("Number of %s segments at %s;#segments;entries",coll[collection].c_str(),ecap[endcap].c_str()),20,0,20);
//       ATH_MSG_DEBUG( " booked overview histo for " << coll[collection] << " collection endcap = " << ecap[endcap]  );
//       for(int sector = 0; sector < 16; sector++){
//         ATH_MSG_DEBUG( " endcap = " << endcap << " sector = " << sector + 1 );
//         h_nsegm_sect[collection][endcap][sector] = new TH1F(Form("%s_%s_csc_nsegm_sec%02d",coll[collection].c_str(),ecap[endcap].c_str(),sector+1),
// 							    Form("Number of %s segments at %s sector %02d;#segments;entries",coll[collection].c_str(),ecap[endcap].c_str(),sector+1),20,0,20);
//         ATH_MSG_DEBUG( " booked histo for " << coll[collection] << " collection endcap = " << ecap[endcap] << " sector = " << sector  );
//       } // sector
//     } // endcap
//   } // collection

//   // Add histograms to MonGroups
//   // nsegm detail histos
//   m_cscSegmDetailEA_Moore.push_back(h_nsegm[0][0]);
//   ATH_MSG_DEBUG( " added histo for " << coll[0] << " collection endcap = " << ecap[0]  );
//   m_cscSegmDetailEA_MuBoy.push_back(h_nsegm[1][0]);
//   ATH_MSG_DEBUG( " added histo for " << coll[1] << " collection endcap = " << ecap[0]  );
//   m_cscSegmDetailEC_Moore.push_back(h_nsegm[0][1]);
//   ATH_MSG_DEBUG( " added histo for " << coll[0] << " collection endcap = " << ecap[1]  );
//   m_cscSegmDetailEC_MuBoy.push_back(h_nsegm[1][1]);
//   ATH_MSG_DEBUG( " added histo for " << coll[1] << " collection endcap = " << ecap[1]  );

//   // nsegm overview histos
//   m_cscSegmOviewEA_Moore.push_back(h_nsegm_oview[0][0]);
//   ATH_MSG_DEBUG( " added overview histo for " << coll[0] << " collection endcap = " << ecap[0]  );
//   m_cscSegmOviewEA_MuBoy.push_back(h_nsegm_oview[1][0]);
//   ATH_MSG_DEBUG( " added overview histo for " << coll[1] << " collection endcap = " << ecap[0]  );
//   m_cscSegmOviewEC_Moore.push_back(h_nsegm_oview[0][1]);
//   ATH_MSG_DEBUG( " added overview histo for " << coll[0] << " collection endcap = " << ecap[1]  );
//   m_cscSegmOviewEC_MuBoy.push_back(h_nsegm_oview[1][1]);
//   ATH_MSG_DEBUG( " added overview histo for " << coll[1] << " collection endcap = " << ecap[1]  );

//   // nsegm_sect histos
//   for(int sector = 0; sector < 16; sector++){
//     // detail histos
//     m_cscSegmDetailEA_Moore.push_back(h_nsegm_sect[0][0][sector]);
//     ATH_MSG_DEBUG( " added histo for " << coll[0] << " collection endcap = " << ecap[0] << " sector = " << sector  );
//     m_cscSegmDetailEA_MuBoy.push_back(h_nsegm_sect[1][0][sector]);
//     ATH_MSG_DEBUG( " added histo for " << coll[1] << " collection endcap = " << ecap[0] << " sector = " << sector  );
//     m_cscSegmDetailEC_Moore.push_back(h_nsegm_sect[0][1][sector]);
//     ATH_MSG_DEBUG( " added histo for " << coll[0] << " collection endcap = " << ecap[1] << " sector = " << sector  );
//     m_cscSegmDetailEC_MuBoy.push_back(h_nsegm_sect[1][1][sector]);
//     ATH_MSG_DEBUG( " added histo for " << coll[1] << " collection endcap = " << ecap[1] << " sector = " << sector  );
//   } // sector

//   //m_cscSegmExpert.push_back(chi2_histo);
//   //m_cscSegmExpert.push_back(h_nclust_per_segm);

// }

// //
// //---------------------------------------------------------------------------------------
// //

// StatusCode CSCSegmValidation::bookHistograms()
// {
//   if (m_debuglevel) {
//     ATH_MSG_DEBUG( "CSCSegmValidation::bookHistograms() to setup tools/services" );
//   }
//   StatusCode sc = StatusCode::SUCCESS;

//   if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

//     histos_from_tool.push_back(histos_from_tool_Moore);
//     histos_from_tool.push_back(histos_from_tool_MBoy);
//     ATH_MSG_DEBUG( "Filled vector with structures histos_from_tool" );

//     for (int j=0;j<(int)m_segmCollectionName.size();j++) {
//       if ( m_segmCollectionFlag[j] == 0 ) continue;

//       std::string m_path_muonsegm = "Muon/MuonSegmentMonitoring";
//       MgmtAttr_t attr = ATTRIB_MANAGED;
//       MonGroup al_muonsegms_shift(this,m_path_muonsegm+"/Global/"+m_segmCollectionName[j],run, attr );
//       MonGroup al_muonsegms_debug(this,m_path_muonsegm+"/Global/"+m_segmCollectionName[j],run, attr );

//       MonGroup al_mBAsegms_debug(this,m_path_muonsegm+"/BarrelA/Detail/"+m_segmCollectionName[j],run, attr );
//       MonGroup al_mBCsegms_debug(this,m_path_muonsegm+"/BarrelC/Detail/"+m_segmCollectionName[j],run, attr );
//       MonGroup al_mEAsegms_debug(this,m_path_muonsegm+"/EndCapA/Detail/"+m_segmCollectionName[j],run, attr );
//       MonGroup al_mECsegms_debug(this,m_path_muonsegm+"/EndCapC/Detail/"+m_segmCollectionName[j],run, attr );

//       MonGroup al_mEsegms_debug(this,m_path_muonsegm+"/EndCap/Detail/"+m_segmCollectionName[j],run, attr );

//       MonGroup al_mBAsegms_shift(this,m_path_muonsegm+"/BarrelA/Overview/"+m_segmCollectionName[j],run, attr );
//       MonGroup al_mBCsegms_shift(this,m_path_muonsegm+"/BarrelC/Overview/"+m_segmCollectionName[j],run, attr );
//       MonGroup al_mEAsegms_shift(this,m_path_muonsegm+"/EndCapA/Overview/"+m_segmCollectionName[j],run, attr );
//       MonGroup al_mECsegms_shift(this,m_path_muonsegm+"/EndCapC/Overview/"+m_segmCollectionName[j],run, attr );

//       if(newEventsBlock){}
//       if(newLumiBlock){}
//       if(newRun)
// 	{      
// 	  ATH_MSG_INFO( "Segment Monitoring : begin of run" );
// 	  std::string prefix = ( m_segmCollectionName[j]=="ConvertedMBoySegments" ? "MuBoy_" : "Moore_" );

// 	  b_status = m_TrackHistoTool->book(histos_from_tool[j],prefix,false,false,false,true);
// 	  ATH_MSG_INFO( "Booked histos for " << m_segmCollectionName[j] << " with histTool" );

// 	  // Book some histograms
// 	  bookSegmentHistograms();

// 	  // Register MonGroups here
// 	  std::vector<TH1 *>::iterator m_iT;
// 	  /*
// 	  // register shift histograms
// 	  MonGroup cscsegm_shift( this, "Muon/MuonSegmentMonitoring/CSC/Segment/Shift", shift, run );
// 	  std::vector<TH1 *>::iterator m_iT = m_cscSegmShift.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmShift.size() << " shift Histograms " );
// 	  for (; m_iT != m_cscSegmShift.end(); ++m_iT) {
//           sc = cscsegm_shift.regHist(*m_iT);
//           if ( sc.isFailure() ) {
// 	  ATH_MSG_ERROR (  "Cannot register histogram " << (*m_iT)->GetName() );
// 	  return sc;
//           }
// 	  }
// 	  */

// 	  /*
// 	  // register expert histograms
// 	  MonGroup cscsegm_expert( this, "Muon/MuonSegmentMonitoring/CSC/Segment/Expert", expert, run );
// 	  m_iT = m_cscSegmExpert.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmExpert.size() << " expert Histograms " );
// 	  for (; m_iT != m_cscSegmExpert.end(); ++m_iT) {
//           sc = cscsegm_expert.regHist(*m_iT);
//           if ( sc.isFailure() ) {
// 	  ATH_MSG_ERROR (  "Cannot register histogram " << (*m_iT)->GetName() );
// 	  return sc;
//           }
// 	  }
// 	  */

// 	  // register detail histograms for EA
// 	  cscsegm_detailEA_Moore = new MonGroup( this, "Muon/MuonSegmentMonitoring/EndCapA/Detail/MooreSegments", run, attr );
// 	  m_iT = m_cscSegmDetailEA_Moore.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmDetailEA_Moore.size() << " CSCEA Moore Detail Histograms " );
// 	  for (; m_iT != m_cscSegmDetailEA_Moore.end(); ++m_iT) {
// 	    sc = cscsegm_detailEA_Moore->regHist(*m_iT);
// 	    if ( sc.isFailure() ) {
// 	      ATH_MSG_ERROR (  "Cannot register detail histogram for Endcap A Moore Segments: " << (*m_iT)->GetName() );
// 	      return sc;
// 	    }
// 	  }

// 	  cscsegm_detailEA_MuBoy = new MonGroup( this, "Muon/MuonSegmentMonitoring/EndCapA/Detail/ConvertedMBoySegments", run, attr );
// 	  m_iT = m_cscSegmDetailEA_MuBoy.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmDetailEA_MuBoy.size() << " CSCEA MuBoy Detail Histograms " );
// 	  for (; m_iT != m_cscSegmDetailEA_MuBoy.end(); ++m_iT) {
// 	    sc = cscsegm_detailEA_MuBoy->regHist(*m_iT);
// 	    if ( sc.isFailure() ) {
// 	      ATH_MSG_ERROR (  "Cannot register detail histogram for Endcap A MuBoy Segments: " << (*m_iT)->GetName() );
// 	      return sc;
// 	    }
// 	  }

// 	  // register overview histograms for EA
// 	  cscsegm_oviewEA_Moore = new MonGroup( this, "Muon/MuonSegmentMonitoring/EndCapA/Overview/MooreSegments", run, attr );
// 	  m_iT = m_cscSegmOviewEA_Moore.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmOviewEA_Moore.size() << " CSCEA Moore Overview Histograms " );
// 	  for (; m_iT != m_cscSegmOviewEA_Moore.end(); ++m_iT) {
// 	    sc = cscsegm_oviewEA_Moore->regHist(*m_iT);
// 	    if ( sc.isFailure() ) {
// 	      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap A Moore Segments: " << (*m_iT)->GetName() );
// 	      return sc;
// 	    }
// 	  }

// 	  cscsegm_oviewEA_MuBoy = new MonGroup( this, "Muon/MuonSegmentMonitoring/EndCapA/Overview/ConvertedMBoySegments", run, attr );
// 	  m_iT = m_cscSegmOviewEA_MuBoy.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmOviewEA_MuBoy.size() << " CSCEA MuBoy Overview Histograms " );
// 	  for (; m_iT != m_cscSegmOviewEA_MuBoy.end(); ++m_iT) {
// 	    sc = cscsegm_oviewEA_MuBoy->regHist(*m_iT);
// 	    if ( sc.isFailure() ) {
// 	      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap A MuBoy Segments: " << (*m_iT)->GetName() );
// 	      return sc;
// 	    }
// 	  }

// 	  // register detail histograms for EC
// 	  cscsegm_detailEC_Moore = new MonGroup( this, "Muon/MuonSegmentMonitoring/EndCapA/Detail/MooreSegments", run, attr );
// 	  m_iT = m_cscSegmDetailEC_Moore.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmDetailEC_Moore.size() << " CSCEC Moore Detail Histograms " );
// 	  for (; m_iT != m_cscSegmDetailEC_Moore.end(); ++m_iT) {
// 	    sc = cscsegm_detailEC_Moore->regHist(*m_iT);
// 	    if ( sc.isFailure() ) {
// 	      ATH_MSG_ERROR (  "Cannot register detail histogram for Endcap A Moore Segments: " << (*m_iT)->GetName() );
// 	      return sc;
// 	    }
// 	  }

// 	  cscsegm_detailEC_MuBoy = new MonGroup( this, "Muon/MuonSegmentMonitoring/EndCapA/Detail/ConvertedMBoySegments", run, attr );
// 	  m_iT = m_cscSegmDetailEC_MuBoy.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmDetailEC_MuBoy.size() << " CSCEC MuBoy Detail Histograms " );
// 	  for (; m_iT != m_cscSegmDetailEC_MuBoy.end(); ++m_iT) {
// 	    sc = cscsegm_detailEC_MuBoy->regHist(*m_iT);
// 	    if ( sc.isFailure() ) {
// 	      ATH_MSG_ERROR (  "Cannot register detail histogram for Endcap A MuBoy Segments: " << (*m_iT)->GetName() );
// 	      return sc;
// 	    }
// 	  }

// 	  // register overview histograms for EC
// 	  cscsegm_oviewEC_Moore = new MonGroup( this, "Muon/MuonSegmentMonitoring/EndCapA/Overview/MooreSegments", run, attr );
// 	  m_iT = m_cscSegmOviewEC_Moore.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmOviewEC_Moore.size() << " CSCEC Moore Overview Histograms " );
// 	  for (; m_iT != m_cscSegmOviewEC_Moore.end(); ++m_iT) {
// 	    sc = cscsegm_oviewEC_Moore->regHist(*m_iT);
// 	    if ( sc.isFailure() ) {
// 	      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap A Moore Segments: " << (*m_iT)->GetName() );
// 	      return sc;
// 	    }
// 	  }

// 	  cscsegm_oviewEC_MuBoy = new MonGroup( this, "Muon/MuonSegmentMonitoring/EndCapA/Overview/ConvertedMBoySegments", run, attr );
// 	  m_iT = m_cscSegmOviewEC_MuBoy.begin();
// 	  ATH_MSG_DEBUG (  "Found " << m_cscSegmOviewEC_MuBoy.size() << " CSCEC MuBoy Overview Histograms " );
// 	  for (; m_iT != m_cscSegmOviewEC_MuBoy.end(); ++m_iT) {
// 	    sc = cscsegm_oviewEC_MuBoy->regHist(*m_iT);
// 	    if ( sc.isFailure() ) {
// 	      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap A MuBoy Segments: " << (*m_iT)->GetName() );
// 	      return sc;
// 	    }
// 	  }

// 	  // Delete a bunch of unwanted histograms
// 	  //deleteUnwantedHistograms(histos_from_tool, m_segmCollectionName, j);

// 	  if (m_debuglevel)  ATH_MSG_DEBUG( "Booked residual histos for " << m_segmCollectionName[j] );
// 	  b_status = m_TrackHistoTool->registerHist(al_muonsegms_shift,al_muonsegms_debug,al_mBAsegms_debug,al_mBCsegms_debug,al_mEsegms_debug,al_mEAsegms_debug,al_mECsegms_debug,histos_from_tool[j]);
// 	  if (m_debuglevel)  ATH_MSG_DEBUG("Finished booking histos for " << m_segmCollectionName[j] );

// 	} // if NewRun

//     } // loop over Muon Segm collections

//   } // environment if  
//   return sc;

// }

// //
// //---------------------------------------------------------------------------------------
// //

// StatusCode CSCSegmValidation::fillHistograms()
// {
//   StatusCode sc = StatusCode::SUCCESS;
//   ATH_MSG_DEBUG(  "******************************************************************************" );
//   ATH_MSG_DEBUG( "                        INSIDE fillHistograms()                             " );
//   ATH_MSG_DEBUG(  "******************************************************************************" );

//   if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ||  m_environment == AthenaMonManager::user ) {

//     // Set the event counter
//     m_eventNumber = m_eventCounter;

//     // Set nsegm counter
//     for(int collection = 0; collection < 2; collection++){
//       for(int endcap = 0; endcap < 2; endcap++){
//         nsegm_Counter[collection][endcap] = 0;
//         for(int sector = 0; sector < 16; sector++){
//           nsegm_sect_Counter[collection][endcap][sector] = 0;
//         } // sector
//       } // endcap
//     } // collection

//     bool got_coll = false;
//     int h_index = -1;
//     for (int j=0;j<(int)m_segmCollectionName.size();++j) {

//       if (m_debuglevel)  ATH_MSG_DEBUG( "Segm collection FLAG is " << m_segmCollectionFlag[j] );

//       if ( m_segmCollectionFlag[j] == 0 ) continue;
//       ++h_index;

//       //Get segms from SG
//       sc = m_storeGate->retrieve(m_segms, m_segmCollectionName[j]);

//       if (m_debuglevel)  ATH_MSG_DEBUG( "Segm collection Name is " << m_segmCollectionName[j] );

//       if ( sc.isFailure() ){
//         if ( (j == (int)m_segmCollectionName.size()-1) && !got_coll ){

//           ATH_MSG_DEBUG( "Unable to retrieve reconstructed segms from any collection... Exiting!" );
//           return StatusCode::FAILURE; }
//         else {

//           ATH_MSG_DEBUG( "Unable to retrieve reconstructed segms from collection... Trying next..." );

//           continue; }
//       }else{

//         if (m_debuglevel){ ATH_MSG_DEBUG( "Segms in StoreGate found");
//           ATH_MSG_DEBUG( "Segm collection Name is " << m_segmCollectionName[j] );}
// 	got_coll = true;
//       }

//       if ( m_segms->empty() ){

//         if (m_debuglevel){
//           ATH_MSG_DEBUG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
//           ATH_MSG_DEBUG( "      Segm Collection is Empty.  Trying next...    ");
//           ATH_MSG_DEBUG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
//         }
//         continue;
//       }

//       if (m_debuglevel){
//         ATH_MSG_DEBUG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
//         ATH_MSG_DEBUG( "      Segm Collection size         " );
//         ATH_MSG_DEBUG( "Number of segms found: " << m_segms->size() );
//         ATH_MSG_DEBUG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
//       }

//       if ( m_segms->size() > 0 ){
//         ATH_MSG_DEBUG(  "********************************* WARNING ************************************" );
//         ATH_MSG_DEBUG(   "Number of segms in event is: " << m_segms->size()                               );
//         ATH_MSG_DEBUG(  "This algorithm is designed to operate for single segm / event only"             );
//         ATH_MSG_DEBUG(  "Processing only the first segm in the segm collection"                         );
//         ATH_MSG_DEBUG(  "******************************************************************************" );
//       }
//       ATH_MSG_DEBUG( "Made it to point A"  );

//       //Loop over segms
//       for (Trk::SegmentCollection::const_iterator s = m_segms->begin();s != m_segms->end(); ++s) {
//         // Get segm
//         Muon::MuonSegment *segm=dynamic_cast<Muon::MuonSegment*>(*s);

//         if (segm == 0) {
//           ATH_MSG_ERROR( "no pointer to segm!!!" );
//           continue;
//         }

//         // Loop over segment ROTs
//         for(unsigned int i =0; i < segm->numberOfContainedROTs(); ++i) {
//           //const Trk::RIO_OnTrack* rio = segm->rioOnTrack(i);
//           //Identifier id = rio->identify();
//           ATH_MSG_DEBUG( "Looking at segment id" );

//           // Skip segment if there are no csc hits
//           if ( cscHits(segm)<1 ) continue;

//           // Get contained measurements
//           const std::vector<const Trk::MeasurementBase*> meas = segm->containedMeasurements();

//           // Skip segment if no measurements
//           if ( meas.size()<1 ) continue;

//           // Analyze segment if it is a csc segment
//           if ( /*m_idHelperTool->isCsc(id)*/ isCscSegment(segm) )
// 	    {

// 	      // Print out segment information
// 	      const Trk::FitQuality* fq = segm->fitQuality();
// 	      double chi2 = 999.;
// 	      int ndof = -1;
// 	      if( fq ) 
// 		{
// 		  chi2 = fq->chiSquared();
// 		  ndof = fq->numberDoF();
// 		  ATH_MSG_DEBUG( "Chi2 " << chi2 );
// 		  //chi2_histo->Fill(chi2);
// 		  ATH_MSG_DEBUG( "Ndof " << ndof );
// 		}

// 	      ATH_MSG_DEBUG( "R " << segm->globalPosition().perp() );
// 	      ATH_MSG_DEBUG( "Z " << segm->globalPosition().z() );
// 	      ATH_MSG_DEBUG( "Phi " << segm->globalPosition().phi() );
// 	      ATH_MSG_DEBUG( "Eta " << segm->globalPosition().eta() );
// 	      ATH_MSG_DEBUG( "Dir Phi " << segm->globalDirection().phi() );
// 	      ATH_MSG_DEBUG( "Dir Eta " << segm->globalDirection().eta() ); 

// 	      //ATH_MSG_DEBUG( "New Segment " << nsegm_Counter[j][(segm->globalDirection().eta()+1)/2] + 1 );
// 	      /*
// 		if(histos.layers || histos.mlayers) {    
// 		const MdtIdHelper *m_mdtIdHelper = m_muonMgr->mdtIdHelper();
// 		std::set<int> multiLayers;
// 		std::set<int> tubeLayers;
// 		const std::vector<const Trk::MeasurementBase*> measurements = segment.containedMeasurements();
// 		for(unsigned int i =0; i < measurements.size(); ++i) {
// 		const Trk::MeasurementBase* meas = measurements[i];
// 		Identifier measId = m_helperTool->getIdentifier(*meas);      
// 		if (m_mdtIdHelper->is_mdt(measId)) {
// 		multiLayers.insert(m_mdtIdHelper->multilayer(measId));
// 		tubeLayers.insert( (m_mdtIdHelper->multilayer(measId)-1)*4 + m_mdtIdHelper->tubeLayer(measId) );
// 		}
// 		}
// 		if( histos.layers )   ATH_MSG_INFO( "Layers " << histos.layers );
// 		if( histos.mlayers )  ATH_MSG_INFO( "mLayers " << histos.mlayers );
// 		}
// 	      */


// 	      // Get segm parameters
// 	      //const std::vector<const Trk::MeasurementBase*> meas = segm->containedMeasurements();
// 	      const Trk::AtaPlane* segPars = m_helperTool->createTrackParameters(*segm,10000.,0.); // segm,momentum,charge    
// 	      if( !segPars ) return false;

// 	      // ==============================================================================
// 	      // Field           Range               Notes
// 	      // ==============================================================================
// 	      // StationName     unsigned integer    maps to "CSS", "CSL", etc.
// 	      // StationEta      [-1,1]              -1 for backward, 1 for forward endcap
// 	      // StationPhi      [1,8]               increases with Phi
// 	      // Technology      [1]                 maps to "CSC"
// 	      // ChamberLayer    [1,2]               increases with |Z|
// 	      // WireLayer       [1,4]               increases with |Z|
// 	      // MeasuresPhi     [0,1]               0 if measures R, 1 if measures Phi
// 	      // Strip           [1,n]               increases with R   for MeasuresPhi=0
// 	      //                                     increases with Phi for MeasuresPhi=1
// 	      // ==============================================================================

// 	      //if (meas.size()>0)
// 	      //{

// 	      const Trk::MeasurementBase* rio = meas[0];
// 	      Identifier id = m_helperTool->getIdentifier(*rio);
// 	      std::string name;

// 	      // Chamber name stuff.  Take info from first hit
// 	      //int PhiStation=0, EtaStation=0;
// 	      int PhiStation=0;
// 	      //bool isInner=false, isMiddle=false, isOuter=false, isSideA=false;
// 	      //int chamberIndex=0;
// 	      int icscstationPhi=0, icscstationEta=0, icscEndcap=0;

// 	      icscstationPhi = int(m_cscIdHelper->stationPhi(id));
// 	      icscstationEta = int(m_cscIdHelper->stationEta(id));
// 	      icscEndcap = (icscstationEta+1)/2;
// 	      bool cscIsLarge = !( m_idHelperTool->isSmallChamber(id));
// 	      if (cscIsLarge) { name = "CSL_"; }
// 	      else { name = "CSS_"; }
// 	      if (icscstationEta>0) { name = name+"A"; }
// 	      else { name = name+"C"; }
// 	      if (cscIsLarge) { PhiStation = icscstationPhi*2-1; }
// 	      else { PhiStation = icscstationPhi*2; }
// 	      //EtaStation = ( icscstationEta>0 ? icscstationEta+14 : icscstationEta-14 );

// 	      // Get first and last hits on segment
// 	      std::vector< const Trk::MeasurementBase* >::const_iterator hit = segm->containedMeasurements().begin();
// 	      std::vector< const Trk::MeasurementBase* >::const_iterator hit_end = segm->containedMeasurements().end();

// 	      // Loop over hits
// 	      for( ;hit!=hit_end;hit++ ){
// 		Identifier clust_id;
// 		const Muon::CscClusterOnTrack* clust_rot = dynamic_cast<const Muon::CscClusterOnTrack*>(*hit);
// 		if( clust_rot ) {

// 		}
// 		else{

// 		}
// 	      }

// 	      // Number of clusters on segment
// 	      int n_clust=meas.size(); 

// 	      // Loop over all clusters on segment
// 	      for(int i = 0; i < n_clust; i++) {
// 		//Identifier clustId = m_helperTool->getIdentifier(meas[i]);
// 	      } // loop over clusters

// 	      //} // if meas.size()>0

// 	      // Count segment
// 	      ATH_MSG_DEBUG( " icscstationEta = " << icscstationEta << " PhiStation = " << PhiStation );
// 	      nsegm_Counter[j][icscEndcap]++;
// 	      ATH_MSG_DEBUG( " " << coll[j] << " collection endcap " << ecap[icscEndcap] << " nsegm counted" );
// 	      nsegm_sect_Counter[j][icscEndcap][PhiStation]++;
// 	      ATH_MSG_DEBUG( " " << coll[j] << " collection endcap " << ecap[icscEndcap] << " sector " << PhiStation << " nsegm counted" );

// 	      // Fill segment histos
// 	      // Fill histos with tool
// 	      b_status = m_TrackHistoTool->fill(histos_from_tool[j],*segm);

// 	      // Fill number of layers histo
// 	      //h_nclust_per_segm->Fill(n_clust);

// 	      // Fill profile histos      
// 	      //const Trk::TrackParameters* pars;  
// 	      //const Trk::ResidualPull* resPull;

// 	      /*
// 		for (unsigned int i=0; i<meas.size(); ++i) {
// 		const Trk::MeasurementBase* rio = meas[i];
// 		Identifier id = m_helperTool->getIdentifier(*rio);

// 		}
// 	      */


// 	      delete segPars; //Added this here because of compiler error
// 	    } // if is csc segment
//         } // loop over ROTs

// 	// delete segPars;
//       } // loop over segms

//       // Fill nsegm histos
//       for(int collection = 0; collection < 2; collection++){
//         for(int endcap = 0; endcap < 2; endcap++){
//           if(nsegm_Counter[collection][endcap]>0){
//             h_nsegm[collection][endcap]->Fill(nsegm_Counter[collection][endcap]);
//             ATH_MSG_DEBUG( " h_nsegm[" << collection << "][" << endcap << "] filled " << nsegm_Counter[collection][endcap] );
//             h_nsegm_oview[collection][endcap]->Fill(nsegm_Counter[collection][endcap]);
//             ATH_MSG_DEBUG( " h_nsegm_oview[" << collection << "][" << endcap << "] filled " << nsegm_Counter[collection][endcap] );
//           }
//           for(int sector = 0; sector < 16; sector++){
//             //ATH_MSG_DEBUG( " nsegm_sect[" << endcap << "][" << sector << "] = " << nsegm_sect_Counter[endcap][sector] );
//             if(nsegm_sect_Counter[collection][endcap][sector]>0){
//               h_nsegm_sect[collection][endcap][sector]->Fill(nsegm_sect_Counter[collection][endcap][sector]);
//               ATH_MSG_DEBUG( " h_nsegm_sect[" << collection << "]" << "[" << endcap << "][" << sector << "] filled " << nsegm_sect_Counter[collection][endcap][sector] );
//             }
//           }
//         }
//       }

//       ATH_MSG_DEBUG( "Number of Segments" << nsegm_Counter  );
//     } // loop over collections

//     ++m_eventCounter;
//     if (m_debuglevel)  ATH_MSG_DEBUG(  "Number of events : "<< m_eventCounter ); 

//   } // environment if

//   if (m_debuglevel)  ATH_MSG_DEBUG(  "CSCSegmValidation::Finalisation of " << name() << " was successful" );

//   return sc;
// }

// //
// //---------------------------------------------------------------------------------------
// //

// StatusCode CSCSegmValidation::procHistograms()
// {
//   if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

//     if (m_debuglevel)  ATH_MSG_DEBUG(  "MuonSegmMonitoring procHistograms()" );
//     if(endOfEventsBlock){}
//     if(endOfLumiBlock){}
//     if(endOfRun){

//     } // endOfRun
//   } // environment if

//   return StatusCode::SUCCESS;   
// }

// //
// //--------------------------------------------------------------------------------------- 
// //

// //=====================================================//
// /** finalize  */
// //=====================================================//


// StatusCode CSCSegmValidation::finalize()
// {
//   StatusCode sc = ManagedMonitorToolBase::finalize();
//   if(!sc.isSuccess()) return sc;

//   m_num_segms.clear();
//   m_num_hits_per_segmvsphi.clear();
//   m_num_hits_per_segm_vs_station.clear();
//   chi2_vs_station.clear();

//   m_segm_etavsphi.clear();
//   m_segm_eta_vs_phi.clear();
//   m_segm_eta_vs_phiInner.clear();
//   m_segm_eta_vs_phiMiddle.clear();
//   m_segm_eta_vs_phiOuter.clear();

//   histos_from_tool.clear();
//   return sc;

// }

// //
// //--------------------------------------------------------------------------------------- 
// //

// bool CSCSegmValidation::isCscSegment( const Muon::MuonSegment* seg ) const {
//   bool isCsc(false);

//   std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
//   for( unsigned int i = 0; i< mbs.size(); ++i){

//     const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (mbs[i]);
//     if (!rot){
//       const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[i]);
//       if (crot) rot  = &(crot->rioOnTrack(0));
//     }  
//     if( !rot ) {
//       continue;
//     }
//     if( m_cscIdHelper->is_csc( rot->identify() ) ) isCsc=true;
//   }

//   return isCsc;
// }

// //
// //--------------------------------------------------------------------------------------- 
// //


// unsigned int CSCSegmValidation::cscHits( const Muon::MuonSegment* seg ) const {
//   unsigned int nrHits(0);
//   if( !isCscSegment(seg) ) return nrHits;

//   std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
//   for( unsigned int i = 0; i< mbs.size(); ++i){

//     const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (mbs[i]);
//     if (!rot){
//       const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[i]);
//       if (crot) rot  = &(crot->rioOnTrack(0));
//     }  
//     if( !rot ) {
//       continue;
//     }
//     if( m_cscIdHelper->is_csc( rot->identify() ) ) ++nrHits;
//   }

//   return nrHits ;
// }

// //
// //--------------------------------------------------------------------------------------- 
// //

// /*
//   void CSCSegmValidation::deleteUnwantedHistograms( Muon::MuonDQATrackHistTool::TrackHistos histos_from_tool, std::string m_segmCollectionName, int j){

//   // numbers of segments/hits...
//   // Delete histos irrelevant for segments, that HistTool books automatically - hack...
//   if ( m_segmCollectionName[j] == "ConvertedMBoySegments" )
//   {
//   if ( histos_from_tool[j].hitHistos.countHistos.netaTrig ){
//   histos_from_tool[j].hitHistos.countHistos.netaTrig->Delete();
//   histos_from_tool[j].hitHistos.countHistos.netaTrig = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.netaTrigCh ){
//   histos_from_tool[j].hitHistos.countHistos.netaTrigCh->Delete();
//   histos_from_tool[j].hitHistos.countHistos.netaTrigCh = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.rpcs.neta ){
//   histos_from_tool[j].hitHistos.countHistos.rpcs.neta->Delete();
//   histos_from_tool[j].hitHistos.countHistos.rpcs.neta = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.rpcs.netaCh ){
//   histos_from_tool[j].hitHistos.countHistos.rpcs.netaCh->Delete();
//   histos_from_tool[j].hitHistos.countHistos.rpcs.netaCh = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.tgcs.neta ){
//   histos_from_tool[j].hitHistos.countHistos.tgcs.neta->Delete();
//   histos_from_tool[j].hitHistos.countHistos.tgcs.neta = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.tgcs.netaCh ){
//   histos_from_tool[j].hitHistos.countHistos.tgcs.netaCh->Delete();
//   histos_from_tool[j].hitHistos.countHistos.tgcs.netaCh = 0; 
//   }
//   }

//   // Segment fit + coordinates
//   // Delete histos irrelevant for segments, that HistTool books automatically - hack...
//   if ( histos_from_tool[j].pt ){
//   histos_from_tool[j].pt->Delete();
//   histos_from_tool[j].pt = 0; 
//   }
//   if ( histos_from_tool[j].momentum ){
//   histos_from_tool[j].momentum->Delete();
//   histos_from_tool[j].momentum = 0; 
//   }

//   // Hit Residuals + pulls
//   // Delete histos irrelevant for segments, that HistTool books automatically - hack...
//   ATH_MSG_DEBUG( "deleting histos for " << m_segmCollectionName[j] );

//   if ( histos_from_tool[j].hitHistos.residualHistos.all.error ){
//   histos_from_tool[j].hitHistos.residualHistos.all.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.all.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.all.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.all.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.all.lposX = 0;
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.all.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.all.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.all.lposY = 0; 
//   }

//   if ( histos_from_tool[j].hitHistos.residualHistos.mdts.resProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdts.resProf->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.mdts.resProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.resProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.resProf->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.resProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.resProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.resProf->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.resProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.mdtsECA.resProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdtsECA.resProf->Delete(); 
//   histos_from_tool[j].hitHistos.residualHistos.mdtsECA.resProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.mdtsECC.resProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdtsECC.resProf->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.mdtsECC.resProf = 0; 
//   }

//   if ( histos_from_tool[j].hitHistos.residualHistos.mdts.pullProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdts.pullProf->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.mdts.pullProf = 0;
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.pullProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.pullProf->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.pullProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.pullProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.pullProf->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.pullProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.mdtsECA.pullProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdtsECA.pullProf->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.mdtsECA.pullProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.mdtsECC.pullProf ){
//   histos_from_tool[j].hitHistos.residualHistos.mdtsECC.pullProf->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.mdtsECC.pullProf = 0; 
//   }

//   if ( histos_from_tool[j].hitHistos.countHistos.mdts.nmdtProf ){
//   histos_from_tool[j].hitHistos.countHistos.mdts.nmdtProf->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdts.nmdtProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nmdtProf ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nmdtProf->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nmdtProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nmdtProf ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nmdtProf->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nmdtProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsECA.nmdtProf ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsECA.nmdtProf->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsECA.nmdtProf = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsECC.nmdtProf ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsECC.nmdtProf->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsECC.nmdtProf = 0; 
//   }

//   if ( histos_from_tool[j].hitHistos.countHistos.mdts.nsegm ){
//   histos_from_tool[j].hitHistos.countHistos.mdts.nsegm->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdts.nsegm = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nsegm ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nsegm->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nsegm = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nsegm ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nsegm->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nsegm = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsECA.nsegm ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsECA.nsegm->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsECA.nsegm = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsECC.nsegm ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsECC.nsegm->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsECC.nsegm = 0; 
//   }

//   if ( histos_from_tool[j].hitHistos.countHistos.mdts.chi2Prof ){
//   histos_from_tool[j].hitHistos.countHistos.mdts.chi2Prof->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdts.chi2Prof = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.chi2Prof ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.chi2Prof->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.chi2Prof = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.chi2Prof ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.chi2Prof->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.chi2Prof = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsECA.chi2Prof ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsECA.chi2Prof->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsECA.chi2Prof = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.countHistos.mdtsECC.chi2Prof ){
//   histos_from_tool[j].hitHistos.countHistos.mdtsECC.chi2Prof->Delete();
//   histos_from_tool[j].hitHistos.countHistos.mdtsECC.chi2Prof = 0; 
//   }


//   if ( m_segmCollectionName[j] == "ConvertedMBoySegments" )
//   {
//   ATH_MSG_DEBUG( "deleting histos specifically for muonboy" );
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposY = 0; 
//   }

//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposY = 0; 
//   }

//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposY = 0; 
//   }

//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposY = 0; 
//   }

//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.error ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.error = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposX ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposX = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposY ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposY = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.res ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.res = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.pull ){
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.pull = 0; 
//   }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.error )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.error = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposX )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposX = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposY )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposY = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.res )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.res = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.pull )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.pull = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.error )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.error = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposX )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposX = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposY )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposY = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.res )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.res = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.pull )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.pull = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.error )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.error = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposX )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposX = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposY )
//   { histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposY = 0; }

//   if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.res )
//   { histos_from_tool[j].hitHistos.residualHistos.tgcsEta.res->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.tgcsEta.res = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.pull )
//   { histos_from_tool[j].hitHistos.residualHistos.tgcsEta.pull->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.tgcsEta.pull = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.error )
//   { histos_from_tool[j].hitHistos.residualHistos.tgcsEta.error->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.tgcsEta.error = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposX )
//   { histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposX->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposX = 0; }
//   if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposY )
//   { histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposY->Delete();
//   histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposY = 0; }
//   ATH_MSG_DEBUG( "deleted histos specifically for muonboy" );
//   }
//   }
// */
