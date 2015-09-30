/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauGF.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "MuGirlInterfaces/SegmentManager.h"
#include <algorithm>
#include <iterator>

//================ Constructor =================================================

MuGirlNS::StauGF::StauGF(StauTool* pStauTool, MsgStream& log,
        const MuGirlNS::MuonSegmentList& muonSegments) :
        m_pStau(pStauTool), m_log(log), m_muonSegments(muonSegments), m_initialTrackParameters(NULL)
{
}

//================ Destructor =================================================

MuGirlNS::StauGF::~StauGF()
{
}

void MuGirlNS::StauGF::initStepData(GlobalFitData* gfData, double beta, double tTrack)
{
    gfData->beta = beta;
    gfData->chi2 = 0.;
    gfData->hitsOnTrk = 0.;
    gfData->outliersOnTrk = 0.;
    gfData->tTrack = tTrack;
}

void MuGirlNS::StauGF::clearStepData(GlobalFitData* gfData)
{
    if (gfData != NULL) delete gfData;
}

void MuGirlNS::StauGF::printStepData(GlobalFitData* gfData)
{
    if (gfData == NULL)
    {
        LOG_VERBOSE << "gf data is NULL" << endreq;
        return;
    }
    LOG_VERBOSE << "gf data: beta=" << gfData->beta
                << " chi2=" << gfData->chi2
                << " dof="<< gfData->hitsOnTrk
                << endreq;
}

void MuGirlNS::StauGF::processGlobalFitWithBeta(double currentBeta, GlobalFitData* gfData)
{
    LOG_VERBOSE << "beta=" << currentBeta
                << " gfData=" << gfData
                << endreq;

    RIO_OnTrackLists* pTriggerHitsInSegments = new RIO_OnTrackLists();
    std::vector<const Muon::MdtDriftCircleOnTrack*>* pMdtHits = new std::vector<
            const Muon::MdtDriftCircleOnTrack*>;

    /*    prepareForGlobalFit(pTriggerHitsInSegments, pMdtHits);
     const Trk::Track* pTrack = m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), pTriggerHitsInSegments, pMdtHits, true);*/

    MuGirlNS::MuonSegmentList muonSegments;
    prepareForGlobalFit(muonSegments);

    const Trk::Track* pTrack = m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), muonSegments,
            false);

    if (pTrack != NULL)
    {
        gfData->chi2 = pTrack->fitQuality()->chiSquared();
        gfData->dof = pTrack->fitQuality()->numberDoF();
        gfData->hitsOnTrk = (double) pTrack->measurementsOnTrack()->size();
        gfData->outliersOnTrk = (double) pTrack->outliersOnTrack()->size();

        double px = (double) pTrack->perigeeParameters()->momentum()[Trk::px];
        double py = (double) pTrack->perigeeParameters()->momentum()[Trk::py];
        double pz = (double) pTrack->perigeeParameters()->momentum()[Trk::pz];
        double p = sqrt(px * px + py * py + pz * pz);
        gfData->p = p;
    }
    else
    {
        gfData->chi2 = 1000.;
        gfData->dof = 1.;
        gfData->hitsOnTrk = 0.;
        gfData->outliersOnTrk = 0.;
    }

    pTriggerHitsInSegments->clear();
    delete pTriggerHitsInSegments;
    pMdtHits->clear();
    delete pMdtHits;

    //if(NULL!=pTrack) delete pTrack;
    delete pTrack;
    // {//delete the previous refitted track and store the new one.
    //   dbg
//	m_pStau->setRefittedTrack(pTrack);
//	dbg
    //  }

    printStepData(gfData);
    LOG_VERBOSE << "done" << endreq;
}

void MuGirlNS::StauGF::prepareForGlobalFit(RIO_OnTrackLists* pTriggerHitsInSegments,
        std::vector<const Muon::MdtDriftCircleOnTrack*>* pMdtHits)
{
    LOG_VERBOSE << "entered" << endreq;
    if (m_pStau->rpcHitsInSegments() != NULL)
        std::copy(m_pStau->rpcHitsInSegments()->begin(), m_pStau->rpcHitsInSegments()->end(),
                std::back_inserter(*pTriggerHitsInSegments));

    if (m_pStau->tgcHitsInSegments() != NULL)
        std::copy(m_pStau->tgcHitsInSegments()->begin(), m_pStau->tgcHitsInSegments()->end(),
                std::back_inserter(*pTriggerHitsInSegments));

    if (m_pStau->newMdtSegments() != NULL)
    {
        for (auto pSegment : *(m_pStau->newMdtSegments()))
        {
            for (unsigned int i = 0; i < pSegment->numberOfContainedROTs(); i++)
            {
                auto pRio = pSegment->rioOnTrack(i);
                auto pMdtRio = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(pRio);
                pMdtHits->push_back(pMdtRio);
            }
        }
    }
    LOG_VERBOSE << "done" << endreq;
}

void MuGirlNS::StauGF::prepareForGlobalFit(MuGirlNS::MuonSegmentList& muonSegments)
{
    muonSegments.clear();

    for (auto pSeg : m_muonSegments)
    {
        if (m_pStau->getSegmentManager()->isNotMDT(pSeg)) muonSegments.push_back(pSeg);
    }

    std::copy(m_pStau->newMdtSegments()->begin(), m_pStau->newMdtSegments()->end(),
            std::back_inserter(muonSegments));
}

void MuGirlNS::StauGF::clear()
{
    delete m_initialTrackParameters;
}

const Trk::Track* MuGirlNS::StauGF::refitTrack()
{
    MuGirlNS::MuonSegmentList muonSegments;
    prepareForGlobalFit(muonSegments);

    if (m_initialTrackParameters != NULL)
    {
        if (m_initialTrackParameters->parameters()[Trk::d0] == 0
                || m_initialTrackParameters->parameters()[Trk::z0] == 0
                || fabs(m_initialTrackParameters->parameters()[Trk::d0]) > 200.
                || fabs(m_initialTrackParameters->parameters()[Trk::z0]) > 500.)
            return NULL;
        else
            return m_pStau->globalFitTool()->globalFit(muonSegments, m_initialTrackParameters,
                    m_pStau->doStauNtuple());
    }
    else
        return m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), muonSegments,
                m_pStau->doStauNtuple());
}

double MuGirlNS::StauGF::stauMomentum()
{
    double p = 0.;
    const Trk::Track* pTrack = m_pStau->refittedTrack();

    if (pTrack != NULL)
    {
        double px = (double) pTrack->perigeeParameters()->momentum()[Trk::px];
        double py = (double) pTrack->perigeeParameters()->momentum()[Trk::py];
        double pz = (double) pTrack->perigeeParameters()->momentum()[Trk::pz];
        p = sqrt(px * px + py * py + pz * pz);
    }
    return p;
}

void MuGirlNS::StauGF::processGlobalFitWithTTrack(double /* tTrack */, GlobalFitData* gfData,
        double /*beta*/)
{
    LOG_VERBOSE << "entered" << endreq;
    RIO_OnTrackLists* pTriggerHitsInSegments = new RIO_OnTrackLists();
    std::vector<const Muon::MdtDriftCircleOnTrack*>* pMdtHits = new std::vector<
            const Muon::MdtDriftCircleOnTrack*>;

    /*    prepareForGlobalFit(pTriggerHitsInSegments, pMdtHits);
     const Trk::Track* pTrack = m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), pTriggerHitsInSegments, pMdtHits, true);*/

    MuGirlNS::MuonSegmentList muonSegments;
    prepareForGlobalFit(muonSegments);
    std::cout << "E123: " << __FILE__ << " " << __LINE__ << " muonSegments.size() = "
            << muonSegments.size() << std::endl;

    const Trk::Track* pTrack = m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), muonSegments,
            false);
    std::cout << "E123: " << __FILE__ << " " << __LINE__ << std::endl;
    if (pTrack != NULL)
    {
        gfData->chi2 = pTrack->fitQuality()->chiSquared();
        gfData->dof = pTrack->fitQuality()->numberDoF();
        gfData->hitsOnTrk = (double) pTrack->measurementsOnTrack()->size();
        gfData->outliersOnTrk = (double) pTrack->outliersOnTrack()->size();

        double px = (double) pTrack->perigeeParameters()->momentum()[Trk::px];
        double py = (double) pTrack->perigeeParameters()->momentum()[Trk::py];
        double pz = (double) pTrack->perigeeParameters()->momentum()[Trk::pz];
        double p = sqrt(px * px + py * py + pz * pz);
        gfData->p = p;

        // Here print what you've got
        //std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << std::endl;
        const DataVector<const Trk::TrackStateOnSurface>* pTSoSs = pTrack->trackStateOnSurfaces();
        if (pTSoSs == NULL)
        {
            pTriggerHitsInSegments->clear();
            delete pTriggerHitsInSegments; pTriggerHitsInSegments=nullptr;
            pMdtHits->clear();
            delete pMdtHits; pMdtHits=nullptr;
            return;
            //std::cout << "E123: " << __FILE__ << " " << __LINE__ << std::endl;
        } // (false);
        for (auto pTSoS : *pTSoSs)
        {
            std::cout << "E123: " << __FILE__ << " " << __LINE__ << std::endl;
            //std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << " "  << *pTSoS <<  std::endl;
            const Trk::MeasurementBase* hit = pTSoS->measurementOnTrack();
            if (hit == NULL)
            {
                std::cout << "E123: " << __FILE__ << " " << __LINE__ << std::endl;
                continue;
            }
            const Muon::MdtDriftCircleOnTrack* mdcot =
                    dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(hit);
            if (mdcot == NULL)
            {
                std::cout << "E123: " << __FILE__ << " " << __LINE__ << " mdcot == NULL"
                        << std::endl;
                continue;
            }
            if (pTSoS->type(Trk::TrackStateOnSurface::Outlier))
            {
                std::cout << "E123: " << __FILE__ << " " << __LINE__ << " mdcot == NULL"
                        << std::endl;
                continue;
            }
            std::cout << "E123: " << __FILE__ << " " << __LINE__ << std::endl;
            std::cout << "E123: " << pTSoS->trackParameters()->parameters()[Trk::locR] << std::endl;
        }

    }
    else
    {
        gfData->chi2 = 1000.;
        gfData->dof = 1.;
        gfData->hitsOnTrk = 0.;
        gfData->outliersOnTrk = 0.;
    }

    pTriggerHitsInSegments->clear();
    delete pTriggerHitsInSegments;
    pMdtHits->clear();
    delete pMdtHits;

    //if(NULL!=pTrack) delete pTrack;
    delete pTrack;
    // {//delete the previous refitted track and store the new one.
    //   dbg
//	m_pStau->setRefittedTrack(pTrack);
//	dbg
    //  }

    printStepData(gfData);
    LOG_VERBOSE << "done" << endreq;
}

void MuGirlNS::StauGF::setInitialTrackParameters(const Trk::Perigee* trk0)
{
    m_initialTrackParameters = trk0;
}

