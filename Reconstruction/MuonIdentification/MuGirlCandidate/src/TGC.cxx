/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkRoad/TrackRoad.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/FitQuality.h"

#include "MuGirlCandidate/TGC.h"
#include "MuGirlCandidate/Chamber.h"
#include "MuGirlCandidate/CandidateTool.h"
#include "MuGirlCandidate/Candidate.h"
#include "MuGirlCandidate/Intersection.h"
#include "MuGirlCandidate/Utilities.h"
#include "MuGirlCandidate/Segment.h"

namespace MuGirlNS
{
TGC::TGC(CandidateTool* pMuGirl, const std::string& sPrepDataCollection) :
        Technology(pMuGirl, sPrepDataCollection),
        m_pPrepDataContainer(NULL),
        m_pPrepDataContainerN(NULL),
        m_pPrepDataContainerP(NULL)
{
    m_eType = TGC_TECH;
    m_detId = ::TGC;
    m_pIdHelper = pMuGirl->muonManager()->tgcIdHelper();
}

const MuonGM::MuonReadoutElement* TGC::readoutElement(const Identifier& id) const
{
    return m_pMuGirl->muonManager()->getTgcReadoutElement(id);
}

int TGC::stationID(const Identifier& id) const
{
    return dynamic_cast<const TgcIdHelper*>(m_pIdHelper)->stationName(id);
}

int TGC::stationNameID(const std::string& name) const
{
    return dynamic_cast<const TgcIdHelper*>(m_pIdHelper)->stationNameIndex(name);
}

StatusCode TGC::retrievePrepData()
{

    if (m_pMuGirl->evtStore()->contains<Muon::TgcPrepDataContainer>(m_sPrepDataCollection))
    {
        StatusCode sc = m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainer, m_sPrepDataCollection);
        if (sc.isFailure() || m_pPrepDataContainer == NULL)
        {
        m_pPrepDataContainer=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "Cannot retrieve TGC PrepData Container " << m_sPrepDataCollection << endreq;
         }
     }
    else
    {
        m_pPrepDataContainer=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "EventStore does not contain TGC PrepData Container " << m_sPrepDataCollection << endreq;
    }

    std::string m_sPrepDataPrior = m_sPrepDataCollection+"PriorBC";
    std::string m_sPrepDataNext = m_sPrepDataCollection+"NextBC";

    if (m_pMuGirl->evtStore()->contains<Muon::TgcPrepDataContainer>(m_sPrepDataPrior))
    {
        StatusCode sc = m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainerP, m_sPrepDataPrior);
        if (sc.isFailure() || m_pPrepDataContainerP == NULL)
        {
        m_pPrepDataContainerP=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "Cannot retrieve TGC PrepData Container " << m_sPrepDataPrior << endreq;
        }
     }
    else
    {
        m_pPrepDataContainerP=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "EventStore does not contain TGC PrepData Container " << m_sPrepDataPrior << endreq;
    }

     if (m_pMuGirl->evtStore()->contains<Muon::TgcPrepDataContainer>(m_sPrepDataNext))
     {
        StatusCode sc = m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainerN, m_sPrepDataNext);
        if (sc.isFailure() || m_pPrepDataContainerN == NULL)
        {
        m_pPrepDataContainerN=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "Cannot retrieve TGC PrepData Container " << m_sPrepDataNext << endreq;
        }
     }
    else
    {
        m_pPrepDataContainerN=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "EventStore does not contain TGC PrepData Container " << m_sPrepDataNext << endreq;
    }
     return StatusCode::SUCCESS;

}

bool TGC::addPrd(const Muon::TgcPrepData* prd, PrepDataList& array)
{
    Identifier id = prd->identify();
    if (m_detElements.find(id) == m_detElements.end())
    {
        m_detElements.insert(id);
        array.push_back(prd);
        return(true);
    }
    return(false);
}

unsigned TGC::prepData(Chamber* pChamber, PrepDataList& array)
{
    array.clear();
    m_detElements.clear();

 // If the container was not retrieved earlier, try the RdoToPrepData tool
    if (m_pPrepDataContainer == NULL)
    {
        std::vector<IdentifierHash> inhash, outhash;
        inhash.push_back(pChamber->hashId());
        // If conversion failed, then there are clearly no hits, so return 0.
        if (m_pMuGirl->tgcRdoToPrepDataTool().empty())
            return 0;
        if (m_pMuGirl->tgcRdoToPrepDataTool()->decode(inhash, outhash).isFailure())
            return 0;
        // If conversion succeeds, then we must be able to get the container, so try it now.
        if (m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainer, m_sPrepDataCollection).isFailure() ||
            m_pPrepDataContainer == NULL)
        {
            m_pMuGirl->msg(MSG::WARNING) << "Cannot retrieve TGC PrepData Container " << m_sPrepDataCollection << endreq;
            return 0;
        }
    }

    if (m_pPrepDataContainer != NULL)
    {
        int nHits = 0;
        Muon::TgcPrepDataContainer::const_iterator itColl = m_pPrepDataContainer->indexFind(pChamber->hashId());
        if (itColl == m_pPrepDataContainer->end())
        {
            std::vector<IdentifierHash> inhash, outhash;
            inhash.push_back(pChamber->hashId());
            if (m_pMuGirl->tgcRdoToPrepDataTool().empty())
                return 0;
            if (m_pMuGirl->tgcRdoToPrepDataTool()->decode(inhash, outhash).isSuccess() && !outhash.empty())
                itColl = m_pPrepDataContainer->indexFind(outhash.front());
        }
        if (itColl != m_pPrepDataContainer->end())
        {
            //DataLink<Muon::TgcPrepDataCollection> pColl = *itColl;
            const Muon::TgcPrepDataCollection* pColl = *itColl;
            for (Muon::TgcPrepDataCollection::const_iterator itPRD = pColl->begin(); itPRD != pColl->end(); itPRD++)
            {
                if (addPrd(*itPRD, array))
                    nHits++;
            }
            MsgStream log(m_pMuGirl->msgSvc(), m_pMuGirl->name());
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "size of TGC PrepData collection from BC = " << nHits << " out of " << pColl->size() << endreq;

        }
    }
    std::string m_sPrepDataPrior = m_sPrepDataCollection+"PriorBC";
    if (m_pPrepDataContainerP == NULL)
    {
        std::vector<IdentifierHash> inhash, outhash;
        inhash.push_back(pChamber->hashId());
        // If conversion failed, then there are clearly no hits, so return 0.
        if (!m_pMuGirl->tgcRdoToPrepDataTool().empty())
            if (m_pMuGirl->tgcRdoToPrepDataTool()->decode(inhash, outhash).isFailure())
                m_pMuGirl->msg(MSG::WARNING) << "Cannot retrieve TGC PrepData Container " << m_sPrepDataPrior << endreq;
        // If conversion succeeds, then we must be able to get the container, so try it now.
        if (m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainerP, m_sPrepDataPrior).isFailure() ||
            m_pPrepDataContainerP == NULL)
            m_pMuGirl->msg(MSG::WARNING) << "Cannot retrieve TGC PrepData Container " << m_sPrepDataPrior << endreq;
    }
    if (m_pPrepDataContainerP != NULL)
    {
        int nHits = 0;
        Muon::TgcPrepDataContainer::const_iterator itColl = m_pPrepDataContainerP->indexFind(pChamber->hashId());
        if (itColl == m_pPrepDataContainerP->end())
        {
            std::vector<IdentifierHash> inhash, outhash;
            inhash.push_back(pChamber->hashId());
            if (!m_pMuGirl->tgcRdoToPrepDataTool().empty())
                if (m_pMuGirl->tgcRdoToPrepDataTool()->decode(inhash, outhash).isSuccess() && !outhash.empty())
                    itColl = m_pPrepDataContainerP->indexFind(outhash.front());
        }
        if (itColl != m_pPrepDataContainerP->end())
        {
            //DataLink<Muon::TgcPrepDataCollection> pColl = *itColl;
            const Muon::TgcPrepDataCollection* pColl = *itColl;
            for (Muon::TgcPrepDataCollection::const_iterator itPRD = pColl->begin(); itPRD != pColl->end(); itPRD++)
            {
                if (addPrd(*itPRD, array))
                    nHits++;
            }
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "size of TGC PrepData collection from BC-1 =" << nHits << " out of " << pColl->size() << endreq;
        }
    }
    std::string m_sPrepDataNext = m_sPrepDataCollection+"NextBC";
    if (m_pPrepDataContainerN == NULL)
    {
        std::vector<IdentifierHash> inhash, outhash;
        inhash.push_back(pChamber->hashId());
        // If conversion failed, then there are clearly no hits, so return 0.
        if (!m_pMuGirl->tgcRdoToPrepDataTool().empty())
            if (m_pMuGirl->tgcRdoToPrepDataTool()->decode(inhash, outhash).isFailure())
                m_pMuGirl->msg(MSG::WARNING) << "Cannot retrieve TGC PrepData Container " << m_sPrepDataNext << endreq;
        // If conversion succeeds, then we must be able to get the container, so try it now.
        if (m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainerN, m_sPrepDataNext).isFailure() ||
            m_pPrepDataContainerN == NULL)
            m_pMuGirl->msg(MSG::WARNING) << "Cannot retrieve TGC PrepData Container " << m_sPrepDataNext << endreq;
    }
    if (m_pPrepDataContainerN != NULL)
    {
        int nHits = 0;
        Muon::TgcPrepDataContainer::const_iterator itColl = m_pPrepDataContainerN->indexFind(pChamber->hashId());
        if (itColl == m_pPrepDataContainerN->end())
        {
            std::vector<IdentifierHash> inhash, outhash;
            inhash.push_back(pChamber->hashId());
                if (!m_pMuGirl->tgcRdoToPrepDataTool().empty())
                    if (m_pMuGirl->tgcRdoToPrepDataTool()->decode(inhash, outhash).isSuccess() && !outhash.empty())
                        itColl = m_pPrepDataContainerN->indexFind(outhash.front());
        }
        if (itColl != m_pPrepDataContainerN->end())
        {
            //DataLink<Muon::TgcPrepDataCollection> pColl = *itColl;
            const Muon::TgcPrepDataCollection* pColl = *itColl;
            for (Muon::TgcPrepDataCollection::const_iterator itPRD = pColl->begin(); itPRD != pColl->end(); itPRD++)
            {
                if (addPrd(*itPRD, array))
                    nHits++;
            }
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "size of TGC PrepData collection from BC+1 =" << nHits << " out of " << pColl->size() << endreq;
        }
    }
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "size of all TGC PrepData =" << array.size() << endreq;
    return array.size();
}

std::vector<IdentifierHash> TGC::retrieveAvailableCollections() const {
    // dummy function needed only for MM and sTGC
    std::vector<IdentifierHash> Ids;
    return Ids;
}

Amg::Vector3D TGC::hitPosition(const Trk::PrepRawData* pPrepData)
{
    const Muon::TgcPrepData* pTgcPrepData = dynamic_cast<const Muon::TgcPrepData*>(pPrepData);
    if (pTgcPrepData == NULL)
    {
        m_pMuGirl->msg(MSG::WARNING) << "Cannt convert from Trk::PrepRawData* to Muon::TgcPrepData*" << endreq;
        return Amg::Vector3D();
    }
    return pTgcPrepData->globalPosition();
}

bool TGC::isEtaHit(const Trk::PrepRawData* pPrepData)
{
    return !dynamic_cast<const TgcIdHelper*>(m_pIdHelper)->isStrip(pPrepData->identify());
}

void TGC::buildSegments(Candidate* pCand, ChamberList& chambers, double)
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "TGC::buildSegments - " << chambers.size() << " chambers" << endreq;

    if (chambers.empty())
        return;

    Chamber* pGoodChamber = NULL;
    std::vector<std::vector<const Muon::MdtDriftCircleOnTrack*> > mdts;
    Clusters& clusters = pCand->getClusters(TGC_TECH,
                                            chambers.front()->distanceType(),
                                            chambers.front()->regionType());
    clusters.clear(); clusters.resize(chambers.size());
    unsigned nChamber = 0;
    for (ChamberList::const_iterator itCh = chambers.begin(); itCh != chambers.end(); itCh++)
    {
        Chamber* pChamber = *itCh;
        const RIOList& rios = pChamber->RIOs();
        if (!rios.empty())
        {
            if (pGoodChamber == NULL)
                pGoodChamber = pChamber;
            std::vector<const Muon::MuonClusterOnTrack*>& mcots = clusters[nChamber++];
            for (RIOList::const_iterator itRIO = rios.begin(); itRIO != rios.end(); itRIO++)
            {
                const Muon::MuonClusterOnTrack* pMcot =
                    dynamic_cast<const Muon::MuonClusterOnTrack*>(*itRIO);
                if (pMcot == NULL)
                {
                    m_pMuGirl->msg(MSG::WARNING) << "Cannot convert Trk::RIO_OnTrack to Muon::MuonClusterOnTrack" << endreq;
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg() << "TGC::buildSegments ended" << endreq;
                    return;
                }
                mcots.push_back(pMcot);
            }
        }
    }
    if (nChamber == 0)
    {
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "No TGC hits\nTGC::buildSegments ended" << endreq;
        return;
    }
    clusters.resize(nChamber);
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "clusters(" << TechnologyTypeName(TGC_TECH) << ","
            << DistanceTypeName(chambers.front()->distanceType()) << ","
            << RegionTypeName(chambers.front()->regionType()) << ")"
            << " contains " << count_clusters(clusters) << " clusters" << endreq;
    Trk::TrackRoad* pRoad = pGoodChamber->baseRoad();
    if (pRoad == NULL)
    {
        m_pMuGirl->msg(MSG::DEBUG) << "Cannot find base road" << endreq;
        return;
    }
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        pRoad->dump(m_pMuGirl->msg());
    std::vector<const Muon::MuonSegment*>* pSegments =
        m_pMuGirl->tgcSegmentMaker()->find(*pRoad, mdts, clusters);
    if (pSegments != NULL)
    {
        for (std::vector<const Muon::MuonSegment*>::const_iterator itSeg = pSegments->begin();
                itSeg != pSegments->end();
                itSeg++)
        {
            const Muon::MuonSegment* pMuonSegment = *itSeg;

            Segment* pSegment = pCand->addSegment(MuGirlNS::LINEAR_SEGMENT,
                                                  pMuonSegment,
                                                  TGC_TECH,
                                                  pGoodChamber->distanceType(),
                                                  pGoodChamber->regionType(),
                                                  pGoodChamber->station());
            pCand->markHits(chambers, pSegment);

            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "Adding TGC segment at " << pMuonSegment->globalPosition()
                << " hits=" << pMuonSegment->numberOfContainedROTs()
                << " chi2=" << pMuonSegment->fitQuality()->chiSquared()
                << " prob=" << pSegment->fitProbability()
                << endreq;

            Trk::TrackSurfaceIntersection* pTrkIsect =
                new Trk::TrackSurfaceIntersection(pMuonSegment->globalPosition(),
                                                  pMuonSegment->globalDirection().unit(),
                                                  0.0);
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "Adding TGC intersection at " << pTrkIsect << endreq;
            Intersection* pIsect = pCand->addIntersection(FIT_INTERSECTION,
                                   pTrkIsect,
                                   TGC_TECH,
                                   pGoodChamber->distanceType(),
                                   TGC_TECH,
                                   pGoodChamber->distanceType(),
                                   ENDCAP_REGION,
                                   NULL);
            pCand->setCellIntersection(TGC_TECH, pGoodChamber->distanceType(), ENDCAP_REGION, pIsect);
            pSegment->setIntersection(pIsect);
        }
        delete pSegments;
    }
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "TGC::buildSegments ended" << endreq;
}

}
