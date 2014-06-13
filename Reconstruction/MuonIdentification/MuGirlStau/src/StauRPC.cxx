/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauRPC.h"
#include "MuGirlStau/StauCalibration.h"
#include "TRandom3.h"

//================ Constructor =================================================

MuGirlNS::StauRPC::StauRPC(StauTool* pStauTool, MsgStream& log, const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments):
    m_pStau(pStauTool),
    m_log(log),
    m_rpcHitsByStations(NULL),
    m_beta(StauBetaDefault),
    m_avgBeta(StauBetaDefault),
    m_rmsBeta(-1.),
    m_hasHits(false)
{
    m_pMuonMgr = pStauTool->muonMgr();
    m_addMuToF = pStauTool->addMuToF();
    
    double bugFix = (m_pStau->rpcBugFix() ? 2*3.125 : 0. ); //reduce half or 1.5 ticks from the time, depend on the rpc bug
    double timeShift = m_pStau->rpcTimeShift();

    m_rpcHitsByStations = new StauRpcHitsLists();
 
    if(m_pStau->doCalibration()) m_pCalibration = m_pStau->calibration().getRpcCalibration();
    if(m_pStau->doCalibration()) m_pNewCalibration = m_pStau->calibration().getNewRpcCalibration();

    const RpcIdHelper* pIdHelper = m_pMuonMgr->rpcIdHelper();// new RpcIdHelper();
    int numOfHits = 0;
    
    for(RIO_OnTrackLists::const_iterator it1 = rpcHitsInSegments.begin(); it1 != rpcHitsInSegments.end(); it1++)
    {
        StauRpcHitsList* pRpcHits = new StauRpcHitsList();
        RIO_OnTrackList* pRioList = *it1;
        for(RIO_OnTrackList::const_iterator it = pRioList->begin(); it!=pRioList->end(); it++)
        {
            const Trk::RIO_OnTrack* pRpcRIO = *it;
            const Trk::PrepRawData* pPrepData = pRpcRIO->prepRawData();
            const Muon::RpcPrepData* pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepData);
            bool b_isEta = !pIdHelper->measuresPhi(pRpcRIO->identify());
	    Amg::Vector3D pos = pRpcRIO->globalPosition();

            double shift = 0;
            double error = RPCRESOLUTION; // default time resolution 
            double distance = fabs(pos.perp())/1000; //[m]
            double muonToF = distance/SPEEDOFLIGHT;
            double measuredTime = pRpcPrepData->time() + 0.5*3.125 - timeShift - bugFix ;
            if(m_pStau->doCalibration())
            {//Use external calibration files
                if (m_pStau->isData())
                {
                    int id = (pRpcRIO->identify()).get_compact();
                    std::map<int, StauCalibrationParameters >::iterator itNewCalib = m_pNewCalibration->find(id);
                    if(itNewCalib == m_pNewCalibration->end()) continue;
                    
                    shift = itNewCalib->second.timeShift - m_pStau->phaseShift();//shift
                    measuredTime += timeShift - 0.5*3.125 - shift;
                }                    
                else 
                { //smear
                      
                   double r = sqrt(pos.x()*pos.x() + pos.y()*pos.y());
                   StauRpcCalibRange range = rpcCalibRange(r, b_isEta);
                   std::map<int, StauCalibrationParameters >::iterator itCalib = m_pCalibration->find(range);
                   if(itCalib == m_pCalibration->end()) continue;
                   error = itCalib->second.error;
                   TRandom3 rand(0);
                   shift = m_pStau->rpcSmearFactor() *  rand.Gaus(0,error); //Low 1.5 Mid 2 High 2.5 
                   measuredTime -=  shift;
                }
            }
            StauRpcHit* pRpcHit = new StauRpcHit();

            if(m_addMuToF)
            {
                //the time is calibrated such that the muon tof is reduced. so add it
                measuredTime += muonToF;
            }


    // moved up        const RpcIdHelper* pIdHelper = m_pMuonMgr->rpcIdHelper();// new RpcIdHelper();
    // moved up       bool b_isEta = !pIdHelper->measuresPhi(pRpcRIO->identify());

            const MuonGM::RpcReadoutElement* pROElement = m_pMuonMgr->getRpcReadoutElement(pRpcRIO->identify());
            double d_propagation = 0;
            if(b_isEta) d_propagation = pROElement->distanceToEtaReadout(pos,pRpcRIO->identify());
            else d_propagation = pROElement->distanceToPhiReadout(pos,pRpcRIO->identify());

            double propTime = (fabs(d_propagation)/1000)*RPCINVSIGVEL;

            pRpcHit->id = pRpcRIO->identify();
	    pRpcHit->distance = distance;
            pRpcHit->propagationTime = propTime;
            pRpcHit->measuredTime = measuredTime;
	    pRpcHit->muonToF = muonToF;
            pRpcHit->error = error;
            pRpcHit->x = pos.x();
            pRpcHit->y = pos.y();
            pRpcHit->z = pos.z();
            pRpcHit->isEta = b_isEta;
	    
            pRpcHits->push_back(pRpcHit);
            numOfHits++;

            StauHit hit(RPC_TECH,measuredTime-propTime,pos.x(),pos.y(),pos.z(),pRpcRIO->identify(),-1, error, shift, b_isEta);
            m_hits.push_back(hit);
        } 
	m_rpcHitsByStations->push_back(pRpcHits);
    }

//    pStauTool->setRpcHitsLists(m_rpcHitsByStations);

    if(numOfHits>0)
    {
       m_hasHits = true;
       averageBeta();
    }
}

//================ Destructor =================================================

MuGirlNS::StauRPC::~StauRPC()
{}

void MuGirlNS::StauRPC::init()
{

}

void MuGirlNS::StauRPC::clear()
{
    if(/*!m_pStau->hasSummary() && */NULL!=m_rpcHitsByStations)
    {
        for(StauRpcHitsLists::iterator it = m_rpcHitsByStations->begin(); it != m_rpcHitsByStations->end(); it++)
	{
	    StauRpcHitsList* pRpcHits = *it;
            for(StauRpcHitsList::iterator it1 = pRpcHits->begin(); it1 != pRpcHits->end(); it1++)
            {
                delete *it1;
            }
	    
            pRpcHits->clear();
	    delete pRpcHits;
	}

	m_rpcHitsByStations->clear();
	delete m_rpcHitsByStations;
        m_rpcHitsByStations = NULL;
    }
    m_beta = StauBetaDefault;
    m_avgBeta = StauBetaDefault;
    m_rmsBeta = -1;
    m_hasHits = false; 
    m_hits.clear(); 
}

void MuGirlNS::StauRPC::initStepData(RpcStepData* rpcData, double beta, double tTrack)
{
    rpcData->pStationDataList = new std::vector< RpcStepStationData* >;
    rpcData->beta = beta;
    rpcData->chi2 = 0.;
    rpcData->dof = 0.;
    rpcData->totNumHits = 0.;
    rpcData->tTrack = tTrack;
}

void MuGirlNS::StauRPC::clearStepData(RpcStepData* rpcData)
{
    if(NULL==rpcData) return;
    std::vector< RpcStepStationData* >* pRpcStepStationsData = rpcData->pStationDataList;
    if(NULL!=pRpcStepStationsData)
    {
        for(std::vector< RpcStepStationData* >::iterator it = pRpcStepStationsData->begin(); it != pRpcStepStationsData->end(); it++)
        {
            delete *it;
        }
        pRpcStepStationsData->clear();
        delete pRpcStepStationsData;
    }
    delete rpcData;
    rpcData = NULL;
}

void MuGirlNS::StauRPC::printStepData(RpcStepData* rpcData)
{
    if(NULL==rpcData) return;
    m_log<<MSG::VERBOSE << "rpc data: beta=" << rpcData->beta
              << " chi2=" << rpcData->chi2
              << " dof=" << rpcData->totNumHits << endreq;
        std::vector< RpcStepStationData* >* pRpcStationDataList = rpcData->pStationDataList;

    int j=0;
    if(NULL!=pRpcStationDataList) 
    {
        for(std::vector< RpcStepStationData* >::iterator it1 = pRpcStationDataList->begin(); it1 != pRpcStationDataList->end(); it1++)
        {
            j++;
            RpcStepStationData* rpcStepStation = *it1;
            m_log<<MSG::VERBOSE << "       station #" << j //<< ": distance=" << rpcStepStation->distance
                      << " chi2=" << rpcStepStation->chi2
                      << " #hits=" << rpcStepStation->numHits << endreq;
        }
    }
}

void MuGirlNS::StauRPC::processRpcWithBeta(double currentBeta, RpcStepData* pRpcStepData)
{
    m_log << MSG::VERBOSE << "processRpcWithBeta( beta=" << currentBeta << " )" << endreq;
    
    pRpcStepData->beta = currentBeta;
    pRpcStepData->chi2 = 0.;
    pRpcStepData->totNumHits = 0.;
    
    for(StauRpcHitsLists::iterator it = m_rpcHitsByStations->begin(); it != m_rpcHitsByStations->end(); it++)
    {
        StauRpcHitsList* pHitsList = *it;
        RpcStepStationData* pRpcStepStationsData = new RpcStepStationData();
        int dof = 0;
        double stationChi2 = rpcStationChi2(currentBeta, pHitsList, dof)/*/pRioList->size()*/;
        pRpcStepStationsData->numHits = dof; 
        pRpcStepStationsData->chi2 = stationChi2;
        pRpcStepData->pStationDataList->push_back(pRpcStepStationsData);
        pRpcStepData->chi2 += stationChi2;
        pRpcStepData->totNumHits += dof;
    }
    pRpcStepData->dof = pRpcStepData->totNumHits;    
 
    printStepData(pRpcStepData);
    m_log << MSG::VERBOSE << "processRpcWithBeta() - done" << endreq;
}

double MuGirlNS::StauRPC::rpcStationChi2(double currentBeta, StauRpcHitsList* pHitsList, int& dof, double tTrack)
{
    double chi2 = 0.;
    double distance;
    double measuredTime;
    double propTime;
    double calculatedToF;
    double stauV;
    double stauToF;
    double error;
    for(StauRpcHitsList::iterator it = pHitsList->begin(); it!=pHitsList->end(); it++)
    {
        StauRpcHit* pHit = *it;
	
        distance = pHit->distance; //[m]
        measuredTime = pHit->measuredTime;
        propTime = pHit->propagationTime;
        calculatedToF = measuredTime - propTime;
        stauV = currentBeta*SPEEDOFLIGHT;
        stauToF = distance/stauV + tTrack; // [ns]
	error = pHit->error;

        dof++;
        chi2 += (calculatedToF-stauToF)*(calculatedToF-stauToF)/(error*error);
    }

    return chi2;
}

void MuGirlNS::StauRPC::averageBeta()
{
    //Average
    double up = 0;
    double dwn = 0;
    for(StauRpcHitsLists::iterator it = m_rpcHitsByStations->begin(); it != m_rpcHitsByStations->end(); it++)
    {
        StauRpcHitsList* pRpcHits = *it;
        for(StauRpcHitsList::iterator it1 = pRpcHits->begin(); it1!=pRpcHits->end(); it1++)
        {
            StauRpcHit* pRpcHit = *it1;
            double measuredToF = pRpcHit->measuredTime - pRpcHit->propagationTime;
            double distance = pRpcHit->distance;
            //double measuredBeta = distance / (measureToF * SPEEDOFLIGHT);
            double invBeta = measuredToF * SPEEDOFLIGHT / distance;
	    double tofError = pRpcHit->error;
            double invBetaError = tofError * SPEEDOFLIGHT / distance;
            //double measuredBetaError = (distance / SPEEDOFLIGHT) * (1 / (measureToF * measureToF)) * tofError;
            up += ( invBeta / (invBetaError * invBetaError) );
            dwn += ( 1. / (invBetaError * invBetaError) );
        }
    }
    double avgInvBeta = ( 0 != dwn) ?  up/dwn : 1./StauBetaDefault;
    m_avgBeta = 1./ avgInvBeta; 

    //RMS
    up = 0;
    dwn = 0;
    for(StauRpcHitsLists::iterator it = m_rpcHitsByStations->begin(); it != m_rpcHitsByStations->end(); it++)
    {
        StauRpcHitsList* pRpcHits = *it;
        for(StauRpcHitsList::iterator it1 = pRpcHits->begin(); it1!=pRpcHits->end(); it1++)
        {
            StauRpcHit* pRpcHit = *it1;
            double measureToF = pRpcHit->measuredTime - pRpcHit->propagationTime;
            double distance = pRpcHit->distance;
            //double measuredBeta = distance / (measureToF * SPEEDOFLIGHT);
            double invBeta = SPEEDOFLIGHT * measureToF / distance;
            double deltaInvBeta = invBeta - avgInvBeta;
            double tofError = pRpcHit->error;
            double invBetaError = tofError * SPEEDOFLIGHT / distance;
    //        double measuredBetaError = (distance / SPEEDOFLIGHT) * (1. / (measureToF * measureToF)) * tofError;
            up +=   deltaInvBeta *  deltaInvBeta / (invBetaError * invBetaError);
            dwn += ( 1. / (invBetaError * invBetaError) );
        }
    }  
    double rmsInvBeta = (0 != dwn) ? sqrt(up/dwn) : -1;
    m_rmsBeta = (0 != dwn) ?  m_avgBeta * m_avgBeta * rmsInvBeta : -1;
}

double MuGirlNS::StauRPC::betaMinTime()
{
    double avgMinBeta = 0;
    
    for(StauRpcHitsLists::iterator it = m_rpcHitsByStations->begin(); it != m_rpcHitsByStations->end(); it++)
    {
        double minTime = 10000000;
	double minDistance = 1000;
	double minPropTime = 1000;
	
        StauRpcHitsList* pHitsList = *it;
        for(StauRpcHitsList::iterator it1 = pHitsList->begin(); it1!=pHitsList->end(); it1++)
        {
            StauRpcHit* pHit = *it1;
	    if(pHit->measuredTime < minTime)
	    {
	        minTime = pHit->measuredTime;
		minDistance = pHit->distance;
		minPropTime = pHit->propagationTime;
	    }
	}
	avgMinBeta += ( minDistance / (minTime - minPropTime) ) / SPEEDOFLIGHT;
    }
    
    return (m_rpcHitsByStations->size() == 0 ? -1 : avgMinBeta/m_rpcHitsByStations->size());
}


unsigned int MuGirlNS::StauRPC::rpcMinIsAt()
{
	// maybe this can be pre-calculated while function is being evaluated
    FcnStepsData firstSteps = *(m_pStau->fcnStepsData());
    unsigned int iMinLoc = 0;
    double minChi2 = 100000.;
    
    if(!(firstSteps[0]->isBarrel)) return 0;
    
    for(unsigned int i=0; i<firstSteps.size(); i++)
    {
        FcnStepData* pStepData = firstSteps[i];
	RpcStepData* pRpcStepData = pStepData->rpcData;
	double chi2 = pRpcStepData->chi2;
	
	if(chi2<minChi2)
	{
	    minChi2 = chi2;
	    iMinLoc = i;
	}
    }
    
    return iMinLoc;
}


void MuGirlNS::StauRPC::processRpcWithTTrack(double tTrack, RpcStepData* pRpcStepData,double beta)
{
	   m_log << MSG::VERBOSE << "processRpcWithTTrack( tTrack=" << tTrack << " )" << endreq;
	    //double currentBeta = 1.0;
	    pRpcStepData->beta = beta;
	    pRpcStepData->tTrack = tTrack;
	    pRpcStepData->chi2 = 0.;
	    pRpcStepData->totNumHits = 0.;
	    
	    for(StauRpcHitsLists::iterator it = m_rpcHitsByStations->begin(); it != m_rpcHitsByStations->end(); it++)
	    {
	        StauRpcHitsList* pHitsList = *it;
		RpcStepStationData* pRpcStepStationsData = new RpcStepStationData();
		int dof = 0;
		double stationChi2 = rpcStationChi2(beta, pHitsList, dof, tTrack)/*/pRioList->size()*/;
		pRpcStepStationsData->numHits = dof; 
		pRpcStepStationsData->chi2 = stationChi2;
		pRpcStepData->pStationDataList->push_back(pRpcStepStationsData);
		pRpcStepData->chi2 += stationChi2;
		pRpcStepData->totNumHits += dof;
	    }
             pRpcStepData->dof =  pRpcStepData->totNumHits;
	     
	    printStepData(pRpcStepData);
	    m_log << MSG::VERBOSE << "processRpcWithTTrack() - done" << endreq;
}



MuGirlNS::StauRpcHitsLists* MuGirlNS::StauRPC::copyStauRpcHitsLists()
{
    if(m_rpcHitsByStations == NULL) return NULL;
    StauRpcHitsLists* newRpcHitsByStation = new StauRpcHitsLists();
    for(StauRpcHitsLists::iterator it = m_rpcHitsByStations->begin(); it != m_rpcHitsByStations->end(); it++)
    {
        StauRpcHitsList* pHitsList = *it; 
        StauRpcHitsList* pCopyList = new StauRpcHitsList();
        newRpcHitsByStation->push_back(pCopyList);
        for(StauRpcHitsList::iterator it2 = pHitsList->begin(); it2!=pHitsList->end(); it2++)
        {
            StauRpcHit* pHit = *it2;
            StauRpcHit* pCopyHit = new StauRpcHit();
            pCopyList->push_back(pCopyHit);
         
            pCopyHit->id = pHit->id;
            pCopyHit->distance = pHit->distance;
            pCopyHit->propagationTime = pHit->propagationTime;
            pCopyHit->measuredTime = pHit->measuredTime;
            pCopyHit->muonToF = pHit->muonToF;
            pCopyHit->error = pHit->error;
            pCopyHit->x = pHit->x;
            pCopyHit->y = pHit->y;
            pCopyHit->z = pHit->z;
            pCopyHit->isEta = pHit->isEta;
        }
    }
    return newRpcHitsByStation;
}
