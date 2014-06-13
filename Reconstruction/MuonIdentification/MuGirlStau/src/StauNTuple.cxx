/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauTool.h"
#include "MuGirlStau/StauNTuple.h"
#include "MuGirlStau/StauRPC.h"
#include "MuGirlStau/StauMDT.h"
#include "MuGirlStau/StauTileCal.h"


MuGirlNS::StauNTuple::StauNTuple(MsgStream& log) :
    m_log(log),
    m_maxSteps(300), //30
    m_maxStationNsteps(800), //130
    m_maxRpcHits(100)
{
}

StatusCode MuGirlNS::StauNTuple::book(NTuple::Tuple* pNTuple)
{
    m_log << MSG::INFO << "StauNTuple initialize" << endreq;
    
    if( pNTuple->addItem("MUGIRL/StauBeta"	,	m_beta).isFailure() ||
        pNTuple->addItem("MUGIRL/StauMass"	,	m_mass).isFailure() ||
	pNTuple->addItem("MUGIRL/StauTZero"	,	m_tzero).isFailure() ||
	pNTuple->addItem("MUGIRL/StauBetaRpc"	,	m_betaRpc).isFailure() ||
	pNTuple->addItem("MUGIRL/StauBetaMdt"	,	m_betaMdt).isFailure() ||
	pNTuple->addItem("MUGIRL/StauBetaTileCal",      m_betaTileCal).isFailure() ||
	pNTuple->addItem("MUGIRL/StauBetaMS"	,	m_betaMS).isFailure() ||
	pNTuple->addItem("MUGIRL/StauBetaNoRpc"	,	m_betaNoRpc).isFailure() ||
	pNTuple->addItem("MUGIRL/StauBetaMdtt_mu"	,	m_betaMdtt_mu).isFailure() ||
	pNTuple->addItem("MUGIRL/StauBetaMdtt_sp"	,	m_betaMdtt_sp).isFailure() ||
	pNTuple->addItem("MUGIRL/StauBetaAll"	,	m_betaAll).isFailure() ||
        pNTuple->addItem("MUGIRL/StauBetaAvgRpc",       m_betaAvgRpc).isFailure() ||
        pNTuple->addItem("MUGIRL/StauBetaAvgTileCal",   m_betaAvgTileCal).isFailure() ||
	pNTuple->addItem("MUGIRL/StauIsBarrel"  ,	m_fcnType).isFailure())
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<stau summary>" << endreq;
        return StatusCode::RECOVERABLE;
    }
    
    if(pNTuple->addItem("MUGIRL/StauSteps", m_steps,0,m_maxSteps).isFailure()||
       
       pNTuple->addItem("MUGIRL/StauStepBeta", m_steps,m_stepBeta).isFailure()||
       pNTuple->addItem("MUGIRL/StauStepChi2", m_steps,m_stepChi2).isFailure()||
       pNTuple->addItem("MUGIRL/StauStepDoF",  m_steps,m_stepDoF).isFailure()||
       
       pNTuple->addItem("MUGIRL/StauMdtStepChi2", m_steps,m_mdtStepChi2).isFailure()||
       pNTuple->addItem("MUGIRL/StauMdtStepHits", m_steps,m_mdtTotNumHits).isFailure()||
       pNTuple->addItem("MUGIRL/StauMdtStepSegs", m_steps,m_mdtNumSegs).isFailure()||
      
       pNTuple->addItem("MUGIRL/StauRpcStepChi2", m_steps,m_rpcStepChi2).isFailure()||
       pNTuple->addItem("MUGIRL/StauRpcStepHits", m_steps,m_rpcTotNumHits).isFailure()||
 
       pNTuple->addItem("MUGIRL/StauTileCalStepChi2",  m_steps,m_tileCalStepChi2).isFailure()||
       pNTuple->addItem("MUGIRL/StauTileCalStepCells", m_steps,m_tileCalNumCells).isFailure()||
      
       pNTuple->addItem("MUGIRL/StauGFStepChi2",  m_steps,m_gfStepChi2).isFailure() ||
       pNTuple->addItem("MUGIRL/StauGFStepDoF",  m_steps, m_gfStepDoF).isFailure() ||
       pNTuple->addItem("MUGIRL/StauGFStepOutliers",  m_steps, m_gfOutliers).isFailure() ||
       pNTuple->addItem("MUGIRL/StauGFStepHitsOnTrk",  m_steps, m_gfHitsOnTrk).isFailure() ||
       pNTuple->addItem("MUGIRL/StauGFStepP",  m_steps, m_gfP).isFailure() ||
       
       pNTuple->addItem("MUGIRL/StauMdtStationNSteps",m_mdtStationNsteps,0,m_maxStationNsteps).isFailure()|| 
       
       pNTuple->addItem("MUGIRL/StauMdtStationBeta", m_mdtStationNsteps,m_mdtStepStationBeta).isFailure()||
       pNTuple->addItem("MUGIRL/StauMdtStationChi2", m_mdtStationNsteps,m_mdtStepStationChi2).isFailure()||
       pNTuple->addItem("MUGIRL/StauMdtStationNumHits", m_mdtStationNsteps,m_mdtStationNumHits).isFailure()||
       pNTuple->addItem("MUGIRL/StauMdtStationNumSegs", m_mdtStationNsteps,m_mdtStationNumSegs).isFailure()||	
       pNTuple->addItem("MUGIRL/StauMdtStationDist", m_mdtStationNsteps,m_mdtStationDistance).isFailure() ||
       pNTuple->addItem("MUGIRL/StauMdtStationEta", m_mdtStationNsteps,m_mdtStationSegEta).isFailure() ||
       pNTuple->addItem("MUGIRL/StauMdtStationPhi", m_mdtStationNsteps,m_mdtStationSegPhi).isFailure() ||
       
       pNTuple->addItem("MUGIRL/StauRpcStationNSteps", m_rpcStationNsteps,0,m_maxStationNsteps).isFailure()||
       
       pNTuple->addItem("MUGIRL/StauRpcStationBeta", m_rpcStationNsteps,m_rpcStepStationBeta).isFailure()||
       pNTuple->addItem("MUGIRL/StauRpcStationChi2", m_rpcStationNsteps,m_rpcStepStationChi2).isFailure()||
       pNTuple->addItem("MUGIRL/StauRpcStationNumHits", m_rpcStationNsteps,m_rpcStationNumHits).isFailure()||

       pNTuple->addItem("MUGIRL/StauRpcHits", m_rpcHits,0,m_maxRpcHits).isFailure()||
       pNTuple->addItem("MUGIRL/StauRpcHitsDistance", m_rpcHits,m_rpcHitDistance).isFailure()||
       pNTuple->addItem("MUGIRL/StauRpcHitsMeasuredTime", m_rpcHits,m_rpcHitMeasuredTime).isFailure()||
       pNTuple->addItem("MUGIRL/StauRpcHitsPropagationTime", m_rpcHits,m_rpcHitPropagationTime).isFailure()||
       pNTuple->addItem("MUGIRL/StauRpcX", m_rpcHits,m_rpcHitX).isFailure() ||
       pNTuple->addItem("MUGIRL/StauRpcY", m_rpcHits,m_rpcHitY).isFailure() ||
       pNTuple->addItem("MUGIRL/StauRpcZ", m_rpcHits,m_rpcHitZ).isFailure() ||
       pNTuple->addItem("MUGIRL/StauRpcHitsIsEta", m_rpcHits,m_rpcHitIsEta).isFailure())
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<stau step data>" << endreq;
        return StatusCode::RECOVERABLE;
    }
    
    
    return StatusCode::SUCCESS;
}

StatusCode MuGirlNS::StauNTuple::fillStau(StauTool* pStau)
{
    m_log << MSG::INFO << "StauNTuple::fillStau" << endreq;
     
    m_beta = (float)pStau->beta();
    m_mass = (float)pStau->mass();
    m_tzero = (float)pStau->tzero();
    m_betaRpc = ( NULL!=pStau->pStauRPC() ? (float)pStau->pStauRPC()->beta() : 0. );
    m_betaMdt = ( NULL!=pStau->pStauMDT() ? (float)pStau->pStauMDT()->beta() : 0. );
    m_betaTileCal = ( NULL!=pStau->pStauTileCal() ? (float)pStau->pStauTileCal()->beta() : 0. );
    m_betaMS = (float)pStau->techBetaChi2()[MS_TECHS].beta;
    m_betaAll = (float)pStau->techBetaChi2()[ALL_TECHS].beta;
    m_betaNoRpc = (float)pStau->techBetaChi2()[NORPC_TECHS].beta;
    m_betaMdtt_mu = (float)pStau->techBetaChi2()[MDTT_MU_TECH].beta;
    m_betaMdtt_sp = (float)pStau->techBetaChi2()[MDTT_SP_TECH].beta;

    m_betaAvgRpc = ( NULL!=pStau->pStauRPC() ? (float)pStau->pStauRPC()->avgBeta() : 0. );
    m_betaAvgTileCal = ( NULL!=pStau->pStauTileCal() ? (float)pStau->pStauTileCal()->avgBeta() : 0. );
   
//     m_inputMdtHits = (float)pStau->inputMdtHits();
//     m_outputMdtHits = (float)pStau->outputMdtHits();
//     m_baseP = (float)pStau->baseP();
//     m_basePt = (float)pStau->basePt();
//     m_baseFitChi2 = (float)pStau->baseFitChi2();
//     m_measurementSetSize = (float)pStau->measurementSetSize();
    
    StauRpcHitsLists* rpcHitsByStations = ( NULL!=pStau->pStauRPC() ? pStau->pStauRPC()->rpcHitsByStations() : NULL );
    m_rpcHits = 0;
    if(NULL!=rpcHitsByStations)
    {
        for(StauRpcHitsLists::iterator it2 = rpcHitsByStations->begin(); it2 != rpcHitsByStations->end(); it2++)
        {
    
            StauRpcHitsList* rpcHits = *it2;
	    for(StauRpcHitsList::iterator it3 = rpcHits->begin(); it3 != rpcHits->end(); it3++)
	    {
                StauRpcHit* pHit = *it3;
                m_rpcHitDistance[m_rpcHits] = pHit->distance;
                m_rpcHitMeasuredTime[m_rpcHits] = pHit->measuredTime;
                m_rpcHitPropagationTime[m_rpcHits] = pHit->propagationTime;
                m_rpcHitX[m_rpcHits] = pHit->x;
                m_rpcHitY[m_rpcHits] = pHit->y;
                m_rpcHitZ[m_rpcHits] = pHit->z;
                m_rpcHitIsEta[m_rpcHits] = pHit->isEta;
                m_rpcHits++; 
	    }
	}
    } 
    
    FcnStepsData* pFcnStepsData = pStau->fcnStepsData();
    if(NULL!=pFcnStepsData && 1<=pFcnStepsData->size()) 
    {
        FcnStepData* pStepData = *(pFcnStepsData->begin());
        if(NULL!=pStepData) m_fcnType = pStepData->isBarrel;
	m_steps = 0;
	m_mdtStationNsteps = 0;
	m_rpcStationNsteps = 0;
	for(FcnStepsData::iterator it = pFcnStepsData->begin(); it != pFcnStepsData->end(); it++)
	{
	    FcnStepData* pStepData = *it;
	    m_stepBeta[m_steps] = ((float)pStepData->beta);     /** the beta of the step   */
            m_stepChi2[m_steps] = pStepData->chi2;     /** the chi2 of the step   */
	    m_stepDoF[m_steps] = pStepData->dof;
       
	    /** Fill MDT ntuple data*/
	    MdtStepData* pMdtStepData = pStepData->mdtData;
	    if(NULL!=pMdtStepData)
	    {
	        std::vector< MdtStepStationData* >* pMdtStationDataList = pMdtStepData->pStationDataList;
	    
                m_mdtStepChi2[m_steps] = pMdtStepData->chi2;     /** the mdt contribution to the chi2   */
                m_mdtTotNumHits[m_steps] = pMdtStepData->hitsInSegments;   
                m_mdtNumSegs[m_steps] = pMdtStepData->numSegs;
	    
	        if(NULL!=pMdtStationDataList)
		{
	            for( std::vector< MdtStepStationData* >::iterator it1 = pMdtStationDataList->begin(); it1 != pMdtStationDataList->end(); it1++)
	            {
	                MdtStepStationData* pMdtStepStationData = *it1;
                        m_mdtStepStationChi2[m_mdtStationNsteps] = pMdtStepStationData->chi2;  
		        m_mdtStepStationBeta[m_mdtStationNsteps] = pStepData->beta;   
                        m_mdtStationNumHits[m_mdtStationNsteps] = pMdtStepStationData->numHits;   
                        m_mdtStationNumSegs[m_mdtStationNsteps] = pMdtStepStationData->numSegs;
                        m_mdtStationDistance[m_mdtStationNsteps] = pMdtStepStationData->distance;  
	    	        m_mdtStationSegEta[m_mdtStationNsteps] = pMdtStepStationData->dirEta;  
	    	        m_mdtStationSegPhi[m_mdtStationNsteps] = pMdtStepStationData->dirPhi;  
	    	        m_mdtStationNsteps++; 
		    }
		}
            }
	    
	    /** Fill Rpc ntuple data */
	    RpcStepData* pRpcStepData = pStepData->rpcData;
	    if(NULL!=pRpcStepData)
	    {
	        std::vector< RpcStepStationData* >* pRpcStationDataList = pRpcStepData->pStationDataList;
	    
                m_rpcStepChi2[m_steps] = pRpcStepData->chi2;     /** the rpc contribution to the chi2   */
                m_rpcTotNumHits[m_steps] = pRpcStepData->totNumHits;   
          
	        if(NULL!=pRpcStationDataList)
		{
	            for( std::vector< RpcStepStationData* >::iterator it1 = pRpcStationDataList->begin(); it1 != pRpcStationDataList->end(); it1++)
	            {
	                RpcStepStationData* pRpcStepStationData = *it1;
                        m_rpcStepStationChi2[m_rpcStationNsteps] = pRpcStepStationData->chi2;  
	   	        m_rpcStepStationBeta[m_rpcStationNsteps] = pStepData->beta;   
                        m_rpcStationNumHits[m_rpcStationNsteps] = pRpcStepStationData->numHits;   
                        m_rpcStationNsteps++; 
		    }
		}
            }
	    
	    /** Fill Tile Cal step data */
	    TileCalStepData* pTileStepData = pStepData->tileCalData;
	    if(NULL!=pTileStepData)
	    {
                m_tileCalStepChi2[m_steps] = pTileStepData->chi2;     /** the tile contribution to the chi2   */
                m_tileCalNumCells[m_steps] = pTileStepData->numOfCells;   
            }
	    /** Fill GlobalFit ntuple data */
	    GlobalFitData* pGFStepData = pStepData->globalFitData;
	    if(NULL!=pGFStepData)
	    {
                m_gfStepChi2[m_steps] = pGFStepData->chi2;     /** the GF contribution to the chi2   */
	        m_gfStepDoF[m_steps] = pGFStepData->dof;  
	        m_gfOutliers[m_steps] = pGFStepData->outliersOnTrk;  
	        m_gfHitsOnTrk[m_steps] = pGFStepData->hitsOnTrk;
	        m_gfP[m_steps] = pGFStepData->p;
	    }
	    m_steps++;
            if(m_maxStationNsteps-5<m_mdtStationNsteps || m_maxSteps-5<m_steps || m_maxRpcHits-5<m_rpcStationNsteps)
            {
                m_log << MSG::INFO << "StauNTuple::fillStau - has too many steps, stop writing them" << endreq;
                return StatusCode::SUCCESS;
            }

	}
    }else
    {
        m_log << MSG::INFO << "StauNTuple::fillStau - Got NULL list of fcn steps" << endreq;
    }
      
    return StatusCode::SUCCESS;
}

