/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CandidateTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuGirlCandidate/CandidateTool.h"
#include "MuGirlCandidate/Chamber.h"
#include "MuGirlCandidate/CSC.h"
#include "MuGirlCandidate/MDT.h"
#include "MuGirlCandidate/RPC.h"
#include "MuGirlCandidate/TGC.h"
#include "MuGirlCandidate/sTGC.h"
#include "MuGirlCandidate/MM.h"
#include "MuGirlCandidate/Candidate.h"
#include "MuGirlCandidate/CandidateNtuple.h"
#include "MuGirlCandidate/StationManager.h"
#include "MuGirlCandidate/Intersection.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "CscClusterization/ICscClusterBuilder.h"
#include "TrkExInterfaces/IIntersector.h"
#include "MuidInterfaces/IMuidCaloEnergy.h"
#include "IRegionSelector/IRegSelSvc.h"
//#include "GaudiKernel/INTupleSvc.h"
#include "xAODTracking/TrackParticle.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonHoughPatternTools/MuonLayerHoughTool.h"

namespace MuGirlNS
{

//================ Constructor =================================================

CandidateTool::CandidateTool(const std::string& t,
                             const std::string& n,
                             const IInterface*  p ) :
        AthAlgTool(t,n,p),
        m_doDecoding(true),
        m_doCSC(true),
        m_doMDT(true),
        m_doRPC(true),
        m_doTGC(true),
        m_dosTGC(false),
        m_doMM(false),
        m_doMdtHough(true),
        m_doSegments(true),
        m_sCscPrepDataCollection("CSC_Clusters"),
        m_sMdtPrepDataCollection("MDT_DriftCircles"),
        m_sRpcPrepDataCollection("RPC_Measurements"),
        m_sTgcPrepDataCollection("TGC_Measurements"),
        m_sStgcPrepDataCollection("STGC_Measurements"),
        m_sMmPrepDataCollection("MM_Measurements"),
        //        m_ntupleName("/NTUPLES/MUGIRL/MUGIRL"),
        //        m_ntupleTitle("Muon Candidates"),
        m_hitDeltaEta(0.02),
        m_roadDeltaPhi(0.2),
        m_roadScaleFactor(1.),
        m_pCscSegmentMaker("Csc4dSegmentMaker"),
        m_pMdtSegmentMaker("Muon::DCMathSegmentMaker"),
        m_pRpcSegmentMaker("RPC_LinearSegmentMakerTool"),
        m_pTgcSegmentMaker("TGC_LinearSegmentMakerTool"),
        m_pCscClusterProviderTool("CscThresholdClusterBuilderTool"),
        m_pCscRdoToPrepDataTool("Muon::CscRdoToCscPrepDataTool/CscPrepDataProviderTool"),
        m_pMdtRdoToPrepDataTool("Muon::MdtRdoToPrepDataTool/MdtPrepDataProviderTool"),
        m_pRpcRdoToPrepDataTool("Muon::RpcRdoToPrepDataTool/RpcPrepDataProviderTool"),
        m_pTgcRdoToPrepDataTool("Muon::TgcRdoToPrepDataTool/TgcPrepDataProviderTool"),
        m_pMdtDriftCircleCreator("Muon::MdtDriftCircleOnTrackCreator"),
        m_pMuonClusterCreator("MuonClusterOnTrackCreator"),
        m_pCscClusterCreator("CscClusterOnTrackCreator"),
        m_pIntersector("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
        m_pCaloEnergyTool("Rec::MuidCaloEnergyTool/MuidCaloEnergyTool"),
        //        m_pNtupleSvc("NtupleSvc", name()),
        m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
        m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
        m_segmentSelectionTool("Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"),
        m_pMuonLayerHoughTool("Muon::MuonLayerHoughTool/MuonLayerHoughTool"),
        m_pRegionSelector("RegSelSvc", name()),
        m_pStationMgr(NULL),
        m_pCandidate(NULL),
        m_pCandidateNtuple(NULL)
{
    declareInterface<ICandidateTool>(this);

    declareProperty("doDecoding",                   m_doDecoding);
    declareProperty("doCSC",                        m_doCSC);
    declareProperty("doMDT",                        m_doMDT);
    declareProperty("doRPC",                        m_doRPC);
    declareProperty("doTGC",                        m_doTGC);
    declareProperty("dosTGC",                       m_dosTGC);
    declareProperty("doMM",                         m_doMM);
    declareProperty("doMdtHough",                   m_doMdtHough, "Call the MDT segment maker");
    declareProperty("doSegments",                   m_doSegments);
    //    declareProperty("doNTuple",                     m_doNTuple);
    declareProperty("CscPrepDataCollection",        m_sCscPrepDataCollection);
    declareProperty("MdtPrepDataCollection",        m_sMdtPrepDataCollection);
    declareProperty("RpcPrepDataCollection",        m_sRpcPrepDataCollection);
    declareProperty("TgcPrepDataCollection",        m_sTgcPrepDataCollection);
    declareProperty("sTgcPrepDataCollection",       m_sStgcPrepDataCollection);
    declareProperty("MmPrepDataCollection",         m_sMmPrepDataCollection);
    declareProperty("CscClusterProviderTool",       m_pCscClusterProviderTool);
    declareProperty("CscRdoToPrepDataTool",         m_pCscRdoToPrepDataTool);
    declareProperty("MdtRdoToPrepDataTool",         m_pMdtRdoToPrepDataTool);
    declareProperty("RpcRdoToPrepDataTool",         m_pRpcRdoToPrepDataTool);
    declareProperty("TgcRdoToPrepDataTool",         m_pTgcRdoToPrepDataTool);
    //    declareProperty("NtupleName",                   m_ntupleName);
    //    declareProperty("NtupleTitle",                  m_ntupleTitle);
    declareProperty("HitDeltaEta",                  m_hitDeltaEta);
    declareProperty("RoadDeltaPhi",                 m_roadDeltaPhi);
    declareProperty("RoadScaleFactor",              m_roadScaleFactor);
    declareProperty("CscSegmentMaker",              m_pCscSegmentMaker);
    declareProperty("MdtSegmentMaker",              m_pMdtSegmentMaker);
    declareProperty("RpcSegmentMaker",              m_pRpcSegmentMaker);
    declareProperty("TgcSegmentMaker",              m_pTgcSegmentMaker);
    declareProperty("MdtDriftCircleOnTrackCreator", m_pMdtDriftCircleCreator);
    declareProperty("MuonClusterOnTrackCreator",    m_pMuonClusterCreator);
    declareProperty("CscClusterOnTrackCreator",     m_pCscClusterCreator);
    declareProperty("Intersector",                  m_pIntersector);
    declareProperty("MuidCaloEnergy",               m_pCaloEnergyTool);
    declareProperty("RegionSelector",               m_pRegionSelector);
    declareProperty("SegmentSelectionTool",         m_segmentSelectionTool);
    declareProperty("MuonLayerHoughTool",           m_pMuonLayerHoughTool);
    //    declareProperty("NTupleService",                m_pNtupleSvc);
}

//================ Destructor =================================================

CandidateTool::~CandidateTool()
{}


//================ Initialisation =================================================

StatusCode CandidateTool::initialize()
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endmsg;
    StatusCode sc = AthAlgTool ::initialize();
    if (sc.isFailure())
        return sc;

    // initialize MuonDetectorManager
    if (detStore()->retrieve(m_pMuonMgr).isFailure() || m_pMuonMgr == NULL)
    {
        msg(MSG::ERROR) << "Cannot retrieve MuonDetectorManager" << endmsg;
        return StatusCode::RECOVERABLE;
    }
    if (m_doCSC)
    {
        if (retrieve(m_pCscSegmentMaker).isFailure())
            return StatusCode::RECOVERABLE;
        if (!m_pCscRdoToPrepDataTool.empty())
            if (retrieve(m_pCscRdoToPrepDataTool).isFailure())
               return StatusCode::RECOVERABLE;
        if (retrieve(m_pCscClusterProviderTool).isFailure())
            return StatusCode::RECOVERABLE;
        if (retrieve(m_pCscClusterCreator).isFailure())
            return StatusCode::RECOVERABLE;
    }
    if (m_doMdtHough)
    {
        if (retrieve(m_pMdtSegmentMaker).isFailure())
            return StatusCode::RECOVERABLE;
        if (!m_pMdtRdoToPrepDataTool.empty())
            if (retrieve(m_pMdtRdoToPrepDataTool).isFailure())
                return StatusCode::RECOVERABLE;

    }
    if (retrieve(m_pRpcSegmentMaker).isFailure())
        return StatusCode::RECOVERABLE;
    if (retrieve(m_pTgcSegmentMaker).isFailure())
        return StatusCode::RECOVERABLE;
    if (!m_pRpcRdoToPrepDataTool.empty())
       if (retrieve(m_pRpcRdoToPrepDataTool).isFailure())
           return StatusCode::RECOVERABLE;
    if (!m_pTgcRdoToPrepDataTool.empty())
       if (retrieve(m_pTgcRdoToPrepDataTool).isFailure())
           return StatusCode::RECOVERABLE;

    if (retrieve(m_pMdtDriftCircleCreator).isFailure())
        return StatusCode::RECOVERABLE;
    if (retrieve(m_pMuonClusterCreator).isFailure())
        return StatusCode::RECOVERABLE;
    if (retrieve(m_pIntersector).isFailure())
        return StatusCode::RECOVERABLE;
    if (retrieve(m_pCaloEnergyTool).isFailure())
        return StatusCode::RECOVERABLE;
    if (retrieve(m_pRegionSelector).isFailure())
        return StatusCode::RECOVERABLE;
    //    if (retrieve(m_pNtupleSvc).isFailure())
    //        return StatusCode::RECOVERABLE;
    if (retrieve(m_helperTool).isFailure())
        msg(MSG::WARNING) << "Could not retrieve EDMHelperTool " << endmsg;
    if (retrieve(m_idHelperTool).isFailure()) {
        msg(MSG::WARNING) << "Could not retrieve the IdHelperTool " << endmsg;
        return StatusCode::FAILURE;
    }
    if (retrieve(m_segmentSelectionTool).isFailure())
        return StatusCode::RECOVERABLE;

    if ( m_pMuonLayerHoughTool.retrieve().isFailure() ) {
        msg(MSG::WARNING) << "Could not retrieve the MuonLayerHoughTool" << endmsg;
        msg(MSG::WARNING) << "Info from the Hough transform will not be in the MuGirl ntuple" << endmsg;
        return StatusCode::RECOVERABLE;
    } else {
        msg(MSG::INFO) << "MuonLayerHoughTool successfully retrieved" << endmsg;
    }

    // initialize distances
    m_distances[INNER_DIST]  = new Distance(INNER_DIST,  4200.,  6800.,  500.,  6500.,    0.,  7400.);
    m_distances[EE_DIST]     = new Distance(EE_DIST,     4200.,  9800., 5400.,  8800., 8700., 11700.);
    m_distances[MIDDLE_DIST] = new Distance(MIDDLE_DIST, 6500., 13000., 1700., 12000.,    0., 10000.);
    m_distances[OUTER_DIST]  = new Distance(OUTER_DIST,  9000., 19000., 2900., 12100.,    0., 13000.);

    // initilaize technologies
    m_technologies[CSC_TECH]  = new CSC(this, m_sCscPrepDataCollection);
    m_technologies[MDT_TECH]  = new MDT(this, m_sMdtPrepDataCollection);
    m_technologies[RPC_TECH]  = new RPC(this, m_sRpcPrepDataCollection);
    m_technologies[TGC_TECH]  = new TGC(this, m_sTgcPrepDataCollection);
    m_technologies[STGC_TECH] = new sTGC(this, m_sStgcPrepDataCollection);
    m_technologies[MM_TECH]   = new MM (this, m_sMmPrepDataCollection);

    m_pStationMgr = new StationManger(this);
    m_pCandidate  = new Candidate(this);

    msg(MSG::INFO) << "initialize() successful in " << name() << endmsg;
    return StatusCode::SUCCESS;
} 

//================ Finalisation =================================================

StatusCode CandidateTool::finalize()
{
    msg(MSG::INFO) << "Finalizing " << name() << " - package version " << PACKAGE_VERSION << endmsg;
    m_pCandidate->clear();
    delete m_distances[INNER_DIST];
    delete m_distances[EE_DIST];
    delete m_distances[MIDDLE_DIST];
    delete m_distances[OUTER_DIST];

    delete m_technologies[CSC_TECH];
    delete m_technologies[MDT_TECH];
    delete m_technologies[RPC_TECH];
    delete m_technologies[TGC_TECH];

    delete m_pStationMgr;
    delete m_pCandidate;
    if (m_pCandidateNtuple != NULL)
        delete m_pCandidateNtuple;

    StatusCode sc = AlgTool::finalize();
    return sc;
}

void CandidateTool::setPassANN(bool pass)
{
    m_pCandidate->setPassANN(pass);
}
void CandidateTool::setPassdoStau(bool pass)
{
    m_pCandidate->setPassdoStau(pass);
}
void CandidateTool::setAnn(double barrelNN,double endcapNN)
{
    m_pCandidate->setAnn(barrelNN,endcapNN);
}

void CandidateTool::setLHR(double lhr)
{
    m_pCandidate->setLHR(lhr);
}

double CandidateTool::innerAngle() const
{
    return m_pCandidate->innerAngle();
}

double CandidateTool::middleAngle() const
{
    return m_pCandidate->middleAngle();
}

void CandidateTool::setIsolation(int isolation)
{
    m_pCandidate->setIsolation(isolation);
}

StatusCode CandidateTool::bookNTuple(NTuple::Tuple* pNTuple)
{
    m_pCandidateNtuple = new CandidateNtuple(msg(),*m_idHelperTool);
    return m_pCandidateNtuple->book(pNTuple);
}

StatusCode CandidateTool::fillNTupleEventInfo(unsigned nRunNumber, unsigned nEventNumber)
{
    return m_pCandidateNtuple->fillEventInfo(nRunNumber, nEventNumber);
}

StatusCode CandidateTool::fillNTuple()
{
    return m_pCandidateNtuple->fillCandidate(m_iTrack, m_pCandidate);
}

StatusCode CandidateTool::fillNTuple(std::vector<MuonHough::MuonLayerHough::Maximum*> hough_data[16][4][2])
{
    return m_pCandidateNtuple->fillHoughData(m_pCandidate, hough_data);   
}

//============================================================================================
StatusCode CandidateTool::fill(int iTrack, const xAOD::TrackParticle* pTrackParticle, const Trk::TrackParameters* extrMuonEntry, bool bSaveMdtSegmentMakerInfo)
{
    if (msgLvl(MSG::DEBUG)) msg() << "in CandidateTool::fill" << endmsg;

    m_iTrack = iTrack;

    // Initialize hit containers
    for (unsigned iTech = 0; iTech < TECHNOLOGIES; iTech++)
    {
        TechnologyType eTech = (TechnologyType)iTech;
        if (!m_doCSC  && eTech  == CSC_TECH) continue; 
        if (msgLvl(MSG::DEBUG)) msg() << "CandidateTool::fill - Calling m_technologies["
            << TechnologyTypeName((TechnologyType)iTech) << "]->retrievePrepData()" << endmsg;
        if (m_technologies[iTech]->retrievePrepData().isFailure())
            continue;
    }

    const Trk::TrackParameters* starting_seed = (extrMuonEntry!=NULL)? extrMuonEntry :
                                          dynamic_cast<const Trk::TrackParameters*>( &pTrackParticle->perigeeParameters() );
    //const Trk::Perigee pPerigee = pTrackParticle->perigeeParameters();

    if (msgLvl(MSG::DEBUG)) msg() << "Extrapolation to Muon Entry: "
        << " q/p = " << starting_seed->parameters()[Trk::qOverP]
        << " eta = " << starting_seed->eta()
        << " phi = " << starting_seed->parameters()[Trk::phi]
        << " pt  = " << starting_seed->pT() << endmsg;

    m_pCandidate->clear();
    m_pCandidate->setSaveMdtSegmentMakerInfo(bSaveMdtSegmentMakerInfo);
    m_pCandidate->setIDPerigee(starting_seed);
    m_pCandidate->setIDTrk(pTrackParticle);
    m_pCandidate->setIsolation(0);
    StatusCode sc = m_pCandidate->crudeExtrapolate();
    if(sc != StatusCode::SUCCESS) return StatusCode::RECOVERABLE;
    m_pCandidate->selectChambers();

    // Inner
    if (m_doTGC)
    {
        m_pCandidate->buildSegments(TGC_TECH, INNER_DIST, ENDCAP_REGION, ID_TECH);
        m_pCandidate->computeChamberIntersections(TGC_TECH, INNER_DIST, INNER_DIST,  ENDCAP_REGION);
        m_pCandidate->computeChamberIntersections(TGC_TECH, INNER_DIST, MIDDLE_DIST, ENDCAP_REGION);
    }
    if (m_doCSC)
    {
        m_pCandidate->buildSegments(CSC_TECH, INNER_DIST, ENDCAP_REGION, ID_TECH);
        m_pCandidate->computeChamberIntersections(CSC_TECH, INNER_DIST, INNER_DIST,  ENDCAP_REGION);
        m_pCandidate->computeChamberIntersections(CSC_TECH, INNER_DIST, MIDDLE_DIST, ENDCAP_REGION);
    }
    if (m_doMDT)
    {
        m_pCandidate->buildSegments(MDT_TECH, INNER_DIST, BARREL_REGION, ID_TECH);
        m_pCandidate->computeChamberIntersections(MDT_TECH, INNER_DIST, INNER_DIST,  BARREL_REGION);
        m_pCandidate->computeChamberIntersections(MDT_TECH, INNER_DIST, MIDDLE_DIST, BARREL_REGION);
        m_pCandidate->buildSegments(MDT_TECH, INNER_DIST, ENDCAP_REGION, ID_TECH);   // temporary fix for problems in the TGC intersection
        //m_pCandidate->buildSegments(MDT_TECH, INNER_DIST, ENDCAP_REGION, TGC_TECH);   
        m_pCandidate->computeChamberIntersections(MDT_TECH, INNER_DIST, INNER_DIST,  ENDCAP_REGION);
        m_pCandidate->computeChamberIntersections(MDT_TECH, INNER_DIST, MIDDLE_DIST, ENDCAP_REGION);
    }
    if (m_dosTGC)
    {
       m_pCandidate->buildSegments(STGC_TECH, INNER_DIST, ENDCAP_REGION, ID_TECH);
    }
    if (m_doMM)
    {
       m_pCandidate->buildSegments(MM_TECH, INNER_DIST, ENDCAP_REGION, ID_TECH);
    }

    m_pCandidate->computeCombinedIntersection(INNER_DIST, BARREL_REGION);
    m_pCandidate->computeCombinedIntersection(INNER_DIST, ENDCAP_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, INNER_DIST, INNER_DIST,  BARREL_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, INNER_DIST, EE_DIST,     BARREL_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, INNER_DIST, MIDDLE_DIST, BARREL_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, INNER_DIST, INNER_DIST,  ENDCAP_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, INNER_DIST, EE_DIST,     ENDCAP_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, INNER_DIST, MIDDLE_DIST, ENDCAP_REGION);

    // EE
    if (m_doMDT)
    {
        m_pCandidate->buildSegments(MDT_TECH, EE_DIST, BARREL_REGION, COMB_TECH);
        m_pCandidate->computeChamberIntersections(MDT_TECH, EE_DIST, EE_DIST, BARREL_REGION);
        m_pCandidate->buildSegments(MDT_TECH, EE_DIST, ENDCAP_REGION, COMB_TECH);
        m_pCandidate->computeChamberIntersections(MDT_TECH, EE_DIST, EE_DIST, ENDCAP_REGION);
    }

    // Middle
    if (m_doTGC)
    {
        m_pCandidate->buildSegments(TGC_TECH, MIDDLE_DIST, ENDCAP_REGION, COMB_TECH);
        m_pCandidate->computeChamberIntersections(TGC_TECH, MIDDLE_DIST, MIDDLE_DIST, ENDCAP_REGION);
        m_pCandidate->computeChamberIntersections(TGC_TECH, MIDDLE_DIST, OUTER_DIST,  ENDCAP_REGION);
    }
    if (m_doRPC)
    {
        m_pCandidate->buildSegments(RPC_TECH, MIDDLE_DIST, BARREL_REGION, COMB_TECH);
        m_pCandidate->computeChamberIntersections(RPC_TECH, MIDDLE_DIST, MIDDLE_DIST, BARREL_REGION);
        m_pCandidate->computeChamberIntersections(RPC_TECH, MIDDLE_DIST, OUTER_DIST,  BARREL_REGION);
    }
    if (m_doMDT)
    {
        m_pCandidate->buildSegments(MDT_TECH, MIDDLE_DIST, BARREL_REGION, RPC_TECH);
        m_pCandidate->computeChamberIntersections(MDT_TECH, MIDDLE_DIST, MIDDLE_DIST, BARREL_REGION);
        m_pCandidate->computeChamberIntersections(MDT_TECH, MIDDLE_DIST, OUTER_DIST,  BARREL_REGION);
        m_pCandidate->buildSegments(MDT_TECH, MIDDLE_DIST, ENDCAP_REGION, TGC_TECH);
        m_pCandidate->computeChamberIntersections(MDT_TECH, MIDDLE_DIST, MIDDLE_DIST, ENDCAP_REGION);
        m_pCandidate->computeChamberIntersections(MDT_TECH, MIDDLE_DIST, OUTER_DIST,  ENDCAP_REGION);
    }
    m_pCandidate->computeCombinedIntersection(MIDDLE_DIST, BARREL_REGION);
    m_pCandidate->computeCombinedIntersection(MIDDLE_DIST, ENDCAP_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, MIDDLE_DIST, MIDDLE_DIST, BARREL_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, MIDDLE_DIST, OUTER_DIST,  BARREL_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, MIDDLE_DIST, INNER_DIST,  BARREL_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, MIDDLE_DIST, MIDDLE_DIST, ENDCAP_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, MIDDLE_DIST, OUTER_DIST,  ENDCAP_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, MIDDLE_DIST, INNER_DIST,  ENDCAP_REGION);

    // Outer
    if (m_doRPC)
    {
        m_pCandidate->buildSegments(RPC_TECH, OUTER_DIST, BARREL_REGION, COMB_TECH);
        m_pCandidate->computeChamberIntersections(RPC_TECH, OUTER_DIST, OUTER_DIST, BARREL_REGION);
    }
    if (m_doMDT)
    {
        m_pCandidate->buildSegments(MDT_TECH, OUTER_DIST, BARREL_REGION, COMB_TECH);
        m_pCandidate->buildSegments(MDT_TECH, OUTER_DIST, ENDCAP_REGION, COMB_TECH);
        m_pCandidate->computeChamberIntersections(MDT_TECH, OUTER_DIST, OUTER_DIST, BARREL_REGION);
        m_pCandidate->computeChamberIntersections(MDT_TECH, OUTER_DIST, OUTER_DIST, ENDCAP_REGION);
    }
    m_pCandidate->computeCombinedIntersection(OUTER_DIST, BARREL_REGION);
    m_pCandidate->computeCombinedIntersection(OUTER_DIST, ENDCAP_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, OUTER_DIST, OUTER_DIST, BARREL_REGION);
    m_pCandidate->computeChamberIntersections(COMB_TECH, OUTER_DIST, OUTER_DIST, ENDCAP_REGION);

    m_pCandidate->computeAngles();

    return StatusCode::SUCCESS;
}

//StatusCode CandidateTool::fillSummary(CandidateSummary* pSummary,const Trk::MeasuredPerigee* pPerigee)
StatusCode CandidateTool::fillSummary(MuGirlNS::CandidateSummary* pSummary)
{
    return m_pCandidate->fillSummary(pSummary);
}

void CandidateTool::fillRefittedTrack(const xAOD::TrackParticle* pRefittedTrack, const Trk::Track* trkRefittedTrack, CandidateSummary* pSummary)
{
    m_pCandidate->setRefittedTrackToSummary(pRefittedTrack,trkRefittedTrack,pSummary);
    if (pRefittedTrack!=NULL) 
    {
        ATH_MSG_DEBUG("fillRefittedTrack: old running schema, using TrackParticle");
        m_pCandidate->setRefittedTrack(pRefittedTrack);   /** Old running schema, using TrackParticle */
    }
    else
    {
        ATH_MSG_DEBUG("fillRefittedTrack: new running schema, using Trk::Track");
        m_pCandidate->setRefittedTrack(trkRefittedTrack); /** New running schema, using Trk::Track */
    }
}
void CandidateTool::fillMSTrack
  (std::unique_ptr<const xAOD::TrackParticle> pRefittedTrack,
   std::unique_ptr<const Trk::Track> trkRefittedTrack,
   CandidateSummary* pSummary)
{
  m_pCandidate->setMSTrackToSummary(std::move(pRefittedTrack),
                                    std::move(trkRefittedTrack),pSummary);
    if (pRefittedTrack!=NULL)
    {
        ATH_MSG_DEBUG("fillMSTrack: old running schema, using TrackParticle");
	//m_pCandidate->setMSTrack(pRefittedTrack.release());    /** Old running schema, using TrackParticle */
    }
    else
    {
        ATH_MSG_DEBUG("fillMSTrack: new running schema, using Trk::Track"); 
        //m_pCandidate->setMSTrack(trkRefittedTrack.release());  /** New running schema, using Trk::Track */
    }
}
const std::vector<MdtSegmentMakerInfo*>& CandidateTool::getMdtSegmentMakerInfo() const
{
    return m_pCandidate->getMdtSegmentMakerInfo();
}

const MuGirlNS::MuonSegmentList& CandidateTool::vectorMuonSegments() const
{
    return m_pCandidate->muonSegments();
}

const Trk::TrackParameters* CandidateTool::getPerigee() const
{
    return m_pCandidate->IDPerigee();
}

const RIO_OnTrackLists& CandidateTool::getRpcHitsInSegments()
{
    return m_pCandidate->getRpcHitsInSegments();
}

const RIO_OnTrackLists& CandidateTool::getTgcHitsInSegments()
{
    return m_pCandidate->getTgcHitsInSegments();
}


void CandidateTool::candidateClear()
{
    m_pCandidate->clear();
}

void CandidateTool::setSegmentManager(SegmentManager* pSegmentManager)
{
    m_pCandidate->setSegmentManager(pSegmentManager);
}


bool CandidateTool::hasInnerStationSegments()
{
    return m_pCandidate->hasInnerStationSegments();
}

bool CandidateTool::isHoughMaxAssociated(MuonHough::MuonLayerHough::Maximum* max, int sector, int distance, int region) {

    MuGirlNS::DistanceType dist = static_cast<MuGirlNS::DistanceType>(distance);
    MuGirlNS::RegionType    reg = static_cast<MuGirlNS::RegionType>(region);

    ATH_MSG_VERBOSE("Starting associating MuonLayerHough::Maximum in sector=" << sector 
                 << " distance=" << DistanceTypeName(dist) << " region=" << RegionTypeName(reg) );

    // collect Candidate chambers from the same MuonSpectrometer region where the Hough maximum was found
    std::vector<Chamber*> chambers;
    for (int tech=MDT_TECH;tech<TECHNOLOGIES;tech++) {
      const ChamberList& clist = m_pCandidate->cellChambers(static_cast<MuGirlNS::TechnologyType>(tech), dist, reg);
      for (unsigned int ch=0;ch<clist.size();ch++) {
        std::string chamber_name = m_idHelperTool->chamberNameString( (clist[ch])->id() );
        if ( chamber_name[0]!='T' ) { // for chambers != TGC check also the sector position
          bool isSmall = chamber_name[2]=='S';
          int stationPhi = m_idHelperTool->stationPhi( (clist[ch])->id() );
          int chamber_sector = (isSmall)? stationPhi*2-1 : stationPhi*2-2;
          if (sector != chamber_sector) continue;
        }
        chambers.push_back( clist[ch] );
      }
    }

    ATH_MSG_VERBOSE("Going to check the data (hit and intersections) of " << chambers.size() << " chambers.");


    // compute eta position and eta_width of the Hough maximum
    float reference_position;
    float posmin;
    float posmax;
    int   dummy = 0;
    max->hough->pars(max->binposmin, dummy, reference_position,posmin);
    max->hough->pars(max->binposmax, dummy, reference_position,posmax);

    float eta_sign = +1.;
    if ( (region==0 && max->pos<0.) || (region==1 && reference_position<0.) ) eta_sign = -1.;

    float max_eta = (region)? -eta_sign*log( tan ( atan2(fabsf(max->pos),fabsf(reference_position))/2. ) ) :
                              -eta_sign*log( tan ( atan2(fabsf(reference_position),fabsf(max->pos))/2. ) );

    float DetaMin = 999.;  // minimum delta eta between fine intersection and Hough maximum


    // reset some hits counters
    float numHitMuGirl = 0;
    float numHitHough  = 0;
    float numHitAssoc  = 0;


    for (unsigned int ch=0;ch<chambers.size();ch++) {

      ATH_MSG_VERBOSE("  checking chamber " << chambers.at(ch)->toString());
      // retrieved the associated intersection
      MuGirlNS::TechnologyType origTech = ID_TECH;
      if( dist==EE_DIST ) origTech = COMB_TECH;
      else if ( dist==MIDDLE_DIST && chambers.at(ch)->technologyType()==MDT_TECH && region==ENDCAP_REGION) origTech = TGC_TECH;
      else if ( dist==MIDDLE_DIST && chambers.at(ch)->technologyType()==MDT_TECH && region==BARREL_REGION) origTech = RPC_TECH;
      else if ( dist==MIDDLE_DIST ) origTech = COMB_TECH;
      else if ( dist==OUTER_DIST  ) origTech = COMB_TECH;
      Intersection* pIsect = chambers.at(ch)->intersection(origTech, Chamber::SAME_DISTANCE);
      if (pIsect == NULL) {
        origTech = COMB_TECH;
        pIsect = chambers.at(ch)->intersection(origTech, Chamber::OTHER_DISTANCE);
      }
      if (pIsect == NULL) {
        origTech = ID_TECH;
        pIsect = chambers.at(ch)->intersection(origTech, Chamber::SAME_DISTANCE);
      }

      // compute delta eta between the chamber intersection and the Hough maximum
      float deta = std::abs(max_eta - pIsect->position().eta());
      if ( deta<DetaMin ) DetaMin = deta;

      ATH_MSG_VERBOSE("    retrieved intersection " << pIsect->toString() << ",  Hough maximum Deta=" << deta);

      // loop ever the chamber hits to see if the Hough hit are associated
      HitSet& hits = chambers[ch]->hits();
      //HitSet::iterator hit = hits.begin();

      for (HitSet::iterator hit=hits.begin(); hit!=hits.end(); hit++) {
      //while ( hit!=hits.end() ) {

        // only hough maxima along eta are checked
        if (! (*hit)->isEta() ) continue;   

        ATH_MSG_VERBOSE("    checking hit " << (*hit)->toString());

        Identifier hit_id = (*hit)->RIO()->prepRawData()->identify();
        std::vector<MuonHough::Hit*>::iterator hough_hit = max->hits.begin();

          // loop over the hough hits
          while (hough_hit!=max->hits.end() ) {

            if ( (*hough_hit)->prd ) {
              numHitHough++;
              Identifier id = (*hough_hit)->prd->identify();
              if (id==hit_id) {
                numHitAssoc++;
                return true;
              }
            }

            if ( (*hough_hit)->tgc ) {
              std::vector< const Muon::TgcPrepData* > tgc = (*hough_hit)->tgc->etaCluster.hitList;
              for (unsigned int tgc_hit=0; tgc_hit<tgc.size(); tgc_hit++) {
                numHitHough++;
                Identifier id = tgc[tgc_hit]->identify();
                if (id==hit_id) {
                  numHitAssoc++;
                  return true;
                }
              }
            }

            hough_hit++;

          }

        //hit++;
        numHitMuGirl++;
      }
    }

    if ( numHitHough!= 0. && numHitAssoc/numHitHough > 0.25 ) return true;
    if ( DetaMin < 0.1 ) return true;


    return false;
}



}
