/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StauTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "MuGirlStau/StauTool.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"
#include "MuGirlInterfaces/SegmentManager.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "MuGirlStau/IStauBetaTofTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODTracking/ParticleCaloExtension.h"
#include "PathResolver/PathResolver.h"

#include "MuGirlStau/StauNTuple.h"
#include "MuGirlStau/StauFcn.h"
#include "MuGirlStau/StauCosmicFcn.h"
#include "MuGirlStau/StauCollection.h"
#include "TrkEventPrimitives/FitQuality.h"
//#include "Particle/TrackParticle.h"

#include "MuGirlStau/StauRPC.h"
#include "MuGirlStau/StauMDT.h"
#include "MuGirlStau/StauTileCal.h"
#include "MuGirlStau/StauGF.h"
#include "MuGirlStau/StauMDTT.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"
#include "TFile.h"

namespace MuGirlNS
{
//================ Constructor =================================================

StauTool::StauTool(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t, n, p), 
  m_addMuToF(true), m_rpcBugFix(false), m_rpcTimeShift(5.),
                m_tileEnergyCut(0.5), m_doGlobalFit(true), m_doMdt(true), m_doRpc(false), m_doTileCal(true),
                m_isData(true), m_mdtSmearFactor(0.9), //default mid smear
                m_rpcSmearFactor(-1), m_tileSmearFactor(-1), m_doCalibration(false),
                m_pCollection(NULL), m_pMdtSegmentMaker("Muon::DCMathSegmentMaker"),
                m_pMdtDriftCircleCreator("Muon::MdtDriftCircleOnTrackCreator"), m_pTofTool("MuGirlNS::StauBetaTofTool"),
                m_pGlobalFitTool("MuGirlNS::GlobalFitTool", 0),    // make this a public tool
                m_caloCellAssociationTool("Rec::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool"),
                m_pMdtCalibDbSvc("MdtCalibrationDbSvc", n),
                m_randSvc ("AtRanluxGenSvc", n),
                m_pMuonMgr(NULL), m_lowerBetaLimit(0.2),
                m_upperBetaLimit(1.5), m_betaTolerance(0.001), m_gfBetaTolerance(0.001), m_minBeta(0.1), m_maxBeta(1.5),
                m_lowerTTrackLimit(-50), m_upperTTrackLimit(50), m_doMuGirlNtuple(false), m_doStauNtuple(false),
                m_pStauRPC(NULL), m_pStauMDT(NULL), m_pStauGF(NULL), m_pStauTileCal(NULL), m_pStauMDTT(NULL),
                m_pSegmentManager(NULL), m_skipStation(1000), m_beta(-1.), m_mass(-100.), m_tzero(-100.), m_p(0.),
                m_pNewMdtSegments(NULL), m_pRefittedTrack(NULL), m_pMuonRefittedTrack(NULL), m_hasSummary(false),
                m_pFcnSteps(NULL), m_numMdtSegs(-1), m_numMdtHitsInSeg(-1), m_calibration(msg())
{
    declareInterface < IStauTool > (this);

    //  template for property declaration
    //  Tools
    declareProperty("StauMdtSegmentMaker", m_pMdtSegmentMaker);
    declareProperty("StauMdtDriftCircleCreator", m_pMdtDriftCircleCreator);
    declareProperty("StauBetaTofTool", m_pTofTool);
    declareProperty("StauGlobalFitTool", m_pGlobalFitTool);
    declareProperty("ParticleCaloCellAssociationTool", m_caloCellAssociationTool);
    declareProperty("MdtCalibrationDbSvc", m_pMdtCalibDbSvc);
    declareProperty("RandSvc", m_randSvc);

    //  other properties
    declareProperty("MinBeta", m_lowerBetaLimit);
    declareProperty("MaxBeta", m_upperBetaLimit);
    declareProperty("betaTolerance", m_betaTolerance);
    declareProperty("gfBetaTolerance", m_gfBetaTolerance);
    declareProperty("doStauNtuple", m_doStauNtuple);
    declareProperty("doMdt", m_doMdt);
    declareProperty("doRpc", m_doRpc);
    declareProperty("doTileCal", m_doTileCal);
    declareProperty("doCalibration", m_doCalibration);
    declareProperty("isData", m_isData);
    declareProperty("mdtSmearFactor", m_mdtSmearFactor);
    declareProperty("rpcSmearFactor", m_rpcSmearFactor);
    declareProperty("tileSmearFactor", m_tileSmearFactor);
    declareProperty("rpcTimeShift", m_rpcTimeShift);
    declareProperty("tileEnergyCut", m_tileEnergyCut);
    declareProperty("RandStreamName", m_randStreamName = "StauTool");

    /* calibration */
    declareProperty("mdtCalibFileName", m_mdtCalibFileName);
    declareProperty("rpcCalibFileName", m_rpcCalibFileName);
    declareProperty("caloCalibFileName", m_caloCalibFileName);

}

//================ Destructor =================================================

StauTool::~StauTool()
{
}

//================ Initialization =================================================

StatusCode StauTool::initialize()
{
    ATH_CHECK( AthAlgTool::initialize() );

    ////calibration files
    if ( m_mdtCalibFileName == "" ) ATH_MSG_INFO( "No mdt calib file" );
    if ( m_rpcCalibFileName == "" ) ATH_MSG_INFO( "No rpc calib file" );
    if ( m_caloCalibFileName == "" ) ATH_MSG_INFO( "No calo calib file" );

    /** initialize tool handles */
    ATH_CHECK( m_pMdtSegmentMaker.retrieve() );
    ATH_CHECK( m_pMdtDriftCircleCreator.retrieve() );
    ATH_CHECK( m_pTofTool.retrieve() );
    ATH_CHECK( m_pGlobalFitTool.retrieve() );
//    if (!m_caloCellAssociationTool.empty()) ATH_CHECK(m_caloCellAssociationTool.retrieve());
    ATH_CHECK( m_pMdtCalibDbSvc.retrieve() );
    ATH_CHECK( m_randSvc.retrieve() );
    ATH_CHECK( m_caloCellAssociationTool.retrieve() );
    /** initialize MuonDetectorManager */
    ATH_CHECK( detStore()->retrieve(m_pMuonMgr) );
    // **** **** **** TagInfo **** **** ****
    const DataHandle<TagInfo> tagInfoH;
    std::string tagInfoKey = "";
    ServiceHandle<ITagInfoMgr> pTagInfoMgr ("TagInfoMgr", name());
    ATH_CHECK( pTagInfoMgr.retrieve() );
    tagInfoKey = pTagInfoMgr->tagInfoKey();
    //register the call-back to matchTagInfo
    ATH_CHECK( detStore()->regFcn(&StauTool::readTagInfo, this, tagInfoH, tagInfoKey) );
    ATH_MSG_DEBUG( "registered matchTagInfo callback for key: " << tagInfoKey );
    ATH_MSG_DEBUG( "initialize() successful in " << name() );
    ATH_MSG_DEBUG( "addMuToF flag is " << m_addMuToF );
    ATH_MSG_DEBUG( "rpcBugFix flag is " << m_rpcBugFix );
    /** calibration */
    if (m_doCalibration)
    {
      ATH_MSG_DEBUG( "Calibration "<<m_mdtCalibFileName<<"  "<<m_rpcCalibFileName<<"  "<<m_caloCalibFileName );
      m_calibration.initialize(m_mdtCalibFileName, m_rpcCalibFileName, m_caloCalibFileName);

    }
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode StauTool::finalize()
{
    clearCandidate();
    return AlgTool::finalize();
}

//============================================================================================

//=============== processStau ===================================================
/*
 @task: call the minimization program
 */
StatusCode StauTool::processStau(const xAOD::TrackParticle* trkParticle,
        const MuonSegmentList& muonSegments,
        const MdtSegmentMakerInfoList& mdtSegmentMakerInfoList,
        const RIO_OnTrackLists& rpcHitsInSegments,
        const RIO_OnTrackLists& tgcHitsInSegments,
        const Trk::Track* pMuonRefittedTrack,
        const xAOD::TrackParticle* pMuonTrackParticle,
        Amg::Vector3D trigMomentum,
        const Trk::Perigee* trk0,
        bool isMuonBetaRefit)
{
  //if(NULL!=trkParticle->originalTrack()) ATH_MSG_VERBOSE(  "processStau()" );
    if (trkParticle != NULL)
      ATH_MSG_DEBUG( "entered" );
    else
      ATH_MSG_DEBUG( "no ID track" );
    //return processStauCosmic(trkParticle, muonSegments , mdtSegmentMakerInfoList,
    //rpcHitsInSegments, tgcHitsInSegments, trigMomentum );
    //clear the candidate
    clearCandidate();
    //initialize the candidate
    if (mdtSegmentMakerInfoList.empty() && rpcHitsInSegments.empty() && tgcHitsInSegments.empty())
    {
        ATH_MSG_DEBUG( "candidate with no MS info - done" );
        return StatusCode::SUCCESS;
    }
    //if (isMuonBetaRefit) m_doGlobalFit = false;
    initializeCandidate(trkParticle, muonSegments, mdtSegmentMakerInfoList, rpcHitsInSegments, tgcHitsInSegments,
            pMuonRefittedTrack, pMuonTrackParticle, trigMomentum);
    //call the minimization functions
    StauFcn stauFcn(this, msg());
    m_minBeta = m_lowerBetaLimit;
    m_maxBeta = m_upperBetaLimit;
    if (m_doRpc || m_doMdt)
    {
        if (!stauFcn.selectRange(m_minBeta, m_maxBeta))
        {
            m_beta = StauBetaDefault;
            ATH_MSG_DEBUG( "beta out of range" );
            if (!isMuonBetaRefit) return StatusCode::SUCCESS;
        }

    }
    m_beta = StauBetaDefault;
    Minimizer minim;
    if (m_doMdt || (m_doRpc && m_pStauRPC->hasHits()) || (m_doTileCal && m_pStauTileCal->hasCells()))
    {
        double mina = minim.minimize(stauFcn, ALL_TECHS, m_minBeta, m_maxBeta, m_betaTolerance).first;
        if (fabs(m_minBeta - mina) < 0.0005 || fabs(m_maxBeta - mina) < 0.0005)
        {
            ATH_MSG_DEBUG( "min is on the edge" );
            if (m_doRpc && m_pStauRPC->hasHits())
                m_beta = m_pStauRPC->avgBeta();
            else
                m_beta = StauBetaOnEdge;
        }
        else
        {
            m_beta = mina;
            FcnStepsData fcnSteps = *m_pFcnSteps;
            FcnStepData* pLastStep = fcnSteps.back();
            m_numMdtSegs = (pLastStep->mdtData != NULL ? (int) pLastStep->mdtData->numSegs : 0);
            m_numMdtHitsInSeg = (pLastStep->mdtData != NULL ? (int) pLastStep->mdtData->hitsInSegments : 0);

        }
    }    //else clearFcnSteps();
         //recalculate and store the mdt segments with the new beta
    if (m_beta > m_lowerBetaLimit)
    {
        m_pStauMDT->findNewSegments(m_beta);
        m_numMdtSegs = 0;
        m_numMdtHitsInSeg = 0;
        for (auto pSeg : *m_pNewMdtSegments)
        {
            m_numMdtSegs++;
            m_numMdtHitsInSeg += pSeg->numberOfContainedROTs();
        }
        //refit the track using global fit
        m_pStauGF->setInitialTrackParameters(trk0);
        if (m_doGlobalFit) m_pRefittedTrack = m_pStauGF->refitTrack();
        m_p = stauMomentum();
        m_mass = stauMass();
    }
    updateTechBetaChi2(ALL_TECHS);
    //refit the track using global fit - if there is no beta there is no need to calculate the mass and momentum
    //if(m_doGlobalFit) m_pRefittedTrack = m_pStauGF->refitTrack();
    //m_p = stauMomentum();
    //m_mass = stauMass();

    //ALLT
    if (m_pRefittedTrack != NULL && m_pStauMDTT->initialize(m_pRefittedTrack))
        computeTechnologyBeta(ALLT_TECHS, m_lowerBetaLimit, m_upperBetaLimit);
    updateTechContribution2Chi2();
    fillStauContainer();
    printTechBetaChi2();
    printTechContribution2Chi2();
    printFcnSteps();
    ATH_MSG_DEBUG( "done" );
    return StatusCode::SUCCESS;
}

void StauTool::updateTechBetaChi2(StauTechnology eTech)
{
    auto pLastStep = m_pFcnSteps->back();
    m_techBetaChi2[eTech].beta = pLastStep->beta;
    m_techBetaChi2[eTech].chi2 = pLastStep->chi2;
    m_techBetaChi2[eTech].dof = pLastStep->dof;
    if (pLastStep->mdtData != NULL)
    {
        m_techBetaChi2[eTech].techContribution[MDT_TECH].chi2 = pLastStep->mdtData->chi2;
        m_techBetaChi2[eTech].techContribution[MDT_TECH].dof = pLastStep->mdtData->dof;
    }
    if (pLastStep->mdttData != NULL)
    {
        m_techBetaChi2[eTech].techContribution[MDTT_TECH].chi2 = pLastStep->mdttData->chi2;
        m_techBetaChi2[eTech].techContribution[MDTT_TECH].dof = pLastStep->mdttData->dof;
    }
    if (pLastStep->rpcData != NULL)
    {
        m_techBetaChi2[eTech].techContribution[RPC_TECH].chi2 = pLastStep->rpcData->chi2;
        m_techBetaChi2[eTech].techContribution[RPC_TECH].dof = pLastStep->rpcData->dof;
    }
    if (pLastStep->tileCalData != NULL)
    {
        m_techBetaChi2[eTech].techContribution[TILECAL_TECH].chi2 = pLastStep->tileCalData->chi2;
        m_techBetaChi2[eTech].techContribution[TILECAL_TECH].dof = pLastStep->tileCalData->dof;
    }
}

void StauTool::updateTechContribution2Chi2()
{
    auto pLastStep = m_pFcnSteps->back();
    m_techContribution2Chi2[MDTT_TECH] = BetaChi2();
    m_techContribution2Chi2[RPC_TECH] = BetaChi2();
    m_techContribution2Chi2[TILECAL_TECH] = BetaChi2();
    if (pLastStep->mdttData != NULL)
    {
        m_techContribution2Chi2[MDTT_TECH].beta = pLastStep->beta;
        m_techContribution2Chi2[MDTT_TECH].chi2 = pLastStep->mdttData->chi2;
        m_techContribution2Chi2[MDTT_TECH].dof = pLastStep->mdttData->dof;
    }
    if (pLastStep->rpcData != NULL)
    {
        m_techContribution2Chi2[RPC_TECH].beta = pLastStep->beta;
        m_techContribution2Chi2[RPC_TECH].chi2 = pLastStep->rpcData->chi2;
        m_techContribution2Chi2[RPC_TECH].dof = pLastStep->rpcData->dof;
    }
    if (pLastStep->tileCalData != NULL)
    {
        m_techContribution2Chi2[TILECAL_TECH].beta = pLastStep->beta;
        m_techContribution2Chi2[TILECAL_TECH].chi2 = pLastStep->tileCalData->chi2;
        m_techContribution2Chi2[TILECAL_TECH].dof = pLastStep->tileCalData->dof;
    }
}

void StauTool::printTechContribution2Chi2()
{
    for (auto& techBetaChi2 : m_techContribution2Chi2)
    {
        StauTechnology eTech = techBetaChi2.first;
        BetaChi2 betaChi2 = techBetaChi2.second;
        ATH_MSG_VERBOSE( "contribution to chi2:: tech " << technologyName(eTech)
                         << " beta=" << betaChi2.beta
                         << " chi2=" << betaChi2.chi2
                         << " dof=" << betaChi2.dof );
    }
}

void StauTool::printTechBetaChi2()
{
    for (auto& techBetaChi2 : m_techBetaChi2)
    {
        StauTechnology eTech = techBetaChi2.first;
        BetaChi2 betaChi2 = techBetaChi2.second;
        ATH_MSG_VERBOSE( "tech " << technologyName(eTech)
                         << " beta=" << betaChi2.beta
                         << " chi2=" << betaChi2.chi2
                         << " dof=" << betaChi2.dof );
    }
}

void StauTool::setCandidateAnn(double ann)
{
    if (m_pCollection != NULL)
    {
        unsigned int nCol = m_pCollection->size();
        if (!m_pCollection || nCol == 0)
        {
          ATH_MSG_ERROR(  "setCandidateAnn() is called when there is no stau collection" );
            return;
        }
        //set the ann value to the last entry in the collection
        (*m_pCollection)[nCol - 1]->setAnn(ann);
    }
    else
        return;
}

void StauTool::fillStauContainer()
{
    //return;
    ATH_MSG_DEBUG( "entered" );
    if (m_pCollection == NULL)
      //ATH_MSG_ERROR(  "StauTool::fillStauContainer - m_pCollection == NULL !!!" );
      recordCollection();
    //add a container for this candidate
    auto cont = new StauContainer();
    //m_pCollection->push_back(cont);
    if (m_pStauRPC)
        cont->setHasRpcHits(m_pStauRPC->hasHits());
    else
        cont->setHasRpcHits(false);
    if (m_pIdTrack)
        cont->setReconstructionParameters(StauContainer::RP_ID);
    else
        cont->setReconstructionParameters(StauContainer::RP_MF);
    if (m_pStauTileCal)
    {
        cont->setHasTileCells(m_pStauTileCal->hasCells());
        cont->setTileCells(m_pStauTileCal->caloCells());
    }
    else if (m_pRefittedTrack != NULL)
    {
        m_pStauTileCal = new StauTileCal(this, msg(),
                                         *m_randSvc->GetEngine(m_randStreamName),
                                         m_pRefittedTrack);
        cont->setHasTileCells(m_pStauTileCal->hasCells());
        cont->setTileCells(m_pStauTileCal->caloCells());
    }
    else if (m_pMuonRefittedTrack != NULL)
    {
        m_pStauTileCal = new StauTileCal(this, msg(),
                                         *m_randSvc->GetEngine(m_randStreamName),
                                         m_pMuonRefittedTrack);
        cont->setHasTileCells(m_pStauTileCal->hasCells());
        cont->setTileCells(m_pStauTileCal->caloCells());
    }
    else
      cont->setHasTileCells(false);
    cont->setBeta(m_beta);
    cont->setIdTrk(m_pIdTrack);
    cont->setMuonTrk(m_pMuonRefittedTrack);
    cont->setStauTrk(m_pRefittedTrack);
    if (m_pStauRPC)
        cont->setRpcHitsByStation(m_pStauRPC->rpcHitsByStations());
    else
        cont->setRpcHitsByStation(NULL);
    cont->setMfP(m_trigP);
    if (m_pMuonRefittedTrack != NULL)
      {
        double px = (double) m_pMuonRefittedTrack->perigeeParameters()->momentum()[Trk::px];
        double py = (double) m_pMuonRefittedTrack->perigeeParameters()->momentum()[Trk::py];
        double pz = (double) m_pMuonRefittedTrack->perigeeParameters()->momentum()[Trk::pz];
        Amg::Vector3D muonP(px, py, pz);
        cont->setMuonP(muonP);
    }
    if (m_pRefittedTrack != NULL)
    {
        double px = (double) m_pRefittedTrack->perigeeParameters()->momentum()[Trk::px];
        double py = (double) m_pRefittedTrack->perigeeParameters()->momentum()[Trk::py];
        double pz = (double) m_pRefittedTrack->perigeeParameters()->momentum()[Trk::pz];
        Amg::Vector3D stauP(px, py, pz);
        cont->setStauP(stauP);
    }
    //MDTT
    if (m_pStauMDTT != NULL && m_pStauMDTT->hasHits())
    {
        //if (!(m_trigP != Amg::Vector3D(0., 0., 0.)))
        cont->addHits(m_pStauMDTT->getHits());
        m_techBetaAvg[MDTT_TECH].beta = m_pStauMDTT->avgBeta();
        m_techBetaAvg[MDTT_TECH].rms = m_pStauMDTT->rmsBeta();
    }
    //RPC only
    if (m_pStauRPC != NULL && m_pStauRPC->hasHits())
    {
        cont->addHits(m_pStauRPC->getHits());
        m_techBetaAvg[RPC_TECH].beta = m_pStauRPC->avgBeta();
        m_techBetaAvg[RPC_TECH].rms = m_pStauRPC->rmsBeta();
    }
    //TileCal only
    if (m_pStauTileCal != NULL && m_pStauTileCal->hasCells())
    {    //if there is no ID track, there is no StauTileCal
        cont->addHits(m_pStauTileCal->getHits());
        m_techBetaAvg[TILECAL_TECH].beta = m_pStauTileCal->avgBeta();
        m_techBetaAvg[TILECAL_TECH].rms = m_pStauTileCal->rmsBeta();
        if (m_trigP != Amg::Vector3D(0., 0., 0.))
          ATH_MSG_DEBUG( "StauTile: hits " << (m_pStauTileCal->getHits()).size() );
    }
    auto consistency = new StauContainer::Consistency();
    cont->consistencyList().push_back(consistency);
    if (m_doRpc)
        consistency->params = StauContainer::CP_WRPC;
    else
        consistency->params = StauContainer::CP_NORPC;
    consistency->betaAll = m_techBetaChi2[ALL_TECHS].beta;
    consistency->betaAllt = m_techBetaChi2[ALLT_TECHS].beta;
    cont->setTechBetaAvg(m_techBetaAvg);
    cont->setTechBetaChi2(m_techBetaChi2);
    cont->setTechContribution2Chi2(m_techContribution2Chi2);
    ATH_MSG_DEBUG( cont->toString() );
    delete cont;
    ATH_MSG_DEBUG( "done" );
}

double StauTool::computeTechnologyBeta(StauTechnology eTech, double minBeta, double maxBeta)
{
    Minimizer minim;
    StauFcn stauFcn(this, msg());
    m_techBetaChi2[eTech] = BetaChi2();
    double beta = StauBetaOnEdge;
    double tmpBeta = minim.minimize(stauFcn, eTech, minBeta, maxBeta, m_betaTolerance).first;
    if (fabs(minBeta - tmpBeta) > 0.0005 && fabs(maxBeta - tmpBeta) > 0.0005)
    { // not on the edge
        beta = tmpBeta;
        updateTechBetaChi2(eTech);
    }
    else
        m_techBetaChi2[eTech].beta = StauBetaOnEdge;
    this->tofTool()->setTShift(0);
    return beta;
}

double StauTool::stauMomentum()
{
    if (m_pRefittedTrack != NULL)
        return m_pStauGF->stauMomentum();
    else
        return m_idP;
}

double StauTool::stauMass()
{
    if (m_p == 0.) m_p = stauMomentum();
    if (m_beta <= m_lowerBetaLimit || m_beta >= 1.)
        return 105.7 * CLHEP::MeV; // not a stau, probably muon
    else
        return m_p * sqrt(1 - m_beta * m_beta) / m_beta;
}

void StauTool::setRefittedTrack(const Trk::Track* pTrack)
{
    //if(NULL!=m_pRefittedTrack) delete m_pRefittedTrack;
    m_pRefittedTrack = pTrack;
}

void StauTool::recordCollection()
{
    ATH_MSG_DEBUG( "entered" );
    m_pCollection = new StauCollection();
//    delete m_pCollection;
    ServiceHandle<StoreGateSvc> pEvtStore("StoreGateSvc", name());
    StatusCode sc = pEvtStore->record(m_pCollection, "StauCandidateCollection", false);
    if (sc.isFailure()) ATH_MSG_VERBOSE( "failed recording StauCandidateCollection" );
}

//============================== NTuple functions =======================================
StatusCode StauTool::bookNTuple(NTuple::Tuple* pNTuple)
{
    m_doMuGirlNtuple = true; //this funtion is called form MuGirl.cxx if and only if muGirl ntuple is required
    if (!m_doStauNtuple) return StatusCode::SUCCESS;
    if (!m_pGlobalFitTool->bookNTuple(pNTuple, "STAU")) return StatusCode::RECOVERABLE;
    m_pStauNtuple = new StauNTuple(msg());
    return m_pStauNtuple->book(pNTuple);
}

StatusCode StauTool::fillNTuple()
{
    if (!m_doStauNtuple) return StatusCode::SUCCESS;
    return m_pStauNtuple->fillStau(this);
}

//================= initializeCandidate ==========================================
/*
 @task: initiate stau candiate (using information from the candidate tool)
 */
void StauTool::initializeCandidate(const xAOD::TrackParticle* trkParticle,
        const MuonSegmentList& muonSegments,
        const MdtSegmentMakerInfoList& mdtSegmentMakerInfoList,
        const RIO_OnTrackLists& rpcHitsInSegments,
        const RIO_OnTrackLists& tgcHitsInSegments,
        const Trk::Track* pMuonRefittedTrack,
        const xAOD::TrackParticle* pMuonTrackParticle,
        Amg::Vector3D trigMomentum)
{
    ATH_MSG_DEBUG( "entered" );
    //The data from MuGirl candidate
    m_pMdtSegmentMakerInfoList = &mdtSegmentMakerInfoList;
    m_pRpcHitsInSegments = &rpcHitsInSegments;
    m_pTgcHitsInSegments = &tgcHitsInSegments;
    m_pTrkParticle = trkParticle;
    if (trkParticle != NULL)
        m_pIdTrack = trkParticle->track();
    else
        m_pIdTrack = NULL;
    m_trigP = trigMomentum;
    m_pMuonRefittedTrack = pMuonRefittedTrack;
    //initiate the 'technologies'
    m_pStauRPC = new StauRPC(this, msg(),
                             *m_randSvc->GetEngine(m_randStreamName),
                             rpcHitsInSegments);
    m_pStauMDT = new StauMDT(this, msg(),
                             *m_randSvc->GetEngine(m_randStreamName),
                             mdtSegmentMakerInfoList);
    if (m_doGlobalFit)
      m_pStauGF = new StauGF(this, msg(), muonSegments);
    else
        m_pStauGF = NULL;
    if (m_pTrkParticle != NULL)
      m_pStauTileCal = new StauTileCal(this, msg(),
                                       *m_randSvc->GetEngine(m_randStreamName),
                                       m_pTrkParticle);
    else if (pMuonTrackParticle != NULL)
      m_pStauTileCal = new StauTileCal(this, msg(),
                                       *m_randSvc->GetEngine(m_randStreamName),
                                       pMuonTrackParticle);
    else
        m_pStauTileCal = NULL;
    m_pStauMDTT = new StauMDTT(this, msg(),
                               *m_randSvc->GetEngine(m_randStreamName));
    if (m_pIdTrack != NULL)
    {
        double px = m_pIdTrack->perigeeParameters()->momentum()[Trk::px];
        double py = m_pIdTrack->perigeeParameters()->momentum()[Trk::py];
        double pz = m_pIdTrack->perigeeParameters()->momentum()[Trk::pz];
        m_idP = sqrt(px * px + py * py + pz * pz);
    }
    else
        m_idP = m_trigP.mag();
    m_skipStation = 1000;
    //Initiate the output parameters
    m_beta = StauBetaDefault;
    m_mass = -100.;
    m_tzero = -100.;
    m_numMdtSegs = -1;
    m_numMdtHitsInSeg = -1;
    //allocate memory for the step data;
    m_pFcnSteps = new FcnStepsData();
    //allocate memory to the new mdt segments
    m_pNewMdtSegments = new MdtSegments;
    //mark that thiws candidate does not have (yet) a summary
    m_hasSummary = false;
    ATH_MSG_DEBUG( "done" );
}

//================= clearCandidate ==========================================
/*
 @task: delete stau candidate and prepare for the next candidate
 */
void StauTool::clearCandidate()

{
    ATH_MSG_VERBOSE( "entered" );
    m_beta = StauBetaDefault;
    m_mass = -100.;
    m_tzero = -100.;
    m_techContribution2Chi2.clear();
    m_techBetaChi2.clear();
    m_techBetaAvg.clear();
    m_pRefittedTrack = NULL;
    //clear the vector of new mdt segments.
    // the segments are handles by theSegmentManager
    ATH_MSG_VERBOSE( "clearing new segments" );
    if (m_pNewMdtSegments != NULL)
    {
        m_pNewMdtSegments->clear();
        delete m_pNewMdtSegments;
    }
    ATH_MSG_VERBOSE( "new segments cleared" );
    if (m_pFcnSteps != NULL) clearFcnSteps();
    {
        delete m_pFcnSteps;
        m_pFcnSteps = NULL;
    }
    if (m_pStauRPC != NULL)
    {
        m_pStauRPC->clear();
        delete m_pStauRPC;
    }
    ATH_MSG_VERBOSE( "clearing MDT" );
    if (m_pStauMDT != NULL)
    {
        m_pStauMDT->clear();
        delete m_pStauMDT;
    }
    ATH_MSG_VERBOSE( "MDT cleared" );
    if (m_pStauGF != NULL)
    {
        m_pStauGF->clear();
        delete m_pStauGF;
    }
    ATH_MSG_VERBOSE( "GF cleared" );
    if (m_pStauTileCal != NULL)
    {
        m_pStauTileCal->clear();
        delete m_pStauTileCal;
    }
    ATH_MSG_VERBOSE( "clearing MDTT" );
    if (m_pStauMDTT != NULL)
    {
        m_pStauMDTT->clear();
        delete m_pStauMDTT;
    }
    ATH_MSG_VERBOSE( "done" );
}

void StauTool::clearNewMdtSegments()
{
    if (m_pNewMdtSegments == NULL) return;
    for (auto pSeg : *m_pNewMdtSegments)
        delete pSeg;
    m_pNewMdtSegments->clear();
}

void StauTool::clearFcnSteps()
{
    for (auto pStepData : *m_pFcnSteps)
    {
        if (pStepData->tgcData != NULL) delete pStepData->tgcData;
        m_pStauMDT->clearStepData(pStepData->mdtData);
        m_pStauRPC->clearStepData(pStepData->rpcData);
        m_pStauTileCal->clearStepData(pStepData->tileCalData);
        m_pStauMDTT->clearStepData(pStepData->mdttData.get());
        delete pStepData;
    }
    m_pFcnSteps->clear();
    delete m_pFcnSteps;
    m_pFcnSteps = NULL;
}

void StauTool::printFcnSteps()
{
    if (m_pFcnSteps == NULL) return;
    for (auto fcnData : *m_pFcnSteps)
    {
        if (m_pStauMDT != NULL) m_pStauMDT->printStepData(fcnData->mdtData);
        if (m_pStauRPC != NULL) m_pStauRPC->printStepData(fcnData->rpcData);
        if (m_pStauTileCal != NULL) m_pStauTileCal->printStepData(fcnData->tileCalData);
        if (m_pStauMDTT != NULL) m_pStauMDTT->printStepData(fcnData->mdttData.get());
    }
}

//=============================== Summary functions ==========================
/*
 @task: prepare stau summary, to be used later in the particle creator tool
 */

StatusCode StauTool::fillStauSummary(const CandidateSummary* summary, CandidateSummary* stauSummary)
{
    ATH_MSG_DEBUG( "entered" );
    //Mark this candidate with hasSummary flag
    m_hasSummary = true;
    //paarmeters identical to the muon summary
    stauSummary->pTrackParticle = summary->pTrackParticle;
    stauSummary->pMuonFeature = summary->pMuonFeature;
    stauSummary->startFromMF = summary->startFromMF;
    stauSummary->pMuonEFTrack = summary->pMuonEFTrack;
    stauSummary->startFromEF = summary->startFromEF;
    stauSummary->pLowBetaTrack = summary->pLowBetaTrack;
    stauSummary->pTrkLowBeta = summary->pTrkLowBeta;
    stauSummary->pLowBetaExtrpTrack = summary->pLowBetaExtrpTrack;
    //    stauSummary->pTrkLowBetaExtr = summary->pTrkLowBetaExtr;
    stauSummary->startFromBetaRefit = summary->startFromBetaRefit;
    stauSummary->numRpcPhiHits = summary->numRpcPhiHits;
    stauSummary->numRpcEtaHits = summary->numRpcEtaHits;
    stauSummary->numCscPhiHits = summary->numCscPhiHits;
    stauSummary->numCscEtaHits = summary->numCscEtaHits;
    stauSummary->numRpcSegs = summary->numRpcSegs;
    stauSummary->numRpcHitsInSeg = summary->numRpcHitsInSeg;
    stauSummary->numTgcRpcPhiHits = summary->numTgcRpcPhiHits;
    stauSummary->numTgcRpcEtaHits = summary->numTgcRpcEtaHits;
    stauSummary->numTgcRpcSegs = summary->numTgcRpcSegs;
    stauSummary->numTgcRpcHitsInSeg = summary->numTgcRpcHitsInSeg;
    stauSummary->numTgcPhiHits = summary->numTgcPhiHits;
    stauSummary->numTgcEtaHits = summary->numTgcEtaHits;
    stauSummary->numTgcSegs = summary->numTgcSegs;
    stauSummary->numTgcHitsInSeg = summary->numTgcHitsInSeg;
    stauSummary->numCscSegs = summary->numCscSegs;
    stauSummary->innAngle = summary->innAngle;
    stauSummary->innDelta = summary->innDelta;
    stauSummary->midAngle = summary->midAngle;
    stauSummary->midDelta = summary->midDelta;
    stauSummary->lhr = summary->lhr;
    stauSummary->numMdtHits = summary->numMdtHits;
    stauSummary->nnBarrel = summary->nnBarrel;
    stauSummary->nnEndCap = summary->nnEndCap;
    stauSummary->PassNNsel = summary->PassNNsel;
    stauSummary->saveAnn = summary->saveAnn;
    stauSummary->saveStau = summary->saveStau;
    stauSummary->rpcHitsLists = (m_pStauRPC != NULL ? m_pStauRPC->copyStauRpcHitsLists() : NULL);
    //stau related parameters
    for (auto pSeg : summary->muonSegmentList)
    {
        if (m_pSegmentManager->isNotMDT(pSeg)) (stauSummary->muonSegmentList).push_back(pSeg);
    }
    for (auto pSeg : *m_pNewMdtSegments)
    {
        (stauSummary->muonSegmentList).push_back(pSeg);
        m_pSegmentManager->addSegment(pSeg, SegmentManager::MDT_STAU);
    }
    // FcnStepsData fcnSteps = *m_pFcnSteps;
    // FcnStepData* pLastStep = fcnSteps.back();
    stauSummary->numMdtSegs = m_numMdtSegs; //(int)pLastStep->mdtData->numSegs;
    stauSummary->numMdtHitsInSeg = m_numMdtHitsInSeg; //(int)pLastStep->mdtData->hitsInSegments;
    stauSummary->beta = m_beta;
    stauSummary->pRefittedTrack = NULL;
    stauSummary->pTrkRefitted = NULL;
    stauSummary->pMSRefittedTrack.reset();
    stauSummary->pTrkMSRefitted.reset();
    
    ATH_MSG_DEBUG( "done" );
    return StatusCode::SUCCESS;
}

static std::string dumpStauExtras(const StauExtras& stauExtras)
{
    std::ostringstream ostr;
    ostr << "  ann:          " << stauExtras.ann << "\n";
    ostr << "  betaAll:      " << stauExtras.betaAll << "\n";
    ostr << "  betaAllt:     " << stauExtras.betaAllt << "\n";
    ostr << "  numCaloCells: " << stauExtras.numCaloCells << "\n";
    ostr << "  RPC:          BetaAvg: " << stauExtras.rpcBetaAvg
            << ", BetaRms: " << stauExtras.rpcBetaRms
            << ", BetaChi2: " << stauExtras.rpcBetaChi2
            << ", BetaDof: " << stauExtras.rpcBetaDof << "\n";
    ostr << "  MDTT:         BetaAvg: " << stauExtras.mdtBetaAvg
            << ", BetaRms: " << stauExtras.mdtBetaRms
            << ", BetaChi2: " << stauExtras.mdtBetaChi2
            << ", BetaDof: " << stauExtras.mdtBetaDof << "\n";
    ostr << "  CALO:         BetaAvg: " << stauExtras.caloBetaAvg
            << ", BetaRms: " << stauExtras.caloBetaRms
            << ", BetaChi2: " << stauExtras.caloBetaChi2
            << ", BetaDof: " << stauExtras.caloBetaDof << "\n";
    return ostr.str();
}

StatusCode StauTool::fillStauExtras(const CandidateSummary* stauSummary, StauExtras* stauExtras)
{
    ATH_MSG_DEBUG( "entered" );
    stauExtras->ann = stauSummary->nnBarrel > 0 ? stauSummary->nnBarrel : stauSummary->nnEndCap;
    stauExtras->betaAll = m_techBetaChi2[ALL_TECHS].beta;
    stauExtras->betaAllt = m_techBetaChi2[ALLT_TECHS].beta;
    stauExtras->numRpcHitsInSeg = stauSummary->numRpcHitsInSeg;
    if (m_pStauTileCal != NULL)
        stauExtras->numCaloCells = m_pStauTileCal->caloCells()->size();
    else
        stauExtras->numCaloCells = 0;
    stauExtras->rpcBetaAvg = m_techBetaAvg[RPC_TECH].beta;
    stauExtras->rpcBetaRms = m_techBetaAvg[RPC_TECH].rms;
    stauExtras->rpcBetaChi2 = m_techContribution2Chi2[RPC_TECH].chi2;
    stauExtras->rpcBetaDof = m_techContribution2Chi2[RPC_TECH].dof;
    stauExtras->mdtBetaAvg = m_techBetaAvg[MDTT_TECH].beta;
    stauExtras->mdtBetaRms = m_techBetaAvg[MDTT_TECH].rms;
    stauExtras->mdtBetaChi2 = m_techContribution2Chi2[MDTT_TECH].chi2;
    stauExtras->mdtBetaDof = m_techContribution2Chi2[MDTT_TECH].dof;
    stauExtras->caloBetaAvg = m_techBetaAvg[TILECAL_TECH].beta;
    stauExtras->caloBetaRms = m_techBetaAvg[TILECAL_TECH].rms;
    stauExtras->caloBetaChi2 = m_techContribution2Chi2[TILECAL_TECH].chi2;
    stauExtras->caloBetaDof = m_techContribution2Chi2[TILECAL_TECH].dof;
    if (m_pStauMDTT != NULL && m_pStauMDTT->hasHits())
        stauExtras->addHits(m_pStauMDTT->getHits());
    if (m_pStauRPC != NULL && m_pStauRPC->hasHits())
        stauExtras->addHits(m_pStauRPC->getHits());
    if (m_pStauTileCal != NULL && m_pStauTileCal->hasCells())
        stauExtras->addHits(m_pStauTileCal->getHits());
    
    ATH_MSG_DEBUG( "stauExtras:\n" << dumpStauExtras(*stauExtras) );
    ATH_MSG_DEBUG( "done" );
    return StatusCode::SUCCESS;
}
static std::string dumpRHExtras(const RHExtras& rhExtras)
{
    std::ostringstream ostr;
    ostr << "  numCaloCells: " << rhExtras.numCaloCells << "\n"
         << "  CALO:         BetaAvg: " << rhExtras.caloBetaAvg
         << ", BetaRms: " << rhExtras.caloBetaRms << "\n";
    return ostr.str();
}

StatusCode StauTool::fillRHExtras(RHExtras* rhExtras)
{
    ATH_MSG_DEBUG( "entered" );
    if (m_pStauTileCal == nullptr)
    {
        if (m_pIdTrack == nullptr)
            return StatusCode::FAILURE;
        else
          m_pStauTileCal = new StauTileCal(this, msg(),
                                           *m_randSvc->GetEngine(m_randStreamName),
                                           m_pIdTrack);
    }
    rhExtras->numCaloCells = m_pStauTileCal->caloCells()->size();
    rhExtras->caloBetaAvg = m_pStauTileCal->avgBeta();
    rhExtras->caloBetaRms = m_pStauTileCal->rmsBeta();
    if (m_pStauTileCal->hasCells())
        rhExtras->addHits(m_pStauTileCal->getHits());
    ATH_MSG_DEBUG( "rhExtras:\n" << dumpRHExtras(*rhExtras) );
    ATH_MSG_DEBUG( "done" );
    return StatusCode::SUCCESS;
}
//=============== functions related to the minimization process================

/*
 Task:
 return code: True = recalculate first steps
 */
bool StauTool::recalculateFirstSteps()
{
    ATH_MSG_VERBOSE( "entered" );
    if (m_pFcnSteps->size() != 4)
    {
        ATH_MSG_ERROR(  "recalculateFirstSteps()::size of m_pFsnSteps != 4  --> BUG" );
        return false;
    }
    auto& firstSteps = *m_pFcnSteps;
    auto pStep0 = firstSteps[0];
    auto pStep1 = firstSteps[1];
    auto pStep2 = firstSteps[2];
    auto pStep3 = firstSteps[3];
    if (pStep0->mdtData->pStationDataList->size() <= 1) return false;
    if ((pStep0->mdtData->pStationDataList->size() != pStep1->mdtData->pStationDataList->size())
            || (pStep0->mdtData->pStationDataList->size() != pStep2->mdtData->pStationDataList->size())
            || (pStep0->mdtData->pStationDataList->size() != pStep3->mdtData->pStationDataList->size()))
    {
        ATH_MSG_ERROR(  "recalculateFirstSteps()::different number of stations  --> BUG" );
        return false;
    }
    for (unsigned int i = 0; i < pStep0->mdtData->pStationDataList->size(); i++)
    {
        double step0ratio = (*(*pStep0->mdtData->pStationDataList)[i]).chi2 / (pStep0->chi2 * (pStep0->dof));
        double step1ratio = (*(*pStep1->mdtData->pStationDataList)[i]).chi2 / (pStep1->chi2 * (pStep1->dof));
        double step2ratio = (*(*pStep2->mdtData->pStationDataList)[i]).chi2 / (pStep2->chi2 * (pStep2->dof));
        double step3ratio = (*(*pStep3->mdtData->pStationDataList)[i]).chi2 / (pStep3->chi2 * (pStep3->dof));
        double chi2ratio = step0ratio + step1ratio + step2ratio + step3ratio;
        if (chi2ratio < 3) continue;
        //remove the station
        ATH_MSG_VERBOSE( "removing station #" << i );
        m_skipStation = i;
        clearFcnSteps();
        return true;
    }
    ATH_MSG_VERBOSE( "done" );
    return false;
}

bool StauTool::selectRange(double& min, double& max)
{
    //return true;
    ATH_MSG_VERBOSE( "entered" );
    m_lowerLimitNoMdt = min;
    m_upperLimitNoMdt = max;
    auto& firstSteps = *m_pFcnSteps;
    unsigned int iFirstStep = 0;
    unsigned int iLastStep = firstSteps.size() - 1;
    unsigned int minRpcLoc = (m_pStauRPC != NULL && m_doRpc ? m_pStauRPC->rpcMinIsAt() : 0);
    unsigned int minTileLoc = (m_pStauTileCal != NULL && m_pStauTileCal->hasCells() ? m_pStauTileCal->tileMinIsAt() : 0);
    unsigned int minRpcTileLoc = (minRpcLoc != 0 ? minRpcLoc : minTileLoc);
    //unsigned int minRpcTileLoc = minRpcLoc; //The tile beta distribution seems wide. Do not select the range according to it.
    if (minRpcTileLoc != 0)
    {
        iFirstStep = (minRpcTileLoc > 3 ? minRpcTileLoc - 3 : 0);
        iLastStep = (minRpcTileLoc + 3 < iLastStep ? minRpcTileLoc + 3 : iLastStep);
        min = m_lowerLimitNoMdt = firstSteps[iFirstStep]->beta; // - 0.1;
        max = m_upperLimitNoMdt = firstSteps[iLastStep]->beta; //  + 0.1;
        if (!m_doMdt) return true;
    }
    ATH_MSG_VERBOSE( "minRpcLoc " << minRpcLoc
                     << " minTileLoc " << minTileLoc
                     << " minRpcTileLoc " << minRpcTileLoc );
    ATH_MSG_VERBOSE( "before mdt selection: min=" << min << " max=" << max );
//    return true;
    double maxHits = 0;
    bool decrease = false;
    bool reduceMaxHits = false;
    int loc = 0;
    for (unsigned int i = iFirstStep; i <= iLastStep; i++)
    {
        auto pStep = firstSteps[i];
        double numSegs = pStep->mdtData->numSegs;
        double numHits = pStep->mdtData->hitsInSegments;
        ATH_MSG_VERBOSE( "beta=" << pStep->beta
                         << " chi2=" << pStep->chi2
                         << " numHits=" << pStep->mdtData->hitsInSegments
                         << " maxHits=" << maxHits
                         << " min=" << min
                         << " max=" << max
                         << " numSegs=" << numSegs );
        if (numSegs == 0) continue;
        if (maxHits < numHits)
        {
            maxHits = numHits;
            loc = i;
            min = pStep->beta;
            max = pStep->beta;
            decrease = false;
        }
        else if (maxHits == numHits)
        {
            max = pStep->beta;
            loc = i;
            if (decrease && !reduceMaxHits) min = pStep->beta;
            decrease = false;
            reduceMaxHits = false;
        }
        else
        {
            double extraHits = maxHits - numHits;
            double diffChi2 = firstSteps[i - 1]->mdtData->chi2 - pStep->mdtData->chi2;
            if (diffChi2 / extraHits > 15)
            { //the additional hits contributes too much to the chi2
                maxHits = numHits;
                min = pStep->beta;
                max = pStep->beta;
                reduceMaxHits = true;
            }
            else
                reduceMaxHits = false;
            decrease = true;
        }
    }
    ATH_MSG_VERBOSE( "done: min=" << min << " max=" << max );
    if (min == max)
    {
        if (loc == 0 || (int) firstSteps.size() - 1 == loc) return false;
        double locChi2 = firstSteps[loc]->chi2;
        double prevChi2 = firstSteps[loc - 1]->chi2;
        double nextChi2 = firstSteps[loc + 1]->chi2;
        if (locChi2 > prevChi2 || locChi2 > nextChi2) return false;
        min -= 0.05;
        max += 0.05;
    }
    return true;
}

StatusCode StauTool::readTagInfo(IOVSVC_CALLBACK_ARGS)
{
    // Get TagInfo and retrieve tags
    ATH_MSG_DEBUG( "entered" );
    if (m_isData)
    {
        ATH_MSG_DEBUG( "This is data: exiting, use default configuration:"
                       << " addMuToF is set to " << m_addMuToF
                       << " timeShift is " << m_rpcTimeShift
                       << " rpcBugFix is " << m_rpcBugFix );
        return StatusCode::SUCCESS;
    }
    const TagInfo* tagInfo = 0;
    ServiceHandle<StoreGateSvc> pDetStore("DetectorStore", name());
    ATH_CHECK( pDetStore.retrieve() );
    StatusCode sc = pDetStore->retrieve(tagInfo);
    std::string rpcDigit = "";
    std::string atlasRel = "";
    std::string timeShift = "";
    std::string atlasRel_main = "";
    if (sc.isFailure() || tagInfo == 0)
    {
        ATH_MSG_DEBUG( "no TagInfo in DetectorStore" );
        m_addMuToF = true;
        return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG( "found TagInfo in DetectorStore" );
    TagInfo::NameTagPairVec aNameTagPairVec;
    tagInfo->getInputTags(aNameTagPairVec);
    for (unsigned int Item = 0; Item < aNameTagPairVec.size(); Item++)
    {
        if (aNameTagPairVec[Item].first == "RPC_TimeSchema")
        {
            rpcDigit = aNameTagPairVec[Item].second;
            ATH_MSG_DEBUG( "RPC_Digitization from TagInfoVector is " << rpcDigit );
        }
        if (aNameTagPairVec[Item].first == "AtlasRelease")
        {
            atlasRel = aNameTagPairVec[Item].second;
            ATH_MSG_DEBUG( "AtlasRelease from TagInfoVector is " << atlasRel );
        }
    }
    if (rpcDigit == "")
    {
        if (atlasRel == "")
        {
            m_addMuToF = true;
            m_rpcTimeShift = 0;
            m_rpcBugFix = false;
            ATH_MSG_DEBUG( "this is Data: addMuToF is set to " << m_addMuToF );
        }
        else
        {
            atlasRel_main = atlasRel.substr(13, 14);
            int atlRel_main = atoi(atlasRel_main.c_str());
            if (atlRel_main >= 15)
            {
                if (atlasRel.substr(0, 19) == "AtlasOffline-15.3.1"
                        && atlasRel.substr(0, 21) != "AtlasOffline-15.3.1.8")
                {
                    m_addMuToF = true;
                    m_rpcTimeShift = 0;
                    m_rpcBugFix = false;

                }
                else if (atlasRel.substr(0, 21) == "AtlasOffline-15.3.1.8")
                {
                    m_addMuToF = true;
                    m_rpcTimeShift = 5;
                    m_rpcBugFix = false;
                }
                else
                {
                    m_addMuToF = false;
                    m_rpcTimeShift = 0;
                    m_rpcBugFix = false;
                }
            }
            else
            {
                m_addMuToF = false;
                m_rpcTimeShift = 0;
                m_rpcBugFix = true;
                ATH_MSG_DEBUG( "release " << atlRel_main << ": addMuToF is set to  " << m_addMuToF );
            }
        }
    }
    else
    {
        if (rpcDigit.substr(0, 8) == "Datalike")
        {
            m_addMuToF = true;
            timeShift = rpcDigit.substr(25, 1);
            m_rpcTimeShift = atoi(timeShift.c_str());
            m_rpcBugFix = false;
        }
        if (rpcDigit.substr(0, 6) == "G4like")
        {
            m_addMuToF = false;
            timeShift = rpcDigit.substr(23, 1);
            m_rpcTimeShift = atoi(timeShift.c_str());
            m_rpcBugFix = false;
        }
    }
    ATH_MSG_DEBUG( "addMuToF is set to  " << m_addMuToF
                   << " timeShift is " << m_rpcTimeShift
                   << " rpcBugFix is " << m_rpcBugFix );
    return StatusCode::SUCCESS;
}

StatusCode StauTool::processStauCosmic(const xAOD::TrackParticle* trkParticle,
        const MuonSegmentList& muonSegments,
        const MdtSegmentMakerInfoList& mdtSegmentMakerInfoList,
        const RIO_OnTrackLists& rpcHitsInSegments,
        const RIO_OnTrackLists& tgcHitsInSegments,
        Amg::Vector3D trigMomentum)
{
    //if(NULL!=trkParticle->originalTrack()) ATH_MSG_VERBOSE(  "processStau()" );
    ATH_MSG_VERBOSE( (trkParticle != NULL ? "entered" : "no ID track") );
    if (trkParticle == NULL) return StatusCode::SUCCESS;
    //clear the candidate
    clearCandidate();
    //double trigMomentum;
    double beta = (trkParticle->phi() > 0) ? -1.0 : 1.0; // TODO change to y coordinate of the hits
    //initialize the candidate
    if (mdtSegmentMakerInfoList.empty() && rpcHitsInSegments.empty() && tgcHitsInSegments.empty())
    {
        ATH_MSG_VERBOSE( "candidate with no MS info -  done" );
        return StatusCode::SUCCESS;
    }
    initializeCandidate(trkParticle, muonSegments, mdtSegmentMakerInfoList, rpcHitsInSegments, tgcHitsInSegments, NULL,
            NULL, trigMomentum);
    //call the minimization functions
    StauCosmicFcn stauCosmicFcn(this, msg(), beta);
    m_minTTrack = m_lowerTTrackLimit;
    m_maxTTrack = m_upperTTrackLimit;
    if (m_doRpc || m_doMdt)
    {
        if (!stauCosmicFcn.selectRange(m_minTTrack, m_maxTTrack))
        {
            m_tTrack = StauTTrackDefault;
            m_beta = beta;
            //std::cout << "E123: " << m_minTTrack << " " <<  m_maxTTrack << std::endl;
            return StatusCode::SUCCESS;
        }
    }
    //
    m_tTrack = StauTTrackDefault;
    Minimizer minim;

    if (m_doMdt || (m_doRpc && m_pStauRPC->hasHits()) || (m_doTileCal && m_pStauTileCal->hasCells()))
    {
        //double max = 25.;
        //double min = -25.;
        m_minTTrack = -50.0;
        m_maxTTrack = 50.0;
        double mina = minim.minimize(stauCosmicFcn, ALL_TECHS, m_minTTrack, m_maxTTrack, m_betaTolerance).first;
        std::cout << "E123: MINA " << mina << " phi " << trkParticle->phi() << " eta " << trkParticle->eta() << " pt:  "
                << trkParticle->pt() << " d0 " << trkParticle->perigeeParameters().parameters()[Trk::d0] << " z0  "
                << trkParticle->perigeeParameters().parameters()[Trk::z0] << std::endl;
        if (fabs(m_minTTrack - mina) < 0.0005 || fabs(m_maxTTrack - mina) < 0.0005)
        {
            ATH_MSG_VERBOSE( "min is on the edge" );
            m_tTrack = StauTTrackOnEdge;
        }
        else
        {
            m_tTrack = mina;
            // FcnStepsData fcnSteps = *m_pFcnSteps;
            // FcnStepData* pLastStep = fcnSteps.back();
            // m_numMdtSegs = (NULL!= pLastStep->mdtData ? (int)pLastStep->mdtData->numSegs : 0);
            // m_numMdtHitsInSeg = (NULL!= pLastStep->mdtData ? (int)pLastStep->mdtData->hitsInSegments : 0);

        }
    }           //else clearFcnSteps();
                //recalculate and store the mdt segments with the new beta
    // printFcnSteps();
    //std::cout << "All betas:  AOD = " << m_beta << " ALL = " << m_betaAll << " MS = " << m_betaMS << " noRpc = " << m_betaNoRpc << " MDT = " << m_pStauMDT->beta() << " RPC = " << m_pStauRPC->beta() << " Tile = " << m_pStauTileCal->beta() << std::endl;
    ATH_MSG_VERBOSE( "done" );
    return StatusCode::SUCCESS;
}

bool StauTool::selectRangeTTrack(double& min, double& max)
{
    //return true;
    ATH_MSG_VERBOSE( "entered" );
    if (noIDTrack()) return false;
    auto& firstSteps = *m_pFcnSteps;
    unsigned int iFirstStep = 0;
    unsigned int iLastStep = firstSteps.size() - 1;
    unsigned int minRpcLoc = (m_pStauRPC != NULL && m_doRpc ? m_pStauRPC->rpcMinIsAt() : 0);
    if (minRpcLoc != 0)
    {
        iFirstStep = (minRpcLoc > 2 ? minRpcLoc - 2 : 0);
        iLastStep = (minRpcLoc + 2 < iLastStep ? minRpcLoc + 2 : iLastStep);
        min = firstSteps[iFirstStep]->tTrack - 2.0;
        max = firstSteps[iLastStep]->tTrack + 2.0;
        if (!m_doMdt) return true;
    }
    //std::cout << "E123:: tTrack,mdtChi2,mdtDof,tileChi2,tileDof,beta" << std::endl;
    //double maxHits = 0;
    //bool decrease = false;
    //bool reduceMaxHits = false;
    unsigned int loc = 0;
    Range bestRange;
    Range currentRange;
    for (unsigned int i = iFirstStep; i <= iLastStep; i++)
    {
        auto pStep = firstSteps[i];
        double numSegs = pStep->mdtData->numSegs;
        double numHits = pStep->mdtData->hitsInSegments;
        //m_log << MSG::VERBOSE  // tTrack:mdtChi2:mdtSegs:mdtHits:
        //std::cout << "E123:: " << pStep->tTrack << "," << pStep->mdtData->chi2 << ","
        //        << pStep->mdtData->totNumHits - 2 * pStep->mdtData->pStationDataList->size() << ","
        //        << pStep->mdtData->hitsInSegments << "," << pStep->tileCalData->chi2 << ","
        //        << pStep->tileCalData->numOfCells << std::endl;
        if (numSegs == 0) continue;
        if (currentRange.nHits != numHits) currentRange.clear();
        if (!currentRange.addStepLast(i, numHits, pStep->mdtData->chi2, pStep->tTrack))
	  //std::cout << "E123: " << __LINE__ << " sanity check failed" << std::endl;
	  //std::cout << "E123: " << __LINE__ << " size " << currentRange.size() << std::endl;
        if (currentRange.isBetter(bestRange))
        {
	  //std::cout << "E123:  current range is better: " << std::endl;
            bestRange = currentRange;
        }
    }
    if (bestRange.size() < 1)
    {
        return false;
        //std::cout << "E123: didnt find a range" << std::endl;
    }
    if (bestRange.size() > 1)
    {
        max = (bestRange.max < (bestRange.minChi2Param + 2.0)) ? bestRange.max : bestRange.minChi2Param + 2.0;
        min = (bestRange.min > (bestRange.minChi2Param - 2.0)) ? bestRange.min : bestRange.minChi2Param - 2.0;
        //std::cout << "E123: " << "range" << max << " " << min << std::endl;
        return true;
    }
    if (bestRange.size() == 1)
    {
        if (loc > firstSteps.size() - 2) return false;
        loc = bestRange.i_minChi2;
        double locChi2 = firstSteps[loc]->chi2;
        double prevChi2 = firstSteps[loc - 1]->chi2;
        double nextChi2 = firstSteps[loc + 1]->chi2;
        if (locChi2 > prevChi2 || locChi2 > nextChi2) return false;
        min = bestRange.min - 2.0;
        max = bestRange.max + 2.0;
        return true;
    }
    ATH_MSG_VERBOSE( "done: min=" << min << " max=" << max );
    return false;
}
} // end of namespace
