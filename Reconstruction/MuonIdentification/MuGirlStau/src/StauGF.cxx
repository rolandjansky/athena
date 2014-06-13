/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauGF.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "MuGirlInterfaces/SegmentManager.h"


//================ Constructor =================================================

MuGirlNS::StauGF::StauGF(StauTool* pStauTool, MsgStream& log, const MuGirlNS::MuonSegmentList& muonSegments):
    m_pStau(pStauTool),
    m_log(log),
    m_muonSegments(muonSegments),
    m_initialTrackParameters(NULL)
{
}

//================ Destructor =================================================

MuGirlNS::StauGF::~StauGF()
{}

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
    if(NULL==gfData) return;
    delete gfData;
    gfData = NULL;
}

void MuGirlNS::StauGF::printStepData(GlobalFitData* gfData)
{
    if(NULL==gfData) 
    {
        m_log<<MSG::VERBOSE << "gf data is NULL" << endreq;
	return;
    }
    
    m_log<<MSG::VERBOSE << "gf data: beta=" << gfData->beta
              << " chi2=" << gfData->chi2
              << " dof=" << gfData->hitsOnTrk << endreq;
}

void MuGirlNS::StauGF::processGlobalFitWithBeta(double currentBeta, GlobalFitData* gfData)
{
    m_log << MSG::VERBOSE << "processGlobalFitWithBeta( beta=" << currentBeta << " ) " << gfData << endreq;
    
    RIO_OnTrackLists* pTriggerHitsInSegments = new RIO_OnTrackLists();
    std::vector<const Muon::MdtDriftCircleOnTrack*>* pMdtHits = new std::vector<const Muon::MdtDriftCircleOnTrack*>;
   
/*    prepareForGlobalFit(pTriggerHitsInSegments, pMdtHits);
    const Trk::Track* pTrack = m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), pTriggerHitsInSegments, pMdtHits, true);*/
    
    MuGirlNS::MuonSegmentList muonSegments;
    prepareForGlobalFit(muonSegments);
   
    const Trk::Track* pTrack = m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), muonSegments, false);
    
    if(NULL!=pTrack)
    {
        gfData->chi2 = pTrack->fitQuality()->chiSquared();
        gfData->dof = pTrack->fitQuality()->numberDoF();
        gfData->hitsOnTrk = (double)pTrack->measurementsOnTrack()->size();
        gfData->outliersOnTrk = (double)pTrack->outliersOnTrack()->size();
	
	double px = (double)pTrack->perigeeParameters()->momentum()[Trk::px];
	double py = (double)pTrack->perigeeParameters()->momentum()[Trk::py];
	double pz = (double)pTrack->perigeeParameters()->momentum()[Trk::pz];
	double p = sqrt(px*px+py*py+pz*pz);
	gfData->p = p;
    }else
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
    m_log << MSG::VERBOSE << "processGlobalFitWithBeta() - done" << endreq;
}

void MuGirlNS::StauGF::prepareForGlobalFit(RIO_OnTrackLists* pTriggerHitsInSegments, std::vector<const Muon::MdtDriftCircleOnTrack*>* pMdtHits)
{
    m_log << MSG::VERBOSE << "prepareForGlobalFit() "  << endreq;
    if(NULL!=m_pStau->rpcHitsInSegments())
    {
        for(RIO_OnTrackLists::const_iterator it = m_pStau->rpcHitsInSegments()->begin(); it != m_pStau->rpcHitsInSegments()->end(); it++)
	{
	    RIO_OnTrackList* pRioList = *it;
	    pTriggerHitsInSegments->push_back(pRioList);
	}
    }
    
    if(NULL!=m_pStau->tgcHitsInSegments())
    {
        for(RIO_OnTrackLists::const_iterator it = m_pStau->tgcHitsInSegments()->begin(); it != m_pStau->tgcHitsInSegments()->end(); it++)
	{
	    RIO_OnTrackList* pRioList = *it;
	    pTriggerHitsInSegments->push_back(pRioList);
	}
    }
    
    if(NULL!=m_pStau->newMdtSegments())
    {
        for(MdtSegments::iterator it = m_pStau->newMdtSegments()->begin(); it != m_pStau->newMdtSegments()->end(); it++)
        {
	    const Muon::MuonSegment* pSegment = *it;
            for(unsigned int i=0; i<pSegment->numberOfContainedROTs(); i++)
            {
		const Trk::RIO_OnTrack* pRio = pSegment->rioOnTrack(i);
                const Muon::MdtDriftCircleOnTrack* pMdtRio = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(pRio);
                pMdtHits->push_back(pMdtRio);
            }
	}
    }
    
}

void MuGirlNS::StauGF::prepareForGlobalFit(MuGirlNS::MuonSegmentList& muonSegments)
{
    muonSegments.clear();
    
    for(MuonSegmentList::const_iterator it = m_muonSegments.begin(); it != m_muonSegments.end(); it++)
    {
        if (m_pStau->getSegmentManager()->isNotMDT(*it)) muonSegments.push_back(*it);
    }
    for(std::vector<const Muon::MuonSegment*>::const_iterator it = m_pStau->newMdtSegments()->begin(); it != m_pStau->newMdtSegments()->end(); it++)
    {
        muonSegments.push_back(*it);
    }
}

void MuGirlNS::StauGF::clear()
{
   delete m_initialTrackParameters;
}

const Trk::Track* MuGirlNS::StauGF::refitTrack()
{
    MuGirlNS::MuonSegmentList muonSegments;
    prepareForGlobalFit(muonSegments);

    if(m_initialTrackParameters != NULL)
    {
        if(m_initialTrackParameters->parameters()[Trk::d0] == 0 || m_initialTrackParameters->parameters()[Trk::z0] == 0 || fabs(m_initialTrackParameters->parameters()[Trk::d0]) > 200. || fabs(m_initialTrackParameters->parameters()[Trk::z0]) > 500. )
             return NULL;
        else
            return m_pStau->globalFitTool()->globalFit(muonSegments, m_initialTrackParameters, m_pStau->doStauNtuple());
    }else 
        return m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), muonSegments, m_pStau->doStauNtuple());
}

double MuGirlNS::StauGF::stauMomentum()
{
    double p = 0.;
    const Trk::Track* pTrack = m_pStau->refittedTrack();
    
    if(NULL!=pTrack)
    {
        double px = (double)pTrack->perigeeParameters()->momentum()[Trk::px];
        double py = (double)pTrack->perigeeParameters()->momentum()[Trk::py];
        double pz = (double)pTrack->perigeeParameters()->momentum()[Trk::pz];
        p = sqrt(px*px+py*py+pz*pz);
    }
    return p;
}


void MuGirlNS::StauGF::processGlobalFitWithTTrack(double /* tTrack */, GlobalFitData* gfData, double /*beta*/)
{
	RIO_OnTrackLists* pTriggerHitsInSegments = new RIO_OnTrackLists();
    	std::vector<const Muon::MdtDriftCircleOnTrack*>* pMdtHits = new std::vector<const Muon::MdtDriftCircleOnTrack*>;
   
/*    prepareForGlobalFit(pTriggerHitsInSegments, pMdtHits);
    const Trk::Track* pTrack = m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), pTriggerHitsInSegments, pMdtHits, true);*/
    
	MuGirlNS::MuonSegmentList muonSegments;
	prepareForGlobalFit(muonSegments);
	std::cout << "E123: " << __FILE__ << " " << __LINE__ <<  " muonSegments.size() = "<< muonSegments.size() << std::endl;
	
	const Trk::Track* pTrack = m_pStau->globalFitTool()->globalFit(m_pStau->idTrack(), muonSegments, false);
	std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << std::endl;
	if(NULL!=pTrack)
	{
		gfData->chi2 = pTrack->fitQuality()->chiSquared();
		gfData->dof = pTrack->fitQuality()->numberDoF();
		gfData->hitsOnTrk = (double)pTrack->measurementsOnTrack()->size();
		gfData->outliersOnTrk = (double)pTrack->outliersOnTrack()->size();
		
		double px = (double)pTrack->perigeeParameters()->momentum()[Trk::px];
		double py = (double)pTrack->perigeeParameters()->momentum()[Trk::py];
		double pz = (double)pTrack->perigeeParameters()->momentum()[Trk::pz];
		double p = sqrt(px*px+py*py+pz*pz);
		gfData->p = p;

	// Here print what you've got
	//std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << std::endl;
      	const DataVector<const Trk::TrackStateOnSurface>* pTSoSs = pTrack->trackStateOnSurfaces();
	if (pTSoSs == NULL) {return; std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << std::endl; } // (false);
		for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it = pTSoSs->begin();
	        it != pTSoSs->end(); ++it)
	    {
		
		std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << std::endl;
	        const Trk::TrackStateOnSurface* pTSoS = *it;
		//std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << " "  << *pTSoS <<  std::endl;
	        const Trk::MeasurementBase* hit = pTSoS->measurementOnTrack();
	        if (hit == NULL) { std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << std::endl; continue; }
	        const Muon::MdtDriftCircleOnTrack* mdcot = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(hit);
	        if (mdcot == NULL) {  std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << " mdcot == NULL" << std::endl;continue;}
	        if (pTSoS->type(Trk::TrackStateOnSurface::Outlier)) {  std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << " mdcot == NULL" << std::endl; continue;}
	        std::cout<< "E123: "<<__FILE__ << " " << __LINE__ << std::endl;
	        std::cout << "E123: " << pTSoS->trackParameters()->parameters()[Trk::locR] << std::endl;
	   }
	
	
	}else
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
    m_log << MSG::VERBOSE << "processGlobalFitWithBeta() - done" << endreq;
}

void MuGirlNS::StauGF::setInitialTrackParameters(const Trk::Perigee* trk0)
{
    m_initialTrackParameters  = trk0;
}

