/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************************
 *     Program        : CSCSegmValAlg                                
 *     Package        : MuonSegmMonitoring                           
 *     Authors        : Venkat Kaushik <Venkat.Kaushik@CERN.CH> Jason Veatch <jason.veatch@cern.ch>        
 *     Purpose        : CSC segment monitoring source code
 *----------------------------------------------------------------------
 */

#include "CscRawDataMonitoring/CSCSegmValAlg.h"

#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepData.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"  
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

// Track
#include "TrkTrack/TrackCollection.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/PlaneSurface.h"


#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

// Gaudi Tools
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/INTupleSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"

// Storegate
#include "StoreGate/StoreGate.h"

// ROOT
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

// STL
#include <sstream>
using CLHEP::cm3;

//________________________________________________________________________________________________________
CSCSegmValAlg::CSCSegmValAlg( const std::string & type, const std::string & name, const IInterface* parent ) 
  : ManagedMonitorToolBase( type, name, parent ), 
    m_storeGate(0),
    m_trigDec( "" ),
    m_cscIdHelper(0),
    m_tgcIdHelper(0),
    m_segms(0),
    m_debuglevel(false),
    m_bookedhistos(false) {
  m_helperTool     = ToolHandle<Muon::MuonEDMHelperTool>("Muon::MuonEDMHelperTool/MuonEDMHelperTool");
  m_idHelperTool   = ToolHandle<Muon::MuonIdHelperTool>("Muon::MuonIdHelperTool/MuonIdHelperTool");

  //declareProperty( "UseCollections", m_segmCollectionFlag);
  declareProperty( "SegmentKeys",  m_segmKey);
  declareProperty( "SegmentPrefixes",  m_segmPrefix);
  declareProperty( "SegmentSlopeCuts",  m_segmSlope);
  declareProperty( "ClusterStatus",  m_clusStatWord);
    
  //trigger aware monitoring
  declareProperty("TrigDecisionTool", m_trigDec );
  declareProperty("DoEventSelection",   m_doEvtSel = false );
  declareProperty("EventSelTriggers", m_sampSelTriggers );

  ATH_MSG_INFO( " in CSCSegmValAlg::CSCSegmValAlg() " );
}


//________________________________________________________________________________________________________
CSCSegmValAlg::~CSCSegmValAlg() {
  for(unsigned int i = 0; i < m_segmDetail_EA.size(); i++) {
    if(m_segmDetail_EA[i]) { delete m_segmDetail_EA[i]; m_segmDetail_EA[i] = 0; }
  }
  m_segmDetail_EA.clear();
  for(unsigned int i = 0; i < m_segmDetail_EC.size(); i++) {
    if(m_segmDetail_EC[i]) { delete m_segmDetail_EC[i]; m_segmDetail_EC[i] = 0; }
  }
  m_segmDetail_EC.clear();
  for(unsigned int i = 0; i < m_segmOview_EA.size(); i++) {
    if(m_segmOview_EA[i]) { delete m_segmOview_EA[i]; m_segmOview_EA[i] = 0; }
  }
  m_segmOview_EA.clear();
  for(unsigned int i = 0; i < m_segmOview_EC.size(); i++) {
    if(m_segmOview_EC[i]) { delete m_segmOview_EC[i]; m_segmOview_EC[i] = 0; }
  }
  m_segmOview_EC.clear();
  
  ATH_MSG_INFO( " in CSCSegmValAlg::~CSCSegmValAlg() " );
}



//________________________________________________________________________________________________________
StatusCode CSCSegmValAlg::initialize() {

  m_bookedhistos=false;

  StatusCode sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  ATH_MSG_INFO( "in CSCSegmValAlg::init()" );

  if( m_doEvtSel ) {
    sc = m_trigDec.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING ( "CSCSegmValAlg: Unable to retrieve trigger decision tool");
      m_doEvtSel = false;
      //return sc;
    } else {
      ATH_MSG_INFO ( "TrigDecisionTool    : " << "Using TDT \"" << m_trigDec->name() << "\"" );
    }
    //m_doEvtSel = false;
  }

  if(m_sampSelTriggers.empty() && m_doEvtSel) {
    ATH_MSG_WARNING("Event selection triggers not specified. Switching off trigger-aware monitoring");
    m_doEvtSel = false;
  }


  // load defaults if items are empty or inconsistent
  if(m_segmKey.empty()) {
    m_segmKey.insert(std::pair<std::string, int>("MuonSegments",1));
  }
  if(m_segmPrefix.empty() || (m_segmPrefix.size() != m_segmKey.size())) {
    m_segmPrefix.clear();
    m_segmSlope.clear();
    for(MuonSegmIter it = m_segmKey.begin(); it != m_segmKey.end(); ++it ) {
      std::string prefix = (*it).first;
      if(prefix.length()>2) prefix = prefix.substr(0,prefix.length()/2);
      m_segmPrefix.insert(std::pair<std::string, std::string>((*it).first,prefix));
      m_segmSlope.insert(std::pair<std::string, double>((*it).first,0.07));

    }
  }
  if(m_clusStatWord.empty()) {
    m_clusStatWord.push_back("Unspoiled"); m_clusStatWord.push_back("Simple"); 
    m_clusStatWord.push_back("Edge"); m_clusStatWord.push_back("MultiPeak"); 
    m_clusStatWord.push_back("Narrow"); m_clusStatWord.push_back("Wide"); 
    m_clusStatWord.push_back("Skewed"); m_clusStatWord.push_back("QRatInc"); 
    m_clusStatWord.push_back("StripFitFailed"); m_clusStatWord.push_back("SplitUnspoiled"); 
    m_clusStatWord.push_back("SplitSimple"); m_clusStatWord.push_back("SplitEdge"); 
    m_clusStatWord.push_back("SplitMultiPeak"); m_clusStatWord.push_back("SplitNarrow"); 
    m_clusStatWord.push_back("SplitWide"); m_clusStatWord.push_back("SplitSkewed"); 
    m_clusStatWord.push_back("SplitQRatInc"); m_clusStatWord.push_back("SplitStripFitFailed");
  }
  
  m_NClusWord.clear();
  m_NClusWord.push_back("234");
  m_NClusWord.push_back("134");
  m_NClusWord.push_back("124");
  m_NClusWord.push_back("123");
  m_NClusWord.push_back("All");


  // initialize num of collections
  //m_ncoll = m_segmKey.size();

  //initializing tools
  // Retrieve the StoreGate service

  sc = service( "StoreGateSvc", m_storeGate );
  if ( sc.isFailure() ){
    ATH_MSG_FATAL( "Unable to retrieve the StoreGate service... Exiting!" );
    return sc;
  }
  if (m_debuglevel) ATH_MSG_DEBUG(  "Defined detector service" );

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "DetectorStore service not found !" );
    return sc;
  }   
  if (m_debuglevel) ATH_MSG_DEBUG(  "Defined DetectorStore" );


  sc = detStore->retrieve(m_cscIdHelper,"CSCIDHELPER");
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Cannot get CscIdHelper" );
    return sc;
  }  
  if (m_debuglevel) ATH_MSG_DEBUG( " Found the CscIdHelper " );

  sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Can't retrieve TgcIdHelper" );
    return sc;
  }	   
  if (m_debuglevel) ATH_MSG_DEBUG( " Found the TgcIdHelper " );

  // Retrieve helper tools
  sc = m_helperTool.retrieve();
  if (sc.isFailure()){
    ATH_MSG_FATAL("Could not get " << m_helperTool ); 
    return sc;
  }
  if (m_debuglevel) ATH_MSG_DEBUG( "Retrieved " << m_helperTool );

  sc = m_idHelperTool.retrieve();
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Could not get " << m_idHelperTool ); 
    return sc;
  }
  if (m_debuglevel) ATH_MSG_DEBUG( "Retrieved " << m_idHelperTool );

  return sc;
}  


//________________________________________________________________________________________________________
void CSCSegmValAlg::bookSegmentHistograms() {


  // Book histograms

  int nybinsEA  = 90;  // 16 chambers (x 5 layers per chamber) + 1 set extra
  float nyminEA =  0.; //
  float nymaxEA = 18.;  //

  int nybinsEC  = 85;  // 16 chambers (x 5 layers per chamber) + 1 set extra
  float nyminEC = -17.; //
  float nymaxEC = 0.;  //

 
  int nsegbins = 20;
  float nsegmin = 0;
  float nsegmax = 80;

  int nclustbins = 50;
  float nclustmin = .5;
  float nclustmax = 5.5;

  // (1ke = 1ADC = 1 count)  80, 0, 4000.
  int nqbins = 80;
  float nqmin = 0.;
  float nqmax = 4000.;

  int ntbins = 40;
  float ntmin = -200.;
  float ntmax = 200.;

  
  int nsbins = 11; 
  float nsmin = -0.5;
  float nsmax = 10.5;

  clearHistogramVectors();
  
  std::string segm_detailA = "Muon/MuonSegmentMonitoring/EndCapA/Detail/";
  std::string segm_detailC = "Muon/MuonSegmentMonitoring/EndCapC/Detail/";
  std::string segm_oviewA = "Muon/MuonSegmentMonitoring/EndCapA/Overview/";
  std::string segm_oviewC = "Muon/MuonSegmentMonitoring/EndCapC/Overview/";

  unsigned int ic = 0;
  for(MuonSegmIter it = m_segmKey.begin(); it != m_segmKey.end(); ++it, ic++ ) {
    std::string theKey = (*it).first;
    int useSegm = (*it).second;
    if (useSegm != 1) continue;
    
    if (m_debuglevel)  ATH_MSG_DEBUG("Finished booking histos for " << theKey );

    std::string pfx = m_segmPrefix[theKey];
    
    float binmin = -0.5;
    int nbins = m_clusStatWord.size();
    float binmax = float(nbins) + binmin;
    
    MgmtAttr_t attr = ATTRIB_MANAGED;
    m_segmDetail_EA.push_back(new MonGroup(this, segm_detailA+theKey+"/CSC", run, attr ));;
    m_segmDetail_EC.push_back(new MonGroup(this, segm_detailC+theKey+"/CSC", run, attr ));  
    m_segmOview_EA.push_back(new MonGroup(this, segm_oviewA+theKey+"/CSC", run, attr ));
    m_segmOview_EC.push_back(new MonGroup(this, segm_oviewC+theKey+"/CSC", run, attr ));
 
    // segment hists
    m_h2CSC_Segm_NumOfSegs_EA.push_back( new TH2F(Form("%s_Segm_NumSegments_EA",pfx.c_str()),
						Form("EndCap A: No. of segments; #segments;[sector] + [0.2 #times layer]"),
						nsegbins,nsegmin,nsegmax,nybinsEA,nyminEA,nymaxEA));
    m_h2CSC_Segm_NumOfSegs_EC.push_back( new TH2F(Form("%s_Segm_NumSegments_EC",pfx.c_str()),
						Form("EndCap C: No. of segments; #segments;[sector] + [0.2 #times layer]"),
						nsegbins,nsegmin,nsegmax,nybinsEC,nyminEC,nymaxEC));
    regCSCHist(m_h2CSC_Segm_NumOfSegs_EA[ic], m_segmOview_EA[ic]);
    regCSCHist(m_h2CSC_Segm_NumOfSegs_EC[ic], m_segmOview_EC[ic]);

    // precision clusters on segment

    m_h2CSC_Segm_NumOfNClusSegs_Eta_EA.push_back(new TH2F(Form("%s_Segm_NumNClusSegments_EA",pfx.c_str()),
							Form("EndCap A: #eta-number of 3 and 4 cluster segments with each layer;counts;[sector] + [0.2 #times layer]"),
							nclustbins,nclustmin,nclustmax,nybinsEA,nyminEA,nymaxEA));
    setCSCLayerLabels(m_h2CSC_Segm_NumOfNClusSegs_Eta_EA[ic], 1);
    m_h2CSC_Segm_NumOfNClusSegs_Eta_EC.push_back(new TH2F(Form("%s_Segm_NumNClusSegments_EC",pfx.c_str()),
							Form("EndCap C: #eta-number of 3 and 4 cluster segments with each layer;counts;[sector] + [0.2 #times layer]"),
							nclustbins,nclustmin,nclustmax,nybinsEC,nyminEC,nymaxEC));
    setCSCLayerLabels(m_h2CSC_Segm_NumOfNClusSegs_Eta_EC[ic], -1);
    regCSCHist(m_h2CSC_Segm_NumOfNClusSegs_Eta_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h2CSC_Segm_NumOfNClusSegs_Eta_EC[ic], m_segmDetail_EC[ic]);
   
    for (size_t j = 0; j < m_NClusWord.size(); j++) {
      m_h2CSC_Segm_NumOfNClusSegs_Eta_EA[ic]->GetXaxis()->SetBinLabel(j*10+5, m_NClusWord[j].c_str());
      m_h2CSC_Segm_NumOfNClusSegs_Eta_EC[ic]->GetXaxis()->SetBinLabel(j*10+5, m_NClusWord[j].c_str());
    }

    m_h1CSC_Segm_Efficiency_Eta_EA.push_back(new TH1F(Form("%s_Segm_Efficiency_EA",pfx.c_str()),
						    Form("EndCap A: #eta-segment efficiency per sector;[sector] + [0.2 #times layer];efficiency"),
						    nybinsEA,nyminEA,nymaxEA));
    setCSCLayerLabels(m_h1CSC_Segm_Efficiency_Eta_EA[ic], 1);
    m_h1CSC_Segm_Efficiency_Eta_EC.push_back(new TH1F(Form("%s_Segm_Efficiency_EC",pfx.c_str()),
						    Form("EndCap C: #eta-segment efficiency per sector;[sector] + [0.2 #times layer];efficiency"),
						    nybinsEC,nyminEC,nymaxEC));
    setCSCLayerLabels(m_h1CSC_Segm_Efficiency_Eta_EC[ic], -1);

    regCSCHist(m_h1CSC_Segm_Efficiency_Eta_EA[ic], m_segmOview_EA[ic]);
    regCSCHist(m_h1CSC_Segm_Efficiency_Eta_EC[ic], m_segmOview_EC[ic]);
    
    m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EA.push_back(new TH2F(Form("%s_Segm_QSumEtaGoodClusPerLayer_EA",pfx.c_str()),
							   Form("EndCap A: #eta-cluster charge per layer;counts;[sector] + [0.2 #times layer]"),
							   nqbins,nqmin,nqmax,nybinsEA,nyminEA,nymaxEA));
    
    m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EC.push_back(new TH2F(Form("%s_Segm_QSumEtaGoodClusPerLayer_EC",pfx.c_str()),
							   Form("EndCap C: #eta-cluster charge per layer;counts;[sector] + [0.2 #times layer]"),
							   nqbins,nqmin,nqmax,nybinsEC,nyminEC,nymaxEC));

    regCSCHist(m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EA[ic], m_segmOview_EA[ic]);
    regCSCHist(m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EC[ic], m_segmOview_EC[ic]);

    m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EA.push_back(new TH2F(Form("%s_Segm_TimeEtaGoodClusPerLayer_EA",pfx.c_str()),
							   Form("EndCap A: #eta-cluster time per layer;time [ns];[sector] + [0.2 #times layer]"),
							   ntbins,ntmin,ntmax,nybinsEA,nyminEA,nymaxEA));
    
    m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EC.push_back(new TH2F(Form("%s_Segm_TimeEtaGoodClusPerLayer_EC",pfx.c_str()),
							   Form("EndCap C: #eta-cluster time per layer;time [ns];[sector] + [0.2 #times layer]"),
							   ntbins,ntmin,ntmax,nybinsEC,nyminEC,nymaxEC));

    regCSCHist(m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EA[ic], m_segmOview_EA[ic]);
    regCSCHist(m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EC[ic], m_segmOview_EC[ic]);


    m_h1CSC_Segm_StatOfClus_Eta_EA.push_back( new TH1F(Form("%s_Segm_EtaClusterStatus_EA",pfx.c_str()), 
						     "Endcap A: #eta-cluster status;;entries",nbins,binmin,binmax));

    m_h1CSC_Segm_StatOfClus_Eta_EC.push_back( new TH1F(Form("%s_Segm_EtaClusterStatus_EC",pfx.c_str()), 
						     "Endcap C: #eta-cluster status;;entries",nbins,binmin,binmax));
    for (size_t j = 0; j < m_clusStatWord.size(); j++) {
      m_h1CSC_Segm_StatOfClus_Eta_EA[ic]->GetXaxis()->SetBinLabel(j+1, m_clusStatWord[j].c_str());
      m_h1CSC_Segm_StatOfClus_Eta_EC[ic]->GetXaxis()->SetBinLabel(j+1, m_clusStatWord[j].c_str());
    }
    regCSCHist(m_h1CSC_Segm_StatOfClus_Eta_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_StatOfClus_Eta_EC[ic], m_segmDetail_EC[ic]);
    
    m_h1CSC_Segm_NumOfClus_Eta_EA.push_back( new TH1F(Form("%s_Segm_NumEtaCluster_EA",pfx.c_str()), 
						    "Endcap A: No. of #eta-clusters on segment;#clusters;entries", nsbins,nsmin,nsmax));
    m_h1CSC_Segm_NumOfClus_Eta_EC.push_back( new TH1F(Form("%s_Segm_NumEtaCluster_EC",pfx.c_str()), 
						    "Endcap C: No. of #eta-clusters on segment;#clusters;entries", nsbins,nsmin,nsmax));
    regCSCHist(m_h1CSC_Segm_NumOfClus_Eta_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_NumOfClus_Eta_EC[ic], m_segmDetail_EC[ic]);

    m_h1CSC_Segm_NumOfGoodClus_Eta_EA.push_back( new TH1F(Form("%s_Segm_NumGoodEtaCluster_EA",pfx.c_str()),
							"Endcap A: No. of good #eta-clusters on segment;#good-clusters;entries", nsbins,nsmin,nsmax));
    m_h1CSC_Segm_NumOfGoodClus_Eta_EC.push_back( new TH1F(Form("%s_Segm_NumGoodEtaCluster_EC",pfx.c_str()),
							"Endcap C: No. of good #eta-clusters on segment;#good-clusters;entries", nsbins,nsmin,nsmax));
    regCSCHist(m_h1CSC_Segm_NumOfGoodClus_Eta_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_NumOfGoodClus_Eta_EC[ic], m_segmDetail_EC[ic]);

    m_h1CSC_Segm_QsumOfClus_Eta_EA.push_back( new TH1F(Form("%s_Segm_QSumEtaCluster_EA",pfx.c_str()), 
						     "Endcap A: #eta-cluster Qsum;counts;entries", nqbins,nqmin,nqmax ));
    m_h1CSC_Segm_QsumOfClus_Eta_EC.push_back( new TH1F(Form("%s_Segm_QSumEtaCluster_EC",pfx.c_str()), 
						     "Endcap C: #eta-cluster Qsum;counts;entries", nqbins,nqmin,nqmax ));
    regCSCHist(m_h1CSC_Segm_QsumOfClus_Eta_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_QsumOfClus_Eta_EC[ic], m_segmDetail_EC[ic]);
    
    m_h1CSC_Segm_TimeOfClus_Eta_EA.push_back( new TH1F(Form("%s_Segm_TimeEtaCluster_EA",pfx.c_str()), 
						     "Endcap A: #eta-cluster Time;time [ns];entries", ntbins,ntmin,ntmax ));
    m_h1CSC_Segm_TimeOfClus_Eta_EC.push_back( new TH1F(Form("%s_Segm_TimeEtaCluster_EC",pfx.c_str()), 
						     "Endcap C: #eta-cluster Time;time [ns];entries", ntbins,ntmin,ntmax ));
    regCSCHist(m_h1CSC_Segm_TimeOfClus_Eta_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_TimeOfClus_Eta_EC[ic], m_segmDetail_EC[ic]);
    
    m_h1CSC_Segm_QsumOfGoodClus_Eta_EA.push_back( new TH1F(Form("%s_Segm_QSumGoodEtaCluster_EA",pfx.c_str()), 
							 "Endcap A: Good #eta-cluster Qsum;counts;entries", nqbins,nqmin,nqmax ));
    m_h1CSC_Segm_QsumOfGoodClus_Eta_EC.push_back( new TH1F(Form("%s_Segm_QSumGoodEtaCluster_EC",pfx.c_str()), 
							 "Endcap C: Good #eta-cluster Qsum;counts;entries", nqbins,nqmin,nqmax ));
    regCSCHist(m_h1CSC_Segm_QsumOfGoodClus_Eta_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_QsumOfGoodClus_Eta_EC[ic], m_segmDetail_EC[ic]);

    m_h1CSC_Segm_TimeOfGoodClus_Eta_EA.push_back( new TH1F(Form("%s_Segm_TimeGoodEtaCluster_EA",pfx.c_str()), 
							 "Endcap A: Good #eta-cluster Time;time [ns];entries", ntbins,ntmin,ntmax ));
    m_h1CSC_Segm_TimeOfGoodClus_Eta_EC.push_back( new TH1F(Form("%s_Segm_TimeGoodEtaCluster_EC",pfx.c_str()), 
							 "Endcap C: Good #eta-cluster Time;time [ns];entries", ntbins,ntmin,ntmax ));
    regCSCHist(m_h1CSC_Segm_TimeOfGoodClus_Eta_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_TimeOfGoodClus_Eta_EC[ic], m_segmDetail_EC[ic]);


    // transverse clusters on segment
    m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EA.push_back(new TH2F(Form("%s_Segm_QSumPhiGoodClusPerLayer_EA",pfx.c_str()),
							   Form("EndCap A: #phi-cluster charge per layer;counts;[sector] + [0.2 #times layer]"),
							   nqbins,nqmin,nqmax,nybinsEA,nyminEA,nymaxEA));
    
    m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EC.push_back(new TH2F(Form("%s_Segm_QSumPhiGoodClusPerLayer_EC",pfx.c_str()),
							   Form("EndCap C: #phi-cluster charge per layer;counts;[sector] + [0.2 #times layer]"),
							   nqbins,nqmin,nqmax,nybinsEC,nyminEC,nymaxEC));

    regCSCHist(m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EA[ic], m_segmOview_EA[ic]);
    regCSCHist(m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EC[ic], m_segmOview_EC[ic]);

    m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EA.push_back(new TH2F(Form("%s_Segm_TimePhiGoodClusPerLayer_EA",pfx.c_str()),
							   Form("EndCap A: #phi-cluster time per layer;time [ns];[sector] + [0.2 #times layer]"),
							   ntbins,ntmin,ntmax,nybinsEA,nyminEA,nymaxEA));
    
    m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EC.push_back(new TH2F(Form("%s_Segm_TimePhiGoodClusPerLayer_EC",pfx.c_str()),
							   Form("EndCap C: #phi-cluster time per layer;time [ns];[sector] + [0.2 #times layer]"),
							   ntbins,ntmin,ntmax,nybinsEC,nyminEC,nymaxEC));

    regCSCHist(m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EA[ic], m_segmOview_EA[ic]);
    regCSCHist(m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EC[ic], m_segmOview_EC[ic]);

    m_h1CSC_Segm_StatOfClus_Phi_EA.push_back( new TH1F(Form("%s_Segm_PhiClusterStatus_EA",pfx.c_str()), 
						     "Endcap A: #phi-cluster status;;entries",nbins,binmin,binmax));

    m_h1CSC_Segm_StatOfClus_Phi_EC.push_back( new TH1F(Form("%s_Segm_PhiClusterStatus_EC",pfx.c_str()), 
						     "Endcap C: #phi-cluster status;;entries",nbins,binmin,binmax));

    for (size_t j = 0; j < m_clusStatWord.size(); j++) {
      m_h1CSC_Segm_StatOfClus_Phi_EA[ic]->GetXaxis()->SetBinLabel(j+1, m_clusStatWord[j].c_str());
      m_h1CSC_Segm_StatOfClus_Phi_EC[ic]->GetXaxis()->SetBinLabel(j+1, m_clusStatWord[j].c_str());
    }
    regCSCHist(m_h1CSC_Segm_StatOfClus_Phi_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_StatOfClus_Phi_EC[ic], m_segmDetail_EC[ic]);
    
    m_h1CSC_Segm_NumOfClus_Phi_EA.push_back( new TH1F(Form("%s_Segm_NumPhiCluster_EA",pfx.c_str()), 
						    "Endcap A: No. of #phi-clusters on segment;#clusters;entries", nsbins,nsmin,nsmax));
    m_h1CSC_Segm_NumOfClus_Phi_EC.push_back( new TH1F(Form("%s_Segm_NumPhiCluster_EC",pfx.c_str()), 
						    "Endcap C: No. of #phi-clusters on segment;#clusters;entries", nsbins,nsmin,nsmax));
    regCSCHist(m_h1CSC_Segm_NumOfClus_Phi_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_NumOfClus_Phi_EC[ic], m_segmDetail_EC[ic]);

    m_h1CSC_Segm_NumOfGoodClus_Phi_EA.push_back( new TH1F(Form("%s_Segm_NumGoodPhiCluster_EA",pfx.c_str()),
							"Endcap A: No. of good #phi-clusters on segment;#good-clusters;entries", nsbins,nsmin,nsmax));
    m_h1CSC_Segm_NumOfGoodClus_Phi_EC.push_back( new TH1F(Form("%s_Segm_NumGoodPhiCluster_EC",pfx.c_str()),
							"Endcap C: No. of good #phi-clusters on segment;#good-clusters;entries", nsbins,nsmin,nsmax));
    regCSCHist(m_h1CSC_Segm_NumOfGoodClus_Phi_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_NumOfGoodClus_Phi_EC[ic], m_segmDetail_EC[ic]);

    m_h1CSC_Segm_QsumOfClus_Phi_EA.push_back( new TH1F(Form("%s_Segm_QSumPhiCluster_EA",pfx.c_str()), 
						     "Endcap A: #phi-cluster Qsum;counts;entries", nqbins,nqmin,nqmax ));
    m_h1CSC_Segm_QsumOfClus_Phi_EC.push_back( new TH1F(Form("%s_Segm_QSumPhiCluster_EC",pfx.c_str()), 
						     "Endcap C: #phi-cluster Qsum;counts;entries", nqbins,nqmin,nqmax ));
    regCSCHist(m_h1CSC_Segm_QsumOfClus_Phi_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_QsumOfClus_Phi_EC[ic], m_segmDetail_EC[ic]);
    
    m_h1CSC_Segm_TimeOfClus_Phi_EA.push_back( new TH1F(Form("%s_Segm_TimePhiCluster_EA",pfx.c_str()), 
						     "Endcap A: #phi-cluster Time;time [ns];entries", ntbins,ntmin,ntmax ));
    m_h1CSC_Segm_TimeOfClus_Phi_EC.push_back( new TH1F(Form("%s_Segm_TimePhiCluster_EC",pfx.c_str()), 
						     "Endcap C: #phi-cluster Time;time [ns];entries", ntbins,ntmin,ntmax ));
    regCSCHist(m_h1CSC_Segm_TimeOfClus_Phi_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_TimeOfClus_Phi_EC[ic], m_segmDetail_EC[ic]);
    
    m_h1CSC_Segm_QsumOfGoodClus_Phi_EA.push_back( new TH1F(Form("%s_Segm_QSumGoodPhiCluster_EA",pfx.c_str()), 
							 "Endcap A: Good #phi-cluster Qsum;counts;entries", nqbins,nqmin,nqmax ));
    m_h1CSC_Segm_QsumOfGoodClus_Phi_EC.push_back( new TH1F(Form("%s_Segm_QSumGoodPhiCluster_EC",pfx.c_str()), 
							 "Endcap C: Good #phi-cluster Qsum;counts;entries", nqbins,nqmin,nqmax ));
    regCSCHist(m_h1CSC_Segm_QsumOfGoodClus_Phi_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_QsumOfGoodClus_Phi_EC[ic], m_segmDetail_EC[ic]);
    
    m_h1CSC_Segm_TimeOfGoodClus_Phi_EA.push_back( new TH1F(Form("%s_Segm_TimeGoodPhiCluster_EA",pfx.c_str()), 
							 "Endcap A: Good #phi-cluster Time;time [ns];entries", ntbins,ntmin,ntmax ));
    m_h1CSC_Segm_TimeOfGoodClus_Phi_EC.push_back( new TH1F(Form("%s_Segm_TimeGoodPhiCluster_EC",pfx.c_str()), 
							 "Endcap C: Good #phi-cluster Time;time [ns];entries", ntbins,ntmin,ntmax ));
    regCSCHist(m_h1CSC_Segm_TimeOfGoodClus_Phi_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h1CSC_Segm_TimeOfGoodClus_Phi_EC[ic], m_segmDetail_EC[ic]);
    
    // eta vs. phi correlation 
    m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EA.push_back( new TH2F(Form("%s_Segm_QSumGoodClusCorrelation_EA",pfx.c_str()), 
							      "Endcap A: #phi-cluster vs. good #eta-cluster;good #eta-cluster counts;good #phi-cluster counts", nqbins,nqmin,nqmax, nqbins,nqmin,nqmax));
    m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EC.push_back( new TH2F(Form("%s_Segm_QSumGoodClusCorrelation_EC",pfx.c_str()), 
							      "Endcap A: #phi-cluster vs. good #eta-cluster;good #eta-cluster counts;good #phi-cluster counts", nqbins,nqmin,nqmax, nqbins,nqmin,nqmax));
    regCSCHist(m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EA[ic], m_segmDetail_EA[ic]);
    regCSCHist(m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EC[ic], m_segmDetail_EC[ic]);

    //m_h2csc_clus_r_vs_z_hitmap = new TH2F("h2csc_clus_r_vs_z_hitmap", "R vs. Z Cluster hitmap;z(CLHEP::mm);R(CLHEP::mm)",200, -10000., 10000., 40, 0., 4000);
    //m_h2csc_clus_y_vs_x_hitmap = new TH2F("h2csc_clus_y_vs_x_hitmap", "Y vs. X Cluster hitmap;x(CLHEP::mm);y(CLHEP::mm)",100, -5000., 5000., 100, -5000., 5000);
    
  }
 
  m_bookedhistos=true;
}


//________________________________________________________________________________________________________
StatusCode CSCSegmValAlg::bookHistograms() {

  if (m_debuglevel) ATH_MSG_DEBUG( "CSCSegmValAlg::bookHistograms() to setup tools/services" );
  StatusCode sc = StatusCode::SUCCESS;

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

    //if(newEventsBlock){}
    //if(newLumiBlock){}
    //if(newRun) {
    if(!m_bookedhistos) bookSegmentHistograms();
    //} // if NewRun

  } // environment if  
  return sc;

}


//________________________________________________________________________________________________________
StatusCode CSCSegmValAlg::fillHistograms() {

  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_DEBUG( "in CSCSegmValAlg::fillHistograms()                             " );

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ||  m_environment == AthenaMonManager::user ) {

    // if required, check if event passed sample-selection triggers
    if(m_doEvtSel) { if(!evtSelTriggersPassed()) return sc; }

    // Segment Cluster counter
    int segmClustCount[33];

    // arrays to hold cluster-count  
    // 32 chambers and 8 layers (each has one extra - index '0' is not counted)
    int clusCount[33][9];
    //, m_sigclusCount[33][9];
    //unsigned int m_nEtaClusWidthCnt = 0, m_nPhiClusWidthCnt = 0;
    for(unsigned int kl = 0; kl < 33; kl++ ) {
      for(unsigned int cm3 = 0; cm3 < 9; cm3++ ) {
        clusCount[kl][cm3] = 0;
      }
    }


    bool got_coll = false; unsigned int ic = 0;
    for (MuonSegmIter it = m_segmKey.begin(); it != m_segmKey.end(); ++it, ic++ ) {

      std::string theKey = (*it).first;
      int useSegm = (*it).second;

      if (m_debuglevel)  ATH_MSG_DEBUG( (useSegm == 1 ? "Using ": "Not using ") << " collection " << theKey );

      if ( useSegm != 1 ) continue;

      //Get segms from SG
      sc = m_storeGate->retrieve(m_segms, theKey);

      if ( sc.isFailure() ){
        if ( (ic == m_segmKey.size()-1) && !got_coll ){
          ATH_MSG_DEBUG( "Unable to retrieve reconstructed segms from any collection... Exiting!" );
          return StatusCode::FAILURE; 
        } else {
          ATH_MSG_DEBUG( "Unable to retrieve reconstructed segms from collection... Trying next..." );
          continue; 
        }
      } else {
        if (m_debuglevel) ATH_MSG_DEBUG( "Segms in StoreGate found. Segm collection is " << theKey );
        got_coll = true;
      }

      if ( m_segms->empty() ){

        if (m_debuglevel){
          ATH_MSG_DEBUG( "      Segm Collection is Empty.  Trying next...    ");
        }
        continue;
      }

      if (m_debuglevel){
        ATH_MSG_DEBUG( "      Segm Collection size         " );
        ATH_MSG_DEBUG( "Number of segms found: " << m_segms->size() );
      }

      if ( m_segms->size() > 0 ){
        ATH_MSG_DEBUG(   "Number of segms in event is: " << m_segms->size()                               );
        ATH_MSG_DEBUG(  "This algorithm is designed to operate for single segm / event only"             );
        ATH_MSG_DEBUG(  "Processing only the first segm in the segm collection"                         );
      }

      int segnum = 0;
      int layerindex = 0;

      //Loop over segms
      for (Trk::SegmentCollection::const_iterator s = m_segms->begin();s != m_segms->end(); ++s) {

        // Get segm
        const Muon::MuonSegment *segm=dynamic_cast<const Muon::MuonSegment*>(*s);

        if (segm == 0) {
          ATH_MSG_ERROR( "no pointer to segm!!!" );
          continue;
        }

        // Loop over segment ROTs - not necessary 03-23
        // for(unsigned int i =0; i < segm->numberOfContainedROTs(); ++i) {
	ATH_MSG_DEBUG( "Looking at segment id" );

	// Skip segment if there are no csc hits
	if ( cscHits(segm)<1 ) continue;

	// Get contained measurements
	const std::vector<const Trk::MeasurementBase*> meas = segm->containedMeasurements();

	// Skip segment if no measurements
	// Number of clusters on segment
	int n_clust=meas.size(); 
	if ( n_clust < 2 ) continue;

	// Analyze segment if it is a csc segment
	if ( isCscSegment(segm) ) {

	  segnum++;

	  // Initialize cluster counter
	  for(int sect = 0; sect < 33; sect++) {
	    segmClustCount[sect] = 0;
	    for(unsigned int ilay = 0; ilay < 9; ilay++ ) {
	      clusCount[sect][ilay] = 0;
	    }

	  }

	  // Print out segment information
	  const Trk::FitQuality* fq = segm->fitQuality();
	  double chi2 = 999.;
	  int ndof = -1;
	  if( fq ) {
	    chi2 = fq->chiSquared();
	    ndof = fq->numberDoF();
	    ATH_MSG_DEBUG( "Chi2 " << chi2 );
	    //chi2_histo->Fill(chi2);
	    ATH_MSG_DEBUG( "Ndof " << ndof );
	  }

	  // cut on segment angle
	  float segm_ly = segm->localParameters()[Trk::locY];
	  float segm_ayz = segm->localDirection().angleYZ();
	  segm_ayz -= M_PI/2.;
	  float segm_cut = m_segmSlope[theKey];
	  bool segmAngle_cut = segmSlopeCut(segm_ly, segm_ayz, segm_cut);
	  ATH_MSG_DEBUG(theKey << " local_pos: " << segm_ly << 
			"\tangle_yz: " << segm_ayz << "\tcut: " << segm_cut << "\t pass = " << segmAngle_cut );
	  //if(!segmAngle_cut) continue; //already in the new reconstruction


	  ATH_MSG_DEBUG( "R " << segm->globalPosition().perp() );
	  ATH_MSG_DEBUG( "Z " << segm->globalPosition().z() );
	  ATH_MSG_DEBUG( "Phi " << segm->globalPosition().phi() );
	  ATH_MSG_DEBUG( "Eta " << segm->globalPosition().eta() );
	  ATH_MSG_DEBUG( "Dir Phi " << segm->globalDirection().phi() );
	  ATH_MSG_DEBUG( "Dir Eta " << segm->globalDirection().eta() ); 


	  // ==============================================================================
	  // Field           Range               Notes
	  // ==============================================================================
	  // StationName     unsigned integer    maps to "CSS", "CSL", etc.
	  // StationEta      [-1,1]              -1 for backward, 1 for forward endcap
	  // StationPhi      [1,8]               increases with Phi
	  // Technology      [1]                 maps to "CSC"
	  // ChamberLayer    [1,2]               increases with |Z|
	  // WireLayer       [1,4]               increases with |Z|
	  // MeasuresPhi     [0,1]               0 if measures R, 1 if measures Phi
	  // Strip           [1,n]               increases with R   for MeasuresPhi=0
	  //                                     increases with Phi for MeasuresPhi=1
	  // ==============================================================================

	  // identify the segment location
	  const Trk::MeasurementBase* rio = meas.at(0);
	  Identifier segmId = m_helperTool->getIdentifier(*rio);

	  int segm_stationPhi  = m_cscIdHelper->stationPhi(segmId);
	  int segm_stationEta  = m_cscIdHelper->stationEta(segmId);
	  int segm_stationName = m_cscIdHelper->stationName(segmId);
	  std::string segm_stationString = m_cscIdHelper->stationNameString(segm_stationName);
	  int segm_chamberType = segm_stationString == "CSS" ? 0 : 1;
	  int segm_sectorNo  = segm_stationEta * (2 * segm_stationPhi - segm_chamberType); // [-16 -> -1] and [+1 -> +16]
	  int segm_isec = segm_sectorNo < 0 ? segm_sectorNo*(-1) : segm_sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
	  ATH_MSG_DEBUG(" sgsec = " << segm_isec << "\tsec = " << segm_sectorNo);
	  if(segm_stationEta == 1) m_h2CSC_Segm_NumOfSegs_EA[ic]->Fill(n_clust, segm_sectorNo);
	  else m_h2CSC_Segm_NumOfSegs_EC[ic]->Fill(n_clust, segm_sectorNo);

	  // Loop over clusters
	  // [i][j] {i == 0(EA), 1(EC) }
	  float clus_kiloele = 1.0e-3; // multiply # of electrons by this number to get kiloElectrons (1 ke = 1 ADC)
	  int eta_clus_count[2][2] = {{0},{0}}, phi_clus_count[2][2] = {{0},{0}}; // no. of prec/trans hits per segment
	  float eta_clus_qsum[2][5] = {{-1.}, {-1.}},  phi_clus_qsum[2][5] = {{-1.}, {-1.}}; // qsum over each prec/trans. layer on segment
	  float eta_clus_time[2][5] = {{-1.}, {-1.}},  phi_clus_time[2][5] = {{-1.}, {-1.}}; // time over each prec/trans. layer on segment
	  int eta_clus_use[2][5] = {{0},{0}}, phi_clus_use[2][5] = {{0}, {0}};
	  //int eta_clus_status[5] = {-1}, phi_clus_status[5] = {-1};

	  layerindex = 0;
         
	  for(TrkSegmIter hit = segm->containedMeasurements().begin(); 
	      hit != segm->containedMeasurements().end(); ++hit ) {
	    const Muon::CscClusterOnTrack* clust_rot = dynamic_cast<const Muon::CscClusterOnTrack*>(*hit);
	    if( clust_rot ) {
	      Identifier clusId = m_helperTool->getIdentifier(*clust_rot);

	      // get the cluster coordinates
	      int clus_stationName = m_cscIdHelper->stationName(clusId);
	      std::string clus_stationString = m_cscIdHelper->stationNameString(clus_stationName);
	      int clus_chamberType = clus_stationString == "CSS" ? 0 : 1;
	      int clus_stationEta  = m_cscIdHelper->stationEta(clusId);
	      int clus_stationPhi  = m_cscIdHelper->stationPhi(clusId);
	      int clus_wireLayer = m_cscIdHelper->wireLayer(clusId);
	      int clus_measuresPhi = m_cscIdHelper->measuresPhi(clusId);

	      // convert to my coordinates
	      int clus_sectorNo  = clus_stationEta * (2 * clus_stationPhi - clus_chamberType);   // [-16 -> -1] and [+1 -> +16]
	      float clus_secLayer = clus_sectorNo + 0.2 * (clus_wireLayer - 1) + 0.1;
	      //int xfac = clus_measuresPhi ? -1 : 1;        // [-1 -> -48] / [+1 -> +192]
	      int clus_isec = clus_sectorNo < 0 ? clus_sectorNo*(-1) : clus_sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
	      int clus_ilay = (clus_measuresPhi ? clus_wireLayer : clus_wireLayer+4);

	      // check the cluster status; probably need to read status info from jobOptions - not done for the moment
	      // status = Muon::CscStatusUnspoiled (i.e 0) or Muon::CscStatusSplitUnspoiled (i.e 10) are considered good for precision clusters
	      // status = Muon::CscStatusSimple (i.e 1) could be good for non-precision clusters (i.e for phi-layers)
	      Muon::CscClusterStatus status = clust_rot->status();
	      if(segm_stationEta == 1) {
		if(clus_measuresPhi == 0) m_h1CSC_Segm_StatOfClus_Eta_EA[ic]->Fill(status);
		else m_h1CSC_Segm_StatOfClus_Phi_EA[ic]->Fill(status);
	      } else {
		if(clus_measuresPhi == 0) m_h1CSC_Segm_StatOfClus_Eta_EC[ic]->Fill(status);
		else m_h1CSC_Segm_StatOfClus_Phi_EC[ic]->Fill(status);
	      }

	      //if(clus_measuresPhi == 0) eta_clus_status[clus_wireLayer] = status;
	      //else phi_clus_status[clus_wireLayer] = status;

	      std::string clus_stat = Muon::toString(status);
	      bool clus_status = ( (clus_stat == "unspoiled")                 ||
				   (clus_stat == "unspoiled with split")                       ||
				   (clus_stat == "simple")
				   ) ? true : false;
	      bool clus_stat_eff = ( (int(status) >= 0 && int(status) < 8)     ||
				     (int(status) > 8 && int(status) < 18) ) ? true : false;

	      // get cluster
	      const Muon::CscPrepData* theClus = clust_rot->prepRawData();
	      float clus_qsum = 0, clus_time = -1.;
	      //float clus_globx = 0., clus_globy = 0., clus_globz = 0., clus_globr = 0.;

	      if(theClus) {
		clus_qsum = theClus->charge() * clus_kiloele;
		clus_time = theClus->time();
		//clus_globx = theClus->globalPosition().x();
		//clus_globy = theClus->globalPosition().y();
		//clus_globz = theClus->globalPosition().z();
		//clus_globr = theClus->globalPosition().perp();
                  
		if(clus_measuresPhi == 0) { 
		  if(clus_stationEta == 1) eta_clus_count[0][0]++;
		  else eta_clus_count[1][0]++;
		} else {
		  if(clus_stationEta == 1) phi_clus_count[0][0]++;
		  else phi_clus_count[1][0]++;
		}
	      }

	      // get no. of strips per cluster
	      unsigned int clus_noStrips = theClus->rdoList().size();

	      // need at least three strips in an eta-cluster
	      bool clus_eta_status = clus_status && ( clus_noStrips > 2 ) && (clus_measuresPhi == 0);
	      bool clus_eta_eff = clus_stat_eff && ( clus_noStrips > 2 ) && (clus_measuresPhi == 0);
	      if(clus_eta_eff) {
		if(clus_stationEta == 1) {
		  eta_clus_count[0][1]++;
		} else {
		  eta_clus_count[1][1]++;
		}
	      }
	      if(clus_eta_status) {
		if(clus_stationEta == 1) {
		  eta_clus_qsum[0][clus_wireLayer] = clus_qsum;
		  eta_clus_time[0][clus_wireLayer] = clus_time;
		  eta_clus_use[0][clus_wireLayer] = 1;
		  m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EA[ic]->Fill(clus_qsum, clus_secLayer);
		  if(fabs(clus_time) <= 200) m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EA[ic]->Fill(clus_time, clus_secLayer);
		} else {
		  eta_clus_qsum[1][clus_wireLayer] = clus_qsum;
		  eta_clus_time[1][clus_wireLayer] = clus_time;
		  eta_clus_use[1][clus_wireLayer] = 1;
		  m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EC[ic]->Fill(clus_qsum, clus_secLayer);
		  if(fabs(clus_time) <= 200) m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EC[ic]->Fill(clus_time, clus_secLayer);
		}
	      }

	      // need at least one strip in a phi-cluster
	      bool clus_phi_status = clus_status && ( clus_noStrips > 0 ) && (clus_measuresPhi == 1);
	      if(clus_phi_status) {
		if(clus_stationEta == 1) {
		  phi_clus_qsum[0][clus_wireLayer] = clus_qsum;
		  phi_clus_time[0][clus_wireLayer] = clus_time;
		  phi_clus_use[0][clus_wireLayer] = 1;
		  phi_clus_count[0][1]++;
		  m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EA[ic]->Fill(clus_qsum, clus_secLayer);
		  if(fabs(clus_time) <= 200) m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EA[ic]->Fill(clus_time, clus_secLayer);
		} else {
		  phi_clus_qsum[1][clus_wireLayer] = clus_qsum;
		  phi_clus_time[1][clus_wireLayer] = clus_time;
		  phi_clus_use[1][clus_wireLayer] = 1;
		  phi_clus_count[1][1]++;
		  m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EC[ic]->Fill(clus_qsum, clus_secLayer);
		  if(fabs(clus_time) <= 200) m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EC[ic]->Fill(clus_time, clus_secLayer);
		}
	      }

	      // increment the cluster-count for this layer
	      if(clus_eta_status || clus_phi_status) clusCount[clus_isec][clus_ilay]++;

	      // increment segment cluster count
	      if(clus_eta_status) segmClustCount[clus_isec]++;
                
	      if(clus_eta_status) layerindex+=clus_wireLayer;

	      ATH_MSG_DEBUG("status = " << clus_stat << "\tcharge = " << clus_qsum << "\ttime= " << clus_time << "\tnstrips = " << clus_noStrips);

	    } else {
	    } // if cluster found
	  } // for loop over clusters           
                
            // Print out segment number and cluster count
            //for (int isect = 1; isect < 33; isect++){
            //  if(segmClustCount[isect] < 3) continue;
            //}

            //ATH_MSG_VERBOSE("eta_count(EA) = " << eta_clus_count[0][0] << "\tphi_count(EA) = " << phi_clus_count[0][0]);
            //ATH_MSG_VERBOSE("eta_count(EC) = " << eta_clus_count[1][0] << "\tphi_count(EC) = " << phi_clus_count[1][0]);
            //ATH_MSG_VERBOSE("eta_good_count(EA) = " << eta_clus_count[0][1] << "\tphi_good_count(EA) = " << phi_clus_count[0][1]);
            //ATH_MSG_VERBOSE("eta_good_count(EC) = " << eta_clus_count[1][1] << "\tphi_good_count(EC) = " << phi_clus_count[1][1]);
            // fill (good- )cluster counts on each eta-station (EA, EC) for eta/phi clusters
	  if(eta_clus_count[0][0] > 0) m_h1CSC_Segm_NumOfClus_Eta_EA[ic]->Fill(eta_clus_count[0][0]);
	  if(phi_clus_count[0][0] > 0) m_h1CSC_Segm_NumOfClus_Phi_EA[ic]->Fill(phi_clus_count[0][0]);
	  if(eta_clus_count[0][1] > 0) m_h1CSC_Segm_NumOfGoodClus_Eta_EA[ic]->Fill(eta_clus_count[0][1]);
	  if(phi_clus_count[0][1] > 0) m_h1CSC_Segm_NumOfGoodClus_Phi_EA[ic]->Fill(phi_clus_count[0][1]);
	  if(eta_clus_count[1][0] > 0) m_h1CSC_Segm_NumOfClus_Eta_EC[ic]->Fill(eta_clus_count[1][0]);
	  if(phi_clus_count[1][0] > 0) m_h1CSC_Segm_NumOfClus_Phi_EC[ic]->Fill(phi_clus_count[1][0]);
	  if(eta_clus_count[1][1] > 0) m_h1CSC_Segm_NumOfGoodClus_Eta_EC[ic]->Fill(eta_clus_count[1][1]);
	  if(phi_clus_count[1][1] > 0) m_h1CSC_Segm_NumOfGoodClus_Phi_EC[ic]->Fill(phi_clus_count[1][1]);

	  // Fill number of 3 and 4 cluster segment histogram
	  for (int isect = 1; isect < 17; isect++) {
	    if(segmClustCount[isect+16] > 2){
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EA[ic]->Fill(layerindex-5, isect);
	    }
	    if(segmClustCount[isect] > 2){
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EC[ic]->Fill(layerindex-5, (-1.)*isect);
	    }
	    if(segmClustCount[isect+16] > 3){
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EA[ic]->Fill(1, isect);
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EA[ic]->Fill(2, isect);
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EA[ic]->Fill(3, isect);
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EA[ic]->Fill(4, isect);
	    }
	    if(segmClustCount[isect] > 3){
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EC[ic]->Fill(1, -1.*isect);
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EC[ic]->Fill(2, -1.*isect);
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EC[ic]->Fill(3, -1.*isect);
	      m_h2CSC_Segm_NumOfNClusSegs_Eta_EC[ic]->Fill(4, -1.*isect);
	    }
	  }

	  float eta_clus_qsum_tot = 0., phi_clus_qsum_tot = 0.; // total qsum over all prec. trans. layers on segment
	  for(unsigned int i = 0; i < 2; i++) {
	    eta_clus_qsum_tot = 0; phi_clus_qsum_tot = 0.;
	    for(unsigned int j = 1; j < 5; j++) {
	      if(i==0) m_h1CSC_Segm_QsumOfClus_Eta_EA[ic]->Fill(eta_clus_qsum[i][j]);
	      if(i==1) m_h1CSC_Segm_QsumOfClus_Eta_EC[ic]->Fill(eta_clus_qsum[i][j]);
	      if(i==0) m_h1CSC_Segm_QsumOfClus_Phi_EA[ic]->Fill(phi_clus_qsum[i][j]);
	      if(i==1) m_h1CSC_Segm_QsumOfClus_Phi_EC[ic]->Fill(phi_clus_qsum[i][j]);
	      if(i==0 && fabs(eta_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfClus_Eta_EA[ic]->Fill(eta_clus_time[i][j]);
	      if(i==1 && fabs(eta_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfClus_Eta_EC[ic]->Fill(eta_clus_time[i][j]);
	      if(i==0 && fabs(phi_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfClus_Phi_EA[ic]->Fill(phi_clus_time[i][j]);
	      if(i==1 && fabs(phi_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfClus_Phi_EC[ic]->Fill(phi_clus_time[i][j]);
	      if(phi_clus_use[i][j] && eta_clus_use[i][j]) {
		eta_clus_qsum_tot += eta_clus_qsum[i][j];
		if(i==0) m_h1CSC_Segm_QsumOfGoodClus_Eta_EA[ic]->Fill(eta_clus_qsum[i][j]);
		if(i==1) m_h1CSC_Segm_QsumOfGoodClus_Eta_EC[ic]->Fill(eta_clus_qsum[i][j]);
		if(i==0 && fabs(eta_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfGoodClus_Eta_EA[ic]->Fill(eta_clus_time[i][j]);
		if(i==1 && fabs(eta_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfGoodClus_Eta_EC[ic]->Fill(eta_clus_time[i][j]);
		phi_clus_qsum_tot += phi_clus_qsum[i][j];
		if(i==0) m_h1CSC_Segm_QsumOfGoodClus_Phi_EA[ic]->Fill(phi_clus_qsum[i][j]);
		if(i==1) m_h1CSC_Segm_QsumOfGoodClus_Phi_EC[ic]->Fill(phi_clus_qsum[i][j]);
		if(i==0 && fabs(phi_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfGoodClus_Phi_EA[ic]->Fill(phi_clus_time[i][j]);
		if(i==1 && fabs(phi_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfGoodClus_Phi_EC[ic]->Fill(phi_clus_time[i][j]);
	      }
	      //ATH_MSG_VERBOSE("phi charge = " << phi_clus_qsum[i][j] << "\tphi time = " << phi_clus_time[i][j]);
	      //ATH_MSG_VERBOSE("eta charge = " << eta_clus_qsum[i][j] << "\teta time = " << eta_clus_time[i][j]);
	    }
	    if(i==0) m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EA[ic]->Fill(eta_clus_qsum_tot,phi_clus_qsum_tot);
	    if(i==1) m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EC[ic]->Fill(eta_clus_qsum_tot,phi_clus_qsum_tot);
	    //ATH_MSG_VERBOSE("eta_qsum_tot = " << eta_clus_qsum_tot << "\tphi_qsum = " << phi_clus_qsum_tot);
	  }

	} // if is csc segment
        // } // loop over ROTs

      } // loop over segms

    } // loop over collections

  } // environment if

  if (m_debuglevel)  ATH_MSG_DEBUG(  " done with CSCSegmValAlg::fillHistograms()" );

  return sc;
}


//________________________________________________________________________________________________________
StatusCode CSCSegmValAlg::procHistograms() {

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

    if (m_debuglevel)  ATH_MSG_DEBUG(  "in CSCSegmValAlg::procHistograms()" );
  } // environment if

  return StatusCode::SUCCESS;   
}

//________________________________________________________________________________________________________
bool CSCSegmValAlg::evtSelTriggersPassed() {

  if(!m_doEvtSel) return true;
  std::vector<std::string>::const_iterator
    it = m_sampSelTriggers.begin(), itE = m_sampSelTriggers.end();
  for ( ; it != itE; it++ ) {
    if (m_trigDec->isPassed(*it, TrigDefs::eventAccepted)) {
      return true;
    }
  }
  return false;

} // end evtSelTriggersPassed 

//________________________________________________________________________________________________________
StatusCode CSCSegmValAlg::finalize() {
  StatusCode sc = ManagedMonitorToolBase::finalize();
  if(!sc.isSuccess()) return sc;
  return sc;

}


//________________________________________________________________________________________________________
bool CSCSegmValAlg::isCscSegment( const Muon::MuonSegment* seg ) const {
  bool isCsc(false);

  std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
  for( unsigned int i = 0; i< mbs.size(); ++i){

    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (mbs[i]);
    if (!rot){
      const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[i]);
      if (crot) rot  = &(crot->rioOnTrack(0));
    }  
    if( !rot ) {
      continue;
    }
    if( m_cscIdHelper->is_csc( rot->identify() ) ) isCsc=true;
  }

  return isCsc;
}


//________________________________________________________________________________________________________
unsigned int CSCSegmValAlg::cscHits( const Muon::MuonSegment* seg ) const {
  unsigned int nrHits(0);
  if( !isCscSegment(seg) ) return nrHits;

  std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
  for( unsigned int i = 0; i< mbs.size(); ++i){

    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (mbs[i]);
    if (!rot){
      const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[i]);
      if (crot) rot  = &(crot->rioOnTrack(0));
    }  
    if( !rot ) {
      continue;
    }
    if( m_cscIdHelper->is_csc( rot->identify() ) ) ++nrHits;
  }

  return nrHits ;
}

//________________________________________________________________________________________________________
void CSCSegmValAlg::setCSCLayerLabels(TH1 *h, int side) {

  if(!h) return;
  if(!(side == 1 || side == -1)) return;
  if(h->GetNbinsX() < 85) return;

  h->GetXaxis()->SetTitle("");
  h->GetXaxis()->SetLabelSize(0.03);
  if(side == -1) {
    for(unsigned int j=6; j<86; j++) {
      if( j%5 != 0 ) {
        float xmid = h->GetBinLowEdge(j) + h->GetBinWidth(j);
        xmid -= 1.0; xmid *= -1.0;
        int seclay = int(ceil(xmid*10)); // get sector/layer
        int sec = seclay/10;
        int lay = (seclay%10)/2 ;
        lay = (4 - lay) + 1;
        h->GetXaxis()->SetBinLabel(j,Form("%c%02d:%d",(sec%2==0?'S':'L'),sec,lay));
      } // end for
    } // end if
  } else if (side == 1) {
    for(unsigned int j=6; j<86; j++) {
      if( j%5 != 0 ) {
        float xmid = h->GetBinLowEdge(j) + h->GetBinWidth(j);
        int seclay = int(ceil(xmid*10)); // get sector/layer
        int sec = seclay/10;
        int lay = (seclay%10)/2 ;
        h->GetXaxis()->SetBinLabel(j,Form("%c%02d:%d",(sec%2==0?'S':'L'),sec,lay));
      }
    } // end for
  } // end else

} // setCSCLayerLabels 


//________________________________________________________________________________________________________
void CSCSegmValAlg::regCSCHist(TH1 *h, MonGroup *cm3 ) {
  StatusCode sc = StatusCode::SUCCESS;
  if(!cm3 || !h) return; 
  
  ATH_MSG_INFO (  "registering histogram: " << h->GetName() << " with group: " << cm3->system() );
  sc = cm3->regHist(h);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR (  "Failed to register histogram: " << h->GetName() << " with group: " << cm3->system() );
  }
  ATH_MSG_VERBOSE (  "registered histogram: " << h->GetName() << " with group: " << cm3->system() );
}

//________________________________________________________________________________________________________
void CSCSegmValAlg::clearHistogramVectors( ) {
  
  m_h2CSC_Segm_NumOfNClusSegs_Eta_EA.clear();
  m_h2CSC_Segm_NumOfNClusSegs_Eta_EC.clear();

  m_h1CSC_Segm_Efficiency_Eta_EA.clear();
  m_h1CSC_Segm_Efficiency_Eta_EC.clear();

  m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EA.clear();
  m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EC.clear();

  m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EA.clear();
  m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EC.clear();

  m_h1CSC_Segm_StatOfClus_Eta_EA.clear();
  m_h1CSC_Segm_NumOfClus_Eta_EA.clear();
  m_h1CSC_Segm_NumOfGoodClus_Eta_EA.clear();
  m_h1CSC_Segm_QsumOfClus_Eta_EA.clear();
  m_h1CSC_Segm_QsumOfGoodClus_Eta_EA.clear();
  m_h1CSC_Segm_TimeOfClus_Eta_EA.clear();
  m_h1CSC_Segm_TimeOfGoodClus_Eta_EA.clear();

  m_h1CSC_Segm_StatOfClus_Eta_EC.clear();
  m_h1CSC_Segm_NumOfClus_Eta_EC.clear();
  m_h1CSC_Segm_NumOfGoodClus_Eta_EC.clear();
  m_h1CSC_Segm_QsumOfClus_Eta_EC.clear();
  m_h1CSC_Segm_QsumOfGoodClus_Eta_EC.clear();
  m_h1CSC_Segm_TimeOfClus_Eta_EC.clear();
  m_h1CSC_Segm_TimeOfGoodClus_Eta_EC.clear();

  m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EA.clear();
  m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EC.clear();

  m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EA.clear();
  m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EC.clear();

  m_h1CSC_Segm_StatOfClus_Phi_EA.clear();
  m_h1CSC_Segm_NumOfClus_Phi_EA.clear();
  m_h1CSC_Segm_NumOfGoodClus_Phi_EA.clear();
  m_h1CSC_Segm_QsumOfClus_Phi_EA.clear();
  m_h1CSC_Segm_QsumOfGoodClus_Phi_EA.clear();
  m_h1CSC_Segm_TimeOfClus_Phi_EA.clear();
  m_h1CSC_Segm_TimeOfGoodClus_Phi_EA.clear();

  m_h1CSC_Segm_StatOfClus_Phi_EC.clear();
  m_h1CSC_Segm_NumOfClus_Phi_EC.clear();
  m_h1CSC_Segm_NumOfGoodClus_Phi_EC.clear();
  m_h1CSC_Segm_QsumOfClus_Phi_EC.clear();
  m_h1CSC_Segm_QsumOfGoodClus_Phi_EC.clear();
  m_h1CSC_Segm_TimeOfClus_Phi_EC.clear();
  m_h1CSC_Segm_TimeOfGoodClus_Phi_EC.clear();

  m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EA.clear();
  m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EC.clear();

  m_segmDetail_EA.clear();
  m_segmDetail_EC.clear();
  m_segmOview_EA.clear();
  m_segmOview_EC.clear();

}

//________________________________________________________________________________________________________
bool CSCSegmValAlg::segmSlopeCut(float& csc_x, float& csc_ax, float& cut ) {
  float s0 = csc_x;
  float s1 = -tan(csc_ax);
  float s1corr = s1 - 0.000119 * s0;
  bool good_segm = fabs(s1corr)<cut ? true : false;
  return good_segm;
}

