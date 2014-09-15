/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkRoad/TrackRoad.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "MuGirlCandidate/MDT.h"
#include "MuGirlCandidate/Chamber.h"
#include "MuGirlCandidate/Station.h"
#include "MuGirlCandidate/Candidate.h"
#include "MuGirlCandidate/Segment.h"
#include "MuGirlCandidate/CandidateTool.h"
#include "MuGirlCandidate/Intersection.h"

namespace MuGirlNS
{
inline std::string status2str(Muon::MdtDriftCircleStatus status)
{
    return
        (status == Muon::MdtStatusMasked         ? "Masked" :
        (status == Muon::MdtStatusDriftTime      ? "DriftTime" :
        (status == Muon::MdtStatusBeforeSpectrum ? "BeforeSpectrum" :
        (status == Muon::MdtStatusAfterSpectrum  ? "AfterSpectrum" : "UnDefined"))));
}

MDT::MDT(CandidateTool* pMuGirl, const std::string& sPrepDataCollection) :
        Technology(pMuGirl, sPrepDataCollection),
        m_pPrepDataContainer(NULL)
{
    m_eType = MDT_TECH;
    m_detId = ::MDT;
    m_pIdHelper = pMuGirl->muonManager()->mdtIdHelper();
}

const MuonGM::MuonReadoutElement* MDT::readoutElement(const Identifier& id) const
{
    return m_pMuGirl->muonManager()->getMdtReadoutElement(id);
}


int MDT::stationID(const Identifier& id) const
{
    return dynamic_cast<const MdtIdHelper*>(m_pIdHelper)->stationName(id);
}

int MDT::stationNameID(const std::string& name) const
{
    return dynamic_cast<const MdtIdHelper*>(m_pIdHelper)->stationNameIndex(name);
}

StatusCode MDT::retrievePrepData()
{
    if (m_pMuGirl->evtStore()->contains<Muon::MdtPrepDataContainer>(m_sPrepDataCollection))
    {
       StatusCode sc = m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainer, m_sPrepDataCollection);
       if (sc.isFailure() || m_pPrepDataContainer == NULL)
       {
           m_pPrepDataContainer=NULL;
           if (m_pMuGirl->msgLvl(MSG::DEBUG))
               m_pMuGirl->msg(MSG::DEBUG) << "Cannot retrieve MDT PrepData Container " << m_sPrepDataCollection << endreq;
       }
       return StatusCode::SUCCESS;
    }
    else
    {
        m_pPrepDataContainer=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "EventStore does not contain MDT PrepData Container " << m_sPrepDataCollection << endreq;
    }
    return StatusCode::SUCCESS;
}

unsigned MDT::prepData(Chamber* pChamber, PrepDataList& array)
{
    array.clear();

    if (m_pPrepDataContainer == NULL)
    {
        std::vector<IdentifierHash> inhash, outhash;
        inhash.push_back(pChamber->hashId());
        // If conversion failed, then there are clearly no hits, so return 0.
        if (m_pMuGirl->mdtRdoToPrepDataTool().empty())
            return 0;
        if (m_pMuGirl->mdtRdoToPrepDataTool()->decode(inhash, outhash).isFailure())
            return 0;
        // If conversion succeeds, then we must be able to get the container, so try it now.
        if (m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainer, m_sPrepDataCollection).isFailure() ||
            m_pPrepDataContainer == NULL)
        {
            m_pMuGirl->msg(MSG::WARNING) << "Cannot retrieve MDT PrepData Container " << m_sPrepDataCollection << endreq;
            return 0;
        }
    }
    if (m_pPrepDataContainer != NULL)
    {
        Muon::MdtPrepDataContainer::const_iterator itColl = m_pPrepDataContainer->indexFind(pChamber->hashId());
        // If the container does not have a collection for this chamber, try the RdoToPrepData tool
        if (itColl == m_pPrepDataContainer->end())
        {
            std::vector<IdentifierHash> inhash, outhash;
            inhash.push_back(pChamber->hashId());
            if (m_pMuGirl->mdtRdoToPrepDataTool().empty())
               return 0;
            if (m_pMuGirl->mdtRdoToPrepDataTool()->decode(inhash, outhash).isSuccess() && !outhash.empty())
                itColl = m_pPrepDataContainer->indexFind(outhash.front());
        }
        if (itColl != m_pPrepDataContainer->end())
        {
            //DataLink<Muon::RpcPrepDataCollection> pColl = *itColl;
            const Muon::MdtPrepDataCollection* pColl = *itColl;
            for (const Muon::MdtPrepData* prd : *pColl)
            {
                if (prd->adc() >= 50 && prd->status() != Muon::MdtStatusMasked && prd->status() != Muon::MdtStatusBeforeSpectrum)
                {
//                  m_pMuGirl->msg(MSG::DEBUG) << "Adding PRD adc=" << prd->adc() << " status=" << status2str(prd->status()) << endreq;
                    array.push_back(prd);
                }
                else if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg(MSG::DEBUG) << "Dropping PRD adc=" << prd->adc() << " status=" << status2str(prd->status()) << endreq;
            }
        }
     }
    return array.size();
}

std::vector<IdentifierHash> MDT::retrieveAvailableCollections() const {
    // dummy function needed only for MM and sTGC
    std::vector<IdentifierHash> Ids;
    return Ids;
}


Amg::Vector3D MDT::hitPosition(const Trk::PrepRawData* pPrepData)
{
    const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepData);
    if (pMdtPrepData == NULL)
    {
        m_pMuGirl->msg(MSG::WARNING) << "Cannot convert from Trk::PrepRawData* to Muon::MdtPrepData*" << endreq;
        return Amg::Vector3D();
    }

    const MuonGM::MdtReadoutElement* pReadoutElement = pMdtPrepData->detectorElement();
    if (pReadoutElement == NULL)
    {
        m_pMuGirl->msg(MSG::WARNING) << "Cannot get MdtReadoutElement" << endreq;
        return Amg::Vector3D();
    }
    return pReadoutElement->tubePos(pPrepData->identify());
}

bool MDT::isEtaHit(const Trk::PrepRawData*)
{
    return true;
}

void MDT::buildSegments(Candidate* pCand, ChamberList& chambers, double QoverP)
{
    MsgStream log(m_pMuGirl->msgSvc(), m_pMuGirl->name());
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "MDT::buildSegments" << endreq;

    StationChamberMap stationChambers;
    StationList stations;

    for (ChamberList::const_iterator itCh = chambers.begin(); itCh != chambers.end(); itCh++)
    {
        Chamber* pChamber = *itCh;
        if (!pChamber->RIOs().empty())
        {
            StationChamberMap::iterator itSta = stationChambers.find(pChamber->station());
            if (itSta == stationChambers.end())
            {
                stations.push_back(pChamber->station());
                std::pair<StationChamberMap::iterator, bool> p =
                    stationChambers.insert(StationChamberMap::value_type(pChamber->station(), ChamberList()));
                itSta = p.first;
            }
            itSta->second.push_back(pChamber);
        }
    }

    TechnologyType triggerTech = chambers.front()->regionType() == BARREL_REGION ? RPC_TECH : TGC_TECH;
    Clusters& clusters = pCand->getClustersOnSegment(triggerTech, chambers.front()->distanceType(), chambers.front()->regionType());
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "getClusters("
            << TechnologyTypeName(triggerTech) << ","
            << DistanceTypeName(chambers.front()->distanceType()) << ","
            << RegionTypeName(chambers.front()->regionType()) << ")"
            << " contains " << count_clusters(clusters) << " clusters" << endreq;

    for (StationList::iterator itSta = stations.begin(); itSta != stations.end(); itSta++)
    {
        Station* pStation = *itSta;
        ChamberList& chamberList = stationChambers[pStation];
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "Station " << pStation->toString()
                << " has " << chamberList.size() << " chambers with ";
        if (m_pMuGirl->doMdtHough())
        {
            std::vector<const Muon::MuonSegment*>* pSegments = NULL;
            const ToolHandle<Muon::IMuonSegmentMaker>& pSegmentMaker = m_pMuGirl->mdtSegmentMaker();
            std::vector<std::vector<const Muon::MdtDriftCircleOnTrack*> > mdts(chamberList.size());
            unsigned nChamber = 0;
            for (ChamberList::const_iterator itCh = chamberList.begin(); itCh != chamberList.end(); itCh++)
            {
                Chamber* pChamber = *itCh;
                const RIOList& rios = pChamber->RIOs();
                std::vector<const Muon::MdtDriftCircleOnTrack*>& mdcots = mdts[nChamber++];
                for (RIOList::const_iterator itRIO = rios.begin(); itRIO != rios.end(); itRIO++)
                {
                    const Muon::MdtDriftCircleOnTrack* pMdcot =
                        dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(*itRIO);
                    if (pMdcot == NULL)
                    {
                        if (m_pMuGirl->msgLvl(MSG::DEBUG))
                            m_pMuGirl->msg(MSG::DEBUG) << "Cannot convert Trk::RIO_OnTrack to Muon::MdtDriftCircleOnTrack" << endreq;
                        return;
                    }
                    mdcots.push_back(pMdcot);
                }
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << mdcots.size() << " ";
            }
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "hits" << endreq;
            Trk::TrackRoad* pRoad = chamberList[0]->baseRoad();
            if (pRoad == NULL)
            {
                log << MSG::DEBUG << "Cannot find base road" << endreq;
                return;
            }
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                pRoad->dump(m_pMuGirl->msg());
            if (pCand->saveMdtSegmentMakerInfo())
            {
                MdtSegmentMakerInfo* pMdtSegmentMakerInfo = new MdtSegmentMakerInfo(pRoad, mdts);
                pCand->getMdtSegmentMakerInfo().push_back(pMdtSegmentMakerInfo);
            }
            double momentum = 0;
            if (QoverP != 0) momentum = fabs(1 / QoverP);
            pSegments = pSegmentMaker->find(*pRoad, mdts, clusters, true, momentum);
            if (pSegments == NULL || pSegments->empty())
            {
                if (pSegments != NULL)
                    delete pSegments;
                //log << MSG::WARNING << "Got NULL segment list from MdtSegmentMaker" << endreq;
                continue;
            }
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "Found " << pSegments->size() << "T0 segments" << endreq;

            //pCand->fillChamberT0s(*pSegments);
            for (std::vector<const Muon::MuonSegment*>::const_iterator itSeg = pSegments->
                    begin();
                    itSeg != pSegments->end();
                    itSeg++)
            {
                 pCand->addT0Segments(*itSeg);
            }
            double maxProb = 0.0;
            int maxQuality = -1;
            const Muon::MuonSegment* pMaxQualityTemp = NULL;
            for (std::vector<const Muon::MuonSegment*>::const_iterator itSeg = pSegments->
                    begin();
                    itSeg != pSegments->end();
                    itSeg++)
            {
                const Muon::MuonSegment* pMuonSegment = *itSeg;
                //const Muon::MuonSegment* pMuonSegment = new Muon::MuonSegment(**itSeg);
                int quality = pCand->segmentQuality(pMuonSegment);
                double prob = Segment::fitProbability(pMuonSegment);

                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << "Found MDT segment at " << pMuonSegment->globalPosition()
                    << " hits=" << pMuonSegment->numberOfContainedROTs()
                    << " chi2=" << pMuonSegment->fitQuality()->chiSquared()
                    << " prob=" << prob
                    << " quality=" << quality
                    << endreq;
                if (maxQuality < quality || (maxQuality == quality && maxProb < prob))
                {
                    maxQuality = quality;
                    maxProb = prob;
                    pMaxQualityTemp = pMuonSegment;
                }
            }
            const Muon::MuonSegment* pMaxQuality = NULL;
            if (pMaxQualityTemp != NULL)
                pMaxQuality = new Muon::MuonSegment(*pMaxQualityTemp);
            delete pSegments;
            if (pMaxQuality != NULL)
            {
                Segment* pSegment = pCand->addSegment(MuGirlNS::HOUGH_SEGMENT,
                                                      pMaxQuality,
                                                      MDT_TECH,
                                                      chamberList[0]->distanceType(),
                                                      chamberList[0]->regionType(),
                                                      chamberList[0]->station());
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << "Selected MDT segment at "
                    << pMaxQuality->globalPosition() << endreq;
                //pCand->addMDTSegmenttoMuonSegments(pSegment->muonSegment());
                pCand->markHits(chamberList, pSegment);
                pCand->addMDTSegmenttoMuonSegments(pMaxQuality);
                Amg::Vector3D segDir = pMaxQuality->globalDirection();
                Amg::Vector3D segPos1 = pMaxQuality->globalPosition();

                if (pSegmentMaker->type() == "HoughMDTTrkFinderTool")
                {
                    //RungeKutta rungeKutta;
                    const Trk::PlaneSurface& surface = pMaxQuality->associatedSurface();
                    const Trk::TrackSurfaceIntersection* pRefTrkIsect =
                        m_pMuGirl->Intersector()->intersectSurface(surface,chamberList[0]->baseIntersection()->trkIntersection(),pCand->qOverP());
                        //rungeKutta.intersectPlaneSurface(surface.center(),
                        //                                 surface.normal(),
                        //                                 chamberList[0]->baseIntersection()->trkIntersection(),
                        //                                 pCand->qOverP());
                    if (pRefTrkIsect == NULL)
                    {
                        m_pMuGirl->msg(MSG::WARNING) << " pRefTrkIsect is NULL !!!! " << endreq;
                        return;
                    }
                    Amg::Vector3D refDir  = pRefTrkIsect->direction();
                    Amg::Vector3D refPos1 = pRefTrkIsect->position();
                    Amg::Vector3D refPos2 = refPos1 + refDir;

                    Amg::Vector3D segPos2 = segPos1 + segDir;
                    double segPhi1 = segPos1.phi();
                    double segPerp1 = segPos1.perp();
                    Amg::setPerp(segPos1, segPerp1 / cos(segPhi1 - refPos1.phi()));
                    Amg::setPhi(segPos1, refPos1.phi());
                    double segPhi2 = segPos2.phi();
                    double segPerp2 = segPos2.perp();
                    Amg::setPerp(segPos2, segPerp2 / cos(segPhi2 - refPos2.phi()));
                    Amg::setPhi(segPos2, refPos2.phi());
                    segDir = segPos2 - segPos1;
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    {
                        m_pMuGirl->msg() << "MDT correction: "
                        << "pos from " << pMaxQuality->globalPosition() << " to " << segPos1 << endreq;
                        m_pMuGirl->msg() << "                "
                        << "dir from " << pMaxQuality->globalDirection() << " to " << segDir << endreq;
                    }
                }
                Trk::TrackSurfaceIntersection* pTrkIsect =
                    new Trk::TrackSurfaceIntersection(segPos1, segDir.unit(), 0.0);
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << "Adding MDT intersection at " << pTrkIsect << endreq;
                Intersection* pIsect = pCand->addIntersection(FIT_INTERSECTION,
                                       pTrkIsect,
                                       MDT_TECH,
                                       chamberList[0]->distanceType(),
                                       MDT_TECH,
                                       chamberList[0]->distanceType(),
                                       pStation->regionType(),
                                       NULL);
                pCand->setCellIntersection(MDT_TECH, chamberList[0]->distanceType(), pStation->regionType(), pIsect);
                pSegment->setIntersection(pIsect);
            }
        }
    }

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "MDT::buildSegments ended" << endreq;
}


}

