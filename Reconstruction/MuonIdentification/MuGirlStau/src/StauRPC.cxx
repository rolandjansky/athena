/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauRPC.h"
#include "MuGirlStau/StauCalibration.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"

//================ Constructor =================================================

MuGirlNS::StauRPC::StauRPC(StauTool* pStauTool, MsgStream& log,
                           CLHEP::HepRandomEngine& randEngine,
        const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments) :
                m_pStau(pStauTool),
                m_log(log),
                m_rpcHitsByStations(NULL),
                m_beta(StauBetaDefault),
                m_avgBeta(StauBetaDefault),
                m_rmsBeta(-1.),
                m_hasHits(false),
                m_pCalibration(nullptr),
                m_randEngine (randEngine)
{
    m_pMuonMgr = pStauTool->muonMgr();
    m_addMuToF = pStauTool->addMuToF();

    double timeShift = m_pStau->rpcTimeShift();

    m_rpcHitsByStations = new StauRpcHitsLists();

    if (m_pStau->doCalibration())
    {
        m_pCalibration = m_pStau->calibration().getRpcCalibration();
    }

    auto pIdHelper = m_pMuonMgr->rpcIdHelper(); // new RpcIdHelper();
    int numOfHits = 0;

    for (auto pRioList : rpcHitsInSegments)
    {
        auto pRpcHits = new StauRpcHitsList();
        for (auto pRIO : *pRioList)
        {
            const Muon::RpcClusterOnTrack* pRpcRIO = dynamic_cast<const Muon::RpcClusterOnTrack*>(pRIO);
            if( !pRpcRIO ) {
                LOG_DEBUG << "Cannot dynamic_cast ROT to const Muon::RpcClusterOnTrack* - continuing" << endreq; 
                continue; 
            }
            auto pPrepData = pRpcRIO->prepRawData();
            auto pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepData);
            if (pRpcPrepData == NULL){ 
              LOG_DEBUG << "Cannot convert PrepRawData to RpcPrepData - continuing" << endreq; 
              continue; 
            } 
            bool b_isEta = !pIdHelper->measuresPhi(pRpcRIO->identify());
            auto pos = pRpcRIO->globalPosition();

            double shift = 0;
            double error = RPCRESOLUTION; // default time resolution
            double distance = fabs(pos.perp()) / 1000; //[m]
            double muonToF = distance / SPEEDOFLIGHT;
            double measuredTime = pRpcRIO->time();
            double propTime = pRpcPrepData->time() - measuredTime;
            LOG_DEBUG << "RPC hit - distance: " << distance
                      << ", pRpcPrepData->time(): " << pRpcPrepData->time()
                      << ", timeShift: " << timeShift
                      << " -> measuredTime: " << measuredTime
                      << endreq;
            if (m_pStau->doCalibration())
            { //Use external calibration files
	      int id = pRpcRIO->identify().get_identifier32().get_compact();
                auto itCalib = m_pCalibration->find(id);
                if (itCalib == m_pCalibration->end()) continue;
                double error = itCalib->second.error;

                if (m_pStau->isData())
                {
                    shift = itCalib->second.timeShift; //shift
                    measuredTime -=  shift;
                }
                else
                { //smear
                    shift = m_pStau->rpcSmearFactor() * CLHEP::RandGaussZiggurat::shoot(&m_randEngine, 0, error); //Low 1.5 Mid 2 High 2.5
                    measuredTime -= shift;
                }
            }
            auto pRpcHit = new StauRpcHit();

            if (m_addMuToF)
            {
                //the time is calibrated such that the muon tof is reduced. so add it
                measuredTime += muonToF;
            }

            // moved up        const RpcIdHelper* pIdHelper = m_pMuonMgr->rpcIdHelper();// new RpcIdHelper();
            // moved up       bool b_isEta = !pIdHelper->measuresPhi(pRpcRIO->identify());

            //auto pROElement = m_pMuonMgr->getRpcReadoutElement(pRpcRIO->identify());
            pRpcHit->id = pRpcRIO->identify();
            pRpcHit->distance = distance;
            pRpcHit->propagationTime = propTime;
            pRpcHit->measuredTime = measuredTime;
            pRpcHit->muonToF = muonToF;
            pRpcHit->error = error;
            pRpcHit->x = pos.x();
            pRpcHit->y = pos.y();
            pRpcHit->z = pos.z();
            pRpcHit->isEta = b_isEta;

            pRpcHits->push_back(pRpcHit);
            numOfHits++;

            StauHit hit( b_isEta ? RPCETA_STAU_HIT : RPC_STAU_HIT, measuredTime, pos.x(), pos.y(), pos.z(),
                    pRpcRIO->identify(), -1, error, shift, b_isEta, propTime);
            m_hits.push_back(hit);
        }
        m_rpcHitsByStations->push_back(pRpcHits);
    }

//    pStauTool->setRpcHitsLists(m_rpcHitsByStations);

    if (numOfHits > 0)
    {
        m_hasHits = true;
        averageBeta();
    }
}

//================ Destructor =================================================

MuGirlNS::StauRPC::~StauRPC()
{
}

void MuGirlNS::StauRPC::init()
{

}

void MuGirlNS::StauRPC::clear()
{
    if (/*!m_pStau->hasSummary() && */m_rpcHitsByStations != NULL)
    {
        for (auto pRpcHits : *m_rpcHitsByStations)
        {
            for (auto pHit : *pRpcHits)
                delete pHit;
            pRpcHits->clear();
            delete pRpcHits;
        }

        m_rpcHitsByStations->clear();
        delete m_rpcHitsByStations;
        m_rpcHitsByStations = NULL;
    }
    m_beta = StauBetaDefault;
    m_avgBeta = StauBetaDefault;
    m_rmsBeta = -1;
    m_hasHits = false;
    m_hits.clear();
}

void MuGirlNS::StauRPC::initStepData(RpcStepData* rpcData, double beta, double tTrack)
{
    rpcData->pStationDataList = new std::vector<RpcStepStationData*>;
    rpcData->beta = beta;
    rpcData->chi2 = 0.;
    rpcData->dof = 0.;
    rpcData->totNumHits = 0.;
    rpcData->tTrack = tTrack;
}

void MuGirlNS::StauRPC::clearStepData(RpcStepData* rpcData)
{
    if (rpcData == NULL) return;
    auto pRpcStepStationsData = rpcData->pStationDataList;
    if (pRpcStepStationsData != NULL)
    {
        for (auto pStationData : *pRpcStepStationsData)
            delete pStationData;
        pRpcStepStationsData->clear();
        delete pRpcStepStationsData;
    }
    delete rpcData;
}

void MuGirlNS::StauRPC::printStepData(RpcStepData* rpcData)
{
    if (rpcData == NULL) return;
    LOG_VERBOSE << "rpc data: beta=" << rpcData->beta
                << " chi2=" << rpcData->chi2
                << " dof=" << rpcData->totNumHits
                << endreq;
    auto pRpcStationDataList = rpcData->pStationDataList;

    int j = 0;
    if (pRpcStationDataList != NULL)
    {
        for (auto rpcStepStation : *pRpcStationDataList)
        {
            j++;
            LOG_VERBOSE << "       station #" << j
//                    << ": distance=" << rpcStepStation->distance
                    << " chi2=" << rpcStepStation->chi2
                    << " #hits=" << rpcStepStation->numHits
                    << endreq;
        }
    }
}

void MuGirlNS::StauRPC::processRpcWithBeta(double currentBeta, RpcStepData* pRpcStepData)
{
    LOG_VERBOSE << "beta=" << currentBeta << endreq;

    pRpcStepData->beta = currentBeta;
    pRpcStepData->chi2 = 0.;
    pRpcStepData->totNumHits = 0.;

    for (auto pHitsList : *m_rpcHitsByStations)
    {
        auto pRpcStepStationsData = new RpcStepStationData();
        int dof = 0;
        double stationChi2 = rpcStationChi2(currentBeta, pHitsList, dof)/*/pRioList->size()*/;
        pRpcStepStationsData->numHits = dof;
        pRpcStepStationsData->chi2 = stationChi2;
        pRpcStepData->pStationDataList->push_back(pRpcStepStationsData);
        pRpcStepData->chi2 += stationChi2;
        pRpcStepData->totNumHits += dof;
    }
    pRpcStepData->dof = pRpcStepData->totNumHits;

    printStepData(pRpcStepData);
    LOG_VERBOSE << "done" << endreq;
}

double MuGirlNS::StauRPC::rpcStationChi2(double currentBeta, StauRpcHitsList* pHitsList, int& dof,
        double tTrack)
{
    double chi2 = 0.;
    double distance;
    double measuredTime;
    double propTime;
    double calculatedToF;
    double stauV;
    double stauToF;
    double error;

    for (auto pHit : *pHitsList)
    {
        distance = pHit->distance; //[m]
        measuredTime = pHit->measuredTime;
        propTime = pHit->propagationTime;
        calculatedToF = measuredTime - propTime;
        stauV = currentBeta * SPEEDOFLIGHT;
        stauToF = distance / stauV + tTrack; // [ns]
        error = pHit->error;
        dof++;
        chi2 += (calculatedToF - stauToF) * (calculatedToF - stauToF) / (error * error);
    }

    return chi2;
}

void MuGirlNS::StauRPC::averageBeta()
{
    //Average
    double up = 0;
    double dwn = 0;

    for (auto pRpcHits : *m_rpcHitsByStations)
    {
        for (auto pRpcHit : *pRpcHits)
        {
            double measuredToF = pRpcHit->measuredTime - pRpcHit->propagationTime;
            double distance = pRpcHit->distance;
            //double measuredBeta = distance / (measureToF * SPEEDOFLIGHT);
            double invBeta = measuredToF * SPEEDOFLIGHT / distance;
            double tofError = pRpcHit->error;
            double invBetaError = tofError * SPEEDOFLIGHT / distance;
            //double measuredBetaError = (distance / SPEEDOFLIGHT) * (1 / (measureToF * measureToF)) * tofError;
            up += (invBeta / (invBetaError * invBetaError));
            dwn += (1. / (invBetaError * invBetaError));
        }
    }
    double avgInvBeta = (dwn != 0) ? up / dwn : 1. / StauBetaDefault;
    m_avgBeta = 1. / avgInvBeta;

    //RMS
    up = 0;
    dwn = 0;

    for (auto pRpcHits : *m_rpcHitsByStations)
    {
        for (auto pRpcHit : *pRpcHits)
        {
            double measureToF = pRpcHit->measuredTime - pRpcHit->propagationTime;
            double distance = pRpcHit->distance;
            //double measuredBeta = distance / (measureToF * SPEEDOFLIGHT);
            double invBeta = SPEEDOFLIGHT * measureToF / distance;
            double deltaInvBeta = invBeta - avgInvBeta;
            double tofError = pRpcHit->error;
            double invBetaError = tofError * SPEEDOFLIGHT / distance;
            //        double measuredBetaError = (distance / SPEEDOFLIGHT) * (1. / (measureToF * measureToF)) * tofError;
            up += deltaInvBeta * deltaInvBeta / (invBetaError * invBetaError);
            dwn += (1. / (invBetaError * invBetaError));
        }
    }
    double rmsInvBeta = (dwn != 0) ? sqrt(up / dwn) : -1;
    m_rmsBeta = (dwn != 0) ? m_avgBeta * m_avgBeta * rmsInvBeta : -1;
}

double MuGirlNS::StauRPC::betaMinTime()
{
    double avgMinBeta = 0;

    for (auto pHitsList : *m_rpcHitsByStations)
    {
        double minTime = 10000000;
        double minDistance = 1000;
        double minPropTime = 1000;

        for (auto pHit : *pHitsList)
        {
            if (pHit->measuredTime < minTime)
            {
                minTime = pHit->measuredTime;
                minDistance = pHit->distance;
                minPropTime = pHit->propagationTime;
            }
        }
        avgMinBeta += (minDistance / (minTime - minPropTime)) / SPEEDOFLIGHT;
    }

    return (m_rpcHitsByStations->empty() ? -1 : avgMinBeta / m_rpcHitsByStations->size());
}

unsigned int MuGirlNS::StauRPC::rpcMinIsAt()
{
    // maybe this can be pre-calculated while function is being evaluated
    auto firstSteps = *(m_pStau->fcnStepsData());
    unsigned int iMinLoc = 0;
    double minChi2 = 100000.;

    if (!(firstSteps[0]->isBarrel)) return 0;

    for (unsigned int i = 0; i < firstSteps.size(); i++)
    {
        auto pStepData = firstSteps[i];
        auto pRpcStepData = pStepData->rpcData;
        double chi2 = pRpcStepData->chi2;

        if (chi2 < minChi2)
        {
            minChi2 = chi2;
            iMinLoc = i;
        }
    }

    return iMinLoc;
}

void MuGirlNS::StauRPC::processRpcWithTTrack(double tTrack, RpcStepData* pRpcStepData, double beta)
{
    LOG_VERBOSE << "tTrack=" << tTrack << endreq;
    //double currentBeta = 1.0;
    pRpcStepData->beta = beta;
    pRpcStepData->tTrack = tTrack;
    pRpcStepData->chi2 = 0.;
    pRpcStepData->totNumHits = 0.;

    for (auto pHitsList : *m_rpcHitsByStations)
    {
        auto pRpcStepStationsData = new RpcStepStationData();
        int dof = 0;
        double stationChi2 = rpcStationChi2(beta, pHitsList, dof, tTrack)/*/pRioList->size()*/;
        pRpcStepStationsData->numHits = dof;
        pRpcStepStationsData->chi2 = stationChi2;
        pRpcStepData->pStationDataList->push_back(pRpcStepStationsData);
        pRpcStepData->chi2 += stationChi2;
        pRpcStepData->totNumHits += dof;
    }
    pRpcStepData->dof = pRpcStepData->totNumHits;

    printStepData(pRpcStepData);
    LOG_VERBOSE << "done" << endreq;
}

MuGirlNS::StauRpcHitsLists* MuGirlNS::StauRPC::copyStauRpcHitsLists()
{
    if (m_rpcHitsByStations == NULL) return NULL;
    auto newRpcHitsByStation = new StauRpcHitsLists();
    for (auto pHitsList : *m_rpcHitsByStations)
    {
        auto pCopyList = new StauRpcHitsList();
        newRpcHitsByStation->push_back(pCopyList);
        for (auto pHit : *pHitsList)
        {
            auto pCopyHit = new StauRpcHit();
            pCopyList->push_back(pCopyHit);

            pCopyHit->id = pHit->id;
            pCopyHit->distance = pHit->distance;
            pCopyHit->propagationTime = pHit->propagationTime;
            pCopyHit->measuredTime = pHit->measuredTime;
            pCopyHit->muonToF = pHit->muonToF;
            pCopyHit->error = pHit->error;
            pCopyHit->x = pHit->x;
            pCopyHit->y = pHit->y;
            pCopyHit->z = pHit->z;
            pCopyHit->isEta = pHit->isEta;
        }
    }
    return newRpcHitsByStation;
}
