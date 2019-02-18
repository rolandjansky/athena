/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DenseEnvironmentsAmbiguityProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkTrack/TrackInfo.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"

#include <map>
#include <ext/functional>
#include <iterator>
#include "TString.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetIdentifier/PixelID.h"


#ifdef SIMPLEAMBIGPROCDEBUGCODE
  #include "TrkTruthData/TrackTruthCollection.h"
  #include "InDetIdentifier/PixelID.h"
  #include "InDetPrepRawData/SCT_ClusterContainer.h"
  #include "InDetPrepRawData/PixelClusterContainer.h"
  #include "InDetPrepRawData/TRT_DriftCircle.h"
  #include "HepMC/GenVertex.h"
  #include "GaudiKernel/IPartPropSvc.h"
  #include "TrkToolInterfaces/ITruthToTrack.h"
  #include "GeneratorObjects/McEventCollection.h"
#endif

bool Trk::DenseEnvironmentsAmbiguityProcessorTool::_checkTrack( const Trk::Track *track) const {
	  if (!track )return true;
	
	  bool error=false;
	  if (track->trackParameters()){
	    int counter=0;
	    for (const  Trk::TrackParameters *param: *(track->trackParameters())) {
	      if (param && param->covariance() && param->covariance()->determinant() < 0) {
	        ATH_MSG_DEBUG( " negative determinant for track param " << counter << " "
	                       << *(param)  << " cov=" << *(param->covariance())
	                       << std::endl
	                       << " det=" << param->covariance()->determinant() );
	        error=true;
	      }
	      ++counter;
	      if (counter>=2) break;
	    }
	  }
	  return !error;
}



//==================================================================================================
Trk::DenseEnvironmentsAmbiguityProcessorTool::DenseEnvironmentsAmbiguityProcessorTool(const std::string& t, 
                                const std::string& n,
                                const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_particleHypothesis{undefined},
  m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"), 
  m_observerTool("Trk::TrkObserverTool/TrkObserverTool"),
  m_extrapolatorTool("Trk::Extrapolator/AtlasExtrapolator"),
  m_selectionTool("InDet::InDetDenseEnvAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool"),
  m_splitProbTool("InDet::NnPixelClusterSplitProbTool/NnPixelClusterSplitProbTool"),  
  m_assoTool("Trk::PRD_AssociationTool/DEAmbi_PRD_AssociationTool"),
  m_finalTracks(0),
  m_Nevents(0),
  m_Ncandidates(4), m_NcandScoreZero(4), m_NcandDouble(4),
  m_NscoreOk(4),m_NscoreZeroBremRefit(4),m_NscoreZeroBremRefitFailed(4),
  m_NscoreZeroBremRefitScoreZero(4),m_NscoreZero(4),
  m_Naccepted(4),m_NsubTrack(4),m_NnoSubTrack(4),m_NacceptedBrem(4),
  m_NbremFits(4),m_Nfits(4),m_NrecoveryBremFits(4),m_NgoodFits(4),m_NfailedFits(4),
  m_monitorTracks(false),
  m_splitClusterMapName("SplitClusterAmbiguityMap") //Unused Remove later
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  ,m_truthToTrack(0)//the comma in front of m_truthToTrack is necessary  
#endif
  ,m_rejectInvalidTracks(false)
{
  // statitics stuff
  m_etabounds.push_back(0.8);
  m_etabounds.push_back(1.6);
  m_etabounds.push_back(2.5);
  m_fitterTool.push_back("Trk::KalmanFitter/InDetTrackFitter");

  declareInterface<ITrackAmbiguityProcessorTool>(this);
  declareProperty("DropDouble"           , m_dropDouble         = true);
  declareProperty("ForceRefit"           , m_forceRefit         = true);
  declareProperty("RefitPrds"            , m_refitPrds          = true); //  True to allow for updated NN information to be taken into account
  declareProperty("applydRcorrection"    , m_applydRcorrection  = false);
  declareProperty("MatEffects"           , m_matEffects         = 3); // pion
  declareProperty("ScoringTool"          , m_scoringTool);
  declareProperty("ObserverTool"         , m_observerTool);
  declareProperty("SelectionTool"        , m_selectionTool);
  declareProperty("Fitter"               , m_fitterTool );
  declareProperty("TrackExtrapolator"    , m_extrapolatorTool);
  declareProperty("SplitProbTool"        , m_splitProbTool);
  declareProperty("AssociationTool"      , m_assoTool);
  declareProperty("SuppressHoleSearch"   , m_suppressHoleSearch = false);
  declareProperty("SuppressTrackFit"     , m_suppressTrackFit   = false);
  declareProperty("tryBremFit"           , m_tryBremFit         = false);
  declareProperty("caloSeededBrem"       , m_caloSeededBrem     = false);
  declareProperty("pTminBrem"            , m_pTminBrem          = 1000.);
  declareProperty("etaBounds"            , m_etabounds,"eta intervals for internal monitoring");
  declareProperty("sharedProbCut"        , m_sharedProbCut           = 0.3);
  declareProperty("sharedProbCut2"       , m_sharedProbCut2          = 0.3);
  declareProperty("SplitClusterAmbiguityMap" , m_splitClusterMapName);//Unused Remove later
  declareProperty("MonitorAmbiguitySolving"  , m_monitorTracks = false);

  //To determine the ROI for high pt Bs
  declareProperty("doHadCaloSeed"        ,m_useHClusSeed = false );
  declareProperty("minPtBjetROI"         ,m_minPtBjetROI = 15000.); //inMeV
  declareProperty("phiWidth"             ,m_phiWidth     = 0.1   );
  declareProperty("etaWidth"             ,m_etaWidth     = 0.1   );
  declareProperty("InputHadClusterContainerName",m_inputHadClusterContainerName="InDetHadCaloClusterROIs");

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  declareProperty("ResolvedTrackConnection", m_resolvedTrackConnection="SiSPS_ResolvedTrackConnection");
  declareProperty("TruthCollection", m_truthCollection="SiSPSeededTracksTruthCollection");
  //to get brem truth
  declareProperty("GeneratedEventCollection", m_generatedEventCollectionName="TruthEvent");

  declareProperty("TruthLocationPixel", m_truth_locationPixel="PRD_MultiTruthPixel");
  declareProperty("TruthLocationSCT", m_truth_locationSCT="PRD_MultiTruthSCT");
  declareProperty("TruthLocationTRT", m_truth_locationTRT="PRD_MultiTruthTRT");

  declare(m_write_resolvedTrackConnection);
#endif
  declareProperty("RejectTracksWithInvalidCov"     ,m_rejectInvalidTracks   );
}
//==================================================================================================

Trk::DenseEnvironmentsAmbiguityProcessorTool::~DenseEnvironmentsAmbiguityProcessorTool()
{
}
//==================================================================================================

StatusCode Trk::DenseEnvironmentsAmbiguityProcessorTool::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_CHECK( m_scoringTool.retrieve());

  if (m_monitorTracks) {
    sc = m_observerTool.retrieve(); //Dot, not asterik! This is a method of the observerTool, not of the tool it holds.
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Failed to retrieve AlgTool " << m_observerTool);
      m_monitorTracks = false;
      sc=StatusCode::RECOVERABLE;
      //return sc;		// continue without observer tool
    }
    else 
      ATH_MSG_INFO( "Retrieved tool " << m_observerTool );
  }
  else {
    m_observerTool.disable();
  }

  ATH_CHECK( m_selectionTool.retrieve());
  ATH_CHECK( m_fitterTool.retrieve());
  if (m_fitterTool.empty()){
    ATH_MSG_FATAL("Failed to retrieve tool " << m_fitterTool );
    sc = StatusCode::FAILURE;
    return sc;
   }

  ATH_CHECK( m_extrapolatorTool.retrieve());
  
  if (!m_splitProbTool.empty() && m_splitProbTool.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Could not retrieve the split probability tool " << m_splitProbTool << "'.");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_splitProbTool );  
  }   
  
  ATH_CHECK(m_assoTool.retrieve()) ;
   
  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if (sc.isFailure())
  {
    ATH_MSG_FATAL( "Could not get PixelID helper !" );
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_idHelper, "AtlasID");
  if (sc.isFailure())
  {
    ATH_MSG_FATAL( "Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_assoTool.retrieve()) ;
  
  // suppress refit overwrites force refit
  if (m_forceRefit && m_suppressTrackFit ) 
  {
    ATH_MSG_WARNING("Inconsistent parameter settings, forced refit is true, but fitting suppressed, resetingt force refit !" );
    m_forceRefit = false;
  }

  // Print out memo that tracks have to be fitted
  if (!m_forceRefit) 
  {
    ATH_MSG_INFO( "The forced refit of Tracks is switched off." );
    ATH_MSG_INFO( "Ensure, that the tracks are fitted after the ambiguity processing!" );
  }

  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_particleHypothesis = particleSwitch.particle[m_matEffects];

  // brem fitting enabled ?
  if (m_tryBremFit)
    ATH_MSG_INFO( "Try brem fit and recovery for electron like tracks." );

  //Initialise the ROI tool
  ATH_CHECK(m_inputHadClusterContainerName.initialize(m_useHClusSeed));

  // statistics
  for (int i=0; i<4; i++) {
    m_Ncandidates[i]      = 0;
    m_NcandScoreZero[i]   = 0;
    m_NcandDouble[i]      = 0;
    m_NscoreOk[i]         = 0;
    m_NscoreZeroBremRefit[i]          = 0;
    m_NscoreZeroBremRefitFailed[i]    = 0;
    m_NscoreZeroBremRefitScoreZero[i] = 0;
    m_NscoreZero[i]       = 0;
    m_Naccepted[i]        = 0;
    m_NacceptedBrem[i]    = 0;
    m_NsubTrack[i]        = 0;
    m_NnoSubTrack[i]      = 0;
    m_NbremFits[i]        = 0;
    m_Nfits[i]            = 0;
    m_NrecoveryBremFits[i]= 0;
    m_NgoodFits[i]        = 0;
    m_NfailedFits[i]      = 0;
  }
  if(!m_dRMap.key().empty()){
     ATH_CHECK(m_dRMap.initialize() );
  }
#ifdef SIMPLEAMBIGPROCDEBUGCODE
  // to get the brem truth
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    ATH_MSG_FATAL( "Toll service not found !" );
    return StatusCode::FAILURE;
  }
  
  sc = toolSvc->retrieveTool("Trk::TruthToTrack", m_truthToTrack);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Cannot retrieve the TruthToTrack tool... Exiting" );
    return StatusCode::FAILURE;
  }
  
  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if (sc.isFailure())
  {
    ATH_MSG_FATAL( "Could not get PixelID helper !" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_truth_locationPixel.initialize());
  ATH_CHECK(m_truth_locationSCT.initialize());
  ATH_CHECK(m_truth_locationTRT.initialize());
  ATH_CHECK(m_generatedEventCollectionName.initialize());
  ATH_CHECK(m_truthCollection.initialize());
  m_has_resolvedTrackConnection = m_resolvedTrackConnection.initialize().isSuccess();
  if (!m_has_resolvedTrackConnection) {
    m_write_resolvedTrackConnection = m_resolvedTrackConnection.key();
    ATH_CHECK(m_write_resolvedTrackConnection.initialize());
  }
#endif


  return sc;
}
//==================================================================================================

StatusCode Trk::DenseEnvironmentsAmbiguityProcessorTool::finalize()
{
  ATH_MSG_INFO (name() << "::finalize() -- statistics:");
  StatusCode sc = AlgTool::finalize();
  return sc;
}

void Trk::DenseEnvironmentsAmbiguityProcessorTool::statistics()
{
  ATH_MSG_INFO (name() << " -- statistics:");
  std::streamsize ss = std::cout.precision();
  if (msgLvl(MSG::INFO)) {
    int iw=9;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of events processed      :   "<< m_Nevents << std::endl;
    std::cout << "  statistics by eta range          ------All---Barrel---Trans.-- Endcap-- " << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of candidates at input   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "  - candidates rejected score 0   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandScoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandScoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandScoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandScoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "  - candidates rejected as double :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandDouble[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandDouble[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandDouble[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandDouble[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  candidates with good score      :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreOk[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreOk[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreOk[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreOk[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + recovered after brem refit    :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefit[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefit[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefit[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefit[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    }
    std::cout << "  candidates rejected score 0     :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + rejected failed brem refit    :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitFailed[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitFailed[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitFailed[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitFailed[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
      std::cout << "  + rejected brem refit score 0   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitScoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitScoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitScoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitScoreZero[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    }
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  number of normal fits           :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + 2nd brem fit for failed fit   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
      std::cout << "  normal brem fits for electrons  :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    }
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  sum of succesful fits           :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "  sum of failed fits              :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of subtracks created     :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NsubTrack[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NsubTrack[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NsubTrack[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NsubTrack[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "  Number of candidates excluded   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NnoSubTrack[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NnoSubTrack[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NnoSubTrack[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NnoSubTrack[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of tracks accepted       :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  including number of brem fits   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NacceptedBrem[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NacceptedBrem[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NacceptedBrem[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NacceptedBrem[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap] << std::endl;
    }
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
              << "    definition: ( 0.0 < Barrel < " << m_etabounds[iBarrel-1] << " < Transition < " << m_etabounds[iTransi-1]
              << " < Endcap < " << m_etabounds[iEndcap-1] << " )" << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
  }
  std::cout.precision (ss);
  return;
}

//==================================================================================================

/** helper function for statistics */

void Trk::DenseEnvironmentsAmbiguityProcessorTool::increment_by_eta(std::vector<int>& Ntracks, const Track* track, bool updateAll) {

  if (updateAll) Ntracks[Trk::DenseEnvironmentsAmbiguityProcessorTool::iAll] += 1;

  // test
  if (!track)
  {
     ATH_MSG_ERROR ("track pointer zero, should not happen!");
     return;
  }

  // use first parameter 
  if (!track->trackParameters())
  {
    ATH_MSG_WARNING ("No track parameters, needed for statistics code in Trk::DenseEnvironmentsAmbiguityProcessorTool!");
  }
  else
  {
    double eta = track->trackParameters()->front()->eta();
    if (fabs(eta) < m_etabounds[0]) ++Ntracks[Trk::DenseEnvironmentsAmbiguityProcessorTool::iBarrel];
    else if (fabs(eta) < m_etabounds[1]) ++Ntracks[Trk::DenseEnvironmentsAmbiguityProcessorTool::iTransi];
    else if (fabs(eta) < m_etabounds[2]) ++Ntracks[Trk::DenseEnvironmentsAmbiguityProcessorTool::iEndcap];
  }
}

//==================================================================================================

/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

TrackCollection*  Trk::DenseEnvironmentsAmbiguityProcessorTool::process(const TrackCollection* tracks)
{
  
  using namespace std;

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  m_truthPIX = SG::makeHandle(m_truth_locationPixel);
  m_truthSCT = SG::makeHandle(m_truth_locationSCT);
  m_truthTRT = SG::makeHandle(m_truth_locationTRT);
#endif
 
  ++m_Nevents; // statistics

  // clear all caches etc.
  reset();
  
  if (m_monitorTracks) {
    m_observerTool->storeInputTracks(*tracks);
    //m_observerTool->dumpTrackMap();
  }
  
  //put tracks into maps etc
  ATH_MSG_DEBUG ("Adding input track candidates to list");
  addNewTracks(tracks);
 


  // The search for overlapping tracks 
  ATH_MSG_DEBUG ("Overlapping Tracks");
  overlapppingTracks();



  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat 

  ATH_MSG_DEBUG ("Solving Tracks");
  solveTracks();
  
  if(m_applydRcorrection)
  {
      std::vector<const Trk::Track*> refit_tracks;
      // create map of track dRs
      storeTrkDistanceMapdR(*m_finalTracks,refit_tracks);
      for(const Trk::Track* track : refit_tracks)
      {
              refitTrack(track);
      }
  }

  if (m_monitorTracks && msgLvl(MSG::INFO)) m_observerTool->dumpTrackMap();

  if (m_monitorTracks)
    m_observerTool->saveTracksToxAOD();

  if (msgLvl(MSG::DEBUG)) dumpTracks(*m_finalTracks);
  
  // memory defragmantation fix. Cleaning before returning the result 
  m_prdSigSet.clear(); 
  m_trackScoreTrackMap.clear(); 
   
  return m_finalTracks;
}

void Trk::DenseEnvironmentsAmbiguityProcessorTool::reset()
{
  //this is a map which contains pointers to copies of all the input tracks
  m_trackScoreTrackMap.clear();

  //Signature Set
  m_prdSigSet.clear();
  
  // clear prdAssociationTool via selection tool
  m_selectionTool->reset();

  //final copy - ownership is passed out of algorithm
  m_finalTracks = new TrackCollection;
  
  if (m_monitorTracks)
    m_observerTool->reset();

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  numOutliersDiff  = 0;
  numFirstFitLost  = 0;
  numSecondFitLost = 0;
  truthBefore      = 0;
  truthAfter       = 0;
#endif

  return;
}

//==================================================================================================
void Trk::DenseEnvironmentsAmbiguityProcessorTool::addNewTracks(const TrackCollection* tracks)
{
  using namespace std;

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  findTrueTracks(tracks);
#endif

  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks->size());
 
  TrackCollection::const_iterator trackIt    = tracks->begin();
  TrackCollection::const_iterator trackItEnd = tracks->end();

 
  for ( ; trackIt != trackItEnd ; ++trackIt)
  {

#ifdef SIMPLEAMBIGPROCDEBUGCODE   
    numOutliersBefore = 0;
    numOutliersAfter  = 0;
#endif


    ATH_MSG_DEBUG ("Processing track candidate "<<*trackIt);
    // statistics
    increment_by_eta(m_Ncandidates,*trackIt);
  
    bool reject = false;
  
    // only fitted tracks get hole search, input is not fitted
    TrackScore score = m_scoringTool->score( **trackIt, true);


    // veto tracks with score 0
    if (score==0) { 
      ATH_MSG_DEBUG ("Candidate score is zero, reject it");
      if (m_monitorTracks)
        m_observerTool->updateTrackMap(**trackIt, static_cast<double>(score), 2);		// rejection location 2: when score is zero
      // statistic
      increment_by_eta(m_NcandScoreZero,*trackIt);
      reject = true;
      
#ifdef SIMPLEAMBIGPROCDEBUGCODE
      if( isTrueTrack(*trackIt)){
        ATH_MSG_INFO( "the # of Outliers before is: " << numOutliersBefore << " and after is: " << numOutliersAfter);
        if(numOutliersBefore != numOutliersAfter){
          ATH_MSG_INFO("Rejecting True Track:"<< origTrack(*trackIt) << " Because of Outlier change" ); 
          numOutliersDiff++;
        } else {
          ATH_MSG_INFO("Rejecting True Track:"<< origTrack(*trackIt) << " Because of Zero Score" ); 
        }
        prdTruth(*trackIt);
        tsosTruth(*trackIt);
      }
#endif
            
    } else {

      ATH_MSG_DEBUG ("Track Score is "<< score);
      
      if (m_monitorTracks)
        m_observerTool->updateScore(**trackIt, static_cast<double>(score));		// save score for this observed track
      // double track rejection
      if (m_dropDouble) {
        std::vector<const Trk::PrepRawData*> prds = m_selectionTool->getPrdsOnTrack(*trackIt);

        // unfortunately PrepRawDataSet is not a set !
        PrdSignature prdSig;
        prdSig.insert( prds.begin(),prds.end() );

        // we try to insert it into the set, if we fail (pair.second), it then exits already
        if ( !(m_prdSigSet.insert(prdSig)).second ) {
          ATH_MSG_DEBUG ("Double track, reject it !");
          // statistic
          increment_by_eta(m_NcandDouble,*trackIt);
          reject = true;
          if (m_monitorTracks)
            m_observerTool->rejectTrack(**trackIt, 3);		// rejection location 3: double track rejection
        } else {
          ATH_MSG_DEBUG ("Insert new track in PrdSignatureSet");
        }
      }
    }

    if (!reject) {
      // this does a deep copy, we need to clean this
      const Track* track = new Track(**trackIt);

      // add track to map, map is sorted small to big ! set if fitted
      ATH_MSG_VERBOSE ("Track ("<< *trackIt <<" --> "<< track << ") has score "<<score);
      m_trackScoreTrackMap.insert( make_pair(-score, make_pair(track, !m_forceRefit)) );

      if (m_monitorTracks) {
      	m_observerTool->rejectTrack(**trackIt, 1);		// "rejection" location 1: actually no "real rejection" because it is a deep copy, but memory adress (=id) changes
      	m_observerTool->addSubTrack(*track, **trackIt);		// a new generated (copied) track to observer tool
      }

#ifdef SIMPLEAMBIGPROCDEBUGCODE
      keepTrackOfTracks(*trackIt,track);
#endif

    }
  }
  
  ATH_MSG_DEBUG ("Number of tracks in map:"<<m_trackScoreTrackMap.size());

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  int numTrueMap = 0;
  TrackScoreMap::iterator itnext = m_trackScoreTrackMap.begin();
  TrackScoreMap::iterator itend = m_trackScoreTrackMap.end();
  for (; itnext != itend; ++itnext){
    if(isTrueTrack(itnext->second.first))
      numTrueMap++;
  }
  ATH_MSG_INFO("Number of TRUE tracks in map:"<< numTrueMap );
#endif

    
  return;
}

//==================================================================================================
  
void Trk::DenseEnvironmentsAmbiguityProcessorTool::addTrack(const Trk::Track* track, const bool fitted)
{
  using namespace std;

  // compute score
  TrackScore score;
  bool suppressHoleSearch = fitted ? m_suppressHoleSearch : true;

  ATH_MSG_DEBUG ("addTrack()::SuppressHoleSearch "<< suppressHoleSearch);
  ATH_MSG_DEBUG ("addTrack()::Fitted             "<< fitted);

  score = m_scoringTool->score( *track, suppressHoleSearch );

  if (m_monitorTracks)
		m_observerTool->updateScore(*track, static_cast<double>(score));		// update score for observed track

  // do we accept the track ?
  if (score!=0)
  {
    ATH_MSG_DEBUG ("Track  ("<< track <<") has score "<<score);
    // statistic
    increment_by_eta(m_NscoreOk,track);

    // add track to map, map is sorted small to big !
    m_trackScoreTrackMap.insert( make_pair(-score, make_pair(track, fitted)) );

    return;
  }

  // do we try to recover the track ?
  if (score==0 && fitted && m_tryBremFit &&
      !track->info().trackProperties(Trk::TrackInfo::BremFit) &&
      track->trackParameters()->front()->pT() > m_pTminBrem &&
      (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
  {

    ATH_MSG_DEBUG ("Track score is zero, try to recover it via brem fit");
    // run track fit using electron hypothesis
    const Trk::Track* bremTrack = fit(*track,true,Trk::electron);


    if (!bremTrack)
    {
      ATH_MSG_DEBUG ("Brem refit failed, drop track");
      // statistic
      increment_by_eta(m_NscoreZeroBremRefitFailed,track);
      increment_by_eta(m_NfailedFits,track);

      if (m_monitorTracks)	// Update observed Track (reject)			
        m_observerTool->rejectTrack(*track, 6);		// rejection location 6: "Brem refit failed"

      // clean up
      delete(track);
      track=nullptr;

    }
    else
    {
      if (m_monitorTracks) {			
        // Update observed track
        m_observerTool->rejectTrack(*track, 7);		// rejection location 7: "Brem refit worked - new subtrack created"
        m_observerTool->addSubTrack(*bremTrack, *track);		// add new subtrack
      }

      // clean up
      //delete(track); <- too early; still want to use track further down...

      // statistic
      increment_by_eta(m_NgoodFits,bremTrack);

      // rerun score
      score = m_scoringTool->score( *bremTrack, suppressHoleSearch );

      if (m_monitorTracks) {
        // Update observed track score
        m_observerTool->updateScore(*track, static_cast<double>(score));	
      }
      delete(track);
      track=nullptr;

      // do we accept the track ?
      if (score!=0)
      {
        ATH_MSG_DEBUG ("Brem refit successful, recovered track  ("<< track <<") has score "<<score);
        // statistics
        increment_by_eta(m_NscoreZeroBremRefit,bremTrack);

        // add track to map, map is sorted small to big !
        m_trackScoreTrackMap.insert( make_pair(-score, make_pair(bremTrack, fitted)) );
        return;
      }
      else
      {
        ATH_MSG_DEBUG ("Brem refit gave still track score zero, reject it");
        // statistic
        increment_by_eta(m_NscoreZeroBremRefitScoreZero,bremTrack);

        if (m_monitorTracks) {
          // Update observed track
          m_observerTool->rejectTrack(*bremTrack, 8);		// rejection location 8: "Brem refit gave still track score zero"
        }

        // clean up
        delete(bremTrack);
      }
    }
  }
  else  
  {
    ATH_MSG_DEBUG ("Track score is zero, reject it");

    if (m_monitorTracks) {
      // Update observed track
			m_observerTool->rejectTrack(*track, 9);		// rejection location 9: "Refit track score 0"
    }

    // statistic
    increment_by_eta(m_NscoreZero,track);

#ifdef SIMPLEAMBIGPROCDEBUGCODE
    if(isTrueTrack(track)){
      ATH_MSG_INFO( "the # of Outliers before is: " << numOutliersBefore << " and after is: " << numOutliersAfter);
      if(numOutliersBefore != numOutliersAfter){
        ATH_MSG_INFO("Rejecting True Track:"<< origTrack(track) << " Because of Outlier change"); 
        numOutliersDiff++;
      }else {
        ATH_MSG_INFO("Rejecting True Track:"<< origTrack(track) << " Because of Zero Score"); 
      }
      prdTruth(track);
      tsosTruth(track);
    }
#endif   
   
   
   
   // clean up
    delete(track);
  }
  return;
}
//==================================================================================================

void Trk::DenseEnvironmentsAmbiguityProcessorTool::solveTracks()
{
  using namespace std;

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  n_trueFitFails = 0;
  n_fitFails     = 0;
#endif  
  

  ATH_MSG_DEBUG ("Starting to solve tracks");

  // now loop as long as map is not empty
  while ( !m_trackScoreTrackMap.empty() )
  {
    // get current best candidate 
    TrackScoreMap::iterator itnext = m_trackScoreTrackMap.begin();

    // clean it out to make sure not to many shared hits
    ATH_MSG_VERBOSE ("--- Trying next track "<<itnext->second.first<<"\t with score "<<-itnext->first);
    const Trk::Track* cleanedTrack = m_selectionTool->getCleanedOutTrack( itnext->second.first , -(itnext->first));
    //Find the HighPtBROI
    reloadHadROIs();
    bool refitBTracks = decideIfInHighPtBROI(itnext->second.first);

    // cleaned track is input track and fitted
    if (cleanedTrack == itnext->second.first && itnext->second.second )
    {

    
      // track can be kept as is and is already fitted
      ATH_MSG_DEBUG ("Accepted track "<<itnext->second.first<<"\t has score "<<-(itnext->first));
      // statistic
      increment_by_eta(m_Naccepted,itnext->second.first);
      if (m_tryBremFit && itnext->second.first->info().trackProperties(Trk::TrackInfo::BremFit))
        increment_by_eta(m_NacceptedBrem,itnext->second.first);

      //Compute the fitQuality
      double fitQual = 0;
      if ( itnext->second.second ) {
        if  (itnext->second.first->fitQuality() && itnext->second.first->fitQuality()->numberDoF()>0 ){ 
          fitQual = itnext->second.first->fitQuality()->chiSquared()/itnext->second.first->fitQuality()->numberDoF();
        }
      } 
      if(fitQual > 1.3 && refitBTracks){
        const Trk::Track* refittedTrack = refitTracksFromB(itnext->second.first, fitQual);
        if(refittedTrack){
          // add track to PRD_AssociationTool
          StatusCode sc = m_selectionTool->registerPRDs(refittedTrack);
          if (sc.isFailure()) ATH_MSG_ERROR( "addPRDs() failed" );
          // add to output list 
          m_finalTracks->push_back( const_cast<Track*>(refittedTrack) );
          delete (cleanedTrack);
        }
        else{
          // add track to PRD_AssociationTool
          StatusCode sc = m_selectionTool->registerPRDs(itnext->second.first);
          if (sc.isFailure()) ATH_MSG_ERROR( "addPRDs() failed" );
          // add to output list 
          m_finalTracks->push_back( const_cast<Track*>(itnext->second.first) );
        }
      } 
      else {
        // add track to PRD_AssociationTool
        StatusCode sc = m_selectionTool->registerPRDs(itnext->second.first);
        if (sc.isFailure()) ATH_MSG_ERROR( "addPRDs() failed" );
        // add to output list 
        m_finalTracks->push_back( const_cast<Track*>(itnext->second.first) );
      }


      // don't forget to drop track from map
      m_trackScoreTrackMap.erase(itnext);
    }
    else if ( cleanedTrack == itnext->second.first )
    {

#ifdef SIMPLEAMBIGPROCDEBUGCODE
      numOutliersBefore = cleanedTrack->outliersOnTrack()->size();
#endif

      // don't forget to drop track from map
      m_trackScoreTrackMap.erase(itnext);
      // track can be kept as is, but is not yet fitted
      ATH_MSG_DEBUG ("Good track, but need to fit this track first, score, add it into map again and retry !");
      refitTrack(cleanedTrack);
      // delete original copy
      delete (cleanedTrack);
    }
    else if ( cleanedTrack )
    {
    
#ifdef SIMPLEAMBIGPROCDEBUGCODE
      keepTrackOfTracks(itnext->second.first, cleanedTrack);
#endif    
    
      if (m_monitorTracks) {
        // ObserverTool: Track already marked as bad (reject because subtrack created) in the SelectionTool - only need to create the subtrack in the observerMap
        m_observerTool->addSubTrack(*cleanedTrack, *(itnext->second.first));		// add new subtrack, maybe move this to SelectionTool
      }

      // now delete original track
      delete itnext->second.first;
      // don't forget to drop track from map
      m_trackScoreTrackMap.erase(itnext);

      // stripped down version should be reconsidered
      ATH_MSG_DEBUG ("Candidate excluded, add subtrack to map. Track "<<cleanedTrack);
      // statistic
      increment_by_eta(m_NsubTrack,cleanedTrack);

      // track needs fitting !
      addTrack( cleanedTrack, false);
    }
    else
    {       
      // track should be discarded
      ATH_MSG_DEBUG ("Track "<< itnext->second.first << " is excluded, no subtrack, reject");
      // statistic
      increment_by_eta(m_NnoSubTrack,itnext->second.first);

      delete (itnext->second.first);
      // don't forget to drop track from map
      m_trackScoreTrackMap.erase(itnext);
    }
  }
  
  ATH_MSG_DEBUG ("Finished, number of track on output: "<<m_finalTracks->size());

#ifdef SIMPLEAMBIGPROCDEBUGCODE
  TrackCollection::const_iterator  m_itFinal = m_finalTracks->begin();
  TrackCollection::const_iterator m_endFinal = m_finalTracks->end();
  for ( ; m_itFinal != m_endFinal ; ++m_itFinal) {
    if ( isTrueTrack(*m_itFinal) )
      truthAfter++;
  }
  ATH_MSG_INFO("Where, the number of true track on output is: "<<truthAfter );
  ATH_MSG_INFO("And the number of TRUE tracks failed fits is:  "<< n_trueFitFails );
  ATH_MSG_INFO("And the number of TRUE tracks rejected due to outliers is: "<< numOutliersDiff );
  ATH_MSG_INFO("And the number of TRUE tracks rejected after the first fit is: "<< numFirstFitLost );
  ATH_MSG_INFO("And the number of TRUE tracks rejected after the second fit is: "<< numSecondFitLost );
  
  if(truthBefore != truthAfter)
    ATH_MSG_INFO("The number of tracks lost this events is:  "<< truthBefore-truthAfter );
  
  if (n_trueFitFails >0){
    ATH_MSG_INFO( "DOING THE BREM TRUTH" );
    getBremTruth().isSuccess();
  }
  
  produceInputOutputConnection();
#endif
  
  return;
}

//==================================================================================================

void Trk::DenseEnvironmentsAmbiguityProcessorTool::updatePixelSplitInformationForCluster(const std::pair<const InDet::PixelCluster* const,
                                                                                         const Trk::TrackParameters*> & clusterTrkPara )
  
{  

  // Recalculate the split prob with the use of the track parameters
  InDet::PixelClusterSplitProb splitProb = m_splitProbTool->splitProbability( *clusterTrkPara.first, *clusterTrkPara.second );
  // update the split prob information on the cluster --  the use of the split flag is now questionable -- possible it will now indicate if the cluster is shared between multiple tracks
  InDet::PixelCluster* pixelCluster = const_cast<InDet::PixelCluster*> ( clusterTrkPara.first );    

  ATH_MSG_DEBUG (  "---- "<< pixelCluster->globalPosition().perp() 
                             <<" Updating split probs 1:  Old " << pixelCluster->splitProbability1() << "  New " << splitProb.splitProbability(2) 
                             <<" Probs 2:  Old " << pixelCluster->splitProbability2() << "  New " << splitProb.splitProbability(3) 
                             << std::endl
                             << " --- pixelCluster: " <<  *pixelCluster
                             << std::endl
                             << " --- trk params: " << *clusterTrkPara.second  );

  if ( splitProb.splitProbability(2)  < 0 ){
    pixelCluster->packSplitInformation( false, 0.0, 0.0 );    
    pixelCluster->setTooBigToBeSplit( true );    
  }else{  
    pixelCluster->packSplitInformation( false, splitProb.splitProbability(2), splitProb.splitProbability(3) ) ;
    pixelCluster->setTooBigToBeSplit( false );    
  }
  



}



// Note need to double check what is filled in the TSOS for the SiS tracks
void Trk::DenseEnvironmentsAmbiguityProcessorTool::updatePixelSplitInformation(std::map< const InDet::PixelCluster*, const Trk::TrackParameters* >& setOfClustersOnTrack)
{
	
  ATH_MSG_DEBUG ("---> Updating " << setOfClustersOnTrack.size() << " pixel clusters");
  for( auto clusterTrkPara : setOfClustersOnTrack){
    // Check to see if this cluster has been updated using an earlier instance of the AmbiguityProcessor
    // If it has already been updated dont do it again 
		
    // Recaculate the split prob with the use of the track parameters
    InDet::PixelClusterSplitProb splitProb = m_splitProbTool->splitProbability( *clusterTrkPara.first, *clusterTrkPara.second );
    // update the split prob information on the cluster --  the use of the split flag is now questionable -- possible it will now indicate if the cluster is shared between multiple tracks
    InDet::PixelCluster* pixelCluster = const_cast<InDet::PixelCluster*> ( clusterTrkPara.first );    

    ATH_MSG_VERBOSE (  "---- "<< pixelCluster->globalPosition().perp() 
                              <<" Updating split probs 1:  Old " << pixelCluster->splitProbability1() << "  New " << splitProb.splitProbability(2) 
                              <<" Probs 2:  Old " << pixelCluster->splitProbability2() << "  New " << splitProb.splitProbability(3) );
    if ( splitProb.splitProbability(2)  < 0 ){
      pixelCluster->packSplitInformation( false, 0.0, 0.0 );    
      pixelCluster->setTooBigToBeSplit( true );    
    }else{  
      pixelCluster->packSplitInformation( false, splitProb.splitProbability(2), splitProb.splitProbability(3) ) ;
      pixelCluster->setTooBigToBeSplit( false );    
    }
    

  }
}


//==================================================================================================

// Note need to double check what is filled in teh TSOS for the SiS tracks
void Trk::DenseEnvironmentsAmbiguityProcessorTool::updateSCT_SplitInformation(std::map< const InDet::SCT_Cluster*, const Trk::TrackParameters* >& setOfClustersOnTrack)
{

  ATH_MSG_DEBUG ("---> Updating " << setOfClustersOnTrack.size() << " SCT clusters");
  for( auto clusterTrkPara : setOfClustersOnTrack){
    //  Update information
   
    const InDet::SCT_Cluster* sctCluster = clusterTrkPara.first;
   
    ATH_MSG_VERBOSE (  "---- "<< sctCluster->globalPosition().perp()
                              <<" SCT width  " << sctCluster->width().colRow()(0) );
    //if ( sctCluster->width().colRow()(0)  > 2 ){
    //  sctCluster->setIsShareable(true);
    //}else{  
    //  sctCluster->setIsShareable(false);
    //}
  }
}

//==================================================================================================
void Trk::DenseEnvironmentsAmbiguityProcessorTool::overlapppingTracks()
{
  // Function currnetly does nothing useful expect for printout debug information
  ATH_MSG_DEBUG ("Starting to resolve overlapping tracks");

  //Reset PRD association tool
  m_assoTool->reset();
  
  // Map to add all pixel clusters on track to
  std::map< const InDet::PixelCluster*, const Trk::TrackParameters* > setOfPixelClustersOnTrack;
  std::map< const InDet::SCT_Cluster*, const Trk::TrackParameters* > setOfSCT_ClustersOnTrack;
  std::map< const InDet::PixelCluster*, const Trk::Track* > setOfPixelClustersToTrackAssoc;

  // Fill pixel cluster into the above map
  // Fill all PRD infromation into the association tool
  for( auto trackScoreMapItem : m_trackScoreTrackMap )
  {
    //Reminder  trackScoreMapItem is pair of the SCORE (float)  and a pair of the Trk::Track  and bool which indicates if the track has been fitted
    
    // clean it out to make sure not to many shared hits
    ATH_MSG_VERBOSE ("--- Adding next track "<<trackScoreMapItem.second.first<<"\t with score "<<-trackScoreMapItem.first << " to PRD map");
     
    //  This should only be done in region defined by Jets 
    //  ..... for now let do the whole detector coudl be slow
    if(m_assoTool->addPRDs( *trackScoreMapItem.second.first ).isSuccess()){
      ATH_MSG_VERBOSE("--- Added hits to the association tool");
    } else {
      ATH_MSG_VERBOSE("--- Failed to add hits to the association tool");
      continue;    
    }
  
  
    // get all prds on 'track'
    const DataVector<const TrackStateOnSurface>* tsosVec = trackScoreMapItem.second.first->trackStateOnSurfaces();  
    if(!tsosVec){
      ATH_MSG_WARNING("TSOS vector does not exist");
      continue;   
    }   


    ATH_MSG_VERBOSE("---> Looping over TSOS's to allow  for cluster updates: "<< tsosVec->size() );

    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos = tsosVec->begin();
    for (; tsos != tsosVec->end(); ++tsos) {
      const MeasurementBase* measurement = (*tsos)->measurementOnTrack();   
        
      if(!measurement || ! (*tsos)->trackParameters()){
        ATH_MSG_VERBOSE("---- TSOS has either no measurement or parameters: "<< measurement << "  " << (*tsos)->trackParameters() );
        continue;           
      }
      
      const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*> ( measurement );
      if(!rio){
        continue;
      }
           
      const InDet::PixelCluster* pixel = dynamic_cast<const InDet::PixelCluster*> ( rio->prepRawData() );
      if (pixel) {
        
        //Update the pixel split information if the element is unique (The second element of the pair indiciates if the element was inserted into the map)
        auto ret =  setOfPixelClustersOnTrack.insert(std::make_pair( pixel, (*tsos)->trackParameters() ));
        if (ret.second) {
          updatePixelSplitInformationForCluster( *(ret.first) );
        }
        
        setOfPixelClustersToTrackAssoc.insert( std::make_pair( pixel, trackScoreMapItem.second.first ) );
        continue;
      }

      //const InDet::SCT_Cluster* sct = dynamic_cast<const InDet::SCT_Cluster*> ( rio->prepRawData() );
      //if (sct) {
      //  setOfSCT_ClustersOnTrack.insert(std::make_pair( sct, (*tsos)->trackParameters() ) );
      //}
    
    }  
  }    
  
  
  
  // This function is no longer used as the clusters are updated in order of insertion into the map
  // Update pixel split information using track information 
  //ATH_MSG_DEBUG("--- Updating Pixel splitting information");
  //updatePixelSplitInformation(setOfPixelClustersOnTrack); 
    

  // This function is no longer used as no method has been developed to intentify SCT clusters which have multiple particle contributions
  // Update SCT split information using track information 
  //ATH_MSG_DEBUG("--- Updating SCT splitting information");
  //updateSCT_SplitInformation(setOfSCT_ClustersOnTrack); 


  // now loop as long as map is not empty
  for( auto pixelTrackItem : setOfPixelClustersToTrackAssoc )
  {     
    ATH_MSG_VERBOSE ("---- Checking if track shares pixel hits if other tracks: " << pixelTrackItem.first << " with R " << pixelTrackItem.first->globalPosition().perp() );

    // find out how many tracks use this hit already
    Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange range = m_assoTool->onTracks( *pixelTrackItem.first );            
    int numberOfTracksWithThisPrd = std::distance(range.first,range.second);
    if (msgLvl(MSG::VERBOSE)) {
      TString tracks("---- number of tracks with this shared Prd: ");
      tracks += numberOfTracksWithThisPrd;
      for (Trk::IPRD_AssociationTool::ConstPRD_MapIt it =range.first; it != range.second;++it ){
       tracks += "    ";
       tracks += Form( " %p",(void*)(it->second)); 
      }
      ATH_MSG_VERBOSE (tracks);
    }

#ifdef SIMPLEAMBIGPROCDEBUGCODE   
    PRD_MultiTruthCollection::const_iterator mce;
    PRD_MultiTruthCollection::const_iterator mc;
    mc=m_truthPIX->find(pixelTrackItem.first->identify()); 
    mce=m_truthPIX->end();

    TString truth("---- match to the following truth particles: "); 
    for(; mc!=mce; ++mc) {
      if((*mc).first != pixelTrackItem.first->identify()) break;
      truth += (*mc).second.barcode();
      truth += "    ";
    }
    ATH_MSG_INFO (truth);
#endif // DebugCode

    
  }
  
  
    // now loop as long as map is not empty
  for( auto sctTrackItem : setOfSCT_ClustersOnTrack )
  {     
    ATH_MSG_VERBOSE ("---- Checking if track shares SCT hits if other tracks: " << sctTrackItem.first << " with R " << sctTrackItem.first->globalPosition().perp() );

    // find out how many tracks use this hit already
    Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange range = m_assoTool->onTracks( *sctTrackItem.first );            
    int numberOfTracksWithThisPrd = std::distance(range.first,range.second);
    if (msgLvl(MSG::VERBOSE)) {
      TString tracks("---- number of tracks with this shared Prd: ");
      tracks += numberOfTracksWithThisPrd;
      for (Trk::IPRD_AssociationTool::ConstPRD_MapIt it =range.first; it != range.second;++it ){
       tracks += "    ";
       tracks += Form( " %p",(void*)(it->second)); 
      }
      ATH_MSG_VERBOSE (tracks);
    }

#ifdef SIMPLEAMBIGPROCDEBUGCODE   
    PRD_MultiTruthCollection::const_iterator mce;
    PRD_MultiTruthCollection::const_iterator mc;
    mc=m_truthSCT->find(sctTrackItem.first->identify()); 
    mce=m_truthSCT->end();

    TString truth("---- match to the following truth particles: "); 
    for(; mc!=mce; ++mc) {
      if((*mc).first != sctTrackItem.first->identify()) break;
      truth += (*mc).second.barcode();
      truth += "    ";
    }
    ATH_MSG_INFO (truth);
#endif // DebugCode

    
  }
  
  
  return;
}

//==================================================================================================
void Trk::DenseEnvironmentsAmbiguityProcessorTool::refitTrack( const Trk::Track* track)
{
  using namespace std;
  const Trk::Track* newTrack = 0;
  if (!m_suppressTrackFit){
    if (m_refitPrds) 
    {
      // simple case, fit PRD directly
      ATH_MSG_VERBOSE ("Refit track "<<track<<" from PRDs");
      newTrack = refitPrds (track);
    }
    else 
    {
      // ok, we fit ROTs
      ATH_MSG_VERBOSE ("Refit track "<<track<<" from ROTs");
      newTrack = refitRots (track);
    }
  } else {
    double reXi2 = 0.; int nDF = 0;
    const DataVector<const TrackStateOnSurface>* tsos = track->trackStateOnSurfaces();
    DataVector<const TrackStateOnSurface>* vecTsos = new DataVector<const TrackStateOnSurface>();
    // loop over TSOS, copy TSOS and push into vector
    DataVector<const TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
    DataVector<const TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 
    for ( ; iTsos != iTsosEnd ; ++iTsos) 
    {
      const TrackStateOnSurface* newTsos = new TrackStateOnSurface(**iTsos);
      vecTsos->push_back(newTsos);
      if((*iTsos)->type(Trk::TrackStateOnSurface::Measurement))
      {  //Get the chi2 and number of hits
        if ((*iTsos)->fitQualityOnSurface()) {
          reXi2 += (*iTsos)->fitQualityOnSurface()->chiSquared();
          nDF   += (*iTsos)->fitQualityOnSurface()->numberDoF();
        }
      }
    }
    Trk::FitQuality* fq = new Trk::FitQuality(reXi2,nDF-5);
    Trk::TrackInfo info;
    info.addPatternRecoAndProperties(track->info());
    Trk::TrackInfo newInfo;
    newInfo.setPatternRecognitionInfo(Trk::TrackInfo::SimpleAmbiguityProcessorTool);
    info.addPatternReco(newInfo); 

    newTrack = new Trk::Track(info, vecTsos, fq);  
  }
  
  if (newTrack!=0) 
  {
    ATH_MSG_DEBUG ("New track successfully fitted "<<newTrack);

    if (m_monitorTracks) {
      // add new track to observed tracks and mark "old" one as rejected
			m_observerTool->rejectTrack(*track, 4);		// rejection location 4: "refit OK" (not a real rejection, but new track copy is made)
			m_observerTool->addSubTrack(*newTrack, *track);		// add new subtrack 
    }

    addTrack( newTrack, true );

#ifdef SIMPLEAMBIGPROCDEBUGCODE
    keepTrackOfTracks(track, newTrack);
#endif
    
    
  }
  else { 
    ATH_MSG_DEBUG ("Fit failed !");

    if (m_monitorTracks) {
      // reject observed track
      m_observerTool->rejectTrack(*track, 5);		// rejection location 5: "refit failed"
    }
    
#ifdef SIMPLEAMBIGPROCDEBUGCODE
    ATH_MSG_INFO( "The Track: " << origTrack(track) << " failed to fit" );
    n_fitFails++;
    ATH_MSG_INFO( "The total number of failed fits is now " << n_fitFails );
    if ( isTrueTrack(track)) {
      n_trueFitFails++;
      ATH_MSG_INFO( "The total number of TRUE failed fits is now " << n_trueFitFails );
      prdTruth(track);
      tsosTruth(track);
    }
#endif
    
  }  
  
  return;
}

//==================================================================================================

const Trk::Track* Trk::DenseEnvironmentsAmbiguityProcessorTool::refitPrds( const Trk::Track* track)
{

  // get vector of PRDs
  std::vector<const Trk::PrepRawData*> prds = m_selectionTool->getPrdsOnTrack(track);

  if ( 0==prds.size() ) {
    ATH_MSG_WARNING( "No PRDs on track");
    return 0;
  }
     
  ATH_MSG_VERBOSE ("Track "<<track<<"\t has "<<prds.size()<<"\t PRDs");

  const TrackParameters* par = track->perigeeParameters();
  if (par==0) {
    ATH_MSG_DEBUG ("Track ("<<track<<") has no perigee! Try any other ?");
    par = track->trackParameters()->front();
    if (par==0) {
      ATH_MSG_DEBUG ("Track ("<<track<<") has no Track Parameters ! No refit !");
      return 0;
    }
  }

  // refit using first parameter, do outliers
  const Trk::Track* newTrack = 0;

  if (m_tryBremFit && track->info().trackProperties(Trk::TrackInfo::BremFit))
  {
    // statistics
    increment_by_eta(m_NbremFits,track);

    ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
    // TODO revert once GlobalChi2Fitter properly handles brem fits when 
    //      starting from prds
    // newTrack = fit(prds, *par, true, Trk::electron);
    newTrack = fit(*track, true, Trk::electron);

  }
  else
  {
    // statistics
    increment_by_eta(m_Nfits,track);

    ATH_MSG_VERBOSE ("Normal track, refit");
    newTrack = fit(prds, *par, true, m_particleHypothesis);

    if (!newTrack && m_tryBremFit && par->pT() > m_pTminBrem &&
  (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
    {
      // statistics
      increment_by_eta(m_NrecoveryBremFits,track);

      ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
      // TODO revert once GlobalChi2Fitter properly handles brem fits when 
      //      starting from prds
      // newTrack = fit(prds, *par, true, Trk::electron);
      newTrack = fit(*track, true, Trk::electron);

    }
  }
  
  if(newTrack)
  {
    // statistic
    increment_by_eta(m_NgoodFits,newTrack);

    //keeping the track of previously accumulated TrackInfo
    const Trk::TrackInfo old_info = track->info();
    newTrack->info().addPatternReco(old_info);
  }
  else
  {
    // statistic
    increment_by_eta(m_NfailedFits,track);
  }
  return newTrack;
}

//==================================================================================================

const Trk::Track* Trk::DenseEnvironmentsAmbiguityProcessorTool::refitRots( const Trk::Track* track)
{

  ATH_MSG_VERBOSE ("Refit track "<<track);

  // refit using first parameter, do outliers
  const Trk::Track* newTrack = 0;

  if (m_tryBremFit &&
      track->info().trackProperties(Trk::TrackInfo::BremFit))
  {
    // statistics
    increment_by_eta(m_NbremFits,track);

    ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
    newTrack = fit(*track, true, Trk::electron);
  }
  else
  {
    // statistics
    increment_by_eta(m_Nfits,track);

    ATH_MSG_VERBOSE ("Normal track, refit");
    newTrack = fit(*track, true, m_particleHypothesis);

    if (!newTrack && m_tryBremFit &&
        track->trackParameters()->front()->pT() > m_pTminBrem &&
        (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
    {
      // statistics
      increment_by_eta(m_NrecoveryBremFits,track);

      ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
      newTrack = fit(*track, true, Trk::electron);
    }
  }

  if(newTrack)
  {
    // statistic
    increment_by_eta(m_NgoodFits,newTrack);

    //keeping the track of previously accumulated TrackInfo
    const Trk::TrackInfo old_info = track->info();
    newTrack->info().addPatternReco(old_info);
  }
  else
  {
    // statistic
    increment_by_eta(m_NfailedFits,track);
  }
  return newTrack;
}

//==================================================================================================

void Trk::DenseEnvironmentsAmbiguityProcessorTool::dumpTracks( const TrackCollection& tracks )
{

  ATH_MSG_VERBOSE ("Dumping tracks in collection");
  int num=0;
  TrackScore totalScore = 0;
  TrackCollection::const_iterator it    = tracks.begin();
  TrackCollection::const_iterator itEnd = tracks.end();
  for (; it != itEnd ; ++it)
  {
    // score track:
    const TrackScore score = m_scoringTool->score( **it, m_suppressHoleSearch );
    ATH_MSG_VERBOSE (num++<<"\tTrack :"<<*it<<"\tScore: "<<score);
    totalScore+=score;
  }
  ATH_MSG_DEBUG ("Total event score : "<<totalScore);
  return;
}

//==================================================================================================

void Trk::DenseEnvironmentsAmbiguityProcessorTool::storeTrkDistanceMapdR( const TrackCollection& tracks, std::vector<const Trk::Track*> &refit_tracks_out )
{
  ATH_MSG_VERBOSE ("Creating track Distance dR map");
  SG::WriteHandle<InDet::DRMap> dRMapHandle (m_dRMap);
  dRMapHandle = std::make_unique<InDet::DRMap>();
  if ( !dRMapHandle.isValid() ){
    ATH_MSG_WARNING("Could not record Distance dR map.");
  } else{
    ATH_MSG_VERBOSE("Distance dR map recorded as '" << m_dRMap.key() <<"'.");
  }

  for (auto track : tracks){
      bool refit = false;
      const DataVector<const TrackStateOnSurface>* tsosVec = track->trackStateOnSurfaces();  
      if(!tsosVec){
        ATH_MSG_WARNING("TSOS vector does not exist");
        continue;   
      }  
      ATH_MSG_VERBOSE("---> Looping over TSOS's to allow  for cluster updates: "<< tsosVec->size() );
      for(auto tsos : *tsosVec){
          const MeasurementBase* measurement = tsos->measurementOnTrack(); 
          if(!measurement || ! tsos->trackParameters()){
            ATH_MSG_VERBOSE("---- TSOS has either no measurement or parameters: "<< measurement << "  " << tsos->trackParameters() );
            continue;           
          }
          
          if(!tsos->type(Trk::TrackStateOnSurface::Measurement)) {continue;}
          
          auto globalPosition = measurement->globalPosition();
          double radius = sqrt(globalPosition[0]*globalPosition[0]+globalPosition[1]*globalPosition[1]);
          // get the associated prd
          const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*> ( measurement );
          if(!rio){
              continue;
          }
          const InDet::PixelCluster* pixel = dynamic_cast<const InDet::PixelCluster*> ( rio->prepRawData() );
          // not pixel or not split
          if (!pixel || !pixel->isSplit() ) {continue ;}

          CylinderSurface iblSurface(radius,3000.0);
          
          const TrackParameters * trackParams = m_extrapolatorTool->extrapolate(*track,iblSurface);
          
          double yOnPix = trackParams->position().y();
          double zOnPix = trackParams->position().z();
          
          double Pi = acos(0);
          double twoPi = 2.*Pi;
          
          // now, find closest track  
          double dr = 0.; 
          double mindR = 99999999.;
          double mindX = 99999999.;
          double mindZ = 99999999.;
          
          for (auto track2 : tracks){
              if(track==track2) continue;
              float dEta = track->perigeeParameters()->momentum().eta() - track2->perigeeParameters()->momentum().eta();
              float dPhi2 = track->perigeeParameters()->momentum().phi() - track2->perigeeParameters()->momentum().phi();
              dr =  sqrtf(dEta*dEta + dPhi2*dPhi2);
              if(dr>0.4) continue;
              
              //extrapolation to pixel hit radius
              const TrackParameters * track2Params = m_extrapolatorTool->extrapolate(*track2,iblSurface);
          
              double y2OnPix = track2Params->position().y();
              double z2OnPix = track2Params->position().z();
              
              float dPhi = asin(yOnPix/radius) -asin(y2OnPix/radius);
              if (dPhi >= Pi) dPhi -= twoPi;
              if (dPhi < -Pi) dPhi += twoPi;
              
              double dx = fabs(radius*dPhi);
              double dz = fabs(zOnPix - z2OnPix);
              if(dx>mindX && dz>mindZ) continue;
              dr = sqrt(dx*dx + dz*dz);
              
              if(dr<mindR && dr > 1.e-4){
                  mindR = dr;
                  mindX = dx;
                  mindZ = dz;
              }
         }
         refit = true;
         std::pair<InDet::DRMap::iterator,bool> ret;
         std::pair<float,float> min (mindX, mindZ);
         ret = dRMapHandle->insert ( std::pair<const InDet::PixelCluster*,std::pair<float,float> >(pixel,min));
         // if we already have a dR for this prd, we update it, if current value is smaller
         if (ret.second==false) {
            InDet::DRMap::iterator it;
            it = dRMapHandle->find(pixel);
            if(sqrt(pow((*it).second.first,2)+pow((*it).second.second,2)) > (float)mindR) {
                (*it).second.first  = (float)mindX;
                (*it).second.second = (float)mindZ;
	        }
         }
      }
      if(refit) refit_tracks_out.push_back(track);
  }
  return;
}

//============================================================================================================
bool Trk::DenseEnvironmentsAmbiguityProcessorTool::decideIfInHighPtBROI(const Trk::Track* ptrTrack)
{
  // Are we in a ROI?
  bool inROIandPTok(true);
  if(  ptrTrack->trackParameters()->front() ){
    if(  ptrTrack->trackParameters()->front()->pT() < m_minPtBjetROI ){
      inROIandPTok = false;
      return false;
    }
    if(inROIandPTok){

      bool inROI  = m_useHClusSeed && isHadCaloCompatible(*ptrTrack->trackParameters()->front());
      return inROI;
    }
    else
      return false;
  } else
    return false;
}

//============================================================================================================
bool Trk::DenseEnvironmentsAmbiguityProcessorTool::isHadCaloCompatible(const Trk::TrackParameters& Tp) const
{
  const double pi = M_PI, pi2 = 2.*M_PI;
    if(m_hadF.empty()) return false;
  
  auto f = m_hadF.begin(), fe = m_hadF.end();
  auto e = m_hadE.begin();
  auto r = m_hadR.begin();
  auto z = m_hadZ.begin();
  
  double F = Tp.momentum().phi();
  double E = Tp.eta();
  
  for(; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if(df > pi        ) df = fabs(pi2-df);
    if(df < m_phiWidth) {
      //Correct eta of cluster to take into account the z postion of the track
      double newZ   = *z - Tp.position().z();
      double newEta =  atanh( newZ / sqrt( (*r) * (*r) + newZ*newZ ) );
  
      double de = fabs(E-newEta);
      if(de < m_etaWidth) return true;
    }
    ++e;
    ++r;
    ++z;
  }
  return false;
}

//============================================================================================================
void Trk::DenseEnvironmentsAmbiguityProcessorTool::reloadHadROIs()
{
  if(m_useHClusSeed) {
    m_hadF.clear();
    m_hadE.clear();
    m_hadR.clear();
    m_hadZ.clear();
  
    SG::ReadHandle<CaloClusterROI_Collection> calo(m_inputHadClusterContainerName);
    for( const auto& ccROI : *calo) {
      m_hadF.push_back( ccROI->globalPosition().phi() );
      m_hadE.push_back( ccROI->globalPosition().eta() );
      m_hadR.push_back( ccROI->globalPosition().perp() );
      m_hadZ.push_back( ccROI->globalPosition().z() );
    }
  }
}
   
//============================================================================================================
void Trk::DenseEnvironmentsAmbiguityProcessorTool::removeInnerHits(std::vector<const Trk::MeasurementBase*>& measurements) const
{
  int count = 0; 
  for (size_t i=0; i < measurements.size(); ++i){
    const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>(measurements.at(i));
    if (rio != 0) {
      const Identifier& surfaceID = (rio->identify()) ;                            
      if(m_idHelper->is_pixel(surfaceID) && count ==0){  
        //Only do this if we want to remove the pixel hits 
        const Identifier& id = m_pixelId->wafer_id(surfaceID);
        int layerDisk = m_pixelId -> layer_disk(id);
        if (layerDisk < 3){
          measurements.erase(measurements.begin()+i);
          break;
        }
        else{
          break;
        }
      }
      else 
        break;
    }
  }
}

//============================================================================================================
const Trk::Track* Trk::DenseEnvironmentsAmbiguityProcessorTool::refitTracksFromB(const Trk::Track* track, double fitQualityOriginal) const 
{
  const Trk::TrackParameters* par = track->perigeeParameters();
  if (par==0) {
    par = track->trackParameters()->front();
    if (par==0) {
      ATH_MSG_DEBUG ("Track ("<<track<<") has no Track Parameters ! No refit !");
      return 0;
    }
  }
  
  //std::vector<const Trk::MeasurementBase*>* measurementSet = new std::vector<const Trk::MeasurementBase*>;
  std::vector<const Trk::MeasurementBase*> measurementSet;
  //store all silicon measurements into the measurementset
  DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurface = track->trackStateOnSurfaces()->begin();
  for ( ; trackStateOnSurface != track->trackStateOnSurfaces()->end(); ++trackStateOnSurface ) {
    if ( !(*trackStateOnSurface) ){
      ATH_MSG_WARNING( "This track contains an empty MeasurementBase object that won't be included in the fit" );
      continue;
    }
    if ( (*trackStateOnSurface)->measurementOnTrack() ){
      if ( (*trackStateOnSurface)->type( Trk::TrackStateOnSurface::Measurement) ){
        const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>( (*trackStateOnSurface)->measurementOnTrack() );
        if (rio != 0) {
          const Identifier& surfaceID = (rio->identify()) ;                            
          if(m_idHelper->is_pixel(surfaceID)|| m_idHelper->is_sct(surfaceID)) {
            measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
          }
        }
      }
    }
  }

  size_t previousMeasSize = measurementSet.size();
  while (true){
    removeInnerHits(measurementSet); 
    if(measurementSet.size()>4){
      const Trk::Track* refittedTrack = fit(measurementSet,*par,true,Trk::pion);
      double fitQualPostRefit = 10;
      if  (refittedTrack && refittedTrack->fitQuality() && refittedTrack->fitQuality()->numberDoF()!=0 ) 
        fitQualPostRefit = refittedTrack->fitQuality()->chiSquared()/refittedTrack->fitQuality()->numberDoF(); 
      if (fitQualityOriginal/fitQualPostRefit > 1){
        if ( fitQualityOriginal/fitQualPostRefit > 1.2){
          return refittedTrack;
        }
      }
      if (previousMeasSize == measurementSet.size()){
        return 0;
      }
      previousMeasSize = measurementSet.size();
    } else {
      //cannot refit the track because we do not have enough measurements
      return 0;
    }
  }
}


//==================================================================================================
//
//
//   FROM HERE EVERYTHING IS DEBUGGING CODE !!!
//
//
//==================================================================================================

//==================================================================================================
// Part II : Truth association
//==================================================================================================
#ifdef SIMPLEAMBIGPROCDEBUGCODE

void Trk::DenseEnvironmentsAmbiguityProcessorTool::findTrueTracks(const TrackCollection* recTracks)
{

  numSharedTruth = 0;
  m_trueTracks.clear();
  m_trackHistory.clear();
  m_tracksShared.clear();

  ATH_MSG_DEBUG("Accessing TrackTruthCollection");
  SG::ReadHandle<TrackTruthCollection> truthMap(m_truthCollection);
  
  std::map<int,std::pair<float,const Trk::Track*> > barcodeMap;
  float minProb =0.80;
  TrackCollection::const_iterator trackIt    = recTracks->begin();
  TrackCollection::const_iterator trackItEnd = recTracks->end();

  for (;trackIt!=trackItEnd;++trackIt)
  {
    ATH_MSG_DEBUG( "The Track is now " << *trackIt << " with " << (*trackIt)->trackStateOnSurfaces()->size() );
  
    // initialise history tracing
    m_trackHistory.insert(std::make_pair(*trackIt,*trackIt));
  
    ElementLink<TrackCollection> tracklink;
    tracklink.setElement(const_cast<Trk::Track*>(*trackIt));
    tracklink.setStorableObject(*recTracks);
    const ElementLink<TrackCollection> tracklink2=tracklink;
  
    // check if the track has a valid agreement in TrackToTruth
    TrackTruthCollection::const_iterator found = truthMap->find(tracklink2);
    if (found != truthMap->end() &&  
    found->second.particleLink().isValid() &&
      found->second.probability() > minProb    )
    {
      if (!isSharedTrack(*trackIt)) addTrackToMap(*trackIt);//add track and pdrs to map
      else numSharedTruth++;
      int barcode=found->second.particleLink().barcode();
      ATH_MSG_DEBUG( "-- The Barcode is: " << barcode );
      std::pair<int , std::pair<float , const Trk::Track*> > insertion 
          = std::make_pair(barcode,std::make_pair(found->second.probability(),*trackIt));
      std::pair<std::map<int,std::pair<float,const Trk::Track*> >::iterator, bool > barcodeMapInsert 
          = barcodeMap.insert(insertion);
      if (!(barcodeMapInsert.second) && insertion.second.first > barcodeMapInsert.first->second.first)
      {
        // if the barcode is already known and the new entry would be better, erase old and insert new
        ATH_MSG_DEBUG( "<-- Erasing Track: " << origTrack(barcodeMapInsert.first->second.second) );         
        barcodeMap.erase(barcodeMapInsert.first);
        ATH_MSG_DEBUG( "--> Inserting Track: "<<  origTrack(insertion.second.second));
        barcodeMap.insert(insertion);
      }
      ATH_MSG_DEBUG( "-- The probability of " << origTrack(barcodeMapInsert.first->second.second) << " is "<< barcodeMapInsert.first->second.first ); 
    } else {
      ATH_MSG_DEBUG( "-- No truth particle found" ); 
    }
  }
  
  // copy the true tracks in the TruthHistorySet:
  std::map<int,std::pair<float,const Trk::Track*> >::const_iterator  it = barcodeMap.begin();
  std::map<int,std::pair<float,const Trk::Track*> >::const_iterator end = barcodeMap.end();
  for (; it!=end; ++it) m_trueTracks.insert(it->second.second);
  truthBefore = m_trueTracks.size();
  ATH_MSG_INFO( "True Track set up with " << truthBefore << " true tracks." );
  ATH_MSG_INFO( "OF THE TRUE TRACKS " << numSharedTruth << " ARE SHARED" );
  
  
}
//==================================================================================================

void Trk::DenseEnvironmentsAmbiguityProcessorTool::keepTrackOfTracks(const Trk::Track* oldTrack, const Trk::Track* newTrack)
{
  m_trackHistory.insert(std::make_pair(newTrack,oldTrack));
  if (m_trueTracks.find(oldTrack) != m_trueTracks.end() ) m_trueTracks.insert(newTrack);
}

//==================================================================================================

void Trk::DenseEnvironmentsAmbiguityProcessorTool::produceInputOutputConnection()
{
  if (!m_has_resolvedTrackConnection) {
    // output map: SiSpSeededTrack, ResolvedTrack  
    TrackCollectionConnection siSP_ResolvedConnection;
    
    TrackCollection::const_iterator  itFinal = m_finalTracks->begin();
    TrackCollection::const_iterator endFinal = m_finalTracks->end();
    for ( ; itFinal != endFinal ; ++itFinal)
    {
      std::map<const Trk::Track*, const Trk::Track*>::iterator pos = m_trackHistory.find(*itFinal);
      while (pos->first != pos->second && pos != m_trackHistory.end())
        pos = m_trackHistory.find(pos->second);
      
      if (pos == m_trackHistory.end())
        ATH_MSG_ERROR( "Track not found in history" );
      else
        siSP_ResolvedConnection.insert(std::make_pair(pos->second,*itFinal));
      
    }
      
    SG::WriteHandle<TrackCollectionConnection> h_write_resolvedTrackConnection(m_write_resolvedTrackConnection);
    h_write_resolvedTrackConnection.record(std::make_unique<TrackCollectionConnection>(siSP_ResolvedConnection));

    ATH_MSG_VERBOSE( "Saved "<<siSP_ResolvedConnection.size()<<" track collection connections"); 
  }
}
//============================================================================================

const Trk::Track* Trk::DenseEnvironmentsAmbiguityProcessorTool::origTrack( const Trk::Track* track){
  
  std::map<const Trk::Track*, const Trk::Track*>::iterator m_pos = m_trackHistory.find(track);
  while (m_pos->first != m_pos->second && m_pos != m_trackHistory.end())
    m_pos = m_trackHistory.find(m_pos->second);
  
  if(m_pos == m_trackHistory.end())
    return 0;
  
  return m_pos->first;
}

//==================================================================================================
bool Trk::DenseEnvironmentsAmbiguityProcessorTool::isSharedTrack(const Trk::Track* Tr){

  int numSharedPRD = 0;
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();
  
  for(; m!=me; ++m) {
    const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if( m_tracksShared.find(prd) != m_tracksShared.end() ) numSharedPRD++;
  }
  if(numSharedPRD>=3) return true;
  else return false;
}
//===================================================================================                            

void Trk::DenseEnvironmentsAmbiguityProcessorTool::addTrackToMap(Trk::Track* Tr)
{
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();
  
  for(; m!=me; ++m) {
    const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if(prd) m_tracksShared.insert(std::make_pair(prd,Tr));
  }
}

//=================================================================================================
bool Trk::DenseEnvironmentsAmbiguityProcessorTool::isTrueTrack(const Trk::Track* track){
  std::set<const Trk::Track*>::const_iterator m_iter = m_trueTracks.find(track);
  if(m_iter != m_trueTracks.end())
    return true;
  else 
    return false;

}

//================================================================================
void Trk::DenseEnvironmentsAmbiguityProcessorTool::prdTruth(const Trk::Track* track){
  
  
  //geting the truth info about th prds
  std::vector<const Trk::PrepRawData*> prds = m_selectionTool->getPrdsOnTrack(track);
  std::vector<const Trk::PrepRawData*>::const_iterator prdit = prds.begin();
  std::vector<const Trk::PrepRawData*>::const_iterator prdite = prds.end();
  double m_eta = (*(track)->trackParameters()->begin())->eta();
  ATH_MSG_INFO( "The eta of this track is " << m_eta );   
  
  for( ; prdit != prdite; ++prdit){
    const InDet::SiCluster      * si = dynamic_cast<const InDet::SiCluster*>      (*prdit);
    const InDet::PixelCluster   * px = dynamic_cast<const InDet::PixelCluster*>   (*prdit);
    const InDet::TRT_DriftCircle* tr = dynamic_cast<const InDet::TRT_DriftCircle*>(*prdit);
    PRD_MultiTruthCollection::const_iterator mce;
    PRD_MultiTruthCollection::const_iterator mc;
    if     (px) {
      mc=m_truthPIX->find((*prdit)->identify()); 
      mce=m_truthPIX->end();
      if(mc==mce) {
        ATH_MSG_INFO( "The hit " << *prdit << " on the track " << origTrack(track) << " was Pixel noise" );
        continue;
      }
      ATH_MSG_INFO( "The hit " << *prdit << " on the track " << origTrack(track) << " was a Pixel hit" );
    }
    else if(si) {
      mc=m_truthSCT->find((*prdit)->identify()); 
      mce=m_truthSCT->end();
      if(mc==mce) {
        ATH_MSG_INFO( "The hit " << *prdit << " on the track " <<origTrack(track) << " was SCT noise" );   
        continue;
      } 
      ATH_MSG_INFO( "The hit " << *prdit << " on the track " << origTrack(track) << " was a SCT hit" );
    }
    else if(tr) {
      mc=m_truthTRT->find((*prdit)->identify()); 
      mce=m_truthTRT->end();
      if(mc==mce) {
        ATH_MSG_INFO( "The hit " << *prdit << " on the track " << origTrack(track) << " was TRT noise" ); 
        continue;
      }
      ATH_MSG_INFO( "The hit " << *prdit << "on the track " << origTrack(track) << " was a TRT hit" );
    }
    else {
      continue;
    }
    
    const HepMC::GenParticle* pa = mc->second.cptr();   
    int partid = pa->pdg_id();
    int partBc = pa->barcode();
    
    //HepLorentzVector m  = pa->momentum();
    double m_theta = pa->momentum().theta();
    ATH_MSG_INFO( "The theta of the hit was " << m_theta );
    ATH_MSG_INFO( "The hit " << *prdit << " came from " << partid << " with a barcode " << partBc );   
  }
}  

//====================================================================================
void Trk::DenseEnvironmentsAmbiguityProcessorTool::tsosTruth(const Trk::Track* track){

  
  const DataVector<const TrackStateOnSurface>* tsos = track->trackStateOnSurfaces();
  // loop over TSOS, copy TSOS and push into vector
  DataVector<const TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
  DataVector<const TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end();   
  for(; iTsos != iTsosEnd; ++iTsos){
    ATH_MSG_INFO( "The type of " << *iTsos << " is "<< (*iTsos)->dumpType() );
    const FitQualityOnSurface* fq = (*iTsos)->fitQualityOnSurface();
    if (fq)
      ATH_MSG_INFO( "The chi2 of " << *iTsos << " is "<< fq->chiSquared() );
    const MeasurementBase* mb = (*iTsos)->measurementOnTrack();
    if (mb){
      Identifier Id =  mb->associatedSurface().associatedDetectorElementIdentifier();
      ATH_MSG_INFO( "The global r of the hit is " << mb->associatedSurface().center().perp() );
      std::string detType = " unknown ";
      if (m_pixelId->is_pixel(Id))
        detType = " Pixel ";
      else if (m_pixelId->is_sct(Id))
        detType = " SCT ";
      else if (m_pixelId->is_trt(Id))
        detType = " TRT ";
      ATH_MSG_INFO( "The surface is " << detType ); 
    }
  } 
}

//=======================================================================================
StatusCode Trk::DenseEnvironmentsAmbiguityProcessorTool::getBremTruth()
{
  // Retrieve McEventCollection from StoreGate
  SG::ReadHandle<McEventCollection> mcEventCollection(m_generatedEventCollectionName);
  
  // Loop over all events in StoreGate
  McEventCollection::const_iterator event = mcEventCollection->begin();
  
  for ( ; event != mcEventCollection->end(); ++event ){
    
    // Synchronise event number with reconstruction event number
    int eventNumber = (*event)->event_number();    
    ATH_MSG_INFO( "Event number: " << eventNumber - 1 );

    // Determine the momentum of the original particle
    const double initialMomentum = originalMomentum( *event );
    ATH_MSG_INFO( "Initial momentum: " << initialMomentum );

   

    // Calculate the total momentum loss as a result of brem
    double pLostByBrem = momentumLostByBrem( *event );
    ATH_MSG_INFO( "Total momentum lost by original particle due to Brem: " << pLostByBrem << " MeV" );
   
    
    // Calculate the fraction of incident energy lost per vertex on track
    const std::vector<double> fractionOfIncidentEnergyLost = fractionOfIncidentMomentumLostPerVertex( *event );
    int countBrem = 0; //Si brem counter
    std::vector<double>::const_iterator fractionLostInLayer = fractionOfIncidentEnergyLost.begin();
    for ( ; fractionLostInLayer != fractionOfIncidentEnergyLost.end(); ++fractionLostInLayer ){
      ATH_MSG_INFO( "Fraction of incident energy lost at vertex: " << (*fractionLostInLayer) );
      countBrem++;
    }
    
    // Determine the positions of the brem vertices
    const std::vector<Amg::Vector3D > positionsOfVertices = positionsOfBremVertices( *event );
    countBrem = 0; //Reinitialize brem counter
    for ( auto positionOfVertex : positionsOfVertices ){
      ATH_MSG_INFO( "(x, y, z) of vertex: " << positionOfVertex.perp() );
      countBrem++;
    }
    
  }

  return StatusCode::SUCCESS;
}
//======================================================================================================
double Trk::DenseEnvironmentsAmbiguityProcessorTool::originalMomentum( const HepMC::GenEvent* genEvent )
{

  // Loop over all particles in the event (info on this from GenEvent documentation)
  HepMC::GenEvent::particle_const_iterator particle = genEvent->particles_begin();

  // Calculate initial energy of electron. Should be the first particle
  HepMC::GenParticle* initialParticle = *( genEvent->particles_begin() );

  double initialMomentum = initialParticle->momentum().rho();

  return initialMomentum;
}

//==================================================================================================
double Trk::DenseEnvironmentsAmbiguityProcessorTool::momentumLostByBrem( const HepMC::GenEvent* genEvent ) const
{

 
  double bremPhotonEnergy(0.);
 
  // Loop over all verticies and determine which are associated to the original particle
  HepMC::GenEvent::vertex_const_iterator vertex = genEvent->vertices_begin();
 
  for ( ; vertex != genEvent->vertices_end(); ++vertex ){
 
    if ( !( vertexAssociatedWithOriginalTrack( *vertex ) ) )
      continue;
 
    // Loop over all particles in vertex. Determine if it is a vertex of the original particle
    HepMC::GenVertex::particle_iterator particleAtVertex = (*vertex)->particles_begin(HepMC::family);
 
    for ( ; particleAtVertex != (*vertex)->particles_end(HepMC::family); ++particleAtVertex ){
 
      if ( (*particleAtVertex)->pdg_id() == 22 )
    bremPhotonEnergy += (*particleAtVertex)->momentum().e();
 
    }
 
  }  

  return bremPhotonEnergy;
}

//=================================================================================================
const std::vector<double> Trk::DenseEnvironmentsAmbiguityProcessorTool::fractionOfIncidentMomentumLostPerVertex( const HepMC::GenEvent* genEvent ) const
{
  
  // Don't forget: Some of the 4-momentum of incident electron always transfered to nucleus. That's why no brem in vacuum.

  std::vector<double> fractionOfIncidentMomentumLostPerVertex;

  // Loop over all verticies and find those associated to the original track
  HepMC::GenEvent::vertex_const_iterator vertex = genEvent->vertices_begin();

  for ( ; vertex != genEvent->vertices_end(); ++vertex ){

    if ( !( vertexAssociatedWithOriginalTrack( *vertex ) ) )
      continue;

    // One incident particle only. THIS IS HARDWIRED TO ASSUME FIRST PARTICLE IS INCIDENT
    HepMC::GenVertex::particle_iterator incidentParticle = (*vertex)->particles_begin(HepMC::family);
    double incidentEnergy = (*incidentParticle)->momentum().e();

    // Loop over outgoing particles and extract the photon
    double photonEnergy(0.);

    HepMC::GenVertex::particles_out_const_iterator outgoingParticle = (*vertex)->particles_out_const_begin();

    for ( ; outgoingParticle != (*vertex)->particles_out_const_end(); ++outgoingParticle ){
      if ( (*outgoingParticle)->pdg_id() == 22 )
         photonEnergy = (*outgoingParticle)->momentum().e();
    }

    // Define the fractional energy loss
    double fractionalEnergyLoss = photonEnergy / incidentEnergy;

    if ( fractionalEnergyLoss != 0. )
      fractionOfIncidentMomentumLostPerVertex.push_back( fractionalEnergyLoss );

  }

  return fractionOfIncidentMomentumLostPerVertex;

}
//=================================================================================================
const std::vector<Amg::Vector3D > Trk::DenseEnvironmentsAmbiguityProcessorTool::positionsOfBremVertices( const HepMC::GenEvent* genEvent ) const
{

  std::vector<Amg::Vector3D> positionsOfVertices;

  // Loop over all verticies and find those associated to the original track
  HepMC::GenEvent::vertex_const_iterator vertex = genEvent->vertices_begin();

  for ( ; vertex != genEvent->vertices_end(); ++vertex ){

    if ( !( vertexAssociatedWithOriginalTrack( *vertex ) ) )
      continue;

    // Loop over vertices and find ones with photons radiated
    HepMC::GenVertex::particles_out_const_iterator outgoingParticle = (*vertex)->particles_out_const_begin();

    for ( ; outgoingParticle != (*vertex)->particles_out_const_end(); ++outgoingParticle ){

      if ( (*outgoingParticle)->pdg_id() == 22 ){
        Amg::Vector3D photonPoint3d((*vertex)->point3d().x(),(*vertex)->point3d().y(),(*vertex)->point3d().z());
        positionsOfVertices.push_back( photonPoint3d );
      }
    }
  }

  return positionsOfVertices;

}
//============================================================================================================
bool Trk::DenseEnvironmentsAmbiguityProcessorTool::vertexAssociatedWithOriginalTrack( HepMC::GenVertex* genVertex) const
{

  bool originalParticleFlag = false;

  // Loop over all particles in vertex. Determine if it is a vertex of the original particle
  HepMC::GenVertex::particle_iterator particleAtVertex = genVertex->particles_begin(HepMC::family);

  for ( ; particleAtVertex != genVertex->particles_end(HepMC::family); ++particleAtVertex ){
      
    int originalParticle = ( (*particleAtVertex)->barcode() - 10001 ) % 1000000;

    if (originalParticle == 0 )
      originalParticleFlag = true;

  }

  return originalParticleFlag;

}
#endif





