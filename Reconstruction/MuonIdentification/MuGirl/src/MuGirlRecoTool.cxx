/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef HAVE_NEW_IOSTREAMS
    #include <iostream>
    #include<iomanip>
#else
    #include <iostream.h>
    #include<iomanip>
#endif

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"

#include "MuGirlRecoTool.h"

#include "MuGirlInterfaces/ICandidateTool.h"
#include "MuGirlInterfaces/IPerformanceTruthTool.h"
#include "MuGirlInterfaces/IGlobalFitTool.h"
#include "MuGirlInterfaces/IANNSelectionTool.h"
#include "MuGirlInterfaces/IMuGirlParticleCreatorTool.h"
#include "MuGirlInterfaces/IStauTool.h"
#include "MuGirlInterfaces/SegmentManager.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "RegionSelector/IRegSelSvc.h"

#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"

#include "TrkTrack/TrackCollection.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "ICaloTrkMuIdTools/ICaloMuonLikelihoodTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "MuonChamberT0s/ChamberT0s.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h" 

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "muonEvent/MuonContainer.h"
#include "DataModel/DataVector.h"
#include "TrkRoad/TrackRoad.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "MuonHoughPatternTools/MuonLayerHoughTool.h"

#include "MuonIdHelpers/MuonStationIndex.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "MuonCombinedEvent/MuGirlTag.h"
#include "MuonCombinedEvent/MuGirlLowBetaTag.h"

using namespace MuGirlNS;
using namespace std;

static bool compareByHits (const std::pair<double, TrigMuonEFInfoTrack*> &x,
                    const std::pair<double, TrigMuonEFInfoTrack*> &y)
{
    return x.first < y.first;
}

static bool compareHitsEqual (const std::pair<double, TrigMuonEFInfoTrack*> &x,
                       const std::pair<double, TrigMuonEFInfoTrack*> &y)
{
    return abs(x.first - y.first)<0.01;
}

MuGirlRecoTool::MuGirlRecoTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_applyLHR("On"),
    m_pMuonMgr(0),
    m_pSegmentManager(0),
    m_pInDetParticleContainer(0),
    m_pClusCollection(0),
    m_pRefittedTrkCollection(0),
    m_pRefittedTrkContainer(0),
    m_pMSRefittedTrkCollection(0),       // not used anymore
    m_pMSRefittedTrkContainer(0),        // not used anymore
    m_pSegmentCollection(0),
    m_chamberT0s(0),
    m_pCandNTuple(0),
    m_doLHR(true),
    m_doANNSelection(true),
    m_doGlobalFit(true),
    m_doParticleCreator(true),
    m_doStau(true),
    m_doConsistency(false),
    m_doSAFit(false),
    m_doMSRefit(true),
    m_doMuonFeature(false),
    m_doMuonBetaRefit(false),
    m_doHoughTransform(false),
    m_doNTuple(false),
    m_doTruth(false),
    m_writeChamberT0s(false),
    m_createTrackParticle(true),
    m_nIDhits(5),
    m_ptLowerLimit(3.0*CLHEP::GeV),
    m_stauPtCut(20000.0),
    m_mfPtCut(20000.0),
    m_roadDeltaPhi(0.2),
    m_barrelNNCut(0.4),
    m_endcapNNCut(0.4),
    m_idR(0.3),
    m_lhr(0.8),
    m_eOverP(0.3),
    m_ntupleName("/NTUPLES/MUGIRL/MUGIRL"),
    m_ntupleTitle("Muon Candidates"),
    m_sSegmentCollection("MuGirlSegments"),
    m_sRefittedTrkCollection("MuGirlRefittedTracks"),
    m_RefittedTrkLocation("MuGirlRefittedTrackParticles"),
    m_MSRefittedTrkCollection("MuGirlMuonSpectrometerTracks"),        // not used anymore
    m_MSRefittedTrkLocation("MuGirlMuonSpectrometerTrackParticles"),  // not used anymore
    m_inDetParticlesLocation("InDetTrackParticles"),                  // move to xAOD,  old key was "TrackParticleCandidate"
    m_chamberT0Location("MuGirlMuonChamberT0s"),
    m_CaloCollection("CaloCalTopoCluster"),
    m_pCandidate("MuGirlNS::CandidateTool"),
    m_pTruthTool("MuGirlNS::PerformanceTruthTool", 0),                // this is a public tool
    m_pGlobalFitTool("MuGirlNS::GlobalFitTool", 0),                   // this is a public tool
    m_pMSGlobalFitTool("MuGirlNS::GlobalFitTool", 0),                 // this is a public tool
    m_pANNSelectionTool("MuGirlNS::ANNSelectionTool", 0),             // this is a public tool
    m_pParticleCreatorTool("MuGirlNS::MuGirlParticleCreatorTool", 0), // this is a public tool
    m_pStauTool("MuGirlNS::StauTool", 0),                             // this is a public tool
    m_pToCalo("ExtrapolateToCaloTool/ExtrapolateToCaloTool"),
    m_pTrackExtrapolationTool("Muon::MuonTrackExtrapolationTool/MuonTrackExtrapolationTool"),
    m_pMuLHR("CaloMuonLikelihoodTool"),
    m_particleCreatorTool("Trk::TrackParticleCreatorTool/MuonCombinedTrackParticleCreator"), //MuidParticleCreatorTool"),
    m_pMuonLayerHoughTool("Muon::MuonLayerHoughTool/MuonLayerHoughTool"),
    m_pIdTrackSelectorTool("MuonCombinedInDetDetailedTrackSelectorTool"), //this is a public tool
    m_MuonIdHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_magFieldSvc("MagField::AtlasFieldSvc/AtlasFieldSvc",name)
{

    declareInterface<MuGirlNS::IMuGirlRecoTool>(this);

    // Steering properties
    declareProperty("doLHR",                        m_doLHR);
    declareProperty("doANNSelection",               m_doANNSelection); 
    declareProperty("doGlobalFit",                  m_doGlobalFit);
    declareProperty("doParticleCreator",            m_doParticleCreator);
    declareProperty("doNTuple",                     m_doNTuple);
    declareProperty("doStau",                       m_doStau);
    declareProperty("doConsistency",                m_doConsistency);
    declareProperty("doSAFit",                      m_doSAFit);
    declareProperty("doMSRefit",                    m_doMSRefit);
    declareProperty("doMuonFeature",                m_doMuonFeature);
    declareProperty("doMuonBetaRefit",              m_doMuonBetaRefit);
    declareProperty("doNTuple",                     m_doNTuple);
    declareProperty("doTruth",                      m_doTruth);
    declareProperty("doHoughTransform",             m_doHoughTransform);
    declareProperty("writeChamberT0s",              m_writeChamberT0s);
    declareProperty("createTrackParticle",          m_createTrackParticle);


    // Variables and threshold cuts
    declareProperty("nIDhits",                      m_nIDhits);
    declareProperty("PtLowerLimit",                 m_ptLowerLimit);
    declareProperty("StauPtCut",                    m_stauPtCut);
    declareProperty("MFPtCut",                      m_mfPtCut);
    declareProperty("RoadDeltaPhi",                 m_roadDeltaPhi);
    declareProperty("BarrelNNCut",                  m_barrelNNCut);
    declareProperty("EndcapNNCut",                  m_endcapNNCut);
    declareProperty("IdConeR",                      m_idR);
    declareProperty("LHRValue",                     m_lhr);
    declareProperty("eCaloOverPCut",                m_eOverP);


    // Container and collection names
    declareProperty("NtupleName",                   m_ntupleName);
    declareProperty("NtupleTitle",                  m_ntupleTitle);
    declareProperty("SegmentCollection",            m_sSegmentCollection);
    declareProperty("RefittedTrkCollection",        m_sRefittedTrkCollection);
    declareProperty("RefittedTrkLocation",          m_RefittedTrkLocation);
    declareProperty("MSRefittedTrkCollection",      m_MSRefittedTrkCollection);
    declareProperty("MSRefittedTrkLocation",        m_MSRefittedTrkLocation);
    declareProperty("InDetTrackParticlesLocation",  m_inDetParticlesLocation);
    declareProperty("ChamberT0Location",            m_chamberT0Location);
    declareProperty("InCaloCollection",             m_CaloCollection);


    // Tools
    declareProperty("MuGirlCandidate",              m_pCandidate);
    declareProperty("PerformanceTruthTool",         m_pTruthTool);
    declareProperty("MuGirlGlobalFitTool",          m_pGlobalFitTool);
    declareProperty("MuGirlMSGlobalFitTool",        m_pMSGlobalFitTool);
    declareProperty("ANNSelectionTool",             m_pANNSelectionTool);
    declareProperty("MuGirlParticleCreatorTool",    m_pParticleCreatorTool);
    declareProperty("MuGirlStauTool",               m_pStauTool);
    declareProperty("ExtrapolateToCaloTool",        m_pToCalo);
    declareProperty("MuonTrackExtrapolator",        m_pTrackExtrapolationTool);
    declareProperty("CaloMuonLikelihoodTool",       m_pMuLHR);
    declareProperty("ParticleCreatorTool",          m_particleCreatorTool);
    declareProperty("MuonLayerHoughTool",           m_pMuonLayerHoughTool);
    declareProperty("IdTrackSelectorTool",          m_pIdTrackSelectorTool);
}

MuGirlRecoTool::~MuGirlRecoTool()
{
    delete m_pSegmentManager;
}

void
MuGirlRecoTool::print_configuration() const {
    ATH_MSG_INFO("Steering configuration:");
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doLHR.name() << m_doLHR.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doANNSelection.name() << m_doANNSelection.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doGlobalFit.name() << m_doGlobalFit.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doParticleCreator.name() << m_doParticleCreator.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doNTuple.name() << m_doNTuple.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doStau.name() << m_doStau.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doConsistency.name() << m_doConsistency.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doSAFit.name() << m_doSAFit.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doMSRefit.name() << m_doMSRefit.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doMuonFeature.name() << m_doMuonFeature.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doMuonBetaRefit.name() << m_doMuonBetaRefit.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doNTuple.name() << m_doNTuple.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doTruth.name() << m_doTruth.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doHoughTransform.name() << m_doHoughTransform.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_writeChamberT0s.name() << m_writeChamberT0s.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_createTrackParticle.name() << m_createTrackParticle.value());
    ATH_MSG_INFO("Thresholds and cuts configuration:");
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_nIDhits.name() << m_nIDhits.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_ptLowerLimit.name() << m_ptLowerLimit.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_stauPtCut.name() << m_stauPtCut.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_mfPtCut.name() << m_mfPtCut.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_roadDeltaPhi.name() << m_roadDeltaPhi.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_barrelNNCut.name() << m_barrelNNCut.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_endcapNNCut.name() << m_endcapNNCut.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_idR.name() << m_idR.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_lhr.name() << m_lhr.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_eOverP.name() << m_eOverP.value());
    ATH_MSG_INFO("Containers and Collections name:");
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_ntupleName.name() << m_ntupleName.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_ntupleTitle.name() << m_ntupleTitle.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_sSegmentCollection.name() << m_sSegmentCollection.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_sRefittedTrkCollection.name() << m_sRefittedTrkCollection.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_RefittedTrkLocation.name() << m_RefittedTrkLocation.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_MSRefittedTrkCollection.name() << m_MSRefittedTrkCollection.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_MSRefittedTrkLocation.name() << m_MSRefittedTrkLocation.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_inDetParticlesLocation.name() << m_inDetParticlesLocation.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_chamberT0Location.name() << m_chamberT0Location.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_CaloCollection.name() << m_CaloCollection.value());
    ATH_MSG_INFO("Tools:");
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pCandidate.name() << m_pCandidate);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pTruthTool.name() << m_pTruthTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pGlobalFitTool.name() << m_pGlobalFitTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pMSGlobalFitTool.name() << m_pMSGlobalFitTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pANNSelectionTool.name() << m_pANNSelectionTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pParticleCreatorTool.name() << m_pParticleCreatorTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pStauTool.name() << m_pStauTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pToCalo.name() << m_pToCalo);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pTrackExtrapolationTool.name() << m_pTrackExtrapolationTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pMuLHR.name() << m_pMuLHR);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_particleCreatorTool.name() << m_particleCreatorTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pMuonLayerHoughTool.name() << m_pMuonLayerHoughTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pIdTrackSelectorTool.name() << m_pIdTrackSelectorTool);
}

StatusCode MuGirlRecoTool::initialize()
{

    ATH_MSG_INFO("Initializing MuGirlRecoTool - package version " << PACKAGE_VERSION);
    print_configuration();

    m_pSegmentManager = new MuGirlNS::SegmentManager;

    if (detStore()->retrieve(m_pMuonMgr).isFailure() || m_pMuonMgr == NULL)
    {
        ATH_MSG_ERROR("initialize: cannot retrieve MuonDetectorManager");
        return StatusCode::RECOVERABLE;
    }

    // Initialize tools
    ATH_CHECK(m_pIdTrackSelectorTool.retrieve());    

    if (m_pToCalo.retrieve().isFailure()) {
        ATH_MSG_WARNING( "Cannot retrieve the Calo Extrapolation tool: " << m_pToCalo << ",  setting it to NULL");
        ATH_MSG_WARNING( "MuGirlrecoTool cannot extrapolate the ID track to the Calo layers");
        m_pToCalo = NULL;
    }
    if (m_pTrackExtrapolationTool.retrieve().isFailure()) {
        ATH_MSG_WARNING( "Cannot retrieve the Calo Extrapolation tool: " << m_pTrackExtrapolationTool << ",  setting it to NULL");
        ATH_MSG_WARNING( "MuGirlrecoTool cannot extrapolate the ID track to the Muon Spetrometer entrance");
        m_pTrackExtrapolationTool = NULL;
    }
    if (m_doTruth)
    {
        if (m_pTruthTool.retrieve().isFailure())
            return StatusCode::RECOVERABLE;
    }
    if (m_doLHR)
    {
        if (m_pMuLHR.retrieve().isFailure())
            m_pMuLHR = NULL;
    }
    if (m_doStau)
    {
	if (m_pStauTool.retrieve().isFailure())
            return StatusCode::RECOVERABLE;
        m_pStauTool->setSegmentManager(m_pSegmentManager);
//        bool initFit = m_doGlobalFit || m_doSAFit;
        m_pStauTool->initTool(m_doGlobalFit);
    }
    if (m_doGlobalFit)
    {
        if (m_pGlobalFitTool.retrieve().isFailure())
            m_pGlobalFitTool = NULL;
    }
    if (m_doMSRefit)
      {
        if (m_pMSGlobalFitTool.retrieve().isFailure())
            m_pMSGlobalFitTool = NULL;
      }
    if (m_doANNSelection)
    {
        if (m_pANNSelectionTool.retrieve().isFailure())
            m_pANNSelectionTool = NULL;
    }
    if (m_doParticleCreator)
    {
        if (m_pParticleCreatorTool.retrieve().isFailure())
            m_pParticleCreatorTool = NULL;
    }
    if (m_doMuonFeature && m_doSAFit )
    {
      bool initFit = m_doGlobalFit || m_doSAFit;
      m_pStauTool->initTool(initFit);
    }

    m_caloLayers.push_back( CaloCell_ID::PreSamplerB );
    m_caloLayers.push_back( CaloCell_ID::EMB1 );
    m_caloLayers.push_back( CaloCell_ID::EMB2 );
    m_caloLayers.push_back( CaloCell_ID::EMB3 );
    m_caloLayers.push_back( CaloCell_ID::TileBar0 );
    m_caloLayers.push_back( CaloCell_ID::TileBar1 );
    m_caloLayers.push_back( CaloCell_ID::TileBar2 );
    m_caloLayers.push_back( CaloCell_ID::PreSamplerE );
    m_caloLayers.push_back( CaloCell_ID::EME1 );
    m_caloLayers.push_back( CaloCell_ID::EME2 );
    m_caloLayers.push_back( CaloCell_ID::EME3 );
    m_caloLayers.push_back( CaloCell_ID::HEC0 );
    m_caloLayers.push_back( CaloCell_ID::HEC1 );
    m_caloLayers.push_back( CaloCell_ID::HEC2 );
    m_caloLayers.push_back( CaloCell_ID::FCAL0 );
    m_caloLayers.push_back( CaloCell_ID::FCAL1 );
    m_caloLayers.push_back( CaloCell_ID::FCAL2 );
    m_caloLayers.push_back( CaloCell_ID::TileExt0 );
    m_caloLayers.push_back( CaloCell_ID::TileExt1 );
    m_caloLayers.push_back( CaloCell_ID::TileExt2 );

    if (m_pCandidate.retrieve().isFailure())
        return StatusCode::RECOVERABLE;
    m_pCandidate->setSegmentManager(m_pSegmentManager);

    // Initialize NTuple
    if (m_doNTuple)
    {
        ServiceHandle<INTupleSvc> ntupleSvc("NTupleSvc/NTupleSvc",this->name());
        if (ntupleSvc.retrieve().isFailure()) {
            ATH_MSG_WARNING("initialize: cannot retrieve the NTuple Svc. MUgirlNTuple will not be initialized!");
        } else {
            m_pCandNTuple = ntupleSvc->book(m_ntupleName, CLID_ColumnWiseTuple, m_ntupleTitle);
            if (m_pCandNTuple == NULL)
            {
                ATH_MSG_ERROR("initialize: could not create NTuple " << m_ntupleName);
                return StatusCode::RECOVERABLE;
            }
            if (m_pCandidate->bookNTuple(m_pCandNTuple).isFailure())
            {
                ATH_MSG_ERROR("initialize: could not book Candidate NTuple " << m_ntupleName);
                return StatusCode::RECOVERABLE;
            }

            if (m_doTruth&& m_pTruthTool->bookNTuple(m_pCandNTuple).isFailure())
            {
                ATH_MSG_ERROR("initialize: could not book Truth NTuple " << m_ntupleName);
                return StatusCode::RECOVERABLE;
            }
            if (m_doGlobalFit&& m_pGlobalFitTool->bookNTuple(m_pCandNTuple,"").isFailure())
            {
                ATH_MSG_ERROR("initialize: could not book GlobalFit NTuple " << m_ntupleName);
                return StatusCode::RECOVERABLE;
            }
            if (m_doGlobalFit&& m_pMSGlobalFitTool->bookNTuple(m_pCandNTuple,"MS").isFailure())
            {
                ATH_MSG_ERROR("initialize: could not book MS GlobalFit NTuple " << m_ntupleName); 
                return StatusCode::RECOVERABLE;
            }
            if (m_doStau)
            {
                if(m_pStauTool->bookNTuple(m_pCandNTuple).isFailure())
                {
                    ATH_MSG_ERROR("initialize: could not book Stau NTuple " << m_ntupleName);
                    return StatusCode::RECOVERABLE;
                }
            }
        } // end of ntuple booking
    } // end of NTuple initialization

    if (m_magFieldSvc.retrieve().isFailure())
    {
        ATH_MSG_ERROR("initialize: could not retrieve magFieldSvc");
        return StatusCode::RECOVERABLE;
    }

    if (m_particleCreatorTool.retrieve().isFailure())
        return StatusCode::RECOVERABLE;

    if (m_doHoughTransform) {
        if ( m_pMuonLayerHoughTool.retrieve().isFailure() ) {
            ATH_MSG_WARNING("initialize: could not retrieve the MuonLayerHoughTool");
            ATH_MSG_WARNING("initialize: info from the Hough transform will not be in the MuGirl ntuple");
            return StatusCode::RECOVERABLE;
        } else {
            ATH_MSG_INFO("MuonLayerHoughTool successfully retrieved");
        }
    }

    if (m_pIdTrackSelectorTool.retrieve().isFailure()) 
        return StatusCode::RECOVERABLE; 
  
     if ( m_MuonIdHelperTool.retrieve().isFailure() )
     {
         ATH_MSG_WARNING("initialize: failed to retrieve tool " << m_MuonIdHelperTool);
         return StatusCode::RECOVERABLE;
     }
     ATH_MSG_INFO("Retrieved tool " << m_MuonIdHelperTool);

    m_pCandidate->setT0Map(&m_T0map);

    InterfaceID id = IMuGirlRecoTool::interfaceID();

    ATH_MSG_INFO("Interface id=" << id.id() << ",  majorVersion=" << id.majorVersion() << ",  minorVersion=" << id.minorVersion() );

    //if (this->queryInterface(id,(void**)this).isSuccess()) {
    //    ATH_MSG_INFO("Success!!");
    //}

    this->clear_hough_data();

    ATH_MSG_INFO("MuGirlrecoTool initialized successfully");
    
    return StatusCode::SUCCESS;
}

StatusCode MuGirlRecoTool::MuGirlReco( const InDetCandidateCollection& InDetSeeds ) {

    ATH_MSG_DEBUG("MuGirlReco( .. ) executing.");

    StatusCode sc;

    /** Stau Collection */
    // ???  if(m_doStau) m_pStauTool->recordCollection();


    /** Hough Data processing */
    if ( m_doHoughTransform && this->processHoughData().isFailure() ) {
      ATH_MSG_INFO("Hough data cannot be processed successfully, cleaning Hough data structure ....");
      this->clear_hough_data();
    }


    /** Retrieve the Container of the primary candidates.  */
    const VxContainer* vxCandidatesContainer = 0;
    if (evtStore()->contains<VxContainer>("VxPrimaryCandidate"))
    {
        sc = evtStore()->retrieve(vxCandidatesContainer, "VxPrimaryCandidate");
        if (sc.isFailure())
        {
            ATH_MSG_WARNING("MuGirlReco: cannot retrieve VxPrimaryCandidate");
        }
    }
    else    ATH_MSG_INFO("MuGirlReco: VxPrimaryCandidate not in StoreGate");


    /**
     * Try to retrieve the EventInfo object for getting the run number and the event number
     */
    const EventInfo* pEventInfo = NULL;
    sc = evtStore()->retrieve(pEventInfo);
    if (sc.isFailure() || pEventInfo == NULL)
        ATH_MSG_WARNING("MuGirlReco: cannot retrieve EventInfo");


    /** Retrieve the CaloCluster container */
    if (evtStore()->contains<xAOD::CaloClusterContainer>(m_CaloCollection))
    {
        sc = evtStore()->retrieve(m_pClusCollection, m_CaloCollection);
        if (sc.isFailure() || m_pClusCollection == NULL)
        {
            ATH_MSG_WARNING("MuGirlReco: cannot retrieve ClusterContainer");
        }
        else
            ATH_MSG_DEBUG("Calo Collection Size: "<<m_pClusCollection->size());
    }


    /** Register into Storegate the MuGirl Segment Collection:
      * check if a collection is already existing and clear it.
      */
    if (evtStore()->contains<Trk::SegmentCollection>(m_sSegmentCollection)) {
        const Trk::SegmentCollection* segments;
        if (StatusCode::SUCCESS == evtStore()->retrieve(segments,m_sSegmentCollection))
            if (StatusCode::SUCCESS == evtStore()->remove(segments))
                ATH_MSG_DEBUG("Removed pre-existing track collection");
    }

    m_pSegmentCollection = new Trk::SegmentCollection();
    sc = evtStore()->record(m_pSegmentCollection, m_sSegmentCollection);
    if (sc.isFailure())
    {
        ATH_MSG_WARNING("execute: cannot record " << m_sSegmentCollection);
        return StatusCode::SUCCESS;
    }


    /** Terminate the processing if the Solenoid is off */
    if (!m_magFieldSvc->solenoidOn())
    {
        ATH_MSG_DEBUG("Solenoid is off: Execute done");
        sc=StatusCode::SUCCESS;
        return sc;
    }



    /** Create the CaloParticles */
    if (msgLvl(MSG::DEBUG))
    {
        ATH_MSG_DEBUG("******************************************");
        ATH_MSG_DEBUG("********  list of TrackParticles  ********");
        ATH_MSG_DEBUG("******************************************");

        for (InDetCandidateCollection::const_iterator ipIt = InDetSeeds.begin();
                    ipIt != InDetSeeds.end();
                    ++ipIt)
        {
            const xAOD::TrackParticle& tp_id = (*ipIt)->indetTrackParticle();
            std::cout << "TrackParticle = " << &tp_id << std::endl;
            const Trk::Perigee pPerigee = tp_id.perigeeParameters();
	    double charge_id = tp_id.charge();
            double d0_id     = pPerigee.parameters()[Trk::d0];
            double z0_id     = pPerigee.parameters()[Trk::z0];
            double phi_id    = pPerigee.parameters()[Trk::phi];
            double eta_id    = asinh(1./tan(pPerigee.parameters()[Trk::theta]));
            double pt_id     = pPerigee.pT();

            if (pt_id < m_ptLowerLimit ) continue;

            ATH_MSG_DEBUG("pt =  " << pt_id  <<
                         " eta = " << eta_id <<
                         " phi = " << phi_id <<
                          " ch = " << charge_id <<
                          " d0 = " << d0_id <<
                          " z0 = " << z0_id );
        }

        ATH_MSG_DEBUG("******************************************");
        ATH_MSG_DEBUG("********           end           *********");
        ATH_MSG_DEBUG("******************************************");
    }

    Amg::Vector3D pt_calo_local(0,0,0);
    int iPart = 0;
    for (InDetCandidateCollection::const_iterator ipIt = InDetSeeds.begin();
            ipIt != InDetSeeds.end();
            ++ipIt, iPart++)
    {
        const xAOD::TrackParticle& tp_id = (*ipIt)->indetTrackParticle();
        const Trk::Perigee pPerigee = tp_id.perigeeParameters();

        if (!m_pIdTrackSelectorTool->decision(tp_id))
        {
            ATH_MSG_DEBUG("Failed IdTrackSelectorTool decision");
            continue;
        }

        /**
          * Check also the track pt as the pt cut of the TrackSelectorTool may be different.
          */
        double pt = pPerigee.pT();
        if (pt < m_ptLowerLimit)
        {
            ATH_MSG_DEBUG("pT=" << pt << " is lower than limit = " << m_ptLowerLimit << ". Skipping track.");
            continue;
        }


        bool hasBeenExtrapolated = false;
        const Trk::TrackParameters* extrParameters = NULL;

        /**
          * If the ID candidate has been extrapolated to the MuonEntry use that extrapolation
          */
        const Rec::ParticleExtrapolationVector* extrapolations = (*ipIt)->particleExtrapolationVector();
        Rec::ParticleExtrapolationVector::const_iterator exIt;
        for(exIt = extrapolations->begin(); exIt!=extrapolations->end(); exIt++) {
            if ( (*exIt).second==RecIntersect::MSEntrance ) {
                hasBeenExtrapolated = true;
                pt_calo_local  = (*exIt).first->position();
                extrParameters = (*exIt).first;
                ATH_MSG_VERBOSE( "ID track already extrapolated to MS entrance, printing the track position at Muon Entry surface:" );
                break;
            }
        }


        /**
          * Otherwise try to extrapolate with the MuonTrackExtrapolator Tool
          */
        if (!hasBeenExtrapolated) {
            /**
              * Check the perigeee parameter of the track to be extrapolated
              */
            if (msgLvl(MSG::VERBOSE)) {
                ATH_MSG_VERBOSE( "extrapolationg to muon entry layer, printing the track perigee:" );
                ATH_MSG_VERBOSE( pPerigee );
            }

            // Remeber to increase the robustness against missingtrack extrapolation once the TrackExtrapolatorTool will be fixed.
            Trk::Track* muon_entry_track = 0;
            // Temporary remove the TrackExtrapolationTool ... => m_pTrackExtrapolationTool->extrapolate( *(tp_id.track()) );
            if (muon_entry_track != 0) {
                hasBeenExtrapolated = true;
                const Trk::Perigee* extPerigee = muon_entry_track->perigeeParameters();
                extrParameters = dynamic_cast<const Trk::TrackParameters*>( extPerigee );
                if (extPerigee==NULL) {
                    ATH_MSG_WARNING("Cannot cast the perigee at the muon entry level to a Trk::MeasuredPerigee. Skiping the track!");
                    continue;
                }
                if (msgLvl(MSG::VERBOSE)) {
                    ATH_MSG_VERBOSE( "Track position at the muon entry layer:" );
                    const DataVector<const Trk::TrackParameters>* track_parameters = muon_entry_track->trackParameters();
                    for (unsigned int i=0; i<track_parameters->size();i++) {
                        ATH_MSG_VERBOSE( *(track_parameters->at(i)) );
                    }
                    ATH_MSG_VERBOSE("Perigee parameter of the extrapolated track:");
                    ATH_MSG_VERBOSE( *(extPerigee) );
                }

                Amg::Vector3D ip_mom = pPerigee.momentum();
                pt_calo_local        = extPerigee->position();
                Amg::Vector3D mu_mom = extPerigee->momentum();

                double ip_pt = ip_mom.perp();
                double mu_pt = mu_mom.perp();
                double ip_p  = ip_mom.mag();
                double mu_p  = mu_mom.mag();
                double muon_entry_radius = sqrt(pt_calo_local.x()*pt_calo_local.x() + pt_calo_local.y()*pt_calo_local.y());

                ATH_MSG_DEBUG("MuonEntryExtrapolator: track extrapolated to r=" <<  muon_entry_radius << ", z=" << pt_calo_local.z());
                ATH_MSG_DEBUG("MuonEntryExtrapolator: momentum before extr. p  = " << ip_p  << ", after extr. p  = " << mu_p);
                ATH_MSG_DEBUG("MuonEntryExtrapolator: momentum before extr. pt = " << ip_pt << ", after extr. pt = " << mu_pt);

            }

        }


        bool seenByCalo = hasBeenExtrapolated;
        if ( !seenByCalo) {
            if (m_pToCalo)
            {
                double offset = 0.;
                const Trk::TrackParameters* calo_local = 0;
                for (unsigned i = 0; i < m_caloLayers.size(); ++i)
                {
                    if ( (calo_local = m_pToCalo->extrapolate(tp_id, m_caloLayers[i], offset, Trk::alongMomentum, Trk::muon))!=0 ) {
                        seenByCalo=true;
                        pt_calo_local = calo_local->position();
                    }
                    if (seenByCalo)
                        break;
                }
            }
            else seenByCalo = true;
        }

        if (!seenByCalo)
        {
            ATH_MSG_DEBUG("Not seen by calo");
            continue;
        }

        ATH_MSG_DEBUG("This TrackParticle is seen by Calo!");

        CaloParticle* pParticle = new CaloParticle((*ipIt),&tp_id, extrParameters, pt_calo_local.eta(), pt_calo_local.phi());
        m_caloParticles.push_back(pParticle);
    }

    /** Run the MuGirl reconstruction */
    MuGirlNS::CandidateSummaryList summaryList;
    this->RunFromID(pEventInfo, vxCandidatesContainer, summaryList, true);


    ATH_MSG_DEBUG("MuGirlReco: number of summaries=" << summaryList.size());

    if (msgLvl(MSG::DEBUG))
    {
        for (MuGirlNS::CandidateSummaryList::iterator it  = summaryList.begin();
                                   it != summaryList.end();
                                   it++)
        {
            const Trk::Track* refitted = (*it)->pTrkRefitted;
            if (refitted==NULL) {
                ATH_MSG_WARNING("MuGirlReco: pointer to the RefittedTrack is NULL");
            }
        }
    }

    /** in order to fill the ntuple storing the MuGirl results */
    if (m_pParticleCreatorTool->fillContainer(&summaryList,true).isFailure())
    {
        ATH_MSG_DEBUG("MuGirlReco: m_pParticleCreatorTool->fillContainer failed.");
    }


    /** record and clear the T0 segments */
    if (m_writeChamberT0s) recordChamberT0s();
    m_T0map.clear();

    /** empty caloParticles */
    for (unsigned i = 0; i < m_caloParticles.size(); i++)
    {
        delete m_caloParticles[i];
    }
    m_caloParticles.clear();

    /** Clear the Summary List */
    for (MuGirlNS::CandidateSummaryList::iterator itSummary = summaryList.begin(); itSummary !=summaryList.end(); itSummary++)
    {
        delete *itSummary;
    }
    summaryList.clear();

    /** Clear the temporary data after the reconstruction */
    m_pCandidate->candidateClear();
    m_pSegmentManager->clear();

    /** clear Hough data */
    if (m_doHoughTransform) {
        ATH_MSG_DEBUG("After MuGirlReco n. " << this->hough_data_size() << " Hough maxima are unassociated");

            if ( this->hough_data_size() && m_doNTuple ) {
            /** put dummy candidate in the ntuple in order to deliver the Hough maxima which has been left unassociated */
                if (m_pCandidate->fillNTuple(m_hough_data).isFailure())
                    ATH_MSG_WARNING("execute: cannot fill the unassociated maxima in the NTuple");
            }

            this->clear_hough_data();
    }


    ATH_MSG_DEBUG("MuGirlReco( .. ) done.");

    return StatusCode::SUCCESS;
}

bool MuGirlRecoTool::MuonBetaRefit(CandidateSummaryList& summaryList) {

    /** Initialize variables for containers and collections */
    Analysis::MuonContainer* pLowBetaMuonContainer = NULL;
    TrackCollection* pLowBetaTracks  = NULL;
    StatusCode sc;
    int itrk = 0;
    
    /** Check if Storegate has the LowBetaMuonContainer; if not returns false and trigger
     *  the standard processing from the ID seed
     */
    if (!evtStore()->contains<Analysis::MuonContainer>("LowBetaMuonContainer")) 
    {
       ATH_MSG_WARNING("MuonBetaRefit: no LowBetaMuonContainer. Moving to ID processing");
    } 
    else 
    { 
       sc = evtStore()->retrieve(pLowBetaMuonContainer,"LowBetaMuonContainer");

       if (pLowBetaMuonContainer ==NULL) 
       {
           ATH_MSG_WARNING("MuonBetaRefit: LowBetaMuonContainer is NULL. Moving to ID processing");
       } 
       else ATH_MSG_DEBUG("LowBetaMuonContainer size"<<pLowBetaMuonContainer->size());
   }
   
   if  (!evtStore()->contains<TrackCollection>("LowBetaMSMuonTracks"))
   {
       ATH_MSG_WARNING("MuonBetaRefit: no LowBetaMSMuonTracks. Moving to ID processing");
       return false;
   }
   sc = evtStore()->retrieve(pLowBetaTracks,"LowBetaMSMuonTracks");

   if (pLowBetaTracks ==NULL)
   {
       ATH_MSG_WARNING("MuonBetarefit: LowBetaMSMuonTracks is NULL. Moving to ID processing");
       return false;
   }
   ATH_MSG_DEBUG("LowBetaMSMuonTracks size" << pLowBetaTracks->size());


   /** loop over the low beta track candidates */
   for (TrackCollection::const_iterator ipIt = pLowBetaTracks->begin();
       ipIt != pLowBetaTracks->end();
      ++ipIt)
   {
        MuGirlNS::MdtSegmentMakerInfoList mdtSegmentMakerInfoList;
        MuGirlNS::RIO_OnTrackLists rpcHitsOnTrack;
        MuGirlNS::RIO_OnTrackLists tgcHitsOnTrack;
        int nrpchits  = 0;
        MuGirlNS::RIO_OnTrackList* rpcList = new MuGirlNS::RIO_OnTrackList();
        MuGirlNS::RIO_OnTrackList* tgcList = new MuGirlNS::RIO_OnTrackList();
        std::vector<const Muon::MdtDriftCircleOnTrack*>  mdtList;
        std::vector<std::vector<const Muon::MdtDriftCircleOnTrack*> > mdtLists;


        const Trk::Track* lowBetaTrack = *ipIt;
        ElementLink<TrackCollection> t_link(*pLowBetaTracks,  ipIt - pLowBetaTracks->begin());

       // do BackExtrapolation
        ATH_MSG_DEBUG("MuonBetaRefit:: doing  backExtrapolation "); 
        const Trk::Track* lowBetaExtrp =   m_pGlobalFitTool->backExtrapolation(*lowBetaTrack);
        //if (lowBetaExtrp == NULL) continue;


        double mu_beta = 0.0;
        for (Analysis::MuonContainer::const_iterator mu_it = pLowBetaMuonContainer->begin();
             mu_it != pLowBetaMuonContainer->end(); ++mu_it)
        {
             const Analysis::Muon* muon = *mu_it;
             const Trk::Track* tt = (muon->muonSpectrometerTrackParticle())->originalTrack();
             if (tt == lowBetaTrack)
             {
                
                ATH_MSG_DEBUG("MuonBetaRefit:: Muon beta == "<< muon->beta());
                mu_beta = muon->beta();
                break; 
             } 
        }
        const xAOD::Vertex* pVxCandidate = NULL;
        xAOD::TrackParticle* tp = NULL;
        if (t_link.isValid()) {
            /** Subordinate TrackParticle creation to boolean configuration switch. */
            if (m_createTrackParticle) tp = m_particleCreatorTool->createParticle(t_link, m_pRefittedTrkContainer, 
                                                       pVxCandidate, xAOD::muon);
        } else {
            ATH_MSG_WARNING("MuonBetaRefit: Element link to the MuonBetaRefit track is not valid; new TrackParticle is not created");
        }

        if (m_pCandidate->fillBetaRefitTrack(itrk, tp, lowBetaTrack, true, false).isFailure())
        {
            ATH_MSG_DEBUG("MuonBetaRefit Cannot fill CANDIDATE");
            continue;
        }
        MuGirlNS::CandidateSummary* summary_beta = new MuGirlNS::CandidateSummary();
        if (m_pCandidate->fillSummary(summary_beta).isFailure())
        {
            ATH_MSG_DEBUG("MuonBetaRefit Cannot fill SUMMARY");
            continue;
        }

        if (lowBetaExtrp != NULL)
        {
           xAOD::TrackParticle* extrp_tp = 0;
           if (m_createTrackParticle) {  //remove TrackParticle creation and storing
              m_pRefittedTrkCollection->push_back(const_cast<Trk::Track*> (lowBetaExtrp));
              ElementLink<TrackCollection> extr_t_link(*m_pRefittedTrkCollection, m_pRefittedTrkCollection->size()-1);
              if (extr_t_link.isValid()) {
                  extrp_tp = m_particleCreatorTool->createParticle(extr_t_link,
                                                    m_pRefittedTrkContainer, pVxCandidate, xAOD::muon);
              } else {
                  ATH_MSG_WARNING("MuonBetaRefit: Element link to the extrapolated MuonBetaRefit track is not valid; new TrackParticle is not created");
              }
           }
           summary_beta->pLowBetaExtrpTrack = extrp_tp;  
           summary_beta->pTrkLowBetaExtr    = lowBetaExtrp;
        }         
        std::vector<std::pair<Trk::TrackRoad*, MuGirlNS::RIO_OnTrackList* > > vlist;

        const DataVector<const Trk::TrackStateOnSurface>* pTSoSs = lowBetaTrack->trackStateOnSurfaces();
        ATH_MSG_DEBUG("MuonBetaRefitTool:: Total Number of hits  "<<pTSoSs->size());
        if (pTSoSs == NULL)
        { 
           ATH_MSG_DEBUG("MuonBetaRefitTool:: TSoSs is NULL  ");
           continue;
        } 
        for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it = pTSoSs->begin();
        it != pTSoSs->end(); ++it)
        {   
           const Trk::TrackStateOnSurface* pTSoS = *it;
           if (pTSoS->type(Trk::TrackStateOnSurface::Outlier)) 
           { 
               ATH_MSG_DEBUG("MuonBetaRefitTool:: TSOS is outlier ");
               continue;
           }
           const Trk::MeasurementBase* hit = pTSoS->measurementOnTrack();
           if (hit == NULL) continue;
           const Trk::RIO_OnTrack *rot = dynamic_cast<const Trk::RIO_OnTrack *>(hit);
           if (!rot)
            { 
               ATH_MSG_DEBUG("MuonBetaRefitTool:: RIOOnTrack cast did not succeed ");
               const Trk::CompetingRIOsOnTrack *testcrot=dynamic_cast<const Trk::CompetingRIOsOnTrack *>(hit); 
               if (!testcrot)
               { 
                  ATH_MSG_DEBUG("MuonBetaRefitTool:: CompetingOnTrack cast did not succeed ");
                 continue;
               } 
               else
               {
                   int nrot = testcrot->numberOfContainedROTs();
                   for (int irot = 0;irot<nrot;irot++)
                   {
                       const Trk::RIO_OnTrack *clus = &testcrot->rioOnTrack(irot);
                       if (!clus) continue;
                       const Identifier& id = clus->identify();
                       if (m_MuonIdHelperTool->isRpc(id))
                       { 
                          ATH_MSG_DEBUG("MuonBetaRefitTool:: found Rpc hit ");
                          ++nrpchits;
                          rpcList->push_back(clus);
                       }
                       if (m_MuonIdHelperTool->isTgc(id)) 
                       {
                          tgcList->push_back(clus);
                          ATH_MSG_VERBOSE("MuonBetaRefitTool:: found Tgc hit ");
                       }
                   }
              }  
            }
            else
            { 
            const Identifier& id = rot->identify();
 
            if (m_MuonIdHelperTool->isMdt(id)) 
            {
                ATH_MSG_VERBOSE("MuonBetaRefitTool:: found Mdt hit ");
                const Muon::MdtDriftCircleOnTrack* mdcot = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(hit);
                if (mdcot == NULL) continue;
                mdtList.push_back(mdcot->clone());
                mdtLists.push_back(mdtList);
                const Trk::TrackParameters* trkParam = pTSoS->trackParameters();
                const Amg::Vector3D& pos = trkParam->position();
                const Amg::Vector3D& mom = trkParam->momentum();
                const Amg::Vector3D& dir = mom/trkParam->momentum().mag();
                Trk::TrackRoad *road = new Trk::TrackRoad(pos,dir,0.01,0.02);
                if (road == NULL) continue;
                MuGirlNS::MdtSegmentMakerInfo* pMdtSegmentMakerInfo = new MuGirlNS::MdtSegmentMakerInfo(road,mdtLists);
                mdtSegmentMakerInfoList.push_back(pMdtSegmentMakerInfo);
            }
            }
        } 
        rpcHitsOnTrack.push_back(rpcList);
        tgcHitsOnTrack.push_back(tgcList);

         
        Amg::Vector3D v(lowBetaTrack->perigeeParameters()->momentum()[Trk::px],
	                lowBetaTrack->perigeeParameters()->momentum()[Trk::py],
			lowBetaTrack->perigeeParameters()->momentum()[Trk::pz]);
        MuGirlNS::MuonSegmentList muonSegments;
        muonSegments.push_back(NULL);
        const Trk::Perigee* trk0 = new Trk::Perigee(*(lowBetaTrack->perigeeParameters()));
        
	if ( m_pStauTool->processStau(NULL,muonSegments,
                                      mdtSegmentMakerInfoList,rpcHitsOnTrack,
                                      tgcHitsOnTrack,lowBetaTrack,tp,v,trk0,true).isFailure())
        {
           ATH_MSG_WARNING("MuonBetaRefit: cannot execute processStau");
        }
        else ATH_MSG_DEBUG("MuonBetaRefit: Staubeta " << m_pStauTool->beta() << " mu_beta " << mu_beta);
        
	summary_beta->beta = mu_beta;

         
        if (summary_beta->beta >0.2)
        {
            MuGirlNS::CandidateSummary* stauSummary = new MuGirlNS::CandidateSummary();
            if (m_pStauTool->fillStauSummary(summary_beta,stauSummary).isFailure())
                ATH_MSG_DEBUG("MuonBetaRefit: cannot fill stausummary");


            stauSummary->saveStau = 1;


            stauSummary->pLowBetaTrack = tp;
            if (tp != NULL)
            {
                ATH_MSG_DEBUG("MuonBetaRefit: Trk::Track converted to Rec::TrackParticle p() "<<tp->p4().P());
                //double stauMass = m_pStauTool->mass();
                //double stauMomentum = m_pStauTool->stauP();
                //double stauEnergy = sqrt(stauMass*stauMass + stauMomentum*stauMomentum);
                //tp->setE(stauEnergy);
            }


            fillMuonSegmentColl(*m_pStauTool->newMdtSegments());
            summaryList.push_back(stauSummary);
        }  
        else
           ATH_MSG_DEBUG("MuonBetaRefit: lost candidate ");

       rpcList->clear();
       tgcList->clear();
       delete rpcList;
       delete tgcList;
   }
   
   return true;
}

bool MuGirlRecoTool::RunFromMuonFeature(const EventInfo* pEventInfo, MuGirlNS::CandidateSummaryList& summaryList) {

    /** Initialize cut variables and collection/containers */
    m_mfPtCut = m_stauPtCut;
    const std::string key = "HLT_MuonEFInfo";
    const TrigMuonEFInfoContainer* coll=0;
    
    /** Check if Storegate has the TrigMuonEFInfoContainer; if not returns false and trigger
     *  the standard processing from the ID seed
     */
    if (!evtStore()->contains<TrigMuonEFInfoContainer>(key))
    {
        ATH_MSG_WARNING("RunFromMuonFeature: no TrigMuonEFInfoContainer HLT. Moving to ID processing");
        return false;
    }
    StatusCode sc = evtStore()->retrieve(coll,key);
    int iMuF = 0;
    if (sc.isFailure())
    {
        ATH_MSG_WARNING("RunFromMuonFeature: can't retrieve the TrigMuonEFInfos. Moving to ID processing");
    }

       
    if (coll)
    {
        TrigMuonEFInfoContainer::const_iterator p;
        ATH_MSG_DEBUG("TrigMuonEFInfo container size " << coll->size());
        if (coll->size() == 0) return false;
             
        std::vector<std::pair<double, TrigMuonEFInfoTrack* > > vpointer;
 
        for (p=coll->begin(); p!=coll->end() ; ++p)
        {
            TrigMuonEFInfo* muonInfo = *p;
            if (!muonInfo) 
            {
                ATH_MSG_DEBUG("No TrigMuonEFInfo found.");
	        continue;
            } 
            else
            {

                if (muonInfo->hasTrack())
                { /** was there a muon in this RoI ? */

                    /**  get the TrackContainer, there might be more then one muon in this RoI */
                    TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();
                    ATH_MSG_DEBUG("TrigMuonEFInfoTrack container size " << tc->size());
                    
		    /** loop over container content */
		    for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = tc->begin() ; TrackItr!=tc->end(); TrackItr++)
                    {
                        TrigMuonEFInfoTrack* muonInfoTrack = (*TrackItr);
                        if (!muonInfoTrack->hasExtrapolatedTrack()) continue;
                        if (muonInfoTrack->hasCombinedTrack() && !m_doConsistency) continue;
                        TrigMuonEFTrack* tr = muonInfoTrack->ExtrapolatedTrack();
                        ATH_MSG_DEBUG("TrigMuonEFInfo found NMdtHits " << muonInfo->NMdtHits() << " ROI Num "
                                                                       << muonInfo->RoINum() <<" pt " << tr->pt());
                        vpointer.push_back(std::make_pair(tr->pt(), muonInfoTrack));
                    }
                }

            }
        }

        std::sort(vpointer.begin(),vpointer.end(),compareByHits);
        std::vector<std::pair<double, TrigMuonEFInfoTrack* > >::iterator end = std::unique(vpointer.begin(),vpointer.end(),compareHitsEqual);
        vpointer.resize(end - vpointer.begin());
        std::vector<std::pair<double, TrigMuonEFInfoTrack* > >::iterator it;


        for (it=vpointer.begin(); it!=end ; ++it)
        {
            TrigMuonEFInfoTrack* muonInfoTrack = (*it).second;
            ATH_MSG_DEBUG("Start processing TrigMuonEfInfoTrack "<<iMuF);

            if (!muonInfoTrack) 
            {
                ATH_MSG_DEBUG("No TrigMuonEFInfoTrack found.");
	        continue;
            } 
            else   
            {
                int found_combined = 0;	
                if (muonInfoTrack->hasCombinedTrack())
                {
                    ATH_MSG_DEBUG("TrigMuonEFInfoTrack has combined ");
                    TrigMuonEFCbTrack* trc = muonInfoTrack->CombinedTrack();
                    ATH_MSG_DEBUG("Retrieved combined track with abs pt "
                                 << (*trc).pt()/CLHEP::GeV << "GeV " << " eta "<< (*trc).eta()<<" phi "<<(*trc).phi());
	            ATH_MSG_DEBUG("Skipping candidate");

                    found_combined = 1; 
                    ATH_MSG_DEBUG("Found CombinedEFMuon: doConsistency "<<m_doConsistency);
                    if (!m_doConsistency)   continue;
                }
                if (!muonInfoTrack->hasExtrapolatedTrack()) 
                {
                    ATH_MSG_DEBUG("Extrapolated track not initialized."); 
	            continue;
                }
                TrigMuonEFTrack* tr = muonInfoTrack->ExtrapolatedTrack();
                if (!tr) 
                {
	            ATH_MSG_DEBUG("No TrigMuonEFTrack found.");
                    continue;
                } 
                else 
                {
                    ATH_MSG_DEBUG("Retrieved extrapolated track with abs pt "
                                  << (*tr).pt()/CLHEP::GeV << "GeV " << " eta "<< (*tr).eta()<<" phi "<<(*tr).phi());
                    ATH_MSG_DEBUG("Start SA processing");


                    if (tr->pt() < m_mfPtCut)  continue;

                    if (m_pCandidate->fillEFTrack(iMuF, tr, true,found_combined).isFailure())
                    {
                        ATH_MSG_DEBUG("RunFromMuonFeature: cannot fill CANDIDATE");
                        continue;
                    }
                    if (!m_pCandidate->hasInnerStationSegments())
                    {
                        ATH_MSG_DEBUG("RunFromMuonFeature: candidate does not have MDT inner station hits. Skip candidate");
                        continue;
                    }
                    MuGirlNS::CandidateSummary* summary_mf = new MuGirlNS::CandidateSummary();

                    if (m_pCandidate->fillSummary(summary_mf).isFailure())
                    {
                        ATH_MSG_DEBUG("RunFromMuonFeature: cannot fill SUMMARY");
                        continue;
                    }

                    if (m_doNTuple)
                    {
                        if (pEventInfo!=0)
                        {
                            if ((m_pCandidate->fillNTupleEventInfo(pEventInfo->event_ID()->run_number(),
                                                                   pEventInfo->event_ID()->event_number())).isFailure())
                                ATH_MSG_WARNING("RunFromMuonFeature: cannot fill NTuple");
                        }
                       
		        if (m_pCandidate->fillNTuple().isFailure())
                            ATH_MSG_WARNING("RunFromMuonFeature: cannot fill NTuple");
                       
		        if (m_doTruth && tr !=0)
		        {
                            ATH_MSG_DEBUG("Calling m_pTruthTool->fillNTuple for MuonFeature processing");
                            if (m_pTruthTool->fillNTuple(summary_mf).isFailure())
                            ATH_MSG_WARNING("RunFromMuonFeature: cannot fill Truth in NTuple for Muonfeature processing"); 
                        }
                    }

             
                    const Trk::Track* StandAloneRefittedTrack = NULL;

                    if (m_doStau && summary_mf->numMdtHits >4 &&
                        (summary_mf->numTgcEtaHits>2 || summary_mf->numTgcPhiHits>2 ||
                         summary_mf->numRpcEtaHits>2 || summary_mf->numRpcPhiHits>2))
                    {
                        summary_mf->saveStau = 1;
                        m_pCandidate->setPassdoStau(true);

                        const MuGirlNS::MdtSegmentMakerInfoList mdtSegmentMakerInfoList = m_pCandidate->getMdtSegmentMakerInfo();
                        const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments = m_pCandidate->getRpcHitsInSegments();
                        const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments = m_pCandidate->getTgcHitsInSegments();

                        double theta = 2.*atan(exp(-(tr->eta())));
                        float ef_px   = fabs((*tr).pt())*cos((*tr).phi());
                        float ef_py   = fabs((*tr).pt())*sin((*tr).phi());
                        float ef_pz   = fabs((*tr).pt())/tan(theta);
                        Amg::Vector3D v(ef_px,ef_py,ef_pz);

                        Amg::Vector3D v0(tr->posX(),tr->posY(),tr->posZ()); 
                                        
                        const Trk::Perigee* trk0 = new Trk::Perigee(tr->d0(),tr->z0(),tr->phi(),theta ,summary_mf->qOverP,v0);
                        if (trk0 != NULL) 
                        {
                            ATH_MSG_DEBUG("RunFromMuonFeature: initial parameters: momentum " << trk0->momentum());
                            ATH_MSG_DEBUG(" position "<<trk0->position()
                                        << " charge "<< trk0->charge() << " pt " << trk0->pT() << " eta " << trk0->eta()
                                        << " z0 " << trk0->parameters()[Trk::z0] << " d0 " << trk0->parameters()[Trk::d0]);
                        } 
                        if ( m_pStauTool->processStau(NULL,m_pCandidate->vectorMuonSegments(),
                                                      mdtSegmentMakerInfoList,rpcHitsInSegments,
                                                      tgcHitsInSegments,NULL,NULL,v,trk0).isFailure())
                        {
                            ATH_MSG_WARNING("RunFromMuonFeature: cannot execute processStau");
                            continue;
                        }
                        summary_mf->beta = m_pStauTool->beta();

                        if (m_doNTuple)
                        {
                            if (m_pStauTool->fillNTuple().isFailure())
                                ATH_MSG_WARNING("RunFromMuonFeature: cannot fill Stau NTuple");
                        }

                        if (summary_mf->beta >0.2)
                        {
                            MuGirlNS::CandidateSummary* stauSummary = new MuGirlNS::CandidateSummary();
                            if (m_pStauTool->fillStauSummary(summary_mf,stauSummary).isFailure())
                                ATH_MSG_DEBUG("RunFromMuonFeature: cannot fill stausummary");

                            //stauSummary->lhr = 0.8;

                            bool ANNstau = generateMuGirl(stauSummary);
                            if (ANNstau <0.2) continue;

                            if (m_doConsistency)
                            {
                                if (stauSummary->nnBarrel >0 ) m_pStauTool->setCandidateAnn(stauSummary->nnBarrel);
                                else m_pStauTool->setCandidateAnn(stauSummary->nnEndCap);
                            }
 
                            ATH_MSG_DEBUG("RunFromMuonFeature: stau passANN "<<ANNstau);
                            stauSummary->saveStau = 1;
                            stauSummary->qOverP = summary_mf->qOverP;
                            fillMuonSegmentColl(*m_pStauTool->newMdtSegments());
                            fillMuonSegmentColl(m_pCandidate->vectorMuonSegments());
                            summaryList.push_back(stauSummary);
                            StandAloneRefittedTrack = m_pStauTool->refittedTrack();


                            if (StandAloneRefittedTrack != NULL)
                            {
                                xAOD::TrackParticle* tp = NULL;

                                if (m_createTrackParticle) {
                                    ATH_MSG_DEBUG("found SA track ");
                                    const xAOD::Vertex* pVxCandidate = 0 ;

                                    m_pRefittedTrkCollection->push_back(const_cast<Trk::Track*> (StandAloneRefittedTrack));
                                    ElementLink<TrackCollection> t_link(*m_pRefittedTrkCollection, m_pRefittedTrkCollection->size()-1);

                                    if (t_link.isValid()) {
                                        tp = m_particleCreatorTool->createParticle(t_link, m_pRefittedTrkContainer, pVxCandidate, xAOD::muon);
                                    } else {
                                        ATH_MSG_WARNING("RunFromMuonFeature: Element link to the StandAlone refitted track is not valid; new TrackParticle is not created");
                                    }
                                }
                                stauSummary->pRefittedTrack = tp;
                                stauSummary->pTrkRefitted   = StandAloneRefittedTrack;

                                if (tp != NULL)
                                {
                                    if (msg().level() <= MSG::DEBUG)
                                    ATH_MSG_DEBUG("MS StandAlone Trk::Track converted to Rec::TrackParticle p() "<<tp->p4().P());
                                    m_pCandidate-> fillRefittedTrack(tp,StandAloneRefittedTrack,stauSummary);
                                    // mass and energy of the stau must be defined within the ParticleCreatorTool
                                    //double stauMass = m_pStauTool->mass();
                                    //double stauMomentum = m_pStauTool->stauP();
                                    //double stauEnergy = sqrt(stauMass*stauMass + stauMomentum*stauMomentum);
                                    //tp->setE(stauEnergy);
                                }
                                else
                                {
                                    m_pCandidate->fillRefittedTrack(NULL,StandAloneRefittedTrack,stauSummary);
                                    delete tp;
                                }
                            }
                            else
                            {
                                delete StandAloneRefittedTrack;
                                m_pCandidate-> fillRefittedTrack(NULL,NULL,stauSummary);
                            }
                            if (m_doNTuple)
                            {

                                ATH_MSG_DEBUG("Calling m_pCandidate->fillNTuple");

                                if (m_pCandidate->fillNTuple().isFailure())
                                    ATH_MSG_WARNING("RunFromMuonFeature: cannot fill NTuple");
                                if (m_doTruth)
                                {
                                    ATH_MSG_DEBUG("Calling m_pTruthTool->fillNTuple");
                                    if (m_pTruthTool->fillNTuple(stauSummary).isFailure())
                                        ATH_MSG_WARNING("RunFromMuonFeature: cannot fill Truth in NTuple");
                                }
                                if (m_pCandNTuple->writeRecord().isFailure())
                                    ATH_MSG_WARNING("RunFromMuonFeature: cannot write NTuple");
                            }
                        } // if beta >0.2 && beta<1.2
                    }   // enter stau selection
                } // If extrp track exists

                iMuF++; 
    
            } //if muonInfo exist 
        }  //loop on muonInfoContainer
    }    // muonInfocontainer is not empty

    ATH_MSG_DEBUG("RunFromMuonFeature: processing is done");

    return true;
}

bool MuGirlRecoTool::RunFromID(const EventInfo* pEventInfo, const VxContainer* vxCandidatesContainer, MuGirlNS::CandidateSummaryList& summaryList, bool generateTag) {

   int iTrack = 0;  /** counts the number of track processed */

   ATH_MSG_DEBUG("RunFromID called with generateTag = " << ((generateTag)? "True" : "False") );

   /** start looping on calo particles */
   for (unsigned int i = 0; i < m_caloParticles.size(); i++)
   {

       CaloParticle* pParticle = m_caloParticles[i];

       ATH_MSG_DEBUG("RunFromID: start processing CaloParticle seed at eta=" << pParticle->eta << " phi=" << pParticle->phi);

       const xAOD::TrackParticle* pTrackParticle = pParticle->pTrackParticle;
       const Trk::TrackParameters* extrMuonEntry = pParticle->pTrackParameters;

       bool doStau = m_doStau && pTrackParticle->perigeeParameters().pT() >= m_stauPtCut;

       if (!m_pCandidate->fill(iTrack, pTrackParticle, extrMuonEntry, doStau).isSuccess()) continue;

       if (m_pClusCollection != NULL)
           if (m_doLHR && m_pMuLHR && pParticle->isolated && m_pClusCollection->size() > 0)
               calculateLHR(pParticle);

       MuGirlNS::CandidateSummary* summary = new MuGirlNS::CandidateSummary();

       bool ANNaccept = false;
       if (m_pCandidate->fillSummary(summary).isFailure())
       {
           ATH_MSG_DEBUG("RunFromID: cannot fill summary");
       }
       else
       {
           if (m_doANNSelection)
           {
               ANNaccept = generateMuGirl(summary);

           } 
       } 

       const Trk::Track* RefittedTrack = NULL;

       if ((m_doANNSelection && ANNaccept) || !(m_doANNSelection))
       {
           summary->saveAnn = 1;


           if (m_doGlobalFit&&(pTrackParticle->trackLink().isValid())&&
                    (m_pCandidate->vectorMuonSegments()).size()>0)
           {
               ATH_MSG_DEBUG("RunFromID: performing the global track fit.");
               const Trk::Track* originalTrack = *pTrackParticle->trackLink();

               RefittedTrack = m_pGlobalFitTool->globalFit(originalTrack,m_pCandidate->vectorMuonSegments(),m_doNTuple);


               if (RefittedTrack != NULL)
               {
                   xAOD::TrackParticle* tp = NULL;
                   /** gather the vertex from the previous track particle */
                   const xAOD::Vertex* pVxCandidate = NULL;
                   ElementLink<xAOD::VertexContainer> v_link = pTrackParticle->vertexLink();
                   if(v_link.isValid()) pVxCandidate = *v_link;

                   if (!generateTag) {   /** for tag generation the running schema does not foresee to create TrackParticles */ // m_createTrackParticle) {
                       ATH_MSG_DEBUG("RunFromID: creating the new xAOD::TrackParticle from the Refitted Track");
                       /** add the refitted track to the refitted track container and create the element link */
                       m_pRefittedTrkCollection->push_back(const_cast<Trk::Track*> (RefittedTrack));
                       ElementLink<TrackCollection> t_link(*m_pRefittedTrkCollection, m_pRefittedTrkCollection->size()-1);
                       if (!t_link.isValid()) {
                           ATH_MSG_WARNING("RunFromID: Element link to the refitted track is not valid; new Refitted xAOD::TrackParticle is not created");
                       }
                       if (pVxCandidate == NULL) {
                           ATH_MSG_WARNING("RunFromID: pVxCandidate is not valid; new Refitted xAOD::TrackParticle is not created");
                       }

                       /** create the new Trackparticle only if the associated vertex exist ..  NULL && vxCandidatesContainer != 0) */
                       if (pVxCandidate != 0  && t_link.isValid()) {
                           tp =m_particleCreatorTool->createParticle(t_link, m_pRefittedTrkContainer, pVxCandidate, xAOD::muon);
                           ATH_MSG_DEBUG("RunFromID: new Refitted xAOD::TrackParticle is created.");
                       }
                   }

                   if (tp != NULL)
                   {
                       ATH_MSG_DEBUG("Refitted Trk::Track converted to xAOD::TrackParticle p() "<<tp->p4().P());
                       m_pCandidate-> fillRefittedTrack(tp,RefittedTrack,summary);
                       ATH_MSG_DEBUG("xAOD::TrackParticle.numberDoF()="<<tp->numberDoF());

                       if (v_link.isValid() && vxCandidatesContainer !=0) tp->setVertexLink(v_link);
                   } else {
                       ATH_MSG_DEBUG("Set a NULL Refitted xAOD::TrackParticle into the CandidateTool");
                       m_pCandidate-> fillRefittedTrack(NULL,RefittedTrack,summary);
                   }

                   if (m_doMSRefit)
	           {
                       //   TEST MS FIT   sofia
		       ATH_MSG_DEBUG("RunFromID: looking for an ms track ...");
		       const Trk::Track* msTrack = m_pMSGlobalFitTool->msFit(m_pCandidate->vectorMuonSegments(),RefittedTrack,m_doNTuple);
		       if (msTrack != NULL)
		       {
                           msTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuGirl); 
		           ATH_MSG_DEBUG("MS standalone Trk::Track p() "   << fabs(1./ (msTrack->perigeeParameters()->parameters())[Trk::qOverP]) );
			   ATH_MSG_DEBUG("MS standalone Trk::Track pt() "  << msTrack->perigeeParameters()->pT() );
                           ATH_MSG_DEBUG("MS standalone Trk::Track eta() " << msTrack->perigeeParameters()->eta() );

                           xAOD::TrackParticle* mstp = NULL;
                           if( !generateTag ) /** for tag generation the running schema does not foresee to create TrackParticles */ //m_createTrackParticle )
                           {
                               ATH_MSG_DEBUG("RunFromID: creating the new xAOD::TrackParticle from the MS Refitted Track");
			       m_pRefittedTrkCollection->push_back(const_cast<Trk::Track*> (msTrack));
                               ElementLink<TrackCollection> t_link(*m_pRefittedTrkCollection, m_pRefittedTrkCollection->size()-1);
                               if (!t_link.isValid()) {
                                   ATH_MSG_WARNING("RunFromID: Element link to the ms refitted track is not valid; new MS Refitted xAOD::TrackParticle is not created");
                               }

                               if (pVxCandidate == NULL) {
                                   ATH_MSG_WARNING("RunFromID: pVxCandidate is not valid; new MS Refitted xAOD::TrackParticle is not created");
                               }

		               if (pVxCandidate != 0 && t_link.isValid() )//use the combined fit vertex info 
			       {
			           mstp = m_particleCreatorTool->createParticle(t_link, m_pRefittedTrkContainer, pVxCandidate, xAOD::muon);
			       }
                           }

			   if (mstp != NULL)
		           {
		               ATH_MSG_DEBUG("MS Refitted Trk::Track converted to xAOD::TrackParticle p() "   << mstp->p4().P() );
                               ATH_MSG_DEBUG("MS Refitted Trk::Track converted to xAOD::TrackParticle pt() "  << mstp->pt());
                               ATH_MSG_DEBUG("MS Refitted Trk::Track converted to xAOD::TrackParticle eta() " << mstp->eta()); 
	                       m_pCandidate-> fillMSTrack(mstp,msTrack,summary);
				     
		               if (v_link.isValid() && vxCandidatesContainer !=0) mstp->setVertexLink(v_link);
			   }
			   else 
                           {
                               ATH_MSG_DEBUG("Set a NULL MS Refitted xAOD::TrackParticle into the CandidateTool");
                               m_pCandidate-> fillMSTrack(NULL,msTrack,summary);
                           }
		       }
		       else 
                       {
                           m_pCandidate-> fillMSTrack(NULL,NULL,summary);
                           ATH_MSG_DEBUG("No ms Track");
                       }
		   }//end if(doMSRefit)
/*
                   }//end if(tp != NULL)
                   else
		   {
	               delete tp;
		   }
*/
               }
               else 
               {
                   //delete RefittedTrack;
                   ATH_MSG_DEBUG("RunFromID: Global Fit produced a NULL Refitted Track");
               }

               ATH_MSG_DEBUG("RunFromID: Global Fit done ");
           }
       }
       else
       {
           ATH_MSG_DEBUG("RunFromID: Candidate is not a muon track, filling a NULL summary");
           m_pCandidate-> fillRefittedTrack(NULL,NULL,summary);
       }

       if (m_doNTuple)
       {
           if (pEventInfo!=0)
           {
               StatusCode sc = m_pCandidate->fillNTupleEventInfo(pEventInfo->event_ID()->run_number(),
                                                      pEventInfo->event_ID()->event_number());

               if (sc.isFailure())
                   ATH_MSG_WARNING("RunFromID: Cannot fill NTuple"); 
           }

           // select the hough maxima associated to the candidate
           HoughData associated_maxima[16][4][2];
           if (m_doHoughTransform) {
               ATH_MSG_DEBUG("Associating the Hough Maxima to MuGirl candidate");
               associateMaxima(associated_maxima);
           }

           // filling the candidate info in NTuple
           ATH_MSG_DEBUG("Calling m_pCandidate->fillNTuple");
           if (m_pCandidate->fillNTuple().isFailure())
             ATH_MSG_WARNING("RunFromID: error during fill NTuple");

           // filling the Hough transfomr info in NTuple
           if (m_doHoughTransform) {
               ATH_MSG_DEBUG("Calling m_pCandidate->fillNTuple(with associated hough maxima)");
               if ( m_pCandidate->fillNTuple(associated_maxima).isFailure() )
                 ATH_MSG_WARNING("RunFromID: Could not include the Hough data into the Candidate NTuple");
           }

           if (m_doNTuple&&m_doTruth&&pTrackParticle!=NULL)
           {
               ATH_MSG_DEBUG("Calling m_pTruthTool->fillNTuple");

               if (m_pTruthTool->fillNTuple(summary).isFailure())
                   ATH_MSG_WARNING("RunFromID: Cannot fill Truth in NTuple");
           } 

           // clear temporay hough data structure
           for (unsigned int i=0;i<16;i++)
               for (unsigned int j=0;j<4;j++)
                   for (unsigned int k=0;k<2;k++) associated_maxima[i][j][k].clear();
       }


       /**
        * Stau processing
        */
       bool passedStau = false;
       if(doStau && summary->numMdtHits >4 &&(summary->numTgcEtaHits>2 || summary->numTgcPhiHits>2 || summary->numRpcEtaHits>2 || summary->numRpcPhiHits>2))
       {
           ATH_MSG_DEBUG("RunFromID: looking for staus ...");
           m_pCandidate->setPassdoStau(true);
           //retrieve segment maker info list
           const MuGirlNS::MdtSegmentMakerInfoList mdtSegmentMakerInfoList = m_pCandidate->getMdtSegmentMakerInfo();
           //retrieve the rpc hits
           const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments = m_pCandidate->getRpcHitsInSegments();
           const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments = m_pCandidate->getTgcHitsInSegments();
           //process stau
           const Trk::Track* pMuonRefittedTrack = RefittedTrack; //NULL;
           //if (summary->pRefittedTrack != NULL)
           //    pMuonRefittedTrack = *summary->pRefittedTrack->track();
           StatusCode sc = m_pStauTool->processStau(pTrackParticle,
                                         m_pCandidate->vectorMuonSegments(),
                                         mdtSegmentMakerInfoList,
                                         rpcHitsInSegments,
                                         tgcHitsInSegments,
                                         pMuonRefittedTrack,NULL);

           if(sc.isFailure()) continue;

           double Beta = m_pStauTool->beta();
           double stauMass = m_pStauTool->mass();
           summary->beta = Beta;
           
           if (Beta >0.2)
           {
               ATH_MSG_DEBUG("stau Beta = " << Beta << " mass " << stauMass); 
               MuGirlNS::CandidateSummary* stauSummary = new MuGirlNS::CandidateSummary();
               if (m_pStauTool->fillStauSummary(summary,stauSummary).isFailure())
               {
                   ATH_MSG_DEBUG("RunFromID: Cannot fill stausummary");
               } 

               bool ANNstau = generateMuGirl(stauSummary);
               ATH_MSG_DEBUG("RunFromID: stau passANN "<<ANNstau);
               if (stauSummary->nnBarrel >0.2 || stauSummary->nnEndCap >0.2) ANNstau = true;
               if (m_doConsistency)
               {
                   if (stauSummary->nnBarrel >0 ) m_pStauTool->setCandidateAnn(stauSummary->nnBarrel);
                   else m_pStauTool->setCandidateAnn(stauSummary->nnEndCap);
               }
               //Store in the low beta container either if pass stau selection or if pass stau ann but fails muon ann
               if ((Beta <0.97 && stauMass >40*CLHEP::GeV && ANNstau) || ( ANNstau && !ANNaccept ))
               {
                   stauSummary->saveStau = 1;
                   summaryList.push_back(stauSummary);
                   passedStau =true;

                   /** Only the non MDT segment will be attached to the MuGirlTag, so fill the Storegate container anyway */
                   fillMuonSegmentColl(*m_pStauTool->newMdtSegments());

                   const Trk::Track* stauRefittedTrack =  m_pStauTool->refittedTrack();
                   if(NULL!=stauRefittedTrack)
                   {
                       xAOD::TrackParticle* stauTrack = NULL;
                       ElementLink<xAOD::VertexContainer> v_link = pTrackParticle->vertexLink();
                       if (!generateTag) { /** for tag generation the running schema does not foresee to create TrackParticles */ //m_createTrackParticle) {
                           ATH_MSG_DEBUG("RunFromID: creating the new xAOD::TrackParticle from Stau Refitted Track");
                           m_pRefittedTrkCollection->push_back(const_cast<Trk::Track*> (stauRefittedTrack));
                           ElementLink<TrackCollection> t_link(*m_pRefittedTrkCollection, m_pRefittedTrkCollection->size()-1);
                           if (!t_link.isValid()) {
                               ATH_MSG_WARNING("RunFromID: Element link to the stau refitted track is not valid; new TrackParticle is not created");
                           }
 
                           double stauMomentum = m_pStauTool->stauP();
                           double stauEnergy = sqrt(stauMass*stauMass + stauMomentum*stauMomentum);

                           ATH_MSG_DEBUG("stauRefittedTrack energy = " << stauEnergy << " momentum " << stauMomentum );

                           const xAOD::Vertex* stauVxCandidate = NULL;
                           stauVxCandidate = *v_link;
                           if (stauVxCandidate != NULL && vxCandidatesContainer != 0 && t_link.isValid())
                               stauTrack = m_particleCreatorTool->createParticle(t_link, m_pRefittedTrkContainer, stauVxCandidate, xAOD::muon);
                       }

                       stauSummary->pRefittedTrack = stauTrack;
                       stauSummary->pTrkRefitted   = stauRefittedTrack;

		       //stau
		       if (generateTag){

			 if (stauRefittedTrack !=NULL){
			   ATH_MSG_DEBUG("MuGirlRecoTool: stauRefittedTrack "<<(m_pCandidate->vectorMuonSegments()).size());

			   std::vector<const Muon::MuonSegment*> stauSegmentList;

			   for (MuGirlNS::MuonSegmentList::iterator itMuonSeg = (*m_pStauTool->newMdtSegments()).begin();
				itMuonSeg != (*m_pStauTool->newMdtSegments()).end();
				itMuonSeg++)
			     {
			       ATH_MSG_DEBUG("MuGirlRecoTool: newMdtSegments"); 
			       stauSegmentList.push_back(*itMuonSeg);
			     }
			   MuonCombined::MuGirlLowBetaTag* lowbetatag = new MuonCombined::MuGirlLowBetaTag(stauRefittedTrack,std::vector<const Muon::MuonSegment*>());

			   pParticle->pInDetCandidate->addTag(*lowbetatag);
			 }
		       }// generateTag


                       if (stauTrack != NULL)
                       {
                           //stauTrack->setE(stauEnergy);   cannot do this for the time being
                           if (vxCandidatesContainer !=0) stauTrack->setVertexLink(v_link);
                       }
                       else
                       {
                           delete stauTrack;
                       }
                   }
                   else
                   {
                       ATH_MSG_DEBUG("stauRefittedTrack is NULL");
                       stauSummary->pRefittedTrack = NULL;
                       stauSummary->pTrkRefitted   = NULL;
                       delete  stauRefittedTrack;
                   }
               }
               else
               {
                   for (MuGirlNS::MuonSegmentList::iterator itMuonSeg = (*m_pStauTool->newMdtSegments()).begin();
                       itMuonSeg != (*m_pStauTool->newMdtSegments()).end();
                       itMuonSeg++)
                   {
                       m_pSegmentManager->addSegment(*itMuonSeg,MuGirlNS::SegmentManager::MDT_STAU);
                   }
                   const Trk::Track* stauRefittedTrack =  m_pStauTool->refittedTrack();
                   delete stauRefittedTrack;
	           delete stauSummary;
               }
           }
           else
           {
               for (MuGirlNS::MuonSegmentList::iterator itMuonSeg = (*m_pStauTool->newMdtSegments()).begin();
                     itMuonSeg != (*m_pStauTool->newMdtSegments()).end();
                     itMuonSeg++)
               {
                   m_pSegmentManager->addSegment(*itMuonSeg,MuGirlNS::SegmentManager::MDT_STAU);
               }
               const Trk::Track* stauRefittedTrack =  m_pStauTool->refittedTrack();
               delete stauRefittedTrack;
           }

           if (m_doNTuple)
           {
               sc = m_pStauTool->fillNTuple();
               if(sc.isFailure())
                   ATH_MSG_DEBUG("RunFromID: stauTool->fillNtuple failed ");
           }
       }
           
       if (summary->saveAnn == 1)
       {
           ATH_MSG_DEBUG("RunFromID: delivering the muon summary to the summaryList");
           summary->saveStau = 0;
           //m_pCandidate->releaseMuonSegments(false);   //the segments released from Candidate ownership
           summaryList.push_back(summary);

           /** Generate the MuGirlTag according to the new running schema: MuGirlTag will delete the segments 
             * Otherwise put the list of MuGirl Segments into Storegate: Storegate will delete the segments   
             */
           if (generateTag)
           {
               ATH_MSG_DEBUG("RunFromID: delivering the muon pTrkRefitted track to the MuGirl tag");
               std::vector<const Muon::MuonSegment*> muonSegmentList = summary->muonSegmentList;
               MuonCombined::MuGirlTag* tag = 0;
               if (summary->pTrkRefitted==NULL) tag = new MuonCombined::MuGirlTag(muonSegmentList);
               else                             tag = new MuonCombined::MuGirlTag(summary->pTrkRefitted, muonSegmentList);
               tag->setUpdatedExtrapolatedTrack(summary->pTrkMSRefitted);
               pParticle->pInDetCandidate->addTag(*tag);
               /** set the segment into SegmentManager undeletable */
               for (MuGirlNS::MuonSegmentList::const_iterator seg=muonSegmentList.begin(); seg!=muonSegmentList.end(); seg++) {
                   const Muon::MuonSegment* s = *seg;
                   m_pSegmentManager->setInStore(s, true);
               }
           } else {
               fillMuonSegmentColl(summary->muonSegmentList);
           }
       }
       else
       {
           if (passedStau)
           {
               //  m_pCandidate->releaseMuonSegments(true);

               /** Generate the MuGirlTag according to the new running schema: MuGirlTag will delete the segments 
                 * Otherwise put the list of MuGirl Segments into Storegate: Storegate will delete the segments   
                 */
               if (generateTag)
               {
                   ATH_MSG_DEBUG("RunFromID: delivering the pTrkLowBeta to the MuGirl tag");
                   std::vector<const Muon::MuonSegment*> muonSegmentList = summary->muonSegmentList;
                   MuonCombined::MuGirlTag* tag = new MuonCombined::MuGirlTag(summary->pTrkLowBeta, muonSegmentList);
                   tag->setUpdatedExtrapolatedTrack(summary->pTrkLowBetaExtr);
                   pParticle->pInDetCandidate->addTag(*tag);
                   /** set the segment into SegmentManager undeletable */
                   for (MuGirlNS::MuonSegmentList::const_iterator seg=muonSegmentList.begin(); seg!=muonSegmentList.end(); seg++)
                       m_pSegmentManager->setInStore(*seg, true);
               } else {
                   ATH_MSG_DEBUG("RunFromID: delivering the noMDT segments into the Storegate container");
                   std::vector<const Muon::MuonSegment*> muonSegmentList_noMDT;
                   for (MuGirlNS::MuonSegmentList::iterator itMuonSeg = (summary->muonSegmentList).begin();
                       itMuonSeg != (summary->muonSegmentList).end();
                       itMuonSeg++)
                   {
                       if (m_pSegmentManager->isNotMDT(*itMuonSeg))
                           muonSegmentList_noMDT.push_back(*itMuonSeg);
                   }
                   fillMuonSegmentColl(muonSegmentList_noMDT);
               }
           }
           delete summary;
       }
       if (m_doNTuple)
       {
           if (m_pCandNTuple->writeRecord().isFailure())
               ATH_MSG_WARNING("RunFromID: Cannot write NTuple");
       }
       ATH_MSG_DEBUG("Done processing track " << iTrack <<" number of cand: "<<summaryList.size());
       iTrack++;
   }

   return true;
}

StatusCode MuGirlRecoTool::execute()
{
    ATH_MSG_DEBUG("execute() executing.");

    StatusCode sc;

    /** Stau Collection */
    if(m_doStau) m_pStauTool->recordCollection();


    /** Hough Data processing */
    if ( m_doHoughTransform && this->processHoughData().isFailure() ) {
      ATH_MSG_INFO("Hough data cannot be processed successfully, cleaning Hough data structure ....");
      this->clear_hough_data();
    }


    /** Register the Collection and the Container for the MuGirl refitted tracks */
    m_pRefittedTrkCollection = new TrackCollection();
   
    if (evtStore()->contains<TrackCollection>(m_sRefittedTrkCollection))
    {
      const TrackCollection* tracks;
      if (StatusCode::SUCCESS == evtStore()->retrieve(tracks,m_sRefittedTrkCollection))
         if (StatusCode::SUCCESS == evtStore()->remove(tracks))
             ATH_MSG_WARNING("execute: removed pre-existing track collection"); 
    }

    sc = evtStore()->record(m_pRefittedTrkCollection, m_sRefittedTrkCollection);
    if (sc.isFailure())
    {
        ATH_MSG_WARNING("execute: cannot record " << m_sRefittedTrkCollection);
        return StatusCode::SUCCESS;
    }
    m_pRefittedTrkContainer = new xAOD::TrackParticleContainer();

    if (evtStore()->contains<xAOD::TrackParticleContainer>(m_RefittedTrkLocation))
    {
      const xAOD::TrackParticleContainer* tracksP;
      if (StatusCode::SUCCESS == evtStore()->retrieve(tracksP,m_RefittedTrkLocation))
         if (StatusCode::SUCCESS == evtStore()->remove(tracksP))
             ATH_MSG_WARNING("execute: removed pre-existing TrackParticles collection"); 
    }

    sc = evtStore()->record(m_pRefittedTrkContainer, m_RefittedTrkLocation);
    if (sc.isFailure())
    {
        ATH_MSG_WARNING("execute: cannot record " << m_RefittedTrkLocation);
        return StatusCode::SUCCESS;
    }

    xAOD::TrackParticleAuxContainer* aux = new xAOD::TrackParticleAuxContainer();
    std::string aux_name = m_RefittedTrkLocation.value();
    aux_name = aux_name + "Aux.";
    sc = evtStore()->record(aux, aux_name);
    if (sc.isFailure())
    {
        ATH_MSG_WARNING("execute: cannot record " << aux_name );
        return StatusCode::SUCCESS;
    }

    m_pRefittedTrkContainer->setStore(aux);


    /** In the doMSRefit processing the MuGirl refitted tracks are not put into StoreGate; 
     *  no Container and Collection registered into StoreGate for these objects.
     */


    /** Register into Storegate the MuGirl Segment Collection:
      * check if a collection is already existing and clear it.
      */
    if (evtStore()->contains<Trk::SegmentCollection>(m_sSegmentCollection)) {
        const Trk::SegmentCollection* segments;
        if (StatusCode::SUCCESS == evtStore()->retrieve(segments,m_sSegmentCollection))
            if (StatusCode::SUCCESS == evtStore()->remove(segments))
                ATH_MSG_DEBUG("Removed pre-existing track collection");
    }

    m_pSegmentCollection = new Trk::SegmentCollection();
    sc = evtStore()->record(m_pSegmentCollection, m_sSegmentCollection);
    if (sc.isFailure())
    {
        ATH_MSG_WARNING("execute: cannot record " << m_sSegmentCollection);
        return StatusCode::SUCCESS;
    }


    /** Terminate the processing if the Solenoid is off */
    if (!m_magFieldSvc->solenoidOn())
    {
        ATH_MSG_DEBUG("Solenoid is off: Execute done"); 
        sc=StatusCode::SUCCESS;
        return sc;
    }


    /** Check if the track seeds from the Inner Detector are available in StoreGate, consequently
     *  set the appropriate running mode (i.e. run_from_id flag) and check the consistency 
     *  with the configuration flag (e.g. assume that the run from Muon Feature should be on).
     */
    bool run_from_id = true; 

    if (evtStore()->contains<xAOD::TrackParticleContainer>(m_inDetParticlesLocation))
    {
        sc = evtStore()->retrieve(m_pInDetParticleContainer, m_inDetParticlesLocation);
        if (sc.isFailure() || m_pInDetParticleContainer == NULL)
        {
            if (!m_doMuonFeature) 
            {
               ATH_MSG_WARNING("execute: cannot retrieve " << m_inDetParticlesLocation);
               return StatusCode::SUCCESS;
            }
            else
            {
               run_from_id = false;           
               ATH_MSG_DEBUG("Cannot retrieve " << m_inDetParticlesLocation <<"MuGirl is seeded by MuonFeature");
            } 
        }
    }
    else
    {   
        if (!m_doMuonFeature) {
           ATH_MSG_WARNING("execute: SG does not contain " << m_inDetParticlesLocation <<" and MuGirl is not seeded by MuonFeature .... terminating!");
           return StatusCode::SUCCESS;
        }
        else 
        {
           run_from_id = false;           
           ATH_MSG_DEBUG("SG does not contain " << m_inDetParticlesLocation <<"MuGirl is seeded by MuonFeature");
        } 
    }

    ATH_MSG_DEBUG("Size of TrackParticleCandidate = " << m_pInDetParticleContainer->size());


    /** Retrieve the Container of the primary candidates.  */
    const VxContainer* vxCandidatesContainer = 0;
    if (evtStore()->contains<VxContainer>("VxPrimaryCandidate"))
    {
        sc = evtStore()->retrieve(vxCandidatesContainer, "VxPrimaryCandidate");
        if (sc.isFailure())
        {
            ATH_MSG_WARNING("execute: cannot retrieve VxPrimaryCandidate");
        }
    }
    else    ATH_MSG_DEBUG("execute: VxPrimaryCandidate not in StoreGate");


    /**
     * Try to retrieve the EventInfo object for getting the run number and the event number
     */
    const EventInfo* pEventInfo = NULL;
    sc = evtStore()->retrieve(pEventInfo);
    if (sc.isFailure() || pEventInfo == NULL)
        ATH_MSG_WARNING("execute: cannot retrieve EventInfo");

    MuGirlNS::CandidateSummaryList summaryList;
    
    
    /** MuonBetaRefit processing */ 
    if (m_doMuonBetaRefit)
    {
        ATH_MSG_DEBUG("Start the MuonBetaRefit processing");
        this->MuonBetaRefit(summaryList);
    }
    
    /** Standalone processing */
    if (m_doMuonFeature)
    {
        ATH_MSG_DEBUG("Start the MuonFeature processing");
        this->RunFromMuonFeature(pEventInfo,summaryList);
    } 

    /** ID track seed processing */
    if (run_from_id)
    { 
        ATH_MSG_DEBUG("Start the ID seeds processing");
        /** Retrieve the CaloCluster container */
        if (evtStore()->contains<xAOD::CaloClusterContainer>(m_CaloCollection))
        {
            StatusCode sc = evtStore()->retrieve(m_pClusCollection, m_CaloCollection);
            if (sc.isFailure() || m_pClusCollection == NULL)
            {
                ATH_MSG_WARNING("execute: cannot retrieve ClusterContainer");
            }
            else
                ATH_MSG_DEBUG("execute: calo Collection Size: "<<m_pClusCollection->size());
        }

        /** Create the CaloParticles */
        if (msgLvl(MSG::DEBUG))
        {
            ATH_MSG_DEBUG("******************************************");
            ATH_MSG_DEBUG("********  list of TrackParticles  ********");
            ATH_MSG_DEBUG("******************************************");

            for (xAOD::TrackParticleContainer::const_iterator ipIt = m_pInDetParticleContainer->begin();
                    ipIt != m_pInDetParticleContainer->end();
                    ++ipIt)
            {
                const xAOD::TrackParticle* tp_id = *ipIt;
                std::cout << "TrackParameters = " << tp_id << std::endl;
                const Trk::Perigee pPerigee = tp_id->perigeeParameters();
                double charge_id = tp_id->charge();
                double d0_id     = pPerigee.parameters()[Trk::d0];
                double z0_id     = pPerigee.parameters()[Trk::z0];
                double phi_id    = pPerigee.parameters()[Trk::phi];
                double eta_id    = asinh(1./tan(pPerigee.parameters()[Trk::theta]));
                double pt_id     = pPerigee.pT();

                if (pt_id < m_ptLowerLimit ) continue;

                ATH_MSG_DEBUG("pt =  " << pt_id  <<
                             " eta = " << eta_id <<
                             " phi = " << phi_id <<
                              " ch = " << charge_id <<
                              " d0 = " << d0_id <<
                              " z0 = " << z0_id );
            }

            ATH_MSG_DEBUG("******************************************");
            ATH_MSG_DEBUG("********           end           *********");
            ATH_MSG_DEBUG("******************************************");
        }

        createCaloParticles();
        ATH_MSG_DEBUG("execute: size of CaloParticles " << m_caloParticles.size());
        IsolatedIdTracks(m_idR);

        this->RunFromID(pEventInfo, vxCandidatesContainer, summaryList);
    }   
    
 
    ATH_MSG_DEBUG("execute: number of summaries=" << summaryList.size());

    if (msgLvl(MSG::DEBUG))
    {
        for (MuGirlNS::CandidateSummaryList::iterator it  = summaryList.begin();
                                   it != summaryList.end();
                                   it++)
        {
            const xAOD::TrackParticle* refitted = (*it)->pRefittedTrack;
            if (refitted!=NULL) {
                ATH_MSG_DEBUG("execute: number of Degree of Freedom: " << refitted->numberDoF());
            } else {
                ATH_MSG_WARNING("execute: pointer to the RefittedTrack is NULL");
            }
        }
    }


    if (m_writeChamberT0s) recordChamberT0s();
    m_T0map.clear();
    if (m_doParticleCreator)
    {
        ATH_MSG_DEBUG("execute: calling m_pParticleCreatorTool->fillContainer");
        if (m_pParticleCreatorTool->fillContainer(&summaryList).isFailure())
        {
            ATH_MSG_DEBUG("execute: m_pParticleCreatorTool->fillContainer failed.");
            for (MuGirlNS::CandidateSummaryList::iterator itSummary = summaryList.begin(); itSummary !=summaryList.end(); itSummary++)
            {
                delete *itSummary;
            }
            summaryList.clear();
            for (unsigned i = 0; i < m_caloParticles.size(); i++)
            {
                delete m_caloParticles[i];
            }
            m_caloParticles.clear();

            m_pCandidate->candidateClear();
            m_pSegmentManager->clear();

            if (m_doHoughTransform) {
                ATH_MSG_DEBUG("After MuGirlReco n. " << this->hough_data_size() << " Hough maxima are unassociated");

                if ( this->hough_data_size() && m_doNTuple ) {
                /** put dummy candidate in the ntuple in order to deliver the Hough maxima which has been left unassociated */
                    if (m_pCandidate->fillNTuple(m_hough_data).isFailure())
                        ATH_MSG_WARNING("execute: cannot fill the unassociated maxima in the NTuple");
                }

                this->clear_hough_data();
            }

            return StatusCode::RECOVERABLE;
        }
    }
    // empty caloParticles
    for (unsigned i = 0; i < m_caloParticles.size(); i++)
    {
        delete m_caloParticles[i];
    }
    m_caloParticles.clear();


    for (MuGirlNS::CandidateSummaryList::iterator itSummary = summaryList.begin(); itSummary !=summaryList.end(); itSummary++)
    {
        delete *itSummary;
    }
    summaryList.clear();
    m_pCandidate->candidateClear();
    m_pSegmentManager->clear();


    // clear Hough data
    if (m_doHoughTransform) {
        ATH_MSG_DEBUG("After MuGirlReco n. " << this->hough_data_size() << " Hough maxima are unassociated");

        if ( this->hough_data_size() && m_doNTuple ) {
            /** put dummy candidate in the ntuple in order to deliver the Hough maxima which has been left unassociated */
            if (m_pCandidate->fillNTuple(m_hough_data).isFailure())
                ATH_MSG_WARNING("execute: cannot fill the unassociated maxima in the NTuple"); 
        }

        this->clear_hough_data();
    }

    ATH_MSG_DEBUG("execute() done");

    return StatusCode::SUCCESS;
}

bool MuGirlRecoTool::generateMuGirl(MuGirlNS::CandidateSummary* summary)
{
    double eta = 999.;
    double pt = 0.;
    if (summary->startFromEF && summary->pMuonEFTrack != NULL)
    {
       eta = (summary->pMuonEFTrack)->eta();
       pt = (summary->pMuonEFTrack)->pt();
    }
    else
    { 
       eta = (summary->pTrackParticle)->perigeeParameters().eta();
       pt = (summary->pTrackParticle)->perigeeParameters().pT();
    }

    ATH_MSG_DEBUG("generateMuGirl:  seed eta=" << eta << " and seed pT=" << pt);

    if (fabs(eta)>2.0 && (summary->numMdtHits <= 2 && summary->numCscSegs == 0 && summary->numTgcEtaHits <= 2 && summary->numTgcPhiHits <= 2 ))
        return false;
    if (fabs(eta)<2.0 && summary->numMdtHits<=2)
        return false;

    ATH_MSG_DEBUG("generateMuGirl:  eta/hits topology satisfied");

    if ((summary->innAngle == 999.) && (summary->midAngle == 999.))
        return false;
//     double nsegs = summary->numMdtSegs + summary->numCscSegs + summary->numRpcSegs + summary->numTgcSegs;
//     if (nsegs == 0)
//         return false;

    double value[8];
    double ANN[2];
    m_pANNSelectionTool->evaluateANN(summary,value,ANN);
    m_pCandidate->setAnn(ANN[0], ANN[1]);
    summary->nnBarrel = ANN[0];
    summary->nnEndCap = ANN[1];

    ATH_MSG_DEBUG(" ANN [0] "<<value[0]<< " ANN [1] "<<value[1]<< " ANN [2] "<<value[2]<< " ANN [3] "<<value[3]<< " ANN [4] "<<value[4]<< " ANN [5] "<<value[5] );
    /*
        if (value[0]>0.3 ||    // BARREL GOOD  PT<10GeV
                value[1]>0.2  ||    // BARREL BAD
                value[2]>0.3 ||    // REGION OVERLAP
                value[3]>0.25 ||     // MAGNET OVERLAP
                value[4]>0.45 ||    // ENDCAP
                value[5]>0.4 )      // FORWARD
             //   value[6] > m_barrelNNCut || //TEST BARREL PT>10GeV
             //   value[7] > m_endcapNNCut)   // TEST ENDCAP PT>10GeV
    */
   
    if (pt>5000)
    {
       /*
        if (value[0]>0.18 ||    // BARREL GOOD  PT>5GeV
            value[1]>0.2  ||    // BARREL BAD
            value[2]>0.2 ||    // REGION OVERLAP
            value[3]>0.18 ||     // MAGNET OVERLAP
       */
        if (value[0]>0.55 ||    // BARREL GOOD  PT>5GeV
            value[1]>0.55  ||    // BARREL BAD
            value[2]>0.55 ||    // REGION OVERLAP
            value[3]>0.55 ||     // MAGNET OVERLAP
            value[4]>0.2 ||    // ENDCAP
            value[5]>0.3 )      // FORWARD
        {
  
           summary->PassNNsel = true;
           m_pCandidate->setPassANN(true);
           return true;
        }
        else
        {
           summary->PassNNsel = false;
           m_pCandidate->setPassANN(false);
           return false;
        }
    }
    else
    {
        if (value[0]>0.55 ||    // BARREL GOOD  PT<5GeV
            value[1]>0.55  ||    // BARREL BAD
            value[2]>0.55 ||    // REGION OVERLAP
            value[3]>0.55 ||     // MAGNET OVERLAP
            value[4]>0.2 ||    // ENDCAP
            value[5]>0.3 )      // FORWARD
        {
  
           summary->PassNNsel = true;
           m_pCandidate->setPassANN(true);
           return true;
        }
        else
        {
           summary->PassNNsel = false;
           m_pCandidate->setPassANN(false);
           return false;
        }
    }
    
}

void MuGirlRecoTool::fillMuonSegmentColl(const std::vector<const Muon::MuonSegment*>& muonSegmentList)
{
    for (MuGirlNS::MuonSegmentList::const_iterator MuonSegItr=muonSegmentList.begin();  MuonSegItr!=muonSegmentList.end(); MuonSegItr++)
    {
        const Muon::MuonSegment* seg = *MuonSegItr;
        m_pSegmentCollection->push_back(const_cast <Muon::MuonSegment*>(seg));
        m_pSegmentManager->setInStore(seg, true);
    }
}

void MuGirlRecoTool::createCaloParticles()
{
    Amg::Vector3D pt_calo_local(0,0,0);

    ATH_MSG_DEBUG("CreateCaloParticles: Pre-selection from " << m_pInDetParticleContainer->size() << " particles.");

    int iPart = 0;
    for (xAOD::TrackParticleContainer::const_iterator ipIt = m_pInDetParticleContainer->begin();
            ipIt != m_pInDetParticleContainer->end();
            ++ipIt, iPart++)
    {
        const xAOD::TrackParticle* tp_id = *ipIt;
        const Trk::Perigee pPerigee = tp_id->perigeeParameters();
/*
        if (pPerigee == NULL)
        {
            ATH_MSG_DEBUG("Missing perigee");
            continue;
        }
*/

        if (!m_pIdTrackSelectorTool->decision(*(tp_id)))
        {
            ATH_MSG_DEBUG("Failed IdTrackSelectorTool decision");
            continue;
        }

/*
        double px = pPerigee->momentum()[Trk::px];
        double py = pPerigee->momentum()[Trk::py];
        double pt = sqrt (px*px + py*py);
        double p_id      = tp_id->p(); 
 	if (p_id<m_momentumLowerLimit) continue; 
*/
        /**
          * Cut on the pTLowerLimit reintroduced because the ID selector Tool may be configured without it
          */
        double pt = pPerigee.pT();
        if (pt < m_ptLowerLimit)
        {
            ATH_MSG_DEBUG("pT=" << pt << " is lower than limit = " << m_ptLowerLimit << ". Skipping track.");
            continue;
        }
/*
        if (tp_id->trackSummary() == NULL)
        {
            if (msgLvl(MSG::DEBUG)) msg() << "Missing trackSummary" << endreq;
            continue;
        }
        int numhits_id = tp_id->trackSummary()->get
                         (Trk::numberOfPixelHits) +
                         tp_id->trackSummary()->get
                         (Trk::numberOfSCTHits);

        if (numhits_id < m_nIDhits)
        {
            if (msgLvl(MSG::DEBUG)) msg() << "Cut on #hits=" << numhits_id << endreq;
            continue;
        }
*/
        bool hasBeenExtrapolated = false;
        const Trk::TrackParameters* extrParameters = NULL;

        /**
          * Check the perigeee parameter of the track to be extrapolated
          */
        if (msgLvl(MSG::VERBOSE)) {
            ATH_MSG_VERBOSE( "extrapolationg to muon entry layer, printing the track perigee:" );
            ATH_MSG_VERBOSE( pPerigee );
        }


        Trk::Track* muon_entry_track = 0;

        // Remeber to increase the robustness against missingtrack extrapolation once the TrackExtrapolatorTool will be fixed.
        // Temporary remove the TrackExtrapolationTool ... => 
        // if (tp_id->track()!=NULL) m_pTrackExtrapolationTool->extrapolate( *(tp_id->track()) );


        if (muon_entry_track != 0) {
            hasBeenExtrapolated = true;
            const Trk::Perigee* extPerigee = muon_entry_track->perigeeParameters();
            extrParameters = dynamic_cast<const Trk::TrackParameters*>( extPerigee );
            if (extPerigee==NULL) {
                ATH_MSG_WARNING("Cannot cast the perigee at the muon entry level to a Trk::MeasuredPerigee. Skiping the track!");
                continue;
            }
            if (msgLvl(MSG::VERBOSE)) {
                ATH_MSG_VERBOSE( "Track position at the muon entry layer:" );
                const DataVector<const Trk::TrackParameters>* track_parameters = muon_entry_track->trackParameters();
                for (unsigned int i=0; i<track_parameters->size();i++) {
                    ATH_MSG_VERBOSE( *(track_parameters->at(i)) );
                }
                ATH_MSG_VERBOSE("Perigee parameter of the extrapolated track:");
                ATH_MSG_VERBOSE( *(extPerigee) );
            }

            Amg::Vector3D ip_mom = pPerigee.momentum();
            pt_calo_local        = extPerigee->position();
            Amg::Vector3D mu_mom = extPerigee->momentum();

            double ip_pt = ip_mom.perp();
            double mu_pt = mu_mom.perp();
            double ip_p  = ip_mom.mag();
            double mu_p  = mu_mom.mag();
            double muon_entry_radius = sqrt(pt_calo_local.x()*pt_calo_local.x() + pt_calo_local.y()*pt_calo_local.y());

            ATH_MSG_DEBUG("MuonEntryExtrapolator: track extrapolated to r=" <<  muon_entry_radius << ", z=" << pt_calo_local.z());
            ATH_MSG_DEBUG("MuonEntryExtrapolator: momentum before extr. p  = " << ip_p  << ", after extr. p  = " << mu_p);
            ATH_MSG_DEBUG("MuonEntryExtrapolator: momentum before extr. pt = " << ip_pt << ", after extr. pt = " << mu_pt);
        }



        bool seenByCalo = hasBeenExtrapolated;
        if ( !hasBeenExtrapolated ) {
            if (m_pToCalo)
            {
                double offset = 0.;
                const Trk::TrackParameters* calo_local = 0;
                for (unsigned i = 0; i < m_caloLayers.size(); ++i)
                {
                    //const Trk::Track* track = *tp_id->track();
                    if ( (calo_local = m_pToCalo->extrapolate(*tp_id, m_caloLayers[i], offset, Trk::alongMomentum, Trk::muon))!=0 ) {
                        seenByCalo=true;
                        pt_calo_local = calo_local->position();
                    }
                    if (seenByCalo)
                        break;
                }
            }
        }
        else
            seenByCalo = true;

        if (!seenByCalo)
        {
            ATH_MSG_DEBUG("Not seen by calo");
            continue;
        }

        ATH_MSG_DEBUG("This TrackParticle is seen by Calo!");

        CaloParticle* pParticle = new CaloParticle(NULL, tp_id, extrParameters, pt_calo_local.eta(), pt_calo_local.phi());
        m_caloParticles.push_back(pParticle);
    }
}

void MuGirlRecoTool::IsolatedIdTracks(double dR)
{
    for (unsigned int i = 0; i < m_caloParticles.size(); i++)
    {
        CaloParticle* pParticle1 = m_caloParticles[i];
        if (!pParticle1->isolated)
            continue;
        const xAOD::TrackParticle* tp1 = pParticle1->pTrackParticle;
        const Trk::Perigee mp1 = tp1->perigeeParameters();
        double phi1 = mp1.parameters()[Trk::phi];
        double eta1 = asinh(1. / tan(mp1.parameters()[Trk::theta]));
        for (unsigned int j = i + 1; j < m_caloParticles.size(); j++)
        {
            CaloParticle* pParticle2 = m_caloParticles[j];
            if (!pParticle2->isolated)
                continue;
            const xAOD::TrackParticle* tp2 = pParticle2->pTrackParticle;
            const Trk::Perigee mp2 = tp2->perigeeParameters();
            double phi2 = mp2.parameters()[Trk::phi];
            double eta2 = asinh(1. / tan(mp2.parameters()[Trk::theta]));
            double deta = fabs(eta2 - eta1);
            double dphi = fabs(phi2 - phi1);
            if (dphi > M_PI)
                dphi = 2 * M_PI - dphi;
            if (sqrt(deta * deta + dphi * dphi) < dR)
            {
                pParticle1->isolated = false;
                pParticle2->isolated = false;
            }
        }
    }
}

void MuGirlRecoTool::calculateLHR(CaloParticle* pParticle)
{
    const xAOD::TrackParticle* pTrackParticle = pParticle->pTrackParticle;
    const Trk::Perigee pPerigee = pTrackParticle->perigeeParameters();
    double charge_id = pTrackParticle->charge();
    double qOverP_id = pPerigee.parameters()[Trk::qOverP];
    double p_id      = fabs(1.0/qOverP_id);
    double phi_id    = pPerigee.parameters()[Trk::phi];
    double eta_id    = asinh(1./tan(pPerigee.parameters()[Trk::theta]));
    double pt_id     = pPerigee.pT();

    ATH_MSG_DEBUG("Parameters of isolated tracks seen by calo: " <<
        " pt =  " << pt_id  <<
        " eta = " << eta_id <<
        " phi = " << phi_id <<
        " charge = " << charge_id);

    double theLHR = m_pMuLHR->getLHR(m_pClusCollection, eta_id, p_id, pParticle->eta, pParticle->phi);
    ATH_MSG_DEBUG("theLHR = " << theLHR);

    m_pCandidate->setIsolation(1);
    m_pCandidate->setLHR(theLHR);
}

void MuGirlRecoTool::recordChamberT0s()
{
   std::vector< std::pair < Identifier, float > > t0s;
   std::map<Identifier,const Muon::MuonSegment* >::iterator chit = m_T0map.begin();
   std::map<Identifier,const Muon::MuonSegment* >::iterator chit_end = m_T0map.end();

   for ( ;chit!=chit_end;++chit )
   {
       t0s.push_back( std::make_pair(chit->first,chit->second->time()) );
       ATH_MSG_DEBUG("Storing t0 "<<chit->second->time()<<" for chamber "<<chit->first);

   }

   m_chamberT0s = new Muon::ChamberT0s(t0s);

   if (evtStore()->record(m_chamberT0s,m_chamberT0Location).isSuccess() )
   {
      ATH_MSG_DEBUG("stored Chambert0s at "  << m_chamberT0Location);
   }
   else
      ATH_MSG_ERROR("recordChamberT0s: Failed to store Chambert0s at " << m_chamberT0Location);
}


StatusCode MuGirlRecoTool::processHoughData(){
    int NumMaxima = 0;

    if ( m_pMuonLayerHoughTool ) {
        Muon::MuonLayerHoughTool::HoughDataPerSectorVec data = m_pMuonLayerHoughTool->houghData();
        ATH_MSG_DEBUG (data.size() << " sector are present in the HoughData");
        for (unsigned int sector=0;sector<data.size();sector++) {
            int sector_id = -99;
            bool hits_in_same_sector = true;
            Muon::MuonLayerHoughTool::HoughDataPerSector sector_data = data.at(sector);
            ATH_MSG_DEBUG("----------------------------- Sector " << sector 
                       << " -----------------------------");
            for (unsigned int region=0;region<sector_data.maxVec.size();region++) {
                std::pair<Muon::MuonStationIndex::DetectorRegionIndex, Muon::MuonStationIndex::LayerIndex> coordinate =
                    Muon::MuonStationIndex::decomposeSectorLayerHash(region);
                Muon::MuonStationIndex::DetectorRegionIndex det_reg_id = coordinate.first;
                Muon::MuonStationIndex::LayerIndex          det_lay_id = coordinate.second;
                std::string det_reg_str = Muon::MuonStationIndex::regionName(det_reg_id);
                std::string det_lay_str = Muon::MuonStationIndex::layerName(det_lay_id);

                int region_id = -99;
                bool hits_in_same_region = true;
                /**  Old code used when the region was not handled directly within the MuonIdHelper package.
                std::string tag;
                switch (region) {
                  case 0 : tag="BI station        "; break;
                  case 1 : tag="BM station        "; break;
                  case 2 : tag="BO station        "; break;
                  case 3 : tag="BE station, side C"; break;
                  case 4 : tag="EI station, side C"; break;
                  case 5 : tag="EM station, side C"; break;
                  case 6 : tag="EO station, side C"; break;
                  case 7 : tag="EE station, side C"; break;
                  case 8 : tag="BE station, side A"; break;
                  case 9 : tag="EI station, side A"; break;
                  case 10: tag="EM station, side A"; break;
                  case 11: tag="EO station, side A"; break;
                  case 12: tag="EE station, side A"; break;
                  default: tag=""; break;
                }
                */
                ATH_MSG_DEBUG("------------------- RegLay " 
                              << setw(14) << setfill(' ') << setiosflags(ios::left) << det_lay_str
                              << setw(7) << setfill(' ') << setiosflags(ios::left) << det_reg_str
                           << " ------------------");
                std::vector<MuonHough::MuonLayerHough::Maximum*> region_data = sector_data.maxVec.at(region);
                for (unsigned int maximum=0;maximum<region_data.size();maximum++) {
                    MuonHough::MuonLayerHough::Maximum* max = region_data.at(maximum);
                    std::vector<MuonHough::Hit*>::iterator it = max->hits.begin();
                    int layer_id = -99;
                    bool hits_in_same_layer = true; 

                    // compute MuGirl region index and check the consistency with the regLayer of the Hough transform
                    int hough_region  = det_reg_id;
                    int MuGirl_region = (hough_region==1)? 0 : 1;

                    // the computation of MuGirl distance requires the Hough hits
                    int MuGirl_distance = 4;

                    ATH_MSG_DEBUG("----------------------------- Maxim. " << maximum
                               << " -----------------------------");
                    float max_value    = max->max;
                    float max_position = max->pos;
                    float max_tetha    = max->theta;
                    float max_tConfirm = max->triggerConfirmed;
                    float max_refPos;
                    float max_posMin;
                    float max_posMax;
                    int   dummy;
                    max->hough->pars(max->binposmin, dummy, max_refPos,max_posMin);
                    max->hough->pars(max->binposmax, dummy, max_refPos,max_posMax);

                    ATH_MSG_DEBUG("maximum value: " << max_value << ",  # of trigger hits associated: " << max_tConfirm);
                    ATH_MSG_DEBUG("reference layer:" << ((region>2)? "Z=" : "R=") << max_refPos
                               << ",  maximum position:" << ((region>2)? "R=" : "Z=") << max_position << ((region>2)? ",  R range: [" : ",  Z range: [")
                               << max_posMin << "," << max_posMax << "]");
                    ATH_MSG_DEBUG("tetha position:" << max_tetha);

                     while (it!=max->hits.end()) {
                        const MuonHough::HitDebugInfo* info = (*it)->debugInfo();

                        if (sector_id==-99) sector_id = info->sector;
                        else hits_in_same_sector &= (sector_id==info->sector);

                        if (region_id==-99) region_id = info->region;
                        else hits_in_same_region &= (region_id==info->region);

                        if (layer_id==-99)  layer_id = info->layer;
                        else hits_in_same_layer &= (layer_id==info->layer);

                        std::string side = "";
                        if ( (*it)->prd ) {
                          MuGirl_distance = det_lay_id; //m_pMuonLayerHoughTool->lay( (*it)->prd->identify() );
                          std::string chamber_name = m_MuonIdHelperTool->chamberNameString( (*it)->prd->identify() );
                          if (msgLvl(MSG::DEBUG)) {
                            const Amg::Vector2D locPos = (*it)->prd->localPosition();
                            const Trk::TrkDetElementBase* detEl = (*it)->prd->detectorElement();
                            const Amg::Vector3D* gPos = detEl->surface( (*it)->prd->identify()).localToGlobal(locPos);
                            if ( m_MuonIdHelperTool->isEndcap( (*it)->prd->identify() ) ) {
                              side = ( gPos->z() < 0. )? ", side C" : ", side A";
                            }
                            ATH_MSG_DEBUG("Hough Hit has a PrePrawData belonging to chamber " << chamber_name.c_str() << side.c_str());
                          }
                        } else {
                          if ( (*it)->tgc ) {
                            std::vector< const Muon::TgcPrepData* > tgc = (*it)->tgc->etaCluster.hitList;
                            MuGirl_distance = det_lay_id; //lay( tgc[0]->identify() );
                            if (msgLvl(MSG::DEBUG)) {
                              std::string chamber_name = m_MuonIdHelperTool->chamberNameString( tgc[0]->identify() );
                              const Amg::Vector2D locPos = tgc[0]->localPosition();
                              const Trk::TrkDetElementBase* detEl = tgc[0]->detectorElement();
                              const Amg::Vector3D* gPos = detEl->surface( tgc[0]->identify()).localToGlobal(locPos);
                              if ( m_MuonIdHelperTool->isEndcap( tgc[0]->identify() ) ) {
                                side = ( gPos->z() < 0. )? ", side C" : ", side A";
                              }
                              ATH_MSG_DEBUG("Hough Hit has not a PrepRawData but a TgcClusterObj3D belonging to " << chamber_name.c_str() << side.c_str());
                            }
                          } else              ATH_MSG_DEBUG("Hough Hit has not a PrepRawData");
                        }

                        // set MuGirl_distance
                        if (MuGirl_distance>=1) MuGirl_distance++;
                        if (MuGirl_distance==4) MuGirl_distance=1;
                        /** Old check done when the region index was not handled within the MuonIdhelper package.
                        // check consistency with MuGirl indices for every hits
                        if ( (MuGirl_distance==0 && (region!=0&&region!=4&&region!=9))  ||
                             (MuGirl_distance==2 && (region!=1&&region!=5&&region!=10)) ||
                             (MuGirl_distance==3 && (region!=2&&region!=6&&region!=11)) ||
                             (MuGirl_distance==1 && (region!=3&&region!=7&&region!=8&&region!=12)) ) {
                          ATH_MSG_ERROR("Inconsistency between MuGirl distance and Hough transform regLayer: distance=" << MuGirl_distance
                                     << ", regLayer=" << region);
                          return StatusCode::FAILURE;
                        }
                        */
                        it++;
                    }

                    ATH_MSG_DEBUG("MuGirl distance=" << MuGirl_distance << ", MuGirl region=" << MuGirl_region);

                    // keep maximum for the ntuple processing (check that we have
                    m_hough_data[sector][MuGirl_distance][MuGirl_region].push_back( max );

                    ++NumMaxima;

                    if ( !hits_in_same_layer ) ATH_MSG_WARNING("processHoughData: hits in Hough are not from the same layer");
                    else if ( layer_id!=-99) ATH_MSG_DEBUG("Hough maximum has hits from layer " << layer_id);
                }
                if ( !hits_in_same_region ) ATH_MSG_WARNING("processHoughData: hits in Hough are not from the same region");
                else if (region_id!=-99) ATH_MSG_DEBUG("Hough maxima belongs to region " << region_id);
            }
            if ( !hits_in_same_sector ) ATH_MSG_WARNING("processHoughData: hits in Hough are not from the same sector");
            else if (sector_id!=-99) ATH_MSG_DEBUG("Hough maxima belongs to sector " << sector_id);
         }
    }

    ATH_MSG_INFO("Retrieved n. " << NumMaxima << " Hough maxima"); 

    return StatusCode::SUCCESS;
}

void MuGirlRecoTool::associateMaxima(HoughData associated[16][4][2]){
    std::vector<MuonHough::MuonLayerHough::Maximum*> maxvec;
    for (unsigned int i=0;i<16;i++) {    // i == sector
      for (unsigned int j=0;j<4;j++) {   // j == distance
        for (unsigned int k=0;k<2;k++) { // k == region
          std::vector<MuonHough::MuonLayerHough::Maximum*>& maxima = m_hough_data[i][j][k];
          std::vector<MuonHough::MuonLayerHough::Maximum*>::iterator  m = maxima.begin();
          while (m!=maxima.end()) {
            if ( m_pCandidate->isHoughMaxAssociated(*m,i,j,k) ) {
              associated[i][j][k].push_back(*m);
              m = maxima.erase(m);
            } else m++;
          }
        }
      }
    }
}

int MuGirlRecoTool::hough_data_size() const {
    int size = 0;
    for (unsigned int i=0;i<16;i++) {
      for (unsigned int j=0;j<4;j++) {
        for (unsigned int k=0;k<2;k++) {
          size += m_hough_data[i][j][k].size();
        }
      }
    }
    return size;
}

void MuGirlRecoTool::clear_hough_data() {
    for (unsigned int i=0;i<16;i++) {
      for (unsigned int j=0;j<4;j++) {
        for (unsigned int k=0;k<2;k++) {
            m_hough_data[i][j][k].clear();
        }
      }
    }
}



