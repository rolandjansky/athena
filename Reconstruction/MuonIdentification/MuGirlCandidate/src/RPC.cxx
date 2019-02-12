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

#include "MuGirlCandidate/RPC.h"
#include "MuGirlCandidate/Chamber.h"
#include "MuGirlCandidate/CandidateTool.h"
#include "MuGirlCandidate/Candidate.h"
#include "MuGirlCandidate/Intersection.h"
#include "MuGirlCandidate/Utilities.h"
#include "MuGirlCandidate/Segment.h"

//<S>===========================================
// stauCandidate dependencies
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
//end of stauCandidate dependencies
//<S>===========================================

namespace MuGirlNS
{
RPC::RPC(CandidateTool* pMuGirl, const std::string& sPrepDataCollection) :
        Technology(pMuGirl, sPrepDataCollection),
        m_pPrepDataContainer(NULL)
{
    m_eType = RPC_TECH;
    m_detId = ::RPC;
    m_pIdHelper = dynamic_cast<const RpcIdHelper*>(pMuGirl->muonManager()->rpcIdHelper());
    if(m_pIdHelper == 0)
      m_pMuGirl->msg(MSG::ERROR) << "IdHelper should be RpcIdHelper, but it is NOT!" << endmsg;
}

const MuonGM::MuonReadoutElement* RPC::readoutElement(const Identifier& id) const
{
    return m_pMuGirl->muonManager()->getRpcReadoutElement(id);
}

int RPC::stationID(const Identifier& id) const
{
    const RpcIdHelper* rpcHelper = dynamic_cast<const RpcIdHelper*>(m_pIdHelper);
    return (rpcHelper) ? rpcHelper->stationName(id) : -1;
}

int RPC::stationNameID(const std::string& name) const
{
    const RpcIdHelper* rpcHelper = dynamic_cast<const RpcIdHelper*>(m_pIdHelper);
    return (rpcHelper) ? rpcHelper->stationNameIndex(name) : -1;
}

StatusCode RPC::retrievePrepData()
{
    // Only try to get the PrepData container if it is there. Otherwise, leave the pointer NULL.
    if (m_pMuGirl->evtStore()->contains<Muon::RpcPrepDataContainer>(m_sPrepDataCollection))
    {
       StatusCode sc = m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainer, m_sPrepDataCollection);
       if (sc.isFailure() || m_pPrepDataContainer == NULL)
       {
           m_pPrepDataContainer=NULL;
           if (m_pMuGirl->msgLvl(MSG::DEBUG))
              m_pMuGirl->msg(MSG::DEBUG) << "Cannot retrieve RPC PrepData Container " << m_sPrepDataCollection << endmsg;
       }
       return StatusCode::SUCCESS;
    }
    else
    {
        m_pPrepDataContainer=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "EventStore does not contain RPC PrepData Container " << m_sPrepDataCollection << endmsg;
    }
    return StatusCode::SUCCESS;
}

unsigned RPC::prepData(Chamber* pChamber, PrepDataList& array)
{
    array.clear();

    if (m_pPrepDataContainer == NULL && m_pMuGirl->doDecoding())
    {
        std::vector<IdentifierHash> inhash, outhash;
        inhash.push_back(pChamber->hashId());
        // If conversion failed, then there are clearly no hits, so return 0.
        if (m_pMuGirl->rpcRdoToPrepDataTool().empty())
            return 0;
        if (m_pMuGirl->rpcRdoToPrepDataTool()->decode(inhash, outhash).isFailure())
            return 0;
        // If conversion succeeds, then we must be able to get the container, so try it now.
        if (m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainer, m_sPrepDataCollection).isFailure() ||
            m_pPrepDataContainer == NULL)
        {
            m_pMuGirl->msg(MSG::WARNING) << "Cannot retrieve RPC PrepData Container " << m_sPrepDataCollection << endmsg;
            return 0;
        }
    }
    if (m_pPrepDataContainer != NULL)
    {
        Muon::RpcPrepDataContainer::const_iterator itColl = m_pPrepDataContainer->indexFind(pChamber->hashId());
        // If the container does not have a collection for this chamber, try the RdoToPrepData tool
        if (itColl == m_pPrepDataContainer->end() && m_pMuGirl->doDecoding())
        {
            std::vector<IdentifierHash> inhash, outhash;
            inhash.push_back(pChamber->hashId());
            if (m_pMuGirl->rpcRdoToPrepDataTool().empty())
                return 0;
            if (m_pMuGirl->rpcRdoToPrepDataTool()->decode(inhash, outhash).isSuccess() && !outhash.empty())
                itColl = m_pPrepDataContainer->indexFind(outhash.front());
        }
        if (itColl != m_pPrepDataContainer->end())
        {
            //DataLink<Muon::RpcPrepDataCollection> pColl = *itColl;
            const Muon::RpcPrepDataCollection* pColl = *itColl;
            array.insert(array.begin(), pColl->begin(), pColl->end());
        }
    }

    return array.size();
}

std::vector<IdentifierHash> RPC::retrieveAvailableCollections() const {
    // dummy function needed only for MM and sTGC
    std::vector<IdentifierHash> Ids;
    return Ids;
}

Amg::Vector3D RPC::hitPosition(const Trk::PrepRawData* pPrepData)
{
    const Muon::RpcPrepData* pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepData);
    if (pRpcPrepData == NULL)
    {
        m_pMuGirl->msg(MSG::WARNING) << "Cannt convert from Trk::PrepRawData* to Muon::RpcPrepData*" << endmsg;
        return Amg::Vector3D();
    }
    return pRpcPrepData->globalPosition();
}

bool RPC::isEtaHit(const Trk::PrepRawData* pPrepData)
{
    const RpcIdHelper* rpcHelper = dynamic_cast<const RpcIdHelper*>(m_pIdHelper);
    return (rpcHelper) ? (!(rpcHelper->measuresPhi(pPrepData->identify()))) : false;
}

void RPC::buildSegments(Candidate* pCand, ChamberList& chambers, double)
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "RPC::buildSegments - " << chambers.size() << " chambers" << endmsg;

    if (chambers.empty())
        return;

    Chamber* pGoodChamber = NULL;
    std::vector<std::vector<const Muon::MdtDriftCircleOnTrack*> > mdts;
    Clusters& clusters = pCand->getClusters(RPC_TECH,
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
                    m_pMuGirl->msg(MSG::WARNING) << "Cannot convert Trk::RIO_OnTrack to Muon::MuonClusterOnTrack" << endmsg;
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg() << "RPC::buildSegments ended" << endmsg;
                    return;
                }
                mcots.push_back(pMcot);
            }
        }
    }
    if (nChamber == 0)
    {
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "No RPC hits" << endmsg;
        return;
    }
    clusters.resize(nChamber);
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "clusters(" << TechnologyTypeName(RPC_TECH) << ","
            << DistanceTypeName(chambers.front()->distanceType()) << ","
            << RegionTypeName(chambers.front()->regionType()) << ")"
            << " contains " << count_clusters(clusters) << " clusters" << endmsg;
    Trk::TrackRoad* pRoad = pGoodChamber->baseRoad();
    if (pRoad == NULL)
    {
        m_pMuGirl->msg(MSG::DEBUG) << "Cannot find base road" << endmsg;
        return;
    }
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        pRoad->dump(m_pMuGirl->msg());
    std::vector<const Muon::MuonSegment*>* pSegments = NULL;
    m_pMuGirl->rpcSegmentMaker()->find(*pRoad, mdts, clusters, 0);
    if (pSegments != NULL)
    {
        for (std::vector<const Muon::MuonSegment*>::const_iterator itSeg = pSegments->begin(); itSeg != pSegments->end(); itSeg++)
        {
            const Muon::MuonSegment* pMuonSegment = *itSeg;

            Segment* pSegment = pCand->addSegment(MuGirlNS::LINEAR_SEGMENT,
                                                  pMuonSegment,
                                                  RPC_TECH,
                                                  pGoodChamber->distanceType(),
                                                  pGoodChamber->regionType(),
                                                  pGoodChamber->station());
            pCand->markHits(chambers, pSegment);

            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "Adding RPC segment at " << pMuonSegment->globalPosition()
                << " hits=" << pMuonSegment->numberOfContainedROTs()
                << " chi2=" << pMuonSegment->fitQuality()->chiSquared()
                << " prob=" << pSegment->fitProbability()
                << endmsg;

            // This is a workaround occasional inversion of direction by MuonSegemnt
            // ---------------------------------------------------------------------
            Amg::Vector3D dir = pMuonSegment->globalDirection();
            //if (dir * pRoad->direction() < 0.0)
            //    dir = -dir;
            // ---------------------------------------------------------------------
            Trk::TrackSurfaceIntersection* pTrkIsect =
                new Trk::TrackSurfaceIntersection(pMuonSegment->globalPosition(), dir, 0.0);
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "Adding RPC intersection at " << pTrkIsect << endmsg;
            Intersection* pIsect = pCand->addIntersection(FIT_INTERSECTION,
                                   pTrkIsect,
                                   RPC_TECH,
                                   pGoodChamber->distanceType(),
                                   RPC_TECH,
                                   pGoodChamber->distanceType(),
                                   BARREL_REGION,
                                   NULL);
            pCand->setCellIntersection(RPC_TECH, pGoodChamber->distanceType(), BARREL_REGION, pIsect);
            pSegment->setIntersection(pIsect);
        }
        delete pSegments;
    }
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "RPC::buildSegments ended" << endmsg;
}

//<S>==============================================================
// stauCandidate functions
void RPC::stauHitBeta(Hit* pHit, double& d_beta, bool& b_isEta)
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "RPC::stauHitBeta - " << endmsg;

    const Trk::RIO_OnTrack* pRIO = pHit->RIO();

    const Trk::PrepRawData* pPrepData = pRIO->prepRawData();
    const Muon::RpcPrepData* pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepData);
    if(!pRpcPrepData) {
      m_pMuGirl->msg(MSG::WARNING) << "PrepData should be RpcPrepData, but it is NOT!" << endmsg;
      m_pMuGirl->msg(MSG::WARNING) << "Returning beta=0. and isEta=false." << endmsg;
      d_beta = 0.;
      b_isEta = false;
      return;
    }

    Amg::Vector3D hitPos = pHit->position();
    float f_distance = fabs(hitPos.mag())/1000; //[m]
    float f_time = pRpcPrepData->time(); //[ns]
    b_isEta = this->isEtaHit(pPrepData);

    const MuonGM::RpcReadoutElement* pROElement = pRpcPrepData->detectorElement();
    double d_propagation = pROElement->distanceToEtaReadout(hitPos,pRpcPrepData->identify()) +
                           (b_isEta?pROElement->gasGapZsize():pROElement->gasGapSsize());//

    double d_propTime = (fabs(d_propagation)/1000)*RPCINVSIGVEL;
    d_beta = (double)f_distance/(f_time-d_propTime)/SPEEDOFLIGHT;

    //std::cout << "d_beta " << d_beta << ", b_isEta " << b_isEta << std::endl;
}
//end of stauCandidate functions
//<S>==============================================================

}
