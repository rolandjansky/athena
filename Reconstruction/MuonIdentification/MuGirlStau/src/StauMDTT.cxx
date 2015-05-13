/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauMDTT.h"

#include "GaudiKernel/MsgStream.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "MuGirlStau/StauTool.h"
#include "MuGirlStau/IStauBetaTofTool.h"
#include "TrkTrack/Track.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MdtCalibData/MdtFullCalibData.h"
#include "MdtCalibData/TrRelation.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include <sstream>

using namespace MuGirlNS;

double StauMDTT::m_inverseSpeedOfLight = 1. / 299.792458;

StauMDTT::MDTT::MDTT(StauTool* pStau, MsgStream&, const Muon::MdtDriftCircleOnTrack* mdcot,
        double locR, double t0Shift, double error, std::string stype) :
        pos(mdcot->globalPosition()), locR(locR), t0Shift(t0Shift), error(error), stype(stype)
{
    status = mdcot->status();
    mag = mdcot->globalPosition().mag();
    driftRadius = mdcot->driftRadius();
    driftTime = mdcot->driftTime();
    auto detEl = mdcot->detectorElement();
    auto data = pStau->mdtCalibrationDbSvc()->getCalibration(detEl->collectionHash(),
            detEl->detectorElementHash());
    auto rtRelation = data.rtRelation;
    bool out_of_bound_flag = false;
    TlocR = rtRelation->tr()->tFromR(fabs(locR), out_of_bound_flag);
    origTOF = mag * m_inverseSpeedOfLight;
    TOF = origTOF + driftTime - TlocR - t0Shift;
    error2 = error * error;

}

StauMDTT::StauMDTT(StauTool* pStau, MsgStream& log,
                   CLHEP::HepRandomEngine& randEngine) :
        m_pStau(pStau), m_log(log), m_pTrack(NULL), m_baseBeta(-1.), m_avgBeta(StauBetaDefault),
        m_rmsBeta(-1.), m_pCalibration(nullptr), m_randEngine (randEngine)
{
    if (m_pStau->doCalibration()) m_pCalibration = m_pStau->calibration().getMdtCalibration();
}

StauMDTT::~StauMDTT()
{
}

bool StauMDTT::initialize(const Trk::Track* pTrack)
{
    clear();
    if (pTrack == NULL) return false;
    m_pTrack = pTrack;
    auto pTSoSs = pTrack->trackStateOnSurfaces();
    if (pTSoSs == NULL) return false;

    m_pStau->tofTool()->setBeta(1.0);
    for (auto pTSoS : *pTSoSs)
    {
        if (pTSoS->type(Trk::TrackStateOnSurface::Outlier)) continue;
        auto hit = pTSoS->measurementOnTrack();
        if (hit == NULL) continue;
        auto mdcot_orig = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(hit);
        if (mdcot_orig == NULL) continue;

        auto mdcot =
                dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(m_pStau->driftCircleCreator()->correct(
                        *(mdcot_orig->prepRawData()), *(pTSoS->trackParameters())));
        if (mdcot == NULL) continue;

        LOG_VERBOSE << "after correct " << mdcot->globalPosition()
                    << " time " << mdcot->driftTime()
                    << endreq;
        //calibration
        double t0Shift = 0;
        double error = MDTTRESOLUTION;
        if (m_pStau->doCalibration())
        {
            int id = mdcot->detectorElement()->identify().get_identifier32().get_compact()
                    & 0xFFFF0000;
            auto itCalib = m_pCalibration->find(id);
            if (itCalib == m_pCalibration->end())
                continue; //don't use the hit if it doesn't have calibration
            else
            {
                error = itCalib->second.error;
                if (m_pStau->isData())
                    t0Shift = itCalib->second.timeShift; //shift
                else
                { //smear
                    t0Shift = m_pStau->mdtSmearFactor() * CLHEP::RandGaussZiggurat::shoot(&m_randEngine, 0, error); //Sofia: Low 0.5 MID 0.9 HIGH 1.4
                }
            }
        }
        MDTT* pMdtt = new MDTT(m_pStau, m_log, mdcot,
                pTSoS->trackParameters()->parameters()[Trk::locR], t0Shift, error,
                pTSoS->dumpType());
        StauHit stauhit(MDTT_STAU_HIT, pMdtt->TOF, mdcot->globalPosition().x(),
                mdcot->globalPosition().y(), mdcot->globalPosition().z(),
                mdcot->prepRawData()->identify(), -1, error, t0Shift); //E.K. change to tube id
        m_hits.push_back(stauhit);
        delete mdcot;
        m_mdtts.push_back(pMdtt);
    }

    if (!m_mdtts.empty()) averageBeta();
    return (true);
}

void StauMDTT::clear()
{
    m_pTrack = NULL;
    for (auto pMdtt : m_mdtts)
        delete pMdtt;
    m_mdtts.clear();
    m_hits.clear();
    m_avgBeta = StauBetaDefault;
    m_rmsBeta = -1.;
}

std::string StauMDTT::toString() const
{
    std::ostringstream oss;
    oss << "hits[" << m_mdtts.size() << "]:\n";
    for (size_t iMdtt = 0; iMdtt < m_mdtts.size(); ++iMdtt)
    {
        auto pMdtt = m_mdtts[iMdtt];
        oss << "  " << iMdtt + 1 << ": status=" << pMdtt->status << " mag=" << pMdtt->mag
                << " driftRadius=" << pMdtt->driftRadius << " driftTime=" << pMdtt->driftTime
                << " locR=" << pMdtt->locR << " TlocR=" << pMdtt->TlocR << " origTOF="
                << pMdtt->origTOF << " TOF=" << pMdtt->TOF << " error=" << pMdtt->error
                << " TSoSType=" << pMdtt->stype << "\n";
    }
    return (oss.str());
}

void StauMDTT::initStepData(MdttStepData* mdttData, double beta)
{
    clearStepData(mdttData);
    mdttData->beta = beta;
}

void StauMDTT::clearStepData(MdttStepData* mdttData)
{
    if (mdttData == NULL) return;
    mdttData->chi2 = 0.0;
    mdttData->dof = 0;
}

void StauMDTT::printStepData(MdttStepData* mdttData)
{
    if (mdttData != NULL)
        LOG_VERBOSE << "mdtt data:"
                    << " beta=" << mdttData->beta
                    << " chi2=" << mdttData->chi2
                    << " dof=" << mdttData->dof
                    << endreq;
}

void StauMDTT::processWithBeta(double currentBeta, MdttStepData* mdttData)
{
    if (mdttData == NULL) return;
    LOG_VERBOSE << "beta=" << currentBeta << endreq;

    mdttData->chi2 = 0.0;
    for (auto pMdtt : m_mdtts)
    {
        double betaTOF = pMdtt->mag * m_inverseSpeedOfLight / currentBeta;
        double diff = pMdtt->TOF - betaTOF;
        mdttData->chi2 += diff * diff / pMdtt->error2;
    }
    mdttData->dof = m_mdtts.size();

    LOG_VERBOSE << "done chi2=" << mdttData->chi2 << " dof=" << mdttData->dof << endreq;
}

void MuGirlNS::StauMDTT::averageBeta()
{
    //Average
    double up = 0;
    double dwn = 0;
    for (auto pMdtt : m_mdtts)
    {
        double measureToF = pMdtt->TOF;
        double distance = pMdtt->mag;
        double invBeta = CLHEP::c_light * measureToF / distance; //distance / measureToF * m_inverseSpeedOfLight;
        double tofError = pMdtt->error;
        double invBetaError = tofError * CLHEP::c_light / distance;
        //double measuredBetaError = distance * m_inverseSpeedOfLight  * (1 / (measureToF * measureToF)) * tofError;
        up += (invBeta / (invBetaError * invBetaError));
        dwn += (1. / (invBetaError * invBetaError));
    }
    double avgInvBeta = (dwn != 0) ? up / dwn : 1. / StauBetaDefault;
    m_avgBeta = 1. / avgInvBeta; // 1/(up/dwn)

    //RMS
    up = 0;
    dwn = 0;
    for (auto pMdtt : m_mdtts)
    {
        double measureToF = pMdtt->TOF;
        double distance = pMdtt->mag;
        double invBeta = CLHEP::c_light * measureToF / distance;
        double deltaInvBeta = invBeta - avgInvBeta;
        double tofError = pMdtt->error;
        double invBetaError = tofError * CLHEP::c_light / distance;
        //double measuredBetaError = distance * m_inverseSpeedOfLight * (1. / (measureToF * measureToF)) * tofError;
        up += deltaInvBeta * deltaInvBeta / (invBetaError * invBetaError);
        dwn += (1. / (invBetaError * invBetaError));
    }
    double rmsInvBeta = (dwn != 0) ? sqrt(up / dwn) : -1;
    m_rmsBeta = (dwn != 0) ? m_avgBeta * m_avgBeta * rmsInvBeta : -1;
}
