/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/* ===========================================================================
   MuonSegmValidation.cxx
   ------------------------------

   AUTHORS:     I. Christidi
   Modified:    N. Benekos (Illinois) 
   created:     January 2008
   description: Implementation code for the MuonSegmValidation
   =========================================================================== */

// #include "MuonSegmMonitoring/MuonSegmValidation.h"
// #include "MuonRecHelperTools/MuonEDMPrinterTool.h"
// #include "MuonRecHelperTools/MuonEDMHelperTool.h"
// #include "MuonIdHelpers/MuonIdHelperTool.h"  
// #include "MuonDQAUtils/MuonChamberNameConverter.h"
// #include "MuonDQAUtils/MuonDQAFitFunc.h"

// // Track
// #include "TrkExInterfaces/IPropagator.h"
// #include "TrkToolInterfaces/IResidualPullCalculator.h"

// #include "EventPrimitives/EventPrimitives.h"
// #include "TrkEventPrimitives/FitQualityOnSurface.h"
// #include "TrkEventPrimitives/FitQuality.h"
// #include "TrkMeasurementBase/MeasurementBase.h"
// #include "TrkSurfaces/BoundaryCheck.h"

// #include "TrkParameters/TrackParameters.h"
// #include "TrkGeometry/MagneticFieldProperties.h"
 
// #include "TrkTrack/TrackCollection.h"
// #include "TrkEventPrimitives/ResidualPull.h"
// #include "TrkEventPrimitives/PropDirection.h"

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


// const int s_maxEvents     = 100000;
// const int s_maxComponents = 50;
// const int s_maxSoS        = 80;

// // *********************************************************************
// // Public Methods
// // *********************************************************************

// MuonSegmValidation::MuonSegmValidation( const std::string & type, const std::string & name, const IInterface* parent )
//   :ManagedMonitorToolBase( type, name, parent ), 
//    m_storeGate(0),
//    m_activeStore(NULL),
//    m_muonMgr(NULL),
//    m_mdtIdHelper(NULL),
//    m_rpcIdHelper(NULL),
//    m_cscIdHelper(NULL),
//    m_tgcIdHelper(NULL),
//    m_segms(NULL),
//    m_eventCounter(0),
//    m_eventNumber(-1),
//    m_rec_nSoS(-1),
//    m_debuglevel(false),
//    b_status(false)
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
// //---------------------------------------------------------------------------------------
// MuonSegmValidation::~MuonSegmValidation()
// {
//   ATH_MSG_INFO( " deleting MuonSegmValidation " );
// }


// //==================================================================//
// /**   Initialize  */
// //==================================================================//

// StatusCode MuonSegmValidation::initialize()
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

//   // Retrieve the subsystem helpers
//   sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL( "Cannot get MdtIdHelper" );
//     return sc;
//   }  
//   if (m_debuglevel) ATH_MSG_DEBUG( " Found the MdtIdHelper " );
 
//   sc = detStore->retrieve(m_cscIdHelper,"CSCIDHELPER");
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL( "Cannot get CscIdHelper" );
//     return sc;
//   }  
//   if (m_debuglevel) ATH_MSG_DEBUG( " Found the CscIdHelper " );
 
//   sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
//   if (sc.isFailure()) {
//     ATH_MSG_FATAL( "Can't retrieve RpcIdHelper" );
//     return sc;
//   }	 
//   if (m_debuglevel) ATH_MSG_DEBUG( " Found the RpcIdHelper " );
 
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

//   return sc;
// }  
  
// //---------------------------------------------------------------------------------------
// //StatusCode MuonSegmValidation::bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
// StatusCode MuonSegmValidation::bookHistogramsRecurrent()
// {
//   if (m_debuglevel) {
//     ATH_MSG_DEBUG( "MuonSegmValidation::bookHistograms() to setup tools/services" );
//   }
//   StatusCode sc = StatusCode::SUCCESS;

//   if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
    
//     histos_from_tool.push_back(histos_from_tool_Moore);
//     histos_from_tool.push_back(histos_from_tool_MBoy);
//     ATH_MSG_DEBUG( "Filled vector with structures histos_from_tool" );
    
//     for (int j=0;j<(int)m_segmCollectionName.size();j++) {
//       if ( m_segmCollectionFlag[j] == 0 ) continue;
      
//       std::string m_path_muonsegm = "Muon/MuonSegmentMonitoring";

//       MgmtAttr_t attr = ATTRIB_UNMANAGED;
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

// 	  b_status = m_TrackHistoTool->book(histos_from_tool[j],prefix,m_doMDT,m_doRPC,m_doTGC,m_doCSC);
// 	  ATH_MSG_INFO( "Booked histos for " << m_segmCollectionName[j] << " with histTool" );

// 	  // Overview histos
// 	  // Pull
// 	  mdt2_pulls_BarrelA.push_back(m_histoTool->makeAndRegisterTH2F(al_muonsegms_shift, (prefix+"mdt2_pull_BA").c_str(),"MDT 2D hit pull vs IMO-phi sector, Barrel A",48,1.,49.,100,-10.,10.));   ;
// 	  mdt_pulls_BarrelA.push_back(m_histoTool->makeAndRegisterTH1F(al_mBAsegms_shift,(prefix+"mdt_pull_BA").c_str(),"MDT hit pull vs IMO-phi sector, Barrel A",48,1.,49.));
// 	  mdt_pulls_BarrelA.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
 
// 	  mdt2_pulls_BarrelC.push_back(m_histoTool->makeAndRegisterTH2F(al_muonsegms_shift,(prefix+"mdt2_pull_BC").c_str(),"MDT 2D hit pull vs IMO-phi sector, Barrel C",48,1.,49.,100,-10.,10.));
// 	  mdt_pulls_BarrelC.push_back(m_histoTool->makeAndRegisterTH1F(al_mBCsegms_shift,(prefix+"mdt_pull_BC").c_str(),"MDT hit pull vs IMO-phi sector, Barrel C",48,1.,49.));
// 	  mdt_pulls_BarrelC.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
	   
// 	  mdt2_pulls_ECA.push_back(m_histoTool->makeAndRegisterTH2F(al_muonsegms_shift,(prefix+"mdt2_pull_ECA").c_str(),"MDT 2D hit pull vs IMO-phi sector, EC A",48,1.,49.,100,-10.,10.));
// 	  mdt_pulls_ECA.push_back(m_histoTool->makeAndRegisterTH1F(al_mEAsegms_shift,(prefix+"mdt_pull_ECA").c_str(),"MDT hit pull vs IMO-phi sector, EC A",48,1.,49.));
// 	  mdt_pulls_ECA.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
 	  
// 	  mdt2_pulls_ECC.push_back(m_histoTool->makeAndRegisterTH2F(al_muonsegms_shift,(prefix+"mdt2_pull_ECC").c_str(),"MDT 2D hit pull vs IMO-phi sector, EC C",48,1.,49.,100,-10.,10.));
// 	  mdt_pulls_ECC.push_back(m_histoTool->makeAndRegisterTH1F(al_mECsegms_shift,(prefix+"mdt_pull_ECC").c_str(),"MDT hit pull vs IMO-phi sector, EC C",48,1.,49.));
// 	  mdt_pulls_ECC.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
 
// 	  // Nhits/segm
// 	  mdt_hits_BarrelA.push_back(new TProfile((prefix+"mdt_hits_BA").c_str(),"MDT hits vs IMO-phi sector, Barrel A",48,1.,49.,0.,90.));
// 	  mdt_hits_BarrelA.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
// 	  sc=al_mBAsegms_shift.regHist(mdt_hits_BarrelA.back());
// 	  mdt_hits_BarrelC.push_back(new TProfile((prefix+"mdt_hits_BC").c_str(),"MDT hits vs IMO-phi sector, Barrel C",48,1.,49.,0.,90.));
// 	  mdt_hits_BarrelC.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
// 	  sc=al_mBCsegms_shift.regHist(mdt_hits_BarrelC.back());
// 	  mdt_hits_ECA.push_back(new TProfile((prefix+"mdt_hits_ECA").c_str(),"MDT hits vs IMO-phi sector, EC A",48,1.,49.,0.,90.));
// 	  mdt_hits_ECA.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
// 	  sc=al_mEAsegms_shift.regHist(mdt_hits_ECA.back());
// 	  mdt_hits_ECC.push_back(new TProfile((prefix+"mdt_hits_ECC").c_str(),"MDT hits vs IMO-phi sector, EC C",48,1.,49.,0.,90.));
// 	  mdt_hits_ECC.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
// 	  sc=al_mECsegms_shift.regHist(mdt_hits_ECC.back());
	 
// 	  // chi2/ndof
// 	  chi2_BarrelA.push_back(new TProfile((prefix+"chi2_BA").c_str(),"Chi2/ndof vs IMO-phi sector, Barrel A",48,1.,49.,0.,3000.));
// 	  chi2_BarrelA.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
// 	  sc=al_mBAsegms_shift.regHist(chi2_BarrelA.back());
// 	  chi2_BarrelC.push_back(new TProfile((prefix+"chi2_BC").c_str(),"Chi2/ndof vs IMO-phi sector, Barrel C",48,1.,49.,0.,3000.));
// 	  chi2_BarrelC.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
// 	  sc=al_mBCsegms_shift.regHist(chi2_BarrelC.back());
// 	  chi2_ECA.push_back(new TProfile((prefix+"chi2_ECA").c_str(),"Chi2/ndof vs IMO-phi sector, EC A",48,1.,49.,0.,3000.));
// 	  chi2_ECA.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
// 	  sc=al_mEAsegms_shift.regHist(chi2_ECA.back());
// 	  chi2_ECC.push_back(new TProfile((prefix+"chi2_ECC").c_str(),"Chi2/ndof vs IMO-phi sector, EC C",48,1.,49.,0.,3000.));
// 	  chi2_ECC.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
// 	  sc=al_mECsegms_shift.regHist(chi2_ECC.back());
	 
// 	  // Nsegm
// 	  nsegm_BarrelA.push_back(m_histoTool->makeAndRegisterTH1F(al_mBAsegms_shift,(prefix+"nsegm_BA").c_str(),"Number of segments vs IMO-phi sector, Barrel A",48,1.,49.));
// 	  nsegm_BarrelA.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
 
// 	  nsegm_BarrelC.push_back(m_histoTool->makeAndRegisterTH1F(al_mBCsegms_shift,(prefix+"nsegm_BC").c_str(),"Number of segments vs IMO-phi sector, Barrel C",48,1.,49.));
// 	  nsegm_BarrelC.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
	
// 	  nsegm_ECA.push_back(m_histoTool->makeAndRegisterTH1F(al_mEAsegms_shift,(prefix+"nsegm_ECA").c_str(),"Number of segments vs IMO-phi sector, EC A",48,1.,49.));
// 	  nsegm_ECA.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
 
// 	  nsegm_ECC.push_back(m_histoTool->makeAndRegisterTH1F(al_mECsegms_shift,(prefix+"nsegm_ECC").c_str(),"Number of segments vs IMO-phi sector, EC C",48,1.,49.));
// 	  nsegm_ECC.back()->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer");
 
// 	  ATH_MSG_DEBUG( "Booked Overview histos for " << m_segmCollectionName[j] );

// 	  // numbers of segments/hits...
// 	  // Delete histos irrelevant for segments, that HistTool books automatically - hack...
// 	  if ( m_segmCollectionName[j] == "ConvertedMBoySegments" )
// 	    {
// 	      if ( histos_from_tool[j].hitHistos.countHistos.netaTrig )
// 		{ histos_from_tool[j].hitHistos.countHistos.netaTrig->Delete();
// 		  histos_from_tool[j].hitHistos.countHistos.netaTrig = 0; }
// 	      if ( histos_from_tool[j].hitHistos.countHistos.netaTrigCh )
// 		{ histos_from_tool[j].hitHistos.countHistos.netaTrigCh->Delete();
// 		  histos_from_tool[j].hitHistos.countHistos.netaTrigCh = 0; }

// 	      if ( histos_from_tool[j].hitHistos.countHistos.rpcs.neta )
// 		{ histos_from_tool[j].hitHistos.countHistos.rpcs.neta->Delete();
// 		  histos_from_tool[j].hitHistos.countHistos.rpcs.neta = 0; }
// 	      if ( histos_from_tool[j].hitHistos.countHistos.rpcs.netaCh )
// 		{ histos_from_tool[j].hitHistos.countHistos.rpcs.netaCh->Delete();
// 		  histos_from_tool[j].hitHistos.countHistos.rpcs.netaCh = 0; }
// 	      if ( histos_from_tool[j].hitHistos.countHistos.tgcs.neta )
// 		{ histos_from_tool[j].hitHistos.countHistos.tgcs.neta->Delete();
// 		  histos_from_tool[j].hitHistos.countHistos.tgcs.neta = 0; }
// 	      if ( histos_from_tool[j].hitHistos.countHistos.tgcs.netaCh )
// 		{ histos_from_tool[j].hitHistos.countHistos.tgcs.netaCh->Delete();
// 		  histos_from_tool[j].hitHistos.countHistos.tgcs.netaCh = 0; }
// 	    }
	  
// 	  m_num_segms.push_back(new TH1F((prefix+"n_segms").c_str(),"Number of segments per event", 30,-0.,30.));
// 	  sc=al_muonsegms_shift.regHist(m_num_segms.back());                                                       
	
// 	  m_num_hits_per_segmvsphi.push_back(new TH2F((prefix+"n_hits_per_segm_vs_phi").c_str(),"Number of hits per segment vs phi", 40,-4.,4., 50,0.,50.));
// 	  sc=al_muonsegms_debug.regHist(m_num_hits_per_segmvsphi.back());                  
	  
// 	  m_num_hits_per_segm_vs_station.push_back(new TH2F((prefix+"n_hits_per_segm_vs_station").c_str(),"Number of hits per segment vs station",32,0.,32.,50,0.,50.));
// 	  sc=al_muonsegms_debug.regHist(m_num_hits_per_segm_vs_station.back());                  

// 	  ATH_MSG_DEBUG( "Booked number of hit histos for " << m_segmCollectionName[j] );

// 	  // Segment fit + coordinates
// 	  // Delete histos irrelevant for segments, that HistTool books automatically - hack...
// 	  if ( histos_from_tool[j].pt )
// 	    { histos_from_tool[j].pt->Delete();
// 	      histos_from_tool[j].pt = 0; }
// 	  if ( histos_from_tool[j].momentum )
// 	    { histos_from_tool[j].momentum->Delete();
// 	      histos_from_tool[j].momentum = 0; }
	  
// 	  chi2_vs_station.push_back(new TH2F((prefix+"chi2_vs_station").c_str(),"Chi2 vs station",32,0.,32.,600,0.,300.));
// 	  sc=al_muonsegms_debug.regHist(chi2_vs_station.back());

// 	  m_segm_etavsphi.push_back(new TH2F((prefix+"segm_eta_vs_phi").c_str(),"Segment eta vs phi", 40,-4.,4., 40,-4.,4.));
// 	  sc=al_muonsegms_debug.regHist(m_segm_etavsphi.back());                  
 
// 	  m_segm_eta_vs_phi.push_back(new TH2F((prefix+"segm_station_eta_vs_phi").c_str(),"Segment station eta vs phi", 16,1.,17.,31,-15.,15.));
// 	  sc=al_muonsegms_debug.regHist(m_segm_eta_vs_phi.back());                   
// 	  m_segm_eta_vs_phiInner.push_back(new TH2F((prefix+"segm_station_eta_vs_phiInner").c_str(),"Segment station eta vs phi Inner", 16,1.,17.,31,-15.,15.));
// 	  sc=al_muonsegms_shift.regHist(m_segm_eta_vs_phiInner.back());
// 	  m_segm_eta_vs_phiMiddle.push_back(new TH2F((prefix+"segm_station_eta_vs_phiMiddle").c_str(),"Segment station eta vs phi Middle", 16,1.,17.,31,-15.,15.));
// 	  sc=al_muonsegms_shift.regHist(m_segm_eta_vs_phiMiddle.back());
// 	  m_segm_eta_vs_phiOuter.push_back(new TH2F((prefix+"segm_station_eta_vs_phiOuter").c_str(),"Segment station eta vs phi Outer", 16,1.,17.,31,-15.,15.));
// 	  sc=al_muonsegms_shift.regHist(m_segm_eta_vs_phiOuter.back());

	  
// 	  /*	  m_Miss_segm_eta_vs_phi.push_back(new TH2F("Miss_segm_station_eta_vs_phi","Missed segment station eta vs phi", 16,1.,17.,31,-15.,15.));
// 		  sc=al_muonsegms_shift.regHist(m_Miss_segm_eta_vs_phi.back());
// 		  m_Miss_segm_eta_vs_phiInner.push_back(new TH2F("Miss_segm_station_eta_vs_phiInner","Missed segment station eta vs phi Inner", 16,1.,17.,31,-15.,15.));
// 		  sc=al_muonsegms_shift.regHist(m_Miss_segm_eta_vs_phiInner.back());
// 		  m_Miss_segm_eta_vs_phiMiddle.push_back(new TH2F("Miss_segm_station_eta_vs_phiMiddle","Missed segment station eta vs phi Middle", 16,1.,17.,31,-15.,15.));
// 		  sc=al_muonsegms_shift.regHist(m_Miss_segm_eta_vs_phiMiddle.back());
// 		  m_Miss_segm_eta_vs_phiOuter.push_back(new TH2F("Miss_segm_station_eta_vs_phiOuter","Missed segment station eta vs phi Outer", 16,1.,17.,31,-15.,15.));
// 		  sc=al_muonsegms_shift.regHist(m_Miss_segm_eta_vs_phiOuter.back());

// 		  m_Expect_segm_eta_vs_phi.push_back(new TH2F("Expect_segm_station_eta_vs_phi","Expected segment station eta vs phi", 16,1.,17.,31,-15.,15.));
// 		  sc=al_muonsegms_shift.regHist(m_Expect_segm_eta_vs_phi.back());
// 		  m_Expect_segm_eta_vs_phiInner.push_back(new TH2F("Expect_segm_station_eta_vs_phiInner","Expected segment station eta vs phi Inner", 16,1.,17.,31,-15.,15.));
// 		  sc=al_muonsegms_shift.regHist(m_Expect_segm_eta_vs_phiInner.back());
// 		  m_Expect_segm_eta_vs_phiMiddle.push_back(new TH2F("Expect_segm_station_eta_vs_phiMiddle","Expected segment station eta vs phi Middle", 16,1.,17.,31,-15.,15.));
// 		  sc=al_muonsegms_shift.regHist(m_Expect_segm_eta_vs_phiMiddle.back());
// 		  m_Expect_segm_eta_vs_phiOuter.push_back(new TH2F("Expect_segm_station_eta_vs_phiOuter","Expected segment station eta vs phi Outer", 16,1.,17.,31,-15.,15.));
// 		  sc=al_muonsegms_shift.regHist(m_Expect_segm_eta_vs_phiOuter.back()); */

// 	  ATH_MSG_DEBUG( "Booked segment fit and coordinate histos for " << m_segmCollectionName[j] );

// 	  // Hit Residuals + pulls
// 	  // Delete histos irrelevant for segments, that HistTool books automatically - hack...
// 	  ATH_MSG_DEBUG( "deleting histos for " << m_segmCollectionName[j] );
	  
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.all.error )
// 	    { histos_from_tool[j].hitHistos.residualHistos.all.error->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.all.error = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.all.lposX )
// 	    { histos_from_tool[j].hitHistos.residualHistos.all.lposX->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.all.lposX = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.all.lposY )
// 	    { histos_from_tool[j].hitHistos.residualHistos.all.lposY->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.all.lposY = 0; }

// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdts.resProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdts.resProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdts.resProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.resProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.resProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.resProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.resProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.resProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.resProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdtsECA.resProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdtsECA.resProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdtsECA.resProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdtsECC.resProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdtsECC.resProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdtsECC.resProf = 0; }

// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdts.pullProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdts.pullProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdts.pullProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.pullProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.pullProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelA.pullProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.pullProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.pullProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdtsBarrelC.pullProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdtsECA.pullProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdtsECA.pullProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdtsECA.pullProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.residualHistos.mdtsECC.pullProf )
// 	    { histos_from_tool[j].hitHistos.residualHistos.mdtsECC.pullProf->Delete();
// 	      histos_from_tool[j].hitHistos.residualHistos.mdtsECC.pullProf = 0; }

// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdts.nmdtProf )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdts.nmdtProf->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdts.nmdtProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nmdtProf )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nmdtProf->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nmdtProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nmdtProf )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nmdtProf->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nmdtProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsECA.nmdtProf )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsECA.nmdtProf->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsECA.nmdtProf = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsECC.nmdtProf )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsECC.nmdtProf->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsECC.nmdtProf = 0; }

// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdts.nsegm )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdts.nsegm->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdts.nsegm = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nsegm )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nsegm->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.nsegm = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nsegm )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nsegm->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.nsegm = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsECA.nsegm )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsECA.nsegm->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsECA.nsegm = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsECC.nsegm )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsECC.nsegm->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsECC.nsegm = 0; }

// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdts.chi2Prof )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdts.chi2Prof->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdts.chi2Prof = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.chi2Prof )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.chi2Prof->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsBarrelA.chi2Prof = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.chi2Prof )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.chi2Prof->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsBarrelC.chi2Prof = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsECA.chi2Prof )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsECA.chi2Prof->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsECA.chi2Prof = 0; }
// 	  if ( histos_from_tool[j].hitHistos.countHistos.mdtsECC.chi2Prof )
// 	    { histos_from_tool[j].hitHistos.countHistos.mdtsECC.chi2Prof->Delete();
// 	      histos_from_tool[j].hitHistos.countHistos.mdtsECC.chi2Prof = 0; }


// 	  if ( m_segmCollectionName[j] == "ConvertedMBoySegments" )
// 	    {
// 	      ATH_MSG_DEBUG( "deleting histos specifically for muonboy" );
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEta.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEta.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEta.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEta.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEta.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEta.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEta.lposY = 0; }

// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA.lposY = 0; }

// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC.lposY = 0; }

// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA1.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA2.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA3.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA4.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA5.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA6.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA7.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaA8.lposY = 0; }

// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC1.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC2.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC3.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC4.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC5.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC6.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC7.lposY = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.rpcsEtaC8.lposY = 0; }

// 	      if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.res )
// 		{ histos_from_tool[j].hitHistos.residualHistos.tgcsEta.res->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.tgcsEta.res = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.pull )
// 		{ histos_from_tool[j].hitHistos.residualHistos.tgcsEta.pull->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.tgcsEta.pull = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.error )
// 		{ histos_from_tool[j].hitHistos.residualHistos.tgcsEta.error->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.tgcsEta.error = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposX )
// 		{ histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposX->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposX = 0; }
// 	      if ( histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposY )
// 		{ histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposY->Delete();
// 		  histos_from_tool[j].hitHistos.residualHistos.tgcsEta.lposY = 0; }
// 	      ATH_MSG_DEBUG( "deleted histos specifically for muonboy" );
// 	    }

	 

// 	  //histos_from_tool[j].hitHistos.residualHistos.mdts.error->Delete();
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.cscsEta.error->Delete();
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.cscsPhi.error->Delete();
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.rpcsEta.error->Delete();
		
// 	  //histos_from_tool[j].hitHistos.residualHistos.rpcsPhi.error->Delete();
		
// 	  //histos_from_tool[j].hitHistos.residualHistos.tgcsEta.error->Delete();
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.tgcsPhi.error->Delete(); 
	  	
// 	  //histos_from_tool[j].hitHistos.residualHistos.mdts.error = 0;
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.cscsEta.error = 0;
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.cscsPhi.error = 0;
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.rpcsEta.error = 0;
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.rpcsPhi.error = 0;
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.tgcsEta.error = 0;
	  
// 	  //histos_from_tool[j].hitHistos.residualHistos.tgcsPhi.error = 0; 
		
		
// 	  if (m_debuglevel)  ATH_MSG_DEBUG( "Booked residual histos for " << m_segmCollectionName[j] );
	  
// 	  //	  b_status = m_TrackHistoTool->registerHist(al_muonsegms_shift,al_muonsegms_debug,al_muonSelBsegms_debug,al_muonSelEAsegms_debug,al_muonSelECsegms_debug,histos_from_tool[j]);
// 	  b_status = m_TrackHistoTool->registerHist(al_muonsegms_shift,al_muonsegms_debug,al_mBAsegms_debug,al_mBCsegms_debug,al_mEsegms_debug,al_mEAsegms_debug,al_mECsegms_debug,histos_from_tool[j]);
// 	  if (m_debuglevel)  ATH_MSG_DEBUG("Finished booking histos for " << m_segmCollectionName[j] );
	  
	  
// 	} // if NewRun

      
//     } // loop over Muon Segm collections

//   } // environment if  
//   return sc;
  
// }
// //---------------------------------------------------------------------------------------
// StatusCode MuonSegmValidation::fillHistograms()
// {
//   StatusCode sc = StatusCode::SUCCESS;
//   ATH_MSG_DEBUG(  "******************************************************************************" );
//   ATH_MSG_DEBUG( "                        INSIDE fillHistograms()                             " );
//   ATH_MSG_DEBUG(  "******************************************************************************" );

//   if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

//     // Set the event counter
//     m_eventNumber = m_eventCounter;
  
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
// 	if ( (j == (int)m_segmCollectionName.size()-1) && !got_coll ){
// 	  ATH_MSG_DEBUG( "Unable to retrieve reconstructed segms from any collection... Exiting!" );
// 	  return StatusCode::FAILURE; }
// 	else {
// 	  ATH_MSG_DEBUG( "Unable to retrieve reconstructed segms from collection... Trying next..." );
	
// 	  continue; }
//       }else{
// 	if (m_debuglevel){ ATH_MSG_DEBUG( "Segms in StoreGate found");
// 	  ATH_MSG_DEBUG( "Segm collection Name is " << m_segmCollectionName[j] );}
// 	got_coll = true;
//       }
    
//       if ( m_segms->empty() ){
// 	if (m_debuglevel){
// 	  ATH_MSG_DEBUG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
// 	  ATH_MSG_DEBUG( "      Segm Collection is Empty.  Trying next...    ");
// 	  ATH_MSG_DEBUG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
// 	}
// 	continue;
//       }
    
//       if (m_num_segms[h_index]) m_num_segms[h_index]->Fill(m_segms->size());
//       if (m_debuglevel){
// 	ATH_MSG_DEBUG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
// 	ATH_MSG_DEBUG( "      Segm Collection size         " );
// 	ATH_MSG_DEBUG( "Number of segms found: " << m_segms->size() );
// 	ATH_MSG_DEBUG( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
//       }
    
//       if ( m_segms->size() > 0 ){
// 	ATH_MSG_DEBUG(  "********************************* WARNING ************************************" );
// 	ATH_MSG_DEBUG(   "Number of segms in event is: " << m_segms->size()                               );
// 	ATH_MSG_DEBUG(  "This algorithm is designed to operate for single segm / event only"             );
// 	ATH_MSG_DEBUG(  "Processing only the first segm in the segm collection"                         );
// 	ATH_MSG_DEBUG(  "******************************************************************************" );
//       }
 
  
//       //Loop over segms
//       for (Trk::SegmentCollection::const_iterator s = m_segms->begin();s != m_segms->end(); ++s) {
// 	// Get segm
// 	Muon::MuonSegment *segm=dynamic_cast<Muon::MuonSegment*>(*s);
// 	if (segm == 0) {
// 	  ATH_MSG_ERROR( "no pointer to segm!!!" );
// 	  break;
// 	}

// 	// check technology and skip segment if wanted
// 	bool skipSegm = false;
// 	for(unsigned int i =0; i < segm->numberOfContainedROTs(); ++i) {
// 	  const Trk::RIO_OnTrack* rio = segm->rioOnTrack(i);
// 	  Identifier id = rio->identify();
// 	  if ( (!m_doMDT&&m_idHelperTool->isMdt(id)) || (!m_doRPC&&m_idHelperTool->isRpc(id)) || (!m_doTGC&&m_idHelperTool->isTgc(id)) || (!m_doCSC&&m_idHelperTool->isCsc(id)) ) 
// 	    {
// 	      skipSegm = true;
// 	      break;
// 	    }
// 	}
// 	if (skipSegm) continue;

// 	// Fill histos with tool
// 	b_status = m_TrackHistoTool->fill(histos_from_tool[j],*segm);

// 	// Get segm parameters
// 	const std::vector<const Trk::MeasurementBase*> meas = segm->containedMeasurements();
// 	const Trk::AtaPlane* segPars = m_helperTool->createTrackParameters(*segm,10000.,0.); // segm,momentum,charge    
// 	if( !segPars ) return false;
// 	const Trk::FitQuality* fq = segm->fitQuality();
// 	ATH_MSG_DEBUG(  "MuonSegment chi2, ndof " << (float)segm->fitQuality()->chiSquared() 
// 			<< "/" <<  (int)segm->fitQuality()->numberDoF() );


// 	const Amg::Vector3D& globalPos = segm->globalPosition();
// 	//const Amg::Vector3D  direction = segm->globalDirection();
// 	if (m_num_hits_per_segmvsphi[h_index]) m_num_hits_per_segmvsphi[h_index]->Fill(globalPos.phi(),(float)meas.size());        	
	
// 	// Chamber name stuff. Take info from first hit.
// 	int PhiStation=0, EtaStation=0;
// 	bool isInner=false, isMiddle=false, isOuter=false, isSideA=false;
// 	int chamberIndex=0;

// 	if (meas.size()>0)
// 	  {
// 	    const Trk::MeasurementBase* rio = meas[0];
// 	    Identifier id = m_helperTool->getIdentifier(*rio);
// 	    std::string name;
// 	    if (m_idHelperTool->isMdt(id))
// 	      {
// 		int imdtstationPhi = int(m_mdtIdHelper->stationPhi(id));
// 		int imdtstationEta = int(m_mdtIdHelper->stationEta(id));
// 		int imdtstationName = int(m_mdtIdHelper->stationName(id));
// 		bool mdtIsLarge = !( m_idHelperTool->isSmallChamber(id));
// 		std::string type="MDT";
// 		std::string hardware_name = convertChamberName(imdtstationName,imdtstationEta,imdtstationPhi,type);
// 		if ( strlen(hardware_name.c_str())<3 ) continue;

// 		isInner = hardware_name[1]=='I';
// 		isMiddle = hardware_name[1]=='M';
// 		isOuter = hardware_name[1]=='O';
// 		isSideA = imdtstationEta>0;
// 		if (!m_idHelperTool->isEndcap(id)) { name = "Barrel_"; }
// 		else { name = "EC_";}
// 		if (imdtstationEta>0) { name = name+"A_"; }
// 		else { name = name+"C_"; }
// 		if (hardware_name[2]!='S' && hardware_name[2]!='L') { name = name+"Special"; }
// 		else {
// 		  if (isInner) { name = name+"Inner_"; }
// 		  else if (isMiddle) { name = name+"Middle_"; }
// 		  else if (isOuter) { name = name+"Outer_"; }
// 		  if (hardware_name[2]=='S') { name = name+"Short"; }
// 		  else if (hardware_name[2]=='L') { name = name+"Long"; }
// 		}
// 		if (mdtIsLarge) { PhiStation = imdtstationPhi*2-1; }
// 		else { PhiStation = imdtstationPhi*2; }
// 		if (!(m_mdtIdHelper->isEndcap(id))) { EtaStation = imdtstationEta; }
// 		else { EtaStation = ( imdtstationEta>0 ? imdtstationEta+8 : imdtstationEta-8 ); }
// 		// Fill overview histos
// 		if (isInner) chamberIndex=abs(PhiStation);
// 		if (isMiddle) chamberIndex=16+abs(PhiStation);
// 		if (isOuter) chamberIndex=32+abs(PhiStation);
// 		if (!m_idHelperTool->isEndcap(id)) // Barrel
// 		  {
// 		    if (isSideA) // side A
// 		      {
// 			nsegm_BarrelA[h_index]->Fill((float)chamberIndex);
// 			mdt_hits_BarrelA[h_index]->Fill((float)chamberIndex,(float)meas.size());
// 			chi2_BarrelA[h_index]->Fill((float)chamberIndex,fq->chiSquared());
// 		      }
// 		    else
// 		      {
// 			nsegm_BarrelC[h_index]->Fill((float)chamberIndex);
// 			mdt_hits_BarrelC[h_index]->Fill((float)chamberIndex,(float)meas.size());
// 			chi2_BarrelC[h_index]->Fill((float)chamberIndex,fq->chiSquared());
// 		      }
// 		  }
// 		else // EC
// 		  {
// 		    if (isSideA) // side A
// 		      {
// 			nsegm_ECA[h_index]->Fill((float)chamberIndex);
// 			mdt_hits_ECA[h_index]->Fill((float)chamberIndex,(float)meas.size());
// 			chi2_ECA[h_index]->Fill((float)chamberIndex,fq->chiSquared());
// 		      }
// 		    else
// 		      {
// 			nsegm_ECC[h_index]->Fill((float)chamberIndex);
// 			mdt_hits_ECC[h_index]->Fill((float)chamberIndex,(float)meas.size());
// 			chi2_ECC[h_index]->Fill((float)chamberIndex,fq->chiSquared());
// 		      }
// 		  }
// 	      }
// 	    else if (m_idHelperTool->isCsc(id))
// 	      {
// 		int icscstationPhi = int(m_cscIdHelper->stationPhi(id));
// 		int icscstationEta = int(m_cscIdHelper->stationEta(id));
// 		bool cscIsLarge = !( m_idHelperTool->isSmallChamber(id));
// 		if (cscIsLarge) { name = "BigWheel_"; }
// 		else { name = "SmallWheel_"; }
// 		if (icscstationEta>0) { name = name+"A"; }
// 		else { name = name+"C"; }
// 		if (cscIsLarge) { PhiStation = icscstationPhi*2-1; }
// 		else { PhiStation = icscstationPhi*2; }
// 		EtaStation = ( icscstationEta>0 ? icscstationEta+14 : icscstationEta-14 );
// 	      }
	  
// 	    if (m_num_hits_per_segm_vs_station[h_index]) m_num_hits_per_segm_vs_station[h_index]->Fill(name.c_str(),(float)meas.size(),1);	  
// 	    if (m_segm_eta_vs_phi[h_index]) m_segm_eta_vs_phi[h_index]->Fill((float)PhiStation,(float)EtaStation);
// 	    if (isInner) if (m_segm_eta_vs_phiInner[h_index]) m_segm_eta_vs_phiInner[h_index]->Fill((float)PhiStation,(float)EtaStation);
// 	    if (isMiddle) if (m_segm_eta_vs_phiMiddle[h_index]) m_segm_eta_vs_phiMiddle[h_index]->Fill((float)PhiStation,(float)EtaStation);
// 	    if (isOuter) if (m_segm_eta_vs_phiOuter[h_index]) m_segm_eta_vs_phiOuter[h_index]->Fill((float)PhiStation,(float)EtaStation);
	  
// 	    if (chi2_vs_station[h_index]) chi2_vs_station[h_index]->Fill(name.c_str(),fq->chiSquared(),1);
// 	  }
// 	if (m_segm_etavsphi[h_index]) m_segm_etavsphi[h_index]->Fill(globalPos.phi(),globalPos.eta());
	
// 	// Fill profile histos
// 	const Trk::TrackParameters* pars;  
// 	const Trk::ResidualPull* resPull;  
	
// 	for (unsigned int i=0; i<meas.size(); ++i) {
// 	  const Trk::MeasurementBase* rio = meas[i];
// 	  Identifier id = m_helperTool->getIdentifier(*rio);

// 	  if (m_idHelperTool->isMdt(id))
// 	    {
// 	      Trk::MagneticFieldProperties magProps; 
// 	      pars = m_propagator->propagate(*segPars,rio->associatedSurface(),Trk::anyDirection,false,magProps); 
// 	      resPull = m_pullCalculator->residualPull(rio, pars, Trk::ResidualPull::Unbiased);
// 	      float pull = resPull ? resPull->pull().front() : -9999;
// 	      delete pars; pars = 0; 
// 	      delete resPull; resPull = 0;
      
// 	      // Fill overview histos
// 	      if (!m_idHelperTool->isEndcap(id)) // Barrel
// 		{
// 		  if (isSideA) // side A
// 		    {
// 		      if(static_cast<int>(mdt2_pulls_BarrelA.size())>h_index && mdt2_pulls_BarrelA[h_index]) {
// 			mdt2_pulls_BarrelA[h_index]->Fill((float)chamberIndex,pull);
// 		      }
// 		    }
// 		  else
// 		    {
// 		      if(static_cast<int>(mdt2_pulls_BarrelC.size())>h_index && mdt2_pulls_BarrelC[h_index]) {
// 			mdt2_pulls_BarrelC[h_index]->Fill((float)chamberIndex,pull);
// 		      }
// 		    }
// 		}
// 	      else // EC
// 		{
// 		  if (isSideA) // side A
// 		    {
// 		      if(static_cast<int>(mdt2_pulls_ECA.size())>h_index && mdt2_pulls_ECA[h_index]) {
// 			mdt2_pulls_ECA[h_index]->Fill((float)chamberIndex,pull);
// 		      }
// 		    }
// 		  else
// 		    {
// 		      if(static_cast<int>(mdt2_pulls_ECC.size())>h_index && mdt2_pulls_ECC[h_index]) {
// 			mdt2_pulls_ECC[h_index]->Fill((float)chamberIndex,pull);
// 		      }
// 		    }
// 		}
// 	    } // if MDT


// 	} // ROT loop

	
// 	delete segPars;
//       } // loop over segms
//     } // loop over collections
  
//     ++m_eventCounter;
//     if (m_debuglevel)  ATH_MSG_DEBUG(  "Number of events : "<< m_eventCounter );
  
//   } // environment if

//   if (m_debuglevel)  ATH_MSG_DEBUG(  "MuonSegmValidation::Finalisation of " << name() << " was successful" );

//   return sc;
// }

// //---------------------------------------------------------------------------------------
// StatusCode MuonSegmValidation::procHistograms()
// {
//   if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

//     if (m_debuglevel)  ATH_MSG_DEBUG(  "MuonSegmMonitoring procHistograms()" );
//     if(endOfEventsBlock){}
//     if(endOfLumiBlock){}
//     if(endOfRun){
      
//       int h_index = -1;
//       for (int i=0;i<(int)m_segmCollectionName.size();i++)
// 	{
// 	  if ( m_segmCollectionFlag[i] == 0 ) continue;
// 	  ++h_index;

// 	  // fill overview pull histos with Gauss fit result
// 	  if(mdt2_pulls_BarrelA[h_index]->GetEntries()>0){
// 	    mdt_pulls_BarrelA[h_index]->Sumw2();
// 	    m_muondqafitfunc->FillGaussMeanAndWidth(mdt2_pulls_BarrelA[h_index],mdt_pulls_BarrelA[h_index],-5.,5.); 
// 	    mdt2_pulls_BarrelA[h_index]->Delete();
// 	    mdt2_pulls_BarrelA[h_index] = 0;
// 	  }
// 	  if(mdt2_pulls_BarrelC[h_index]->GetEntries()>0){
// 	    mdt_pulls_BarrelC[h_index]->Sumw2();
// 	    m_muondqafitfunc->FillGaussMeanAndWidth(mdt2_pulls_BarrelC[h_index],mdt_pulls_BarrelC[h_index],-5.,5.);
// 	    mdt2_pulls_BarrelC[h_index]->Delete();
// 	    mdt2_pulls_BarrelC[h_index] = 0;
// 	  }
// 	  if(mdt2_pulls_ECA[h_index]->GetEntries()>0){
// 	    mdt_pulls_ECA[h_index]->Sumw2();
// 	    m_muondqafitfunc->FillGaussMeanAndWidth(mdt2_pulls_ECA[h_index],mdt_pulls_ECA[h_index],-5.,5.);
// 	    mdt2_pulls_ECA[h_index]->Delete();
// 	    mdt2_pulls_ECA[h_index] = 0;
// 	  }
// 	  if(mdt2_pulls_ECC[h_index]->GetEntries()>0){
// 	    mdt_pulls_ECC[h_index]->Sumw2();
// 	    m_muondqafitfunc->FillGaussMeanAndWidth(mdt2_pulls_ECC[h_index],mdt_pulls_ECC[h_index],-5.,5.);
// 	    mdt2_pulls_ECC[h_index]->Delete();
// 	    mdt2_pulls_ECC[h_index] = 0;
// 	  }

//           mdt2_pulls_BarrelA.clear();
//           mdt_pulls_BarrelA.clear();
//           mdt2_pulls_BarrelC.clear();
//           mdt_pulls_BarrelC.clear();
//           mdt2_pulls_ECA.clear();
//           mdt_pulls_ECA.clear();
//           mdt2_pulls_ECC.clear();
//           mdt_pulls_ECC.clear();

//           mdt_hits_BarrelA.clear();
//           mdt_hits_BarrelC.clear();
//           mdt_hits_ECA.clear();
//           mdt_hits_ECC.clear();

//           chi2_BarrelA.clear();
//           chi2_BarrelC.clear();
//           chi2_ECA.clear();
//           chi2_ECC.clear();

//           nsegm_BarrelA.clear();
//           nsegm_BarrelC.clear();
//           nsegm_ECA.clear();
//           nsegm_ECC.clear();

//           m_num_segms.clear();
//           m_num_hits_per_segmvsphi.clear();
//           m_num_hits_per_segm_vs_station.clear();
//           chi2_vs_station.clear();

//           m_segm_etavsphi.clear();
//           m_segm_eta_vs_phi.clear();
//           m_segm_eta_vs_phiInner.clear();
//           m_segm_eta_vs_phiMiddle.clear();
//           m_segm_eta_vs_phiOuter.clear();

// 	  if ( histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA1.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA1.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA1.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA1.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA2.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA2.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA2.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA2.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA3.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA3.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA3.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA3.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA4.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA4.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA4.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA4.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA5.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA5.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA5.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA5.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA6.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA6.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA6.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA6.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA7.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA7.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA7.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA7.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA8.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA8.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA8.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA8.resProf->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC1.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC1.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC1.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC1.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC2.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC2.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC2.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC2.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC3.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC3.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC3.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC3.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC4.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC4.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC4.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC4.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC5.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC5.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC5.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC5.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC6.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC6.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC6.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC6.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC7.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC7.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC7.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC7.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC8.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC8.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC8.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC8.resProf->LabelsOption("va","X"); }
	  
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA1.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA1.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA1.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA1.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA2.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA2.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA2.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA2.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA3.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA3.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA3.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA3.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA4.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA4.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA4.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA4.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA5.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA5.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA5.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA5.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA6.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA6.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA6.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA6.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA7.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA7.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA7.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA7.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA8.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA8.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA8.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA8.resProf->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC1.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC1.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC1.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC1.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC2.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC2.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC2.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC2.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC3.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC3.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC3.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC3.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC4.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC4.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC4.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC4.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC5.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC5.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC5.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC5.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC6.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC6.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC6.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC6.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC7.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC7.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC7.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC7.resProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC8.resProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC8.resProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC8.resProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC8.resProf->LabelsOption("va","X"); }
	  
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA1.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA1.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA1.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA1.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA2.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA2.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA2.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA2.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA3.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA3.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA3.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA3.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA4.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA4.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA4.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA4.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA5.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA5.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA5.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA5.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA6.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA6.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA6.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA6.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA7.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA7.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA7.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA7.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA8.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA8.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA8.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelA8.pullProf->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC1.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC1.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC1.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC1.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC2.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC2.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC2.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC2.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC3.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC3.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC3.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC3.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC4.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC4.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC4.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC4.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC5.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC5.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC5.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC5.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC6.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC6.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC6.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC6.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC7.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC7.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC7.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC7.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC8.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC8.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC8.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsBarrelC8.pullProf->LabelsOption("va","X"); }
	  
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA1.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA1.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA1.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA1.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA2.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA2.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA2.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA2.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA3.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA3.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA3.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA3.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA4.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA4.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA4.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA4.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA5.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA5.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA5.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA5.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA6.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA6.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA6.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA6.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA7.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA7.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA7.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA7.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA8.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA8.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA8.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECA8.pullProf->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC1.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC1.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC1.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC1.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC2.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC2.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC2.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC2.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC3.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC3.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC3.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC3.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC4.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC4.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC4.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC4.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC5.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC5.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC5.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC5.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC6.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC6.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC6.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC6.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC7.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC7.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC7.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC7.pullProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC8.pullProf && histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC8.pullProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC8.pullProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.residualHistos.mdtsECC8.pullProf->LabelsOption("va","X"); }
	  
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.nsegm->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.nsegm->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.nsegm->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.nsegm->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.nsegm && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.nsegm->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.nsegm->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.nsegm->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.chi2Prof->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.chi2Prof->LabelsOption("va","X"); }
	  
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.chi2Prof->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.chi2Prof->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.chi2Prof && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.chi2Prof->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.chi2Prof->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.chi2Prof->LabelsOption("va","X"); }
	  
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA1.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA2.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA3.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA4.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA5.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA6.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA7.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelA8.nmdtProf->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC1.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC2.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC3.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC4.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC5.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC6.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC7.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsBarrelC8.nmdtProf->LabelsOption("va","X"); }
	  
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA1.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA2.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA3.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA4.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA5.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA6.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA7.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECA8.nmdtProf->LabelsOption("va","X"); }

// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC1.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC2.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC3.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC4.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC5.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC6.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC7.nmdtProf->LabelsOption("va","X"); }
// 	  if (histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.nmdtProf && histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.nmdtProf->GetEntries()!=0 ) {
// 	    //histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.nmdtProf->LabelsDeflate("X");
// 	    histos_from_tool[h_index].hitHistos.countHistos.mdtsECC8.nmdtProf->LabelsOption("va","X"); }
	  
// 	}
         
//     } // isEndOfRun
//     histos_from_tool.clear();
//   } // environment if
  
//   return StatusCode::SUCCESS;   
// }

// //--------------------------------------------------------------------------------------- 

  
// //=====================================================//
// /** finalize  */
// //=====================================================//


// StatusCode MuonSegmValidation::finalize()
// {
//   StatusCode sc = ManagedMonitorToolBase::finalize();
//   if(!sc.isSuccess()) return sc;

//   mdt2_pulls_BarrelA.clear();	
//   mdt_pulls_BarrelA.clear();
//   mdt2_pulls_BarrelC.clear();
//   mdt_pulls_BarrelC.clear();
//   mdt2_pulls_ECA.clear();
//   mdt_pulls_ECA.clear();
//   mdt2_pulls_ECC.clear();
//   mdt_pulls_ECC.clear();
 
//   mdt_hits_BarrelA.clear();
//   mdt_hits_BarrelC.clear();
//   mdt_hits_ECA.clear();
//   mdt_hits_ECC.clear();

//   chi2_BarrelA.clear();
//   chi2_BarrelC.clear();
//   chi2_ECA.clear();
//   chi2_ECC.clear();

//   nsegm_BarrelA.clear();
//   nsegm_BarrelC.clear();
//   nsegm_ECA.clear();
//   nsegm_ECC.clear();

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
// 
