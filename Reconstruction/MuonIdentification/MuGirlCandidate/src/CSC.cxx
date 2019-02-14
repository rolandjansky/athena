/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlCandidate/CSC.h"
#include "MuGirlCandidate/Chamber.h"
#include "MuGirlCandidate/CandidateTool.h"
#include "MuGirlCandidate/Candidate.h"
#include "MuGirlCandidate/Intersection.h"
#include "MuGirlCandidate/Utilities.h"
#include "MuGirlCandidate/Segment.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkRoad/TrackRoad.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "CscClusterization/ICscClusterBuilder.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Muon
{
class MdtDriftCircleOnTrack;
}

namespace MuGirlNS
{
CSC::CSC(CandidateTool* pMuGirl, const std::string& sPrepDataCollection) :
        Technology(pMuGirl, sPrepDataCollection),
        m_pPrepDataContainer(NULL)
{
    m_eType = CSC_TECH;
    m_detId = ::CSC;
    m_pIdHelper = dynamic_cast<const CscIdHelper*>(pMuGirl->muonManager()->cscIdHelper());
    if(m_pIdHelper == 0)
      m_pMuGirl->msg(MSG::ERROR) << "IdHelper should be CscIdHelper, but it is NOT!" << endmsg;
}

const MuonGM::MuonReadoutElement* CSC::readoutElement(const Identifier& id) const
{
    return m_pMuGirl->muonManager()->getCscReadoutElement(id);
}


int CSC::stationID(const Identifier& id) const
{
    const CscIdHelper* cscHelper = dynamic_cast<const CscIdHelper*>(m_pIdHelper);
    return (cscHelper) ? cscHelper->stationName(id) : -1;
}

int CSC::stationNameID(const std::string& name) const
{
    const CscIdHelper* cscHelper = dynamic_cast<const CscIdHelper*>(m_pIdHelper);
    return (cscHelper) ? cscHelper->stationNameIndex(name) : -1;
}

StatusCode CSC::retrievePrepData()
{
    if (m_pMuGirl->evtStore()->contains<Muon::CscPrepDataContainer>(m_sPrepDataCollection))
    {
        StatusCode sc = m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainer, m_sPrepDataCollection);
        if (sc.isFailure() || m_pPrepDataContainer == NULL)
        {
            m_pPrepDataContainer=NULL;
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg(MSG::DEBUG) << "Cannot retrieve CSC PrepData Container " << m_sPrepDataCollection << endmsg;
        }
        return StatusCode::SUCCESS;
    }
    else
    {
        m_pPrepDataContainer=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "EventStore does not contain CSC PrepData Container " << m_sPrepDataCollection << endmsg;

    }
    return StatusCode::SUCCESS;
}

unsigned CSC::prepData(Chamber* pChamber, PrepDataList& array)
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg(MSG::DEBUG) << "CSC::prepData():" <<
        " m_pPrepDataContainer!=NULL = " << (m_pPrepDataContainer != NULL ? "True" : "False") << endmsg;

    array.clear();

    if (m_pPrepDataContainer == NULL && m_pMuGirl->doDecoding())
    {
        std::vector<IdentifierHash> inhash, outhash;
        inhash.push_back(pChamber->hashId());
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "CSC::prepData() decoding hashId=" << pChamber->hashId() << endmsg;
        // If conversion failed, then there are clearly no hits, so return 0.
        if (m_pMuGirl->cscRdoToPrepDataTool().empty())
            return 0;
        if (m_pMuGirl->cscRdoToPrepDataTool()->decode(inhash, outhash).isFailure())
        {
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg(MSG::DEBUG) << "cscRdoToPrepDataTool()->decode() failed!" << endmsg;
            return 0;
        }
        if (m_pMuGirl->cscClusterProviderTool()->getClusters(inhash, outhash).isFailure())
        {
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg(MSG::DEBUG) << "cscRdoToPrepDataTool()->getClusters() failed!" << endmsg;
            return 0;
        }
        // If conversion succeeds, then we must be able to get the container, so try it now.
        if (m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainer, m_sPrepDataCollection).isFailure() ||
                m_pPrepDataContainer == NULL)
        {
            m_pMuGirl->msg(MSG::WARNING) << "Cannot retrieve CSC PrepData Container " << m_sPrepDataCollection << endmsg;
            return 0;
        }
    }
    if (m_pPrepDataContainer != NULL)
    {
        Muon::CscPrepDataContainer::const_iterator itColl = m_pPrepDataContainer->indexFind(pChamber->hashId());
        // If the container does not have a collection for this chamber, try the RdoToPrepData tool
        if (itColl == m_pPrepDataContainer->end() && m_pMuGirl->doDecoding())
        {
            std::vector<IdentifierHash> inhash, outhash;
            inhash.push_back(pChamber->hashId());
            if (m_pMuGirl->cscRdoToPrepDataTool().empty())
                return 0;
            if (m_pMuGirl->cscRdoToPrepDataTool()->decode(inhash, outhash).isSuccess() && !outhash.empty())
                itColl = m_pPrepDataContainer->indexFind(outhash.front());
            if (m_pMuGirl->cscClusterProviderTool()->getClusters(inhash, outhash).isSuccess() && !outhash.empty())
                itColl = m_pPrepDataContainer->indexFind(outhash.front());
        }
        if (itColl != m_pPrepDataContainer->end())
        {
            //DataLink<Muon::RpcPrepDataCollection> pColl = *itColl;
            const Muon::CscPrepDataCollection* pColl = *itColl;
            array.insert(array.begin(), pColl->begin(), pColl->end());
        }
    }
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg(MSG::DEBUG) << "CSC::prepData() returning with " << array.size() << " hits" << endmsg;
 
    return array.size();
}

std::vector<IdentifierHash> CSC::retrieveAvailableCollections() const {
    // dummy function needed only for MM and sTGC
    std::vector<IdentifierHash> Ids;
    return Ids;
}

Amg::Vector3D CSC::hitPosition(const Trk::PrepRawData* pPrepData)
{
    const Muon::CscPrepData* pCscPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepData);
    if (pCscPrepData == NULL)
    {
        m_pMuGirl->msg(MSG::WARNING) << "Cannot convert from Trk::PrepRawData* to Muon::CscPrepData*" << endmsg;
        return Amg::Vector3D();
    }
    return pCscPrepData->globalPosition();
}

bool CSC::isEtaHit(const Trk::PrepRawData*)
{
    return false;
}

void CSC::buildSegments(Candidate* pCand, ChamberList& chambers, double)
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "CSC::buildSegments - " << chambers.size() << " chambers" << endmsg;

    if (chambers.empty())
        return;
    Chamber* pGoodChamber = NULL;
    std::vector<std::vector<const Muon::MdtDriftCircleOnTrack*> > mdts;
    std::vector<std::vector<const Muon::MuonClusterOnTrack*> > clusters(chambers.size());
    unsigned nChamber = 0;
    for (ChamberList::const_iterator itCh = chambers.begin(); itCh != chambers.end(); itCh++)
    {
        Chamber* pChamber = *itCh;
        const RIOList& rios = pChamber->RIOs();
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG)<< rios.size()<< " CSC hit in chamber " << pChamber << endmsg;
        //if (!rios.empty())
        if (rios.size()>3)
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
                    m_pMuGirl->msg(MSG::WARNING) << "Cannot convert Trk::RIO_OnTrack to Muon::MuonClusterOnTrack" << endmsg;
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg() << "CSC::buildSegments ended" << endmsg;
                    return;
                }
                mcots.push_back(pMcot);
            }
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg(MSG::DEBUG)<< mcots.size()<< " CSC hit in chamber " << pChamber << endmsg;
            break;
        }
    }
    if (nChamber == 0)
    {
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "No CSC hits" << endmsg;
        return;
    }
    clusters.resize(nChamber);

    Trk::TrackRoad* pRoad = chambers[0]->baseRoad();
    if (pRoad == NULL)
    {
        m_pMuGirl->msg(MSG::DEBUG)<< "Cannot find base road" << endmsg;
        return;
    }
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        pRoad->dump(m_pMuGirl->msg());
    std::vector<const Muon::MuonSegment*>* pSegments = NULL;
    m_pMuGirl->cscSegmentMaker()->find(*pRoad, mdts, clusters, 0);
    if (pSegments != NULL)
    {
        for (std::vector<const Muon::MuonSegment*>::const_iterator itSeg = pSegments->begin(); itSeg != pSegments->end(); itSeg++)
        {
            const Muon::MuonSegment* pMuonSegment = *itSeg;

            Segment* pSegment = pCand->addSegment(MuGirlNS::LINEAR_SEGMENT,
                                                  pMuonSegment,
                                                  CSC_TECH,
                                                  chambers[0]->distanceType(),
                                                  chambers[0]->regionType(),
                                                  chambers[0]->station());
            pCand->markHits(chambers, pSegment);

            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "Adding CSC segment at " << pMuonSegment->globalPosition()
                << " hits=" << pMuonSegment->numberOfContainedROTs()
                << " chi2=" << pMuonSegment->fitQuality()->chiSquared()
                << " prob=" << pSegment->fitProbability()
                << endmsg;

            Trk::TrackSurfaceIntersection* pTrkIsect =
                new Trk::TrackSurfaceIntersection(pMuonSegment->globalPosition(),
                                                  pMuonSegment->globalDirection().unit(),
                                                  0.0);
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "Adding CSC intersection at " << pTrkIsect << endmsg;
            Intersection* pIsect = pCand->addIntersection(FIT_INTERSECTION,
                                   pTrkIsect,
                                   CSC_TECH,
                                   chambers[0]->distanceType(),
                                   CSC_TECH,
                                   chambers[0]->distanceType(),
                                   ENDCAP_REGION,
                                   NULL);
            pCand->setCellIntersection(CSC_TECH, chambers[0]->distanceType(), ENDCAP_REGION, pIsect);
            pSegment->setIntersection(pIsect);
        }
        delete pSegments;
    }
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "CSC::buildSegments ended" << endmsg;
}
}
