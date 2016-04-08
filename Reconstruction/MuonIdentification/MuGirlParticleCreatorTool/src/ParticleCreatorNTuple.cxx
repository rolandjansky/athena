/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlParticleCreatorTool/ParticleCreatorNTuple.h"
#include "MuGirlInterfaces/CandidateSummary.h"
#include "MuGirlInterfaces/IPerformanceTruthTool.h"

using MuGirlNS::ParticleCreatorNTuple;

ParticleCreatorNTuple::ParticleCreatorNTuple(MsgStream& log) : m_log(log)
{}

StatusCode ParticleCreatorNTuple::book(NTuple::Tuple* pNTuple)
{
    m_log << MSG::INFO << " ParticleCreatorNTuple initialization" << endreq;

    if (    pNTuple->addItem("AOD/RunNumber",   m_RunNumber).isFailure() ||
            pNTuple->addItem("AOD/EventNumber", m_EventNumber).isFailure())
    {
        m_log << MSG::WARNING << "Cannot addItem(AOD/<Run and event numbers>)" << endreq;
        return StatusCode::FAILURE;
    }

    if (
            pNTuple->addItem("NumMdtHits",      m_NumMdtHits).isFailure() ||
            pNTuple->addItem("NumRpcPhiHits",   m_NumRpcPhiHits).isFailure() ||
            pNTuple->addItem("NumRpcEtaHits",   m_NumRpcEtaHits).isFailure() ||
            pNTuple->addItem("NumCscPhiHits",   m_NumCscPhiHits).isFailure() ||
            pNTuple->addItem("NumCscEtaHits",   m_NumCscEtaHits).isFailure() ||
            pNTuple->addItem("NumMdtSegs",      m_NumMdtSegs).isFailure() ||
            pNTuple->addItem("NumRpcSegs",      m_NumRpcSegs).isFailure() ||
            pNTuple->addItem("NumMdtHitsInSeg", m_NumMdtHitsInSeg).isFailure() ||
            pNTuple->addItem("NumRpcHitsInSeg", m_NumRpcHitsInSeg).isFailure() ||
            pNTuple->addItem("NumTgcRpcPhiHits",m_NumTgcRpcPhiHits).isFailure() ||
            pNTuple->addItem("NumTgcRpcEtaHits",m_NumTgcRpcEtaHits).isFailure() ||
            pNTuple->addItem("NumTgcRpcSegs",   m_NumTgcRpcSegs).isFailure() ||
            pNTuple->addItem("NumTgcRpcHitsInSeg",m_NumTgcRpcHitsInSeg).isFailure() ||
            pNTuple->addItem("NumTgcPhiHits",   m_NumTgcPhiHits).isFailure() ||
            pNTuple->addItem("NumTgcEtaHits",   m_NumTgcEtaHits).isFailure() ||
            pNTuple->addItem("NumTgcSegs",      m_NumTgcSegs).isFailure() ||
            pNTuple->addItem("NumTgcHitsInSeg", m_NumTgcHitsInSeg).isFailure() ||
            pNTuple->addItem("NumCscSegs",      m_NumCscSegs).isFailure() ||
            pNTuple->addItem("AOD/IDEta",       m_IDEta).isFailure() ||
            pNTuple->addItem("AOD/IDPhi",       m_IDPhi).isFailure() ||
            pNTuple->addItem("AOD/IDPt",        m_IDPt).isFailure() ||
            pNTuple->addItem("AOD/RefitEta",    m_RefitEta).isFailure() ||
            pNTuple->addItem("AOD/RefitPhi",    m_RefitPhi).isFailure() ||
            pNTuple->addItem("AOD/RefitPt",     m_RefitPt).isFailure() ||
            pNTuple->addItem("AOD/ChiSq",       m_ChiSq).isFailure() ||
            pNTuple->addItem("AOD/Dof",         m_Dof).isFailure() ||
            pNTuple->addItem("AOD/AnnBarrel",   m_AnnBarrel).isFailure() ||
            pNTuple->addItem("AOD/AnnEndcap",   m_AnnEndcap).isFailure() ||
            pNTuple->addItem("AOD/InnAngle",    m_InnAngle).isFailure() ||
            pNTuple->addItem("AOD/InnDelta",    m_InnDelta).isFailure() ||
            pNTuple->addItem("AOD/MidAngle",    m_MidAngle).isFailure() ||
            pNTuple->addItem("AOD/MidDelta",    m_MidDelta).isFailure() ||
            pNTuple->addItem("AOD/PdgID",       m_PdgID).isFailure() ||
            pNTuple->addItem("AOD/PassNNsel",   m_PassNNsel).isFailure() ||
            pNTuple->addItem("AOD/Beta",        m_Beta).isFailure() ||
            pNTuple->addItem("AOD/Lhr",         m_Lhr).isFailure() ||
            pNTuple->addItem("AOD/SaveAnn",     m_SaveAnn).isFailure() ||
            pNTuple->addItem("AOD/SaveStau",    m_SaveStau).isFailure() )
    {
        m_log << MSG::WARNING << "Cannot addItem(AOD/items)" << endreq;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode ParticleCreatorNTuple::fillEventInfo(unsigned nRunNumber, unsigned nEventNumber)
{
    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "ParticleCreatorNTuple::fillEventInfo(" << nRunNumber << ", " << nEventNumber << ")" << endreq;

    m_RunNumber   = (long)nRunNumber;
    m_EventNumber = (long)nEventNumber;

    return StatusCode::SUCCESS;
}

StatusCode ParticleCreatorNTuple::fill(const CandidateSummary* pSummary,
                                       bool doTruth,
                                       const ToolHandle<IPerformanceTruthTool> &pTruthTool)
{
    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "ParticleCreatorNTuple::fill" << endreq;

    reset();

    m_NumMdtHits = pSummary->numMdtHits;
    m_NumRpcPhiHits = pSummary->numRpcPhiHits;
    m_NumRpcEtaHits = pSummary->numRpcEtaHits;
    m_NumCscPhiHits = pSummary->numCscPhiHits;
    m_NumCscEtaHits = pSummary->numCscEtaHits;
    m_NumMdtSegs = pSummary->numMdtSegs;
    m_NumRpcSegs = pSummary->numRpcSegs;
    m_NumMdtHitsInSeg = pSummary->numMdtHitsInSeg;
    m_NumRpcHitsInSeg = pSummary->numRpcHitsInSeg;
    m_NumTgcRpcPhiHits = pSummary->numTgcRpcPhiHits;
    m_NumTgcRpcEtaHits = pSummary->numTgcRpcEtaHits;
    m_NumTgcRpcSegs = pSummary->numTgcRpcSegs;
    m_NumTgcRpcHitsInSeg = pSummary->numTgcRpcHitsInSeg;
    m_NumTgcPhiHits = pSummary->numTgcPhiHits;
    m_NumTgcEtaHits = pSummary->numTgcEtaHits;
    m_NumTgcSegs = pSummary->numTgcSegs;
    m_NumTgcHitsInSeg = pSummary->numTgcHitsInSeg;
    m_NumCscSegs = pSummary->numCscSegs;
    if (pSummary->pTrackParticle != NULL ) {
        const Trk::Perigee prg = pSummary->pTrackParticle->perigeeParameters();

        m_IDPhi = prg.parameters()[Trk::phi];
        m_IDEta = asinh(1.0 / tan(prg.parameters()[Trk::theta]));
        m_IDPt = prg.pT();

        const xAOD::TrackParticle* refit = pSummary->pRefittedTrack;
        if (refit != NULL)
        {
            const Trk::Perigee prg = refit->perigeeParameters();

            m_RefitPhi = prg.parameters()[Trk::phi];
            m_RefitEta = asinh(1.0 / tan(prg.parameters()[Trk::theta]));
            m_RefitPt = prg.pT();
        
            m_ChiSq = refit->chiSquared();
            m_Dof = refit->numberDoF();
        } 
        else if (pSummary->pTrkRefitted!=NULL) 
        {
            const Trk::Perigee* prg = pSummary->pTrkRefitted->perigeeParameters();
            m_RefitPhi = prg->parameters()[Trk::phi];
            m_RefitEta = asinh(1.0 / tan(prg->parameters()[Trk::theta]));
            m_RefitPt = prg->pT();

            const Trk::FitQuality* fitq = pSummary->pTrkRefitted->fitQuality();

            if (fitq!=NULL) {
                m_ChiSq = fitq->chiSquared();
                m_Dof   = fitq->numberDoF();
            }
        } 
    }

    m_AnnBarrel = pSummary->nnBarrel;
    m_AnnEndcap = pSummary->nnEndCap;
    if (pSummary->innAngle != 999.0) m_InnAngle = pSummary->innAngle;
    m_InnDelta = pSummary->innDelta;
    if (pSummary->midAngle != 999.0) m_MidAngle = pSummary->midAngle;
    m_MidDelta = pSummary->midDelta;

    if (doTruth)
    {
        IPerformanceTruthTool::TruthTrack truthTrack;
        if (pSummary->pTrackParticle != NULL)
        {
           if (pTruthTool->getTruthTrack(pSummary->pTrackParticle, truthTrack,const_cast<CandidateSummary*> (pSummary)).isSuccess())
              m_PdgID = truthTrack.PDG_ID;
        }
        if (pSummary->pMuonFeature != NULL)
        {
            if (pTruthTool->getTruthTrack(pSummary->pMuonFeature, truthTrack,const_cast<CandidateSummary*> (pSummary)).isSuccess())
                m_PdgID = truthTrack.PDG_ID;
        }
        if (pSummary->pMuonEFTrack != NULL)
        {
            if (pTruthTool->getTruthTrack(pSummary->pMuonEFTrack, truthTrack,const_cast<CandidateSummary*> (pSummary)).isSuccess())
                m_PdgID = truthTrack.PDG_ID;
        } 
    }

    m_PassNNsel = pSummary->PassNNsel;
    m_Beta = pSummary->beta;
    m_Lhr = pSummary->lhr;
    m_SaveAnn = pSummary->saveAnn;
    m_SaveStau = pSummary->saveStau;

    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "ParticleCreatorNTuple::fill ended" << endreq;
    return StatusCode::SUCCESS;
}

void ParticleCreatorNTuple::reset()
{
    m_NumMdtHits = -100;
    m_NumRpcPhiHits = -100;
    m_NumRpcEtaHits = -100;
    m_NumCscPhiHits = -100;
    m_NumCscEtaHits = -100;
    m_NumMdtSegs = -100;
    m_NumRpcSegs = -100;
    m_NumMdtHitsInSeg = -100;
    m_NumRpcHitsInSeg = -100;
    m_NumTgcRpcPhiHits = -100;
    m_NumTgcRpcEtaHits = -100;
    m_NumTgcRpcSegs = -100;
    m_NumTgcRpcHitsInSeg = -100;
    m_NumTgcPhiHits = -100;
    m_NumTgcEtaHits = -100;
    m_NumTgcSegs = -100;
    m_NumTgcHitsInSeg = -100;
    m_NumCscSegs = -100;
    m_IDEta = -100.;
    m_IDPhi = -100.;
    m_IDPt = -100.;
    m_RefitEta = -100.;
    m_RefitPhi = -100.;
    m_RefitPt = -100.;
    m_ChiSq = -100.;
    m_Dof = -100;
    m_AnnBarrel = -100.;
    m_AnnEndcap = -100.;
    m_InnAngle = -100.;
    m_InnDelta = -100.;
    m_MidAngle = -100.;
    m_MidDelta = -100.;
    m_PdgID = -100;
    m_PassNNsel = -100.;
    m_Beta = -100.;
    m_Lhr = -100.;
    m_SaveAnn = -100;
    m_SaveStau = -100;
}
