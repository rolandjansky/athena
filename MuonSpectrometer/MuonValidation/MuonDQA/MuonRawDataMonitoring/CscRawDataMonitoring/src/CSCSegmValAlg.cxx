/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

// Track
#include "TrkTrack/TrackCollection.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/PlaneSurface.h"


#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

// ROOT
#include "TH1F.h"
#include "TH2F.h"

// STL
#include <sstream>
#include <utility>
using CLHEP::cm3;

//________________________________________________________________________________________________________
CSCSegmValAlg::CSCSegmValAlg( const std::string & type, const std::string & name, const IInterface* parent ) 
  : ManagedMonitorToolBase( type, name, parent ), 
    m_trigDec( "" ),
    m_debuglevel(false),
    m_bookedhistos(false) {

  declareProperty( "SegmentSlopeCut",  m_segmSlope);
  declareProperty( "ClusterStatus",  m_clusStatWord);
    
  //trigger aware monitoring
  declareProperty("DoEventSelection",   m_doEvtSel = false );
  declareProperty("EventSelTriggers", m_sampSelTriggers );

  ATH_MSG_INFO( " in CSCSegmValAlg::CSCSegmValAlg() " );
}

//________________________________________________________________________________________________________
StatusCode CSCSegmValAlg::initialize() {

  m_bookedhistos=false;

  ATH_CHECK(ManagedMonitorToolBase::initialize());

  ATH_MSG_INFO( "in CSCSegmValAlg::init()" );
  if( m_doEvtSel ) {
    if (m_trigDec.retrieve().isFailure()) {
      ATH_MSG_WARNING ( "CSCSegmValAlg: Unable to retrieve trigger decision tool");
      m_doEvtSel = false;
    } else {
      ATH_MSG_INFO ( "TrigDecisionTool    : " << "Using TDT \"" << m_trigDec->name() << "\"" );
    }
  }

  if(m_sampSelTriggers.empty() && m_doEvtSel) {
    ATH_MSG_WARNING("Event selection triggers not specified. Switching off trigger-aware monitoring");
    m_doEvtSel = false;
  }


  // load defaults if items are empty or inconsistent
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

  StoreGateSvc* detStore = nullptr;
  ATH_CHECK(service("DetectorStore", detStore));
  ATH_CHECK(m_edmHelperSvc.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_segmKey.initialize());
  return StatusCode::SUCCESS;
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

  std::string segm_detailA = "Muon/MuonSegmentMonitoring/EndCapA/Detail/";
  std::string segm_detailC = "Muon/MuonSegmentMonitoring/EndCapC/Detail/";
  std::string segm_oviewA = "Muon/MuonSegmentMonitoring/EndCapA/Overview/";
  std::string segm_oviewC = "Muon/MuonSegmentMonitoring/EndCapC/Overview/";

  std::string theKey = m_segmKey.key();
    
  if (m_debuglevel)  ATH_MSG_DEBUG("Finished booking histos for " << theKey );

  float binmin = -0.5;
  int nbins = m_clusStatWord.size();
  float binmax = float(nbins) + binmin;
  
  MgmtAttr_t attr = ATTRIB_MANAGED;
  m_segmDetail_EA=std::unique_ptr<MonGroup>(new MonGroup(this, segm_detailA+theKey+"/CSC", run, attr ));
  m_segmDetail_EC=std::unique_ptr<MonGroup>(new MonGroup(this, segm_detailC+theKey+"/CSC", run, attr ));
  m_segmOview_EA=std::unique_ptr<MonGroup>(new MonGroup(this, segm_oviewA+theKey+"/CSC", run, attr ));
  m_segmOview_EC=std::unique_ptr<MonGroup>(new MonGroup(this, segm_oviewC+theKey+"/CSC", run, attr ));
  
  // segment hists
  m_h2CSC_Segm_NumOfSegs_EA= new TH2F("Muon_Segm_NumSegments_EA",
						Form("EndCap A: No. of segments; #segments;[sector] + [0.2 #times layer]"),
						nsegbins,nsegmin,nsegmax,nybinsEA,nyminEA,nymaxEA);
  m_h2CSC_Segm_NumOfSegs_EC= new TH2F("Muon_Segm_NumSegments_EC",
						Form("EndCap C: No. of segments; #segments;[sector] + [0.2 #times layer]"),
						nsegbins,nsegmin,nsegmax,nybinsEC,nyminEC,nymaxEC);
  regCSCHist(m_h2CSC_Segm_NumOfSegs_EA, m_segmOview_EA.get());
  regCSCHist(m_h2CSC_Segm_NumOfSegs_EC, m_segmOview_EC.get());
  
  // precision clusters on segment
  
  m_h2CSC_Segm_NumOfNClusSegs_Eta_EA=new TH2F("Muon_Segm_NumNClusSegments_EA",
							Form("EndCap A: #eta-number of 3 and 4 cluster segments with each layer;counts;[sector] + [0.2 #times layer]"),
							nclustbins,nclustmin,nclustmax,nybinsEA,nyminEA,nymaxEA);
  setCSCLayerLabels(m_h2CSC_Segm_NumOfNClusSegs_Eta_EA, 1);
  m_h2CSC_Segm_NumOfNClusSegs_Eta_EC=new TH2F("Muon_Segm_NumNClusSegments_EC",
							Form("EndCap C: #eta-number of 3 and 4 cluster segments with each layer;counts;[sector] + [0.2 #times layer]"),
							nclustbins,nclustmin,nclustmax,nybinsEC,nyminEC,nymaxEC);
  setCSCLayerLabels(m_h2CSC_Segm_NumOfNClusSegs_Eta_EC, -1);
  regCSCHist(m_h2CSC_Segm_NumOfNClusSegs_Eta_EA, m_segmDetail_EA.get());
  regCSCHist(m_h2CSC_Segm_NumOfNClusSegs_Eta_EC, m_segmDetail_EC.get());
  
  for (size_t j = 0; j < m_NClusWord.size(); j++) {
    m_h2CSC_Segm_NumOfNClusSegs_Eta_EA->GetXaxis()->SetBinLabel(j*10+5, m_NClusWord[j].c_str());
    m_h2CSC_Segm_NumOfNClusSegs_Eta_EC->GetXaxis()->SetBinLabel(j*10+5, m_NClusWord[j].c_str());
  }
  
  m_h1CSC_Segm_Efficiency_Eta_EA=new TH1F("Muon_Segm_Efficiency_EA",
						    Form("EndCap A: #eta-segment efficiency per sector;[sector] + [0.2 #times layer];efficiency"),
						    nybinsEA,nyminEA,nymaxEA);
  setCSCLayerLabels(m_h1CSC_Segm_Efficiency_Eta_EA, 1);
  m_h1CSC_Segm_Efficiency_Eta_EC=new TH1F("Muon_Segm_Efficiency_EC",
						    Form("EndCap C: #eta-segment efficiency per sector;[sector] + [0.2 #times layer];efficiency"),
						    nybinsEC,nyminEC,nymaxEC);
  setCSCLayerLabels(m_h1CSC_Segm_Efficiency_Eta_EC, -1);
  
  regCSCHist(m_h1CSC_Segm_Efficiency_Eta_EA, m_segmOview_EA.get());
  regCSCHist(m_h1CSC_Segm_Efficiency_Eta_EC, m_segmOview_EC.get());
  
  m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EA=new TH2F("Muon_Segm_QSumEtaGoodClusPerLayer_EA",
							   Form("EndCap A: #eta-cluster charge per layer;counts;[sector] + [0.2 #times layer]"),
							   nqbins,nqmin,nqmax,nybinsEA,nyminEA,nymaxEA);
  
  m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EC=new TH2F("Muon_Segm_QSumEtaGoodClusPerLayer_EC",
							   Form("EndCap C: #eta-cluster charge per layer;counts;[sector] + [0.2 #times layer]"),
							   nqbins,nqmin,nqmax,nybinsEC,nyminEC,nymaxEC);
  
  regCSCHist(m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EA, m_segmOview_EA.get());
  regCSCHist(m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EC, m_segmOview_EC.get());
  
  m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EA=new TH2F("Muon_Segm_TimeEtaGoodClusPerLayer_EA",
							   Form("EndCap A: #eta-cluster time per layer;time [ns];[sector] + [0.2 #times layer]"),
							   ntbins,ntmin,ntmax,nybinsEA,nyminEA,nymaxEA);
  
  m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EC=new TH2F("Muon_Segm_TimeEtaGoodClusPerLayer_EC",
							   Form("EndCap C: #eta-cluster time per layer;time [ns];[sector] + [0.2 #times layer]"),
							   ntbins,ntmin,ntmax,nybinsEC,nyminEC,nymaxEC);
  
  regCSCHist(m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EA, m_segmOview_EA.get());
  regCSCHist(m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EC, m_segmOview_EC.get());
  
  
  m_h1CSC_Segm_StatOfClus_Eta_EA= new TH1F("Muon_Segm_EtaClusterStatus_EA", 
						     "Endcap A: #eta-cluster status;;entries",nbins,binmin,binmax);
  
  m_h1CSC_Segm_StatOfClus_Eta_EC= new TH1F("Muon_Segm_EtaClusterStatus_EC", 
						     "Endcap C: #eta-cluster status;;entries",nbins,binmin,binmax);
  for (size_t j = 0; j < m_clusStatWord.size(); j++) {
    m_h1CSC_Segm_StatOfClus_Eta_EA->GetXaxis()->SetBinLabel(j+1, m_clusStatWord[j].c_str());
    m_h1CSC_Segm_StatOfClus_Eta_EC->GetXaxis()->SetBinLabel(j+1, m_clusStatWord[j].c_str());
  }
  regCSCHist(m_h1CSC_Segm_StatOfClus_Eta_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_StatOfClus_Eta_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_NumOfClus_Eta_EA= new TH1F("Muon_Segm_NumEtaCluster_EA", 
						    "Endcap A: No. of #eta-clusters on segment;#clusters;entries", nsbins,nsmin,nsmax);
  m_h1CSC_Segm_NumOfClus_Eta_EC= new TH1F("Muon_Segm_NumEtaCluster_EC", 
						    "Endcap C: No. of #eta-clusters on segment;#clusters;entries", nsbins,nsmin,nsmax);
  regCSCHist(m_h1CSC_Segm_NumOfClus_Eta_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_NumOfClus_Eta_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_NumOfGoodClus_Eta_EA= new TH1F("Muon_Segm_NumGoodEtaCluster_EA",
							"Endcap A: No. of good #eta-clusters on segment;#good-clusters;entries", nsbins,nsmin,nsmax);
  m_h1CSC_Segm_NumOfGoodClus_Eta_EC= new TH1F("Muon_Segm_NumGoodEtaCluster_EC",
							"Endcap C: No. of good #eta-clusters on segment;#good-clusters;entries", nsbins,nsmin,nsmax);
  regCSCHist(m_h1CSC_Segm_NumOfGoodClus_Eta_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_NumOfGoodClus_Eta_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_QsumOfClus_Eta_EA= new TH1F("Muon_Segm_QSumEtaCluster_EA", 
						     "Endcap A: #eta-cluster Qsum;counts;entries", nqbins,nqmin,nqmax );
  m_h1CSC_Segm_QsumOfClus_Eta_EC= new TH1F("Muon_Segm_QSumEtaCluster_EC", 
						     "Endcap C: #eta-cluster Qsum;counts;entries", nqbins,nqmin,nqmax );
  regCSCHist(m_h1CSC_Segm_QsumOfClus_Eta_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_QsumOfClus_Eta_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_TimeOfClus_Eta_EA= new TH1F("Muon_Segm_TimeEtaCluster_EA", 
						     "Endcap A: #eta-cluster Time;time [ns];entries", ntbins,ntmin,ntmax );
  m_h1CSC_Segm_TimeOfClus_Eta_EC= new TH1F("Muon_Segm_TimeEtaCluster_EC", 
						     "Endcap C: #eta-cluster Time;time [ns];entries", ntbins,ntmin,ntmax );
  regCSCHist(m_h1CSC_Segm_TimeOfClus_Eta_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_TimeOfClus_Eta_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_QsumOfGoodClus_Eta_EA= new TH1F("Muon_Segm_QSumGoodEtaCluster_EA", 
							 "Endcap A: Good #eta-cluster Qsum;counts;entries", nqbins,nqmin,nqmax );
  m_h1CSC_Segm_QsumOfGoodClus_Eta_EC= new TH1F("Muon_Segm_QSumGoodEtaCluster_EC", 
							 "Endcap C: Good #eta-cluster Qsum;counts;entries", nqbins,nqmin,nqmax );
  regCSCHist(m_h1CSC_Segm_QsumOfGoodClus_Eta_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_QsumOfGoodClus_Eta_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_TimeOfGoodClus_Eta_EA= new TH1F("Muon_Segm_TimeGoodEtaCluster_EA", 
							 "Endcap A: Good #eta-cluster Time;time [ns];entries", ntbins,ntmin,ntmax );
  m_h1CSC_Segm_TimeOfGoodClus_Eta_EC= new TH1F("Muon_Segm_TimeGoodEtaCluster_EC", 
							 "Endcap C: Good #eta-cluster Time;time [ns];entries", ntbins,ntmin,ntmax );
  regCSCHist(m_h1CSC_Segm_TimeOfGoodClus_Eta_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_TimeOfGoodClus_Eta_EC, m_segmDetail_EC.get());
  
  
  // transverse clusters on segment
  m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EA=new TH2F("Muon_Segm_QSumPhiGoodClusPerLayer_EA",
							   Form("EndCap A: #phi-cluster charge per layer;counts;[sector] + [0.2 #times layer]"),
							   nqbins,nqmin,nqmax,nybinsEA,nyminEA,nymaxEA);
  
  m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EC=new TH2F("Muon_Segm_QSumPhiGoodClusPerLayer_EC",
							   Form("EndCap C: #phi-cluster charge per layer;counts;[sector] + [0.2 #times layer]"),
							   nqbins,nqmin,nqmax,nybinsEC,nyminEC,nymaxEC);
  
  regCSCHist(m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EA, m_segmOview_EA.get());
  regCSCHist(m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EC, m_segmOview_EC.get());
  
  m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EA=new TH2F("Muon_Segm_TimePhiGoodClusPerLayer_EA",
							   Form("EndCap A: #phi-cluster time per layer;time [ns];[sector] + [0.2 #times layer]"),
							   ntbins,ntmin,ntmax,nybinsEA,nyminEA,nymaxEA);
  
  m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EC=new TH2F("Muon_Segm_TimePhiGoodClusPerLayer_EC",
							   Form("EndCap C: #phi-cluster time per layer;time [ns];[sector] + [0.2 #times layer]"),
							   ntbins,ntmin,ntmax,nybinsEC,nyminEC,nymaxEC);
  
  regCSCHist(m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EA, m_segmOview_EA.get());
  regCSCHist(m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EC, m_segmOview_EC.get());
  
  m_h1CSC_Segm_StatOfClus_Phi_EA= new TH1F("Muon_Segm_PhiClusterStatus_EA", 
						     "Endcap A: #phi-cluster status;;entries",nbins,binmin,binmax);
  
  m_h1CSC_Segm_StatOfClus_Phi_EC= new TH1F("Muon_Segm_PhiClusterStatus_EC", 
						     "Endcap C: #phi-cluster status;;entries",nbins,binmin,binmax);
  
  for (size_t j = 0; j < m_clusStatWord.size(); j++) {
    m_h1CSC_Segm_StatOfClus_Phi_EA->GetXaxis()->SetBinLabel(j+1, m_clusStatWord[j].c_str());
    m_h1CSC_Segm_StatOfClus_Phi_EC->GetXaxis()->SetBinLabel(j+1, m_clusStatWord[j].c_str());
  }
  regCSCHist(m_h1CSC_Segm_StatOfClus_Phi_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_StatOfClus_Phi_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_NumOfClus_Phi_EA= new TH1F("Muon_Segm_NumPhiCluster_EA", 
						    "Endcap A: No. of #phi-clusters on segment;#clusters;entries", nsbins,nsmin,nsmax);
  m_h1CSC_Segm_NumOfClus_Phi_EC= new TH1F("Muon_Segm_NumPhiCluster_EC", 
						    "Endcap C: No. of #phi-clusters on segment;#clusters;entries", nsbins,nsmin,nsmax);
  regCSCHist(m_h1CSC_Segm_NumOfClus_Phi_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_NumOfClus_Phi_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_NumOfGoodClus_Phi_EA= new TH1F("Muon_Segm_NumGoodPhiCluster_EA",
							"Endcap A: No. of good #phi-clusters on segment;#good-clusters;entries", nsbins,nsmin,nsmax);
  m_h1CSC_Segm_NumOfGoodClus_Phi_EC= new TH1F("Muon_Segm_NumGoodPhiCluster_EC",
							"Endcap C: No. of good #phi-clusters on segment;#good-clusters;entries", nsbins,nsmin,nsmax);
  regCSCHist(m_h1CSC_Segm_NumOfGoodClus_Phi_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_NumOfGoodClus_Phi_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_QsumOfClus_Phi_EA= new TH1F("Muon_Segm_QSumPhiCluster_EA", 
						     "Endcap A: #phi-cluster Qsum;counts;entries", nqbins,nqmin,nqmax );
  m_h1CSC_Segm_QsumOfClus_Phi_EC= new TH1F("Muon_Segm_QSumPhiCluster_EC", 
						     "Endcap C: #phi-cluster Qsum;counts;entries", nqbins,nqmin,nqmax );
  regCSCHist(m_h1CSC_Segm_QsumOfClus_Phi_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_QsumOfClus_Phi_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_TimeOfClus_Phi_EA= new TH1F("Muon_Segm_TimePhiCluster_EA", 
						     "Endcap A: #phi-cluster Time;time [ns];entries", ntbins,ntmin,ntmax );
  m_h1CSC_Segm_TimeOfClus_Phi_EC= new TH1F("Muon_Segm_TimePhiCluster_EC", 
						     "Endcap C: #phi-cluster Time;time [ns];entries", ntbins,ntmin,ntmax );
  regCSCHist(m_h1CSC_Segm_TimeOfClus_Phi_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_TimeOfClus_Phi_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_QsumOfGoodClus_Phi_EA= new TH1F("Muon_Segm_QSumGoodPhiCluster_EA", 
							 "Endcap A: Good #phi-cluster Qsum;counts;entries", nqbins,nqmin,nqmax );
  m_h1CSC_Segm_QsumOfGoodClus_Phi_EC= new TH1F("Muon_Segm_QSumGoodPhiCluster_EC", 
							 "Endcap C: Good #phi-cluster Qsum;counts;entries", nqbins,nqmin,nqmax );
  regCSCHist(m_h1CSC_Segm_QsumOfGoodClus_Phi_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_QsumOfGoodClus_Phi_EC, m_segmDetail_EC.get());
  
  m_h1CSC_Segm_TimeOfGoodClus_Phi_EA= new TH1F("Muon_Segm_TimeGoodPhiCluster_EA", 
							 "Endcap A: Good #phi-cluster Time;time [ns];entries", ntbins,ntmin,ntmax );
  m_h1CSC_Segm_TimeOfGoodClus_Phi_EC= new TH1F("Muon_Segm_TimeGoodPhiCluster_EC", 
							 "Endcap C: Good #phi-cluster Time;time [ns];entries", ntbins,ntmin,ntmax );
  regCSCHist(m_h1CSC_Segm_TimeOfGoodClus_Phi_EA, m_segmDetail_EA.get());
  regCSCHist(m_h1CSC_Segm_TimeOfGoodClus_Phi_EC, m_segmDetail_EC.get());
  
  // eta vs. phi correlation 
  m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EA= new TH2F("Muon_Segm_QSumGoodClusCorrelation_EA", 
							      "Endcap A: #phi-cluster vs. good #eta-cluster;good #eta-cluster counts;good #phi-cluster counts", nqbins,nqmin,nqmax, nqbins,nqmin,nqmax);
  m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EC= new TH2F("Muon_Segm_QSumGoodClusCorrelation_EC", 
							      "Endcap A: #phi-cluster vs. good #eta-cluster;good #eta-cluster counts;good #phi-cluster counts", nqbins,nqmin,nqmax, nqbins,nqmin,nqmax);
  regCSCHist(m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EA, m_segmDetail_EA.get());
  regCSCHist(m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EC, m_segmDetail_EC.get());

  m_bookedhistos=true;
}


//________________________________________________________________________________________________________
StatusCode CSCSegmValAlg::bookHistograms() {

  if (m_debuglevel) ATH_MSG_DEBUG( "CSCSegmValAlg::bookHistograms() to setup tools/services" );
  StatusCode sc = StatusCode::SUCCESS;

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

    if(!m_bookedhistos) bookSegmentHistograms();

  } // environment if  
  return sc;
}


//________________________________________________________________________________________________________
StatusCode CSCSegmValAlg::fillHistograms() {

  ATH_MSG_DEBUG( "in CSCSegmValAlg::fillHistograms()                             " );

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ||  m_environment == AthenaMonManager::user ) {

    // if required, check if event passed sample-selection triggers
    if(m_doEvtSel) { if(!evtSelTriggersPassed()) return StatusCode::SUCCESS; }

    // Segment Cluster counter
    int segmClustCount[33];

    // arrays to hold cluster-count  
    // 32 chambers and 8 layers (each has one extra - index '0' is not counted)
    int clusCount[33][9];
    for(unsigned int kl = 0; kl < 33; kl++ ) {
      for(unsigned int cm3 = 0; cm3 < 9; cm3++ ) {
        clusCount[kl][cm3] = 0;
      }
    }


    SG::ReadHandle<Trk::SegmentCollection> segments(m_segmKey);

    if ( segments->empty() ){
      
      if (m_debuglevel){
	ATH_MSG_DEBUG( "      Segm Collection is Empty, done...    ");
      }
      return StatusCode::SUCCESS;
    }

    if (m_debuglevel){
      ATH_MSG_DEBUG( "      Segm Collection size         " );
      ATH_MSG_DEBUG( "Number of segms found: " << segments->size() );
    }

    if ( segments->size() > 0 ){
      ATH_MSG_DEBUG(   "Number of segms in event is: " << segments->size()                               );
      ATH_MSG_DEBUG(  "This algorithm is designed to operate for single segm / event only"             );
      ATH_MSG_DEBUG(  "Processing only the first segm in the segm collection"                         );
    }

    int segnum = 0;
    int layerindex = 0;

    //Loop over segms
    for (Trk::SegmentCollection::const_iterator s = segments->begin();s != segments->end(); ++s) {

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
	  ATH_MSG_DEBUG( "Ndof " << ndof );
	}
	
	// cut on segment angle
	float segm_ly = segm->localParameters()[Trk::locY];
	float segm_ayz = segm->localDirection().angleYZ();
	segm_ayz -= M_PI/2.;
	float segm_cut = m_segmSlope;
	bool segmAngle_cut = segmSlopeCut(segm_ly, segm_ayz, segm_cut);
	ATH_MSG_DEBUG(" local_pos: " << segm_ly << 
		      "\tangle_yz: " << segm_ayz << "\tcut: " << segm_cut << "\t pass = " << segmAngle_cut );
	
	
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
	Identifier segmId = m_edmHelperSvc->getIdentifier(*rio);
	
	int segm_stationPhi  = m_idHelperSvc->cscIdHelper().stationPhi(segmId);
	int segm_stationEta  = m_idHelperSvc->cscIdHelper().stationEta(segmId);
	int segm_stationName = m_idHelperSvc->cscIdHelper().stationName(segmId);
	std::string segm_stationString = m_idHelperSvc->cscIdHelper().stationNameString(segm_stationName);
	int segm_chamberType = segm_stationString == "CSS" ? 0 : 1;
	int segm_sectorNo  = segm_stationEta * (2 * segm_stationPhi - segm_chamberType); // [-16 -> -1] and [+1 -> +16]
	int segm_isec = segm_sectorNo < 0 ? segm_sectorNo*(-1) : segm_sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
	ATH_MSG_DEBUG(" sgsec = " << segm_isec << "\tsec = " << segm_sectorNo);
	if(segm_stationEta == 1) m_h2CSC_Segm_NumOfSegs_EA->Fill(n_clust, segm_sectorNo);
	else m_h2CSC_Segm_NumOfSegs_EC->Fill(n_clust, segm_sectorNo);
	
	// Loop over clusters
	float clus_kiloele = 1.0e-3; // multiply # of electrons by this number to get kiloElectrons (1 ke = 1 ADC)
	int eta_clus_count[2][2] = {{0},{0}}, phi_clus_count[2][2] = {{0},{0}}; // no. of prec/trans hits per segment
	float eta_clus_qsum[2][5] = {{-1.}, {-1.}},  phi_clus_qsum[2][5] = {{-1.}, {-1.}}; // qsum over each prec/trans. layer on segment
	float eta_clus_time[2][5] = {{-1.}, {-1.}},  phi_clus_time[2][5] = {{-1.}, {-1.}}; // time over each prec/trans. layer on segment
	int eta_clus_use[2][5] = {{0},{0}}, phi_clus_use[2][5] = {{0}, {0}};
	
	layerindex = 0;
        
	for(TrkSegmIter hit = segm->containedMeasurements().begin(); 
	    hit != segm->containedMeasurements().end(); ++hit ) {
	  const Muon::CscClusterOnTrack* clust_rot = dynamic_cast<const Muon::CscClusterOnTrack*>(*hit);
	  if( clust_rot ) {
	    Identifier clusId = m_edmHelperSvc->getIdentifier(*clust_rot);
	    
	    // get the cluster coordinates
	    int clus_stationName = m_idHelperSvc->cscIdHelper().stationName(clusId);
	    std::string clus_stationString = m_idHelperSvc->cscIdHelper().stationNameString(clus_stationName);
	    int clus_chamberType = clus_stationString == "CSS" ? 0 : 1;
	    int clus_stationEta  = m_idHelperSvc->cscIdHelper().stationEta(clusId);
	    int clus_stationPhi  = m_idHelperSvc->cscIdHelper().stationPhi(clusId);
	    int clus_wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(clusId);
	    int clus_measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(clusId);
	    
	    // convert to my coordinates
	    int clus_sectorNo  = clus_stationEta * (2 * clus_stationPhi - clus_chamberType);   // [-16 -> -1] and [+1 -> +16]
	    float clus_secLayer = clus_sectorNo + 0.2 * (clus_wireLayer - 1) + 0.1;
	    int clus_isec = clus_sectorNo < 0 ? clus_sectorNo*(-1) : clus_sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
	    int clus_ilay = (clus_measuresPhi ? clus_wireLayer : clus_wireLayer+4);
	    
	    // check the cluster status; probably need to read status info from jobOptions - not done for the moment
	    Muon::CscClusterStatus status = clust_rot->status();
	    if(segm_stationEta == 1) {
	      if(clus_measuresPhi == 0) m_h1CSC_Segm_StatOfClus_Eta_EA->Fill(status);
	      else m_h1CSC_Segm_StatOfClus_Phi_EA->Fill(status);
	    } else {
	      if(clus_measuresPhi == 0) m_h1CSC_Segm_StatOfClus_Eta_EC->Fill(status);
	      else m_h1CSC_Segm_StatOfClus_Phi_EC->Fill(status);
	    }

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
	    
	    if(theClus) {
	      clus_qsum = theClus->charge() * clus_kiloele;
	      clus_time = theClus->time();

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
		m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EA->Fill(clus_qsum, clus_secLayer);
		if(std::abs(clus_time) <= 200) m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EA->Fill(clus_time, clus_secLayer);
	      } else {
		eta_clus_qsum[1][clus_wireLayer] = clus_qsum;
		eta_clus_time[1][clus_wireLayer] = clus_time;
		eta_clus_use[1][clus_wireLayer] = 1;
		m_h2CSC_Segm_QsumOfGoodClusMap_Eta_EC->Fill(clus_qsum, clus_secLayer);
		if(std::abs(clus_time) <= 200) m_h2CSC_Segm_TimeOfGoodClusMap_Eta_EC->Fill(clus_time, clus_secLayer);
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
		m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EA->Fill(clus_qsum, clus_secLayer);
		if(std::abs(clus_time) <= 200) m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EA->Fill(clus_time, clus_secLayer);
	      } else {
		phi_clus_qsum[1][clus_wireLayer] = clus_qsum;
		phi_clus_time[1][clus_wireLayer] = clus_time;
		phi_clus_use[1][clus_wireLayer] = 1;
		phi_clus_count[1][1]++;
		m_h2CSC_Segm_QsumOfGoodClusMap_Phi_EC->Fill(clus_qsum, clus_secLayer);
		if(std::abs(clus_time) <= 200) m_h2CSC_Segm_TimeOfGoodClusMap_Phi_EC->Fill(clus_time, clus_secLayer);
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
        
	if(eta_clus_count[0][0] > 0) m_h1CSC_Segm_NumOfClus_Eta_EA->Fill(eta_clus_count[0][0]);
	if(phi_clus_count[0][0] > 0) m_h1CSC_Segm_NumOfClus_Phi_EA->Fill(phi_clus_count[0][0]);
	if(eta_clus_count[0][1] > 0) m_h1CSC_Segm_NumOfGoodClus_Eta_EA->Fill(eta_clus_count[0][1]);
	if(phi_clus_count[0][1] > 0) m_h1CSC_Segm_NumOfGoodClus_Phi_EA->Fill(phi_clus_count[0][1]);
	if(eta_clus_count[1][0] > 0) m_h1CSC_Segm_NumOfClus_Eta_EC->Fill(eta_clus_count[1][0]);
	if(phi_clus_count[1][0] > 0) m_h1CSC_Segm_NumOfClus_Phi_EC->Fill(phi_clus_count[1][0]);
	if(eta_clus_count[1][1] > 0) m_h1CSC_Segm_NumOfGoodClus_Eta_EC->Fill(eta_clus_count[1][1]);
	if(phi_clus_count[1][1] > 0) m_h1CSC_Segm_NumOfGoodClus_Phi_EC->Fill(phi_clus_count[1][1]);
	
	// Fill number of 3 and 4 cluster segment histogram
	for (int isect = 1; isect < 17; isect++) {
	  if(segmClustCount[isect+16] > 2){
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EA->Fill(layerindex-5, isect);
	  }
	  if(segmClustCount[isect] > 2){
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EC->Fill(layerindex-5, (-1.)*isect);
	  }
	  if(segmClustCount[isect+16] > 3){
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EA->Fill(1, isect);
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EA->Fill(2, isect);
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EA->Fill(3, isect);
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EA->Fill(4, isect);
	  }
	  if(segmClustCount[isect] > 3){
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EC->Fill(1, -1.*isect);
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EC->Fill(2, -1.*isect);
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EC->Fill(3, -1.*isect);
	    m_h2CSC_Segm_NumOfNClusSegs_Eta_EC->Fill(4, -1.*isect);
	  }
	}
	
	float eta_clus_qsum_tot = 0., phi_clus_qsum_tot = 0.; // total qsum over all prec. trans. layers on segment
	for(unsigned int i = 0; i < 2; i++) {
	  eta_clus_qsum_tot = 0; phi_clus_qsum_tot = 0.;
	  for(unsigned int j = 1; j < 5; j++) {
	    if(i==0) m_h1CSC_Segm_QsumOfClus_Eta_EA->Fill(eta_clus_qsum[i][j]);
	    if(i==1) m_h1CSC_Segm_QsumOfClus_Eta_EC->Fill(eta_clus_qsum[i][j]);
	    if(i==0) m_h1CSC_Segm_QsumOfClus_Phi_EA->Fill(phi_clus_qsum[i][j]);
	    if(i==1) m_h1CSC_Segm_QsumOfClus_Phi_EC->Fill(phi_clus_qsum[i][j]);
	    if(i==0 && std::abs(eta_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfClus_Eta_EA->Fill(eta_clus_time[i][j]);
	    if(i==1 && std::abs(eta_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfClus_Eta_EC->Fill(eta_clus_time[i][j]);
	    if(i==0 && std::abs(phi_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfClus_Phi_EA->Fill(phi_clus_time[i][j]);
	    if(i==1 && std::abs(phi_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfClus_Phi_EC->Fill(phi_clus_time[i][j]);
	    if(phi_clus_use[i][j] && eta_clus_use[i][j]) {
	      eta_clus_qsum_tot += eta_clus_qsum[i][j];
	      if(i==0) m_h1CSC_Segm_QsumOfGoodClus_Eta_EA->Fill(eta_clus_qsum[i][j]);
	      if(i==1) m_h1CSC_Segm_QsumOfGoodClus_Eta_EC->Fill(eta_clus_qsum[i][j]);
	      if(i==0 && std::abs(eta_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfGoodClus_Eta_EA->Fill(eta_clus_time[i][j]);
	      if(i==1 && std::abs(eta_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfGoodClus_Eta_EC->Fill(eta_clus_time[i][j]);
	      phi_clus_qsum_tot += phi_clus_qsum[i][j];
	      if(i==0) m_h1CSC_Segm_QsumOfGoodClus_Phi_EA->Fill(phi_clus_qsum[i][j]);
	      if(i==1) m_h1CSC_Segm_QsumOfGoodClus_Phi_EC->Fill(phi_clus_qsum[i][j]);
	      if(i==0 && std::abs(phi_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfGoodClus_Phi_EA->Fill(phi_clus_time[i][j]);
	      if(i==1 && std::abs(phi_clus_time[i][j]) <= 200) m_h1CSC_Segm_TimeOfGoodClus_Phi_EC->Fill(phi_clus_time[i][j]);
	    }
	  }
	  if(i==0) m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EA->Fill(eta_clus_qsum_tot,phi_clus_qsum_tot);
	  if(i==1) m_h2CSC_Segm_QsumOfGoodClus_PhiVsEta_EC->Fill(eta_clus_qsum_tot,phi_clus_qsum_tot);
	}
	
      } // if is csc segment
      
    } // loop over segms

  } // environment if

  if (m_debuglevel)  ATH_MSG_DEBUG(  " done with CSCSegmValAlg::fillHistograms()" );

  return StatusCode::SUCCESS;
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
    if( m_idHelperSvc->isCsc( rot->identify() ) ) isCsc=true;
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
    if( m_idHelperSvc->isCsc( rot->identify() ) ) ++nrHits;
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
bool CSCSegmValAlg::segmSlopeCut(float& csc_x, float& csc_ax, float& cut ) {
  float s0 = csc_x;
  float s1 = -tan(csc_ax);
  float s1corr = s1 - 0.000119 * s0;
  bool good_segm = std::abs(s1corr)<cut ? true : false;
  return good_segm;
}

