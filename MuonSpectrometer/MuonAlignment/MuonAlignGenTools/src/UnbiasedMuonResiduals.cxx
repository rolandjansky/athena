/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************************
   UnbiasedMuonResiduals.cxx

   Author:      Dominique Fortin - TRIUMF
   Robert Harrington - BU
   created:     May 2010
   Description: Implementation code for the validation of the alignment of the MDT
	        
*********************************************************************/

#include "MuonAlignGenTools/UnbiasedMuonResiduals.h"
#include "MuonAlignGenTools/MuonAlignHelperTool.h"
#include "MuonAlignGenTools/MuonAlignRefitTool.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrigDecisionInterface/ITrigDecisionTool.h"

#include "muonEvent/MuonContainer.h"

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"

#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "MuonLinearSegmentMakerUtilities/Fit2D.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/CscDetectorElement.h"


// Muon Helpers
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

// Muon Segment + Quality
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"

// Building track from segments
#include "MuonRecToolInterfaces/IMuonSegmentTrackBuilder.h"

// Track info
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkExInterfaces/IExtrapolator.h"
//#include "TrkMagFieldInterfaces/IMagneticFieldTool.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/MagneticFieldProperties.h"
//#include "TrkParameters/MeasuredPerigee.h"

#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/MeasuredTrackParameters.h"

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/JacobianPhiThetaLocalAngles.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/MeasuredAtaPlane.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"

#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

#include "TFile.h"
#include "TTree.h"

#include <set>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <functional>

#include <map>

#ifndef REDO_CSC_ROTS
#define REDO_CSC_ROTS 0
#endif

using namespace Rec;
using namespace Trk;

namespace Muon {

  /*
  //________________________________________________________________________
  static std::string point2String(const HepGeom::Point3D<double>& pt)
  {
    std::ostringstream oss;
    oss << "eta=" << pt.eta() << ",phi=" << pt.phi() << ",perp=" << pt.perp() << ",z=" << pt.z();
    return oss.str();
  }
  
  //________________________________________________________________________
  static std::string dir2String(const HepGeom::Vector3D<double>& dir)
  {
    std::ostringstream oss;
    oss << "eta=" << dir.eta() << ",phi=" << dir.phi()<< ",z=" << dir.z();
    return oss.str();
  }
  */

  //________________________________________________________________________
  UnbiasedMuonResiduals::UnbiasedMuonResiduals( const std::string & name, 
						ISvcLocator* pSvcLocator)
    : AthAlgorithm(name,pSvcLocator)
    , m_detMgr(0)
    , m_storeGate(0)
    , m_tagTool("")
    , p_muonEDMHelperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool")
    , p_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
    , p_trackBuilder("Muon::MuonSegmentTrackBuilder/MuonSegmentTrackBuilder")
      //, p_trackRefitTool("Muon::MuonRefitTool/MuonRefitTool")
    , p_IExtrapolator("Trk::Extrapolator/AtlasExtrapolator")
    , m_combinedTrackBuilder("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder")
    , m_trackFitter("Trk::GlobalChi2Fitter/MCTBFitter")
    , m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
    , m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator")
    , m_mdtRotCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator")
    , m_cscRotCreator("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator")
    , m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
    , m_file(0) 
    , m_tree(0)
    , m_MAXNMDTHITS(80)
    , m_MAXNTGCHITS(80)
    , m_MAXNCSCHITS(20)
    , m_MAXNRPCHITS(80)
    , m_cscHitLocX(new double[m_MAXNCSCHITS])
    , m_cscHitLocY(new double[m_MAXNCSCHITS])
    , m_cscHitErrors(new double[m_MAXNCSCHITS])
    , m_mdtRes(new double[m_MAXNMDTHITS])
    , m_tgcRes(new double[m_MAXNTGCHITS])
    , m_cscRes(new double[m_MAXNCSCHITS])
    , m_rpcRes(new double[m_MAXNRPCHITS])
    , m_mdtPull(new double[m_MAXNMDTHITS])
    , m_tgcPull(new double[m_MAXNTGCHITS])
    , m_cscPull(new double[m_MAXNCSCHITS])
    , m_rpcPull(new double[m_MAXNRPCHITS])
    , m_dR(0.)
    , m_dZlocal(0.)
  {

    /** switches to control the analysis through job options */
    declareProperty("TagTool",                 m_tagTool);
    declareProperty("ResidualPullCalculator",  m_pullCalculator);
    declareProperty("Extrapolator",            p_IExtrapolator);

    declareProperty("WhichSegmCollections",    m_segmCollectionFlag);
    declareProperty("SegmentCollectionName",   m_segmCollectionName);
    declareProperty("MuonTrackCollections",    m_trackCollectionName);  
    declareProperty("InDetTrackCollections",   m_inDetTrackCollectionName);

    // development for TGC residuals not ready yet
    //declareProperty("MuonTgcPrdKey",           m_muonTgcPrdKey="TGC_Measurements");
    //declareProperty("MuonTgcPrdKeyPrev",       m_muonTgcPrdKey_prev="TGC_MeasurementsPriorBC");
    //declareProperty("MuonTgcPrdKeyNext",       m_muonTgcPrdKey_next="TGC_MeasurementsNextBC");


    declareProperty("Fitter",                  m_trackFitter);
    declareProperty("CombinedTrackBuilder",    m_combinedTrackBuilder);
    declareProperty("TrackFittingType",        m_trackFittingType=2);

    declareProperty("UseTrackContainer",       m_useTrackContainer=true);

    declareProperty("ExtrapolateIDTrack",      m_extrapolateIDTrack=false);

    /** Requirements on combined muon track */
    declareProperty("maxIDTrackd0",        m_MaxIDd0           =  50.0);
    declareProperty("maxIDTrackz0",        m_MaxIDz0           = 200.0);
    declareProperty("maxIDTrackChidof",    m_MaxIDChiDof       = 5.0 ); 
    declareProperty("minMuonMom",          m_MinMuonMom        = 5.0 ); // minimum ms and id track momentum 
    declareProperty("trackMatchDeltaR",    m_trackMatchDeltaR  = 0.5);
    declareProperty("matchChi2Cut",        m_matchChi2Cut      = 25.);

    declareProperty("minPIXHits",          m_minPIXHits        = 2);
    declareProperty("minSCTHits",          m_minSCTHits        = 4);
    declareProperty("minTRTHits",          m_minTRTHits        = 0);
    declareProperty("minCSCHits",          m_minCSCHits        = 0);
    declareProperty("minMDTHits",          m_minMDTHits        = 3);

    declareProperty("TriggerList",         m_triggerList);

    declareProperty("doMdt",               m_doMdt=true);
    declareProperty("doCsc",               m_doCsc=true);
    declareProperty("doTgc",               m_doTgc=true);
    declareProperty("doRpc",               m_doRpc=true);

    declareProperty("MakeTgcOutliers",     m_makeTGCOutliers=true);
    declareProperty("RedoErrorScaling",    m_redoErrorScaling=true);

    m_nPixHits=m_nSCTHits=m_nTRTHits=m_nCSCHits=m_nMDTHits=m_nRPCHits=m_nTGCHits=0;
    m_nInnerMDTHits=m_nMiddleMDTHits=m_nOuterMDTHits=m_nTriggerPhiHits=0;

    m_author=m_hIndex=m_phiSector=m_isEndcap=m_identifier=m_isCSide=
      m_detType=-999;
    
    m_momentum=m_charge=m_d0=m_z0=m_phi0=m_theta0=m_eta0=-999.;
    m_trackChi2=m_calEnergyLoss=m_calEnergyLossErr=-999.;
    m_materialOnTrack=0.;
    
    m_deltaPhi=m_deltaTheta=m_dR=m_dXlocal=m_dZlocal=m_dYlocal=
      m_dXlocalAngle=m_dYlocalAngle=-999.;
    
    m_layer=m_stName=m_stPhi=m_stEta=-999;
    m_pullXlocal=m_pullYlocal=m_pullXlocalAngle=m_pullYlocalAngle=-999.;

    m_run=m_evt=0;

    m_dXglobal=m_dYglobal=m_dZglobal=m_dXglobalAngle=m_dYglobalAngle=m_dZglobalAngle=-999.;
    m_detTheta=m_detThetaErr=-999.;
  }

  //________________________________________________________________________
  UnbiasedMuonResiduals::~UnbiasedMuonResiduals() 
  {

    delete [] m_cscHitLocX;
    delete [] m_cscHitLocY;
    delete [] m_cscHitErrors;

    delete [] m_mdtRes;
    delete [] m_tgcRes;
    delete [] m_cscRes;
    delete [] m_rpcRes;   

    delete [] m_mdtPull;
    delete [] m_tgcPull;
    delete [] m_cscPull;
    delete [] m_rpcPull;

  }

  //________________________________________________________________________
  StatusCode UnbiasedMuonResiduals::initialize() 
  {
    
    /** retrieve MuonTrackTagTool */
    if (m_tagTool.name()!=""){
      if (m_tagTool.retrieve().isFailure()) {
	ATH_MSG_FATAL("Failed to retrieve tool " << m_tagTool);
	return StatusCode::FAILURE;
      }
      else {
	ATH_MSG_INFO("Retrieved tool " << m_tagTool);
      }       
    }

    // check if TagTool exists
    if (m_trackFittingType == MSStandaloneWithIDTrackMatch) {      
      if (m_tagTool.empty()) {
	ATH_MSG_ERROR("need TagTool to match MS track to ID track!");
	return StatusCode::FAILURE;
      }	
    }
    
    /** retrieve MuonDetectorManager */
    StatusCode sc = detStore()->retrieve(m_detMgr,"Muon");
    if(sc.isFailure()) {
      ATH_MSG_ERROR("Could not find the MuonDetectorManager! ");
      return StatusCode::FAILURE;
    }
  
    /** Helper tool */
    sc = p_muonEDMHelperTool.retrieve();
    if (sc.isFailure()){
      ATH_MSG_FATAL("Fatal to retrieve - Could not get " << p_muonEDMHelperTool); 
      return sc;
    } else {
      ATH_MSG_DEBUG("Retrieved tool" << p_muonEDMHelperTool);
    } 

    /** Id helpers tools */
    sc = p_idHelperTool.retrieve();
    if (sc.isFailure()){
      ATH_MSG_ERROR("Failed to retrieve tool   " << p_idHelperTool);
      return sc;
    } else {
      ATH_MSG_DEBUG("Retrieved tool   " << p_idHelperTool);
    }    

    /** Track builder from a pair of segments */
    sc = p_trackBuilder.retrieve();

    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not get " << p_trackBuilder);
      return sc;
    } else {
      ATH_MSG_INFO("Retrieved " << p_trackBuilder);
    }

    /** Track refit tool */
    /*
      sc = p_trackRefitTool.retrieve();
      if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not get " << p_trackRefitTool);
      return sc;
      } else {
      ATH_MSG_INFO("Retrieved " << p_trackRefitTool);
      }
    */

    sc = m_trackFitter.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not find refit tool "<<m_trackFitter<<". Exiting.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Refit tool "<<m_trackFitter);
    }
  
    sc = m_combinedTrackBuilder.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not find refit tool "<<m_combinedTrackBuilder<<". Exiting.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Refit tool "<<m_combinedTrackBuilder);
    }
    
    /** Track extrapolator */
    sc = p_IExtrapolator.retrieve();
    if (sc.isFailure()){
      ATH_MSG_ERROR("Could not get " << p_IExtrapolator);
      return sc;
    } else {
      ATH_MSG_INFO("Retrieved " << p_IExtrapolator);
    }
   
    /** Trigger decision tool */
    sc = m_trigDec.retrieve();
    if (sc.isFailure()){
      ATH_MSG_ERROR("Could not get " << m_trigDec);
      return sc;
    } else {
      ATH_MSG_INFO("Retrieved " << m_trigDec);
    }

    /** muonAlignHelperTool */
    if (m_muonAlignHelperTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved "<<m_muonAlignHelperTool);
    else {
      ATH_MSG_FATAL("Could not get "<<m_muonAlignHelperTool);
      return StatusCode::FAILURE;
    }

    /** muonAlignRefitTool */
    if (m_muonAlignRefitTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved "<<m_muonAlignRefitTool);
    else {
      ATH_MSG_FATAL("Could not get "<<m_muonAlignRefitTool);
      return StatusCode::FAILURE;
    }

    /** residual pull calculator */
    if (m_pullCalculator.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_pullCalculator);
    else{
      ATH_MSG_FATAL("Could not get " << m_pullCalculator);
      return StatusCode::FAILURE;
    }

    /** MdtDriftCircleOnTrackCreator*/
    if (m_mdtRotCreator.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_mdtRotCreator);
    else{
      ATH_MSG_FATAL("Could not get " << m_mdtRotCreator);
      return StatusCode::FAILURE;
    }
    
    /** CscClusterOnTrackCreator*/
    if (m_cscRotCreator.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_cscRotCreator);
    else{
      ATH_MSG_FATAL("Could not get " << m_cscRotCreator);
      return StatusCode::FAILURE;
    }

    /** MuonEDMPrinterTool */
    if (m_printer.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved "<<m_printer);
    else {
      ATH_MSG_WARNING("Could not get "<<m_printer);
      return StatusCode::FAILURE;
    }

    m_file=new TFile("UnbiasedMuonResiduals.root","RECREATE");
    m_tree=new TTree("unbiasedMuonResiduals","UnbiasedMuonResiduals");

    m_tree->Branch("run",          &m_run,          "run/I");
    m_tree->Branch("evt",          &m_evt,          "evt/I");

    m_tree->Branch("nPixHits",     &m_nPixHits,     "nPixHits/I");
    m_tree->Branch("nSCTHits",     &m_nSCTHits,     "nSCTHits/I");
    m_tree->Branch("nTRTHits",     &m_nTRTHits,     "nTRTHits/I");
    m_tree->Branch("nCSCHits",     &m_nCSCHits,     "nCSCHits/I");
    m_tree->Branch("nMDTHits",     &m_nMDTHits,     "nMDTHits/I");
    m_tree->Branch("nTGCHits",     &m_nTGCHits,     "nTGCHits/I");
    m_tree->Branch("nRPCHits",     &m_nRPCHits,     "nRPCHits/I");

    m_tree->Branch("nInnerMDTHits",  &m_nInnerMDTHits,  "nInnerMDTHits/I");
    m_tree->Branch("nMiddleMDTHits", &m_nMiddleMDTHits, "nMiddleMDTHits/I");
    m_tree->Branch("nOuterMDTHits",  &m_nOuterMDTHits,  "nOuterMDTHits/I");
    m_tree->Branch("nTriggerPhiHits",&m_nTriggerPhiHits,"nTriggerPhiHits/I");

    m_tree->Branch("cscHitLocX",    m_cscHitLocX,   "cscHitLocX[nCSCHits]/D");
    m_tree->Branch("cscHitLocY",    m_cscHitLocY,   "cscHitLocY[nCSCHits]/D");
    m_tree->Branch("cscHitErrors",  m_cscHitErrors, "cscHitErrors[nCSCHits]/D");
    
    m_tree->Branch("mdtRes",        m_mdtRes,       "mdtRes[nMDTHits]/D");
    m_tree->Branch("tgcRes",        m_tgcRes,       "tgcRes[nTGCHits]/D");
    m_tree->Branch("cscRes",        m_cscRes,       "cscRes[nCSCHits]/D");
    m_tree->Branch("rpcRes",        m_rpcRes,       "rpcRes[nRPCHits]/D");

    m_tree->Branch("mdtPull",       m_mdtPull,      "mdtPull[nMDTHits]/D");
    m_tree->Branch("tgcPull",       m_tgcPull,      "tgcPull[nTGCHits]/D");
    m_tree->Branch("cscPull",       m_cscPull,      "cscPull[nCSCHits]/D");
    m_tree->Branch("rpcPull",       m_rpcPull,      "rpcPull[nRPCHits]/D");

    m_tree->Branch("author",       &m_author,       "author/I");

    m_tree->Branch("hIndex",       &m_hIndex,       "hIndex/I");
    m_tree->Branch("phiSector",    &m_phiSector,    "phiSector/I");
    m_tree->Branch("isEndcap",     &m_isEndcap,     "isEndcap/I");
    m_tree->Branch("identifier",   &m_identifier,   "identifier/I");
    m_tree->Branch("isCSide",      &m_isCSide,      "isCSide/I");
    m_tree->Branch("detType",      &m_detType,      "detType/I");

    m_tree->Branch("layer",        &m_layer,        "layer/I");
    m_tree->Branch("stName",       &m_stName,       "stName/I");
    m_tree->Branch("stPhi",        &m_stPhi,        "stPhi/I");
    m_tree->Branch("stEta",        &m_stEta,        "stEta/I");
  
    m_tree->Branch("momentum",     &m_momentum,     "momentum/D");
    m_tree->Branch("charge",       &m_charge,       "charge/D");
    m_tree->Branch("d0",           &m_d0,           "d0/D");
    m_tree->Branch("z0",           &m_z0,           "z0/D");
    m_tree->Branch("phi0",         &m_phi0,         "phi0/D");
    m_tree->Branch("theta0",       &m_theta0,       "theta0/D");
    m_tree->Branch("eta0",         &m_eta0,         "eta0/D");

    m_tree->Branch("trackChi2",    &m_trackChi2,    "trackChi2/D");

    m_tree->Branch("calEnergyLoss",    &m_calEnergyLoss,    "calEnergyLoss/D");
    m_tree->Branch("calEnergyLossErr", &m_calEnergyLossErr, "calEnergyLossErr/D");
    m_tree->Branch("materialOnTrack",  &m_materialOnTrack,  "materialOnTrack/D");

    m_tree->Branch("deltaPhi",     &m_deltaPhi,     "deltaPhi/D");
    m_tree->Branch("deltaTheta",   &m_deltaTheta,   "deltaTheta/D");

    m_tree->Branch("dR",           &m_dR,           "dR/D");
    m_tree->Branch("dZlocal",      &m_dZlocal,      "dZlocal/D");
    m_tree->Branch("dXlocal",      &m_dXlocal,      "dXlocal/D");
    m_tree->Branch("dYlocal",      &m_dYlocal,      "dYlocal/D");
    m_tree->Branch("dXlocalAngle", &m_dXlocalAngle, "dXlocalAngle/D");
    m_tree->Branch("dYlocalAngle", &m_dYlocalAngle, "dYlocalAngle/D");
    m_tree->Branch("pullXlocal",   &m_pullXlocal,   "pullXlocal/D");
    m_tree->Branch("pullYlocal",   &m_pullYlocal,   "pullYlocal/D");
    m_tree->Branch("pullXlocalAngle", &m_pullXlocalAngle, "pullXlocalAngle/D");
    m_tree->Branch("pullYlocalAngle", &m_pullYlocalAngle, "pullYlocalAngle/D");
    m_tree->Branch("dXglobal",     &m_dXglobal,     "dXglobal/D");
    m_tree->Branch("dYglobal",     &m_dYglobal,     "dYglobal/D");
    m_tree->Branch("dZglobal",     &m_dZglobal,     "dZglobal/D");
  
    m_tree->Branch("detTheta",     &m_detTheta,     "detTheta/D");
    m_tree->Branch("detThetaErr",  &m_detThetaErr,  "detThetaErr/D");

    return sc;
  }

  //________________________________________________________________________
  StatusCode UnbiasedMuonResiduals::execute() 
  {
    ATH_MSG_ERROR("UnbiasedMuonResiduals::execute()");
    return StatusCode::FAILURE;


    /*
    StatusCode sc=StatusCode::SUCCESS;
 
    m_hIndex = -1;

    // get run and event numbers
    ATH_MSG_DEBUG("Retrieving event info.");
    const EventInfo* eventInfo;
    if (evtStore()->retrieve(eventInfo).isFailure()) {
      ATH_MSG_ERROR("Could not retrieve event info.");
    }
    m_run=eventInfo->event_ID()->run_number();
    m_evt=eventInfo->event_ID()->event_number();
    
    TrackCollection* trackColl=new TrackCollection;
  
    for ( unsigned j=0; j<m_segmCollectionName.size(); ++j ) {
    
      // check if using this collection from steering file
      if ( m_segmCollectionFlag[j] == 0 ) continue; 
        
      ++m_hIndex;
 
      ATH_MSG_DEBUG("hIndex: "<<m_hIndex);
 
      // get the segments matched to muons
      for (int isubdet=0;isubdet<4;isubdet++) {

	ATH_MSG_DEBUG("isubdet: "<<isubdet);

	// detector type
	Trk::AlignModule::DetectorType detType=Trk::AlignModule::unidentified;
	switch (isubdet) {
	case (0): detType=Trk::AlignModule::MDT; break;
	case (1): detType=Trk::AlignModule::CSC; break;
	case (2): detType=Trk::AlignModule::RPC; break;
	case (3): detType=Trk::AlignModule::TGC; break;
	}
	
	if (detType==Trk::AlignModule::MDT && !m_doMdt) continue;
	if (detType==Trk::AlignModule::CSC && !m_doCsc) continue;
	if (detType==Trk::AlignModule::RPC && !m_doRpc) continue;
	if (detType==Trk::AlignModule::TGC && !m_doTgc) continue; // TGC not ready yet

	m_detType=isubdet;
      
	ATH_MSG_DEBUG("");
	if (isubdet==0)      ATH_MSG_DEBUG("getting MDT segments");
	else if (isubdet==1) ATH_MSG_DEBUG("getting CSC segments");
	else if (isubdet==2) ATH_MSG_DEBUG("getting RPC segments");
	else                 ATH_MSG_DEBUG("getting TGC segments");

	// get segments from either the track container or the Analysis::Muon

	int iflag=1; // 0 for inner MDT layer or CSC, 1 for any MDT layer or CSC

	std::vector<SegMatch>      muonInn;
	std::vector<SegMatchTrack> muonInnTrack;      
	if (!m_useTrackContainer) {
	  sc = getSegments(detType,muonInn,iflag);
	  if ( muonInn.size() < 1 || sc.isFailure() ) continue;  
	}
	else {
	  sc = getSegments(detType,muonInnTrack,iflag);
	  ATH_MSG_DEBUG("muonInnTrack size: "<<muonInnTrack.size());
	  if ( muonInnTrack.size() < 1 || sc.isFailure()) continue;  
	}

	// First loop over segment collection to find match with combined track
	int nSegments = (muonInn.size()>0) ? muonInn.size() : muonInnTrack.size();
	ATH_MSG_DEBUG(nSegments<<" segment(s) selected");

	// loop over segments
	for (int i = 0; i < nSegments; ++i ) {
	
	  // get segment
	  const Muon::MuonSegment* mSeg = 
	    muonInn.size()>0 ? muonInn[i].first : muonInnTrack[i].first;
	
	  // check if mSeg has a RIO_OnTrack
	  const Trk::RIO_OnTrack* mSegRio=mSeg->rioOnTrack(0);
	  if (!mSegRio) continue;

	  // get detector surface to which tracks and segments will be extrapolated
	  const Trk::PlaneSurface* detSurface=getDetectorSurface(detType,mSeg);
	  if (!detSurface) continue;
	  
	  // extrapolate segment to detector surface
	  const Trk::TrackParameters* segPar=getSegmentExtrapolated(mSeg,detSurface);
	  if ( !segPar ) continue;

	  // get Analysis::Muon or Trk::Track
	  const Analysis::Muon* muon  = muonInn.size()>0      ? muonInn[i].second :      0;
	  const Trk::Track*     track = muonInnTrack.size()>0 ? muonInnTrack[i].second : 0;

	  if (muon && track) {
	    ATH_MSG_ERROR("have muon and track!");	  
	    delete trackColl;
	    return StatusCode::FAILURE;
	  }
	  if (!muon && !track) {
	    ATH_MSG_ERROR("have neither muon nor track!");
	    delete trackColl;
	    return StatusCode::FAILURE;
	  }

	  if (muon && !checkMuon(*muon)) continue;

	  m_trackChi2=
	    track->fitQuality()->chiSquared()/track->fitQuality()->doubleNumberDoF();
	  ATH_MSG_DEBUG("track chi2/ndof: "<<m_trackChi2);
	  
	  Identifier id = chamberId( detType, *mSeg );
	  if ( !id.is_valid() ) {
	    delete trackColl;
	    return StatusCode::FAILURE; 
	  }
	  ATH_MSG_DEBUG("mSeg id: "<<id);

	  // get track from muon or initial track
	  const Trk::Track* initTrack = muon ?
	    getTrackFromMuon(muon) : getTrackFromTrack(track);
	  if (!initTrack) {
	    ATH_MSG_DEBUG("failed to get initial track");
	    continue;
	  }

	  if (!initTrack->perigeeParameters()) 
	    ATH_MSG_DEBUG("before cleanedTrack, track has no perigee");

	  bool removePerigee=m_trackFittingType==MSStandaloneWithIDMomentum;
	  Trk::Track* cleanedTrack=cleanTrack(id,*initTrack, removePerigee);
	  if (!cleanedTrack) {
	    ATH_MSG_DEBUG("failed to clean track");
	    continue;
	  }
	  delete initTrack;
	  
	  // check track
	  if (!checkTrack(*cleanedTrack)) {
	    ATH_MSG_DEBUG("failed checkTrack");
	    continue;
	  }

	  // get biased residuals from initial track
	  getBiasedResiduals(*cleanedTrack);

	  // unbias initial track by changing hits in detector to outliers       
	  Trk::Track* unbiasedTrack = 
	    getUnbiasedTrack(id,cleanedTrack,detSurface,detType,iflag);
	  delete cleanedTrack;

	  if (!unbiasedTrack) {
	    ATH_MSG_DEBUG("failed to get unbiased track");
	    continue;
	  }

	  // extrapolate unbiased track to detector surface
	  const TrackParameters* trackPar = getTrackExtrapolated(id,unbiasedTrack,
								 detSurface);
	  if (!trackPar) {
	    ATH_MSG_DEBUG("muon track extrapolation failed");
	    delete unbiasedTrack;
	    continue;
	  }
	  
	  // calculate residuals
	  const Trk::MeasuredAtaPlane* trkMeasuredAtaPlane=dynamic_cast<const Trk::MeasuredAtaPlane*>(trackPar);
	  if (!trkMeasuredAtaPlane) {
	    ATH_MSG_WARNING("no trkMeasuredAtaPlane!");
	    delete unbiasedTrack;
	    continue;
	  }
	  const ErrorMatrix& trkErrMat=trkMeasuredAtaPlane->localErrorMatrix();

	  const Trk::MeasuredAtaPlane* segMeasuredAtaPlane=dynamic_cast<const Trk::MeasuredAtaPlane*>(segPar);
	  if (!segMeasuredAtaPlane) {
	    ATH_MSG_WARNING("no segMeasuredAtaPlane!");
	    delete unbiasedTrack;
	    continue;
	  }
	  
	  const ErrorMatrix& segErrMat=segMeasuredAtaPlane->localErrorMatrix();
	  calcResForSegmentAtDetSurface(*trackPar,*segPar,trkErrMat,segErrMat);

	  // set some variables used in output tree
	  setIdentificationVariables(detType,*mSeg);
	  
	  // calculate residuals for extrapolated ID track to muon detector surface
	  if (m_extrapolateIDTrack) calcResForExtrapolatedIDToDetSurface(*muon,*segPar,
									 detSurface,detType);
	  
	  m_file->cd();
	  m_tree->Fill();
	  
	  ATH_MSG_DEBUG("hIndex/detType/momentum/d0/phi0/ID: "<<m_hIndex<<"/"<<m_detType<<"/"<<m_momentum<<"/"<<m_d0<<"/"<<m_phi0<<"/"<<m_identifier);
	  
	  delete segPar;
	  delete trackPar;
	  delete detSurface;
	  
	  //if (sc!=StatusCode::SUCCESS) return StatusCode::FAILURE;

	  // record unbiased track in StoreGate
	  trackColl->push_back(unbiasedTrack);	  
	} // segments

	muonInn.clear();
	if (muonInnTrack.size()>0) 
	  for (int i=0;i<(int)muonInnTrack.size();i++) delete muonInnTrack[i].second;
	muonInnTrack.clear();
	
      } // detType                  
    } // end loop on collection

    if (StatusCode::SUCCESS!=evtStore()->record(trackColl,"unbiasedTracks")) {
      ATH_MSG_WARNING("problem with recording tracks to StoreGate!");
    }

    if (sc.isFailure()) return StatusCode::SUCCESS;

    return sc; 
    */
  }

  //________________________________________________________________________
  StatusCode UnbiasedMuonResiduals::finalize() 
  {

    m_file->cd();
    m_tree->Write();
    m_file->Close();

    StatusCode sc = StatusCode::SUCCESS;

    return sc;
  }

  //________________________________________________________________________
  StatusCode
  UnbiasedMuonResiduals::getSegments( Trk::AlignModule::DetectorType detType,
				      std::vector<SegMatch>& muonInn, int iflag)
  {
    ATH_MSG_DEBUG("in getSegments, detType: "<<detTypeStr(detType));

    std::string segmCollectionName=m_segmCollectionName[m_hIndex];
    std::string trackCollectionName=m_trackCollectionName[m_hIndex];

    /** get a pointer to the segments */
    const Trk::SegmentCollection* segmentCollection = 0;
    StatusCode sc = evtStore()->retrieve(segmentCollection, segmCollectionName); 
    if ( sc.isFailure() ) return sc;

    Trk::SegmentCollection::const_iterator s;
    int nSegments = segmentCollection->end() - segmentCollection->begin();
    if ( nSegments < 1 ) return sc;
    ATH_MSG_DEBUG("found "<<nSegments<<" segments, getting muon container");

    // get the container of the original muons 
    const Analysis::MuonContainer* muonTES=0;
    sc = evtStore()->retrieve( muonTES, trackCollectionName);
    if ( sc.isFailure() || muonTES == 0 || muonTES->size() < 1 ) return sc;
    ATH_MSG_DEBUG("found "<<muonTES->size()<<" muons, looping through segments");
  
    // Sort segment by station
    for ( s = segmentCollection->begin(); s != segmentCollection->end(); ++s) {
      Muon::MuonSegment* mSeg               = dynamic_cast<Muon::MuonSegment*> (*s);

      if ( !mSeg ) {
         ATH_MSG_WARNING("muon segments cannot be retrieved");
	 continue;
      }

      Identifier id1                        = chamberId( detType, *mSeg );
    
      if ( !id1.is_valid() ) continue;  
      if (!p_idHelperTool->isMuon(id1)) continue;

      if (p_idHelperTool->isMdt(id1)) ATH_MSG_DEBUG("isMdt: "<<p_idHelperTool->isMdt(id1));
      if (p_idHelperTool->isCsc(id1)) ATH_MSG_DEBUG("isCsc: "<<p_idHelperTool->isCsc(id1));
      if (p_idHelperTool->isCsc(id1)) ATH_MSG_DEBUG("isRpc: "<<p_idHelperTool->isRpc(id1));
      if (p_idHelperTool->isCsc(id1)) ATH_MSG_DEBUG("isTgc: "<<p_idHelperTool->isTgc(id1));
      
      if ( (detType==Trk::AlignModule::MDT && p_idHelperTool->isMdt(id1)) ||
           (detType==Trk::AlignModule::CSC && p_idHelperTool->isCsc(id1)) ||
           (detType==Trk::AlignModule::RPC && p_idHelperTool->isRpc(id1)) ||
           (detType==Trk::AlignModule::TGC && p_idHelperTool->isTgc(id1)) ) {
      
        if (chamberIds(detType,*mSeg).size()>1) {
          continue;
        }
      
        if (!passesChamberSelection(detType,id1,iflag)) continue;

        
        if (muonTES) {
          const Analysis::Muon* goodMuon=findMuon(muonTES,mSeg);
          if (goodMuon) {
            muonInn.push_back( std::make_pair(mSeg,goodMuon) );
            continue;
          }
        } 
      }
    }

    return sc;
  }

  //________________________________________________________________________
  StatusCode
  UnbiasedMuonResiduals::getSegments( Trk::AlignModule::DetectorType /*detType*/,
				      std::vector<SegMatchTrack>& /*muonInn*/, int /*iflag*/)
  {

    ATH_MSG_ERROR("migrate getSegments to Eigen");
    return StatusCode::FAILURE;
    
    /*
    // get vector<MuonSegment*,Track*> for all pairings of segments and tracks where 
    // segment has at least one chamber with the right detector type

    ATH_MSG_DEBUG("in getSegments for detType: "<<detTypeStr(detType));
    
    if (detType!=Trk::AlignModule::TGC) {
      
      std::string segmCollectionName      =m_segmCollectionName [m_hIndex];
      std::string trackCollectionName     =m_trackCollectionName[m_hIndex];
      std::string inDetTrackCollectionName="";
      if (m_inDetTrackCollectionName.size()>0)
	inDetTrackCollectionName=m_inDetTrackCollectionName[m_hIndex];
      
      // get a pointer to the segments 
      const Trk::SegmentCollection* segmentCollection = 0;
      StatusCode sc = evtStore()->retrieve(segmentCollection, segmCollectionName); 
      ATH_MSG_DEBUG("found "<<segmentCollection->size()<<" segments");
      if ( sc.isFailure() || segmentCollection->size()<1 ) return StatusCode::FAILURE;
      
      Trk::SegmentCollection::const_iterator s;
      int nSegments = segmentCollection->end() - segmentCollection->begin();
      if ( nSegments < 1 ) {
	ATH_MSG_DEBUG("found no segments!");
	return StatusCode::FAILURE;
      }
      
      // use tracks from track container
      const TrackCollection* tracks=0;
      const TrackCollection* inDetTracks=0;
      sc = evtStore()->retrieve(tracks,trackCollectionName);
      if (sc.isFailure() || tracks==0 || tracks->size()<1) {
	ATH_MSG_DEBUG("found no tracks");
	return StatusCode::FAILURE;    
      }

      if (inDetTrackCollectionName!="") {
	sc = evtStore()->retrieve(inDetTracks,inDetTrackCollectionName);
	if (sc.isFailure() || inDetTracks==0 || inDetTracks->size()<1) {
	  ATH_MSG_DEBUG("found no indet tracks");
	  return StatusCode::FAILURE;
	}
	//ATH_MSG_DEBUG("found "<<inDetTracks->size()<<" inner detector tracks");            
      }
      ATH_MSG_DEBUG("found "<<tracks->size()<<" tracks, looping through segments");            
      
      // Sort segment by station
      std::list<Muon::MuonSegment*> goodSegments;
      for ( s = segmentCollection->begin(); s != segmentCollection->end(); ++s) {
	
	Muon::MuonSegment* mSeg = dynamic_cast<Muon::MuonSegment*> (*s);
	
	// get id(s)
	std::set<Identifier> ids=chamberIds(detType,*mSeg);      
	int nChambers=ids.size();
	if (nChambers>1 && 
	    (detType==Trk::AlignModule::MDT || detType==Trk::AlignModule::CSC) ) {
	  ATH_MSG_DEBUG("segment has too many chambers ("<<nChambers<<")");
	  continue;
	}
	
	// check chi2/ndof of segment
	double chi2perdof=
	  mSeg->fitQuality()->chiSquared()/mSeg->fitQuality()->doubleNumberDoF();
	ATH_MSG_DEBUG("seg chi2/ndof: "<<chi2perdof);
	if (chi2perdof>10.0) continue;
	
	// check if segment has the right detector among all IDs in this segment
	bool goodSeg=false;
	std::set<Identifier>::iterator idIt   =ids.begin();
	std::set<Identifier>::iterator idItEnd=ids.end();
	for ( ; idIt!=idItEnd; ++idIt) {
	  
	  Identifier id=*idIt;
	  if ( (detType==Trk::AlignModule::MDT && p_idHelperTool->isMdt(id)) ||
	       (detType==Trk::AlignModule::CSC && p_idHelperTool->isCsc(id)) ||
	       (detType==Trk::AlignModule::RPC && p_idHelperTool->isRpc(id)) ||
	       (detType==Trk::AlignModule::TGC && p_idHelperTool->isTgc(id)) ) {
	    
	    ATH_MSG_DEBUG("calling passesChamberSel with detType "<<detTypeStr(detType));
	    if (!passesChamberSelection(detType,id,iflag)) {
	      ATH_MSG_DEBUG("fails chamber selection");
	      continue;
	    }
	    else {
	      goodSeg=true;
	      break;
	    }
	  }
	}
	
	
	if (goodSeg) {
	  ATH_MSG_DEBUG("found good segment");
	  goodSegments.push_back(mSeg);
	}
	else continue;
	
	if (detType==Trk::AlignModule::MDT) ATH_MSG_DEBUG("good MDT segment");
	if (detType==Trk::AlignModule::CSC) ATH_MSG_DEBUG("good CSC segment");
	if (detType==Trk::AlignModule::RPC) ATH_MSG_DEBUG("good RPC segment");
	if (detType==Trk::AlignModule::TGC) ATH_MSG_DEBUG("good TGC segment");
	
      }
      
      if (goodSegments.size()<1) {
	ATH_MSG_DEBUG("found no good segments");
	return StatusCode::FAILURE;
      }
      
      ATH_MSG_DEBUG("have "<<goodSegments.size()<<" good segments");
      
      // remove extra MuonSegments if more than one in a chamber
      removeExtraSegments(detType,goodSegments);
      
      std::list<Muon::MuonSegment*>::iterator segIt=goodSegments.begin();
      std::list<Muon::MuonSegment*>::iterator segItEnd=goodSegments.end();
      for ( ; segIt != segItEnd; ++segIt ) {
	Muon::MuonSegment* mSeg = *segIt;
	
	ATH_MSG_DEBUG("looking for track for following segment: "<<*mSeg);
	
	if (tracks) {
	  
	  // find best track for this segment
	  const Trk::Track* bestTrack=findBestTrack(detType,tracks,inDetTracks,mSeg);
	  if (!bestTrack) continue;
	  
	  double qOverP=m_charge/(m_momentum*1000.);
	  ATH_MSG_DEBUG("before calling prepareTrack: ");
	  ATH_MSG_DEBUG(m_printer->print(*bestTrack)<<endmsg<<
			m_printer->printStations(*bestTrack)<<endmsg<<
			m_printer->printMeasurements(*bestTrack));

	  const Trk::Track* goodTrack=prepareTrack(*bestTrack,qOverP);

	  if (goodTrack) {
	    muonInn.push_back( std::make_pair(mSeg,goodTrack) );
	    continue;
	  }
	}
	else ATH_MSG_DEBUG("no tracks!");
      }      
    }
    else {

      //
      // TGC segments
      //
     
      // // get TGC PRDs from StoreGate
      // const Muon::TgcPrepDataContainer* tgc_container[3] = {0, 0, 0};
      // std::list<Identifier> vecIds;
      // for(int ibc=0; ibc<3; ibc++){
      // int bcTag=ibc+1;
      // std::ostringstream location;
      // location << m_muonTgcPrdKey  << (bcTag==TgcDigit::BC_PREVIOUS ? "PriorBC" : "")
      // << (bcTag==TgcDigit::BC_NEXT ? "NextBC" : "");
      // StatusCode sc;
      // sc = this->evtStore()->retrieve(tgc_container[ibc], location.str());
      // if (sc.isFailure()) {
      // return sc;
      // }
	
      // }
      
      // // loop over PRDs, get collection of PRDs for each chamber
      
      // int bunch=-1;
      // for(int ibc=0; ibc<3; ibc++){
      // for (Muon::TgcPrepDataContainer::const_iterator containerit = tgc_container[ibc]->begin() ;
      // containerit != tgc_container[ibc]->end() ; ++containerit) {                  
      // for (Muon::TgcPrepDataCollection::const_iterator tgcPrd = (*containerit)->begin() ;
      // tgcPrd != (*containerit)->end() ; ++tgcPrd) {
      // if((*tgcPrd)->identify() == id ) { bunch = ibc+1; continue; }
      // }
      // if(bunch!=-1) continue;
      // }
      // if(bunch!=-1) continue;
      // }      

      // ignore segments in segment container, we will create segments using TGC hits on the track

      // use tracks from track container
      std::string trackCollectionName     =m_trackCollectionName[m_hIndex];
      std::string inDetTrackCollectionName="";
      if (m_inDetTrackCollectionName.size()>0)
	inDetTrackCollectionName=m_inDetTrackCollectionName[m_hIndex];

      const TrackCollection* tracks=0;
      const TrackCollection* inDetTracks=0;
      StatusCode sc = evtStore()->retrieve(tracks,trackCollectionName);
      if (sc.isFailure() || tracks==0 || tracks->size()<1) 
	return StatusCode::FAILURE;    
      
      if (inDetTrackCollectionName!="") {
	sc = evtStore()->retrieve(inDetTracks,inDetTrackCollectionName);
	if (sc.isFailure() || inDetTracks==0 || inDetTracks->size()<1) 
	  return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("found "<<tracks->size()<<" tracks, getting first track");  

      const Trk::Track* bestTrack=findBestTrack(detType,tracks,inDetTracks,0);
      if (!bestTrack) return StatusCode::FAILURE;

      HepGeom::Vector3D<double> dir(bestTrack->perigeeParameters()->momentum());

      double qOverP=m_charge/(m_momentum*1000);
      const Trk::Track* goodTrack=prepareTrack(*bestTrack,qOverP);

      // have good track, now make TGC segment out of middle layer
      const DataVector<const Trk::TrackStateOnSurface>* tsosvec=goodTrack->trackStateOnSurfaces();
      
      // loop over TSOS and collect TGC hits
      Muon::Fit2D::PointArray rhoPoints,phiPoints;
      std::set<std::string> rhoStations, phiStations;
      const MuonGM::TgcReadoutElement* tgc=0;

      int itsos(0),iHit(0);
      DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=tsosvec->begin(); 
      for (; iTsos!=tsosvec->end(); ++iTsos,itsos++) {
	
	ATH_MSG_DEBUG("itsos: "<<itsos);

	const Trk::MeasurementBase*      mesb = (*iTsos)->measurementOnTrack();	
	const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=crio->rioOnTrack(0);
	if (!rio) continue;

	ATH_MSG_DEBUG("have rio");

	// check if TGC
	//const TgcPrepData* tgc_prp_data = dynamic_cast<const TgcPrepData*>(rio);	
	//if (!tgc_prp_data) continue;
	//ATH_MSG_DEBUG("have tgc_prp_data");
	if (!p_idHelperTool->isMuon(rio->identify()) || 
	    !p_idHelperTool->isTgc(rio->identify())) continue;
	ATH_MSG_DEBUG("have TGC RIO");
	
	// get station name, skip if inner layer
	std::string stName=
	  dynamic_cast<const MuonGM::TgcReadoutElement*>(rio->detectorElement())->getStationName();
	ATH_MSG_DEBUG("hit in TGC chamber: "<<stName);
	
	if (stName.substr(1,1)!="T") ATH_MSG_ERROR("not TGC chamber"); // sanity check
	if (stName.substr(2,1)=="1") continue;
	
	if (!tgc) tgc=dynamic_cast<const MuonGM::TgcReadoutElement*>(rio->detectorElement());
	
	// get weight from error matrix if it exists
	const Trk::ErrorMatrix& errMat = mesb->localErrorMatrix();
	if (!errMat.weightPointer()) {
	  ATH_MSG_DEBUG("RIO has a singular matrix, skipping");
	  continue;
	}
	double w = errMat.weightValue(Trk::locX);
	
	// create rho or phi hit	  
	if (p_idHelperTool->tgcIdHelper().isStrip(rio->identify())) {
	  double rho = mesb->globalPosition().perp();
	  w *= rho * rho;
	  Muon::Fit2D::Point* pPt = new Muon::Fit2D::Point(iHit,mesb->globalPosition().z(),
							   mesb->globalPosition().phi(),w,
							   (void*)mesb);
	  phiPoints.push_back(pPt);
	  phiStations.insert(stName);
	}
	else {
	  Muon::Fit2D::Point* pPt = new Muon::Fit2D::Point(iHit,mesb->globalPosition().z(),
							   mesb->globalPosition().perp(),w,
							   (void*)mesb);
	  rhoPoints.push_back(pPt);
	  rhoStations.insert(stName);
	}
	iHit++;
      }   
      
      if (rhoPoints.empty() && phiPoints.empty()) {
	ATH_MSG_DEBUG("no rho or phi hits");
	return StatusCode::FAILURE;
      }
      
      // loop over phi and rho hit collections, create segment
      double dChi2 = 0.0;
      int nDegf = 0;
      double zCov[2][2], phiCov[2][2];
      memset(zCov,   0, sizeof(zCov));
      memset(phiCov, 0, sizeof(phiCov));
      
      Muon::Fit2D fit;
      HepGeom::Point3D<double> pos;
      Muon::Fit2D::SimpleStats rhoSimpleStats, phiSimpleStats;
      Muon::Fit2D::LinStats rhoLinStats, phiLinStats;

      double outlierChi2Cut=10.;
      double baseZ = tgc->center().z();

      if (rhoStations.size() == 1 || rhoPoints.size() < 4) {
	fit.fitPoint(rhoPoints, outlierChi2Cut, msgLvl(MSG::DEBUG), rhoSimpleStats);
	fit.fitPoint(phiPoints, outlierChi2Cut, msgLvl(MSG::DEBUG), phiSimpleStats);

	ATH_MSG_DEBUG("Rho: " << rhoSimpleStats.toString());
	ATH_MSG_DEBUG("Phi: " << phiSimpleStats.toString());
	
	pos.set(rhoSimpleStats.fMean, 0.0, baseZ);
	pos.setPhi(phiSimpleStats.fMean);
	dChi2 = rhoSimpleStats.fChi2 + phiSimpleStats.fChi2;
	nDegf = rhoSimpleStats.n + phiSimpleStats.n - 2;
	zCov[0][0]=(rhoSimpleStats.fStd)*(rhoSimpleStats.fStd);
      }
      else {
	double rho, rhoErr, phi, phiErr, z = baseZ;
	fit.fitLine(rhoPoints, outlierChi2Cut, msgLvl(MSG::DEBUG), rhoLinStats);
	ATH_MSG_DEBUG("Rho: " << rhoLinStats.toString());
	rhoLinStats.eval(z, rho, rhoErr);
	pos.set(rho, 0.0, z);
	dChi2 = rhoLinStats.fChi2;
	nDegf = rhoLinStats.n - 2;
	
	if (phiStations.size() == 1 || phiPoints.size() < 4) {
	  fit.fitPoint(phiPoints, outlierChi2Cut, msgLvl(MSG::DEBUG), phiSimpleStats);
	  ATH_MSG_DEBUG("Phi: " << phiSimpleStats.toString());
	  pos.setPhi(phiSimpleStats.fMean);
	  phiLinStats.fSlope = 0.0;
	  dChi2 += phiSimpleStats.fChi2;
	  nDegf += phiSimpleStats.n - 1;
	  phiCov[0][0]=(phiSimpleStats.fStd)*(phiSimpleStats.fStd);
	}
	else {
	  fit.fitLine(phiPoints, outlierChi2Cut, msgLvl(MSG::DEBUG), phiLinStats);
	  ATH_MSG_DEBUG("Phi: " << phiLinStats.toString());
	  phiLinStats.eval(z, phi, phiErr);
	  pos.setPhi(phi);
	  dChi2 += phiLinStats.fChi2;
	  nDegf += phiLinStats.n - 2;
	  memcpy(phiCov, phiLinStats.fCov, sizeof(phiCov));
	}
	
	double z1 = pos.z() + copysign(1.0, pos.z());
	rhoLinStats.eval(z1, rho, rhoErr);
	HepGeom::Point3D<double> p1(rho, 0.0, z1);
	phiLinStats.eval(z1, phi, phiErr);
	p1.setPhi(phi);
	dir = p1 - pos;
      }
      ATH_MSG_DEBUG("Segment position " << point2String(pos));
      ATH_MSG_DEBUG("Segment direction " << dir2String(dir));
      
      
      const Trk::Surface& surface = tgc->surface();      
      ATH_MSG_DEBUG("Surface at " << point2String(surface.center()));
      
      const Trk::PlaneSurface* pSurface = dynamic_cast<const Trk::PlaneSurface*>(&surface);
      if (pSurface == NULL) {
	ATH_MSG_WARNING("Cannot get a PlaneSurface from TgcClusterOnTrack");
	for (int i=0;i<(int)rhoPoints.size();i++) delete rhoPoints[i];
	for (int i=0;i<(int)phiPoints.size();i++) delete phiPoints[i];
	return StatusCode::FAILURE;
      }
      const Trk::LocalPosition* pSegPos = pSurface->globalToLocal(pos);
      if (pSegPos == NULL) {
	ATH_MSG_WARNING("Cannot get LocalPosition from surface for " << point2String(pos));
	for (int i=0;i<(int)rhoPoints.size();i++) delete rhoPoints[i];
	for (int i=0;i<(int)phiPoints.size();i++) delete phiPoints[i];
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Segment local position is"
		    << " x=" << (*pSegPos)[Trk::locX]
		    << ",y=" << (*pSegPos)[Trk::locY]);
	
      const Trk::LocalDirection* pSegDir = pSurface->globalToLocalDirection(dir.unit());
      if (pSegDir == NULL) {
	ATH_MSG_WARNING("Cannot get LocalDirection from surface for " << dir2String(dir));
	for (int i=0;i<(int)rhoPoints.size();i++) delete rhoPoints[i];
	for (int i=0;i<(int)phiPoints.size();i++) delete phiPoints[i];
	return StatusCode::FAILURE;
      }
	
      ATH_MSG_DEBUG("Segment local direction is"
		    << " angleXZ=" << pSegDir->angleXZ()
		    << ",angleYZ=" << pSegDir->angleYZ());
	
      Trk::CovarianceMatrix* pcov = new Trk::CovarianceMatrix(4);
      (*pcov)[0][0] = zCov[0][0];
      (*pcov)[0][1] = zCov[0][1];
      (*pcov)[1][1] = zCov[1][1];
      (*pcov)[2][2] = phiCov[0][0];
      (*pcov)[2][3] = phiCov[0][1];
      (*pcov)[3][3] = phiCov[1][1];
      (*pcov)[0][2] = 0.0;
      (*pcov)[0][3] = 0.0;
      (*pcov)[1][2] = 0.0;
      (*pcov)[1][3] = 0.0;
	
      Trk::ErrorMatrix* perr = new Trk::ErrorMatrix(pcov);
      Trk::FitQuality* pFitQuality = new Trk::FitQuality(dChi2, nDegf);
      DataVector<const Trk::MeasurementBase>* pRios = new DataVector<const Trk::MeasurementBase>;
      for (Muon::Fit2D::PointArray::const_iterator itPt = rhoPoints.begin(); 
	   itPt != rhoPoints.end(); itPt++) {
	Muon::Fit2D::Point* pPt = *itPt;
	if (!pPt->bExclude) {
	  pRios->push_back(static_cast<const Trk::MeasurementBase*>(((const Muon::MuonClusterOnTrack*)(pPt->pData))->clone()));
	}
      }
      for (Muon::Fit2D::PointArray::const_iterator itPt = phiPoints.begin(); 
	   itPt != phiPoints.end(); itPt++) {
	Muon::Fit2D::Point* pPt = *itPt;
	if (!pPt->bExclude) {
	  pRios->push_back(static_cast<const Trk::MeasurementBase*>(((const Muon::MuonClusterOnTrack*)(pPt->pData))->clone()));
	}
      }
      Muon::MuonSegment* pMuonSeg = new Muon::MuonSegment(*pSegPos,
							  pSegDir,
							  perr,
							  const_cast<Trk::PlaneSurface*>(pSurface->clone()),
							  pRios,
							  pFitQuality);
      ATH_MSG_DEBUG("Created a new Muon::MuonSegment with surface: "<<*pSurface);
      delete pSegPos;	    
      
      for (int i=0;i<(int)rhoPoints.size();i++) delete rhoPoints[i];
      for (int i=0;i<(int)phiPoints.size();i++) delete phiPoints[i];

      muonInn.push_back(std::make_pair(pMuonSeg,goodTrack));
    }
    
    return StatusCode::SUCCESS;
    */

  }
  
  /*
  //________________________________________________________________________
  StatusCode
  UnbiasedMuonResiduals::getAllSegments( const std::string& segmCollectionName,
  const std::string& trackCollectionName,
  std::vector<SegMatch>& muonInn) 
  {
  
  // get a pointer to the segments 
  const Trk::SegmentCollection* segmentCollection = 0;
  StatusCode sc = evtStore()->retrieve(segmentCollection, segmCollectionName); 
  if ( sc.isFailure() ) return sc;

  Trk::SegmentCollection::const_iterator s;
  int nSegments = segmentCollection->end() - segmentCollection->begin();
  ATH_MSG_DEBUG("found "<<nSegments<<" segments");
  if ( nSegments < 1 ) return sc;

  // get the container of the original muons -
  const Analysis::MuonContainer* muonTES=0;
  sc = evtStore()->retrieve( muonTES, trackCollectionName);
  if ( sc.isFailure() || muonTES == 0 ) return sc;
  if ( muonTES->size() < 1 ) return sc;

  // Sort segment by station
  std::list<Muon::MuonSegment*> goodSegments;
  for ( s = segmentCollection->begin(); s != segmentCollection->end(); ++s ) {
  Muon::MuonSegment* mSeg               = dynamic_cast<Muon::MuonSegment*> (*s);
  Identifier id1                        = chamberId(detType,*mSeg );
  if ( !id1.is_valid() ) continue;  

  if (mSeg->fitQuality()->chiSquared()/mSeg->fitQuality()->doubleNumberDoF()>10.0) continue;

  if ( p_idHelperTool->isMuon( id1) && p_idHelperTool->isMdt( id1 ) ) {
  if (chamberIds(detType,*mSeg).size()>1) {
  ATH_MSG_DEBUG("more than 1 chamber") ;
  continue;
  }
	
  goodSegments.push_back(mSeg);
  }
  }

  // remove extra MuonSegment if more than one per chamber
  removeExtraSegments(goodSegments);
    
  std::list<Muon::MuonSegment*>::iterator segIt=goodSegments.begin();
  std::list<Muon::MuonSegment*>::iterator segItEnd=goodSegments.end();

  for (; segIt != segItEnd; ++segIt) {
      
  Muon::MuonSegment* mSeg=*segIt;
      
  Analysis::MuonContainer::const_iterator muonIt  = muonTES->begin();
  Analysis::MuonContainer::const_iterator muonItE = muonTES->end();
      
  // Loop over muon collection
  for ( ; muonIt!=muonItE; ++muonIt ) {
	
  if (muonIt == muonItE || !*muonIt) continue;
	
  // Only keep combined muons with ID and MS tracks, and best match !
  if ( !(*muonIt)->isCombinedMuon() ) continue;
	
  muonInn.push_back( std::make_pair(mSeg,*muonIt) );
  }
  }    
  
  return sc;
  }
  */

  //________________________________________________________________________
  const Trk::PlaneSurface* 
  UnbiasedMuonResiduals::getDetectorSurface(Trk::AlignModule::DetectorType /*detType*/,
					    const Muon::MuonSegment* /*mSeg*/)
  {
    ATH_MSG_ERROR("migrate getDetectorSurface to Eigen");
    return 0;

    /*
    ATH_MSG_DEBUG("in getDetectorSurface, detType: "<<detTypeStr(detType));
  
    // get detector surface
    const Trk::RIO_OnTrack* rio(0);
    const Trk::CompetingRIOsOnTrack* crio(0);
    std::vector<const Trk::MeasurementBase*> mbs = mSeg->containedMeasurements();
    const Trk::TrkDetElementBase* det(0);
    for( unsigned int imbs = 0; imbs< mbs.size(); ++imbs) {
      
      ATH_MSG_DEBUG ("meas["<<imbs<<"]: "<<*(mbs[imbs]));
      rio = dynamic_cast<const Trk::RIO_OnTrack*>(mbs[imbs]);
      
      if (rio) ATH_MSG_DEBUG("have rio");
      
      if (!rio) {
	crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[imbs]);
	if (crio) ATH_MSG_DEBUG("have crio");

	if (crio) rio  = &crio->rioOnTrack(0);
      }
      if (rio && detType==Trk::AlignModule::MDT) {
	det=rio->associatedSurface().associatedDetectorElement();
	if (dynamic_cast<const MuonGM::MdtReadoutElement*>(det)) {
	  ATH_MSG_DEBUG("have MDT");
	  ATH_MSG_DEBUG("MDT det surface: "<<det->surface());
	  break;    
	}
	else { det=0; rio=0; }
      }
      else if (rio && detType==Trk::AlignModule::CSC) {
	det=rio->associatedSurface().associatedDetectorElement();
	if (dynamic_cast<const MuonGM::CscReadoutElement*>(det)) {
	  ATH_MSG_VERBOSE("have CSC");
	  break;    
	}
	else { det=0; rio=0; }
      }
      else if (crio && rio && detType==Trk::AlignModule::RPC) {
	det= rio->associatedSurface().associatedDetectorElement();
	if (dynamic_cast<const MuonGM::RpcReadoutElement*>(det)) {
	  ATH_MSG_VERBOSE("have RPC");
	  break;
	}
	else { det=0; rio=0; }
      }

      else if (crio && rio && detType==Trk::AlignModule::TGC) {
	det=rio->associatedSurface().associatedDetectorElement();
	if (dynamic_cast<const MuonGM::TgcReadoutElement*>(det)) {
	  ATH_MSG_VERBOSE("have TGC");
	  break;
	}
	else { det=0; rio=0; }
      }
    }
    if (!rio || !det) {
      ATH_MSG_DEBUG("no RIO or detector surface in MuonSegment!");
      return 0;
    }

    // get detector surface and correct if segment surface has a different orientation
    const Trk::PlaneSurface& uncorrectedDetSurface=
      //dynamic_cast<const Trk::PlaneSurface&>( rio->associatedSurface().associatedDetectorElement()->surface() );
      dynamic_cast<const Trk::PlaneSurface&>( det->surface() );
    ATH_MSG_DEBUG("uncorrectedDetSurface: "<<uncorrectedDetSurface);

    const MuonGM::MuonReadoutElement* detEl=
      //dynamic_cast<const MuonGM::MuonReadoutElement*>(rio->associatedSurface().associatedDetectorElement());
      dynamic_cast<const MuonGM::MuonReadoutElement*>(det);
   
    
    // // get detector element for 1st multilayer
    // if (detType==Trk::AlignModule::MDT) {
    // int nMultilayers=detEl->nMDTinStation();
    // if (nMultilayers==2) {
    // const MdtIdHelper& mdtIdHelper=p_idHelperTool->mdtIdHelper(); 
    // Identifier id=rio->detectorElement()->identify();
    // int stName=mdtIdHelper.stationName(id);
    // int stEta =mdtIdHelper.stationEta(id);
    // int stPhi =mdtIdHelper.stationPhi(id);
    // detEl = m_detMgr->getMdtReadoutElement(mdtIdHelper.channelID(stName,stEta,stPhi,1,1,1));
    // }
    // }  
    

    // surface transform in AMDB frame
    const HepGeom::Transform3D amdbToGlobal = detEl->AmdbLRSToGlobalTransform();
    HepGeom::Vector3D<double> lx = amdbToGlobal*HepGeom::Vector3D<double>(1.,0.,0.);
    HepGeom::Vector3D<double> ly = amdbToGlobal*HepGeom::Vector3D<double>(0.,1.,0.);
    HepGeom::Vector3D<double> lz = amdbToGlobal*HepGeom::Vector3D<double>(0.,0.,1.);
    CLHEP::HepRotation surfaceRot(lx,ly,lz);
    HepGeom::Transform3D* surfaceTransform = new HepGeom::Transform3D(surfaceRot,uncorrectedDetSurface.center());

    if (msgLvl(MSG::DEBUG)) {
      double distance=uncorrectedDetSurface.center().distance(mSeg->associatedSurface().center());
      ATH_MSG_DEBUG("surface sep: "<<distance);
    }
      
    // create new surface
    double surfDim = 500.;
    Trk::PlaneSurface* detSurface=new Trk::PlaneSurface(surfaceTransform,surfDim,surfDim);
  
    return detSurface; 
    */
  }

  //________________________________________________________________________
  const Trk::Track*
  UnbiasedMuonResiduals::getTrackFromMuon(const Analysis::Muon* muon)
  {
    const Trk::Track* idTrack   =  muon->inDetTrkTrack();
    const Trk::Track* muTrack   =  muon->muonExtrapolatedTrkTrack();
    const Trk::Track* combTrack =  muon->combinedMuonTrkTrack();
    return getTrack(idTrack,muTrack,combTrack);
  }

  //________________________________________________________________________
  const Trk::Track*
  UnbiasedMuonResiduals::getTrackFromTrack(const Trk::Track* track)
  {
    const Trk::Track* idTrack   =  0;
    const Trk::Track* muTrack   =  track;
    const Trk::Track* combTrack =  track;
    return getTrack(idTrack,muTrack,combTrack);
  }

  //________________________________________________________________________
  const Trk::Track*
  UnbiasedMuonResiduals::getTrack(const Trk::Track* idTrack,
				  const Trk::Track* muTrack,
				  const Trk::Track* combTrack)
  {
    if (!muTrack) {
      ATH_MSG_ERROR("no muon track!");
      return 0;
    }
    
    const Trk::Track* initTrack(0);
    if (idTrack && muTrack && combTrack &&
	(m_trackFittingType==CombinedMuonTrackBuilder ||
	 m_trackFittingType==MSStandaloneFromCombinedMuon)) { 
      
      initTrack = m_combinedTrackBuilder->combinedFit(*idTrack,*muTrack,*muTrack);
      if (!initTrack) ATH_MSG_DEBUG("no initTrack");
      
      if (initTrack && m_trackFittingType==MSStandaloneFromCombinedMuon) { 
	const Trk::Track* standaloneTrack = m_combinedTrackBuilder->standaloneRefit(*initTrack);      
	delete initTrack;
	initTrack = standaloneTrack;
      }
    }
    else if (idTrack && m_trackFittingType==MSStandaloneWithIDMomentum) {

      ATH_MSG_ERROR("this doesn't work yet!");
      return 0;

      //double pT =idTrack->perigeeParameters()->pT();
      //double eta=idTrack->perigeeParameters()->eta();
      //double momentum=std::fabs(pT)*std::sqrt(1.+sinh(eta)*sinh(eta));
      
      //checkCovVar(*muTrack);
      
      //initTrack = m_trackFitter->fit(*muTrack,false,Trk::muon);
    }
    else if (combTrack && m_trackFittingType==NormalTrackFitter)
      initTrack = new Trk::Track(*combTrack);
    else if (m_trackFittingType==MSStandaloneWithIDTrackMatch) {
      initTrack=new Trk::Track(*muTrack);

      if (msgLvl(MSG::VERBOSE)) {
	ATH_MSG_VERBOSE("refitting track");
	Trk::Track* debugTrack=m_trackFitter->fit(*muTrack,false,Trk::muon);    
	delete debugTrack;
      }
    }
    else {
      ATH_MSG_ERROR("bad value for TrackFittingType or incorrect tracks provided!");
      return 0;
    }
    return initTrack;
  }
  
  //________________________________________________________________________
  Trk::Track*
  UnbiasedMuonResiduals::getUnbiasedTrack(Identifier id1,
					  const Trk::Track* initTrack,
					  const Trk::PlaneSurface* /*detSurface*/,
					  Trk::AlignModule::DetectorType detType,
					  int flag)
  {
    ATH_MSG_DEBUG("in getUnbiasedTrack");

    if (!initTrack->perigeeParameters()) 
      ATH_MSG_DEBUG("no perigeeParameters for initTrack");
    
    m_materialOnTrack=0.;
    const DataVector<const Trk::TrackStateOnSurface>* states = initTrack->trackStateOnSurfaces();
    if ( !states ) return 0;
    
    // loop over TSOSs, find MDT hits, and mark them as outliers:
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    
    //std::cout << "MDT: will loop on the TSOS" << std::endl;
    
    bool isBEE1=(p_idHelperTool->isMdt(id1) && p_idHelperTool->mdtIdHelper().stationNameString(p_idHelperTool->mdtIdHelper().stationName(id1)).substr(0,3)=="BEE");
    bool removeBarrel=(p_idHelperTool->isEndcap(id1)||isBEE1);

    // This is the list of new TSOS I'll use without the MDTs
    DataVector<const Trk::TrackStateOnSurface>* myNewTrackStateOnSurfaces = 
      new DataVector<const Trk::TrackStateOnSurface>();
    myNewTrackStateOnSurfaces->reserve( states->size() );
    

    int nGoodChambers=0;
    Identifier lastId=Identifier();
    int itsit(0);
    for ( ; tsit!=tsit_end ; ++tsit,itsit++ ) {

      if ((**tsit).type(Trk::TrackStateOnSurface::Perigee))
	ATH_MSG_DEBUG("in getUnbiasedTrack, have perigee: "<<**tsit);

      const Trk:: MaterialEffectsBase* materialEffects = 
	(*tsit)->materialEffectsOnTrack();
      if (materialEffects) m_materialOnTrack += materialEffects->thicknessInX0();
      
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if ( !meas ) {
	myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit) );
	continue;
      }
      
      const Trk::MeasurementBase*      mesb = (*tsit)->measurementOnTrack();      
      const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
      if (!rio && crio) rio=&crio->rioOnTrack(0);
      if (!rio) {
	myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit) );
	continue;
      }
      
      Identifier id = p_idHelperTool->chamberId(rio->detectorElement()->identify());

      // check if TGC hit
      if (m_makeTGCOutliers && p_idHelperTool->isTgc(id) &&
	  !p_idHelperTool->tgcIdHelper().isStrip(rio->prepRawData()->identify())) {
	ATH_MSG_DEBUG("setting TGC eta to outlier");
	myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit,true));
	continue;
      }
      
      // if barrel and doing endcap cap or vice versa, remove from track
      bool isBEE=(p_idHelperTool->isMdt(id) && p_idHelperTool->mdtIdHelper().stationNameString(p_idHelperTool->mdtIdHelper().stationName(id)).substr(0,3)=="BEE");
      bool isEndcap=p_idHelperTool->isEndcap(id)||isBEE;
      ATH_MSG_DEBUG("isEndcap: "<<isEndcap);
      if ( p_idHelperTool->isMuon(id) && isEndcap!=removeBarrel ) {
	ATH_MSG_DEBUG("have endcap and doing barrel (or vice versa)");
	continue;
      }

      // ID hits, so keep
      if (!p_idHelperTool->isMuon(id)) {
	myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit) );
	continue;
      }	

      // increment number MDT chambers if new MDT chamber
      if (id!=lastId && p_idHelperTool->isMdt(id)) {
	lastId=id;
	nGoodChambers++;
      }
      
      // clone TSOS if hits in other chamber
      if (id!=id1) {
	myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit) );
	continue;
      }	

      // see if in selected chamber
      if (!passesChamberSelection(detType,id,flag)) {
	bool makeOutlier=true;
	myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit,makeOutlier) );
	continue;
      }
      
      // I have found a good hit, so mark it as outlier so not used in the new track fit      
      bool makeOutlier=true;
      ATH_MSG_DEBUG("outlier for hit at position: "<<mesb->globalPosition());
      myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit,makeOutlier) );     
    }

    // check if at least 2 MDT chambers on track
    if (nGoodChambers<2) {
      ATH_MSG_DEBUG("good chambers: "<<nGoodChambers);
      return 0;
    }

    // Create a new track with hits on track and outliers:
    Trk::Track* newTrack =  new Trk::Track( initTrack->info(), myNewTrackStateOnSurfaces, 
					    initTrack->fitQuality() ? 
					    initTrack->fitQuality()->clone():0 );
    
    // check that track exists and has enough hits left after turning hits to outliers
    if ( !newTrack) {
      ATH_MSG_DEBUG("no track");
      //delete myNewTrackStateOnSurfaces;
      return 0;
    }
    ATH_MSG_DEBUG("tsos size: "<<newTrack->trackStateOnSurfaces()->size());

    if (newTrack->trackStateOnSurfaces()->size() < 10 ) {
      ATH_MSG_DEBUG("not enough TSOS");
      delete newTrack;
      return 0;
    }
    
    // Refit track with MDT marked as outliers
    ATH_MSG_DEBUG("refitting track with hits marked as outliers");
    //ATH_MSG_DEBUG("perigee params: "<<newTrack->perigeeParameters());
    if (newTrack->perigeeParameters()) ATH_MSG_DEBUG(*newTrack->perigeeParameters());
    else {
      ATH_MSG_DEBUG("no perigeeParams, printing first trackParam");
      ATH_MSG_DEBUG(**(newTrack->trackParameters()->begin()));
    }    
    Trk::Track* refitNewTrack = 
      (m_trackFittingType==CombinedMuonTrackBuilder ||
       m_trackFittingType==MSStandaloneFromCombinedMuon) ? 
      m_combinedTrackBuilder->fit(*newTrack,false,Trk::muon) : 
      m_trackFitter->fit(*newTrack,false,Trk::muon);
    
    if ( !refitNewTrack ) {
      //delete myNewTrackStateOnSurfaces;
      delete newTrack;
      return 0;
    }
    if (!refitNewTrack->trackStateOnSurfaces() ) {
      delete newTrack;
      delete refitNewTrack;
      return 0;
    }
    
    return refitNewTrack;
  }

  //________________________________________________________________________
  Trk::Track*
  UnbiasedMuonResiduals::cleanTrack(Identifier id1,
				    const Trk::Track& track, 
				    bool removePerigee)
  {
    ATH_MSG_DEBUG("in cleanTrack");
    
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if ( !states ) return 0;
    
    // loop over TSOSs, find MDT hits, and mark them as outliers:
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
        

    bool isBEE1=(p_idHelperTool->isMdt(id1) && p_idHelperTool->mdtIdHelper().stationNameString(p_idHelperTool->mdtIdHelper().stationName(id1)).substr(0,3)=="BEE");
    bool removeBarrel=p_idHelperTool->isEndcap(id1)||isBEE1;

    // This is the list of new TSOS I'll use without the MDTs
    DataVector<const Trk::TrackStateOnSurface>* myNewTrackStateOnSurfaces = 
      new DataVector<const Trk::TrackStateOnSurface>();
    myNewTrackStateOnSurfaces->reserve( states->size() );
    

    int itsit(0);
    for ( ; tsit!=tsit_end ; ++tsit,itsit++ ) {

      if ((**tsit).type(Trk::TrackStateOnSurface::Perigee)) {
	ATH_MSG_DEBUG("in cleanTrack, have perigee: "<<**tsit);
	if (!removePerigee) 
	  myNewTrackStateOnSurfaces->push_back(cloneTSOS(*tsit) );
	continue;
      }

      const Trk:: MaterialEffectsBase* materialEffects = 
	(*tsit)->materialEffectsOnTrack();
      if (materialEffects) m_materialOnTrack += materialEffects->thicknessInX0();
      
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if ( !meas ) {
	myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit) );
	continue;
      }
      
      const Trk::MeasurementBase*      mesb = (*tsit)->measurementOnTrack();      
      const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
      if (!rio && crio) rio=&crio->rioOnTrack(0);
      if (!rio) {
	myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit) );
	continue;
      }
      
      Identifier id = p_idHelperTool->chamberId(rio->detectorElement()->identify());

      // if barrel and doing endcap cap or vice versa, remove from track
      bool isBEE=(p_idHelperTool->isMdt(id) && p_idHelperTool->mdtIdHelper().stationNameString(p_idHelperTool->mdtIdHelper().stationName(id)).substr(0,3)=="BEE");
      bool isEndcap=p_idHelperTool->isEndcap(id)||isBEE;
      ATH_MSG_DEBUG("isEndcap: "<<isEndcap);
      if ( p_idHelperTool->isMuon(id) && isEndcap!=removeBarrel ) {
	ATH_MSG_DEBUG("have endcap and doing barrel (or vice versa)");
	continue;
      }

      myNewTrackStateOnSurfaces->push_back( cloneTSOS(*tsit) );
      continue;
    }
    
    // Create a new track with hits on track and outliers:
    Trk::Track* newTrack =  new Trk::Track( track.info(), myNewTrackStateOnSurfaces, 
					    track.fitQuality() ? 
					    track.fitQuality()->clone():0 );
    
    if (!newTrack) delete myNewTrackStateOnSurfaces;
    
    return newTrack;
  }

  //________________________________________________________________________
  const Trk::TrackParameters* 
  UnbiasedMuonResiduals::getTrackExtrapolated(Identifier id1,
					      const Trk::Track* unbiasedTrack,
					      const Trk::PlaneSurface* detSurface)
  {  


    // Now loop over the TSOS from above track and get track parameters in the MDT
    const DataVector<const Trk::TrackStateOnSurface>* states = 
      unbiasedTrack->trackStateOnSurfaces();
  
    // loop over TSOSs, find first TSOS in selected chamber, return track parameters
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit     = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();

    for ( ; tsit!=tsit_end ; ++tsit ) {
    
      const Trk::MeasurementBase*      mesb = (*tsit)->measurementOnTrack();      
      const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
      if (!rio && crio) rio=&crio->rioOnTrack(0);
      if (!rio) continue;
    
      if (!(*tsit)->type(Trk::TrackStateOnSurface::Outlier)) continue;

      Identifier id = p_idHelperTool->chamberId(rio->detectorElement()->identify());
      if (id!=id1) continue;

      // Track parameters on detector surface
      const Trk::TrackParameters* trackPar = (*tsit)->trackParameters();
      if ( !trackPar ) continue;
    
      // extrapolate to detector surface
      const Trk::TrackParameters* trackParAtDetSurface = 
	p_IExtrapolator->extrapolate( *trackPar, *detSurface, 
				      Trk::anyDirection, false, Trk::nonInteracting);
      if ( !trackParAtDetSurface ) continue;    

      ATH_MSG_DEBUG("original trackPar:    "<<*trackPar);
      ATH_MSG_DEBUG("trackParAtDetSurface: "<<*trackParAtDetSurface);

      return trackParAtDetSurface;
    }

    return 0;
  }

  //________________________________________________________________________
  bool UnbiasedMuonResiduals::checkMuon(const Analysis::Muon& muon)
  {
    const Rec::TrackParticle * cbTrackParticle = muon.combinedMuonTrackParticle();
    if ( !cbTrackParticle ) return 0;
    
    const Trk::Perigee * cbperigee = cbTrackParticle->measuredPerigee();
    if ( !cbperigee ) return 0;
    
    // Only keep combined muons with ID and MS tracks, and best match !
    if ( !muon.isCombinedMuon() ) return 0;
  
    // Cut on momentum
    if ( muon.p() < m_MinMuonMom ) return 0;

    // Cut on track hit multiplicity
    if ( muon.numberOfPixelHits()  < m_minPIXHits ) return 0;
    if ( muon.numberOfSCTHits()    < m_minSCTHits ) return 0;
    if ( muon.numberOfTRTHits()    < m_minTRTHits ) return 0;
    if ( muon.numberOfMDTHits()    < m_minMDTHits ) return 0;
    if ( muon.numberOfCSCEtaHits() < m_minCSCHits ) return 0;
    
    m_nPixHits=muon.numberOfPixelHits();
    m_nSCTHits=muon.numberOfSCTHits();
    m_nTRTHits=muon.numberOfTRTHits();
    m_nCSCHits=muon.numberOfCSCEtaHits();
    m_nMDTHits=muon.numberOfMDTHits();
  
    m_author=(int)muon.author();
    std::pair<double,double> eloss=muon.energyLoss();
    m_calEnergyLoss=eloss.first;
    m_calEnergyLossErr=eloss.second;

    // fill some tree variables (filled separately if Analysis::Muon not used)
    m_charge=cbperigee->charge();
    m_momentum = muon.p() /1000.;
    m_d0       = cbperigee->parameters()[Trk::d0];
    m_z0       = cbperigee->parameters()[Trk::z0];
    m_phi0     = MuonAlign::RadsToDegrees(cbperigee->parameters()[Trk::phi0]);
    m_theta0   = MuonAlign::RadsToDegrees(cbperigee->parameters()[Trk::theta]);
    m_eta0     = cbperigee->eta();          

    ATH_MSG_DEBUG("momentum: "<<m_momentum<<", charge: "<<m_charge);
    ATH_MSG_DEBUG("d0: "<<m_d0<<", z0: "<<m_z0<<", phi0: "<<m_phi0);
    ATH_MSG_DEBUG("theta: "<<m_theta0<<", eta: "<<m_eta0);

    return true;
  }

  //________________________________________________________________________
  bool UnbiasedMuonResiduals::checkTrack(const Trk::Track& track)
  {  
    ATH_MSG_DEBUG("in checkTrack");

    int nLayerHits[4]={0,0,0,0};

    int nBEEHits=0;
    int nEEHits =0;

    m_nTriggerPhiHits=0;

    bool cscSector5Aor9A=false;

    const DataVector<const Trk::TrackStateOnSurface>* tsosCollection=track.trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=tsosCollection->begin(); 
    for (; iTsos!=tsosCollection->end(); ++iTsos) {

      if ((*iTsos)->type(Trk::TrackStateOnSurface::Perigee))
	ATH_MSG_DEBUG("track has perigee");

      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) {
      
	// get measurement, continue if hit to be removed
	const Trk::MeasurementBase*      mesb = (*iTsos)->measurementOnTrack();      
	const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=&crio->rioOnTrack(0);
	if (!rio) continue;
      
	Identifier detId=rio->detectorElement()->identify();
	//Identifier rioId=rio->identify();

	if (!p_idHelperTool->isMuon(detId))  continue;      

	// get measurement type
	TrackState::MeasurementType measType;       
	if      (p_idHelperTool->isMdt(detId)) measType=TrackState::MDT;
	else if (p_idHelperTool->isCsc(detId)) measType=TrackState::CSC;
	else if (p_idHelperTool->isRpc(detId)) measType=TrackState::RPC;
	else if (p_idHelperTool->isTgc(detId)) measType=TrackState::TGC;
	else {
	  ATH_MSG_WARNING("bad muon ID");
	  continue;
	}

	if (measType==TrackState::MDT) {	
	  int stationLayer = getMdtStatLayer(detId);
	  if (stationLayer>3) { ATH_MSG_WARNING("stationLayer>3!"); continue; }
	  nLayerHits[stationLayer]++;
	  if (stationLayer==3) {
	    const std::string stName=
	      p_idHelperTool->mdtIdHelper().stationNameString(p_idHelperTool->mdtIdHelper().stationName(detId));
	    if (stName.substr(0,3)=="BEE") nBEEHits++;
	    if (stName.substr(0,2)=="EE")  nEEHits++;
	  }	  
	} 
	else if (measType==TrackState::CSC) {
	  nLayerHits[0]++;	  
	  int stEta  = p_idHelperTool->cscIdHelper().stationEta(detId);
	  int stPhi  = p_idHelperTool->cscIdHelper().stationPhi(detId);	  
	  if ((stPhi==3||stPhi==5) && stEta>0) {
	    bool isLarge = ! (p_idHelperTool->isSmallChamber( detId ));
	    if (isLarge)  cscSector5Aor9A=true;
	  }
	}
	else if (measType==TrackState::TGC) { 
	  if ( (p_idHelperTool->isTgc(detId) && 
		p_idHelperTool->tgcIdHelper().isStrip(rio->prepRawData()->identify())) ||
	       (p_idHelperTool->isRpc(detId) && 
		p_idHelperTool->rpcIdHelper().measuresPhi(rio->prepRawData()->identify())) )
	    m_nTriggerPhiHits++;
	  
	}	
      }
    }
    
    m_nInnerMDTHits =nLayerHits[0];
    m_nMiddleMDTHits=nLayerHits[1];
    m_nOuterMDTHits =nLayerHits[2];
  
    int minInnerMDTHits=(cscSector5Aor9A)?6:7;
    
    if ( ( m_nInnerMDTHits<minInnerMDTHits && nBEEHits==0 && nEEHits==0) ||
	 ( m_nMiddleMDTHits<5 ) || 
	 ( m_nOuterMDTHits<5 && nEEHits==0 ) ) {
      if ( m_nInnerMDTHits<minInnerMDTHits && nBEEHits==0 && nEEHits==0)
	ATH_MSG_DEBUG("failed inner cut");
      if ( m_nMiddleMDTHits<5 ) ATH_MSG_DEBUG("failed middle cut");
      if ( m_nOuterMDTHits<5 && nEEHits==0 ) {
	ATH_MSG_DEBUG("failed outer cut, outerMDTHits: "<<m_nOuterMDTHits<<", nEE: "<<nEEHits);
      }
      return false;
    }
    if (m_nTriggerPhiHits<1) {
      ATH_MSG_DEBUG("failed trigger phi cut");
      return false;
    }

    if (nEEHits>0) ATH_MSG_DEBUG("have EE hits");

    return true;
  }

  //________________________________________________________________________
  void UnbiasedMuonResiduals::getBiasedResiduals(const Trk::Track& /*track*/)
  {  
    ATH_MSG_ERROR("update getBiasedResiduals for Eigen");  
    return;

    /*
    m_nCSCHits=m_nMDTHits=m_nRPCHits=m_nTGCHits=0;
    
    for (int i=0;i<m_MAXNMDTHITS;i++) { m_mdtRes[i]=-999.; m_mdtPull[i]=-999.; }
    for (int i=0;i<m_MAXNTGCHITS;i++) { m_tgcRes[i]=-999.; m_tgcPull[i]=-999.; }
    for (int i=0;i<m_MAXNCSCHITS;i++) { m_cscRes[i]=-999.; m_cscPull[i]=-999.; }
    for (int i=0;i<m_MAXNRPCHITS;i++) { m_rpcRes[i]=-999.; m_rpcPull[i]=-999.; }
    
    const DataVector<const Trk::TrackStateOnSurface>* tsosCollection=track.trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=tsosCollection->begin(); 
    for (; iTsos!=tsosCollection->end(); ++iTsos) {
    
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) {
      
	// get measurement, continue if hit to be removed
	const Trk::MeasurementBase*      mesb = (*iTsos)->measurementOnTrack();      
	const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=&crio->rioOnTrack(0);
	if (!rio) continue;
      
	Identifier detId=rio->detectorElement()->identify();
	Identifier rioId=rio->identify();

	if (!p_idHelperTool->isMuon(detId))  continue;      

	// get measurement type
	TrackState::MeasurementType measType;       
	if (p_idHelperTool->isMdt(detId))      measType=TrackState::MDT;
	else if (p_idHelperTool->isCsc(detId)) measType=TrackState::CSC;
	else if (p_idHelperTool->isRpc(detId)) measType=TrackState::RPC;
	else if (p_idHelperTool->isTgc(detId)) measType=TrackState::TGC;
	else {
	  ATH_MSG_WARNING("bad muon ID");
	  continue;
	}

	// get residualPull
	const Trk::ResidualPull* resPull = 0;
	const Trk::TrackParameters* trackPars = (*iTsos)->trackParameters();
	
	if ( trackPars ) {
	  resPull = m_pullCalculator->residualPull( mesb, trackPars,
						    Trk::ResidualPull::HitOnly,
						    measType);
	}
	
	if (measType==TrackState::MDT) {	

	  if (m_nMDTHits<=m_MAXNMDTHITS) {
	    if (resPull) {
	      m_mdtRes [m_nMDTHits]=resPull->residual().front();
	      m_mdtPull[m_nMDTHits]=resPull->pull().front();
	    }
	    m_nMDTHits++;
	  }
	} 
	else if (measType==TrackState::CSC) {
	  if (m_nCSCHits<=m_MAXNCSCHITS) {
	    m_cscHitLocX  [m_nCSCHits]=rio->localParameters()[Trk::locX];
	    m_cscHitLocY  [m_nCSCHits]=rio->localParameters()[Trk::locY];
	    m_cscHitErrors[m_nCSCHits]=rio->localErrorMatrix().error(Trk::loc1);
	    if (resPull) {
	      m_cscRes [m_nCSCHits]=resPull->residual().front();
	      m_cscPull[m_nCSCHits]=resPull->pull().front();
	    }
	    m_nCSCHits++;
	  }
	}
	else if (measType==TrackState::TGC) { 
	  if (m_nTGCHits<=m_MAXNTGCHITS) {
	    if (resPull) {
	      m_tgcRes [m_nTGCHits]=resPull->residual().front();
	      m_tgcPull[m_nTGCHits]=resPull->pull().front();
	    }
	    m_nTGCHits++;
	  }     
	}	
	else if (measType==TrackState::RPC) { 

	  if (m_nRPCHits<=m_MAXNRPCHITS) {
	    if (resPull) {
	      m_rpcRes [m_nRPCHits]=resPull->residual().front();
	      m_rpcPull[m_nRPCHits]=resPull->pull().front();
	    }
	    m_nRPCHits++;	    
	  }
	}
      }
    }
    
    return;
    */
  } 

  //________________________________________________________________________  
  const Trk::TrackParameters* 
  UnbiasedMuonResiduals::getIDTrackExtrapolated(const Trk::Track* idtrack,
						const Trk::PlaneSurface* detSurface)
  {
    if ( !idtrack ) return 0;
    if ( fabs(idtrack->perigeeParameters()->parameters()[Trk::z0]) > m_MaxIDz0 ) return 0;
    if ( fabs(idtrack->perigeeParameters()->parameters()[Trk::d0]) > m_MaxIDd0 ) return 0;
  
    // PROPAGATE TRACK TO MDT SEGMENT USING THE SURFACE ON WHICH THE SEGMENT IS DEFINED
    const Trk::TrackParameters* extraPar = 
      p_IExtrapolator->extrapolate( *idtrack, *detSurface, Trk::alongMomentum, false, Trk::muon);
    if (!extraPar) return 0;
 
    return extraPar;
  }

  //________________________________________________________________________  
  const Trk::TrackParameters* 
  UnbiasedMuonResiduals::getSegmentExtrapolated(const Muon::MuonSegment* /*mSeg*/,
						const Trk::PlaneSurface* /*detSurface*/)
  {
    ATH_MSG_ERROR("migrate getSegmentExtrapolated to Eigen");
    return 0;

    /*
    double qOverP=m_charge/100000.;

    ATH_MSG_DEBUG("in getSegmentExtrapolated, mSeg: "<<*mSeg);

    // construct track parameters
    double locx = mSeg->localParameters().contains(Trk::locX) ? 
      mSeg->localParameters()[Trk::locX] : 0.;
    double locy = mSeg->localParameters().contains(Trk::locY) ? 
      mSeg->localParameters()[Trk::locY] : 0.;
    double globPhi   = mSeg->globalDirection().phi();
    double globTheta = mSeg->globalDirection().theta();
  
    Trk::JacobianPhiThetaLocalAngles globalToLocalMeasAnglesJacobian(globPhi,globTheta,
								     detSurface->transform().getRotation().inverse());
 
    CLHEP::HepSymMatrix globalToLocalMeasJacobian(5,0);
    globalToLocalMeasJacobian[Trk::locX][Trk::locX]   = 1.0;
    globalToLocalMeasJacobian[Trk::locY][Trk::locY]   = 1.0;
    globalToLocalMeasJacobian[Trk::phi][Trk::phi]     = globalToLocalMeasAnglesJacobian[0][0];
    globalToLocalMeasJacobian[Trk::theta][Trk::theta] = globalToLocalMeasAnglesJacobian[1][1];
    globalToLocalMeasJacobian[Trk::theta][Trk::phi]   = globalToLocalMeasAnglesJacobian[0][1]; // also fills [Trk::phi][Trk::theta]
    globalToLocalMeasJacobian[Trk::qOverP][Trk::qOverP] = 1.0;     
    
    
    Trk::JacobianPhiThetaLocalAngles localToGlobalMeasAnglesJacobian(mSeg->localParameters()[Trk::phi],
								     mSeg->localParameters()[Trk::theta],
								     detSurface->transform().getRotation());
    
    ATH_MSG_DEBUG("loc angles: "<<localToGlobalMeasAnglesJacobian);

    CLHEP::HepSymMatrix localToGlobalMeasJacobian(5,0);
    localToGlobalMeasJacobian[Trk::locX] [Trk::locX]  = 1.0;
    localToGlobalMeasJacobian[Trk::locY] [Trk::locY]  = 1.0;
    localToGlobalMeasJacobian[Trk::phi]  [Trk::phi]   = localToGlobalMeasAnglesJacobian[0][0];
    localToGlobalMeasJacobian[Trk::theta][Trk::theta] = localToGlobalMeasAnglesJacobian[1][1];
    localToGlobalMeasJacobian[Trk::theta][Trk::phi]   = localToGlobalMeasAnglesJacobian[0][1]; // also fills [Trk::phi][Trk::theta]
    localToGlobalMeasJacobian[Trk::qOverP][Trk::qOverP] = 1.0;     
    ATH_MSG_DEBUG("jac: "<<localToGlobalMeasJacobian);
    
    Trk::CovarianceMatrix locCovMat=mSeg->localErrorMatrix().covariance();
    
    CLHEP::HepSymMatrix locCovMatFull(5,0);
    for (int i=0;i<4;i++)
      for (int j=i;j<4;j++)
	locCovMatFull.fast(j+1,i+1)=locCovMat.fast(j+1,i+1);
    locCovMatFull.fast(5,5)=1.e-12;
    ATH_MSG_DEBUG("locCovMatFull: "<<locCovMatFull);
    

    Trk::CovarianceMatrix* globCovMat = new Trk::CovarianceMatrix( locCovMatFull.similarity(localToGlobalMeasJacobian) );

    const Trk::MeasuredAtaPlane* segPar=
      new Trk::MeasuredAtaPlane(locx,locy,globPhi,globTheta,qOverP,mSeg->associatedSurface(),globCovMat);

    ATH_MSG_DEBUG("segPar: "<<*segPar);
    ATH_MSG_DEBUG("seg surface: "<<mSeg->associatedSurface());
    ATH_MSG_DEBUG("detSurface: "<<*detSurface);

    // extrapolate segment to detector surface
    const Trk::TrackParameters* segParAtDetSurface = 
      p_IExtrapolator->extrapolate( *segPar, *detSurface, Trk::anyDirection, false,Trk::nonInteracting);
    if (!segParAtDetSurface) ATH_MSG_ERROR("no segParAtDetSurface!");
    else ATH_MSG_DEBUG("segParAtDetSurface: "<<*segParAtDetSurface);

    return segParAtDetSurface;
    */
  }



  //________________________________________________________________________
  void UnbiasedMuonResiduals::compareSurfaces(const std::string& /*trackType*/,
					      const Trk::Surface& /*segSurface*/, 
					      const Trk::Surface& /*detSurf*/)
  {
    ATH_MSG_ERROR("migrate to Eigen");
    return;

    /*
    ATH_MSG_VERBOSE(trackType<<" track, isFree: "
		    <<segSurface.isFree()<<", segSurface: "<<endmsg<<segSurface);
  
    //ATH_MSG_DEBUG(trackType<< " track, normal cross normal: "<<segSurface.normal().cross(detSurf.normal()).mag());
    //ATH_MSG_DEBUG(trackType<<" track, angle: "
    //	<<segSurface.normal().angle(detSurf.normal())*360./6.2832<<" degrees");
  
    const HepGeom::Vector3D<double> unitxvec(1.,0.,0.);
    const HepGeom::Vector3D<double> segSurfX=segSurface.transform()*unitxvec;
    const HepGeom::Vector3D<double> detSurfX=detSurf.transform()*unitxvec;
  
    //ATH_MSG_DEBUG("x dot x: "<<segSurfX.dot(detSurfX));
  
    return;
    */

  } 

  /*
  //________________________________________________________________________
  const Trk::MeasuredAtaPlane* 
  UnbiasedMuonResiduals::trackParameters(const Trk::TrackStateOnSurface* tsit, 
					 const Trk::PlaneSurface& surface)
  {
  
    const Trk::TrackParameters* trackPar = tsit->trackParameters();
  
    if ( !trackPar ) {
      return 0;
    }
  
    ATH_MSG_DEBUG("after refit, tsos trackPar: "<<*trackPar);
  
    if (!trackPar->covariance())) {
      return 0;
    }
  
    const Trk::TrackParameters* newTrackPar = 
      p_IExtrapolator->extrapolate( *trackPar, surface, 
				    Trk::anyDirection, false, 
				    Trk::nonInteracting); // Track parameters at same surface as MDT segment
  
    if ( !newTrackPar ) return 0;
  
    const Trk::MeasuredAtaPlane* trkMeasuredAtaPlane = 
      dynamic_cast<const Trk::MeasuredAtaPlane*> (newTrackPar);
    if ( !trkMeasuredAtaPlane ) {
      ATH_MSG_ERROR("no MeasuredAtaPlane!");
    }
  
    return trkMeasuredAtaPlane;
  } 
  */

  //________________________________________________________________________
  const Analysis::Muon* UnbiasedMuonResiduals::findMuon(const Analysis::MuonContainer* muonTES, 
							Muon::MuonSegment* mSeg) const
  {
  
    Muon::MuonSegmentKey segKeys(*mSeg);
  
    Analysis::MuonContainer::const_iterator muonIt  = muonTES->begin();
    Analysis::MuonContainer::const_iterator muonItE = muonTES->end();
  
    const Analysis::Muon* bestMuon(0);
    int bestNMatches(-1);

    // Loop over muon collection
    for ( ; muonIt!=muonItE; ++muonIt ) {
    
      if (muonIt == muonItE || !*muonIt) continue;

      // Only keep combined muons with ID and MS tracks, and best match !
      if ( !(*muonIt)->isCombinedMuon() ) {
	ATH_MSG_DEBUG("no combined muon!");
	continue;
      }
      const Trk::Track* track = (*muonIt)->combinedMuonTrkTrack();

      // Collect hits from track
      std::vector<const Trk::MeasurementBase*> measurements;	

      DataVector<const Trk::TrackStateOnSurface>::const_iterator it  = track->trackStateOnSurfaces()->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = track->trackStateOnSurfaces()->end();
      for ( ; it!=it_end; ++it) {
	if ( (*it) != 0 ){
	  const Trk::MeasurementBase* rotTest = (*it)->measurementOnTrack();  
	  if (rotTest) measurements.push_back(rotTest);
	}
      }

      Muon::MuonSegmentKey trkKeys(measurements);
    
      std::vector< std::pair<unsigned int,int> > intersection;
      std::set_intersection(segKeys.keys().begin(),segKeys.keys().end(),
			    trkKeys.keys().begin(),trkKeys.keys().end(),
			    std::back_inserter(intersection));
      int intersectionSize = intersection.size();
      int segmentSize = segKeys.keys().size();
    
      // if subset, we're done
      if( segmentSize == intersectionSize ) return *muonIt;
    
      if (intersectionSize>bestNMatches) {
	bestNMatches=intersectionSize;
	bestMuon=*muonIt;
      }
    }
  
    return bestMuon;
  }

  //________________________________________________________________________
  const Trk::Track* UnbiasedMuonResiduals::prepareTrack(const Trk::Track& track, double qOverP)
  {
    ATH_MSG_DEBUG("in findTrack");
        
    const Trk::Track* newTrack=&track;

    bool refitPT=(m_trackFittingType==MSStandaloneWithIDMomentum);
    ATH_MSG_DEBUG("refitPT: "<<refitPT<<", m_trackFittingType: "<<m_trackFittingType);

    if (refitPT) {
      
      ATH_MSG_DEBUG("calling pTCorrectTrack with qOverP="<<qOverP);
      newTrack=m_muonAlignRefitTool->pTCorrectTrack(track,qOverP,true);
      if (!newTrack) {
	ATH_MSG_DEBUG("failed to pT-correct track!");
	return 0;
      }
    }
    
    ATH_MSG_DEBUG("recalibrating hits with muonSegmentTrackBuilder");
    
    //ATH_MSG_DEBUG("before redoing CROTs: ");
    ATH_MSG_DEBUG("before recalibrating hits: ");
    ATH_MSG_DEBUG(m_printer->print(*newTrack)<<endmsg<<
		  m_printer->printStations(*newTrack)<<endmsg<<
		  m_printer->printMeasurements(*newTrack));
  
    const Trk::Track* oldTrack=newTrack;
    newTrack=p_trackBuilder->recalibrateHitsOnTrack(*oldTrack,false,true);
    //newTrack=m_muonAlignRefitTool->redoHitsOnTrack(*bestTrack);
    if (refitPT) delete oldTrack;
    
    if (!newTrack) {
      ATH_MSG_DEBUG("failed to redo CROTs on track!");
      return 0;
    }
    
    //ATH_MSG_DEBUG("after redoing CROTs: ");
    ATH_MSG_DEBUG("after recalibrating hits: ");
    ATH_MSG_DEBUG(m_printer->print(*newTrack)<<endmsg<<
		  m_printer->printStations(*newTrack)<<endmsg<<
		  m_printer->printMeasurements(*newTrack));
    
    if (refitPT) {
      ATH_MSG_DEBUG("calling pTCorrectTrack (again) with qOverP="<<qOverP);
      oldTrack=newTrack;
      newTrack=m_muonAlignRefitTool->pTCorrectTrack(*oldTrack,qOverP,
						       true);
      delete oldTrack;
    }

    /*
      if (m_redoErrorScaling) {
      ATH_MSG_DEBUG("redoing error scaling");
      oldTrack=newTrack;
      newTrack=m_muonAlignRefitTool->rescaleHitErrorsOnTrack(*oldTrack);
      delete oldTrack;
      
      oldTrack=newTrack;
      newTrack=m_muonAlignRefitTool->pTCorrectTrack(*oldTrack,qOverP);
      delete oldTrack;
      }    
    */

    /*
      ATH_MSG_DEBUG("printing perigee for pTCorrectTrack (before fit)");    
      printPerigee(*newTrack);
      ATH_MSG_DEBUG("refitting pTCorrectTrack");
      ATH_MSG_DEBUG("perigee params: "<<newTrack->perigeeParameters());
      if (newTrack->perigeeParameters()) ATH_MSG_DEBUG(*newTrack->perigeeParameters());
      else {
      ATH_MSG_DEBUG("no perigeeParams, printing first trackParam");
      ATH_MSG_DEBUG(**(newTrack->trackParameters()->begin()));
      }
    */
    /*
      const Trk::Track* refittedNewTrack=m_trackFitter->fit(*newTrack,false,Trk::muon);
      delete newTrack;
      ATH_MSG_DEBUG("printing perigee for pTCorretedTrack (after fit)");
    */  

    return newTrack;
  }
  
  //________________________________________________________________________
  const Trk::TrackStateOnSurface* 
  UnbiasedMuonResiduals::cloneTSOS(const Trk::TrackStateOnSurface* tsos,
				   bool makeOutlier)
  {
    
    if (tsos->type(Trk::TrackStateOnSurface::Perigee)) return tsos->clone();

    const Trk::MeasurementBase*     newMeas = tsos->measurementOnTrack() ? tsos->measurementOnTrack()->clone() : 0;
    const Trk::TrackParameters*     newPars = tsos->trackParameters() ? tsos->trackParameters()->clone() : 0;
    const Trk::FitQualityOnSurface* newFitQoS = tsos->fitQualityOnSurface() ? tsos->fitQualityOnSurface()->clone() : 0;
    const Trk::MaterialEffectsBase* meb = tsos->materialEffectsOnTrack() ? tsos->materialEffectsOnTrack()->clone() : 0;
  
    
    if (meb) {
      const Trk::MaterialEffectsOnTrack* meot=dynamic_cast<const Trk::MaterialEffectsOnTrack*>(meb);
      if (meot) {
	double tinX0=meot->thicknessInX0();
	const Trk::EnergyLoss* eLoss=meot->energyLoss() ? meot->energyLoss()->clone() : 0;
	const Trk::Surface& surf=meot->associatedSurface();
	std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes> typeMaterial;
	if (eLoss) typeMaterial.set(MaterialEffectsBase::EnergyLossEffects);
	const Trk::MaterialEffectsOnTrack* newmeot=
	  new Trk::MaterialEffectsOnTrack(tinX0,0,eLoss,surf,typeMaterial);
	delete meb;
	meb=newmeot;
      }
    }    
      
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    for (int i=0;i<(int)Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes;i++) {
      if (tsos->type(Trk::TrackStateOnSurface::TrackStateOnSurfaceType(i)))
	typePattern.set(i);
    }
    if (makeOutlier) typePattern.set(Trk::TrackStateOnSurface::Outlier);
    const Trk::TrackStateOnSurface* newTsos= new Trk::TrackStateOnSurface( newMeas, newPars, newFitQoS, meb, typePattern);
    return newTsos;
  
  }  

  //________________________________________________________________________
  bool UnbiasedMuonResiduals::passesChamberSelection(Trk::AlignModule::DetectorType detType,
						     Identifier id, int flag)
  {
    ATH_MSG_DEBUG("in passesChamberSelection, detTYpe="<<detTypeStr(detType));

    if (detType==Trk::AlignModule::CSC) {
      ATH_MSG_DEBUG("in passesChamberSelection for CSC");
      if ( p_idHelperTool->isCsc(id) ) {
	ATH_MSG_DEBUG("is CSC");
	return true; // Not a CSC
      }
    }
    else if (detType==Trk::AlignModule::MDT) {
      ATH_MSG_DEBUG("in passesChamberSelection for MDT");
      if ( p_idHelperTool->isMdt(id) ) {
	ATH_MSG_DEBUG("is MDT");      
	std::string stName=p_idHelperTool->chamberNameString(id);
	if (flag==0) {
	  if (stName[1]=='I' || (stName.length()>4 && stName[4]=='4') ) {
	    ATH_MSG_DEBUG("returning true (inner)");
	    return true;
	  }
	}
	else return true;
      }
    }
    else if (detType==Trk::AlignModule::TGC) {
      ATH_MSG_DEBUG("in passesChamberSelection for TGC");
      if ( p_idHelperTool->isTgc(id) ) {
	ATH_MSG_DEBUG("is TGC");      
	return true;
      }
    }
    else if (detType==Trk::AlignModule::RPC) {
      ATH_MSG_DEBUG("in passesChamberSelection for RPC");
      if ( p_idHelperTool->isRpc(id) ) {
	ATH_MSG_DEBUG("is RPC");      
	return true;
      }      
    }
    
    ATH_MSG_DEBUG("returning false");
    return false;
  }

  //________________________________________________________________________
  void UnbiasedMuonResiduals::checkCovVar(const Trk::Track& /*track*/) const
  {
    ATH_MSG_ERROR("migrate to Eigen");
    return;

    /*
    if (!msgLvl(MSG::DEBUG)) return;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=track.trackStateOnSurfaces()->begin(); 
    for (; iTsos!=track.trackStateOnSurfaces()->end(); ++iTsos) {
      
      if ((*iTsos)->measurementOnTrack()) 
	ATH_MSG_DEBUG("in UnbiasedMuonResiduals, cov: "<<(*iTsos)->measurementOnTrack()->localErrorMatrix().covValue(Trk::loc1));
      
    }
    return;
    */
  }

  //________________________________________________________________________
  int UnbiasedMuonResiduals::getMdtStatLayer(Identifier id) const 
  {
    
    const std::string stName=
      p_idHelperTool->mdtIdHelper().stationNameString(p_idHelperTool->mdtIdHelper().stationName(id));
    
    // station layer
    switch ( stName[1] ) {
    case 'I' :
      return 0;
    case 'M' :
      return 1;
    case 'O' :
      return 2;
    default:
      //ATH_MSG_INFO("stName: "<<stName<<", id: "<<id);
      return 3;
    }    
    return 3;
  }

  //________________________________________________________________________
  void UnbiasedMuonResiduals::setIdentificationVariables(Trk::AlignModule::DetectorType /*detType*/, const MuonSegment& /*mSeg*/) 
  {

    ATH_MSG_ERROR("migrate to Eigen");
    return; 

    /*
    Identifier segId = chamberId( detType,mSeg );
    bool isSmall     = p_idHelperTool->isSmallChamber( segId );

    bool isBEE_Seg=(p_idHelperTool->isMdt(segId) && p_idHelperTool->mdtIdHelper().stationNameString(p_idHelperTool->mdtIdHelper().stationName(segId)).substr(0,3)=="BEE");
    
    m_isEndcap       = p_idHelperTool->isEndcap( segId ) || isBEE_Seg;
    m_identifier     = (int)segId.get_identifier32().get_compact();
    
    if (detType==Trk::AlignModule::MDT) {
      m_stName = p_idHelperTool->mdtIdHelper().stationName(segId);
      m_stEta  = p_idHelperTool->mdtIdHelper().stationEta(segId);
      m_stPhi  = p_idHelperTool->mdtIdHelper().stationPhi(segId);
    }
    else if (detType==Trk::AlignModule::CSC) {
      m_stName = p_idHelperTool->cscIdHelper().stationName(segId);
      m_stEta  = p_idHelperTool->cscIdHelper().stationEta(segId);
      m_stPhi  = p_idHelperTool->cscIdHelper().stationPhi(segId);	  
    }    
    else if (detType==Trk::AlignModule::RPC) {
      m_stName = p_idHelperTool->rpcIdHelper().stationName(segId);
      m_stEta  = p_idHelperTool->rpcIdHelper().stationEta(segId);
      m_stPhi  = p_idHelperTool->rpcIdHelper().stationPhi(segId);	  
    }    
    else if (detType==Trk::AlignModule::TGC) {
      m_stName = p_idHelperTool->tgcIdHelper().stationName(segId);
      m_stEta  = p_idHelperTool->tgcIdHelper().stationEta(segId);
      m_stPhi  = p_idHelperTool->tgcIdHelper().stationPhi(segId);	  
    }
    
    // Here, modify phiSector so it matches the 1-16 map instead of 1-8:
    m_phiSector = p_idHelperTool->stationPhi( segId );
    if (detType!=Trk::AlignModule::TGC) {
      if ( isSmall ) 
	m_phiSector = 2 * m_phiSector;     // Small chambers start at 2    
      else 
	m_phiSector = 2 * m_phiSector - 1; // Large chambers start at 1    
    }

    const Trk::GlobalPosition& globalPos = mSeg.globalPosition();
    m_isCSide  = globalPos.z()>0 ? 0 : 1;

    ATH_MSG_DEBUG("isEC: "<<m_isEndcap<<", id: "<<m_identifier);
    ATH_MSG_DEBUG("isCSide: "<<m_isCSide);

    m_layer = (detType==Trk::AlignModule::MDT) ? getMdtStatLayer(segId) : 0;
    if (detType==Trk::AlignModule::TGC) m_layer=p_idHelperTool->tgcIdHelper().gasGap(segId);

    return;
    */
  }

  //________________________________________________________________________
  void UnbiasedMuonResiduals::calcResForExtrapolatedIDToDetSurface(const Analysis::Muon& /*muon*/,
								   const Trk::TrackParameters& /*segPar*/,
								   const Trk::PlaneSurface* /*detSurface*/,
								   Trk::AlignModule::DetectorType /*detType*/)
								  
  {
    ATH_MSG_ERROR("migrate to Eigen");
    return;
    
    /*
    m_dZlocal=m_dR=-999.;

    const Trk::Track* idTrack = muon.inDetTrkTrack();

    const Trk::TrackParameters* idTrackPar=0;
    if (idTrack)
      idTrackPar = getIDTrackExtrapolated(idTrack,detSurface);    
    if ( !idTrackPar ) {
      ATH_MSG_DEBUG("ID track extrapolation failed");
      return;
    }      
    
    // ID track extrapolated       
    const Trk::PlaneSurface* segSurface=
      dynamic_cast<const Trk::PlaneSurface*>(segPar.associatedSurface());    

    const Trk::LocalPosition&  trklocalPos  = idTrackPar->localPosition();
    const Trk::GlobalPosition* trkglobalPos = segSurface->localToGlobal( trklocalPos );

    const Trk::GlobalPosition& globalPos = segPar.position();
    const Trk::LocalPosition*  localPos = segSurface->globalToLocal(globalPos);
    
    // This is the precision direction in local coordinates (better for endcap)
    m_dZlocal = trklocalPos.y() - localPos->y();  
    
    // Radial distance between track and segment at surface:
    float Rtrk = sqrt( trkglobalPos->x()*trkglobalPos->x() + trkglobalPos->y()*trkglobalPos->y() );
    float Rseg = sqrt( globalPos.x()*globalPos.x() + globalPos.y()*globalPos.y() );
    
    m_dR   = Rtrk - Rseg;
    if (detType==Trk::AlignModule::CSC) {
      //float sinTilt = 0.9796;  // to account for 11.59 degree tilt of chambers
      //m_dR /= sinTilt; 
      m_dR *= cos(11.59/CLHEP::degree);
    }
    ATH_MSG_DEBUG("dR: "<<m_dR<<", dZ: "<<m_dZlocal);

    delete idTrackPar;    
    return;
    */

  }
  
  //________________________________________________________________________
  void UnbiasedMuonResiduals::calcResForSegmentAtDetSurface(const Trk::TrackParameters& /*trackPar*/,
							    const Trk::TrackParameters& /*segPar*/,
							    const Trk::ErrorMatrix& /*trkErrMat*/,
							    const Trk::ErrorMatrix& /*segErrMat*/)
  {

    ATH_MSG_ERROR("migrate to Eigen");
    return;

    /*
    ATH_MSG_DEBUG("trackparam extrapolated: "<<trackPar);
    //ATH_MSG_DEBUG("trackparam extrapolated to surface: "<<trackPar.associatedSurface());
    ATH_MSG_DEBUG("segment extrapolated:    "<<segPar);
    
    // delta phi and theta
    m_deltaPhi  =std::fabs(trackPar.parameters()[Trk::phi]-segPar.parameters()[Trk::phi]);
    m_deltaTheta=std::fabs(trackPar.parameters()[Trk::theta]-segPar.parameters()[Trk::theta]);
    
    // unbiased residuals -- locX and locY
    double locX_Segment = segPar.parameters()[Trk::locX];
    double locY_Segment = segPar.parameters()[Trk::locY];
    double localTrkX    = trackPar.parameters()[Trk::locX];
    double localTrkY    = trackPar.parameters()[Trk::locY];  

    m_detTheta    = MuonAlign::RadsToDegrees(trackPar.parameters()[Trk::theta]);
    m_detThetaErr = MuonAlign::RadsToDegrees(trkErrMat.error(Trk::theta));

    // Y 'seems' to be precision coordinate
    m_dXlocal           = localTrkX - locX_Segment;
    m_dYlocal           = localTrkY - locY_Segment;
    ATH_MSG_INFO("detType: "<<m_detType<<", dXlocal: "<<m_dXlocal<<", dYlocal: "<<m_dYlocal);

    Trk::CovarianceMatrix trackCovMat=trkErrMat.covariance();
    double ErrX_trk2 = trackCovMat[Trk::locX][Trk::locX];
    double ErrY_trk2 = trackCovMat[Trk::locY][Trk::locY];
    
    // don't include segment error because it's small 
    //Trk::CovarianceMatrix segCovMat=segErrMat.covariance();
    //ATH_MSG_DEBUG("segCovMat: "<<segCovMat);
    //double ErrX_seg2 = segCovMat[Trk::locX][Trk::locX];
    //double ErrY_seg2 = segCovMat[Trk::locY][Trk::locY];    

    double ErrX=std::sqrt(ErrX_trk2);// + ErrX_seg2);
    double ErrY=std::sqrt(ErrY_trk2);// + ErrY_seg2);

    m_pullXlocal = m_dXlocal / ErrX;
    m_pullYlocal = m_dYlocal / ErrY;

    const Trk::PlaneSurface* segSurface=
      dynamic_cast<const Trk::PlaneSurface*>(segPar.associatedSurface());    

    //HepGeom::Vector3D<double> SegmentGlobalDirection(segPar.momentum());
    ATH_MSG_DEBUG("seg momentum: "<<segPar.momentum());
    const Trk::LocalDirection* SegmentLocalDirection = 
      segSurface->globalToLocalDirection(segPar.momentum());
    double locAX_Segment = SegmentLocalDirection->angleXZ();
    double locAY_Segment = SegmentLocalDirection->angleYZ();  
    
    // global positions
    const Trk::PlaneSurface*   segPlane=dynamic_cast<const Trk::PlaneSurface*>(segSurface);
    Trk::LocalPosition         segLoc(locX_Segment,locY_Segment);
    const Trk::GlobalPosition* segGlob=segPlane->localToGlobal(segLoc);
    
    Trk::LocalPosition trkLoc(localTrkX,localTrkY);
    const Trk::GlobalPosition* trkGlob=segPlane->localToGlobal(trkLoc);
    
    m_dXglobal=trkGlob->x()-segGlob->x();
    m_dYglobal=trkGlob->y()-segGlob->y();
    m_dZglobal=trkGlob->z()-segGlob->z();
    ATH_MSG_DEBUG("dXglobal: "<<m_dXglobal<<", dYglobal: "<<m_dYglobal
		  <<", dZglobal: "<<m_dZglobal);
    
    delete segGlob;
    delete trkGlob;
    
    // Track direction ( angles )
    HepGeom::Vector3D<double> TrkGlobalDirection( trackPar.momentum() );
    const Trk::LocalDirection* pTrackLocalDirection = 
      dynamic_cast<const Trk::PlaneSurface*>(segSurface)->globalToLocalDirection( TrkGlobalDirection );
    double locAX_trk  = pTrackLocalDirection->angleXZ() ;
    double locAY_trk  = pTrackLocalDirection->angleYZ() ;    
    double pullXAngle = MuonAlign::FixRad(locAX_trk - locAX_Segment);	
    double pullYAngle = MuonAlign::FixRad(locAY_trk - locAY_Segment);
    m_dXlocalAngle    = pullXAngle*1000.;  // to mrad !!!
    m_dYlocalAngle    = pullYAngle*1000.;      
    ATH_MSG_DEBUG("dXlocalAngle: "<<m_dXlocalAngle<<", dYlocalAngle: "<<m_dYlocalAngle);

    m_pullXlocalAngle=pullXAngle;
    m_pullYlocalAngle=pullYAngle;

    delete pTrackLocalDirection;
    
    return;
    */
  }

  //________________________________________________________________________
  void UnbiasedMuonResiduals::removeExtraSegments(Trk::AlignModule::DetectorType /* detType*/,
						  std::list<Muon::MuonSegment*>& /*muonSegments*/) const
  {

    ATH_MSG_ERROR("migrate to Eigen");
    return;

    /*
    ATH_MSG_DEBUG("removeExtraSegments, initial segments size: "<<muonSegments.size());

    std::list<Muon::MuonSegment*>::const_iterator segIt   =muonSegments.begin();
    std::list<Muon::MuonSegment*>::const_iterator segItEnd=muonSegments.end();

    // get list of chamber IDs
    std::list<Identifier> chIds;
    for ( ; segIt != segItEnd; ++segIt) {
      std::set<Identifier> segIds=chamberIds(detType,**segIt);
      for (std::set<Identifier>::iterator idIt=segIds.begin();
	   idIt!=segIds.end();++idIt) {
	std::list<Identifier>::iterator result=find(chIds.begin(),chIds.end(),*idIt);
	if (result==chIds.end()) {
	  chIds.push_back(*idIt);
	  ATH_MSG_VERBOSE("adding "<<*idIt<<" to chamberIds");
	}
      }
    }
    
    // iterator over chamber IDs and find best segment
    std::list<Muon::MuonSegment*> bestSegments;
    std::list<Identifier>::iterator idIt=chIds.begin();
    std::list<Identifier>::iterator idItEnd=chIds.end();
    for (; idIt != idItEnd; ++idIt) {
      Identifier id=*idIt;
      ATH_MSG_VERBOSE("getting best seg for id "<<id);
      
      double bestChi2=99999.;
      Muon::MuonSegment* bestSeg=0;
      for (segIt=muonSegments.begin();segIt!=segItEnd;++segIt) {
	double chi2perdof=(**segIt).fitQuality()->chiSquared()/(**segIt).fitQuality()->doubleNumberDoF();
	if (id==chamberId(detType,**segIt) &&
	    chi2perdof<bestChi2) {
	  ATH_MSG_VERBOSE("new best chi2 for ID: "<<chamberId(detType,**segIt)
			  <<", seg chi2/dof: "<<chi2perdof);
	  bestChi2=chi2perdof;
	  bestSeg =*segIt;
	}
      }
      if (bestSeg) {
	bestSegments.push_back(bestSeg);
	ATH_MSG_DEBUG("best ID: "<<chamberId(detType,*bestSeg)
		      <<", seg chi2/dof: "<<bestChi2);
      }
      else ATH_MSG_VERBOSE("no segment found");
    }
    muonSegments=bestSegments;
    ATH_MSG_DEBUG("final segments size: "<<muonSegments.size());
    
    return;
    */
  }

  //________________________________________________________________________
  Trk::Track* UnbiasedMuonResiduals::correctErrors(const Trk::Track& track)
  {
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if ( !states ) return 0;
    
    // loop over TSOSs, find MDT hits, and mark them as outliers:
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    
        
    DataVector<const Trk::TrackStateOnSurface>* newTrackStatesOnSurface =
      new DataVector<const Trk::TrackStateOnSurface>;

    for ( ; tsit!=tsit_end ; ++tsit ) {

      const Trk::MeasurementBase*      mesb = (**tsit).measurementOnTrack();
      const Trk::TrackParameters*      tpar = (**tsit).trackParameters();
      const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);

      Identifier id(0);
      if      (rio)  id=rio->identify();
      else if (crio) id=crio->rioOnTrack(0).identify();

      // check if muon hit with track parameters
      if (mesb && tpar && (rio || crio) && p_idHelperTool->isMuon(id) ) {
	
	// MDT or CSC RIO_OnTrack
	if (rio) {
	  
	  // get updated RIO
	  const Trk::RIO_OnTrack* updatedRIO=0;
	  if (p_idHelperTool->isMdt(id)) 
	    updatedRIO=m_mdtRotCreator->correct(*rio->prepRawData(),*tpar);
	  else if (p_idHelperTool->isCsc(id))
	    updatedRIO=m_cscRotCreator->correct(*rio->prepRawData(),*tpar);
	  
	  if (updatedRIO) {
	    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
	    type.set(Trk::TrackStateOnSurface::Measurement);
	    if ((**tsit).type(Trk::TrackStateOnSurface::Outlier))
	      type.set(Trk::TrackStateOnSurface::Outlier);
	    const Trk::FitQualityOnSurface* fqos=(**tsit).fitQualityOnSurface() ? 
	      new Trk::FitQualityOnSurface(*(**tsit).fitQualityOnSurface()) : 0;
	    const Trk::MaterialEffectsBase* meb=(**tsit).materialEffectsOnTrack() ?
	      (**tsit).materialEffectsOnTrack()->clone() : 0;
	    
	    const Trk::TrackStateOnSurface* newTsos=
	      new Trk::TrackStateOnSurface(updatedRIO,tpar->clone(),fqos,meb,type);
	    newTrackStatesOnSurface->push_back(newTsos);
	  }
	} // rio
	
	// RPC or TGC CompetingRIOsOnTrack
	else if (crio) {
	  
	  const Trk::FitQualityOnSurface* fqos=(**tsit).fitQualityOnSurface() ? 
	    new Trk::FitQualityOnSurface(*(**tsit).fitQualityOnSurface()) : 0;
	  const Trk::MaterialEffectsBase* meb=(**tsit).materialEffectsOnTrack() ?
	    (**tsit).materialEffectsOnTrack()->clone() : 0;
	  const Trk::MeasurementBase* newMeas = mesb->clone();
	  const Trk::TrackStateOnSurface* newTsos = new Trk::TrackStateOnSurface(newMeas, tpar->clone(),fqos,meb,(**tsit).types());
	  newTrackStatesOnSurface->push_back(newTsos);
	} // crio
      } // good muon hit
      else {
	const Trk::TrackStateOnSurface* newTsos=(**tsit).clone();
	newTrackStatesOnSurface->push_back(newTsos);
      }
    }
        
    return  new Trk::Track(Trk::TrackInfo(track.info()),
			   newTrackStatesOnSurface,
			   track.fitQuality()->clone());
  }

  //________________________________________________________________________
  std::set<Identifier> UnbiasedMuonResiduals::chamberIds(Trk::AlignModule::DetectorType detType,
							 const MuonSegment& seg) const
  {
    ATH_MSG_DEBUG("getting chamberIds for detType: "<<detType);
    std::set<Identifier> chIds;
    if (detType==Trk::AlignModule::MDT || detType==Trk::AlignModule::CSC) 
      chIds=p_muonEDMHelperTool->chamberIds(seg);
    else {
      Identifier chid;
      std::vector<const Trk::MeasurementBase*>::const_iterator mit = seg.containedMeasurements().begin();
      std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg.containedMeasurements().end();
      for( ;mit!=mit_end;++mit ) {
	
	// get Identifier
	Identifier id;
	const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(*mit);
	if (rio)  id=rio->identify();
	else {
	  const Trk::CompetingRIOsOnTrack* crio=dynamic_cast<const Trk::CompetingRIOsOnTrack*>(*mit);
	  if (crio) id=crio->rioOnTrack(0).identify();
	  else continue;
	}
	
	// check if valid ID
	if( !id.is_valid() ) continue;

	// check if muon
	if (!p_idHelperTool->isMuon(id)) continue;

	// create chamber ID
	chid = p_idHelperTool->chamberId(id);
	
	// check if valid trigger ID
	if( p_idHelperTool->isTrigger(id) && chid.is_valid() ) {
	  chIds.insert(chid);
	}
      }
    }
    ATH_MSG_DEBUG("found "<<chIds.size()<<" chamberIds");
    return chIds;
  }  
  
  //________________________________________________________________________
  Identifier UnbiasedMuonResiduals::chamberId(Trk::AlignModule::DetectorType detType,
					      const MuonSegment& seg) const
  {
    ATH_MSG_DEBUG("getting chamberId for detType: "<<detType);
    Identifier id;
    if (detType==Trk::AlignModule::MDT || detType==Trk::AlignModule::CSC) 
      id=p_muonEDMHelperTool->chamberId(seg);
    else 
      id=triggerChamberId(seg);
    ATH_MSG_DEBUG("id: "<<id);
    if (!id.is_valid() || !p_idHelperTool->isMuon(id)) return Identifier();
    return id;
  }

  //________________________________________________________________________
  Identifier UnbiasedMuonResiduals::triggerChamberId(const MuonSegment& seg) const
  {
    Identifier chid;
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = seg.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg.containedMeasurements().end();
    for ( ;mit!=mit_end;++mit ){
      
      // get Identifier
      Identifier id;
      const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(*mit);
      if (rio)  id=rio->identify();
      else {
	const Trk::CompetingRIOsOnTrack* crio=dynamic_cast<const Trk::CompetingRIOsOnTrack*>(*mit);
	if (crio) id=crio->rioOnTrack(0).identify();
	else continue;
      }

      if ( !id.is_valid() ) continue;
      
      // create chamber ID
      chid = p_idHelperTool->chamberId(id);
      
      // stop at first trigger hit
      if ( p_idHelperTool->isTrigger(id) ) break;

      // check if valid
      if (!id.is_valid()) break;
    }
    return chid;
  }
	
  //________________________________________________________________________
  void UnbiasedMuonResiduals::printPerigee(const Trk::Track& track)
  {
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if ( !states ) return;
    
    // loop over TSOSs, find MDT hits, and mark them as outliers:
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    
    for ( ; tsit!=tsit_end ; ++tsit ) {
      
      if ((**tsit).type(Trk::TrackStateOnSurface::Perigee))
	ATH_MSG_DEBUG("in printPerigee, have perigee: "<<**tsit);   
    }
    return;
  }

  //________________________________________________________________________
  const Trk::Track* UnbiasedMuonResiduals::findBestTrack(Trk::AlignModule::DetectorType detType,
							 const TrackCollection* tracks, 
							 const TrackCollection* inDetTracks,
							 Muon::MuonSegment* mSeg=0)
  {
    
    // this method returns the track which has the same hits as the segment if mSeg!=0.  
    // otherwise, it returns the first track in the collection

    ATH_MSG_DEBUG("in findBestTrack");

    TrackCollection::const_iterator trackIt  = tracks->begin();
    TrackCollection::const_iterator trackItE = tracks->end();
    
    const Trk::Track* bestTrack(0);
    int bestNMatches(-1);
    
    // Loop over muon collection
    int itrack(0);
    std::vector<const Trk::Track*> tracksToDelete;
    for ( ; trackIt!=trackItE; ++trackIt,itrack++ ) {
      
      if (trackIt == trackItE || !*trackIt) continue;
      
      const Trk::Track* track=*trackIt;

      ATH_MSG_DEBUG("in findBestTrack: track "<<itrack<<"/"<<tracks->size());      
      ATH_MSG_DEBUG(m_printer->print(*track)<<endmsg<<
		    m_printer->printStations(*track)<<endmsg<<
		    m_printer->printMeasurements(*track));
      
      //double muonEta=track->perigeeParameters()->eta();
      //double muonPhi=track->perigeeParameters()->parameters()[Trk::phi];
      
      // ATH_MSG_DEBUG("inDetTracks: "<<inDetTracks);
      ATH_MSG_DEBUG("m_trackFittingType: "<<m_trackFittingType);
      
      if (inDetTracks && m_trackFittingType == MSStandaloneWithIDTrackMatch) {
	
	// loop over ID tracks and see if there is a good match
        TrackCollection::const_iterator inDetTrackIt  = inDetTracks->begin();
        TrackCollection::const_iterator inDetTrackItE = inDetTracks->end();  
        const Trk::Track* bestIDTrack = 0;	
	double bestMatchChi2 = m_matchChi2Cut;
        int iIdTrack=0;
        for ( ; inDetTrackIt!=inDetTrackItE; ++inDetTrackIt,iIdTrack++ ) {

	  const Trk::Track* inDetTrack = *inDetTrackIt;
	  
	  // check momentum
	  if (!inDetTrack->perigeeParameters()) continue;
	  
	  double matchChi2 = m_tagTool->chi2(*inDetTrack,*track);
	  if (matchChi2<bestMatchChi2) {
	    bestMatchChi2=matchChi2;
	    bestIDTrack=inDetTrack;
	  } 	  
	}
	
	if (bestIDTrack) {
	  m_charge        = (*(bestIDTrack->trackParameters()->begin()))->charge();
	  double pT       = bestIDTrack->perigeeParameters()->pT();
	  double inDetEta = bestIDTrack->perigeeParameters()->eta();
	  m_momentum      = std::fabs(pT)*std::sqrt(1.+sinh(inDetEta)*sinh(inDetEta))/1000.;
	  if (m_momentum<m_MinMuonMom) continue;	  

	  ATH_MSG_DEBUG("charge: "<<m_charge<<", momentum: "<<m_momentum);
	  
	  m_d0       = bestIDTrack->perigeeParameters()->parameters()[Trk::d0];
	  m_z0       = bestIDTrack->perigeeParameters()->parameters()[Trk::z0];
	  m_phi0     = MuonAlign::RadsToDegrees(bestIDTrack->perigeeParameters()->parameters()[Trk::phi0]);
	  m_theta0   = MuonAlign::RadsToDegrees(bestIDTrack->perigeeParameters()->parameters()[Trk::theta]);
	  m_eta0     = bestIDTrack->perigeeParameters()->eta();
	}
	else {
	  ATH_MSG_DEBUG("no good ID track, continuing");
	  continue;
	}
      }

      if (mSeg) {

	// get a list of ID for muon hits on track
	std::list<Identifier> trackIds;
	getTrackIds(*track,trackIds);

	Identifier mSegId=chamberId(detType,*mSeg);
	if (find(trackIds.begin(),trackIds.end(),mSegId)==trackIds.end()) continue;
	
	Muon::MuonSegmentKey segKeys(*mSeg);

	// Collect hits from track
	ATH_MSG_DEBUG("getting hits from track");
	std::vector<const Trk::MeasurementBase*> measurements;	
	
	DataVector<const Trk::TrackStateOnSurface>::const_iterator it  = track->trackStateOnSurfaces()->begin();
	DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = track->trackStateOnSurfaces()->end();
	for ( ; it!=it_end; ++it) {
	  if ( (*it) != 0 ){
	    const Trk::MeasurementBase* rotTest = (*it)->measurementOnTrack();  
	    if (rotTest) measurements.push_back(rotTest);
	  }
	}
	ATH_MSG_DEBUG("have "<<measurements.size()<<" measurements");
	
	Muon::MuonSegmentKey trkKeys(measurements);
	
	std::vector< std::pair<unsigned int,int> > intersection;
	std::set_intersection(segKeys.keys().begin(),segKeys.keys().end(),
			      trkKeys.keys().begin(),trkKeys.keys().end(),
			      std::back_inserter(intersection));
	int intersectionSize = intersection.size();
	int segmentSize = segKeys.keys().size();
	
	// if subset, we're done
	if (segmentSize == intersectionSize) {
	  ATH_MSG_DEBUG("perfect match, returning track");
	  bestTrack=track;
	  break;
	}
	
	else if (intersectionSize>bestNMatches) {
	  bestNMatches=intersectionSize;
	  ATH_MSG_DEBUG("bestNMatches: "<<bestNMatches);
	  bestTrack=track;
	}
      }
      else 
	bestTrack=track;
    }
    return bestTrack;        
  }

  //________________________________________________________________________
  void UnbiasedMuonResiduals::getTrackIds(const Trk::Track& track, 
					  std::list<Identifier>& trackIds) const
  {
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it  = track.trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = track.trackStateOnSurfaces()->end();
    for ( ; it!=it_end; ++it) {
      const Trk::MeasurementBase*      mesb = (*it)->measurementOnTrack();	
      const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
      if (!rio && crio) rio=&crio->rioOnTrack(0);
      if (!rio) continue;
      
      Identifier id=rio->identify();
      if (find(trackIds.begin(),trackIds.end(),id)==trackIds.end()) 
	trackIds.push_back(id);      
    }
    return;
  }
  
} // end namespace
