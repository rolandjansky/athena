/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauMDT.h"

#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuGirlStau/IStauBetaTofTool.h"

#include "GeoPrimitives/GeoPrimitives.h"


//================ Constructor =================================================

MuGirlNS::StauMDT::StauMDT(StauTool* pStauTool, MsgStream& log, const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList):
    m_pStau(pStauTool),
    m_log(log),
    m_beta(-1.),
    m_segmentNumber(-1),
    m_chamberNumber(-1)
{
    m_pMdtSegmentMakerInfoList = &mdtSegmentMakerInfoList;

    if(m_pStau->doCalibration()) initCalibrationParameters();
}

/*
    Task: prepare the calibration parameters. 
    The t0 fit is done per segment (usually a segment is equivalent to a chamber), hence the shifts are per segment as well.
*/
void MuGirlNS::StauMDT::initCalibrationParameters()
{
    m_shifts.clear();
    m_pCalibration = m_pStau->calibration().getMdtCalibration();
    for(MuGirlNS::MdtSegmentMakerInfoList::const_iterator it = m_pMdtSegmentMakerInfoList->begin(); it != m_pMdtSegmentMakerInfoList->end(); it++)
    {
        std::vector<double> segmentShifts;
        MdtSegmentMakerInfo* pMdtSegmentMakerInfo = *it;
        MDCOTLists mdts = pMdtSegmentMakerInfo->mdts;
        for(MDCOTLists::iterator itMdts = mdts.begin(); itMdts != mdts.end(); itMdts++)
        {
             MDCOTList mdcotList = *itMdts;

             MDCOTList::iterator itMdt = mdcotList.begin();
             if(itMdt == mdcotList.end()) continue;
             const Muon::MdtDriftCircleOnTrack* pMdcot = *itMdt;
             const Muon::MdtPrepData* pMdtPrepData = pMdcot->prepRawData();
             //calibrate the tof
             int id = pMdtPrepData->identify().get_identifier32().get_compact() & 0xFFFF0000;
             double shift = 0;
             std::map<int, StauCalibrationParameters >::iterator itCalib = m_pCalibration->find(id);
             if(itCalib == m_pCalibration->end())
             {
                //std::cout << "E123: cannot find shift for id " << hex << id << std::endl;         
                shift = 0;
             }else
             {
                 if(m_pStau->isData()) shift = itCalib->second.timeShift;//shift
                 else
                 {//smear
                      double error = itCalib->second.error;
                      TRandom3 rand(0);
                      shift = m_pStau->mdtSmearFactor() * rand.Gaus(0,error); //Sofia: Low 0.5 MID 0.9 HIGH 1.4
                 }
             }
             segmentShifts.push_back(shift);
        }
        m_shifts.push_back(segmentShifts);
    }
}

//================ Destructor =================================================

MuGirlNS::StauMDT::~StauMDT()
{}

void MuGirlNS::StauMDT::initStepData(MdtStepData* mdtData, double beta, double /*tTrack*/)
{
    mdtData->pStationDataList = new std::vector< MdtStepStationData* >;
    mdtData->beta = beta;
    mdtData->numSegs = 0;
    mdtData->totNumHits = 0;
    mdtData->hitsInSegments = 0;
    mdtData->tTrack = 0;
    mdtData->dof = 0;
}

void MuGirlNS::StauMDT::clearStepData(MdtStepData* mdtData)
{
    if(NULL==mdtData) return;
    std::vector< MdtStepStationData* >* pMdtStepStationsData = mdtData->pStationDataList;
    if(NULL!=pMdtStepStationsData)
    {
        for(std::vector< MdtStepStationData* >::iterator it = pMdtStepStationsData->begin(); it != pMdtStepStationsData->end(); it++)
        {
            delete *it;
	}
	pMdtStepStationsData->clear();
        delete pMdtStepStationsData;
    }
    
    delete mdtData;
    mdtData = NULL;
}

void MuGirlNS::StauMDT::printStepData(MdtStepData* mdtData)
{
    if(NULL==mdtData) return;
    m_log<<MSG::VERBOSE << "mdt data: beta=" << mdtData->beta 
          << " chi2=" << mdtData->chi2
	  << " dof=" << mdtData->dof
	  << " numOfHits=" << mdtData->totNumHits << endreq;
    std::vector< MdtStepStationData* >* pStationDataList = mdtData->pStationDataList;
    int i=0;
    if(NULL!=pStationDataList) 
    {
        for(std::vector< MdtStepStationData* >::iterator it1 = pStationDataList->begin(); it1 != pStationDataList->end(); it1++)
        {
            i++;
            MdtStepStationData* mdtStepStation = *it1;
            m_log<<MSG::VERBOSE << "       station #" << i << ": distance=" << mdtStepStation->distance
                      << " chi2=" << mdtStepStation->chi2 
                      << " dof=" << mdtStepStation->dof 
                      << " #hits=" << mdtStepStation->numHits
                      << " #segs=" << mdtStepStation->numSegs << endreq;
        }
    }
}

void MuGirlNS::StauMDT::findNewSegments(double beta)
{
    m_log << MSG::VERBOSE << "findNewSegments( beta=" << beta << " )" << endreq;
    
    //clear new mdt segments
    m_pStau->clearNewMdtSegments();
    
    //Set the correct beta in the ToF tool
    m_pStau->tofTool()->setBeta(beta);
   
    if(m_pStau->doCalibration()) m_segmentNumber = 0;
 
    //unsigned int iStation = 0;
    for(MuGirlNS::MdtSegmentMakerInfoList::const_iterator it = m_pMdtSegmentMakerInfoList->begin(); it != m_pMdtSegmentMakerInfoList->end(); it++)
    {
        MdtSegmentMakerInfo* pMdtSegmentMakerInfo = *it;
        MDCOTLists mdts = pMdtSegmentMakerInfo->mdts;
        const Trk::TrackRoad* pRoad = pMdtSegmentMakerInfo->pRoad;
	//recreate MDCOTLists
        MDCOTLists* pMdocotLists = new MDCOTLists();
	recreateMdcots(mdts, pMdocotLists); 
	
	//build the segments
        TriggerClusters clusters;
	MdtSegments* pSegments = m_pStau->mdtSegmentMaker()->find(*pRoad, *pMdocotLists, clusters, true, m_pStau->idP());
	
	//choose the best segment and store it, ignore the number of hits
	const Muon::MuonSegment* pBestSegment = NULL;
    	bestMdtSegment(pSegments,pBestSegment,true);
	if(NULL!=pBestSegment) m_pStau->newMdtSegments()->push_back(pBestSegment);
    
	//delete the MDCOTLists
        deleteMdcotLists(pMdocotLists);
        delete pMdocotLists;
	
	//store the best segment and delete the others 
	if(NULL!=pSegments)
	{ 
            for(MdtSegments::const_iterator itSeg = pSegments->begin(); itSeg != pSegments->end(); itSeg++)
            {
                const Muon::MuonSegment* pMuonSegment = *itSeg;
                if(pBestSegment!=pMuonSegment) delete pMuonSegment;
            }
            pSegments->clear();
            delete pSegments;
	}

        if(m_pStau->doCalibration()) m_segmentNumber++;
     }

}

void MuGirlNS::StauMDT::processMdtWithBeta(double currentBeta, MdtStepData* mdtData)
{
    m_log << MSG::VERBOSE << "processMdtWithBeta( beta=" << currentBeta << " )" << endreq;
    
    //Set the correct beta in the ToF tool
    m_pStau->tofTool()->setBeta(currentBeta);
   
    //calibration
    if(m_pStau->doCalibration()) m_segmentNumber = 0;
 
    //unsigned int iStation = 0;
    for(MuGirlNS::MdtSegmentMakerInfoList::const_iterator it = m_pMdtSegmentMakerInfoList->begin(); it != m_pMdtSegmentMakerInfoList->end(); it++)
    {
        MdtSegmentMakerInfo* pMdtSegmentMakerInfo = *it;
        MDCOTLists mdts = pMdtSegmentMakerInfo->mdts;
        const Trk::TrackRoad* pRoad = pMdtSegmentMakerInfo->pRoad;
	//recreate MDCOTLists
        MDCOTLists* pMdocotLists = new MDCOTLists();
	int numOfHits = recreateMdcots(mdts, pMdocotLists); // the current beta is already set to the ToF tool
	if(numOfHits<=4)
	{
	   deleteMdcotLists(pMdocotLists);
	   delete pMdocotLists;
	   continue;
	}
	//build the segments
        TriggerClusters clusters;
	MdtSegments* pSegments = m_pStau->mdtSegmentMaker()->find(*pRoad, *pMdocotLists, clusters, true, m_pStau->idP());
	
	//store the data in the iteration
	if(NULL!=pSegments) fillStationData(mdtData, pSegments);
	
	//delete the MDCOTLists
        deleteMdcotLists(pMdocotLists);
        delete pMdocotLists;
	
	//delete the segments 
	if(NULL!=pSegments)
	{ 
            for(MdtSegments::const_iterator itSeg = pSegments->begin(); itSeg != pSegments->end(); itSeg++)
            {
                const Muon::MuonSegment* pMuonSegment = *itSeg;
                delete pMuonSegment;
            }
            pSegments->clear();
            delete pSegments;
	}

        if(m_pStau->doCalibration()) m_segmentNumber++;
    }
      
//    mdtData->dof = mdtData->totNumHits - 2*mdtData->pStationDataList->size();
    printStepData(mdtData);     
      
    m_log << MSG::VERBOSE << "processMdtWithBeta() - done" << endreq;
}

void MuGirlNS::StauMDT::fillStationData(MdtStepData* mdtData, MdtSegments* pSegments)
{
    //choose the best segment and delete the others
    int numSegsInStation = (NULL!=pSegments ? pSegments->size() : 0 );
    const Muon::MuonSegment* pBestSegment = NULL;
    bestMdtSegment(pSegments,pBestSegment);
    if(NULL==pBestSegment) return;
    
    //found the best segment - store the data
    MdtStepStationData* pMdtStationData = new MdtStepStationData();
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
    if(m_pStau->doCalibration()) m_chamberNumber = 0;
    for(MDCOTLists::iterator itMdts = mdts.begin(); itMdts != mdts.end(); itMdts++)
    {
        MDCOTList mdcotList = *itMdts;

        if(m_pStau->doCalibration())
        {
            double shift = m_shifts[m_segmentNumber][m_chamberNumber];
            if(0==shift) continue; //we don't know how to shift this chamber, so don't use it 
            m_pStau->tofTool()->setTShift(shift);
        }

        MDCOTList NewMdcotList;

        for( MDCOTList::iterator itMdt = mdcotList.begin(); itMdt != mdcotList.end(); itMdt++)
        {
	    numOfHits++;
            const Muon::MdtDriftCircleOnTrack* pMdcot = *itMdt;

            const Trk::RIO_OnTrack* pRio = this->recreateRio(pMdcot);
            if(NULL==pRio)
            {
                m_log << MSG::VERBOSE << "createRIO_OnTrack failed" << endreq;
                continue;
            }
            const Muon::MdtDriftCircleOnTrack* pNewMdcot = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(pRio);

            NewMdcotList.push_back(pNewMdcot);
        }
        pMdocotLists->push_back(NewMdcotList);
        if(m_pStau->doCalibration()) m_chamberNumber++;
    }
    
    return numOfHits;
}

const Trk::RIO_OnTrack* MuGirlNS::StauMDT::recreateRio(const Muon::MdtDriftCircleOnTrack* pMdcot)
{
    const Muon::MdtPrepData* pMdtPrepData = pMdcot->prepRawData();
    //recreate the RIOs with the beta which was set to the ToF
    const Amg::Vector3D* gdir = new Amg::Vector3D(pMdcot->globalPosition());
    const Trk::RIO_OnTrack* rio = m_pStau->driftCircleCreator()->createRIO_OnTrack(*pMdtPrepData, pMdcot->globalPosition(),gdir);
    delete gdir;
    return rio;
    //return m_pStau->driftCircleCreator()->createRIO_OnTrack(*pMdtPrepData, pMdcot->globalPosition(),gdir);
}

void MuGirlNS::StauMDT::deleteMdcotLists(MuGirlNS::MDCOTLists* pMdocotLists)
{
    for(MuGirlNS::MDCOTLists::iterator itLists = pMdocotLists->begin(); itLists!=pMdocotLists->end(); itLists++)
    {
        MuGirlNS::MDCOTList mdcotList = *itLists;
        for(MuGirlNS::MDCOTList::iterator itList = mdcotList.begin(); itList!=mdcotList.end(); itList++)
        {
            delete *itList;
        }
        mdcotList.clear();
    }
    pMdocotLists->clear();
}

void MuGirlNS::StauMDT::bestMdtSegment(MdtSegments* pSegments, const Muon::MuonSegment*& pBestSegment, bool ignoreNumHits)
{
    pBestSegment = NULL;
    if(pSegments == NULL)
    {
        m_log << MSG::VERBOSE << "Got NULL segment list from MdtSegmentMaker" << endreq;
	return;
    }
    
    m_log << MSG::VERBOSE << "Got " << pSegments->size() << " MDT segments" << endreq;
    
    double numHits = 0;
    
    for(MdtSegments::const_iterator itSeg = pSegments->begin(); itSeg != pSegments->end(); itSeg++)
    {
        const Muon::MuonSegment* pMuonSegment = *itSeg;
	
	m_log << MSG::VERBOSE << "numHits " << pMuonSegment->numberOfContainedROTs() 
	                      << " chi2 " << pMuonSegment->fitQuality()->chiSquared() << endreq;

        if(!ignoreNumHits && 4 >= pMuonSegment->numberOfContainedROTs() ) continue;
	if(pMuonSegment->numberOfContainedROTs() > numHits)
	{
	    numHits = pMuonSegment->numberOfContainedROTs();
	    pBestSegment = pMuonSegment;
	}else if(pMuonSegment->numberOfContainedROTs() == numHits)
	{
            if(pMuonSegment->fitQuality()->chiSquared() <= pBestSegment->fitQuality()->chiSquared())
            {
                pBestSegment = pMuonSegment;
            }
        }
    }
    
    if(NULL != pBestSegment)
    {
        m_log << MSG::VERBOSE << "bestSeg: " << pBestSegment 
                          << " numHits " << pBestSegment->numberOfContainedROTs() 
	                  << " chi2 " << pBestSegment->fitQuality()->chiSquared() 
	                  << " dof " << pBestSegment->fitQuality()->numberDoF() << endreq;
    }else
    {
        m_log << MSG::VERBOSE << "bestMdtSegment found no best segment" << endreq;
    }
    
    return;
}

void MuGirlNS::StauMDT::clear()
{

}


void MuGirlNS::StauMDT::processMdtWithTTrack(double tTrack, MdtStepData* mdtData, double beta)
{
	
	   //Set the tTrack in the ToF tool
            m_pStau->tofTool()->setBeta(beta);
	    m_pStau->tofTool()->setTTrack(tTrack);
	    
	    //unsigned int iStation = 0;
	    for(MuGirlNS::MdtSegmentMakerInfoList::const_iterator it = m_pMdtSegmentMakerInfoList->begin(); it != m_pMdtSegmentMakerInfoList->end(); it++)
	    {
	        MdtSegmentMakerInfo* pMdtSegmentMakerInfo = *it;
	        MDCOTLists mdts = pMdtSegmentMakerInfo->mdts;
	        const Trk::TrackRoad* pRoad = pMdtSegmentMakerInfo->pRoad;
		//recreate MDCOTLists
	        MDCOTLists* pMdocotLists = new MDCOTLists();
		int numOfHits = recreateMdcots(mdts, pMdocotLists); // the tTrack is already set to the ToF tool
		if(numOfHits<=4)
		{
		   deleteMdcotLists(pMdocotLists);
		   delete pMdocotLists;
		   continue;
		}
		//build the segments
	        TriggerClusters clusters;
		MdtSegments* pSegments = m_pStau->mdtSegmentMaker()->find(*pRoad, *pMdocotLists, clusters, true, m_pStau->idP());
		
		//store the data in the iteration
		if(NULL!=pSegments) fillStationData(mdtData, pSegments);
		
		//delete the MDCOTLists
	        deleteMdcotLists(pMdocotLists);
	        delete pMdocotLists;
		
		//delete the segments 
		if(NULL!=pSegments)
		{ 
	            for(MdtSegments::const_iterator itSeg = pSegments->begin(); itSeg != pSegments->end(); itSeg++)
	            {
	                const Muon::MuonSegment* pMuonSegment = *itSeg;
	                delete pMuonSegment;
	            }
	            pSegments->clear();
	            delete pSegments;
		}
	    }
	      
            //mdtData->dof = mdtData->totNumHits - 2*mdtData->pStationDataList->size();
	    printStepData(mdtData);     
	      
	    m_log << MSG::VERBOSE << "processMdtWithTTrack() - done" << endreq;
}

//const Trk::RIO_OnTrack* MuGirlNS::StauMDT::processWithTTrackRT(const Muon::MdtDriftCircleOnTrack* pMdcot)
//{
// 1. Take the hits, 2. calculate t(R), 3. obtain actual hit time (hit - chamberT0 -t(R)) 4. chi2 = sum (tof-actual_hit_time)^2/error_hit^2

//}
