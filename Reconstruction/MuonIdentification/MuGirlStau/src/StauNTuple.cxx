/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauTool.h"
#include "MuGirlStau/StauNTuple.h"
#include "MuGirlStau/StauRPC.h"
#include "MuGirlStau/StauMDT.h"
#include "MuGirlStau/StauTileCal.h"

MuGirlNS::StauNTuple::StauNTuple(MsgStream& log) :
        m_log(log), m_maxSteps(300), //30
        m_maxStationNsteps(800), //130
        m_maxRpcHits(100)
{
}

StatusCode MuGirlNS::StauNTuple::book(NTuple::Tuple* pNTuple)
{
    LOG_DEBUG << "entered" << endreq;

    if (pNTuple->addItem("MUGIRL/StauBeta", m_beta).isFailure()
            || pNTuple->addItem("MUGIRL/StauMass", m_mass).isFailure()
            || pNTuple->addItem("MUGIRL/StauTZero", m_tzero).isFailure()
            || pNTuple->addItem("MUGIRL/StauBetaAll", m_betaAll).isFailure()
            || pNTuple->addItem("MUGIRL/StauBetaAllt", m_betaAllt).isFailure()
            || pNTuple->addItem("MUGIRL/StauBetaAvgRpc", m_betaAvgRpc).isFailure()
            || pNTuple->addItem("MUGIRL/StauBetaAvgTileCal", m_betaAvgTileCal).isFailure()
            || pNTuple->addItem("MUGIRL/StauIsBarrel", m_fcnType).isFailure())
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<stau summary>" << endreq;
        return StatusCode::RECOVERABLE;
    }

    if (pNTuple->addItem("MUGIRL/StauSteps", m_steps, 0, m_maxSteps).isFailure() ||

    pNTuple->addItem("MUGIRL/StauStepBeta", m_steps, m_stepBeta).isFailure()
            || pNTuple->addItem("MUGIRL/StauStepChi2", m_steps, m_stepChi2).isFailure()
            || pNTuple->addItem("MUGIRL/StauStepDoF", m_steps, m_stepDoF).isFailure() ||

            pNTuple->addItem("MUGIRL/StauMdtStepChi2", m_steps, m_mdtStepChi2).isFailure()
            || pNTuple->addItem("MUGIRL/StauMdtStepHits", m_steps, m_mdtTotNumHits).isFailure()
            || pNTuple->addItem("MUGIRL/StauMdtStepSegs", m_steps, m_mdtNumSegs).isFailure() ||

            pNTuple->addItem("MUGIRL/StauRpcStepChi2", m_steps, m_rpcStepChi2).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcStepHits", m_steps, m_rpcTotNumHits).isFailure() ||

            pNTuple->addItem("MUGIRL/StauTileCalStepChi2", m_steps, m_tileCalStepChi2).isFailure()
            || pNTuple->addItem("MUGIRL/StauTileCalStepCells", m_steps, m_tileCalNumCells).isFailure()
            ||

            pNTuple->addItem("MUGIRL/StauMdtStationNSteps", m_mdtStationNsteps, 0,
                    m_maxStationNsteps).isFailure()
            ||

            pNTuple->addItem("MUGIRL/StauMdtStationBeta", m_mdtStationNsteps, m_mdtStepStationBeta).isFailure()
            || pNTuple->addItem("MUGIRL/StauMdtStationChi2", m_mdtStationNsteps,
                    m_mdtStepStationChi2).isFailure()
            || pNTuple->addItem("MUGIRL/StauMdtStationNumHits", m_mdtStationNsteps,
                    m_mdtStationNumHits).isFailure()
            || pNTuple->addItem("MUGIRL/StauMdtStationNumSegs", m_mdtStationNsteps,
                    m_mdtStationNumSegs).isFailure()
            || pNTuple->addItem("MUGIRL/StauMdtStationDist", m_mdtStationNsteps,
                    m_mdtStationDistance).isFailure()
            || pNTuple->addItem("MUGIRL/StauMdtStationEta", m_mdtStationNsteps, m_mdtStationSegEta).isFailure()
            || pNTuple->addItem("MUGIRL/StauMdtStationPhi", m_mdtStationNsteps, m_mdtStationSegPhi).isFailure()
            ||

            pNTuple->addItem("MUGIRL/StauRpcStationNSteps", m_rpcStationNsteps, 0,
                    m_maxStationNsteps).isFailure()
            ||

            pNTuple->addItem("MUGIRL/StauRpcStationBeta", m_rpcStationNsteps, m_rpcStepStationBeta).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcStationChi2", m_rpcStationNsteps,
                    m_rpcStepStationChi2).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcStationNumHits", m_rpcStationNsteps,
                    m_rpcStationNumHits).isFailure() ||

            pNTuple->addItem("MUGIRL/StauRpcHits", m_rpcHits, 0, m_maxRpcHits).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcHitsDistance", m_rpcHits, m_rpcHitDistance).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcHitsMeasuredTime", m_rpcHits, m_rpcHitMeasuredTime).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcHitsPropagationTime", m_rpcHits,
                    m_rpcHitPropagationTime).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcX", m_rpcHits, m_rpcHitX).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcY", m_rpcHits, m_rpcHitY).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcZ", m_rpcHits, m_rpcHitZ).isFailure()
            || pNTuple->addItem("MUGIRL/StauRpcHitsIsEta", m_rpcHits, m_rpcHitIsEta).isFailure())
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<stau step data>" << endreq;
        return StatusCode::RECOVERABLE;
    }
    LOG_DEBUG << "done" << endreq;

    return StatusCode::SUCCESS;
}

StatusCode MuGirlNS::StauNTuple::fillStau(StauTool* pStau)
{
    LOG_DEBUG << "entered" << endreq;

    m_beta = (float) pStau->beta();
    m_mass = (float) pStau->mass();
    m_tzero = (float) pStau->tzero();
    m_betaAll = (float) pStau->techBetaChi2()[ALL_TECHS].beta;
    m_betaAllt = (float) pStau->techBetaChi2()[ALLT_TECHS].beta;

    m_betaAvgRpc = (pStau->pStauRPC() != NULL ? (float) pStau->pStauRPC()->avgBeta() : 0.);
    m_betaAvgTileCal = (
            pStau->pStauTileCal() != NULL ? (float) pStau->pStauTileCal()->avgBeta() : 0.);

//     m_inputMdtHits = (float)pStau->inputMdtHits();
//     m_outputMdtHits = (float)pStau->outputMdtHits();
//     m_baseP = (float)pStau->baseP();
//     m_basePt = (float)pStau->basePt();
//     m_baseFitChi2 = (float)pStau->baseFitChi2();
//     m_measurementSetSize = (float)pStau->measurementSetSize();

    auto rpcHitsByStations = (
            pStau->pStauRPC() != NULL ? pStau->pStauRPC()->rpcHitsByStations() : NULL);
    m_rpcHits = 0;
    if (rpcHitsByStations != NULL)
    {
        for (auto rpcHits : *rpcHitsByStations)
        {
            for (auto pHit : *rpcHits)
            {
                m_rpcHitDistance[m_rpcHits] = pHit->distance;
                m_rpcHitMeasuredTime[m_rpcHits] = pHit->measuredTime;
                m_rpcHitPropagationTime[m_rpcHits] = pHit->propagationTime;
                m_rpcHitX[m_rpcHits] = pHit->x;
                m_rpcHitY[m_rpcHits] = pHit->y;
                m_rpcHitZ[m_rpcHits] = pHit->z;
                m_rpcHitIsEta[m_rpcHits] = pHit->isEta;
                m_rpcHits++;
            }
        }
    }

    FcnStepsData* pFcnStepsData = pStau->fcnStepsData();
    if (pFcnStepsData != NULL && pFcnStepsData->size() >= 1)
    {
        auto pStepData = pFcnStepsData->front();
        if (pStepData != NULL) m_fcnType = pStepData->isBarrel;
        m_steps = 0;
        m_mdtStationNsteps = 0;
        m_rpcStationNsteps = 0;
        for (auto pStepData : *pFcnStepsData)
        {
            m_stepBeta[m_steps] = ((float) pStepData->beta); /** the beta of the step   */
            m_stepChi2[m_steps] = pStepData->chi2; /** the chi2 of the step   */
            m_stepDoF[m_steps] = pStepData->dof;

            /** Fill MDT ntuple data*/
            auto pMdtStepData = pStepData->mdtData;
            if (pMdtStepData != NULL)
            {
                auto pMdtStationDataList = pMdtStepData->pStationDataList;
                m_mdtStepChi2[m_steps] = pMdtStepData->chi2; /** the mdt contribution to the chi2   */
                m_mdtTotNumHits[m_steps] = pMdtStepData->hitsInSegments;
                m_mdtNumSegs[m_steps] = pMdtStepData->numSegs;

                if (pMdtStationDataList != NULL)
                {
                    for (auto pMdtStepStationData : *pMdtStationDataList)
                    {
                        m_mdtStepStationChi2[m_mdtStationNsteps] = pMdtStepStationData->chi2;
                        m_mdtStepStationBeta[m_mdtStationNsteps] = pStepData->beta;
                        m_mdtStationNumHits[m_mdtStationNsteps] = pMdtStepStationData->numHits;
                        m_mdtStationNumSegs[m_mdtStationNsteps] = pMdtStepStationData->numSegs;
                        m_mdtStationDistance[m_mdtStationNsteps] = pMdtStepStationData->distance;
                        m_mdtStationSegEta[m_mdtStationNsteps] = pMdtStepStationData->dirEta;
                        m_mdtStationSegPhi[m_mdtStationNsteps] = pMdtStepStationData->dirPhi;
                        m_mdtStationNsteps++;
                    }
                }
            }

            /** Fill Rpc ntuple data */
            auto pRpcStepData = pStepData->rpcData;
            if (pRpcStepData != NULL)
            {
                auto pRpcStationDataList = pRpcStepData->pStationDataList;
                m_rpcStepChi2[m_steps] = pRpcStepData->chi2; /** the rpc contribution to the chi2   */
                m_rpcTotNumHits[m_steps] = pRpcStepData->totNumHits;

                if (pRpcStationDataList != NULL)
                {
                    for (auto pRpcStepStationData : *pRpcStationDataList)
                    {
                        m_rpcStepStationChi2[m_rpcStationNsteps] = pRpcStepStationData->chi2;
                        m_rpcStepStationBeta[m_rpcStationNsteps] = pStepData->beta;
                        m_rpcStationNumHits[m_rpcStationNsteps] = pRpcStepStationData->numHits;
                        m_rpcStationNsteps++;
                    }
                }
            }

            /** Fill Tile Cal step data */
            auto pTileStepData = pStepData->tileCalData;
            if (pTileStepData != NULL)
            {
                m_tileCalStepChi2[m_steps] = pTileStepData->chi2; /** the tile contribution to the chi2   */
                m_tileCalNumCells[m_steps] = pTileStepData->numOfCells;
            }
            m_steps++;
            if (m_maxStationNsteps - 5 < m_mdtStationNsteps || m_maxSteps - 5 < m_steps
                    || m_maxRpcHits - 5 < m_rpcStationNsteps)
            {
                LOG_INFO << "has too many steps, stop writing them" << endreq;
                return StatusCode::SUCCESS;
            }

        }
    }
    else
        LOG_INFO << "got NULL list of fcn steps" << endreq;
    LOG_DEBUG << "done" << endreq;

    return StatusCode::SUCCESS;
}

