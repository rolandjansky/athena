/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauFcn.h"
#include "MuGirlStau/StauTool.h"
#include "MuGirlStau/StauRPC.h"
#include "MuGirlStau/StauMDT.h"
#include "MuGirlStau/StauGF.h"
#include "MuGirlStau/StauTileCal.h"
#include "MuGirlStau/StauMDTT.h"
#include "MuGirlStau/StauTRT.h"


//================ Constructor =================================================

MuGirlNS::StauFcn::StauFcn(StauTool* pStauTool, MsgStream& log):
    m_pStau(pStauTool),
    m_log(log)
{
}

//================ Destructor =================================================

MuGirlNS::StauFcn::~StauFcn()
{}

//================ The goal function =================================================

double MuGirlNS::StauFcn::function(double currentBeta, StauTechnology eTech)
{
    m_log << MSG::VERBOSE << "StauFcn::function() - technology " << technologyName(eTech) << endreq;
    
    //allocate memory for the data of a new step
    FcnStepData* pFcnStep = new FcnStepData();
    pFcnStep->isBarrel = 0.;
    pFcnStep->beta = currentBeta;
    pFcnStep->chi2 = 0.;
    pFcnStep->isGlobalFit = true;
    
    
    pFcnStep->mdtData = NULL;
    pFcnStep->rpcData = NULL;
    pFcnStep->tileCalData = NULL;
    pFcnStep->tgcData = NULL;
    pFcnStep->globalFitData = NULL;
    pFcnStep->mdttData = NULL;
    
    
    double mdtStepChi2 = 0., rpcStepChi2 = 0., tileCalStepChi2 = 0., gfStepChi2 = 0., tgcStepChi2 = 0., mdttStepChi2 = 0. , trtStepChi2 = 0.;
    double mdtStepDoF = 0., rpcStepDoF = 0., tileCalStepDoF = 0., gfStepDoF = 0., tgcStepDoF = 0., mdttStepDoF = 0., trtStepDoF = 0.;
   
    bool doMdt = (eTech == MuGirlNS::AOD_TECHS || eTech == MuGirlNS::MDT_TECH || eTech == MuGirlNS::MS_TECHS || eTech == MuGirlNS::NORPC_TECHS || eTech == MuGirlNS::NOTRT_TECHS || eTech == MuGirlNS::ALL_TECHS);
    bool doMdtt = (eTech == MuGirlNS::MDTT_MU_TECH || eTech == MuGirlNS::MDTT_SP_TECH || eTech == MuGirlNS::MST_TECHS || eTech == MuGirlNS::NORPCT_TECHS || eTech == MuGirlNS::NOTRTT_TECHS || eTech == MuGirlNS::ALLT_TECHS);
    bool doRpc = ((eTech == MuGirlNS::AOD_TECHS && m_pStau->doRpc()) || eTech == MuGirlNS::RPC_TECH || eTech == MuGirlNS::MS_TECHS || eTech == MuGirlNS::MST_TECHS || eTech == MuGirlNS::NOTRT_TECHS || eTech == MuGirlNS::NOTRTT_TECHS ||  eTech == MuGirlNS::ALL_TECHS || eTech == MuGirlNS::ALLT_TECHS);
    bool doTile = (eTech == MuGirlNS::TILECAL_TECH || eTech == MuGirlNS::NORPC_TECHS || eTech == MuGirlNS::NORPCT_TECHS || eTech == MuGirlNS::NOTRT_TECHS || eTech == MuGirlNS::NOTRTT_TECHS ||  eTech == MuGirlNS::ALL_TECHS || eTech == MuGirlNS::ALLT_TECHS);
    bool doTrt = (!m_pStau->doTrt() ? false : (eTech == MuGirlNS::TRT_TECH || eTech == MuGirlNS::ALL_TECHS || eTech == MuGirlNS::ALLT_TECHS || eTech == MuGirlNS::NORPC_TECHS || eTech == MuGirlNS::NORPCT_TECHS));

    m_log << MSG::VERBOSE << "StauFcn::function() - doMdt=" << doMdt << " doMdtt=" << doMdtt << " doRpc=" << doRpc << " doTile=" << doTile << " doTrt=" << doTrt << endreq;
    
    if( doMdt )
    {
        pFcnStep->mdtData = new MdtStepData();
        m_pStau->pStauMDT()->initStepData(pFcnStep->mdtData, currentBeta);
        m_pStau->pStauMDT()->processMdtWithBeta(currentBeta, pFcnStep->mdtData);
        mdtStepChi2 = pFcnStep->mdtData->chi2;
        mdtStepDoF = pFcnStep->mdtData->dof;
    }
    
    if( doRpc )
    {
        pFcnStep->rpcData = new RpcStepData();
        m_pStau->pStauRPC()->initStepData(pFcnStep->rpcData, currentBeta);
        m_pStau->pStauRPC()->processRpcWithBeta(currentBeta, pFcnStep->rpcData);
        rpcStepChi2 = pFcnStep->rpcData->chi2;
        rpcStepDoF = pFcnStep->rpcData->dof;
    }
    
    if( !m_pStau->noIDTrack() && doTile )
    {
         pFcnStep->tileCalData = new TileCalStepData();
         m_pStau->pStauTileCal()->initStepData(pFcnStep->tileCalData, currentBeta);
	 m_pStau->pStauTileCal()->processTileCalWithBeta(currentBeta, pFcnStep->tileCalData);
	 tileCalStepChi2 = pFcnStep->tileCalData->chi2;
	 tileCalStepDoF = pFcnStep->tileCalData->dof;
    }

    if( !m_pStau->noIDTrack() && doTrt  )
    {
       	 pFcnStep->trtData = new TrtStepData();
    	 m_pStau->pStauTRT()->initStepData(pFcnStep->trtData, currentBeta, 0.0);
    	 m_pStau->pStauTRT()->processTrtWithBeta(currentBeta,pFcnStep->trtData);
    	 trtStepChi2 = pFcnStep->trtData->chi2;
    	 trtStepDoF = pFcnStep->trtData->dof;
    }
    
    
    if(0 && !m_pStau->noIDTrack())//currently don't use GF in the iteration
    {
	pFcnStep->globalFitData = new GlobalFitData();
        m_pStau->pStauGF()->initStepData(pFcnStep->globalFitData, currentBeta);
        m_pStau->pStauGF()->processGlobalFitWithBeta(currentBeta, pFcnStep->globalFitData);
	gfStepChi2 = pFcnStep->globalFitData->chi2;
	gfStepDoF = pFcnStep->globalFitData->dof;
    }
    

    if( doMdtt )
    {
        pFcnStep->mdttData = new MdttStepData();
        m_pStau->pStauMDTT()->initStepData(pFcnStep->mdttData, currentBeta);
        m_pStau->pStauMDTT()->processWithBeta(currentBeta, pFcnStep->mdttData);
        mdttStepChi2 = pFcnStep->mdttData->chi2;
        mdttStepDoF  = pFcnStep->mdttData->dof;
    }

//    if(NULL!=pFcnStep->tileCalData) std::cout << "shikma:: tile dof " << tileCalStepDoF << " chi2 " << tileCalStepChi2 << " numOfCells " << pFcnStep->tileCalData->numOfCells   << std::endl; 
    
    double fcnStepChi2 = mdtStepChi2 + rpcStepChi2 + tileCalStepChi2 + gfStepChi2 + tgcStepChi2 + mdttStepChi2 + trtStepChi2;
    double fcnStepDoF =  mdtStepDoF  + rpcStepDoF  + tileCalStepDoF  + gfStepDoF  + tgcStepDoF  + mdttStepDoF + trtStepDoF - 1;
    
    pFcnStep->chi2 = (fcnStepDoF!=0 ? fcnStepChi2 : 1000);
    pFcnStep->dof = fcnStepDoF;
    pFcnStep->isBarrel = (m_pStau->pStauRPC()->hasHits() ? true : false );
    pFcnStep->isGlobalFit = (0 && !m_pStau->noIDTrack());

    m_pStau->fcnStepsData()->push_back(pFcnStep);
    
    return pFcnStep->chi2;
}

//================== 
bool MuGirlNS::StauFcn::selectRange(double& min, double& max)
{
    double value = 0.;
    double epsilon = 0.0001;
    do{
        if(m_pStau->lowerBetaLimit()-epsilon < value && m_pStau->upperBetaLimit()+epsilon > value) function(value,MuGirlNS::MS_TECHS);
        value += 0.1;
    }while(value < m_pStau->upperBetaLimit()+epsilon); 
    
    return m_pStau->selectRange(min,max);
}

void MuGirlNS::StauFcn::singleBeta(double beta)
{
    function(beta,MuGirlNS::ALL_TECHS);
}
