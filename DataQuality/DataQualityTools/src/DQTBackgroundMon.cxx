/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTBackgroundMon
// PACKAGE:  DataQualityTools
// AUTHORS:  I. Chalupkova <Ina.Chalupkova@cern.ch>
//           D. Salek <David.Salek@cern.ch>
// based on code from A.Canepa and M.Tibbetts
//
// ********************************************************************


#include "DataQualityTools/DQTBackgroundMon.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
//#include "xAODCaloEvent/CaloVariableType.h"
//#include "xAODCaloEvent/CaloCellContainer.h"
#include "xAODJet/Jet.h"
//#include "JetUtils/JetCaloQualityUtils.h"
#include "xAODTracking/Vertex.h"

using xAOD::Jet;
using xAOD::JetContainer;
using xAOD::JetAttribute;
using xAOD::CaloCluster;
using xAOD::EventInfo;

//----------------------------------------------------------------------------------
DQTBackgroundMon::DQTBackgroundMon(const std::string & type,
					   const std::string & name,
					   const IInterface* parent) :
  DataQualityFatherMonTool(type, name, parent),
  m_trigDec("TrigDecisionTool"),
  m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
  m_idHelperTool("Muon::MuonIdHelperTool"),
  m_idToFixedIdTool("MuonCalib::IdToFixedIdTool"),
  m_doMuons(1),
  m_doTrigger(1),
  m_FilledBG(0),
  m_EmptyBG(0),
  m_UnpairIsoBG(0),
  m_UnpairNonIsoBG(0),
  m_LArEC_SideCut(0),
  m_MBTS_SideCut(0),
  m_MBTS_TimeCut(0.0),
  m_MBTS_ThresholdCut(0.0),
  m_mbtsContainerName("MBTSContainer"),
  m_MBTS_mask(TileCell::MASK_BADCH | TileCell::MASK_OVER | TileCell::MASK_TIME),
  m_MBTS_pattern(TileCell::MASK_TIME),
  m_nBins_PixSP(50),
  m_nBins_SctSP(50),
  m_nBins_SPHuge(50),
  m_nBins_TimeDiff(50),
  m_Up_PixSP(1000.0),
  m_Up_SctSP(1000.0),
  m_Up_SPHuge(1000000.0),
  m_Low_TimeDiff(-50.0),
  m_Up_TimeDiff(50.0),
  m_nBkgWords(32)
{
  declareProperty("histoPath",        m_path           = "GLOBAL/DQTBackgroundMon");
  //declareProperty("doRunCosmics",     m_doRunCosmics   = 1);
  //declareProperty("doRunBeam",        m_doRunBeam      = 1);
  //declareProperty("doOfflineHists",   m_doOfflineHists = 1);
  //declareProperty("doOnlineHists",    m_doOnlineHists  = 1);
  declareProperty("doMuons",          m_doMuons        = 1);
  declareProperty("doTrigger",        m_doTrigger      = 1);

  declareProperty("FilledBGIdx",      m_FilledBG      =1);
  declareProperty("EmptyBGIdx",       m_EmptyBG       =3);
  declareProperty("UnpairIsoBGIdx",   m_UnpairIsoBG   =4);
  declareProperty("UnpairNonIsoBGIdx",m_UnpairNonIsoBG=5);

  declareProperty("LArEC_SideCut",    m_LArEC_SideCut = 1  );
  declareProperty("MBTS_SideCut",     m_MBTS_SideCut  = 2  );
  declareProperty("MBTS_TimeCut",     m_MBTS_TimeCut  = 15.);
  declareProperty("MBTS_ThresholdCut",m_MBTS_ThresholdCut = 40.0/222.0); //Value in pC

  declareProperty("nBins_PixSP",   m_nBins_PixSP    = 200);
  declareProperty("nBins_SctSP",   m_nBins_SctSP    = 200);
  declareProperty("nBins_SPHuge",  m_nBins_SPHuge   = 200);
  declareProperty("nBins_TimeDiff",m_nBins_TimeDiff = 100);

  declareProperty("Up_PixSP",      m_Up_PixSP     = 5000.0);
  declareProperty("Up_SctSP",      m_Up_SctSP     = 10000.0);
  declareProperty("Up_SPHuge",     m_Up_SPHuge    = 1000000.0);
  declareProperty("Low_TimeDiff",  m_Low_TimeDiff =-50.0);
  declareProperty("Up_TimeDiff",   m_Up_TimeDiff  = 50.0);
  declareProperty("nBkgWords",     m_nBkgWords    = 32);
  
  declareProperty("LocalTrigDecTool", m_trigDec);
}

//----------------------------------------------------------------------------------
DQTBackgroundMon::~DQTBackgroundMon()
//----------------------------------------------------------------------------------
{
}

StatusCode DQTBackgroundMon::initialize()
{
  
  ATH_CHECK( m_EventInfoKey.initialize() );
  ATH_CHECK( m_RawInfoSummaryForTagKey.initialize() );
  ATH_CHECK( m_LArCollisionTimeKey.initialize() );
  ATH_CHECK( m_MBTSCollisionTimeKey.initialize() );
  ATH_CHECK( m_TileCellContainerKey.initialize() );
  ATH_CHECK( m_LUCID_RawDataContainerKey.initialize() );
  ATH_CHECK( m_SegmentCollectionKey.initialize() );
  ATH_CHECK( m_BeamBackgroundDataKey.initialize() );
  ATH_CHECK( m_VertexContainerKey.initialize() );

  return DataQualityFatherMonTool::initialize();
}

//----------------------------------------------------------------------------------
StatusCode DQTBackgroundMon::bookHistograms()
//StatusCode DQTBackgroundMon::bookHistograms(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
//----------------------------------------------------------------------------------
{
  bool failure(false);

  ATH_MSG_INFO( "In bookHistograms(): " <<  "m_doRunCosmics = " << m_doRunCosmics << "; m_doRunBeam = "<< m_doRunBeam << "; m_doMuons = "<< m_doMuons );
    //This will do the real booking
    failure = bookDQTBackgroundMon();

  if (failure)    {
    return StatusCode::FAILURE;
  }
  else    {
    return StatusCode::SUCCESS;
  }

}

//----------------------------------------------------------------------------------
bool DQTBackgroundMon::bookDQTBackgroundMon()
//----------------------------------------------------------------------------------
{
  bool failure(false);

  std::string  fullPath = m_path+"/BackgroundMon";
  std::string labels[32]={"MBTSTimeDiffHalo", "MBTSTimeDiffCol",
						  "LArECTimeDiffHalo", "LArECTimeDiffCol", 
						  "PixMultiplicityHuge", "PixSPNonEmpty",
						  "SCTMultiplicityHuge", "SCTSPNonEmpty",
						  "CSCTimeDiffHalo", "CSCTimeDiffCol",
						  "BCMTimeDiffHalo", "BCMTimeDiffCol", 
						  "MuonTimingCol", "MuonTimingCosmic",
						  "MBTSBeamVeto",
						  "BCMBeamVeto",
						  "LUCIDBeamVeto",
						  "HaloMuonSegment", "HaloClusterShape",
						  "HaloMuonOneSided", "HaloMuonTwoSided",
						  "HaloTileClusterPattern",
						  "BeamGasPixel",
						  "CosmicStandAlone", "CosmicStandAloneTight",
						  "CosmicCombined", "CosmicCombinedTight",
						  "BkgdResvBit1", "BkgdResvBit2",
						  "BkgdResvBit3", "BkgdResvBit4",
						  "BkgdResvBit5"};

  // Background word histograms  
  failure = failure | registerHist(fullPath,
				   m_HistBitSet = TH1F_LW::create("m_BitSet","Background Word",m_nBkgWords,0.0,m_nBkgWords),
				   run).isFailure();
				   //ManagedMonitorToolBase::expert,run).isFailure();
  failure = failure | registerHist(fullPath,
                                   m_HistBitSet_Filled = TH1F_LW::create("m_BitSet_Filled","Background Word(Filled BG)",m_nBkgWords,0.0,m_nBkgWords),
                                   run).isFailure();
  failure = failure | registerHist(fullPath,
                                   m_HistBitSet_Empty = TH1F_LW::create("m_BitSet_Empty","Background Word (Empty BG)",m_nBkgWords,0.0,m_nBkgWords),
                                   run).isFailure();
  failure = failure | registerHist(fullPath,
				   m_HistBitSet_UnpairIso = TH1F_LW::create("m_BitSet_UnpairIso","Background Word (UnpairIso BG)",m_nBkgWords,0.0,m_nBkgWords),
				   run).isFailure();
  failure = failure | registerHist(fullPath,
				   m_HistBitSet_UnpairNonIso = TH1F_LW::create("m_BitSet_UnpairNonIso","Background Word (UnpairNonIso BG)",m_nBkgWords,0.0,m_nBkgWords),
				   run).isFailure();

  // Pixel histograms
  failure = failure | registerHist(fullPath, 
				   m_HistPixSP = TH1F_LW::create("m_PixSP","Pixel SpacePoints Low Multiplicity",m_nBins_PixSP,0.0,m_Up_PixSP),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistPixSP_UnpairIso = TH1F_LW::create("m_PixSP_UnpairIso","Pixel SpacePoints Low Multiplicity (UnpairIso BG)",m_nBins_PixSP,0.0,m_Up_PixSP),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistPixSP_UnpairNonIso = TH1F_LW::create("m_PixSP_UnpairNonIso","Pixel SpacePoints Low Multiplicity (UnpairNonIso BG)",m_nBins_PixSP,0.0,m_Up_PixSP),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistPixSPHuge = TH1F_LW::create("m_PixSPHuge","Pixel SpacePoints Huge Multiplicity",m_nBins_SPHuge,0.0,m_Up_SPHuge),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistPixSPHuge_UnpairIso = TH1F_LW::create("m_PixSPHuge_UnpairIso","Pixel SpacePoints Huge Multiplicity (UnpairIso BG)",m_nBins_SPHuge,0.0,m_Up_SPHuge),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistPixSPHuge_UnpairNonIso = TH1F_LW::create("m_PixSPHuge_UnpairNonIso","Pixel SpacePoints Huge Multiplicity (UnpairNonIso BG)",m_nBins_SPHuge,0.0,m_Up_SPHuge),
				   run).isFailure();

  // SCT histograms
  failure = failure | registerHist(fullPath, 
				   m_HistSctSP = TH1F_LW::create("m_SctSP","SCT SpacePoints Low Multiplicity",m_nBins_SctSP,0.,m_Up_SctSP),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistSctSP_UnpairIso = TH1F_LW::create("m_SctSP_UnpairIso","SCT SpacePoints Low Multiplicity (UnpairIso BG)",m_nBins_SctSP,0.,m_Up_SctSP),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistSctSP_UnpairNonIso = TH1F_LW::create("m_SctSP_UnpairNonIso","SCT SpacePoints Low Multiplicity (UnpairNonIso BG)",m_nBins_SctSP,0.,m_Up_SctSP),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistSctSPHuge = TH1F_LW::create("m_SctSPHuge","SCT SpacePoints Huge Multiplicity",m_nBins_SPHuge,0.0,m_Up_SPHuge),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistSctSPHuge_UnpairIso = TH1F_LW::create("m_SctSPHuge_UnpairIso","SCT SpacePoints Huge Multiplicity (UnpairIso BG)",m_nBins_SPHuge,0.0,m_Up_SPHuge),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistSctSPHuge_UnpairNonIso = TH1F_LW::create("m_SctSPHuge_UnpairNonIso","SCT SpacePoints Huge Multiplicity (UnpairNonIso BG)",m_nBins_SPHuge,0.0,m_Up_SPHuge),
				   run).isFailure();

  // LAr histograms
  failure = failure | registerHist(fullPath, 
				   m_HistLArTimeDiff = TH1F_LW::create("m_LArTimeDiff","LAr EC collision time difference",m_nBins_TimeDiff,m_Low_TimeDiff,m_Up_TimeDiff),
				   run).isFailure();

  // MBTS histograms
  failure = failure | registerHist(fullPath, 
				   m_HistMBTSTimeDiff = TH1F_LW::create("m_MBTSTimeDiff","MBTS collision time difference",m_nBins_TimeDiff,m_Low_TimeDiff,m_Up_TimeDiff),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistMBTSVetoHits = TH1F_LW::create("m_MBTSVetoHits","Total MBTS Hits Setting Veto Bit",40,0.,40.),
				   run).isFailure();
  
  // Lucid histograms
  failure = failure | registerHist(fullPath, 
				   m_HistLucidHits = TH1F_LW::create("m_LucidHits","Total Lucid Hits",64,0.,32.),
				   run).isFailure();

  // Beam Background Identification Method
  // muon segments
if (m_doMuons==true){
  failure = failure | registerHist(fullPath, 
				   m_HistThetaCsc = TH1F_LW::create("m_ThetaCsc","Theta of the CSC segments",100,-1,1),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
                                   m_HistThetaCscA = TH1F_LW::create("m_ThetaCscA","Theta of the CSC segments, A side",100,-1,1),
                                   run).isFailure();
  failure = failure | registerHist(fullPath, 
                                   m_HistThetaCscC = TH1F_LW::create("m_ThetaCscC","Theta of the CSC segments, C side",100,-1,1),
                                   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistThetaMdt = TH1F_LW::create("m_ThetaMdt","Theta of the inner MDT end-cap segments",100,-1,1),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistTimeSeg0Csc = TH1F_LW::create("m_TimeSeg0Csc","Time of the CSC segments",100,-100.,100.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistTimeSeg0Mdt = TH1F_LW::create("m_TimeSeg0Mdt","Time of the inner MDT end-cap segments",100,-100.,100.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistXYSeg0 = TH2F_LW::create("m_XYSeg0","Position of the muon segments",100,-4.,4.,100,-4.,4.),
				   run).isFailure();

  // distributions of the beam background calorimeter clusters
  failure = failure | registerHist(fullPath, 
				   m_HistClusterEnergy = TH1F_LW::create("m_ClusterEnergy","Cluster energy",150,0.,1500.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistClusterEtaPhi = TH2F_LW::create("m_ClusterEtaPhi","Cluster eta-phi",50,-3.,3.,50,-3.15,3.15),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistClusterEtaTime = TH2F_LW::create("m_ClusterEtaTime","Cluster eta-time",50,-3.,3.,50,-25.,25.),
				   run).isFailure();

  // index of the jets tagged by the two-sided method
  failure = failure | registerHist(fullPath, 
				   m_HistFakeJetIndex = TH1F_LW::create("m_FakeJetIndex","Fake jet index",10,0.,10.),
				   run).isFailure();

  // leading jet distributions in the events where the leading jet is tagged by the two-sided method
  failure = failure | registerHist(fullPath, 
				   m_HistFakeJet1Pt = TH1F_LW::create("m_FakeJet1Pt","Fake jet pT",150,0.,1500.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistFakeJet1Eta = TH1F_LW::create("m_FakeJet1Eta","Fake jet eta",50,-3.,3.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistFakeJet1Phi = TH1F_LW::create("m_FakeJet1Phi","Fake jet phi",50,-3.15,3.15),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistFakeJet1Time = TH1F_LW::create("m_FakeJet1Time","Fake jet time",120,-30.,30.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistFakeJet1Chf = TH1F_LW::create("m_FakeJet1Chf","Fake jet charge fraction",100,0.,1.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistFakeJet1EtaTime = TH2F_LW::create("m_FakeJet1EtaTime","Fake jet eta-time",50,-3.,3.,50,-25.,25.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistFakeJet1EmfChf = TH2F_LW::create("m_FakeJet1EmfChf","Fake jet emf-chf",50,0.,1.,50,0.,1.),
           run).isFailure();

  // beam background muon rate per BCID in A->C and C->A direction
  failure = failure | registerHist(fullPath, 
				   m_HistRateBcidACTwoSided = TH1F_LW::create("m_RateBcidACTwoSided","Rate per BCID in the A->C direction (two-sided)",3564,0.,3564.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistRateBcidCATwoSided = TH1F_LW::create("m_RateBcidCATwoSided","Rate per BCID in the C->A direction (two-sided)",3564,0.,3564.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistRateBcidACTwoSidedOneSided = TH1F_LW::create("m_RateBcidACTwoSidedOneSided","Rate per BCID in the A->C direction (two-sided and one-sided)",3564,0.,3564.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistRateBcidCATwoSidedOneSided = TH1F_LW::create("m_RateBcidCATwoSidedOneSided","Rate per BCID in the C->A direction (two-sided and one-sided)",3564,0.,3564.),
				   run).isFailure();
} // end of m_doMuons==true

  // Bad jets
  // leading jet distributions in the events where the leading jet is bad (looser)
  failure = failure | registerHist(fullPath, 
				   m_HistBadLooserJet1Pt = TH1F_LW::create("m_BadLooserJet1Pt","Bad (looser) jet pT",150,0.,1500.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistBadLooserJet1Eta = TH1F_LW::create("m_BadLooserJet1Eta","Bad (looser) jet eta",50,-3.,3.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistBadLooserJet1Phi = TH1F_LW::create("m_BadLooserJet1Phi","Bad (looser) jet phi",50,-3.15,3.15),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistBadLooserJet1Time = TH1F_LW::create("m_BadLooserJet1Time","Bad (looser) jet time",120,-30.,30.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistBadLooserJet1Chf = TH1F_LW::create("m_BadLooserJet1Chf","Bad (looser) jet charge fraction",100,0.,1.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistBadLooserJet1EtaTime = TH2F_LW::create("m_BadLooserJet1EtaTime","Bad (looser) jet eta-time",50,-3.,3.,50,-25.,25.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistBadLooserJet1EmfChf = TH2F_LW::create("m_BadLooserJet1EmfChf","Bad (looser) jet emf-chf",50,0.,1.,50,0.,1.),
				   run).isFailure();

  // Cosmic Muon Identification

  // General histograms
  failure = failure | registerHist(fullPath, 
				   m_HistNumVertex = TH1F_LW::create("m_NumVertex","Number of primary vertices",50,0.,50.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistNumVertex_UnpairIso = TH1F_LW::create("m_NumVertex_UnpairIso","Number of primary vertices (UnpairIso BG)",50,0.,50.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistNumVertex_UnpairNonIso = TH1F_LW::create("m_NumVertex_UnpairNonIso","Number of primary vertices (UnpairNonIso BG)",50,0.,50.),
				   run).isFailure();
  failure = failure | registerHist(fullPath, 
				   m_HistMetLocHadTopo = TH1F_LW::create("m_MetLocHadTopo","Missing transverse momentum (LocHadTopo)",150,0.,1500.),
				   run).isFailure();


  if (failure) {
    ATH_MSG_ERROR( "Error Booking histograms " );
    return failure;
  }

  if(!m_HistBitSet->GetXaxis()||!m_HistBitSet_UnpairIso->GetXaxis()||!m_HistBitSet_UnpairNonIso->GetXaxis())
    ATH_MSG_WARNING( " At least one background word histogram returned NULL pointer to X axis, labels will not be set!" );
  else{
    ATH_MSG_DEBUG( "Setting bin labels for background word histograms" );
    for(int i=0; i<EventInfo::NBackgroundWords; ++i){
      m_HistBitSet->GetXaxis()->SetBinLabel(i+1,labels[i].c_str());
      m_HistBitSet_UnpairIso->GetXaxis()->SetBinLabel(i+1,labels[i].c_str());
      m_HistBitSet_UnpairNonIso->GetXaxis()->SetBinLabel(i+1,labels[i].c_str());
    }
  }

//  if(!m_HistPixSP->GetXaxis()||!m_HistPixSP_Filled->GetXaxis()||!m_HistPixSP_Empty->GetXaxis()||!m_HistPixSP_UnpairIso->GetXaxis()||!m_HistPixSP_UnpairNonIso->GetXaxis()||
//     !m_HistPixSPHuge->GetXaxis()||!m_HistPixSPHuge_Filled->GetXaxis()||!m_HistPixSPHuge_Empty->GetXaxis()||!m_HistPixSPHuge_UnpairIso->GetXaxis()||!m_HistPixSPHuge_UnpairNonIso->GetXaxis())
  if(!m_HistPixSP->GetXaxis()||!m_HistPixSP_UnpairIso->GetXaxis()||!m_HistPixSP_UnpairNonIso->GetXaxis()||
     !m_HistPixSPHuge->GetXaxis()||!m_HistPixSPHuge_UnpairIso->GetXaxis()||!m_HistPixSPHuge_UnpairNonIso->GetXaxis())
    ATH_MSG_WARNING( "At least one Pixel SP histogram returned NULL pointer to X axis, axis titles will not be set!" );
  else{
    m_HistPixSP->GetXaxis()->SetTitle("Number of Pixel SPs");
//    m_HistPixSP_Filled->GetXaxis()->SetTitle("Number of Pixel SPs");
//    m_HistPixSP_Empty->GetXaxis()->SetTitle("Number of Pixel SPs");
    m_HistPixSP_UnpairIso->GetXaxis()->SetTitle("Number of Pixel SPs");
    m_HistPixSP_UnpairNonIso->GetXaxis()->SetTitle("Number of Pixel SPs");
    m_HistPixSPHuge->GetXaxis()->SetTitle("Number of Pixel SPs");
    m_HistPixSPHuge_UnpairIso->GetXaxis()->SetTitle("Number of Pixel SPs");
    m_HistPixSPHuge_UnpairNonIso->GetXaxis()->SetTitle("Number of Pixel SPs");
  }

  if(!m_HistSctSP->GetXaxis()||!m_HistSctSP_UnpairIso->GetXaxis()||!m_HistSctSP_UnpairNonIso->GetXaxis()||
     !m_HistSctSPHuge->GetXaxis()||!m_HistSctSPHuge_UnpairIso->GetXaxis()||!m_HistSctSPHuge_UnpairNonIso->GetXaxis())
    ATH_MSG_WARNING( "At least one SCT SP histogram returned NULL pointer to X axis, axis titles will not be set!" );
  else{
    m_HistSctSP->GetXaxis()->SetTitle("Number of SCT SPs");
    m_HistSctSP_UnpairIso->GetXaxis()->SetTitle("Number of SCT SPs");
    m_HistSctSP_UnpairNonIso->GetXaxis()->SetTitle("Number of SCT SPs");
    m_HistSctSPHuge->GetXaxis()->SetTitle("Number of SCT SPs");
    m_HistSctSPHuge_UnpairIso->GetXaxis()->SetTitle("Number of SCT SPs");
    m_HistSctSPHuge_UnpairNonIso->GetXaxis()->SetTitle("Number of SCT SPs");
  }

  //if(!m_HistLArTimeDiff->GetXaxis()||!m_HistLArTimeDiff_Filled->GetXaxis()||!m_HistLArTimeDiff_Empty->GetXaxis()||!m_HistLArTimeDiff_UnpairIso->GetXaxis()||!m_HistLArTimeDiff_UnpairNonIso->GetXaxis())
  if(!m_HistLArTimeDiff->GetXaxis())
    ATH_MSG_WARNING( "At least one LAr time difference histogram returned NULL pointer to X axis, axis titles will not be set!" );
  else{
    m_HistLArTimeDiff->GetXaxis()->SetTitle("<t_{A}>-<t_{C}> [ns]");
  }

  if(!m_HistMBTSTimeDiff->GetXaxis())
    ATH_MSG_WARNING( "At least one MBTS time difference histogram returned NULL pointer to X axis, axis titles will not be set!" );
  else{
    m_HistMBTSTimeDiff->GetXaxis()->SetTitle("<t_{A}>-<t_{C}> [ns]");
  }

  if(!m_HistMBTSVetoHits->GetXaxis())
    ATH_MSG_WARNING( "At least one MBTS hit histogram returned NULL pointer to X axis, axis titles will not be set!" );
  else{
    m_HistMBTSVetoHits->GetXaxis()->SetTitle("Number of MBTS 'veto' hits");
  }

  if(!m_HistLucidHits->GetXaxis())
    ATH_MSG_WARNING( "At least one LUCID hit histogram returned NULL pointer to X axis, axis titles will not be set!" );
  else{
    m_HistLucidHits->GetXaxis()->SetTitle("Number of LUCID 'veto' hits");
  }

  return failure;
}

//----------------------------------------------------------------------------------
StatusCode DQTBackgroundMon::fillHistograms()
//----------------------------------------------------------------------------------
{
  m_bg=999;
  m_filled=false;
  m_empty=false;
  m_unpairiso=false;
  m_unpairnoniso=false;

  if(m_doTrigger){
    ATH_MSG_DEBUG( " doTrigger = true" );
    m_bg = (unsigned char)(m_trigDec->getBGCode());
    ATH_MSG_DEBUG( "BG code is " << m_bg );
    if( m_bg & (0x1<<m_FilledBG)){
      ATH_MSG_DEBUG( "Filled Bunch Group" );
      m_filled=true;
    }
    if( m_bg & (0x1<<m_EmptyBG)){
      ATH_MSG_DEBUG( "Emtpy Bunch Group" );
      m_empty=true;
    }
    if( m_bg & (0x1<<m_UnpairIsoBG)){
      ATH_MSG_DEBUG( "Unpair Isolated Bunch Group" );
      m_unpairiso=true;
    }
    if( m_bg & (0x1<<m_UnpairNonIsoBG)){
      ATH_MSG_DEBUG("Unpair Non-Isolated Bunch Group" );
      m_unpairnoniso=true;
    }
  } else {
    ATH_MSG_WARNING( " doTrigger = false" );
  }
  //}


  unsigned bunch_crossing_id = 0;
  // this events background word
  SG::ReadHandle<EventInfo> eventInfo(m_EventInfoKey);
  if (!eventInfo.isValid()) {
    ATH_MSG_WARNING( "  Could not retrieve EventInfo object, background word histograms will not be filled for this event!" );
  }
  else{
    bunch_crossing_id = eventInfo->bcid();
    for(int bkg_i=0;bkg_i<m_nBkgWords;++bkg_i){
      if(! (eventInfo->eventFlags(EventInfo::Background) & 0x1<<bkg_i)){
        continue;
      }
      ATH_MSG_DEBUG( " Background word is " << eventInfo->eventFlags(EventInfo::Background) );
      m_HistBitSet->Fill(bkg_i);
      if(m_filled)
        m_HistBitSet_Filled->Fill(bkg_i);
      else if(m_empty)
        m_HistBitSet_Empty->Fill(bkg_i);
      else if(m_unpairiso)
        m_HistBitSet_UnpairIso->Fill(bkg_i);
      else if(m_unpairnoniso)
        m_HistBitSet_UnpairNonIso->Fill(bkg_i);
    }
  }


  SG::ReadHandle<RawInfoSummaryForTag> rawinfo(m_RawInfoSummaryForTagKey);
  if (rawinfo.isValid()) {
  //this events spacepoints
      float pixSPs=(float)rawinfo->getNpixSPs();
      float sctSPs=(float)rawinfo->getNsctSPs();

      m_HistSctSPHuge->Fill(sctSPs);
      if(m_unpairiso)
        m_HistSctSPHuge_UnpairIso->Fill(sctSPs);
      else if(m_unpairnoniso)
        m_HistSctSPHuge_UnpairNonIso->Fill(sctSPs);

      if(sctSPs<m_Up_SctSP){
        m_HistSctSP->Fill(sctSPs);
        if(m_unpairiso)
          m_HistSctSP_UnpairIso->Fill(sctSPs);
        else if(m_unpairnoniso)
          m_HistSctSP_UnpairNonIso->Fill(sctSPs);
      }

      m_HistPixSPHuge->Fill(pixSPs);
      if(m_unpairiso)
        m_HistPixSPHuge_UnpairIso->Fill(pixSPs);
      else if(m_unpairnoniso)
        m_HistPixSPHuge_UnpairNonIso->Fill(pixSPs);

      if(pixSPs<m_Up_PixSP){
        m_HistPixSP->Fill(pixSPs);
      if(m_unpairiso)
        m_HistPixSP_UnpairIso->Fill(pixSPs);
      else if(m_unpairnoniso)
        m_HistPixSP_UnpairNonIso->Fill(pixSPs);
      }
  } // SG RawInfo veto


  SG::ReadHandle<LArCollisionTime> tps(m_LArCollisionTimeKey);
  if (tps.isValid()) {
    //this events LAr collision time
      if (tps->ncellA() > m_LArEC_SideCut && tps->ncellC() > m_LArEC_SideCut) {
        float LArECtimeDiff = tps->timeA()-tps->timeC();
        m_HistLArTimeDiff->Fill(LArECtimeDiff);
      } // enough hits per side
  } else {
    ATH_MSG_WARNING("Failed to retrieve LArCollisionTime object, LAr EC time difference histograms will not be filled for this event!");
  }// LAr time SG veto


  SG::ReadHandle<MBTSCollisionTime> mbtsTime(m_MBTSCollisionTimeKey);
  if (mbtsTime.isValid()) {
    // this events MBTS time difference
      if(mbtsTime->ncellA()>m_MBTS_SideCut && mbtsTime->ncellC()>m_MBTS_SideCut){
        float MBTStimeDiff = mbtsTime->time();
        m_HistMBTSTimeDiff->Fill(MBTStimeDiff);
      } //enough hits per side
  } else {
    ATH_MSG_WARNING("Failed to retrieve MBTSCollisionTime object, MBTS time difference histograms will not be filled for this event!");
  } //MBTS time SG veto


  SG::ReadHandle<TileCellContainer> tileCellCnt(m_TileCellContainerKey);
  if (tileCellCnt.isValid()) {
    // this events MBTS veto count
    int MBTScount(0);
    TileCellContainer::const_iterator itr = tileCellCnt->begin();
    TileCellContainer::const_iterator itr_end = tileCellCnt->end();
    for(; itr != itr_end; ++itr) {
      if ((*itr)->energy()<m_MBTS_ThresholdCut) continue;
      const uint8_t qbit1=(*itr)->qbit1();
      if ((qbit1 & m_MBTS_mask) != m_MBTS_pattern) {
	ATH_MSG_DEBUG("Rejected based on quality bits");
	continue;
      }
      if (fabs((*itr)->time())<m_MBTS_TimeCut) MBTScount++;
    }
    m_HistMBTSVetoHits->Fill((float)MBTScount);
  } else {
    ATH_MSG_WARNING("Error retrieving " << m_mbtsContainerName  << ", MBTS hit histogram will not be filled for this event!");
  } // MBTS tile SG veto


  SG::ReadHandle<LUCID_RawDataContainer> LUCID_RawDataContainer(m_LUCID_RawDataContainerKey);
  if (LUCID_RawDataContainer.isValid()) {
    // this events Lucid hit count
    LUCID_RawDataContainer::const_iterator LUCID_RawData_itr = LUCID_RawDataContainer->begin();
    LUCID_RawDataContainer::const_iterator LUCID_RawData_end = LUCID_RawDataContainer->end();
    int LUCIDcounter(0);
    for (; LUCID_RawData_itr != LUCID_RawData_end; LUCID_RawData_itr++) {
      LUCIDcounter+=(*LUCID_RawData_itr)->getNhitsPMTsideA();
      LUCIDcounter+=(*LUCID_RawData_itr)->getNhitsPMTsideC();
    }
    m_HistLucidHits->Fill(LUCIDcounter);
  } else {
    ATH_MSG_WARNING("  Could not retrieve Lucid_RawData, LUCID hit histograms will not be filled for this event!");
  } // LUCID SG veto


  if (m_doMuons==true){

    CHECK( m_helperTool.retrieve() );
    CHECK( m_idHelperTool.retrieve() );
    CHECK( m_idToFixedIdTool.retrieve() );

    SG::ReadHandle<Trk::SegmentCollection> segmentContainer(m_SegmentCollectionKey);
    if (segmentContainer.isValid()) {
      for(unsigned int i=0; i<segmentContainer->size(); i++) {
	const xAOD::MuonSegment* seg = dynamic_cast<const xAOD::MuonSegment*>(segmentContainer->at(i));
	if ( seg == 0 ) continue ;
	// The tool MuonEDMHelperTool (m_helperTool)
	// has not been migrated to xAOD yet
	// one could use the converter:
	// https://svnweb.cern.ch/trac/atlasoff/browser/MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools/trunk/src/MuonSegmentConverterTool.h
	// but, hoping that migration will happen soon, we temporarily disable this part.
	/*
          Identifier id = m_helperTool->chamberId(*seg);
          if ( !id.is_valid() ) continue;
          if ( !m_idHelperTool->isMuon(id) ) continue;
	  
          MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId( id );
          int stationName = fid.stationName();
          int stationEta = fid.eta();
	  
          // station names: CSC=33,34; MDT=15,24
          if( !(stationName==33 || stationName==34 || stationName==15 || stationName==24 )) continue;
	  
          const  Amg::Vector3D&  globalPos = seg->globalPosition(); // changed to ->x() , y() , z()
          const Amg::Vector3D& globalDir = seg->globalDirection();  // changed to ->px(), py(), pz()
          double thetaPos = globalPos.theta();
          double thetaDir = globalDir.theta();
          double theta = TMath::Cos(2.*(thetaPos-thetaDir));
	  
          if(stationName==15 || stationName==24) m_HistThetaMdt->Fill(theta);
          if(stationName==33 || stationName==34){
	  m_HistThetaCsc->Fill(theta);
	  if(stationEta==1) m_HistThetaCscA->Fill(theta);
	  if(stationEta==-1) m_HistThetaCscC->Fill(theta);
          }
	*/
      }
    } else {
      ATH_MSG_WARNING("  Could not retrieve ConvertedMBoySegments, histograms will not be filled for this event!");
    }//MBoy segments theta


    SG::ReadHandle<BeamBackgroundData> beamBackgroundData(m_BeamBackgroundDataKey);
    if (beamBackgroundData.isValid()) {
      for(int i=0; i<beamBackgroundData->GetNumSegment(); i++) {
	const Trk::Segment* tSeg = beamBackgroundData->GetIndexSeg(i);
	const xAOD::MuonSegment* mSeg = dynamic_cast<const xAOD::MuonSegment*>(tSeg);
	if ( mSeg == 0 ) continue ;
	// As above, comment out while waiting for the tool migration
	/*
          Identifier id = m_helperTool->chamberId(*mSeg);
          if ( !id.is_valid() ) continue;
          if ( !m_idHelperTool->isMuon(id) ) continue;
	  
          MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId( id );
          int stationName = fid.stationName();
	  
          if( !(stationName==33 || stationName==34 || stationName==15 || stationName==24 )) continue;
	*/
	double x = mSeg->x();
	double y = mSeg->y();
	//double time = mSeg->t0();
	
	/*if(stationName==33 || stationName==34) m_HistTimeSeg0Csc->Fill(time);
          if(stationName==15 || stationName==24) m_HistTimeSeg0Mdt->Fill(time);
	*/
	m_HistXYSeg0->Fill(x/1000., y/1000.);
      }
      
      for(int i=0; i<beamBackgroundData->GetNumMatched(); i++) {
	const xAOD::CaloCluster* clus = beamBackgroundData->GetIndexClus(i);
	
	double energy = clus->e();
	double eta = clus->eta();
	double phi = clus->phi();
	//double time = clus->getTime();
	double time = clus->time();
	
	m_HistClusterEnergy->Fill(energy/1000.);
	m_HistClusterEtaPhi->Fill(eta, phi);
	m_HistClusterEtaTime->Fill(eta, time);
      }
      
      if( beamBackgroundData->GetNumNoTimeTight() ) {       // GetNumTwoSided, GetNumTwoSidedNoTime or GetNumNoTimeTight
	for(int i=0; i<beamBackgroundData->GetNumJet(); i++) {
	  const xAOD::Jet* jet = beamBackgroundData->GetIndexJet(i);
	  int index = beamBackgroundData->GetIndexJet(jet);
	  
	  m_HistFakeJetIndex->Fill(index);
	  
	  if( index == 0 ) {  // leading jet
	    double pt = jet->pt();
	    double eta = jet->eta();
	    double phi = jet->phi();
	    
	    //double sumPtTrk = jet->getAttribute<std::vector<float>>(JetAttribute::SumPtTrkPt1000)[0];
	    // Suggested fix for the above line
	    std::vector<float> sumPt_v;
	    double sumPtTrk = 0;
	    bool hasSumPt = jet->getAttribute<std::vector<float> >(xAOD::JetAttribute::SumPtTrkPt1000, sumPt_v); 
	    if (hasSumPt) sumPtTrk = sumPt_v[0];
	    
	    double chf = sumPtTrk / pt;
	    double time = jet->getAttribute<float>(JetAttribute::Timing);
	    double emf = jet->getAttribute<float>(JetAttribute::EMFrac);
	    
	    m_HistFakeJet1Pt->Fill(pt/1000.);
	    m_HistFakeJet1Eta->Fill(eta);
	    m_HistFakeJet1Phi->Fill(phi);
	    m_HistFakeJet1Time->Fill(time);
	    m_HistFakeJet1Chf->Fill(chf);
	    m_HistFakeJet1EtaTime->Fill(eta, time);
	    m_HistFakeJet1EmfChf->Fill(emf, chf);
	  }
	}  // end of loop through jets
	
	if( beamBackgroundData->GetDirection() > 0 ) {
	  m_HistRateBcidACTwoSided->Fill(bunch_crossing_id);
	  if( beamBackgroundData->GetNumOneSidedLoose() ) m_HistRateBcidACTwoSidedOneSided->Fill(bunch_crossing_id);
	}
	else if( beamBackgroundData->GetDirection() < 0 ) {
	  m_HistRateBcidCATwoSided->Fill(bunch_crossing_id);
	  if( beamBackgroundData->GetNumOneSidedLoose() ) m_HistRateBcidCATwoSidedOneSided->Fill(bunch_crossing_id);
	}
      } // end fake jets
    } else {
      ATH_MSG_WARNING("  Could not retrieve BeamBackgroundData, histograms will not be filled for this event!");
    }
  } // end of m_doMuons==true

  SG::ReadHandle<xAOD::VertexContainer> vxContainer(m_VertexContainerKey);
  if (vxContainer.isValid()) {
    int numVertex = vxContainer->size() - 1;  // exclude dummy vertex
    m_HistNumVertex->Fill(numVertex);
    if(m_unpairiso)
      m_HistNumVertex_UnpairIso->Fill(numVertex);
    else if(m_unpairnoniso)
      m_HistNumVertex_UnpairNonIso->Fill(numVertex);
  } else {
    ATH_MSG_DEBUG ("Could not retrieve xAOD::VertexContainer 'PrimaryVertices', histograms will not be filled for this event!");
  }


/*      //TODO
  if( evtStore()->contains<JetContainer>("AntiKt4TopoEMJets") ){

    const JetContainer* jettContainer;
    if (evtStore()->retrieve(jetContainer, "AntiKt4TopoEMJets").isFailure() ) {
      ATH_MSG_WARNING( "  Could not retrieve AntiKt4TopoEMJets, histograms will not be filled for this event!" );
    }
    else {

      for(unsigned int i=0; i<jetContainer->size(); i++) {
        const Jet* jet = jetContainer->at(i);

       }
    }
  }
*/

  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------------
StatusCode DQTBackgroundMon::procHistograms( )
//StatusCode DQTBackgroundMon::procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
//----------------------------------------------------------------------------------
{
  ATH_MSG_DEBUG( "In procHists()" );
  //if ( isEndOfEventsBlock || isEndOfLumiBlock || isEndOfRun ) {

  //}
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------------
StatusCode DQTBackgroundMon::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------
{
  ATH_MSG_DEBUG( "In checkHists()" );
  return StatusCode::SUCCESS;
}
