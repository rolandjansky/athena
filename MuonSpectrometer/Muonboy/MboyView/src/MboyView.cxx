/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyView.h"


MboyView::MboyView(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
 p_MboyViewEventSelectionTool         ( "MboyViewEventSelectionTool/ConfiguredMboyViewEventSelectionTool"                ) ,
 p_MboyViewGenEventSelectionTool      ( "MboyViewGenEventSelectionTool/ConfiguredMboyViewGenEventSelectionTool"          ) ,
 p_MboyViewEventInfoTool              ( "MboyViewEventInfoTool/ConfiguredMboyViewEventInfoTool"                          ) ,
 p_MboyViewMuonRecDigiStoreTDSTool    ( "MboyViewMuonRecDigiStoreTDSTool/ConfiguredMboyViewMuonRecDigiStoreTDSTool"      ) ,
 p_MboyViewIDRecDigiTool              ( "MboyViewIDRecDigiTool/ConfiguredMboyViewIDRecDigiTool"                          ) ,
 p_MboyViewTrackRecordCollectionTool  ( "MboyViewTrackRecordCollectionTool/ConfiguredMboyViewTrackRecordCollectionTool"  ) ,
 p_MboyViewCaloTool                   ( "MboyViewCaloTool/ConfiguredMboyViewCaloTool"                                    ) ,
 p_MboyViewCombinedMuonContainerTool  ( "MboyViewCombinedMuonContainerTool/ConfiguredMboyViewCombinedMuonContainerTool"  ) ,
 p_MboyViewTrackParticleContainerTool ( "MboyViewTrackParticleContainerTool/ConfiguredMboyViewTrackParticleContainerTool") ,
 p_MboyViewTrkTrackCollectionTool     ( "MboyViewTrkTrackCollectionTool/ConfiguredMboyViewTrkTrackCollectionTool"        ) ,
 p_MboyViewTrkSegmentCollectionTool   ( "MboyViewTrkSegmentCollectionTool/ConfiguredMboyViewTrkSegmentCollectionTool"    ) ,
 p_MboyViewMcEventCollectionTool      ( "MboyViewMcEventCollectionTool/ConfiguredMboyViewMcEventCollectionTool"          ) ,
 p_MboyViewEgammaContainerTool        ( "MboyViewEgammaContainerTool/ConfiguredMboyViewEgammaContainerTool"              ) ,
//apolici
 p_MboyViewJetContainerTool           ( "MboyViewJetContainerTool/ConfiguredMboyViewJetContainerTool"                    ) ,
 p_MboyViewMETContainerTool           ( "MboyViewMETContainerTool/ConfiguredMboyViewMETContainerTool"                    ) ,
 p_MboyViewL1RoIContainerTool         ( "MboyViewL1RoIContainerTool/ConfiguredMboyViewL1RoIContainerTool"                ) ,
//apolici
 p_MboyViewVertexContainerTool        ( "MboyViewVertexContainerTool/ConfiguredMboyViewVertexContainerTool"              ) ,
 p_MboyViewDumpCombinedMuonContainerTool        ( "MboyViewDumpCombinedMuonContainerTool/ConfiguredMboyViewDumpCombinedMuonContainerTool"              ) 
{

  declareProperty("MboyViewEventSelectionTool"        		 , p_MboyViewEventSelectionTool         ) ;
  declareProperty("MboyViewGenEventSelectionTool"     		 , p_MboyViewGenEventSelectionTool      ) ;
  declareProperty("MboyViewEventInfoTool"             		 , p_MboyViewEventInfoTool              ) ;
  declareProperty("MboyViewMuonRecDigiStoreTDSTool"   		 , p_MboyViewMuonRecDigiStoreTDSTool    ) ;
  declareProperty("MboyViewIDRecDigiTool"             		 , p_MboyViewIDRecDigiTool              ) ; 
  declareProperty("MboyViewTrackRecordCollectionTool" 		 , p_MboyViewTrackRecordCollectionTool  ) ;
  declareProperty("MboyViewCaloTool"                  		 , p_MboyViewCaloTool                   ) ;
  declareProperty("MboyViewCombinedMuonContainerTool" 		 , p_MboyViewCombinedMuonContainerTool  ) ;
  declareProperty("MboyViewTrackParticleContainerTool" 		 , p_MboyViewTrackParticleContainerTool ) ;
  declareProperty("MboyViewTrkTrackCollectionTool"    		 , p_MboyViewTrkTrackCollectionTool     ) ;
  declareProperty("MboyViewTrkSegmentCollectionTool"   		 , p_MboyViewTrkSegmentCollectionTool   ) ;
  declareProperty("MboyViewMcEventCollectionTool"      		 , p_MboyViewMcEventCollectionTool      ) ;
  declareProperty("MboyViewEgammaContainerTool"        		 , p_MboyViewEgammaContainerTool        ) ;
//apolici
  declareProperty("MboyViewJetContainerTool"     	         , p_MboyViewJetContainerTool           ) ;
  declareProperty("MboyViewMETContainerTool"     	         , p_MboyViewMETContainerTool           ) ;
  declareProperty("MboyViewL1RoIContainerTool"     	         , p_MboyViewL1RoIContainerTool         ) ;
//apolici
  declareProperty("MboyViewVertexContainerTool"       		 , p_MboyViewVertexContainerTool        ) ; 
  declareProperty("MboyViewDumpCombinedMuonContainerTool"        , p_MboyViewDumpCombinedMuonContainerTool        ) ; 

  declareProperty("SwitchOff"                                              , m_SwitchOff                                    = 1                                    ) ;

//-----------------------------------------------------------------
//Output File Name
  declareProperty("NameOfMboyViewOutputFile"                               , m_NameOfMboyViewOutputFile                     = "Out.MboyView"                       ) ;
  declareProperty("NameOfMboyViewOutputFileCombinedDump"                   , m_NameOfMboyViewOutputFileCombinedDump         = "Out.MboyViewCombinedDump"           ) ;

//-----------------------------------------------------------------
// Event Selection: EvtSelectionType and EvtSelectionThreshold 
//           = 1 view without Selection
//           = 2 view only for evts Nber of Tracks .le. EvtSelectionThreshold
//           = 3 view only for evts Nber of Tracks .ge. EvtSelectionThreshold
//           = 4 view only for evts Nber of Tracks .eq. EvtSelectionThreshold

  declareProperty("EvtSelectionType"                                       , m_EvtSelectionType                             = 1                                    ) ;
  declareProperty("EvtSelectionThreshold"                                  , m_EvtSelectionThreshold                        = 0                                    ) ;

  declareProperty("MuonRecDigiStoreTDSLocationForEventSelection"           , m_MuonRecDigiStoreTDSLocationForEventSelection = "Muonboy_DigiStore"                  ) ;
  declareProperty("TrkTrackCollectionLocationForEventSelection"            , m_TrkTrackCollectionLocationForEventSelection  = "ConvertedMBoyMuonSpectroOnlyTracks" ) ;

//-----------------------------------------------------------------
// GenEvent Selection: GenEvtSelectionType and GenEvtSelectionEta<Min/Max>
//           = 1 view without Selection
//           = 2 view only for evts with at least a gen track with GenEvtSelectionEtaMin<abs(eta)<GenEvtSelectionEtaMax

  declareProperty("GenEvtSelectionType"                                    , m_GenEvtSelectionType                          = 1                                    ) ;
  declareProperty("GenEvtSelectionEtaMin"                                  , m_GenEvtSelectionEtaMin                        = 0.                                   ) ;
  declareProperty("GenEvtSelectionEtaMax"                                  , m_GenEvtSelectionEtaMax                        = 3.                                   ) ;

  m_TrackRecordCollectionLocationForGenEventSelectionList.clear();
  m_TrackRecordCollectionLocationForGenEventSelectionList.push_back(                                                          "MuonEntryLayerFilter"               ) ;
  m_TrackRecordCollectionLocationForGenEventSelectionList.push_back(                                                          "MuonEntryRecordFilter"              ) ;
  declareProperty("TrackRecordCollectionLocationForGenEventSelectionList"  , m_TrackRecordCollectionLocationForGenEventSelectionList                               ) ;

//-----------------------------------------------------------------
// MuonRecDigiStoreTDS to be viewed
//  ViewAthIds 1/0: View Ath/Amdb digits Ids
//  PrintRSLine 1/0: Print/do not print RS lines
//  ATCOSM 1/0: set value of printed ATCOSM

  declareProperty("ViewDigits"                                             , m_ViewDigits                                   = 1                                    ) ;
  declareProperty("MuonRecDigiStoreTDSLocation"                            , m_MuonRecDigiStoreTDSLocation                  = "Muonboy_DigiStore"                  ) ;
  declareProperty("ViewAthIds"                                             , m_ViewAthIds                                   = 1                                    ) ;
  declareProperty("PrintRSLine"                                            , m_PrintRSLine                                  = 1                                    ) ;
  declareProperty("ATCOSM"                                                 , m_ATCOSM                                       = 0                                    ) ;

//-----------------------------------------------------------------
// TrackRecordCollection to be Viewed

  declareProperty("ViewTrackRecordCollection"                              , m_ViewTrackRecordCollection                    = 1                                    ) ;

  m_TrackRecordCollectionLocationList.clear();
  m_TrackRecordCollectionLocationList.push_back(                                                                             "MuonEntryLayerFilter" );
  declareProperty("TrackRecordCollectionLocationList"                      , m_TrackRecordCollectionLocationList                                                   ) ;

//-----------------------------------------------------------------
// Calo View

  declareProperty("ViewCalo"                                               , m_ViewCalo                                     = 1                                    ) ;
  declareProperty("CaloMinimumE"                                           , m_CaloMinimumE                                 = 0.1*CLHEP::GeV                       ) ;

//-----------------------------------------------------------------
// CombinedMuonContainer to be Viewed

  declareProperty("ViewCombinedMuonContainer"                              , m_ViewCombinedMuonContainer                    = 1                                    ) ;

  m_CombinedMuonContainerLocationList.clear();
  m_CombinedMuonContainerLocationList.push_back(                                                                             "MboyESDMuonContainer"                ) ;
  m_CombinedMuonContainerLocationList.push_back(                                                                             "StacoCombinedMuonContainer"          ) ;
  m_CombinedMuonContainerLocationList.push_back(                                                                             "StacoCombinedMuonContainerLowPt"     ) ;
  m_CombinedMuonContainerLocationList.push_back(                                                                             "MuidCombinedMuonContainer"           ) ;
  m_CombinedMuonContainerLocationList.push_back(                                                                             "MuGirlCombinedMuonContainer"         ) ;
  m_CombinedMuonContainerLocationList.push_back(                                                                             "MuidExtrCombinedMuonContainer"       ) ;
  declareProperty("CombinedMuonContainerLocationList"                      , m_CombinedMuonContainerLocationList                                                   ) ;

//-----------------------------------------------------------------
// TrackParticleContainer to be Viewed

  declareProperty("ViewTrackParticleContainer"                             , m_ViewTrackParticleContainer                   = 0                                    ) ;

  m_TrackParticleContainerLocationList.clear();
  m_TrackParticleContainerLocationList.push_back(                                                                            "MuonboyTrackParticles"               ) ;
  m_TrackParticleContainerLocationList.push_back(                                                                            "MuonboyMuonSpectroOnlyTrackParticles") ;
  m_TrackParticleContainerLocationList.push_back(                                                                            "TrackParticleCandidate"              ) ;
  m_TrackParticleContainerLocationList.push_back(                                                                            "StacoTrackParticles"                 ) ;
  m_TrackParticleContainerLocationList.push_back(                                                                            "MuTagTrackParticles"                 ) ;
  m_TrackParticleContainerLocationList.push_back(                                                                            "MooreTrackParticles"                 ) ;
  m_TrackParticleContainerLocationList.push_back(                                                                            "MuidCombTrackParticles"              ) ;
  m_TrackParticleContainerLocationList.push_back(                                                                            "MuidExtrTrackParticles"              ) ;
  declareProperty("TrackParticleContainerLocationList"                     , m_TrackParticleContainerLocationList                                                  ) ;

//-----------------------------------------------------------------
// TrkTrackCollection to be Viewed

  declareProperty("ViewTrkTrackCollection"                                 , m_ViewTrkTrackCollection                       = 1                                    ) ;

  m_TrkTrackCollectionLocationList.clear();
  m_TrkTrackCollectionLocationList.push_back(                                                                                "Tracks"                              ) ;
  m_TrkTrackCollectionLocationList.push_back(                                                                                "ConvertedMBoyMuonSpectroOnlyTracks"  ) ;
//m_TrkTrackCollectionLocationList.push_back(                                                                                "ConvertedMBoyTracks"                 ) ;
  m_TrkTrackCollectionLocationList.push_back(                                                                                "MooreTracks"                         ) ;
//m_TrkTrackCollectionLocationList.push_back(                                                                                "MuIdCBTracks"                        ) ;
//m_TrkTrackCollectionLocationList.push_back(                                                                                "MuIdExtrTracks"                      ) ;
//m_TrkTrackCollectionLocationList.push_back(                                                                                "ConvertedStacoTracks"                ) ;
//m_TrkTrackCollectionLocationList.push_back(                                                                                "ConvertedMuTagTracks"                ) ;
  declareProperty("TrkTrackCollectionLocationList"                         , m_TrkTrackCollectionLocationList                                                      ) ;

//-----------------------------------------------------------------
// TrkSegmentCollection to be Viewed

  declareProperty("ViewTrkSegmentCollection"                               , m_ViewTrkSegmentCollection                     = 1                                    ) ;

  m_TrkSegmentCollectionLocationList.clear();
  m_TrkSegmentCollectionLocationList.push_back(                                                                              "ConvertedMBoySegments"               ) ;
  declareProperty("TrkSegmentCollectionLocationList"                       , m_TrkSegmentCollectionLocationList                                                    ) ;

//-----------------------------------------------------------------
// McEventCollection to be Viewed

  declareProperty("ViewMcEventCollection"                                  , m_ViewMcEventCollection                        = 1                                    ) ;
  declareProperty("PtMinID"                                                , m_PtMinID                                      = 0.5*CLHEP::GeV                       ) ;

  m_McEventCollectionLocationList.clear();
  m_McEventCollectionLocationList.push_back(                                                                                 "TruthEvent"                          ) ;
  declareProperty("McEventCollectionLocationList"                          , m_McEventCollectionLocationList                                                       ) ;

//-----------------------------------------------------------------
// EgammaContainer to be Viewed

  declareProperty("ViewEgammaContainer"                                    , m_ViewEgammaContainer                          = 1                                    ) ;
  declareProperty("EgammaEMTrackMatchContainerName"                        , m_EMTrackMatchContainerName                    = "EMTrackMatchContainer"              ) ;
  declareProperty("EgammaEtCut"                                            , m_etEgammaCut                                  = 2.0*CLHEP::GeV                              ) ;
  declareProperty("EgammaEtaCut"                                           , m_etaEgammaCut                                 = 2.5                                  ) ;

  m_EgammaContainerLocationList.clear();
//m_EgammaContainerLocationList.push_back(                                                                                   "egammaCollection"                    ) ;
  m_EgammaContainerLocationList.push_back(                                                                                   "ElectronCollection"                  ) ;
  m_EgammaContainerLocationList.push_back(                                                                                   "PhotonCollection"                    ) ;
  declareProperty("EgammaContainerLocationList"                            , m_EgammaContainerLocationList                                                         ) ;

//apolici
//-----------------------------------------------------------------
// JetContainer to be Viewed

  declareProperty("ViewJetContainer"                                    , m_ViewJetContainer                          = 1                                         ) ;
  declareProperty("JetEtCut"                                            , m_etJetCut                                  = 3.0*CLHEP::GeV                                   ) ;
  declareProperty("JetEtaCut"                                           , m_etaJetCut                                 = 2.7                                       ) ;

  m_JetContainerLocationList.clear();
  m_JetContainerLocationList.push_back(                                                                                   "AntiKt4TopoEMJets"                     ) ;
  declareProperty("JetContainerLocationList"                            , m_JetContainerLocationList                                                              ) ;

// METContainer to be Viewed

  declareProperty("ViewMETContainer"                                    , m_ViewMETContainer                          = 1                                         ) ;
  declareProperty("METEtCut"                                            , m_etMETCut                                  = 3.0*CLHEP::GeV                                   ) ;

  m_METContainerLocationList.clear();
  m_METContainerLocationList.push_back(                                                                                   "MET_RefFinal"                          ) ;
  declareProperty("METContainerLocationList"                            , m_METContainerLocationList                                                              ) ;

// L1RoIContainer to be Viewed

  declareProperty("ViewL1RoIContainer"                                  , m_ViewL1RoIContainer                          = 1                                         ) ;

  m_L1RoIContainerLocationList.clear();
  m_L1RoIContainerLocationList.push_back(                                                                                   "LVL1_ROI"                          ) ;
  declareProperty("L1RoiContainerLocationList"                          , m_L1RoIContainerLocationList                                                              ) ;
//apolici

//-----------------------------------------------------------------
// VertexContainer to be Viewed

  declareProperty("ViewVertexContainer"                                    , m_ViewVertexContainer                          = 1                                    ) ;

  m_VertexContainerLocationList.clear();
  m_VertexContainerLocationList.push_back(                                                                                   "VxPrimaryCandidate"                  ) ;
  declareProperty("VertexContainerLocationList"                            , m_VertexContainerLocationList                                                         ) ;

//-----------------------------------------------------------------
// Combined dumps to be Viewed

  declareProperty("ViewCombinedDump"                                       , m_ViewCombinedDump                             = 0                                    ) ;

  m_CombinedDumpLocationList.clear();
  m_CombinedDumpLocationList.push_back(                                                                                      "StacoMuonCollection"                 ) ;
  declareProperty("CombinedDumpLocationList"                               , m_CombinedDumpLocationList                                                            ) ;

//-----------------------------------------------------------------
// ID digits to be Viewed
  declareProperty("ViewIDDigits"                                           , m_ViewIDDigits                                 = 0                                    ) ;
  declareProperty("ViewTRTRDO"                                             , m_ViewTRTRDO                                   = 0                                    ) ;
  declareProperty("TRTRDOContainerName"                                    , m_TRTRDOContainerName                          = "TRT_RDOs"                           ) ;
  declareProperty("TRTToTMin"                                              , m_TRTToTMin                                    = 5                                    ) ;
  declareProperty("TRTToTMax"                                              , m_TRTToTMax                                    = 60                                   ) ;
  declareProperty("ViewSCTRDO"                                             , m_ViewSCTRDO                                   = 0                                    ) ;
  declareProperty("SCTRDOContainerName"                                    , m_SCTRDOContainerName                          = "SCT_RDOs"                           ) ;
  declareProperty("ViewPixelRDO"                                           , m_ViewPixelRDO                                 = 0                                    ) ;
  declareProperty("PixelRDOContainerName"                                  , m_PixelRDOContainerName                        = "PixelRDOs"                          ) ;

}

// Initialize
StatusCode MboyView::initialize(){

  if (m_SwitchOff == 1) {
   ATH_MSG_INFO( "================================" ) ;
   ATH_MSG_INFO( "=Proprieties are     " ) ;
   ATH_MSG_INFO( "= SwitchOff             " << m_SwitchOff                   ) ;
   ATH_MSG_INFO( "================================" ) ;
   return StatusCode::SUCCESS;
  }
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;


  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are       " ) ;

  ATH_MSG_INFO( "= NameOfMboyViewOutputFile                       " << m_NameOfMboyViewOutputFile    ) ;

  ATH_MSG_INFO( "= EvtSelectionType                               " << m_EvtSelectionType ) ;
  ATH_MSG_INFO( "= EvtSelectionThreshold                          " << m_EvtSelectionThreshold ) ;
  ATH_MSG_INFO( "= MuonRecDigiStoreTDSLocationForEventSelection   " << m_MuonRecDigiStoreTDSLocationForEventSelection    ) ;
  ATH_MSG_INFO( "= TrkTrackCollectionLocationForEventSelection    " << m_TrkTrackCollectionLocationForEventSelection ) ;

  ATH_MSG_INFO( "= GenEvtSelectionType                            " << m_GenEvtSelectionType ) ;
  ATH_MSG_INFO( "= GenEvtSelectionEtaMin                          " << m_GenEvtSelectionEtaMin ) ;
  ATH_MSG_INFO( "= GenEvtSelectionEtaMax                          " << m_GenEvtSelectionEtaMax ) ;
  ATH_MSG_INFO( "= TrackRecordCollectionLocationForGenEventSelectionList              " ) ;
  int TrackRecordCollectionLocationForGenEventSelectionListSize =  m_TrackRecordCollectionLocationForGenEventSelectionList.size() ;
  for (int TrackRecordCollectionLocationForGenEventSelectionListItem=0; TrackRecordCollectionLocationForGenEventSelectionListItem<TrackRecordCollectionLocationForGenEventSelectionListSize ; TrackRecordCollectionLocationForGenEventSelectionListItem++){
    ATH_MSG_INFO( "=                                                " << m_TrackRecordCollectionLocationForGenEventSelectionList[TrackRecordCollectionLocationForGenEventSelectionListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewDigits                                     " << m_ViewDigits                ) ;
  ATH_MSG_INFO( "= MuonRecDigiStoreTDSLocation                    " << m_MuonRecDigiStoreTDSLocation    ) ;
  ATH_MSG_INFO( "= ViewAthIds                                     " << m_ViewAthIds ) ;
  ATH_MSG_INFO( "= PrintRSLine                                    " << m_PrintRSLine ) ;
  ATH_MSG_INFO( "= ATCOSM                                         " << m_ATCOSM ) ;

  ATH_MSG_INFO( "=*ViewTrackRecordCollection                      " << m_ViewTrackRecordCollection ) ;
  ATH_MSG_INFO( "= TrackRecordCollectionLocationList              " ) ;
  int TrackRecordCollectionLocationListSize =  m_TrackRecordCollectionLocationList.size() ;
  for (int TrackRecordCollectionLocationListItem=0; TrackRecordCollectionLocationListItem<TrackRecordCollectionLocationListSize ; TrackRecordCollectionLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_TrackRecordCollectionLocationList[TrackRecordCollectionLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewCalo                                       " << m_ViewCalo                  ) ;
  ATH_MSG_INFO( "= CaloMinimumE                                   " << m_CaloMinimumE ) ;

  ATH_MSG_INFO( "=*ViewCombinedMuonContainer                      " << m_ViewCombinedMuonContainer                  ) ;
  ATH_MSG_INFO( "= CombinedMuonContainerLocationList              " ) ;
  int CombinedMuonContainerLocationListSize =  m_CombinedMuonContainerLocationList.size() ;
  for (int CombinedMuonContainerLocationListItem=0; CombinedMuonContainerLocationListItem<CombinedMuonContainerLocationListSize ; CombinedMuonContainerLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_CombinedMuonContainerLocationList[CombinedMuonContainerLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewTrackParticleContainer                     " << m_ViewTrackParticleContainer                  ) ;
  ATH_MSG_INFO( "= TrackParticleContainerLocationList             " ) ;
  int TrackParticleContainerLocationListSize =  m_TrackParticleContainerLocationList.size() ;
  for (int TrackParticleContainerLocationListItem=0; TrackParticleContainerLocationListItem<TrackParticleContainerLocationListSize ; TrackParticleContainerLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_TrackParticleContainerLocationList[TrackParticleContainerLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewTrkTrackCollection                         " << m_ViewTrkTrackCollection                  ) ;
  ATH_MSG_INFO( "= TrkTrackCollectionLocationList                 " ) ;
  int TrkTrackCollectionLocationListSize =  m_TrkTrackCollectionLocationList.size() ;
  for (int TrkTrackCollectionLocationListItem=0; TrkTrackCollectionLocationListItem<TrkTrackCollectionLocationListSize ; TrkTrackCollectionLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_TrkTrackCollectionLocationList[TrkTrackCollectionLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewTrkSegmentCollection                       " << m_ViewTrkSegmentCollection                  ) ;
  ATH_MSG_INFO( "= TrkSegmentCollectionLocationList               " ) ;
  int TrkSegmentCollectionLocationListSize =  m_TrkSegmentCollectionLocationList.size() ;
  for (int TrkSegmentCollectionLocationListItem=0; TrkSegmentCollectionLocationListItem<TrkSegmentCollectionLocationListSize ; TrkSegmentCollectionLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_TrkSegmentCollectionLocationList[TrkSegmentCollectionLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewMcEventCollection                           " << m_ViewMcEventCollection                  ) ;
  ATH_MSG_INFO( "= PtMinID                                         " << m_PtMinID ) ;
  ATH_MSG_INFO( "= McEventCollectionLocationList                   " ) ;
  int McEventCollectionLocationListSize =  m_McEventCollectionLocationList.size() ;
  for (int McEventCollectionLocationListItem=0; McEventCollectionLocationListItem<McEventCollectionLocationListSize ; McEventCollectionLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_McEventCollectionLocationList[McEventCollectionLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewEgammaContainer                            " << m_ViewEgammaContainer                  ) ;
  ATH_MSG_INFO( "= EgammaEMTrackMatchContainerName                 " << m_EMTrackMatchContainerName ) ;
  ATH_MSG_INFO( "= EgammaEtCut                                     " << m_etEgammaCut ) ;
  ATH_MSG_INFO( "= EgammaEtaCut                                    " << m_etaEgammaCut ) ;
  ATH_MSG_INFO( "= EgammaContainerLocationList                    " ) ;
  int EgammaContainerLocationListSize =  m_EgammaContainerLocationList.size() ;
  for (int EgammaContainerLocationListItem=0; EgammaContainerLocationListItem<EgammaContainerLocationListSize ; EgammaContainerLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_EgammaContainerLocationList[EgammaContainerLocationListItem] ) ;
  }


//apolici
  ATH_MSG_INFO( "=*ViewJetContainer                             " << m_ViewJetContainer                  ) ;
  ATH_MSG_INFO( "= JetEtCut                                     " << m_etJetCut ) ;
  ATH_MSG_INFO( "= JetEtaCut                                    " << m_etaJetCut ) ;
  ATH_MSG_INFO( "= JetContainerLocationList                     " ) ;
  int JetContainerLocationListSize =  m_JetContainerLocationList.size() ;
  for (int JetContainerLocationListItem=0; JetContainerLocationListItem<JetContainerLocationListSize ; JetContainerLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_JetContainerLocationList[JetContainerLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewMETContainer                             " << m_ViewMETContainer                  ) ;
  ATH_MSG_INFO( "= METEtCut                                     " << m_etMETCut ) ;
  ATH_MSG_INFO( "= METContainerLocationList                     " ) ;
  int METContainerLocationListSize =  m_METContainerLocationList.size() ;
  for (int METContainerLocationListItem=0; METContainerLocationListItem<METContainerLocationListSize ; METContainerLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_METContainerLocationList[METContainerLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewL1RoIContainer                           " << m_ViewL1RoIContainer                  ) ;
  ATH_MSG_INFO( "= L1RoIContainerLocationList                   " ) ;
  int L1RoIContainerLocationListSize =  m_L1RoIContainerLocationList.size() ;
  for (int L1RoIContainerLocationListItem=0; L1RoIContainerLocationListItem<L1RoIContainerLocationListSize ; L1RoIContainerLocationListItem++){
    ATH_MSG_INFO( "=                                              " << m_L1RoIContainerLocationList[L1RoIContainerLocationListItem] ) ;
  }
//apolici

  ATH_MSG_INFO( "=*ViewVertexContainer                            " << m_ViewVertexContainer                  ) ;
  ATH_MSG_INFO( "= VertexContainerLocationList                    " ) ;
  int VertexContainerLocationListSize =  m_VertexContainerLocationList.size() ;
  for (int VertexContainerLocationListItem=0; VertexContainerLocationListItem<VertexContainerLocationListSize ; VertexContainerLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_VertexContainerLocationList[VertexContainerLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewCombinedDump                            " << m_ViewCombinedDump                  ) ;
  ATH_MSG_INFO( "= CombinedDumpLocationList                    " ) ;
  int CombinedDumpLocationListSize =  m_CombinedDumpLocationList.size() ;
  for (int CombinedDumpLocationListItem=0; CombinedDumpLocationListItem<CombinedDumpLocationListSize ; CombinedDumpLocationListItem++){
    ATH_MSG_INFO( "=                                                " << m_CombinedDumpLocationList[CombinedDumpLocationListItem] ) ;
  }

  ATH_MSG_INFO( "=*ViewIDDigits                                   " << m_ViewIDDigits                   ) ;
  ATH_MSG_INFO( "= ViewTRTRDO                                     " << m_ViewTRTRDO                     ) ;
  ATH_MSG_INFO( "=  TRTToTMin                                     " << m_TRTToTMin                      ) ;
  ATH_MSG_INFO( "=  TRTToTMax                                     " << m_TRTToTMax                      ) ;
  ATH_MSG_INFO( "=  TRTRDOConateinerName                          " << m_TRTRDOContainerName            ) ;
  ATH_MSG_INFO( "= ViewSCTRDO                                     " << m_ViewSCTRDO                     ) ;
  ATH_MSG_INFO( "=  SCTRDOConateinerName                          " << m_SCTRDOContainerName            ) ;
  ATH_MSG_INFO( "= ViewPixelRDO                                   " << m_ViewPixelRDO                   ) ;
  ATH_MSG_INFO( "=  PixelRDOConateinerName                        " << m_PixelRDOContainerName          ) ;


  ATH_MSG_INFO( "=*SwitchOff                                      " << m_SwitchOff                   ) ;
  ATH_MSG_INFO( "================================" ) ;


//Get Tools services
  IToolSvc* toolsvc;
  sc=service("ToolSvc",toolsvc);
  if( sc.isFailure() ){
    ATH_MSG_FATAL( "Could not get tool sevices ") ; 
    return( StatusCode::FAILURE );
  }

  if ( p_MboyViewEventSelectionTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewEventSelectionTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewEventSelectionTool ) ;

  if ( p_MboyViewGenEventSelectionTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewGenEventSelectionTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewGenEventSelectionTool ) ;

  if ( p_MboyViewEventInfoTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewEventInfoTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewEventInfoTool ) ;


  if (m_ViewDigits==1){
    if ( p_MboyViewMuonRecDigiStoreTDSTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewMuonRecDigiStoreTDSTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewMuonRecDigiStoreTDSTool ) ;
  }

  if (m_ViewIDDigits==1){
    if ( p_MboyViewIDRecDigiTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewIDRecDigiTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " <<  p_MboyViewIDRecDigiTool ) ;
  }

  if (m_ViewTrackRecordCollection==1){
    if ( p_MboyViewTrackRecordCollectionTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewTrackRecordCollectionTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewTrackRecordCollectionTool ) ;
  }

  if (m_ViewCalo==1){
    if ( p_MboyViewCaloTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewCaloTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewCaloTool ) ;
  }

  if (m_ViewCombinedMuonContainer==1){
    if ( p_MboyViewCombinedMuonContainerTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewCombinedMuonContainerTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewCombinedMuonContainerTool ) ;
  }

  if (m_ViewTrackParticleContainer==1){
    if ( p_MboyViewTrackParticleContainerTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewTrackParticleContainerTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewTrackParticleContainerTool ) ;
  }

  if (m_ViewTrkTrackCollection==1){
    if ( p_MboyViewTrkTrackCollectionTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewTrkTrackCollectionTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewTrkTrackCollectionTool ) ;
  }

  if (m_ViewTrkSegmentCollection==1){
    if ( p_MboyViewTrkSegmentCollectionTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewTrkSegmentCollectionTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewTrkSegmentCollectionTool ) ;
  }

  if (m_ViewMcEventCollection==1){
    if ( p_MboyViewMcEventCollectionTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewMcEventCollectionTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewMcEventCollectionTool ) ;
  }

  if (m_ViewEgammaContainer==1){
    if ( p_MboyViewEgammaContainerTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewEgammaContainerTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewEgammaContainerTool ) ;
  }

//apolici
  if (m_ViewJetContainer==1){
    if ( p_MboyViewJetContainerTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewJetContainerTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewJetContainerTool ) ;
  }


  if (m_ViewMETContainer==1){
    if ( p_MboyViewMETContainerTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewMETContainerTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewMETContainerTool ) ;
  }


  if (m_ViewL1RoIContainer==1){
    if ( p_MboyViewL1RoIContainerTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewL1RoIContainerTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewL1RoIContainerTool ) ;
  }
//apolici

  if (m_ViewVertexContainer==1){
    if ( p_MboyViewVertexContainerTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewVertexContainerTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewVertexContainerTool ) ;
  }

  if (m_ViewCombinedDump==1){
    if ( p_MboyViewDumpCombinedMuonContainerTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_MboyViewDumpCombinedMuonContainerTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewDumpCombinedMuonContainerTool ) ;
  }

  //Set the file to dump the View
  m_OutMboyView.open(m_NameOfMboyViewOutputFile.c_str());
  m_OutMboyView << setiosflags(std::ios::fixed);
  
  //Set the file to dump the View
  m_OutMboyViewCombinedDumpList.clear() ;
  for (int CombinedDumpLocationListItem=0; CombinedDumpLocationListItem<CombinedDumpLocationListSize ; CombinedDumpLocationListItem++){
    std::string FileName = m_NameOfMboyViewOutputFileCombinedDump + m_CombinedDumpLocationList[CombinedDumpLocationListItem] ;
    std::ofstream* pofstream = new std::ofstream(FileName.c_str()) ;
    *pofstream << setiosflags(std::ios::fixed);
    m_OutMboyViewCombinedDumpList.push_back(pofstream);
  }
  
  ATH_MSG_INFO( "Initialisation ended     " ) ;
  
  return StatusCode::SUCCESS;
  
}


// Execute
StatusCode MboyView::execute(){

  if (m_SwitchOff == 1) return StatusCode::SUCCESS;
  StatusCode sc = StatusCode::SUCCESS;


//Select Evt
  if (p_MboyViewEventSelectionTool) {
    sc = p_MboyViewEventSelectionTool->EventSelection(
                                        m_MuonRecDigiStoreTDSLocationForEventSelection,
                                        m_TrkTrackCollectionLocationForEventSelection,
                                        m_EvtSelectionType,
                                        m_EvtSelectionThreshold
                                       ) ;
    if (sc.isFailure())  return StatusCode::SUCCESS;
  }


//Select Gen Evt
  if (p_MboyViewGenEventSelectionTool) {
    sc = p_MboyViewGenEventSelectionTool->EventSelection(
   					   m_TrackRecordCollectionLocationForGenEventSelectionList,
					   m_GenEvtSelectionType,
					   m_GenEvtSelectionEtaMin,
					   m_GenEvtSelectionEtaMax
   					  );
    if (sc.isFailure())  return StatusCode::SUCCESS;
  }


//View Event info
  if (p_MboyViewEventInfoTool) {
    sc = p_MboyViewEventInfoTool->ViewEventInfo(
                                   m_MuonRecDigiStoreTDSLocationForEventSelection,
                                   m_ViewAthIds,
                                   m_ATCOSM,
                                   &m_OutMboyView
                                  );
    if (sc.isFailure()){
      ATH_MSG_FATAL( " ViewEventInfo failed " ) ;
      return( StatusCode::FAILURE );
    }
  }


//View digits
  if (m_ViewDigits==1){
    if (p_MboyViewMuonRecDigiStoreTDSTool) {
      sc = p_MboyViewMuonRecDigiStoreTDSTool->ViewMuonRecDigiStoreTDS(
                                               m_MuonRecDigiStoreTDSLocation,
                                               m_ViewAthIds,
                                               m_PrintRSLine,
                                               &m_OutMboyView
                                              );
      if (sc.isFailure()){
        ATH_MSG_FATAL( " ViewMuonRecDigiStoreTDS failed " ) ;
        return( StatusCode::FAILURE );
      }
    }
  }


//View ID digits
  if (m_ViewIDDigits==1){
    if (p_MboyViewIDRecDigiTool) {

      if (m_ViewTRTRDO){
        sc = p_MboyViewIDRecDigiTool->TRTRDORetrive(m_TRTRDOContainerName,m_TRTToTMin,m_TRTToTMax,&m_OutMboyView);
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewMuonIDRecDigiTool TRTRDORetriv failed " ) ;
          return( StatusCode::FAILURE );
        }
      }

      if (m_ViewSCTRDO){
        sc = p_MboyViewIDRecDigiTool->SCTRDORetrive(m_SCTRDOContainerName,&m_OutMboyView);
        if (sc.isFailure()){
          ATH_MSG_FATAL( "  ViewMuonIDRecDigiTool SCTRDORetrive failed " ) ;
          return( StatusCode::FAILURE );
        }
      }

      if (m_ViewPixelRDO){
        sc = p_MboyViewIDRecDigiTool->PixelRDORetrive(m_PixelRDOContainerName,&m_OutMboyView);
        if (sc.isFailure()){
          ATH_MSG_FATAL( "  ViewMuonIDRecDigiTool PixelRDORetrive failed " ) ;
          return( StatusCode::FAILURE );
        }
      }

    }
  }


//View TrackRecordCollection
  if (m_ViewTrackRecordCollection==1){
    if (p_MboyViewTrackRecordCollectionTool) {
      int TrackRecordCollectionLocationListSize =  m_TrackRecordCollectionLocationList.size() ;
      for (int TrackRecordCollectionLocationListItem=0; TrackRecordCollectionLocationListItem<TrackRecordCollectionLocationListSize ; TrackRecordCollectionLocationListItem++){
        sc = p_MboyViewTrackRecordCollectionTool->ViewTrackRecordCollection(
                                                   m_TrackRecordCollectionLocationList[TrackRecordCollectionLocationListItem],
                                                   &m_OutMboyView
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewTrackRecordCollection failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }

//View McEventCollection
  if (m_ViewMcEventCollection==1){
    if (p_MboyViewMcEventCollectionTool) {
      int McEventCollectionLocationListSize =  m_McEventCollectionLocationList.size() ;
      for (int McEventCollectionLocationListItem=0; McEventCollectionLocationListItem<McEventCollectionLocationListSize ; McEventCollectionLocationListItem++){
        sc = p_MboyViewMcEventCollectionTool->ViewMcEventCollection(
                                                   m_McEventCollectionLocationList[McEventCollectionLocationListItem],
                                                   &m_OutMboyView,
                                                   m_PtMinID
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewMcEventCollection failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }



//View Calo
  if (m_ViewCalo==1){
    if (p_MboyViewCaloTool) {
      sc = p_MboyViewCaloTool->ViewCalo(
                                &m_OutMboyView,
                                m_CaloMinimumE
                               );
      if (sc.isFailure()){
        ATH_MSG_FATAL( " ViewCalo failed " ) ;
        return( StatusCode::FAILURE );
      }
    }
  }


//View CombinedMuonContainer
  if (m_ViewCombinedMuonContainer==1){
    if (p_MboyViewCombinedMuonContainerTool) {
      int CombinedMuonContainerLocationListSize =  m_CombinedMuonContainerLocationList.size() ;
      for (int CombinedMuonContainerLocationListItem=0; CombinedMuonContainerLocationListItem<CombinedMuonContainerLocationListSize ; CombinedMuonContainerLocationListItem++){
        sc = p_MboyViewCombinedMuonContainerTool->ViewCombinedMuonContainer(
                                                    m_CombinedMuonContainerLocationList[CombinedMuonContainerLocationListItem],
                                                    &m_OutMboyView
                                                   );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewCombinedMuonContainer failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }


//View TrackParticleContainer
  if (m_ViewTrackParticleContainer==1){
    if (p_MboyViewTrackParticleContainerTool) {
      int TrackParticleContainerLocationListSize =  m_TrackParticleContainerLocationList.size() ;
      for (int TrackParticleContainerLocationListItem=0; TrackParticleContainerLocationListItem<TrackParticleContainerLocationListSize ; TrackParticleContainerLocationListItem++){
        sc = p_MboyViewTrackParticleContainerTool->ViewTrackParticleContainer(
                                                     m_TrackParticleContainerLocationList[TrackParticleContainerLocationListItem],
                                                     &m_OutMboyView
                                                    );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewTrackParticleContainer failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }


//View TrkTrackCollection
  if (m_ViewTrkTrackCollection==1){
    if (p_MboyViewTrkTrackCollectionTool) {
      int TrkTrackCollectionLocationListSize =  m_TrkTrackCollectionLocationList.size() ;
      for (int TrkTrackCollectionLocationListItem=0; TrkTrackCollectionLocationListItem<TrkTrackCollectionLocationListSize ; TrkTrackCollectionLocationListItem++){
        sc = p_MboyViewTrkTrackCollectionTool->ViewTrkTrackCollection(
                                                   m_TrkTrackCollectionLocationList[TrkTrackCollectionLocationListItem],
                                                   &m_OutMboyView
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewTrkTrackCollection failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }



//View EgammaContainer
  if (m_ViewEgammaContainer==1){
    if (p_MboyViewEgammaContainerTool) {
      int EgammaContainerLocationListSize =  m_EgammaContainerLocationList.size() ;
      for (int EgammaContainerLocationListItem=0; EgammaContainerLocationListItem<EgammaContainerLocationListSize ; EgammaContainerLocationListItem++){
        sc = p_MboyViewEgammaContainerTool->ViewEgammaContainer(
                                                   m_EgammaContainerLocationList[EgammaContainerLocationListItem],
						   m_EMTrackMatchContainerName,
                                                   &m_OutMboyView,
                                                   m_etEgammaCut, m_etaEgammaCut,
						   m_CaloMinimumE
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewEgammaContainer failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }

//apolici
//View JetContainer
  if (m_ViewJetContainer==1){
    if (p_MboyViewJetContainerTool) {
      int JetContainerLocationListSize =  m_JetContainerLocationList.size() ;
      for (int JetContainerLocationListItem=0; JetContainerLocationListItem<JetContainerLocationListSize ; JetContainerLocationListItem++){
        sc = p_MboyViewJetContainerTool->ViewJetContainer(
                                                   m_JetContainerLocationList[JetContainerLocationListItem],
                                                   &m_OutMboyView,
                                                   m_etJetCut, m_etaJetCut
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewJetContainer failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }

//View METContainer
  if (m_ViewMETContainer==1){
    if (p_MboyViewMETContainerTool) {
      int METContainerLocationListSize =  m_METContainerLocationList.size() ;
      for (int METContainerLocationListItem=0; METContainerLocationListItem<METContainerLocationListSize ; METContainerLocationListItem++){
        sc = p_MboyViewMETContainerTool->ViewMETContainer(
                                                   m_METContainerLocationList[METContainerLocationListItem],
                                                   &m_OutMboyView,
                                                   m_etMETCut
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewMETContainer failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }

//View L1RoIContainer
  if (m_ViewL1RoIContainer==1){
    if (p_MboyViewL1RoIContainerTool) {
      int L1RoIContainerLocationListSize =  m_L1RoIContainerLocationList.size() ;
      for (int L1RoIContainerLocationListItem=0; L1RoIContainerLocationListItem<L1RoIContainerLocationListSize ; L1RoIContainerLocationListItem++){
        sc = p_MboyViewL1RoIContainerTool->ViewL1RoIContainer(
                                                   m_L1RoIContainerLocationList[L1RoIContainerLocationListItem],
                                                   &m_OutMboyView
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewL1RoIContainer failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }
//apolici

//View VertexContainer
  if (m_ViewVertexContainer==1){
    if (p_MboyViewVertexContainerTool) {
      int VertexContainerLocationListSize =  m_VertexContainerLocationList.size() ;
      for (int VertexContainerLocationListItem=0; VertexContainerLocationListItem<VertexContainerLocationListSize ; VertexContainerLocationListItem++){
        sc = p_MboyViewVertexContainerTool->ViewVertexContainer(
                                                   m_VertexContainerLocationList[VertexContainerLocationListItem],
                                                   &m_OutMboyView
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewVertexContainer failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }

// Dump  CombinedMuonContainer Viewer  
  if (m_ViewCombinedDump==1){
    if (p_MboyViewDumpCombinedMuonContainerTool) {
      int CombinedDumpLocationListSize =  m_CombinedDumpLocationList.size() ;
      for (int CombinedDumpLocationListItem=0; CombinedDumpLocationListItem<CombinedDumpLocationListSize ; CombinedDumpLocationListItem++){
        sc = p_MboyViewDumpCombinedMuonContainerTool->ViewCombinedDump(
                                                   m_CombinedDumpLocationList[CombinedDumpLocationListItem],
                                                   m_OutMboyViewCombinedDumpList[CombinedDumpLocationListItem]
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewCombinedDump failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }


//View TrkSegmentCollection
  if (m_ViewTrkSegmentCollection==1){
    if (p_MboyViewTrkSegmentCollectionTool) {
      int TrkSegmentCollectionLocationListSize =  m_TrkSegmentCollectionLocationList.size() ;
      for (int TrkSegmentCollectionLocationListItem=0; TrkSegmentCollectionLocationListItem<TrkSegmentCollectionLocationListSize ; TrkSegmentCollectionLocationListItem++){
        sc = p_MboyViewTrkSegmentCollectionTool->ViewTrkSegmentCollection(
                                                   m_TrkSegmentCollectionLocationList[TrkSegmentCollectionLocationListItem],
                                                   &m_OutMboyView
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewTrkSegmentCollection failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }
  }


//Tag end of evt dump
  m_OutMboyView <<"END EVT"<<std::endl;


  return StatusCode::SUCCESS;

}
// Finalize
StatusCode MboyView::finalize(){return StatusCode::SUCCESS;}

