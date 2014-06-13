/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauTRT.h"
#include <iostream>

namespace MuGirlNS
{

StauTRT::StauTRT(StauTool* pStau, MsgStream& log, const Trk::Track* track, double /*beta*/ )
:m_stauTool(pStau), 
 m_log(log),
 m_track(track),
 m_drifttool("TRT_DriftFunctionTool"),
 m_trtconddbsvc("TRT_CalDbSvc",""),
 m_sg("StoreGateSvc",""),
 m_tracksName("CombinedInDetTracks"),
 m_tracksCollection(0),
 m_residual(0),
 m_error(0),
 m_ePhase(0),
 m_nHits(0), 
 m_beta(-2),
 m_minChi2(123456789.)
{

	m_track = track;
	int nHits = 0;
	//double error = 0;
	//double stdev= 0; 
        m_nHits = nHits; 
	TRTCond::RtRelation const *rtr = 0; //new TRTCond::BasicRtRelation(data);
	const Trk::Perigee* perigee = track->perigeeParameters(); 
	if (perigee == NULL)
	{
		std::cout << "StauTRT: cannot procede withouth perigeeParameters" << std::endl;
	}
   	StatusCode sc = m_sg->retrieve( m_tracksCollection, m_tracksName );
	if (!m_tracksCollection)
	{
	 	std::cout << "StauTRT: cannot procede withouth tracksCollection" << std::endl;
	}

	TrackCollection::const_iterator i_trk = m_tracksCollection->begin();
	for ( ; i_trk != m_tracksCollection->end(); ++i_trk ) 
	{

		const Trk::Perigee* i_perigee = (*i_trk)->perigeeParameters();
		if (!i_perigee) continue;
		if (fabs(i_perigee->pT() - perigee->pT()) > 10 ||
		    fabs(i_perigee->eta() - perigee->eta()) > 0.01 ||
		    fabs(i_perigee->parameters()[Trk::phi] -perigee->parameters()[Trk::phi]) > 0.01 ) continue;  // it should be the SAME track!!
	
                for (Trk::TrackStateOnSurface const* state : *(*i_trk)->trackStateOnSurfaces())
                {
			Trk::MeasurementBase const * mesb = state->measurementOnTrack();
			if (  !mesb || !state->type(Trk::TrackStateOnSurface::Measurement) 
				|| state->type(Trk::TrackStateOnSurface::Outlier)) continue;
			
			InDet::TRT_DriftCircleOnTrack const * trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const *>(mesb);
		
			if ( !trtcirc ) continue;
			
			InDet::TRT_DriftCircle const * rawhit = trtcirc->prepRawData();
			if ( !rawhit ) continue;
			if ( !rawhit->driftTimeValid() || rawhit->firstBinHigh() ) continue;
			Identifier const & ident = trtcirc->identify();
			double rawdrifttime = rawhit->rawDriftTime();
			
			double t0 = m_trtconddbsvc->getT0(ident);
			
			rtr = m_trtconddbsvc->getRtRelation(ident) ;
			
			Trk::TrackParameters const * tparp=state->trackParameters();
			if ( !tparp ) continue;
			double trkdistance  = tparp->parameters()[Trk::driftRadius];
			double trkdrifttime = rtr->drifttime( fabs(trkdistance) ) ;
			if ( fabs(trkdistance) > 2.9 || fabs(trkdrifttime) > 100 ) continue; 
			TRTHit trthit(trkdistance, trkdrifttime, rawdrifttime, t0, TRTRESOLUTION, 
					tparp->position().x(), tparp->position().y(), tparp->position().z(),
					 rawhit->detectorElement()->center().x(),  rawhit->detectorElement()->center().y(),  rawhit->detectorElement()->center().z());
			m_trthits.push_back( trthit );
                        double mToF = tparp->position().mag()/CLHEP::c_light;
                        StauHit hit(TRT_TECH, rawdrifttime-t0+mToF-trkdrifttime ,tparp->position().x(), tparp->position().y(), tparp->position().z(),ident,-1);
                        m_hits.push_back(hit);
		}

	}
        m_nHits = m_hits.size();
	//preProcess();
}
StauTRT::~StauTRT(){}
void StauTRT::initStepData(TrtStepData* /*pTrtStepData*/, double /*beta*/, double /*tTrack*/){}
void StauTRT::clearStepData(TrtStepData* /*pTrtStepData*/){}
void StauTRT::printStepData(TrtStepData* pTrtStepData)
{
    if(NULL==pTrtStepData) return;
   	m_log<<MSG::VERBOSE << "trt data: beta=" << pTrtStepData->beta
              << " chi2=" << pTrtStepData->chi2
              << " error=" << pTrtStepData->error 
	      << " nHits=" << pTrtStepData->nHits << endreq;
}
void StauTRT::processTrtWithBeta(double currentBeta, TrtStepData* pTrtStepData)
{
    if(NULL == pTrtStepData) return;	
	
    double nHits = m_nHits;
    double error  = TRTRESOLUTION / sqrt(nHits);
    double chi2 = 0;
    std::vector<TRTHit>::iterator i_hit = m_trthits.begin();
    for ( ; i_hit != m_trthits.end(); ++i_hit)
    {
        chi2 += ( i_hit->timeResidual(currentBeta)  * i_hit->timeResidual(currentBeta) / (i_hit->error * i_hit->error) ) ;
    }
    pTrtStepData->tTrack = 0.0;
    pTrtStepData->chi2 = chi2;
    pTrtStepData->beta = currentBeta;
    pTrtStepData->error = error;
    pTrtStepData->nHits = nHits;
    pTrtStepData->dof = nHits;
    printStepData(pTrtStepData);
	
    if (m_minChi2 > chi2){m_minChi2 = chi2; m_dofAtMinChi2 = nHits;}
}
void StauTRT::clear(){}
void StauTRT::processTrtWithTTrack(double tTrack, TrtStepData* pTrtStepData,double beta)
{ 
	double nHits = m_nHits;
	
	double error  = TRTRESOLUTION / sqrt(nHits);
	double chi2 = (m_residual - tTrack )* ( m_residual - tTrack) /error /error ; //(nHits > 10) ?  average / error / error : 0 ;
	pTrtStepData->tTrack = tTrack;
	pTrtStepData->chi2 = chi2;
	pTrtStepData->beta = beta;
	//std::cout << "E123: chi2 = " <<  pTrtStepData->chi2  << std::endl;
	pTrtStepData->error = error;
	pTrtStepData->nHits = nHits;
        pTrtStepData->dof = nHits;
	printStepData(pTrtStepData);
}
bool StauTRT::hasHits()
{
	return m_nHits > 0;
}
void StauTRT::setAverageTTrack(double tTrack){ m_ePhase = tTrack; }
double StauTRT::averageTTrack(){return m_ePhase;}
void StauTRT::preProcess( bool cosmics)
{
    int maxIter = 5;
    double window = 15.0;
    double sumResidual = 0.; 
    double nHits = 0.;
    double average = 0;
    if (m_trthits.size() == 0 ) return;
    for (int i_iter = 0; i_iter < maxIter; ++i_iter )
    {
        nHits = 0.0;
        sumResidual = 0.0;
        std::vector<TRTHit>::iterator i_hit  =  m_trthits.begin();
        for(; i_hit != m_trthits.end(); ++i_hit)
        {
            double beta = (!cosmics) ? 1 : ( (i_hit->globalPosition.Y() > 0) ? -1.0 : 1.0 ) ;
            double residual = i_hit->timeResidual(beta, 0); 
            if (i_iter == 0 || (average - window < residual && residual < average + window ))
            {
                sumResidual += residual;
                nHits++;
            }
        }
        average = sumResidual / nHits;	
    }
    m_nHits = nHits;
    m_residual = average;
}
void StauTRT::setBeta(double beta)
{
	m_beta = beta;
}
double StauTRT::beta()
{
	return m_beta;
}

double StauTRT::dofAtMinChi2()
{
	return m_dofAtMinChi2;
}

void StauTRT::setDofAtMinChi2(double dof)
{
	m_dofAtMinChi2 = dof;
}

double StauTRT::minChi2()
{
	return m_minChi2;
}

void StauTRT::setMinChi2(double chi2)
{
	m_minChi2 = chi2;
}
}

