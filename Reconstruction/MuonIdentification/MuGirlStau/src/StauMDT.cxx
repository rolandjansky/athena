/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauMDT.h"

#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuGirlStau/IStauBetaTofTool.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "GeoPrimitives/GeoPrimitives.h"

//================ Constructor =================================================
bool MUGIRLNS_STAUMDT_DO_CALIBRATION = false;

MuGirlNS::StauMDT::StauMDT(StauTool* pStauTool, MsgStream& log,
                           CLHEP::HepRandomEngine& randEngine,
        const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList) :
        m_pStau(pStauTool), m_log(log), m_beta(-1.), m_pCalibration(nullptr),
        m_segmentNumber(-1), m_chamberNumber(-1), m_randEngine (randEngine)
{
    m_pMdtSegmentMakerInfoList = &mdtSegmentMakerInfoList;

    if (m_pStau->doCalibration() && MUGIRLNS_STAUMDT_DO_CALIBRATION) initCalibrationParameters();
}

/*
 Task: prepare the calibration parameters.
 The t0 fit is done per segment (usually a segment is equivalent to a chamber), hence the shifts are per segment as well.
 */
void MuGirlNS::StauMDT::initCalibrationParameters()
{
    m_shifts.clear();
    m_pCalibration = m_pStau->calibration().getMdtCalibration();
    for (auto pMdtSegmentMakerInfo : *m_pMdtSegmentMakerInfoList)
    {
        std::vector<double> segmentShifts;
        for (auto mdcotList : pMdtSegmentMakerInfo->mdts)
        {
            if (mdcotList.empty()) continue;
            auto pMdcot = mdcotList.front();
            auto pMdtPrepData = pMdcot->prepRawData();
            //calibrate the tof
            int id = pMdtPrepData->identify().get_identifier32().get_compact() & 0xFFFF0000;
            double shift = 0;
            auto itCalib = m_pCalibration->find(id);
            if (itCalib == m_pCalibration->end())
            {
                //std::cout << "E123: cannot find shift for id " << hex << id << std::endl;
                shift = 0;
            }
            else
            {
                if (m_pStau->isData())
                    shift = itCalib->second.timeShift; //shift
                else
                { //smear
                    double error = itCalib->second.error;
                    shift = m_pStau->mdtSmearFactor() * CLHEP::RandGaussZiggurat::shoot(&m_randEngine, 0, error); //Sofia: Low 0.5 MID 0.9 HIGH 1.4
                }
            }
            segmentShifts.push_back(shift);
        }
        m_shifts.push_back(segmentShifts);
    }
}

//================ Destructor =================================================

MuGirlNS::StauMDT::~StauMDT()
{
}

void MuGirlNS::StauMDT::initStepData(MdtStepData* mdtData, double beta, double /*tTrack*/)
{
    mdtData->pStationDataList = new std::vector<MdtStepStationData*>;
    mdtData->beta = beta;
    mdtData->numSegs = 0;
    mdtData->totNumHits = 0;
    mdtData->hitsInSegments = 0;
    mdtData->tTrack = 0;
    mdtData->dof = 0;
}

void MuGirlNS::StauMDT::clearStepData(MdtStepData* mdtData)
{
    if (mdtData == NULL) return;
    auto pMdtStepStationsData = mdtData->pStationDataList;
    if (pMdtStepStationsData != NULL)
    {
        for (auto pStationData : *pMdtStepStationsData)
            delete pStationData;
        pMdtStepStationsData->clear();
        delete pMdtStepStationsData;
    }

    delete mdtData;
}

void MuGirlNS::StauMDT::printStepData(MdtStepData* mdtData)
{
    if (mdtData == NULL) return;
    LOG_VERBOSE << "mdt data: beta=" << mdtData->beta
                << " chi2=" << mdtData->chi2
                << " dof=" << mdtData->dof
                << " numOfHits=" << mdtData->totNumHits
                << endmsg;
    auto pStationDataList = mdtData->pStationDataList;
    int i = 0;
    if (pStationDataList != NULL)
    {
        for (auto pMdtStepStation : *pStationDataList)
        {
            i++;
            LOG_VERBOSE << "       station #" << i << ": distance=" << pMdtStepStation->distance
                        << " chi2=" << pMdtStepStation->chi2
                        << " dof="<< pMdtStepStation->dof
                        << " #hits=" << pMdtStepStation->numHits
                        << " #segs="<< pMdtStepStation->numSegs
                        << endmsg;
        }
    }
}

void MuGirlNS::StauMDT::findNewSegments(double beta)
{
    LOG_VERBOSE << "beta=" << beta << endmsg;

    //clear new mdt segments
    m_pStau->clearNewMdtSegments();

    //Set the correct beta in the ToF tool
    m_pStau->tofTool()->setBeta(beta);

    if (m_pStau->doCalibration() && MUGIRLNS_STAUMDT_DO_CALIBRATION) m_segmentNumber = 0;

    //unsigned int iStation = 0;
    for (auto pMdtSegmentMakerInfo : *m_pMdtSegmentMakerInfoList)
    {
        auto mdts = pMdtSegmentMakerInfo->mdts;
        const Trk::TrackRoad* pRoad = pMdtSegmentMakerInfo->pRoad;
        //recreate MDCOTLists
        auto pMdocotLists = new MDCOTLists();
        recreateMdcots(mdts, pMdocotLists);

        //build the segments
        TriggerClusters clusters;
	m_pStau->mdtSegmentMaker()->find(*pRoad, *pMdocotLists, clusters, 0, true,
                m_pStau->idP());

        //choose the best segment and store it, ignore the number of hits
        const Muon::MuonSegment* pBestSegment = NULL;
        if (pBestSegment != NULL) m_pStau->newMdtSegments()->push_back(pBestSegment);

        //delete the MDCOTLists
        deleteMdcotLists(pMdocotLists);
        delete pMdocotLists;

        if (m_pStau->doCalibration() && MUGIRLNS_STAUMDT_DO_CALIBRATION) m_segmentNumber++;
    }
    LOG_VERBOSE << "done" << endmsg;
}

void MuGirlNS::StauMDT::processMdtWithBeta(double currentBeta, MdtStepData* mdtData)
{
    LOG_VERBOSE << "beta=" << currentBeta << endmsg;

    //Set the correct beta in the ToF tool
    m_pStau->tofTool()->setBeta(currentBeta);

    //calibration
    if (m_pStau->doCalibration() && MUGIRLNS_STAUMDT_DO_CALIBRATION) m_segmentNumber = 0;

    //unsigned int iStation = 0;
    for (auto pMdtSegmentMakerInfo : *m_pMdtSegmentMakerInfoList)
    {
        auto mdts = pMdtSegmentMakerInfo->mdts;
        const Trk::TrackRoad* pRoad = pMdtSegmentMakerInfo->pRoad;
        //recreate MDCOTLists
        auto pMdocotLists = new MDCOTLists();
        int numOfHits = recreateMdcots(mdts, pMdocotLists); // the current beta is already set to the ToF tool
        if (numOfHits <= 4)
        {
            deleteMdcotLists(pMdocotLists);
            delete pMdocotLists;
            continue;
        }
        //build the segments
        TriggerClusters clusters;
	m_pStau->mdtSegmentMaker()->find(*pRoad, *pMdocotLists, clusters, 0, true,
                m_pStau->idP());

        //delete the MDCOTLists
        deleteMdcotLists(pMdocotLists);
        delete pMdocotLists;

        if (m_pStau->doCalibration() && MUGIRLNS_STAUMDT_DO_CALIBRATION) m_segmentNumber++;
    }

//    mdtData->dof = mdtData->totNumHits - 2*mdtData->pStationDataList->size();
    printStepData(mdtData);

    LOG_VERBOSE << "done" << endmsg;
}

void MuGirlNS::StauMDT::fillStationData(MdtStepData* mdtData, MdtSegments* pSegments)
{
    //choose the best segment and delete the others
    int numSegsInStation = (pSegments != NULL ? pSegments->size() : 0);
    const Muon::MuonSegment* pBestSegment = NULL;
    bestMdtSegment(pSegments, pBestSegment);
    if (pBestSegment == NULL) return;

    //found the best segment - store the data
    auto pMdtStationData = new MdtStepStationData();
    pMdtStationData->chi2 = pBestSegment->fitQuality()->chiSquared();
    pMdtStationData->dof = pBestSegment->fitQuality()->numberDoF();

    pMdtStationData->numHits = pBestSegment->numberOfContainedROTs();
    pMdtStationData->distance = pBestSegment->globalPosition().perp();
    pMdtStationData->numSegs = numSegsInStation;
    pMdtStationData->dirEta = pBestSegment->globalDirection().eta();
    pMdtStationData->dirPhi = pBestSegment->globalDirection().phi();

    mdtData->numSegs++;
    mdtData->hitsInSegments += pMdtStationData->numHits;

    mdtData->chi2 += pMdtStationData->chi2;
    mdtData->dof += pMdtStationData->dof;
    mdtData->totNumHits += pMdtStationData->numHits;
    mdtData->pStationDataList->push_back(pMdtStationData);
}

int MuGirlNS::StauMDT::recreateMdcots(MDCOTLists& mdts, MDCOTLists* pMdocotLists)
{
    int numOfHits = 0;
    if (m_pStau->doCalibration() && MUGIRLNS_STAUMDT_DO_CALIBRATION) m_chamberNumber = 0;
    for (auto mdcotList : mdts)
    {
        if (m_pStau->doCalibration() && MUGIRLNS_STAUMDT_DO_CALIBRATION)
        {
            double shift = m_shifts[m_segmentNumber][m_chamberNumber];
            if (shift == 0) continue; //we don't know how to shift this chamber, so don't use it
            m_pStau->tofTool()->setTShift(shift);
        }

        MDCOTList NewMdcotList;
        for (auto pMdcot : mdcotList)
        {
            numOfHits++;
            const Trk::RIO_OnTrack* pRio = this->recreateRio(pMdcot);
            if (pRio == NULL)
            {
                LOG_VERBOSE << "createRIO_OnTrack failed" << endmsg;
                continue;
            }
            NewMdcotList.push_back(dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(pRio));
        }
        pMdocotLists->push_back(NewMdcotList);
        if (m_pStau->doCalibration() && MUGIRLNS_STAUMDT_DO_CALIBRATION) m_chamberNumber++;
    }

    return numOfHits;
}

const Trk::RIO_OnTrack* MuGirlNS::StauMDT::recreateRio(const Muon::MdtDriftCircleOnTrack* pMdcot)
{
    auto pMdtPrepData = pMdcot->prepRawData();
    //recreate the RIOs with the beta which was set to the ToF
    auto gdir = Amg::Vector3D(pMdcot->globalPosition());
    auto* rio = m_pStau->driftCircleCreator()->createRIO_OnTrack(*pMdtPrepData,
            pMdcot->globalPosition(), &gdir);
    return rio;
    //return m_pStau->driftCircleCreator()->createRIO_OnTrack(*pMdtPrepData, pMdcot->globalPosition(),gdir);
}

void MuGirlNS::StauMDT::deleteMdcotLists(MuGirlNS::MDCOTLists* pMdocotLists)
{
    for (auto mdcotList : *pMdocotLists)
    {
        for (auto pMdcot : mdcotList)
            delete pMdcot;
        mdcotList.clear();
    }
    pMdocotLists->clear();
}

void MuGirlNS::StauMDT::bestMdtSegment(MdtSegments* pSegments,
        const Muon::MuonSegment*& pBestSegment, bool ignoreNumHits)
{
    pBestSegment = NULL;
    if (pSegments == NULL)
    {
        LOG_VERBOSE << "got NULL segment list from MdtSegmentMaker" << endmsg;
        return;
    }

    LOG_VERBOSE << "got " << pSegments->size() << " MDT segments" << endmsg;

    double numHits = 0;

    for (auto pMuonSegment : *pSegments)
    {
        LOG_VERBOSE << "numHits: " << pMuonSegment->numberOfContainedROTs()
                    << " chi2: " << pMuonSegment->fitQuality()->chiSquared()
                    << endmsg;

        if (!ignoreNumHits && 4 >= pMuonSegment->numberOfContainedROTs()) continue;
        if (pMuonSegment->numberOfContainedROTs() > numHits)
        {
            numHits = pMuonSegment->numberOfContainedROTs();
            pBestSegment = pMuonSegment;
        }
        else if (pMuonSegment->numberOfContainedROTs() == numHits)
        {
            if (pBestSegment)
            {
                if (pMuonSegment->fitQuality()->chiSquared()
                    <= pBestSegment->fitQuality()->chiSquared()) pBestSegment = pMuonSegment;
            }
            else
              m_log << MSG::WARNING << "There is no best Segment to compare with - doing nothing." << endmsg;
        }
    }

    if (pBestSegment != NULL)
        LOG_VERBOSE << "bestSeg: " << pBestSegment
                    << " numHits " << pBestSegment->numberOfContainedROTs()
                    << " chi2 " << pBestSegment->fitQuality()->chiSquared()
                    << " dof " << pBestSegment->fitQuality()->numberDoF()
                    << endmsg;
    else
        LOG_VERBOSE << "bestMdtSegment found no best segment" << endmsg;

    return;
}

void MuGirlNS::StauMDT::clear()
{

}

void MuGirlNS::StauMDT::processMdtWithTTrack(double tTrack, MdtStepData* mdtData, double beta)
{
    LOG_VERBOSE << "entered" << endmsg;

    //Set the tTrack in the ToF tool
    m_pStau->tofTool()->setBeta(beta);
    m_pStau->tofTool()->setTTrack(tTrack);

    //unsigned int iStation = 0;
    for (auto pMdtSegmentMakerInfo : *m_pMdtSegmentMakerInfoList)
    {
        MDCOTLists mdts = pMdtSegmentMakerInfo->mdts;
        auto pRoad = pMdtSegmentMakerInfo->pRoad;
        //recreate MDCOTLists
        auto pMdocotLists = new MDCOTLists();
        int numOfHits = recreateMdcots(mdts, pMdocotLists); // the tTrack is already set to the ToF tool
        if (numOfHits <= 4)
        {
            deleteMdcotLists(pMdocotLists);
            delete pMdocotLists;
            continue;
        }
        //build the segments
        TriggerClusters clusters;
	m_pStau->mdtSegmentMaker()->find(*pRoad, *pMdocotLists, clusters, 0, true,
                m_pStau->idP());

        //delete the MDCOTLists
        deleteMdcotLists(pMdocotLists);
        delete pMdocotLists;

    }

    //mdtData->dof = mdtData->totNumHits - 2*mdtData->pStationDataList->size();
    printStepData(mdtData);

    LOG_VERBOSE << "done" << endmsg;
}

//const Trk::RIO_OnTrack* MuGirlNS::StauMDT::processWithTTrackRT(const Muon::MdtDriftCircleOnTrack* pMdcot)
//{
// 1. Take the hits, 2. calculate t(R), 3. obtain actual hit time (hit - chamberT0 -t(R)) 4. chi2 = sum (tof-actual_hit_time)^2/error_hit^2

//}
