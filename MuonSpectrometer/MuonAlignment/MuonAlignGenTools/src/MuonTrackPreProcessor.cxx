/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterInterfaces/IGlobalTrackFitter.h"

//#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkAlignEvent/AlignTrack.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkEventUtils/MeasurementTypeID.h"
//#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkVolumes/Volume.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/StraightLineSurface.h"

#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonAlignGenTools/MuonTrackPreProcessor.h"
#include "MuonAlignGenTools/MuonAlignRefitTool.h"
#include "MuonAlignGenTools/MuonAlignHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonRecToolInterfaces/IMuonSegmentTrackBuilder.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"

#include "muonEvent/MuonContainer.h"
#include "Particle/TrackParticle.h"

#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/ParamDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "TFile.h"
#include "TTree.h"

namespace Muon {
  
  //________________________________________________________________________
  MuonTrackPreProcessor::MuonTrackPreProcessor(const std::string& type,
					       const std::string& name,
					       const IInterface* parent)
    : AthAlgTool(type,name,parent)
    , p_muonMgr(0)
    , m_storeGate(0)
    , m_ntuple(0)
    , m_trackFitterTool("Trk::GlobalChi2Fitter/MCTBFitter")
    , m_SLTrackFitterTool("Trk::GlobalChi2Fitter/MCTBSLFitter")
    , m_muonIdHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
    , m_segmentTrackBuilder("Muon::MuonSegmentTrackBuilder/MuonSegmentTrackBuilder")
    , m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator")
    , m_muonAlignRefitTool("Muon::MuonAlignRefitTool/MuonAlignRefitTool")
    , m_muonAlignHelperTool("Muon::MuonAlignHelperTool/MuonAlignHelperTool")
    , m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
      //, m_trackingVolumesSvc("TrackingVolumesSvc/TrackingVolumesSvc",name) //rdh problems with Eigen migration!
    , m_idHelper(0)   
    , m_measTypeIdHelper(0)
    , m_particleNumber(2)
    , m_particleHypothesis(Trk::muon)
    , m_tree(0)
    , m_runNumber(0)
    , m_evtNumber(0)
    , m_nresiduals(0)
    , m_residuals(new double[100])
    , m_respulls(new double[100])
    , m_nInnerLayersHit(0)
    , m_nMiddleLayersHit(0)
    , m_nOuterLayersHit(0)
    , m_nOtherLayersHit(0)
    , m_nTubeLayersHitInner(0)
    , m_nTubeLayersHitMiddle(0)
    , m_nTubeLayersHitOuter(0)
    , m_nTubeLayersHitOther(0)
    , m_nChambersHit(0)
    , m_nLargeChambersHit(0)
    , m_nSmallChambersHit(0)
    , m_driftSignsInnerLayer(0)
    , m_driftSignsMiddleLayer(0)
    , m_driftSignsOuterLayer(0)
    , m_driftSignsOtherLayer(0)
    , m_nInnerLayerOutliers(0)
    , m_nMiddleLayerOutliers(0)
    , m_nOuterLayerOutliers(0)
    , m_nOtherLayerOutliers(0)
    , m_nInnerLayerHoles(0)
    , m_nMiddleLayerHoles(0)
    , m_nOuterLayerHoles(0)
    , m_nOtherLayerHoles(0)
    , m_trackNDoF(0)
    , m_trackChi2(0)
    , m_trackPt(0)
    , m_trackEta(0)
    , m_qOverP(0)
    , m_IDres(new double[100]), m_IDerr(new double[100])
    , m_IDscatPhiRes(new double[100]), m_IDscatPhiErr(new double[100])
    , m_IDscatThetaRes(new double[100]), m_IDscatThetaErr(new double[100])
    , m_MSres(new double[150]), m_MSerr(new double[150])
    , m_MSstatLayer(new int[150]), m_MSisEndcap(new int[150])
    , m_MStrigRes(new double[50]), m_MStrigErr(new double[50])
    , m_MStrigStatLayer(new int[50]), m_MStrigIsEndcap(new int[50])
    , m_MSscatPhiRes(new double[100]), m_MSscatPhiErr(new double[100])
    , m_MSscatThetaRes(new double[100]), m_MSscatThetaErr(new double[100])
    , m_CalRes(new double[10]), m_CalErr(new double[10])
    , m_nChambers(0)
    , m_chamberId(new int[50])
    , m_resIndex(new int[50])
    , m_beePosX(0)
    , m_beePosY(0)
    , m_nProcessed(0)
    , m_nPassHitRemoval(0)
    , m_nPassTGCHitRemoval(0)
    , m_nPassBarrelVeto(0)
    , m_nPassInnerLayer(0)
    , m_nPassOuterLayer(0)
    , m_nPassSmallLargeOverlap(0)
    , m_nPassSectorOverlap(0)
    , m_nPassMinChambers(0)
    , m_nPassMinSmallChambers(0)
    , m_nPassMinLargeChambers(0)
    , m_nPassRPCPhiHit(0)
    , m_nPassStandardTrackSel(0)
    , m_nPassInitTrackSel(0)
    , m_nPassTubeLayerEC(0)
    , m_nPassTubeLayerBarrel(0)
    , m_nPassDriftSignSumEC(0)
    , m_nPassDriftSignSumBarrel(0)
    , m_nPassTrackRefit(0)
    , m_nPassTrackChi2Cut(0)
    , m_nPassOutlierCut(0)
    , m_nPassHoleCut(0)
    , m_nPassFinalTrackSel(0)
    , m_nProcessTrackSelection(0)
    , m_nPassBarrelECOverlap(0)
    , m_nPassNMdtHits(0)
    , m_nPassNLayers(0)
    , m_nPassTrackChi2(0)
    , m_nPassMomentum(0)
    , m_nPassD0(0)
    , m_nPassZ0(0)
  {
    declareInterface<Trk::IAlignTrackPreProcessor>(this);
    
    declareProperty("TrackFitterTool",             m_trackFitterTool);
    declareProperty("SLTrackFitterTool",           m_SLTrackFitterTool);
    declareProperty("MuonIdHelperTool",            m_muonIdHelperTool);
    //declareProperty("TrackSelectorTools",          m_trackSelectorTools);
    declareProperty("MuonAlignRefitTool",          m_muonAlignRefitTool);
    declareProperty("MuonAlignHelperTool",         m_muonAlignHelperTool);

    declareProperty("MuonContainer",               m_muonContainer="");

    //declareProperty("TrackingVolumesSvc",          m_trackingVolumesSvc);

    declareProperty("RefitTracks",                 m_refitTracks = false );    
    declareProperty("ParticleNumber",              m_particleNumber);
    //declareProperty("RunOutlierRemoval",           m_runOutlierRemoval);

    declareProperty("ResetScatteringAngles",       m_resetScatteringAngles = false);
    declareProperty("RemoveScattererTSOS",         m_removeScattererTSOS = false);

    //declareProperty("DeclusterTGCHits",            m_declusterTGCHits = true);
    //declareProperty("RedoErrorScaling",            m_redoErrorScaling = false);
    //declareProperty("RemovePerigeeBeforeRefit",    m_removePerigeeBeforeRefit = false);
    declareProperty("pTCorrectTrack",              m_pTCorrectTrack = false);


    declareProperty("ApplyStandardSelectionCuts",  m_applyStandardSelectionCuts = true);
    declareProperty("ApplySelectionCuts",          m_applySelectionCuts = true);
    declareProperty("RequireInnerLayerEndcapMdt",  m_requireInnerLayerEndcapMdt = false);
    declareProperty("RequireOuterLayerEndcapMdt",  m_requireOuterLayerEndcapMdt = false);
    //declareProperty("CutOnBarrel",                 m_cutOnBarrel = false);
    declareProperty("RequireOneHitPerTubeLayerEC", m_requireOneHitPerTubeLayerEC = false);
    declareProperty("RequireSmallLargeOverlap",    m_requireSmallLargeOverlap = false);
    declareProperty("RequireSectorOverlap",        m_requireSectorOverlap = false);
    declareProperty("OverlapSector1",              m_sector1 = 0);
    declareProperty("OverlapSector2",              m_sector2 = 0);
    declareProperty("RequireBarrelECOverlap",      m_requireBarrelECOverlap = false);
    declareProperty("RequireRPCPhiHit",            m_requireRPCPhiHit = false);

    declareProperty("MinMdtChambers",              m_minMdtChambers = 0);
    declareProperty("MinSmallMdtChambers",         m_minSmallMdtChambers = 0);
    declareProperty("MinLargeMdtChambers",         m_minLargeMdtChambers = 0);

    declareProperty("nAllowedHoles",               m_nAllowedHoles=0);
    declareProperty("nAllowedOutliers",            m_nAllowedOutliers=1);
    declareProperty("MaxDriftSignSumInner",        m_maxDriftSignSumInner=2);
    declareProperty("MaxDriftSignSum",             m_maxDriftSignSum=2);
    declareProperty("chiSqCut",                    m_chiSqCut=5.);
    declareProperty("MSID_ptDiffCut",              m_MSID_ptDiffCut=0.15);
    declareProperty("SelectCombinedMuons",         m_selectCombinedMuons=false);

    m_nIDres=m_nIDscatRes=m_nMSres=m_nMStrigRes=m_nMSscatRes=m_nCalRes=0;
    
  }

  //________________________________________________________________________
  MuonTrackPreProcessor::~MuonTrackPreProcessor()
  {

  }
  
  //________________________________________________________________________
  StatusCode MuonTrackPreProcessor::initialize()
  {

    // check StoreGate service available
    if (StatusCode::SUCCESS != service("StoreGateSvc", m_storeGate)) {
      ATH_MSG_FATAL("StoreGate service not found");
      return StatusCode::FAILURE;
    }

    // get muon detector manager
    if (detStore()->retrieve(p_muonMgr).isFailure()) {
      ATH_MSG_FATAL("Cannot retrieve MuonDetectorManager");
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("retrieved MuonDetectorManager");
    

    // Set up ATLAS ID helper 
    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
      ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
      return StatusCode::FAILURE;
    }    
    m_measTypeIdHelper=new Trk::MeasurementTypeID(m_idHelper);

    // set up MuonIdHelperTool
    if (m_muonIdHelperTool.retrieve().isSuccess()) 
      ATH_MSG_INFO("Retrieved  "<<m_muonIdHelperTool);
    else {
      ATH_MSG_FATAL("Could not get MuonIdHelperTool!");
      return StatusCode::FAILURE;
    }    

    Trk::ParticleSwitcher particleSwitch;
    m_particleHypothesis = particleSwitch.particle[m_particleNumber];
    ATH_MSG_INFO("ParticleNumber: "     << m_particleNumber);
    ATH_MSG_INFO("ParticleHypothesis: " << m_particleHypothesis);
    
    // get track fitter tools
    if (!m_refitTracks && (m_trackFitterTool || m_SLTrackFitterTool)) {
      ATH_MSG_WARNING("Track fitter tool specified but RefitTracks false.  Check configuration.");
    }
    
    //check overlap configuration
    if(m_requireSectorOverlap && m_sector1==0 && m_sector2==0){
      ATH_MSG_FATAL("Sector overlap required but no sectors specified.");
      return StatusCode::FAILURE;
    }


    //get track selector tools
    //if(m_trackSelectorTools.retrieve().isSuccess()){
    //  for (unsigned int k=0; k<m_trackSelectorTools.size(); k++){
    //    ATH_MSG_INFO("Retrieved  "<<m_trackSelectorTools[k]);
    //  }
    //}
    //else{
    //  ATH_MSG_FATAL("Could not get TrackSelectorTools!");
    //  return StatusCode::FAILURE;
    //}
    
    if (m_refitTracks) {
      if (m_trackFitterTool.retrieve().isSuccess()) {
	ATH_MSG_INFO("Retrieved " << m_trackFitterTool);
      }
      else{
	ATH_MSG_FATAL("Could not get " << m_trackFitterTool);
	return StatusCode::FAILURE;
      }
      
      if (m_SLTrackFitterTool.retrieve().isSuccess()) {
	ATH_MSG_INFO("Retrieved " << m_SLTrackFitterTool);
      }
      else{
	ATH_MSG_FATAL("Could not get " << m_SLTrackFitterTool); 
	return StatusCode::FAILURE;
      }          
    }

    /* problems with Eigen migration!!!
    // need to know which TrackingVolume we are in: indet/calo/spectrometer
    if (m_trackingVolumesSvc.retrieve().isFailure())
      {
        ATH_MSG_FATAL( "Failed to retrieve Svc " << m_trackingVolumesSvc );
        return StatusCode::FAILURE;
      } 
    else
    {
      ATH_MSG_INFO( "Retrieved Svc " << m_trackingVolumesSvc );
      m_msVolume    = 0;//rdh migrate to Eigen!!! new Trk::Volume(m_trackingVolumesSvc->volume(ITrackingVolumesSvc::MuonSpectrometerExitLayer));
      m_calVolume   = 0;//rdh migrate to Eigen!!! new Trk::Volume(m_trackingVolumesSvc->volume(ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
      m_indetVolume = 0;//rdh migrate to Eigen!!! new Trk::Volume(m_trackingVolumesSvc->volume(ITrackingVolumesSvc::CalorimeterEntryLayer));
    }
    */

    if (m_pullCalculator.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_pullCalculator);
    else{
      ATH_MSG_FATAL("Could not get " << m_pullCalculator);
      return StatusCode::FAILURE;
    }

    if (m_segmentTrackBuilder.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_segmentTrackBuilder);
    else{
      ATH_MSG_FATAL("Could not get " << m_segmentTrackBuilder); 
      return StatusCode::FAILURE;
    }
    
    /** muonAlignRefitTool */
    if (m_muonAlignRefitTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved "<<m_muonAlignRefitTool);
    else {
      ATH_MSG_FATAL("Could not get "<<m_muonAlignRefitTool);
      return StatusCode::FAILURE;
    }    

    /** muonAlignHelperTool */
    if (m_muonAlignHelperTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved "<<m_muonAlignHelperTool);
    else {
      ATH_MSG_FATAL("Could not get "<<m_muonAlignHelperTool);
      return StatusCode::FAILURE;
    }    

    /** MuonEDMPrinterTool */
    if (m_printer.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved "<<m_printer);
    else {
      ATH_MSG_WARNING("Could not get "<<m_printer);
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
  
  //________________________________________________________________________
  StatusCode MuonTrackPreProcessor::finalize()
  {

    // cut flow
    std::vector<std::string> names;
    std::vector<int>    npass;
    names.push_back("ProcessedTracks");
    names.push_back("HitRemoval");
    names.push_back("TGCHitRemoval");
    names.push_back("ProcessedTrackSelection");
    names.push_back("BarrelECOverlap");
    names.push_back("NMdtHits");
    names.push_back("NLayers");
    names.push_back("TrackChi2");
    names.push_back("Momentum");
    names.push_back("D0");
    names.push_back("Z0");
    names.push_back("StandardCuts");
    names.push_back("BarrelVeto");
    names.push_back("InnerLayer");
    names.push_back("OuterLayer");
    names.push_back("SmallLargeOverlap");
    names.push_back("SectorOverlap");
    names.push_back("MinChambers");
    names.push_back("MinSmallChambers");
    names.push_back("MinLargeChambers");
    names.push_back("RpcPhiHit");
    names.push_back("TubeLayersEC");
    names.push_back("TubeLayersBarrel");
    names.push_back("DriftSignSumEC");
    names.push_back("DriftSignSumBarrel");
    names.push_back("TrackRefit");
    names.push_back("TrackChi2");
    names.push_back("Outliers");
    names.push_back("Holes");
    
    std::cout<<"\nCUT FLOW:" << "\n________\n"<<std::endl;
    npass.push_back(m_nProcessed);    
    npass.push_back(m_nPassHitRemoval);    
    npass.push_back(m_nPassTGCHitRemoval);    
    npass.push_back(m_nProcessTrackSelection);
    npass.push_back(m_nPassBarrelECOverlap);
    npass.push_back(m_nPassNMdtHits);
    npass.push_back(m_nPassNLayers);
    npass.push_back(m_nPassTrackChi2);
    npass.push_back(m_nPassMomentum);
    npass.push_back(m_nPassD0);
    npass.push_back(m_nPassZ0);
    npass.push_back(m_nPassStandardTrackSel);
    npass.push_back(m_nPassBarrelVeto);
    npass.push_back(m_nPassInnerLayer);
    npass.push_back(m_nPassOuterLayer);
    npass.push_back(m_nPassSmallLargeOverlap);
    npass.push_back(m_nPassSectorOverlap);
    npass.push_back(m_nPassMinChambers);
    npass.push_back(m_nPassMinSmallChambers);
    npass.push_back(m_nPassMinLargeChambers);
    npass.push_back(m_nPassRPCPhiHit);
    npass.push_back(m_nPassTubeLayerEC);
    npass.push_back(m_nPassTubeLayerBarrel);
    npass.push_back(m_nPassDriftSignSumEC);
    npass.push_back(m_nPassDriftSignSumBarrel);
    npass.push_back(m_nPassTrackRefit);
    npass.push_back(m_nPassTrackChi2Cut);
    npass.push_back(m_nPassOutlierCut);
    npass.push_back(m_nPassHoleCut);
        
    for (int j=0;j<(int)names.size();j++) {
      std::cout << std::resetiosflags(std::ios::adjustfield)
		<< std::setiosflags(std::ios::left)
		<< std::setw(20)
		<< names[j]
		<< std::resetiosflags(std::ios::adjustfield)
		<< std::setiosflags(std::ios::right)
		<< std::setw(6)
		<< npass[j]
		<< "  ";
      
      if (j>0 && npass[j-1]>0 && npass[0]>0) {
	std::cout << std::setw(6)
		  << std::setprecision(3)
		  << npass[j]/(double)npass[j-1]
		  << "  "
		  << std::setw(6)
		  << std::setprecision(3)
		  << npass[j]/(double)npass[0]
		  << std::endl;
      }
      else std::cout << std::endl;
    }
    
    std::cout << std::endl;
    
    delete m_measTypeIdHelper;

    //delete m_msVolume;
    //delete m_calVolume;
    //delete m_indetVolume;
    
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode MuonTrackPreProcessor::fillNtuple()
  {

    int success=1;
    if (m_tree && m_ntuple) {
      ATH_MSG_DEBUG("in finalize, writing tree");
      m_ntuple->cd();
      success=m_tree->Write();
    }
    return success>0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }

  //________________________________________________________________________
  DataVector<Trk::Track>* 
  MuonTrackPreProcessor::processTrackCollection(const DataVector<Trk::Track>* tracks) 
  {
    if (!m_tree && m_ntuple) createTree();

    //get combined muons, find muon 
    const Analysis::MuonContainer* muons = 0; 
    if(m_selectCombinedMuons) {
      if (evtStore()->contains<Analysis::MuonContainer>(m_muonContainer)) {
	StatusCode sc = evtStore()->retrieve(muons,m_muonContainer);
	if (sc.isFailure()) {	  
	  ATH_MSG_WARNING("No Collection with name "<<m_muonContainer<<" found in StoreGate");
	  return 0;
	}
      } 
      else {
	ATH_MSG_DEBUG("StoreGate does not contain Collection with name "<<m_muonContainer);
	return 0;
      }
    }

    // the output collection of AlignTracks
    // we define it as collection of Tracks but fill AlignTracks inside
    DataVector<Trk::Track>* alignTracks = new DataVector<Trk::Track>;

    // loop over tracks and create AlignTracks
    TrackCollection::const_iterator it     = tracks->begin();
    TrackCollection::const_iterator it_end = tracks->end();
    int itrack(0);
    for ( ;	it != it_end ; ++it,itrack++) {

      ATH_MSG_DEBUG("\ntrack "<<itrack<<"/"<<tracks->size());

      const Trk::Track* origTrack=*it;    
      const Trk::Track* track=0;

      ATH_MSG_DEBUG("original track nDoF: "<<origTrack->fitQuality()->numberDoF());

      // get track fitter
      bool isSLTrack = Trk::AlignTrack::isSLTrack(origTrack);
      ToolHandle<Trk::ITrackFitter> fitter = isSLTrack ? m_SLTrackFitterTool : m_trackFitterTool;
      
      // get qOverP from 1st TSOS on track
      double qOverP=0.;
      if (m_pTCorrectTrack) {
	const DataVector<const Trk::TrackStateOnSurface>* tsos=
	  origTrack->trackStateOnSurfaces();

	if ( !(tsos->empty()) ) {
          const Trk::TrackStateOnSurface* iTsos=tsos->front();
	  ATH_MSG_DEBUG("tsos: "<<*iTsos);
	  qOverP=iTsos->trackParameters()->parameters()[Trk::qOverP];
	  ATH_MSG_DEBUG("qOverP: "<<qOverP<<", 1/qOverP: "<<1./qOverP);
	}

      }

      if (msgLvl(MSG::VERBOSE)) {
	ATH_MSG_VERBOSE("refitting original track for debugging");
	const Trk::Track* tmpTrack=origTrack;
        if (m_pTCorrectTrack) {
	  ATH_MSG_DEBUG("calling pTCorrectTrack with qOverP: "<<qOverP);
	  tmpTrack=m_muonAlignRefitTool->pTCorrectTrack(*origTrack,qOverP,true);
	}
	fitter->fit(*tmpTrack,false,Trk::muon);
	if (m_pTCorrectTrack) delete tmpTrack;
      }
      
      m_nProcessed++;
      
      ATH_MSG_DEBUG("before removing hits: ");
      ATH_MSG_DEBUG(m_printer->print(*origTrack)<<"\n"<<
		    m_printer->printStations(*origTrack)<<"\n"<<
		    m_printer->printMeasurements(*origTrack));
		    
      /*      
      if (m_removePerigeeBeforeRefit ||
	  m_removeTGCHits       || m_removeTRTHits || m_removeCSCHits || 
          m_removeScattererTSOS || m_removeBarrelMuon || m_removeBEEHits || m_removeEEHits ||
	  m_removeSmallBarrelHits || m_removeSpoiledCSCHits || m_removeCSCPhiHits ||
          m_removeTGCEtaHits || m_removeRPCEtaHits) {
	ATH_MSG_DEBUG("calling removeHits");
	track=removeHits(*origTrack); //rdh perigee,scatterer
      */

      // remove hits for badly aligned chambers
      ATH_MSG_DEBUG("calling prepareHitsOnTrack");
      track=m_muonAlignRefitTool->prepareHitsOnTrack(*origTrack);
      if (!track) {
	ATH_MSG_DEBUG("failed removeHits");
	continue; 
      }
      m_nPassHitRemoval++;
      
      // refit track
      const Trk::Track* oldTrack=track;
      ATH_MSG_DEBUG("before calling refitForAlignment: ");
      ATH_MSG_DEBUG(m_printer->print(*oldTrack)<<"\n"<<
		    m_printer->printStations(*oldTrack)<<"\n"<<
		    m_printer->printMeasurements(*oldTrack));

      ATH_MSG_DEBUG("calling refitForAlignment");
      track=m_muonAlignRefitTool->refitForAlignment(*oldTrack);
      delete oldTrack;
      if (!track) {
	ATH_MSG_INFO("failed track refit");
	continue;
      }

      // correct pT of track (this may not be necessary with new MuonRefitTool)
      if (m_pTCorrectTrack) {
	oldTrack=track;
	ATH_MSG_DEBUG("getting pTCorrectTrack with qOverP: "<<qOverP);
	track=m_muonAlignRefitTool->pTCorrectTrack(*oldTrack,qOverP,true);
	delete oldTrack;
	if (!track) {
	  ATH_MSG_DEBUG("failed pTCorrectTrack");
	  continue;
	}
      }

      // check if track passes standard selection
      if (!passesStandardTrackSelection(*track)) {
	if (track!=origTrack) delete track;
	continue;
      }
      m_nPassStandardTrackSel++;

      // check if track passes initial track selection
      if (!passesInitTrackSelection(*track)) {
	if (track!=origTrack) {
	  ATH_MSG_DEBUG("deleting track");
	  delete track;
	}
	continue;
      }
      m_nPassInitTrackSel++;
            
      // refit track with local fitter to get derivatives   
      Trk::AlignTrack* alignTrack(0);
      if (track) {
	if (!track->perigeeParameters()) ATH_MSG_DEBUG("track has no perigee");
	ATH_MSG_DEBUG("creating alignTrack");
	alignTrack=refitTrack(*track);		
      }
      
      if (track!=origTrack) delete track;
      
      // go to next track if track refit refails
      if (!alignTrack) {
	ATH_MSG_DEBUG("track refit failed");
	continue;
      }
      m_nPassTrackRefit++;

      // dump chamber names
      //if (msgLvl(MSG::DEBUG)) dumpChamberNames(*alignTrack);

      if (!origTrack->perigeeParameters()) ATH_MSG_DEBUG("no perigee parameters for orig track!");
      alignTrack->setOriginalTrack(origTrack);
     
      // check if track passes final track selection
      if (!passesFinalTrackSelection(*alignTrack)) {
	delete alignTrack; 
	continue;
      }      

      m_nPassFinalTrackSel++;
      
      // pT cut
      if (!m_pTCorrectTrack) {
	double pT=alignTrack->perigeeParameters()->pT();
	ATH_MSG_DEBUG("pT="<<pT);      
	if (pT>500000.) {
	  ATH_MSG_DEBUG("pT="<<pT<<", too high! (cut at 500 GeV)");
	  //continue;
	}
      }

      if (m_selectCombinedMuons) {

        // see if passes pTDiff cut
        bool passesPtDiffCut=false;
   
        if (muons->size()>1) ATH_MSG_WARNING("Need to implement selection to get correct muon!");

        for (Analysis::MuonContainer::const_iterator muonItr=muons->begin(); 
             muonItr != muons->end(); ++muonItr) {

          if (m_selectCombinedMuons && 
              !(*muonItr)->hasCombinedMuonTrackParticle()) {
            ATH_MSG_DEBUG("no combinedMuonTrackParticle!");
            continue;
          }
	
          double idPt = (*muonItr)->hasInDetTrackParticle() ? 
            (*muonItr)->inDetTrackParticle()->pt() : 0.;
          ATH_MSG_DEBUG("idPt: "<<idPt);

	  // put cuts: 1 pixel, 5 SCT

          double msExtrapPt = (*muonItr)->muonExtrapolatedTrackParticle() ? 
            (*muonItr)->muonExtrapolatedTrackParticle()->pt() : 0.;
          ATH_MSG_DEBUG("msExtrapPt: "<<msExtrapPt);
	
          double ptDiff = std::fabs(msExtrapPt/idPt-1.);
          ATH_MSG_DEBUG("pT diff="<<ptDiff);
		
          if (ptDiff<m_MSID_ptDiffCut) passesPtDiffCut = true;	
        }
      
        if (!passesPtDiffCut) {
	  ATH_MSG_DEBUG("fails ptDiff cut");
          delete alignTrack;
          continue;
        }
      }
      
      ATH_MSG_DEBUG("after refit and passing final selection: ");
      ATH_MSG_DEBUG(m_printer->print(*alignTrack)<<"\n"<<
		    m_printer->printStations(*alignTrack)<<"\n"<<
		    m_printer->printMeasurements(*alignTrack));


      // get run and event numbers
      ATH_MSG_DEBUG("Retrieving event info.");
      const xAOD::EventInfo* eventInfo;
      StatusCode sc = m_storeGate->retrieve(eventInfo);
      if (sc.isFailure()) {
	ATH_MSG_ERROR("Could not retrieve event info.");
      }
      m_runNumber = eventInfo->runNumber();
      m_evtNumber = eventInfo->eventNumber();
      

      // calculate chi2
      calculateTrackChi2(alignTrack);

      // fill tree
      if (m_ntuple) {
	m_ntuple->cd();
	m_tree->Fill();
      }

      // add to alignTracks
      alignTracks->push_back(alignTrack);
      
      // dump some track information
      /*
      if (msgLvl(MSG::DEBUG)) {// && !msgLvl(MSG::VERBOSE)) {
	ATH_MSG_DEBUG("before refit and hit removal: ");
	Trk::AlignTrack::dumpLessTrackInfo(*origTrack,msg(MSG::DEBUG));
	ATH_MSG_DEBUG("after refit: ");
	Trk::AlignTrack::dumpLessTrackInfo(*alignTrack,msg(MSG::DEBUG));
	ATH_MSG_DEBUG(" ");
      */      
    }
    return alignTracks;
  }  

  //________________________________________________________________________
  bool MuonTrackPreProcessor::passesStandardTrackSelection(const Trk::Track& track) 
  {
    ATH_MSG_DEBUG("in passesStandardTrackSelection");

    int nBIS8Hits=0;
    int nECLayerHits[4]={0,0,0,0};
    int nBarrelLayerHits[4]={0,0,0,0};
    
    // search first valid TSOS first    
    const DataVector<const Trk::TrackStateOnSurface>* oldTSOS=track.trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=oldTSOS->begin(); 
    for (iTsos=oldTSOS->begin(); iTsos!=oldTSOS->end(); ++iTsos) {

      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) {
	
	// get measurement, continue if hit to be removed
	const Trk::MeasurementBase*      mesb = (*iTsos)->measurementOnTrack();      
	const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=&crio->rioOnTrack(0);
	if (!rio) continue;
	
	Identifier detId=rio->detectorElement()->identify();
	//Identifier rioId=rio->identify();
	if (!m_muonIdHelperTool->isMuon(detId))  continue;
	
	// variables needed for MDT checks
	bool       isMdt=m_muonIdHelperTool->isMdt(detId);
	bool       isCsc=m_muonIdHelperTool->isCsc(detId);
	int        stationLayer(3);
	
	if (isMdt)	    
	  stationLayer = (int)m_muonAlignHelperTool->getMdtStatLayer(detId);
	else if (isCsc)
	  stationLayer = 0;

	// barrel selection
	if (!isEndcap(detId) && !isBIS8(detId)) {
	  
	  if (isMdt) 
	    nBarrelLayerHits[stationLayer]++;
	    
	}
	// selection cuts specific to endcap
	else {
	  
	  if (isMdt) { 
	    nECLayerHits[stationLayer]++;
	    if (isBIS8(detId)) nBIS8Hits++;
	  }
	  else if (isCsc)
	    nECLayerHits[stationLayer]++;

	} // EC checks
	
      } // measurement checks     
    } // end loop over TSOS

    double chiSq=track.fitQuality()->chiSquared()/
      (double)track.fitQuality()->numberDoF();

    
    //int trackNDoF = track.fitQuality()->numberDoF();    
    double d0(0.),z0(0.),pT(-99999.),eta(-999.),momentum(-99999.);
    
    ATH_MSG_DEBUG("getting d0/z0...");

    if (!m_pTCorrectTrack) {
      if (!track.perigeeParameters()) ATH_MSG_ERROR("no perigee parameters!");
      d0=track.perigeeParameters()->parameters()[Trk::d0];
      z0=track.perigeeParameters()->parameters()[Trk::z0];
      pT=track.perigeeParameters()->pT();
      eta=track.perigeeParameters()->eta();
      momentum=std::fabs(pT)*std::sqrt(1.+sinh(eta)*sinh(eta));
    }

    //
    // check selections all at once
    //
    ATH_MSG_DEBUG("checking layer hits");

    int nECHits    =nECLayerHits[0]+nECLayerHits[1]+nECLayerHits[2]+nECLayerHits[3];
    int nBarrelHits=nBarrelLayerHits[0]+nBarrelLayerHits[1]+nBarrelLayerHits[2];

    ATH_MSG_DEBUG("EC: "<<nECHits<<", Barrel: "<<nBarrelHits);
    ATH_MSG_DEBUG("EC[3]: "<<nECLayerHits[3]<<", Barrel[3]: "<<nBarrelLayerHits[3]);
    m_nProcessTrackSelection++;

    if (m_requireBarrelECOverlap && (nECHits==0 || nBarrelHits==0)) {
      ATH_MSG_DEBUG("failed barrel-EC overlap");
      return false;
    }
    m_nPassBarrelECOverlap++;

    if (!m_applyStandardSelectionCuts) {
      ATH_MSG_DEBUG("returning without applying standard selection cuts");
      return true;
    }
    
    // cut on number MDT hits
    if (nECHits+nBarrelHits < 17) {
      ATH_MSG_DEBUG("fail # MDT hits with "<<nECHits<<" EC and "<<nBarrelHits<<" Barrel");
      return 0;
    }
    m_nPassNMdtHits++;
    
    ATH_MSG_DEBUG("cut on number layers");

    // cut on number of layers
    if (nECLayerHits[3]<4 && nBIS8Hits==0 && 
	(nECLayerHits[0]+nBarrelLayerHits[0]<6 || 
	 (nECLayerHits[1]+nBarrelLayerHits[1]<4) || 
	 nECLayerHits[2]+nBarrelLayerHits[2]<4)) {
      ATH_MSG_DEBUG("failed # layers with "<<nECLayerHits[0]+nBarrelLayerHits[0]<<" inner, "<<
		    nECLayerHits[1]+nBarrelLayerHits[1]<<" middle, "<<
		    nECLayerHits[2]+nBarrelLayerHits[2]<<" outer, "<<
		    nECLayerHits[3]<<" other");
      return 0; 
    }
    m_nPassNLayers++;
    
    // track chi2 cut
    if (chiSq > 3.) {
      ATH_MSG_DEBUG("failed track chi2 cut with chiSq/nDoF="<<chiSq);
      return 0;
    }
    m_nPassTrackChi2++;

    // if perigee removed from track, bypass remaining cuts, which don't make sense 
    // since we don't know them
    if (m_pTCorrectTrack) return true;

    // momentum cut
    if (momentum<5000.) {
      ATH_MSG_DEBUG("failed momentum cut with "<<momentum/1000.<<" GeV");
      return 0.;
    }
    m_nPassMomentum++;

    // d0 cut
    if (std::fabs(d0)>1000.) {
      ATH_MSG_DEBUG("failed d0 cut with "<<std::fabs(d0));
      return 0;
    }
    m_nPassD0++;

    // z0 cut
    if (std::fabs(z0)>2000.) {
      ATH_MSG_DEBUG("failed z0 cut with "<<std::fabs(z0));
      return 0;
    }
    m_nPassZ0++;

    return true;

  }
  //________________________________________________________________________
  bool MuonTrackPreProcessor::passesInitTrackSelection(const Trk::Track& /*track*/) 
  {
    ATH_MSG_ERROR("migrate passesInitTrackSelection to Eigen");
    return false;

    /*
    ATH_MSG_DEBUG("in passesInitTrackSelection");

    bool hasBarrelHit     =false;
    bool hasEndcapHit     =false;
    bool hitsSmallChamber =false;
    bool hitsLargeChamber =false;
    bool hitsFirstSector  =false;
    bool hitsSecondSector =false;
    bool hasRPCPhiHit     =false;

    int nECLayerHits[4]={0,0,0,0};
    int nBarrelLayerHits[4]={0,0,0,0};

    std::map<Identifier, std::vector<Identifier> > endcapTubeLayerIdMap; 
    std::map<Identifier, std::vector<Identifier> > barrelTubeLayerIdMap; 
    std::map<Identifier, std::vector<int> >        endcapDriftSignSumMap; 
    std::map<Identifier, std::vector<int> >        barrelDriftSignSumMap; 
    std::map<Identifier, std::vector<int> >        endcapDriftSignNHitsMap; 
    std::map<Identifier, std::vector<int> >        barrelDriftSignNHitsMap; 

    std::vector<Identifier> chamberIDs;
    std::vector<Identifier> smallChamberIDs;
    std::vector<Identifier> largeChamberIDs;
    
    // search first valid TSOS first    
    m_nChambers=-1;
    Amg::Vector3D avgBeePos(0.,0.,0.);
    int nBeeHits(0);
    Trk::Surface* beeSurf(0);
    m_beePosX=m_beePosY=-9999.;
    
    const DataVector<const Trk::TrackStateOnSurface>* oldTSOS=track.trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=oldTSOS->begin(); 
    for (iTsos=oldTSOS->begin(); iTsos!=oldTSOS->end(); ++iTsos) {
      
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) {
	
	// get measurement, continue if hit to be removed
	const Trk::MeasurementBase*      mesb = (*iTsos)->measurementOnTrack();      
	const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=&crio->rioOnTrack(0);
	if (!rio) continue;
	
	Identifier detId=rio->detectorElement()->identify();
	Identifier rioId=rio->identify();
	if (!m_muonIdHelperTool->isMuon(detId))  continue;
	
	// variables needed for MDT checks
	bool       isMdt=m_muonIdHelperTool->isMdt(detId);
	Identifier chamberId(0);
	Identifier tubeLayerId(0);
	int        driftSign(0);
	int        multilayer(0);
	int        stationLayer(3);

	if (isMdt) {	    
	  
	  chamberId    = m_muonIdHelperTool->chamberId(detId);
	  tubeLayerId  = m_muonIdHelperTool->gasGapId(rioId);
	  driftSign    = rio->localParameters()[Trk::driftRadius] > 0. ? 1 : -1;
	  multilayer   = m_muonIdHelperTool->mdtIdHelper().multilayer(rioId)-1;
	  stationLayer = getMdtStatLayer(detId);
	  ATH_MSG_VERBOSE("chamberId: "<<chamberId<<", tubeLayerId: "<<tubeLayerId<<", multilayer: "<<multilayer);
	  
	  // check small or large chamber
	  bool isSmall=m_muonIdHelperTool->isSmallChamber(detId);
	  if (isSmall) hitsSmallChamber = true;
	  else         hitsLargeChamber = true;	    
	  
	  // check sector hit
	  int sector=m_muonIdHelperTool->sector(detId)-1;
	  if (sector==m_sector1) hitsFirstSector =true;
	  if (sector==m_sector2) hitsSecondSector=true;
	  
	  // count number of chambers (incl. small and large)	
	  if (std::find(chamberIDs.begin(),chamberIDs.end(),chamberId)
	      ==chamberIDs.end()) {
	    chamberIDs.push_back(chamberId);
	  }
	  if (m_muonIdHelperTool->isSmallChamber(detId)) {
	    if (std::find(smallChamberIDs.begin(),smallChamberIDs.end(),chamberId)
		==smallChamberIDs.end()) {
	      smallChamberIDs.push_back(chamberId);
	    }
	  }
	  else {
	    if (std::find(largeChamberIDs.begin(),largeChamberIDs.end(),chamberId)
		==largeChamberIDs.end()) {
	      largeChamberIDs.push_back(chamberId);
	    }
	  }

	  // BEE hits
	  if (m_muonIdHelperTool->mdtIdHelper().stationNameString(m_muonIdHelperTool->mdtIdHelper().stationName(detId)).substr(0,3)=="BEE") {
	    const Amg::Vector3D globPos=rio->globalPosition();
	    avgBeePos += globPos;
	    nBeeHits++;	    
	    if (!beeSurf) beeSurf=rio->associatedSurface().associatedDetectorElement()->surface().clone();
	  }
	} // MDT checks 
	
	// barrel selection
	if (!isEndcap(detId) && !isBIS8(detId)) {

	  hasBarrelHit=true;

	  nBarrelLayerHits[stationLayer]++;

	  if (isMdt) {
	    // count barrel tube layers
	    std::map<Identifier, std::vector<Identifier> >::iterator tubeLayerVec = 
	      barrelTubeLayerIdMap.find(chamberId);
	    if (tubeLayerVec==barrelTubeLayerIdMap.end()) {
	      std::vector<Identifier> ids; ids.push_back(tubeLayerId);
	      barrelTubeLayerIdMap.insert(std::pair<Identifier, std::vector<Identifier> >(chamberId,ids));
	    }
	    else {
	      if (std::find(tubeLayerVec->second.begin(),tubeLayerVec->second.end(),tubeLayerId)==
		  tubeLayerVec->second.end()) {
		tubeLayerVec->second.push_back(tubeLayerId);
	      }
	    }
	    
	    
	    // check drift sign sum per multilayer
	    std::map<Identifier, std::vector<int> >::iterator driftSignSumIt = 
	      barrelDriftSignSumMap.find(chamberId);
	    std::map<Identifier, std::vector<int> >::iterator driftSignNHitsIt = 
	      barrelDriftSignNHitsMap.find(chamberId);
	    if (driftSignSumIt==barrelDriftSignSumMap.end()) {
	      
	      // create vector with 2 zeros (one for each multilayer)
	      std::vector<int> driftSigns; driftSigns.push_back(0); driftSigns.push_back(0);
	      std::vector<int> nHits;      nHits.push_back(0);      nHits.push_back(0);
	      barrelDriftSignSumMap.insert(std::pair<Identifier, std::vector<int> >(chamberId,driftSigns)); 
	      barrelDriftSignNHitsMap.insert(std::pair<Identifier, std::vector<int> >(chamberId,nHits)); 
	      driftSignSumIt   = barrelDriftSignSumMap.find(chamberId);
	      driftSignNHitsIt = barrelDriftSignNHitsMap.find(chamberId);
	    }	      

	    // add drift sign to appropriate multilayer entry
	    driftSignSumIt->second[multilayer] += driftSign;
	    driftSignNHitsIt->second[multilayer]++;

	  } // MDT checks
          
          if (m_requireRPCPhiHit && !hasRPCPhiHit) {
            if (m_muonIdHelperTool->isRpc(detId)) {
              if (m_muonIdHelperTool->measuresPhi(rioId)) {
                hasRPCPhiHit = true;
              }
            }
          }

	} // Barrel checks
	
	// selection cuts specific to endcap
	else {

	  hasEndcapHit=true;

	  if (isMdt) {
	    nECLayerHits[stationLayer]++;
	    
	    std::map<Identifier, std::vector<Identifier> >::iterator tubeLayerVec = 
	      endcapTubeLayerIdMap.find(chamberId);
	    if (tubeLayerVec==endcapTubeLayerIdMap.end()) {
	      std::vector<Identifier> ids; ids.push_back(tubeLayerId);
	      endcapTubeLayerIdMap.insert(std::pair<Identifier, std::vector<Identifier> >(chamberId,ids));
	    }
	    else {
	      if (std::find(tubeLayerVec->second.begin(),tubeLayerVec->second.end(),tubeLayerId)==
		  tubeLayerVec->second.end()) {
		tubeLayerVec->second.push_back(tubeLayerId);
	      }
	    }
	    
	    // check drift sign sum per multilayer
	    std::map<Identifier, std::vector<int> >::iterator driftSignSumIt = 
	      endcapDriftSignSumMap.find(chamberId);
	    std::map<Identifier, std::vector<int> >::iterator driftSignNHitsIt = 
	      endcapDriftSignNHitsMap.find(chamberId);
	    if (driftSignSumIt==endcapDriftSignSumMap.end()) {
	      
	      // create vector with 2 zeros (one for each multilayer)
	      std::vector<int> driftSigns; driftSigns.push_back(0); driftSigns.push_back(0);
	      std::vector<int> nHits;      nHits.push_back(0);      nHits.push_back(0);
	      endcapDriftSignSumMap.insert(std::pair<Identifier, std::vector<int> >(chamberId,driftSigns)); 
	      endcapDriftSignNHitsMap.insert(std::pair<Identifier, std::vector<int> >(chamberId,nHits));
	      driftSignSumIt   = endcapDriftSignSumMap.find(chamberId);
	      driftSignNHitsIt = endcapDriftSignNHitsMap.find(chamberId);
	    }	      

	    // add drift sign to appropriate multilayer entry
	    driftSignSumIt->second[multilayer] += driftSign;   
	    driftSignNHitsIt->second[multilayer]++;   

	  } // MDT checks  	  
	} // EC checks
      } // measurement checks     
    } // end loop over TSOS

    // average BEE position
    if (nBeeHits>0) {
      avgBeePos/=(double)nBeeHits;
      const Amg::Vector2D* locPos=beeSurf->positionOnSurface(avgBeePos,false,1000.);
      m_beePosX=locPos->get(Trk::locX);
      m_beePosY=locPos->get(Trk::locY);
      delete beeSurf;
      delete locPos;
    }

    // check tube layers hit
    int nTubeLayersHitEC[4]={0,0,0,0}; // inner,middle,outer,other
    std::map<Identifier, std::vector<Identifier> >::const_iterator it   =endcapTubeLayerIdMap.begin();
    std::map<Identifier, std::vector<Identifier> >::const_iterator itend=endcapTubeLayerIdMap.end();    
    for (; it!=itend; ++it) {	
      int statLayer=getMdtStatLayer(it->first);
      nTubeLayersHitEC[statLayer] += it->second.size();
    }
    
    int nTubeLayersHitBarrel[4]={0,0,0,0}; 
    it   =endcapTubeLayerIdMap.begin();
    itend=endcapTubeLayerIdMap.end();   
    for (; it!=itend; ++it) {	
      int statLayer=getMdtStatLayer(it->first);
      nTubeLayersHitBarrel[statLayer] += it->second.size();
    }
    
    // check drift sign sums
    double maxDriftSignSumEC[4]={0.,0.,0.,0.};    
    std::map<Identifier, std::vector<int> >::const_iterator driftSignSumIt = 
      endcapDriftSignSumMap.begin();
    std::map<Identifier, std::vector<int> >::const_iterator driftSignSumItEnd = 
      endcapDriftSignSumMap.end();      
    std::map<Identifier, std::vector<int> >::const_iterator driftSignNHitsIt = 
      endcapDriftSignNHitsMap.begin();
    for (; driftSignSumIt!=driftSignSumItEnd;++driftSignSumIt,++driftSignNHitsIt) {	
      int statLayer=getMdtStatLayer(driftSignSumIt->first);
      int nhits0=driftSignNHitsIt->second[0];
      int nhits1=driftSignNHitsIt->second[1];
      double driftSignSum0 = (nhits0>0) ? (double)driftSignSumIt->second[0]/(double)nhits0 : 0.;
      double driftSignSum1 = (nhits1>0) ? (double)driftSignSumIt->second[1]/(double)nhits1 : 0.;
      if (driftSignSum0>maxDriftSignSumEC[statLayer])
	maxDriftSignSumEC[statLayer]=std::fabs(driftSignSum0);
      if (driftSignSum1>maxDriftSignSumEC[statLayer])
	maxDriftSignSumEC[statLayer]=std::fabs(driftSignSum1);
    }
    
    double maxDriftSignSumBarrel[4]={0.,0.,0.,0.};    
    driftSignSumIt    = barrelDriftSignSumMap.begin();
    driftSignSumItEnd = barrelDriftSignSumMap.end();      
    driftSignNHitsIt  = barrelDriftSignNHitsMap.begin();
    for (; driftSignSumIt!=driftSignSumItEnd;++driftSignSumIt,++driftSignNHitsIt) {	
      int statLayer=getMdtStatLayer(driftSignSumIt->first);
      int nhits0=driftSignNHitsIt->second[0];
      int nhits1=driftSignNHitsIt->second[1];
      double driftSignSum0 = (nhits0>0) ? (double)driftSignSumIt->second[0]/(double)nhits0 : 0.;
      double driftSignSum1 = (nhits1>0) ? (double)driftSignSumIt->second[1]/(double)nhits1 : 0.;
      if (driftSignSum0>maxDriftSignSumBarrel[statLayer])
	maxDriftSignSumBarrel[statLayer]=std::fabs(driftSignSum0);
      if (driftSignSum1>maxDriftSignSumBarrel[statLayer])
	maxDriftSignSumBarrel[statLayer]=std::fabs(driftSignSum1);
    }
    
    // set variables for tree
    m_nInnerLayersHit      = nECLayerHits[0]+nBarrelLayerHits[0];
    m_nMiddleLayersHit     = nECLayerHits[1]+nBarrelLayerHits[1];
    m_nOuterLayersHit      = nECLayerHits[2]+nBarrelLayerHits[2];
    m_nOtherLayersHit      = nECLayerHits[3]+nBarrelLayerHits[3];
    m_nTubeLayersHitInner  = nTubeLayersHitEC[0]+nTubeLayersHitBarrel[0];
    m_nTubeLayersHitMiddle = nTubeLayersHitEC[1]+nTubeLayersHitBarrel[0];
    m_nTubeLayersHitOuter  = nTubeLayersHitEC[2]+nTubeLayersHitBarrel[0];
    m_nTubeLayersHitOther  = nTubeLayersHitEC[3]+nTubeLayersHitBarrel[0];
    m_driftSignsInnerLayer = maxDriftSignSumEC[0];
    m_driftSignsMiddleLayer= maxDriftSignSumEC[1];
    m_driftSignsOuterLayer = maxDriftSignSumEC[2];
    m_driftSignsOtherLayer = maxDriftSignSumEC[3];
    
    //
    // check selections all at once
    //    
    if (!m_applySelectionCuts) return true;

    if (m_cutOnBarrel                && hasBarrelHit                                  ) return 0; m_nPassBarrelVeto++;
    if (m_requireInnerLayerEndcapMdt && nECLayerHits[0]==0                            ) return 0; m_nPassInnerLayer++;
    if (m_requireOuterLayerEndcapMdt && nECLayerHits[2]==0                            ) return 0; m_nPassOuterLayer++;
    if (m_requireSmallLargeOverlap   && (!hitsSmallChamber || !hitsLargeChamber)      ) return 0; m_nPassSmallLargeOverlap++;
    if (m_requireSectorOverlap       && (!hitsFirstSector || !hitsSecondSector)       ) return 0; m_nPassSectorOverlap++;
    if (m_minMdtChambers>0           && chamberIDs.size() < m_minMdtChambers          ) return 0; m_nPassMinChambers++;
    if (m_minSmallMdtChambers>0      && smallChamberIDs.size() < m_minSmallMdtChambers) return 0; m_nPassMinSmallChambers++;
    if (m_minLargeMdtChambers>0      && largeChamberIDs.size() < m_minLargeMdtChambers) return 0; m_nPassMinLargeChambers++;
    if (m_requireRPCPhiHit           && !hasRPCPhiHit)  return false; m_nPassRPCPhiHit++;
    
    // check tube layers hit
    if (m_requireOneHitPerTubeLayerEC && nTubeLayersHitEC[3] == 0 && 
	(nTubeLayersHitEC[0] != 8 || 
	 nTubeLayersHitEC[1] != 6 || 
	 nTubeLayersHitEC[2] != 6) ) return false; 
    m_nPassTubeLayerEC++;
    
    // check drift sign sums for each multilayer
    if ((m_maxDriftSignSum>0 || m_maxDriftSignSumInner>0) && 
	(maxDriftSignSumEC[0]>m_maxDriftSignSumInner ||
	 maxDriftSignSumEC[1]>m_maxDriftSignSum ||
	 maxDriftSignSumEC[2]>m_maxDriftSignSum)) return 0; 
    m_nPassDriftSignSumEC++;

    return true;
    */
  }

  /*
  //________________________________________________________________________
  const Trk::Track* MuonTrackPreProcessor::removeHits(const Trk::Track& track) const
  {
    if (m_removePerigeeBeforeRefit) ATH_MSG_DEBUG("MTPP: removePerigeeBeforeRefit");
    if (m_removeTGCHits)         ATH_MSG_DEBUG("MTPP: removeTGCHits");
    if (m_removeTRTHits)         ATH_MSG_DEBUG("MTPP: removeTRTHits");
    if (m_removeCSCHits)         ATH_MSG_DEBUG("MTPP: removeCSCHits");
    if (m_removeScattererTSOS)   ATH_MSG_DEBUG("MTPP: removeScattererTSOS");
    if (m_removeBarrelMuon)      ATH_MSG_DEBUG("MTPP: removeBarrelMuon");
    if (m_removeSmallBarrelHits) ATH_MSG_DEBUG("MTPP: removeSmallBarrelHits");
    if (m_removeSpoiledCSCHits)  ATH_MSG_DEBUG("MTPP: removeSpoiledCSCHits");
    if (m_removeCSCPhiHits)      ATH_MSG_DEBUG("MTPP: removeCSCPhiHits");
    if (m_removeTGCEtaHits)      ATH_MSG_DEBUG("MTPP: removeTGCEtaHits");
    if (m_removeRPCEtaHits)      ATH_MSG_DEBUG("MTPP: removeRPCEtaHits");
    if (m_removeBEEHits)         ATH_MSG_DEBUG("MTPP: removeBEEHits");
    if (m_removeEEHits)          ATH_MSG_DEBUG("MTPP: removeEEHits");

    const DataVector<const Trk::TrackStateOnSurface>* oldTSOS=track.trackStateOnSurfaces();
            
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces =
      new DataVector<const Trk::TrackStateOnSurface>;

    //int nGoodHits(0);

    // search first valid TSOS first
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=oldTSOS->begin(); 
    for (iTsos=oldTSOS->begin(); iTsos!=oldTSOS->end(); ++iTsos) {

      if ((*iTsos)->type(Trk::TrackStateOnSurface::Perigee) && 
	  m_removePerigeeBeforeRefit) {
	continue;
      }

      // get measurement, continue if hit to be removed
      const Trk::MeasurementBase*      mesb     = (*iTsos)->measurementOnTrack();
      Trk::TrackState::MeasurementType measType = m_measTypeIdHelper->defineType(mesb);

      ATH_MSG_VERBOSE("(before) measType="<<measType<<", type: "<<(*iTsos)->dumpType());
      
      // removal of specific types of measurements
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement) &&
	  ((measType==Trk::TrackState::TGC && m_removeTGCHits) ||
	   (measType==Trk::TrackState::TRT && m_removeTRTHits) ||
	   (measType==Trk::TrackState::CSC && m_removeCSCHits) )) continue;

      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement) &&	  
	  measType==Trk::TrackState::CSC && 
	  (m_removeSpoiledCSCHits || m_removeCSCPhiHits)) {

	ATH_MSG_DEBUG("checking CSC hit");

	const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	if (rio) {
	  const CscPrepData* csc_prp_data = dynamic_cast<const CscPrepData*>(rio);	
	  if (csc_prp_data) {
	    bool is_measphi_strip = p_muonMgr->cscIdHelper()->measuresPhi(csc_prp_data->identify());
	    if (is_measphi_strip&&m_removeCSCPhiHits) continue;                           
	    if (!is_measphi_strip && 
		csc_prp_data->status() != CscStatusUnspoiled &&
		m_removeSpoiledCSCHits) {
	      ATH_MSG_DEBUG("spoiled CSC hit");
	      continue;
	    }
	  }
	}
      }
      
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement) &&
	  measType==Trk::TrackState::TGC && m_removeTGCEtaHits) {

	const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=crio->rioOnTrack(0);
	if (rio) {
	  const TgcPrepData* tgc_prp_data = dynamic_cast<const TgcPrepData*>(rio);	
	  if (tgc_prp_data) {
	    if (!p_muonMgr->tgcIdHelper()->isStrip(tgc_prp_data->identify())) continue;
	  }
	}
      }
      
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement) &&
	  measType==Trk::TrackState::RPC && m_removeRPCEtaHits) {
	
       	const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
        const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=crio->rioOnTrack(0);
        if (rio) {
          if(!m_muonIdHelperTool->measuresPhi(rio->identify())){
            continue;
          }
        }
      }

      ATH_MSG_VERBOSE("good hit");
      // removal of scatterers inside MS
      if (m_removeScattererTSOS &&
	  !(*iTsos)->type(Trk::TrackStateOnSurface::Measurement) && 
	  (**iTsos).materialEffectsOnTrack()) {
	Trk::GlobalPosition position =
	  (**iTsos).materialEffectsOnTrack()->associatedSurface().globalReferencePoint();
	if (!m_indetVolume->inside(position) && !m_calVolume->inside(position)) continue;
      }    
      
      // removal of barrel hits
      if ( (m_removeBarrelMuon || m_removeSmallBarrelHits || m_removeBEEHits || m_removeEEHits ) && 
	  (*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) {
	
	const Trk::MeasurementBase*             mesb       = (*iTsos)->measurementOnTrack();	
	const Trk::RIO_OnTrack*                 rio        = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack*        crio       = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=crio->rioOnTrack(0);
	if (rio) {
	  if (m_muonIdHelperTool->isMuon(rio->detectorElement()->identify())) {
	    if (!isEndcap(rio->detectorElement()->identify())) {
	      if (m_removeBarrelMuon) {
		ATH_MSG_VERBOSE("removing barrel muon hit");
		continue;
	      }
	      if (m_removeSmallBarrelHits) { 
		Identifier id=rio->detectorElement()->identify();
		const std::string stName=
		  m_muonIdHelperTool->mdtIdHelper().stationNameString(m_muonIdHelperTool->mdtIdHelper().stationName(id));
		if (stName[2]!='L') {
		  ATH_MSG_VERBOSE("removing small barrel hit");
		  continue;
		}
	      }
	    }
	    
	    ATH_MSG_VERBOSE("keeping hit in "
			  << m_muonIdHelperTool->toStringGasGap(rio->identify()));
	  }
	  if (m_removeBEEHits||m_removeEEHits) {
	    const std::string stName=
	      m_muonIdHelperTool->mdtIdHelper().stationNameString(m_muonIdHelperTool->mdtIdHelper().stationName(rio->identify()));
	    if (m_removeBEEHits && stName.substr(0,3)=="BEE") continue;
	    if (m_removeEEHits  && stName.substr(0,2)=="EE") continue;
	  }	  
	}
      }


      // have a good TSOS, create a new TSOS from old one, removing scattering if necessary
      const Trk::TrackStateOnSurface* newTsos;
      if (m_removeScattererTSOS && 
	  ((**iTsos).type(Trk::TrackStateOnSurface::Scatterer) ||
	   (**iTsos).type(Trk::TrackStateOnSurface::InertMaterial))) {

	ATH_MSG_DEBUG("removing scattering from shared TSOS");

	// check if pure scatterer and remove if it is
	if ((**iTsos).measurementOnTrack()==0) continue;
	
	ATH_MSG_DEBUG("creating new TSOS");

	// have to build a new TSOS without the scattering part
	const Trk::TrackParameters*     tpar=(**iTsos).trackParameters() ?
	  (**iTsos).trackParameters()->clone():0;
	const Trk::MeasurementBase*     meas=(**iTsos).measurementOnTrack() ?
	  (**iTsos).measurementOnTrack()->clone():0;
	const Trk::FitQualityOnSurface* fqos=(**iTsos).fitQualityOnSurface() ? 
	  new Trk::FitQualityOnSurface(*(**iTsos).fitQualityOnSurface()) : 0;

	newTsos=new Trk::TrackStateOnSurface(meas,tpar,fqos);
      }
      else {
	ATH_MSG_VERBOSE("cloning TSOS");
	newTsos=(**iTsos).clone();
      }
      
      //if (newTsos->type(Trk::TrackStateOnSurface::Measurement) &&
      //  !newTsos->type(Trk::TrackStateOnSurface::Outlier)) nGoodHits++;

      ATH_MSG_VERBOSE("(after) measType="<<measType<<", type: "<<newTsos->dumpType());

      trackStateOnSurfaces->push_back(newTsos);
      
    }

    const Trk::Track* newTrack = new Trk::Track(Trk::TrackInfo(track.info()),
                                                trackStateOnSurfaces,
                                                track.fitQuality()->clone());

    //ATH_MSG_DEBUG("# good measurements: "<<nGoodHits);
    //if (nGoodHits<3) {
    //delete newTrack;
    //return 0;
    //}

    ATH_MSG_DEBUG("created newTrack:");
    ATH_MSG_DEBUG(m_printer->print(*newTrack)<<"\n"<<
		  m_printer->printStations(*newTrack)<<"\n");//<<

    return newTrack;    
  }
  */

  /*
  //________________________________________________________________________
  const Trk::Track* MuonTrackPreProcessor::declusterTGCHits(const Trk::Track& track) const
  {
    ATH_MSG_DEBUG("MTPP: declusterTGCHits");

    // layer Identifiers for TGC hits
    std::set<Identifier> layIds;
    
    const DataVector<const Trk::TrackStateOnSurface>* oldTSOS=track.trackStateOnSurfaces();
    
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces =
      new DataVector<const Trk::TrackStateOnSurface>;
    
    // search first valid TSOS first
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=oldTSOS->begin(); 
    for (iTsos=oldTSOS->begin(); iTsos!=oldTSOS->end(); ++iTsos) {

      // get measurement
      const Trk::MeasurementBase*      mesb     = (*iTsos)->measurementOnTrack();
      Trk::TrackState::MeasurementType measType = m_measTypeIdHelper->defineType(mesb);

      ATH_MSG_VERBOSE("(before) measType="<<measType<<", type: "<<(*iTsos)->dumpType());
      
      
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement) &&
	  measType==Trk::TrackState::TGC) {

	// loop over RIO in CRIO, create TSOS from each one
	const Trk::MeasurementBase*             mesb       = (*iTsos)->measurementOnTrack();	
	const Trk::CompetingRIOsOnTrack*        crio       = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!crio) { 
	  ATH_MSG_DEBUG("TGC with no crio!"); 
	  const Trk::RIO_OnTrack* rio=dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	  if (!rio) { ATH_MSG_ERROR("no crio or rio for TGC hit!"); continue; }
	  const Trk::MeasurementBase*     meas=dynamic_cast<const Trk::MeasurementBase*>(rio);
	  const Trk::TrackParameters*     tpar=(**iTsos).trackParameters()->clone();
	  const Trk::FitQualityOnSurface* fqos=(**iTsos).fitQualityOnSurface() ? 
	    new Trk::FitQualityOnSurface(*(**iTsos).fitQualityOnSurface()) : 0;
	  const Trk::MaterialEffectsBase* meb=( (*iTsos)->materialEffectsOnTrack()) ?
	    (*iTsos)->materialEffectsOnTrack()->clone() : 0;

	  const Trk::TrackStateOnSurface* newTsos=
	    new Trk::TrackStateOnSurface(meas,tpar,fqos,meb);

	  trackStateOnSurfaces->push_back(newTsos);
	  continue; 
	}

	for (unsigned int irio=0;irio<crio->numberOfContainedROTs();irio++) {
	  const Trk::RIO_OnTrack* rio=crio->rioOnTrack(irio);
	  if (!rio) continue;
	  Identifier layId = m_muonIdHelperTool->layerId(rio->identify());
	  ATH_MSG_DEBUG("have ID "<<layId<<" for rioId "<<rio->identify());
	  std::pair<std::set<Identifier>::iterator, bool> pr=layIds.insert(layId);
	  if (pr.second == false) continue;
	  ATH_MSG_DEBUG("adding TGC RIO with rioId "<<rio->identify());

	  // attach any material effects to first RIO
	  const Trk::MeasurementBase*     meas=dynamic_cast<const Trk::MeasurementBase*>(rio);
	  const Trk::TrackParameters*     tpar=(**iTsos).trackParameters()->clone();
	  const Trk::FitQualityOnSurface* fqos=(**iTsos).fitQualityOnSurface() ? 
	    new Trk::FitQualityOnSurface(*(**iTsos).fitQualityOnSurface()) : 0;
	  const Trk::MaterialEffectsBase* meb=(irio==0 && ((*iTsos)->materialEffectsOnTrack())) ?
	    (*iTsos)->materialEffectsOnTrack()->clone() : 0;
	  const Trk::TrackStateOnSurface* newTsos=
	    new Trk::TrackStateOnSurface(meas,tpar,fqos,meb);
	  trackStateOnSurfaces->push_back(newTsos);
	}
      }
      else {
	const Trk::TrackStateOnSurface* newTsos=(**iTsos).clone();
	trackStateOnSurfaces->push_back(newTsos);
      }
    }

    const Trk::Track* newTrack = new Trk::Track(Trk::TrackInfo(track.info()),
                                                trackStateOnSurfaces,
                                                track.fitQuality()->clone());
    return newTrack;    
  }
  */

  //________________________________________________________________________
  Trk::AlignTrack* MuonTrackPreProcessor::refitTrack(const Trk::Track& /*origTrack*/) const
  {
    ATH_MSG_ERROR("update refitTrack for Eigen");
    return 0;

    /*
    bool isSLTrack = Trk::AlignTrack::isSLTrack(&origTrack);
    ToolHandle<Trk::ITrackFitter> fitter = isSLTrack ? m_SLTrackFitterTool : m_trackFitterTool;
    
    ATH_MSG_DEBUG("refitting track");
    const Trk::Track* newTrack = 
      fitter->fit(origTrack,m_runOutlierRemoval,m_particleHypothesis);
    ATH_MSG_DEBUG("track refit done");

    if (!newTrack) {
      return 0;
    }

    //ATH_MSG_DEBUG("creating new AlignTrack");
    Trk::AlignTrack* alignTrack=new Trk::AlignTrack(*newTrack);

    // if IGlobalTrackFitter, the covariance and derivative matrices are available, 
    // so set them on the AlignTrack
    const Trk::IGlobalTrackFitter* globFitter = 
      dynamic_cast<const Trk::IGlobalTrackFitter*>(& (*fitter) );
    
    if (globFitter) {
      const CLHEP::HepSymMatrix* mat=globFitter->FullCovarianceMatrix();
      ATH_MSG_DEBUG("mat has "<<mat->num_row()<<" x "<<mat->num_col()<<" entries");
      alignTrack->setFullCovarianceMatrix(mat);
      alignTrack->setDerivativeMatrix(globFitter->DerivMatrix());
    }
    
    // cleanup
    delete newTrack;
    return alignTrack;
    */
  }

  //________________________________________________________________________
  void MuonTrackPreProcessor::identifyScatterers(const Trk::Track& track) const
  {

    const DataVector<const Trk::TrackStateOnSurface>* TSOS=track.trackStateOnSurfaces();
            
    // search first valid TSOS first
    int itsos(0);
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=TSOS->begin(); 
    for (; iTsos!=TSOS->end(); ++iTsos,itsos++) {
      
      ATH_MSG_VERBOSE("\nTSOS: "<<itsos<<"\n"<<**iTsos);
      
      if (!(*iTsos)->type(Trk::TrackStateOnSurface::Scatterer) &&
	  !(*iTsos)->type(Trk::TrackStateOnSurface::InertMaterial)) continue;
      
      if (!(*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) {	
	ATH_MSG_DEBUG("not a measurement, but is a scatterer or inert material!");
      }
      else {
	ATH_MSG_DEBUG("scatterer with measurement");

	const Trk::MeasurementBase*             mesb       = (*iTsos)->measurementOnTrack();	
	const Trk::RIO_OnTrack*                 rio        = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack*        crio       = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=&crio->rioOnTrack(0);
	Identifier rid;
	if (rio) {
	  ATH_MSG_DEBUG("rio detelement identifier: "<<rio->detectorElement()->identify());
	  rid=rio->detectorElement()->identify();
	  if (!m_idHelper->is_muon(rid)) { ATH_MSG_DEBUG("not muon"); continue; }
	  if (m_idHelper->is_mdt(rid)) ATH_MSG_DEBUG("have MDT hit, ID "<<rid);
	  if (m_idHelper->is_tgc(rid)) ATH_MSG_DEBUG("have TGC hit, ID "<<rid);
	  if (m_idHelper->is_csc(rid)) ATH_MSG_DEBUG("have CSC hit, ID "<<rid);
	  if (m_idHelper->is_rpc(rid)) ATH_MSG_DEBUG("have RPC hit, ID "<<rid); 
	}
      }

      const Trk::MaterialEffectsBase* meb = (**iTsos).materialEffectsOnTrack();
      if (!meb) { ATH_MSG_DEBUG("no meb"); continue; }
      ATH_MSG_DEBUG("Surface: \n" << meb->associatedSurface());
      ATH_MSG_DEBUG("Surface ID:\n" << meb->associatedSurface().associatedDetectorElementIdentifier());
      
      const Trk::TrkDetElementBase* detelement=meb->associatedSurface().associatedDetectorElement();
      if (!detelement) { ATH_MSG_DEBUG("no detelement"); continue; }
      
      Identifier id=meb->associatedSurface().associatedDetectorElementIdentifier();
      if (!m_idHelper->is_muon(id)) { ATH_MSG_DEBUG("not muon"); continue; }
      if (m_idHelper->is_mdt(id)) ATH_MSG_DEBUG("have MDT scat, ID "<<id);
      if (m_idHelper->is_tgc(id)) ATH_MSG_DEBUG("have TGC scat, ID "<<id);
      if (m_idHelper->is_csc(id)) ATH_MSG_DEBUG("have CSC scat, ID "<<id);
      if (m_idHelper->is_rpc(id)) ATH_MSG_DEBUG("have RPC scat, ID "<<id);
    } 
    return;
  }	

  //________________________________________________________________________
  bool MuonTrackPreProcessor::passesFinalTrackSelection(const Trk::Track& track) 
  {

    double chiSq=track.fitQuality()->chiSquared()/
      (double)track.fitQuality()->numberDoF();
    int trackNDoF = track.fitQuality()->numberDoF();

    int nOutliers(0);
    int nOutliersPerLayer[3]={0,0,0};
    int nHoles(0);
    int nHolesPerLayer[3]={0,0,0};
    
    // search first valid TSOS first
    const DataVector<const Trk::TrackStateOnSurface>* oldTSOS=track.trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=oldTSOS->begin(); 
    for (iTsos=oldTSOS->begin(); iTsos!=oldTSOS->end(); ++iTsos) {

      if ((*iTsos)->type(Trk::TrackStateOnSurface::Outlier) &&
	  (*iTsos)->measurementOnTrack())                 nOutliers++;
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Hole)) nHoles++;

      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) {
	
	// get measurement, continue if hit to be removed
	const Trk::MeasurementBase*      mesb = (*iTsos)->measurementOnTrack();      
	const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=&crio->rioOnTrack(0);
	if (!rio) continue;
	
	Identifier detId=rio->detectorElement()->identify();
	if (!m_muonIdHelperTool->isMuon(detId))  continue;
	
	int statLayer=getMdtStatLayer(detId);
	if (statLayer>2) continue;
	
	if ((*iTsos)->type(Trk::TrackStateOnSurface::Hole))    nHolesPerLayer[statLayer]++;
	if ((*iTsos)->type(Trk::TrackStateOnSurface::Outlier)) nOutliersPerLayer[statLayer]++;

      } // measurement checks     
    } // end loop over TSOS

    // fill tree variables
    m_trackNDoF            = trackNDoF;
    m_trackChi2            = chiSq;
    //m_trackSagitta         = 0.; //trackSagitta(track);
    m_nInnerLayerOutliers  = nOutliersPerLayer[0];
    m_nMiddleLayerOutliers = nOutliersPerLayer[1];
    m_nOuterLayerOutliers  = nOutliersPerLayer[2];
    m_nInnerLayerHoles     = nHolesPerLayer[0];
    m_nMiddleLayerHoles    = nHolesPerLayer[1];
    m_nOuterLayerHoles     = nHolesPerLayer[2];
    m_trackPt  = track.perigeeParameters() ? track.perigeeParameters()->pT() : 0.;
    m_trackEta = track.perigeeParameters() ? track.perigeeParameters()->eta() : -999.;
    double momentum = track.perigeeParameters() ? 
      std::fabs(m_trackPt)*std::sqrt(1.+sinh(m_trackEta)*sinh(m_trackEta)) : -999.;
    m_qOverP   = (momentum>0.) ? track.perigeeParameters()->charge() / momentum : 0.;  

    // get residuals from original tree (for output tree only... these aren't used for alignment)
    m_nresiduals=0;
    m_nChambers=-1;
    iTsos=oldTSOS->begin(); 
    for (; iTsos!=oldTSOS->end(); ++iTsos) {
      
      if (m_nresiduals>=100) break;
      
      const Trk::MeasurementBase*      mesb     = (*iTsos)->measurementOnTrack();
      if (!mesb) continue;
      Trk::TrackState::MeasurementType measType = m_measTypeIdHelper->defineType(mesb);
      if (measType!=Trk::TrackState::MDT) continue;
      
      const Trk::RIO_OnTrack* rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      if (!rio) continue;
      Identifier detId=rio->detectorElement()->identify();
      int chid = m_muonIdHelperTool->chamberId(detId).get_identifier32().get_compact();
      if (m_nChambers==-1 || m_chamberId[m_nChambers]!=chid) {
	m_nChambers++;
	if (m_nChambers>=50) continue;
	m_chamberId[m_nChambers]=chid;
	m_resIndex[m_nChambers]=m_nresiduals;
      }
      
      const Trk::ResidualPull* resPull=
	m_pullCalculator->residualPull((**iTsos).measurementOnTrack(),
				       (**iTsos).trackParameters(),
				       Trk::ResidualPull::HitOnly,measType);
      double res=(resPull->residual())[Trk::driftRadius];
      m_residuals[m_nresiduals]=res;
      m_respulls [m_nresiduals++]=(resPull->pull())[Trk::driftRadius];

    }
        
    //
    // check selections all at once
    //  
    if (!m_applySelectionCuts) return true;

    if (m_chiSqCut>0.         && chiSq>m_chiSqCut            ) return 0;
    m_nPassTrackChi2Cut++;    
    if (m_nAllowedOutliers>=0 && nOutliers>m_nAllowedOutliers) return 0;
    m_nPassOutlierCut++;
    if (m_nAllowedHoles>=0    && nOutliers>m_nAllowedHoles   ) return 0;
    m_nPassHoleCut++;

    return true;
  }

  //________________________________________________________________________
  void MuonTrackPreProcessor::createTree() 
  {
    ATH_MSG_INFO("creating tree in MuonTrackPreProcessor");
    m_ntuple->cd();
    m_tree=new TTree("MuonTrackPreProcessor","MuonTrackPreProcessor");
    m_tree->Branch("run",                    &m_runNumber,              "run/I");
    m_tree->Branch("evt",                    &m_evtNumber,              "evt/I");
    m_tree->Branch("nresiduals",             &m_nresiduals,             "nresiduals/I");
    m_tree->Branch("residuals",               m_residuals,              "residuals[nresiduals]/D");
    m_tree->Branch("respulls",                m_respulls,               "respulls[nresiduals]/D");
    m_tree->Branch("nInnerLayersHit",        &m_nInnerLayersHit,        "nInnerLayersHit/I");
    m_tree->Branch("nMiddleLayersHit",       &m_nMiddleLayersHit,       "nMiddleLayersHit/I");
    m_tree->Branch("nOuterLayersHit",        &m_nOuterLayersHit,        "nOuterLayersHit/I");
    m_tree->Branch("nOtherLayersHit",        &m_nOtherLayersHit,        "nOtherLayersHit/I");
    m_tree->Branch("nChambersHit",           &m_nChambersHit,           "nChambersHit/I");
    m_tree->Branch("nLargeChambersHit",      &m_nLargeChambersHit,      "nLargeChambersHit/I");
    m_tree->Branch("nSmallChambersHit",      &m_nSmallChambersHit,      "nSmallChambersHit/I");
    m_tree->Branch("nTubeLayersHitInner",    &m_nTubeLayersHitInner,    "nTubeLayersHitInner/I");
    m_tree->Branch("nTubeLayersHitMiddle",   &m_nTubeLayersHitMiddle,   "nTubeLayersHitMiddle/I");
    m_tree->Branch("nTubeLayersHitOuter",    &m_nTubeLayersHitOuter,    "nTubeLayersHitOuter/I");
    m_tree->Branch("nTubeLayersHitOther",    &m_nTubeLayersHitOther,    "nTubeLayersHitOther/I");
    m_tree->Branch("DriftSignsInnerLayer",   &m_driftSignsInnerLayer,   "driftSignsInnerLayer/D");
    m_tree->Branch("DriftSignsMiddleLayer",  &m_driftSignsMiddleLayer,  "driftSignsMiddleLayer/D");
    m_tree->Branch("DriftSignsOuterLayer",   &m_driftSignsOuterLayer,   "driftSignsOuterLayer/D");
    m_tree->Branch("DriftSignsOtherLayer",   &m_driftSignsOtherLayer,   "driftSignsOtherLayer/D");
    m_tree->Branch("trackNDoF",              &m_trackNDoF,              "trackNDoF/I");
    m_tree->Branch("trackChi2",              &m_trackChi2,              "trackChi2/D");
    m_tree->Branch("trackPt",                &m_trackPt,                "trackPt/D");
    m_tree->Branch("trackEta",               &m_trackEta,               "trackEta/D");
    m_tree->Branch("qOverP",                 &m_qOverP,                 "qOverP/D");
    m_tree->Branch("BEEPosX",                &m_beePosX,                "BEEPosX/D");
    m_tree->Branch("BEEPosY",                &m_beePosY,                "BEEPosY/D");
    //m_tree->Branch("trackSagitta",           &m_trackSagitta,           "trackSagitta/D");
    m_tree->Branch("nChambers",              &m_nChambers,              "nChambers/I");
    m_tree->Branch("chamberId",               m_chamberId,              "chamberId[nChambers]/I");
    m_tree->Branch("resIndex",                m_resIndex,               "resIndex[nChambers]/I");
    m_tree->Branch("nInnerLayerOutliers",    &m_nInnerLayerOutliers,    "nInnerLayerOutliers/I");
    m_tree->Branch("nMiddleLayerOutliers",   &m_nMiddleLayerOutliers,   "nMiddleLayerOutliers/I");
    m_tree->Branch("nOuterLayerOutliers",    &m_nOuterLayerOutliers,    "nOuterLayerOutliers/I");
    m_tree->Branch("nInnerLayerHoles",       &m_nInnerLayerHoles,       "nInnerLayerHoles/I");
    m_tree->Branch("nMiddleLayerHoles",      &m_nMiddleLayerHoles,      "nMiddleLayerHoles/I");
    m_tree->Branch("nOuterLayerHoles",       &m_nOuterLayerHoles,       "nOuterLayerHoles/I");
        
    m_tree->Branch("nIDres",                 &m_nIDres,                "nIDres/I");
    m_tree->Branch("IDres",                   m_IDres,                 "IDres[nIDres]/D");
    m_tree->Branch("IDerr",                   m_IDerr,                 "IDerr[nIDres]/D");
    
    m_tree->Branch("nIDscatRes",             &m_nIDscatRes,            "nIDscatRes/I");
    m_tree->Branch("IDscatPhiRes",            m_IDscatPhiRes,          "IDscatPhiRes[nIDscatRes]/D");
    m_tree->Branch("IDscatPhiErr",            m_IDscatPhiErr,          "IDscatPhiErr[nIDscatRes]/D");
    m_tree->Branch("IDscatThetaRes",          m_IDscatThetaRes,        "IDscatThetaRes[nIDscatRes]/D");
    m_tree->Branch("IDscatThetaErr",          m_IDscatThetaErr,        "IDscatThetaErr[nIDscatRes]/D");
    
    m_tree->Branch("nMSres",                 &m_nMSres,                "nMSres/I");
    m_tree->Branch("MSres",                   m_MSres,                 "MSres[nMSres]/D");
    m_tree->Branch("MSerr",                   m_MSerr,                 "MSerr[nMSres]/D");
    m_tree->Branch("MSstatLayer",             m_MSstatLayer,           "MSstatLayer[nMSres]/I");
    m_tree->Branch("MSisEndcap",              m_MSisEndcap,            "MSisendcap[nMSres]/I");

    m_tree->Branch("nMStrigRes",             &m_nMStrigRes,            "nMStrigRes/I");
    m_tree->Branch("MStrigRes",               m_MStrigRes,             "MStrigRes[nMStrigRes]/D");
    m_tree->Branch("MStrigErr",               m_MStrigErr,             "MStrigErr[nMStrigRes]/D");
    m_tree->Branch("MStrigStatLayer",         m_MStrigStatLayer,       "MStrigStatLayer[nMStrigRes]/I");
    m_tree->Branch("MStrigIsEndcap",          m_MStrigIsEndcap,        "MStrigIsendcap[nMStrigRes]/I");

    m_tree->Branch("nMSscatRes",             &m_nMSscatRes,            "nMSscatRes/I");
    m_tree->Branch("MSscatPhiRes",            m_MSscatPhiRes,          "MSscatPhiRes[nMSscatRes]/D");
    m_tree->Branch("MSscatPhiErr",            m_MSscatPhiErr,          "MSscatPhiErr[nMSscatRes]/D");
    m_tree->Branch("MSscatThetaRes",          m_MSscatThetaRes,        "MSscatThetaRes[nMSscatRes]/D");
    m_tree->Branch("MSscatThetaErr",          m_MSscatThetaErr,        "MSscatThetaErr[nMSscatRes]/D");

    m_tree->Branch("nCalRes",                &m_nCalRes,               "nCalRes/I");
    m_tree->Branch("CalRes",                  m_CalRes,                "CalRes[nCalRes]/D");
    m_tree->Branch("CalErr",                  m_CalErr,                "CalErr[nCalRes]/D");
    
    return;
  }

  //________________________________________________________________________
  int MuonTrackPreProcessor::getMdtStatLayer(Identifier id) const 
  {
    
    const std::string stName=
      m_muonIdHelperTool->mdtIdHelper().stationNameString(m_muonIdHelperTool->mdtIdHelper().stationName(id));
    
    // station layer
    switch ( stName[1] ) {
    case 'I' :
      return 0;
    case 'M' :
      return 1;
    case 'O' :
      return 2;
    default:
      return 3;
    }    
    return 3;
  }

  //________________________________________________________________________
  void MuonTrackPreProcessor::dumpChamberNames(const Trk::Track& track) const
  {
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=track.trackStateOnSurfaces()->begin(); 
    for (; iTsos!=track.trackStateOnSurfaces()->end(); ++iTsos) {

      const Trk::MeasurementBase*      mesb = (*iTsos)->measurementOnTrack();      
      const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
      if (!rio && crio) rio=&crio->rioOnTrack(0);
      if (!rio) continue;
      
      Identifier detId=rio->detectorElement()->identify();
      if (!m_muonIdHelperTool->isMuon(detId)) continue;
      
      const MuonIdHelper& muonIdHelper=dynamic_cast<const MuonIdHelper&>(m_muonIdHelperTool->mdtIdHelper());
      ATH_MSG_DEBUG("have muon hit in "
		    <<muonIdHelper.stationNameString(muonIdHelper.stationName(detId))
		    <<" "<<muonIdHelper.stationPhi(detId)
		    <<" "<<muonIdHelper.stationEta(detId));
    }
          
    
    return;
  }
  
  //________________________________________________________________________
  void MuonTrackPreProcessor::printCompetingROTs(const Trk::Track& track) const
  {
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=track.trackStateOnSurfaces()->begin(); 
    for (; iTsos!=track.trackStateOnSurfaces()->end(); ++iTsos) {
      const Trk::MeasurementBase*      mesb = (*iTsos)->measurementOnTrack();      
      const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
      if (!crio) continue;

      if (!rio) rio=&crio->rioOnTrack(0);
      if (!rio) continue;      
      Identifier detId=rio->detectorElement()->identify();
      //Identifier rioId=rio->identify();
      if (!m_muonIdHelperTool->isMuon(detId))  continue;
      
      //get detector type
      Trk::AlignModule::DetectorType detType=Trk::AlignModule::unidentified;
      if      (m_muonIdHelperTool->isTgc(detId)) detType=Trk::AlignModule::TGC;
      else if (m_muonIdHelperTool->isRpc(detId)) detType=Trk::AlignModule::RPC;
      else ATH_MSG_DEBUG("not TGC or RPC... what is it?");

      ATH_MSG_DEBUG(detTypeStr(detType)<<" global position: "<<mesb->globalPosition());      
    }
  }

  //_______________________________________________________________________
  bool MuonTrackPreProcessor::isEndcap(Identifier id) const
  {
    if (m_muonIdHelperTool->isEndcap(id)) return true;        
    if (m_muonIdHelperTool->mdtIdHelper().stationNameString(m_muonIdHelperTool->mdtIdHelper().stationName(id)).substr(0,3)=="BEE") return true;
    return false;
  }

  //_______________________________________________________________________
  bool MuonTrackPreProcessor::isBIS8(Identifier id) const
  {
    if (m_muonIdHelperTool->mdtIdHelper().stationNameString(m_muonIdHelperTool->mdtIdHelper().stationName(id)).substr(0,3)=="BIS" && std::abs(m_muonIdHelperTool->mdtIdHelper().stationEta(id))==8) return true;
    return false;
  }

  /*
  //________________________________________________________________________
  double MuonTrackPreProcessor::trackSagitta(const Trk::Track& track) const
  {

    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if ( !states ) return -999.;
    
    // loop over TSOSs, find MDT hits, and mark them as outliers:
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();

    // loop over hits, find out how many middle chambers
    int nmiddlechambers(0);
    int middleStatPhi(0);
    Identifier middleChamberId(0);
    for ( ; tsit!=tsit_end ; ++tsit ) {
      
      if (!(*tsit)->type(Trk::TrackStateOnSurface::Measurement)) continue;
      
      const Trk::MeasurementBase* mesb = (*tsit)->measurementOnTrack();      
      const Trk::RIO_OnTrack*     rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      if (!rio) continue;
      
      Identifier detId=rio->detectorElement()->identify();
      if (!m_muonIdHelperTool->isMuon(detId) || 
	  !m_muonIdHelperTool->isMdt(detId))  continue;
      
      ATH_MSG_DEBUG("detId: "<<detId<<", middle chamberId="<<middleChamberId);
      ATH_MSG_DEBUG("ch: "<<m_muonIdHelperTool->mdtIdHelper().stationNameString(m_muonIdHelperTool->mdtIdHelper().stationName(detId)));
      
      if (detId!=middleChamberId && 
	  m_muonAlignHelperTool->getMdtStatLayer(detId)==Middle) {
	if (m_muonIdHelperTool->mdtIdHelper().stationPhi(detId)!=middleStatPhi) {
	  nmiddlechambers++;
	  ATH_MSG_DEBUG("incremented nmiddlechambers: "<<nmiddlechambers);
	}
	if (nmiddlechambers>1) break;
	middleStatPhi=m_muonIdHelperTool->mdtIdHelper().stationPhi(detId);
	middleChamberId=detId;
      }    
    }
    ATH_MSG_DEBUG("nmiddlechambers: "<<nmiddlechambers);
    if (nmiddlechambers!=1) {
      return -999.;
    }
    
    ATH_MSG_DEBUG("middleStatPhi: "<<middleStatPhi);

    // get directions
    Trk::GlobalPosition innerGP,middleGP,outerGP;
    int                 nInner(0),nMiddle(0),nOuter(0);
    HepGeom::Vector3D<double>*        wireDir(0);

    for ( tsit=states->begin(); tsit!=tsit_end ; ++tsit ) {

      if (!(*tsit)->type(Trk::TrackStateOnSurface::Measurement)) continue;
      
      const Trk::MeasurementBase* mesb = (*tsit)->measurementOnTrack();      
      const Trk::RIO_OnTrack*     rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      if (!rio) continue;
      
      Identifier detId=rio->detectorElement()->identify();
      if (!m_muonIdHelperTool->isMuon(detId) || !m_muonIdHelperTool->isMdt(detId)) continue;
      
      StationLayer statlayer=m_muonAlignHelperTool->getMdtStatLayer(detId);
      ATH_MSG_DEBUG("statlayer: "<<statlayer);

      if (statlayer==Inner) {
	ATH_MSG_DEBUG("inner phi: "<<m_muonIdHelperTool->mdtIdHelper().stationPhi(detId));
	if (m_muonIdHelperTool->mdtIdHelper().stationPhi(detId)!=middleStatPhi) continue;
	innerGP+=(**tsit).trackParameters()->position();
	nInner++;
      }
      else if (statlayer==Middle) {
	middleGP+=(**tsit).trackParameters()->position();
	nMiddle++;
	if (!wireDir) {
	  const Trk::StraightLineSurface* wireSurf=dynamic_cast<const Trk::StraightLineSurface*>(&rio->associatedSurface());
	  if (!wireSurf) continue;
	  wireDir = new Trk::GlobalDirection(wireSurf->transform().getRotation().colZ());
	}
      }
      else if (statlayer==Outer) {
	ATH_MSG_DEBUG("outer phi: "<<m_muonIdHelperTool->mdtIdHelper().stationPhi(detId));
	if (m_muonIdHelperTool->mdtIdHelper().stationPhi(detId)!=middleStatPhi) continue;
	outerGP+=(**tsit).trackParameters()->position();
	nOuter++;
      }
    }
    if (nInner==0||nMiddle==0||nOuter==0) {
      delete wireDir;
      return -999.;
    }
    ATH_MSG_DEBUG("good track");
    
    outerGP  /= (double)nOuter;
    middleGP /= (double)nMiddle;
    innerGP  /= (double)nInner;

    if (!wireDir) {
      ATH_MSG_WARNING("no wire direction!  what's wrong?");
      return -999.;
    }

    HepGeom::Vector3D<double> vecEOEI(outerGP-innerGP);
    HepGeom::Vector3D<double> vecEMEI(middleGP-innerGP);
    HepGeom::Vector3D<double> u=vecEOEI.cross(*wireDir).unit();
    
    ATH_MSG_DEBUG("vecEOEI: "<<vecEOEI);
    ATH_MSG_DEBUG("vecEMEI: "<<vecEMEI);
    ATH_MSG_DEBUG("u: "<<u);
    double sagitta=vecEMEI.dot(u);
    ATH_MSG_DEBUG("sagitta: "<<sagitta);

    delete wireDir;

    return sagitta;
  }
  */

  //________________________________________________________________________
  void MuonTrackPreProcessor::calculateTrackChi2(const Trk::Track* /*track*/)
  {
    ATH_MSG_ERROR("migrate calculateTrackChi2 for Eigen");
    return;

    /*
    ATH_MSG_DEBUG("calculateTrackChi2");

    double previousQOverP(0.),qOverP(0.);
    m_nIDres=m_nIDscatRes=m_nMSres=m_nMStrigRes=m_nMSscatRes=m_nCalRes=0;

    bool afterCal=false;

    double trackChi2(0.);

    int itsos(0);
    for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator itTsos=
	   track->trackStateOnSurfaces()->begin();
	 itTsos!=track->trackStateOnSurfaces()->end(); ++itTsos,itsos++) {
      
      // scattering residual(s) and/or energy loss first
      ATH_MSG_DEBUG("\ntsos: "<<itsos);
      
      const Trk::TrackStateOnSurface*    tsos       = *itTsos;
      const Trk::TrackParameters *       tparp      = tsos->trackParameters();

      const Trk::MaterialEffectsBase*    meb        = tsos->materialEffectsOnTrack();
      const Trk::MaterialEffectsOnTrack* meot       = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(meb);
      const Trk::ScatteringAngles*       scatterer  = (meot) ? meot->scatteringAngles() : 0;
      const Trk::EnergyLoss*             energyLoss = (meot) ? meot->energyLoss() : 0;
      const Trk::MeasurementBase*        mesb       = dynamic_cast<const Trk::MeasurementBase*>(tsos->measurementOnTrack());      
      const Trk::RIO_OnTrack*            rio        = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack*   crio       = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
      if (!rio && crio)                  rio        = &crio->rioOnTrack(0);

      Trk::TrackState::MeasurementType measType = m_measTypeIdHelper->defineType(mesb);
      
      if (msgLvl(MSG::DEBUG)) {
	if      (measType==Trk::TrackState::unidentified) ATH_MSG_DEBUG("measType: unidentified");
	else if (measType==Trk::TrackState::Pixel) ATH_MSG_DEBUG("measType: Pixel");
	else if (measType==Trk::TrackState::SCT)   ATH_MSG_DEBUG("measType: SCT");
	else if (measType==Trk::TrackState::TRT)   ATH_MSG_DEBUG("measType: TRT");
	else if (measType==Trk::TrackState::MDT)   ATH_MSG_DEBUG("measType: MDT");
	else if (measType==Trk::TrackState::CSC)   ATH_MSG_DEBUG("measType: CSC");
	else if (measType==Trk::TrackState::RPC)   ATH_MSG_DEBUG("measType: RPC");
	else if (measType==Trk::TrackState::TGC)   ATH_MSG_DEBUG("measType: TGC");
	else if (measType==Trk::TrackState::Pseudo)  ATH_MSG_DEBUG("measType: Pseudo");
	else if (measType==Trk::TrackState::Vertex)  ATH_MSG_DEBUG("measType: Vertex");
	else if (measType==Trk::TrackState::Segment) ATH_MSG_DEBUG("measType: Segment");
	else if (measType==Trk::TrackState::SpacePoint)   ATH_MSG_DEBUG("measType: SpacePoint");
	else if (measType==Trk::TrackState::LArCal)  ATH_MSG_DEBUG("measType: LArCal");
	else if (measType==Trk::TrackState::TileCal) ATH_MSG_DEBUG("measType: TileCal");
	else ATH_MSG_DEBUG("other");
      }
      
      // determine which volume 
      bool isID =m_indetVolume->inside(tparp->position());
      bool isCal=!isID && m_calVolume->inside(tparp->position());
      bool isMS =!isID && !isCal;

      if (msgLvl(MSG::DEBUG)) {
	ATH_MSG_DEBUG("position: "<<tparp->position());
	if      (isMS)  ATH_MSG_DEBUG("isMS");
	else if (isCal) ATH_MSG_DEBUG("isCal");
	else if (isID)  ATH_MSG_DEBUG("isID");
	else            ATH_MSG_DEBUG("not MS, Cal, or ID!!!");
      }

      // scattering 
      int nscatparam=0;
      if (meb && meot) {
	ATH_MSG_DEBUG("has meb and meot");
	nscatparam = (scatterer) ? 2 : 1;      
      }
      else ATH_MSG_DEBUG("doesn't have meb and meot");

      ATH_MSG_DEBUG("MTPP tsos: "<<tsos<<", meb: "<<meb<<", meot: "<<meot);
      ATH_MSG_DEBUG("nscatparam: "<<nscatparam);

      // for energy loss 
      if (meb&&meot) {
	qOverP=tparp->parameters()[Trk::qOverP];
	ATH_MSG_DEBUG("qOverP: "<<qOverP<<", dyncast: "<<dynamic_cast<const CaloEnergy*>(energyLoss)<<", nscatparam: "<<nscatparam);
	if (!dynamic_cast<const CaloEnergy*>(energyLoss) &&
	    nscatparam!=1) {
	  previousQOverP     = qOverP;
	  ATH_MSG_DEBUG("previousQOverP: "<<previousQOverP);
	}
	else ATH_MSG_DEBUG("have calo energy loss");
      }


      int&    nScatRes     =isID||(isCal&&!afterCal) ? m_nIDscatRes      : m_nMSscatRes;
      double* scatPhiRes   =isID||(isCal&&!afterCal) ? m_IDscatPhiRes    : m_MSscatPhiRes;
      double* scatPhiErr   =isID||(isCal&&!afterCal) ? m_IDscatPhiErr    : m_MSscatPhiErr;
      double* scatThetaRes =isID||(isCal&&!afterCal) ? m_IDscatThetaRes  : m_MSscatThetaRes;
      double* scatThetaErr =isID||(isCal&&!afterCal) ? m_IDscatThetaErr  : m_MSscatThetaErr;
      int nMAX=100;

      for (int iparam=0;iparam<nscatparam;iparam++) {
	
	double errSq=0.;
	double residual(-999.);
	
	if (nscatparam==2) {

	  ATH_MSG_DEBUG("  scatparam: "<<iparam);

	  if (tparp) {
	    if (iparam==0) {
	      residual=-scatterer->deltaPhi()*tparp->sinTheta();
	      errSq   = scatterer->sigmaDeltaTheta();
	      errSq  *=errSq;

	      ATH_MSG_DEBUG("    sigmaDeltaTheta="<<scatterer->sigmaDeltaTheta());
	      ATH_MSG_DEBUG("    sigmaDeltaPhi  ="<<scatterer->sigmaDeltaPhi());
	      ATH_MSG_DEBUG("    residual:"<<residual<<", errSq: "<<errSq<<", err="<<std::sqrt(errSq));
	      if (nScatRes<nMAX) {		
		scatPhiRes[nScatRes]=residual;
		scatPhiErr[nScatRes]=std::sqrt(errSq);
		ATH_MSG_DEBUG("    scatPhiRes["<<nScatRes<<"]="<<scatPhiRes[nScatRes]
			      <<", scatPhiErr["<<nScatRes<<"]="<<scatPhiErr[nScatRes]);
		trackChi2+=residual*residual/errSq;
	      }
	      else 
		ATH_MSG_WARNING("too many residuals("<<nScatRes<<"), raise array size!");
	      
	    }
	    else{
	      residual=-scatterer->deltaTheta();
	      errSq   = scatterer->sigmaDeltaTheta();
	      errSq  *= errSq;

	      ATH_MSG_DEBUG("    residual:"<<residual<<", errSq: "<<errSq<<", err="<<std::sqrt(errSq));
	      if (nScatRes<nMAX) {		
		scatThetaRes[nScatRes]=residual;
		scatThetaErr[nScatRes]=std::sqrt(errSq);
		ATH_MSG_DEBUG("    scatThetaRes["<<nScatRes<<"]="<<scatThetaRes[nScatRes]
			      <<", scatThetaErr["<<nScatRes<<"]="<<scatThetaErr[nScatRes]);
		trackChi2+=residual*residual/errSq;
	      }
	      else 
		ATH_MSG_WARNING("too many residuals, raise array size!");
	    }
	    if (iparam==1) nScatRes++;
	  }
	  else{ 
	    ATH_MSG_WARNING("scatterer has no TrackParameters!");	
	  }	  
	}
	else if (!rio && !crio) {

	  ATH_MSG_DEBUG("  energy deposit");
	  
	  // energy deposit						
	  double mass      = 105.7;
	  double E0        = std::sqrt(1./previousQOverP/previousQOverP+mass*mass);
	  double E1        = std::sqrt(1./qOverP/qOverP + mass*mass);
	  double energyLoss= std::fabs(meot->energyLoss()->deltaE());
	  
	  double residual = .001*(E0-E1-energyLoss);
	  double err      = .001*meot->energyLoss()->sigmaDeltaE();
	  
	  ATH_MSG_DEBUG("    E0/E1/energyLoss: "<<E0<<"/"<<E1<<"/"<<energyLoss);
	  ATH_MSG_DEBUG("    calorimeter residual: "<<residual/err);

	  if (m_nCalRes<10) {		
	    m_CalRes[m_nCalRes]=residual;
	    m_CalErr[m_nCalRes]=err;
	    ATH_MSG_DEBUG("    calRes["<<m_nCalRes<<"]="<<m_CalRes[m_nCalRes]
			  <<", calErr["<<m_nCalRes<<"]="<<m_CalErr[m_nCalRes]);
	    m_nCalRes++;
	    trackChi2+=residual*residual/err/err;
	  }
	  else 
	    ATH_MSG_WARNING("   too many residuals, raise array size!");
	}
      }
      
      
      // residuals from measurement
      if (rio) {
	ATH_MSG_DEBUG("  measurement");


	int&    nMeasRes=isID||(isCal&&!afterCal) ? m_nIDres : m_nMSres;
	double* measRes =isID||(isCal&&!afterCal) ? m_IDres  : m_MSres;
	double* measErr =isID||(isCal&&!afterCal) ? m_IDerr  : m_MSerr;

	int nMeasMAX=100;

	int isEC=-1;
	int stationLayer=-1;

	if (isMS) {
	  Identifier detId=rio->detectorElement()->identify();
	  stationLayer=getMdtStatLayer(detId);
	  isEC=isEndcap(detId);
	  ATH_MSG_DEBUG("stationLayer: "<<stationLayer);
	  ATH_MSG_DEBUG("isEndcap: "<<isEC);	  
	}
	
	int nparams = (measType==Trk::TrackState::Pixel) ? 2 : 1;
	for (int iparam=0;iparam<nparams;iparam++) {
	  
	  ATH_MSG_DEBUG("    iparam: "<<iparam);
	  double errSq=0.;
	  double residual(-999.);
	  
	  if ( measType!=Trk::TrackState::unidentified &&
	       (rio!=0 || crio!=0) ) {
	    
	    const Trk::ResidualPull * resPull = 0;
	    
	    if ( tparp ) {
	      	      
	      ATH_MSG_DEBUG("      Calling Trk::ResidualPullCalculator for residual type "
			    <<Trk::ResidualPull::HitOnly<<")");
	      ATH_MSG_DEBUG("      mesb->localErrorMatrix().covValue(Trk::loc1): "<<mesb->localErrorMatrix().covValue(Trk::loc1));
	      resPull = m_pullCalculator->residualPull(mesb, tparp,
						       Trk::ResidualPull::HitOnly,
						       measType);
	      if ( resPull ) {
		residual = (resPull->residual())[iparam];
		double pull=(resPull->pull())[iparam];
		if (pull!=0.) {
		  errSq  = residual/pull;
		}
		errSq*=errSq;
		
		// MDT residual 
		bool isTrigger=(m_idHelper->is_tgc(rio->identify()) ||
				m_idHelper->is_rpc(rio->identify()));

		if (!isTrigger || isID) {
		  if (nMeasRes<nMeasMAX) {
		    measRes[nMeasRes]=residual;
		    measErr[nMeasRes]=std::sqrt(errSq);
		    ATH_MSG_DEBUG("      measRes["<<nMeasRes<<"]="<<measRes[nMeasRes]
				  <<", measErr["<<nMeasRes<<"]="<<measErr[nMeasRes]);
		    if (isMS) {
		      m_MSstatLayer[nMeasRes]=stationLayer;
		      m_MSisEndcap [nMeasRes]=isEC;
		    }
		    nMeasRes++;

		    trackChi2+=residual*residual/errSq;
		  }
		  else 
		    ATH_MSG_WARNING("     too many residuals, raise array size!");
		}

		// trigger chamber residual
		else {
		  if (m_nMStrigRes<50) {
		    m_MStrigRes[m_nMStrigRes]=residual;
		    m_MStrigErr[m_nMStrigRes]=std::sqrt(errSq);
		    ATH_MSG_DEBUG("      MStrigRes["<<m_nMStrigRes<<"]="<<m_MStrigRes[m_nMStrigRes]
				  <<", MStrigErr["<<m_nMStrigRes<<"]="<<m_MStrigErr[m_nMStrigRes]);
		    m_MStrigStatLayer[m_nMStrigRes]=stationLayer;
		    m_MStrigIsEndcap [m_nMStrigRes]=isEC;
		    
		    trackChi2+=residual*residual/errSq;

		    m_nMStrigRes++;
		  }
		  else 
		    ATH_MSG_WARNING("     too many trigger residuals, raise array size!");		  
		}
		
		ATH_MSG_DEBUG("      residual="<<residual<<", pull="<<pull);
		ATH_MSG_DEBUG("      pos: ("<<mesb->globalPosition().x()<<", "
			      <<mesb->globalPosition().y()<<", "
			      <<mesb->globalPosition().z()<<")");
		ATH_MSG_DEBUG("      residual:"<<residual<<", errSq: "<<errSq<<", err="<<std::sqrt(errSq));
	      }
	    }
	    delete resPull;
	  }
	}
      }

      if (isID) afterCal=true;
    }

    ATH_MSG_DEBUG("nMSscatRes: "<<m_nMSscatRes<<", nIDscatRes: "<<m_nIDscatRes);

    ATH_MSG_DEBUG("MTPP track chi2: "<<trackChi2<<", good track chi2: "<<m_trackChi2*(double)m_trackNDoF);
    return;
    */
  }

} // end Muon namespace
