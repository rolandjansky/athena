/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkRoad/TrackRoad.h"
//#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"  // needed because MMPrepData is not included by the MuonPrepDataContainer.h

#include "MuGirlCandidate/MM.h"
#include "MuGirlCandidate/Chamber.h"
#include "MuGirlCandidate/Station.h"
#include "MuGirlCandidate/Candidate.h"
#include "MuGirlCandidate/Segment.h"
#include "MuGirlCandidate/CandidateTool.h"
#include "MuGirlCandidate/Intersection.h"

namespace MuGirlNS
{

MM::MM(CandidateTool* pMuGirl, const std::string& sPrepDataCollection) :
        Technology(pMuGirl, sPrepDataCollection),
        m_pPrepDataContainer(NULL)
{
    m_eType = MM_TECH;
    m_detId = ::PIXEL;   // dummy as we do not yet have the detector elemement implemented in the region selector
    m_pIdHelper = dynamic_cast<const MmIdHelper*>(pMuGirl->muonManager()->mmIdHelper());
    if(m_pIdHelper == 0)
      m_pMuGirl->msg(MSG::ERROR) << "IdHelper should be MmIdHelper, but it is NOT!" << endmsg;
}

const MuonGM::MuonReadoutElement* MM::readoutElement(const Identifier& id) const
{
    return m_pMuGirl->muonManager()->getMMReadoutElement(id);
}


int MM::stationID(const Identifier& id) const
{
    const MmIdHelper* mmHelper = dynamic_cast<const MmIdHelper*>(m_pIdHelper);
    return (mmHelper) ? mmHelper->stationName(id) : -1;
}

int MM::stationNameID(const std::string& name) const
{
    const MmIdHelper* mmHelper = dynamic_cast<const MmIdHelper*>(m_pIdHelper);
    return (mmHelper) ? mmHelper->stationNameIndex(name) : -1;
}

StatusCode MM::retrievePrepData()
{
    if (m_pMuGirl->evtStore()->contains<Muon::MMPrepDataContainer>(m_sPrepDataCollection))
    {
       StatusCode sc = m_pMuGirl->evtStore()->retrieve(m_pPrepDataContainer, m_sPrepDataCollection);
       if (sc.isFailure() || m_pPrepDataContainer == NULL)
       {
           m_pPrepDataContainer=NULL;
           if (m_pMuGirl->msgLvl(MSG::DEBUG))
               m_pMuGirl->msg(MSG::DEBUG) << "Cannot retrieve MM PrepData Container " << m_sPrepDataCollection << endmsg;
       }
       return StatusCode::SUCCESS;
    }
    else
    {
        m_pPrepDataContainer=NULL;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg(MSG::DEBUG) << "EventStore does not contain MM PrepData Container " << m_sPrepDataCollection << endmsg;
    }
    return StatusCode::SUCCESS;
}

unsigned MM::prepData(Chamber* pChamber, PrepDataList& array)
{
    array.clear();

    if (m_pPrepDataContainer == NULL)
    {
        m_pMuGirl->msg(MSG::INFO) << "Cannot retrieve MM PrepData Container " << m_sPrepDataCollection << endmsg;
        return 0;

        // so far the RdoToPrepData converter are not implemented for the MicroMegas
        /*
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
            m_pMuGirl->msg(MSG::WARNING) << "Cannot retrieve MDT PrepData Container " << m_sPrepDataCollection << endmsg;
            return 0;
        }
        */
    }
    if (m_pPrepDataContainer != NULL)
    {
        Muon::MMPrepDataContainer::const_iterator itColl = m_pPrepDataContainer->indexFind(pChamber->hashId());
        // If the container does not have a collection for this chamber, try the RdoToPrepData tool
        if (itColl == m_pPrepDataContainer->end())
        {
            // so far it is impossible to decode MicroMegas PrepRawData; if the collection is not in, then return an empty array
            m_pMuGirl->msg(MSG::WARNING) << "MM chamber hash ID " << pChamber->hashId() 
                     << " may be invalid: no collection found into the MmPrepDataContainer" << endmsg;

            /*
            std::vector<IdentifierHash> inhash, outhash;
            inhash.push_back(pChamber->hashId());
            if (m_pMuGirl->mdtRdoToPrepDataTool().empty())
               return 0;
            if (m_pMuGirl->mdtRdoToPrepDataTool()->decode(inhash, outhash).isSuccess() && !outhash.empty())
                itColl = m_pPrepDataContainer->indexFind(outhash.front());
            */
        }
        if (itColl != m_pPrepDataContainer->end())
        {
            //DataLink<Muon::RpcPrepDataCollection> pColl = *itColl;
            const Muon::MMPrepDataCollection* pColl = *itColl;
            for (const Muon::MMPrepData* prd : *pColl)
            {
                array.push_back(prd);  // collecting all the MM PrepRawData
            }
        }
     }
    return array.size();
}

std::vector<IdentifierHash> MM::retrieveAvailableCollections() const {
    std::vector<IdentifierHash> Ids;
    if (m_pPrepDataContainer == NULL)
    {
        m_pMuGirl->msg(MSG::DEBUG) << "Cannot retrieve MM PrepData Container " << m_sPrepDataCollection << endmsg;
        // so far the RdoToPrepData converter are not implemented for the sTGC
    }

    if (m_pPrepDataContainer != NULL)
    {
        Muon::MMPrepDataContainer::const_iterator itColl;
        for (itColl=m_pPrepDataContainer->begin(); itColl!=m_pPrepDataContainer->end();++itColl) {
            // return only the ID hash of collections with data
            if ( (*itColl)->size()!=0 ) Ids.push_back( (*itColl)->identifyHash() );
        }
    }

    return Ids;
}

Amg::Vector3D MM::hitPosition(const Trk::PrepRawData* pPrepData)
{
    const Muon::MMPrepData* pMmPrepData = dynamic_cast<const Muon::MMPrepData*>(pPrepData);
    if (pMmPrepData == NULL)
    {
        m_pMuGirl->msg(MSG::WARNING) << "Cannot convert from Trk::PrepRawData* to Muon::MMPrepData*" << endmsg;
        return Amg::Vector3D();
    }

    const MuonGM::MMReadoutElement* pReadoutElement = pMmPrepData->detectorElement();
    if (pReadoutElement == NULL)
    {
        m_pMuGirl->msg(MSG::WARNING) << "Cannot get MMReadoutElement" << endmsg;
        return Amg::Vector3D();
    }

    Amg::Vector2D strip_local_position;
    Amg::Vector3D strip_global_position,dummy;
    
    // return the position only if the stripPosition conversion is valid
    if ( pReadoutElement->stripPosition(pPrepData->identify(),strip_local_position) ) {
        pReadoutElement->surface(pPrepData->identify()).localToGlobal(strip_local_position,dummy,strip_global_position);
    }

    return strip_global_position;
}

bool MM::isEtaHit(const Trk::PrepRawData*)
{
    return true;
}

void MM::buildSegments(Candidate* /*pCand*/, ChamberList& /*chambers*/, double /*QoverP*/)
{
    // Not implemented yet
    /*
    MsgStream log(m_pMuGirl->msgSvc(), m_pMuGirl->name());
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "MDT::buildSegments" << endmsg;

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
            << " contains " << count_clusters(clusters) << " clusters" << endmsg;

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
                            m_pMuGirl->msg(MSG::DEBUG) << "Cannot convert Trk::RIO_OnTrack to Muon::MdtDriftCircleOnTrack" << endmsg;
                        return;
                    }
                    mdcots.push_back(pMdcot);
                }
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << mdcots.size() << " ";
            }
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "hits" << endmsg;
            Trk::TrackRoad* pRoad = chamberList[0]->baseRoad();
            if (pRoad == NULL)
            {
                log << MSG::DEBUG << "Cannot find base road" << endmsg;
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
                //log << MSG::WARNING << "Got NULL segment list from MdtSegmentMaker" << endmsg;
                continue;
            }
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "Found " << pSegments->size() << "T0 segments" << endmsg;

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
                    << endmsg;
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
                    << pMaxQuality->globalPosition() << endmsg;
                //pCand->addMDTSegmenttoMuonSegments(pSegment->muonSegment());
                pCand->markHits(chamberList, pSegment);
                pCand->addMDTSegmenttoMuonSegments(pMaxQuality);
                HepGeom::Vector3D<double> segDir = pMaxQuality->globalDirection();
                HepGeom::Point3D<double> segPos1 = pMaxQuality->globalPosition();

                if (pSegmentMaker->type() == "HoughMDTTrkFinderTool")
                {
                    RungeKutta rungeKutta;
                    const Trk::PlaneSurface& surface = pMaxQuality->associatedSurface();
                    const Trk::TrackSurfaceIntersection* pRefTrkIsect =
                        rungeKutta.intersectPlaneSurface(surface.center(),
                                                         surface.normal(),
                                                         chamberList[0]->baseIntersection()->trkIntersection(),
                                                         pCand->qOverP());
                    if (pRefTrkIsect == NULL)
                    {
                        m_pMuGirl->msg(MSG::WARNING) << " pRefTrkIsect is NULL !!!! " << endmsg;
                        return;
                    }
                    HepGeom::Vector3D<double> refDir = pRefTrkIsect->direction();
                    HepGeom::Point3D<double> refPos1 = pRefTrkIsect->position();
                    HepGeom::Point3D<double> refPos2 = refPos1 + refDir;

                    HepGeom::Point3D<double> segPos2 = segPos1 + segDir;
                    double segPhi1 = segPos1.phi();
                    double segPerp1 = segPos1.perp();
                    segPos1.setPerp(segPerp1 / cos(segPhi1 - refPos1.phi()));
                    segPos1.setPhi(refPos1.phi());
                    double segPhi2 = segPos2.phi();
                    double segPerp2 = segPos2.perp();
                    segPos2.setPerp(segPerp2 / cos(segPhi2 - refPos2.phi()));
                    segPos2.setPhi(refPos2.phi());
                    segDir = segPos2 - segPos1;
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    {
                        m_pMuGirl->msg() << "MDT correction: "
                        << "pos from " << pMaxQuality->globalPosition() << " to " << segPos1 << endmsg;
                        m_pMuGirl->msg() << "                "
                        << "dir from " << pMaxQuality->globalDirection() << " to " << segDir << endmsg;
                    }
                }
                Trk::TrackSurfaceIntersection* pTrkIsect =
                    new Trk::TrackSurfaceIntersection(segPos1, segDir.unit(), 0.0);
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << "Adding MDT intersection at " << pTrkIsect << endmsg;
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
        m_pMuGirl->msg() << "MDT::buildSegments ended" << endmsg;
    */
}


}

