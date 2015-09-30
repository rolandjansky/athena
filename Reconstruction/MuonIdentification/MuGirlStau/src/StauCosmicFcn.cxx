/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauCosmicFcn.h"
#include "MuGirlStau/StauTool.h"
#include "MuGirlStau/StauRPC.h"
#include "MuGirlStau/StauMDT.h"
#include "MuGirlStau/StauGF.h"
#include "MuGirlStau/StauTileCal.h"

//================ Constructor =================================================

MuGirlNS::StauCosmicFcn::StauCosmicFcn(StauTool* pStauTool, MsgStream& log, double beta) :
        m_pStau(pStauTool), m_log(log), m_beta(beta)
{
}

//================ Destructor =================================================

MuGirlNS::StauCosmicFcn::~StauCosmicFcn()
{
}

//================ The goal function =================================================

double MuGirlNS::StauCosmicFcn::function(double tTrack, StauTechnology eTech)
{

    std::cout << "E123: " << __LINE__ << " " << __FILE__ << std::endl;
    LOG_VERBOSE << "technology " << technologyName(eTech) << endreq;
//    dbgE
    //allocate memory for the data of a new step
    double beta = m_beta;
    FcnStepData* pFcnStep = new FcnStepData();
    pFcnStep->isBarrel = 0.;
    pFcnStep->beta = beta;
    pFcnStep->chi2 = 0.;
    pFcnStep->tTrack = tTrack;

    pFcnStep->mdtData = NULL;
    pFcnStep->rpcData = NULL;
    pFcnStep->tileCalData = NULL;
    pFcnStep->tgcData = NULL;

    double mdtStepChi2 = 0., rpcStepChi2 = 0., tileCalStepChi2 = 0., gfStepChi2 = 0., tgcStepChi2 =
            0.;
    double mdtStepDoF = 0., rpcStepDoF = 0., tileCalStepDoF = 0., gfStepDoF = 0., tgcStepDoF = 0.;

    if ((m_pStau->doMdt() && eTech == MuGirlNS::AOD_TECHS) || eTech == MuGirlNS::ALL_TECHS
            || eTech == MuGirlNS::MDT_TECH || eTech == MuGirlNS::MS_TECHS)

    {
        pFcnStep->mdtData = new MdtStepData();
        m_pStau->pStauMDT()->initStepData(pFcnStep->mdtData, beta, tTrack);
        m_pStau->pStauMDT()->processMdtWithTTrack(tTrack, pFcnStep->mdtData, beta);
        mdtStepChi2 = pFcnStep->mdtData->chi2;
        mdtStepDoF = pFcnStep->mdtData->totNumHits
                - 2 * pFcnStep->mdtData->pStationDataList->size();
    }

    /*if( (m_pStau->doRpc() && eTech == MuGirlNS::AOD_TECHS) ||
     eTech == MuGirlNS::ALL_TECHS ||
     eTech == MuGirlNS::RPC_TECH ||
     eTech == MuGirlNS::MS_TECHS)
     {
     pFcnStep->rpcData = new RpcStepData();
     m_pStau->pStauRPC()->initStepData(pFcnStep->rpcData, beta, tTrack);
     m_pStau->pStauRPC()->processRpcWithTTrack(tTrack, pFcnStep->rpcData, beta);
     rpcStepChi2 = pFcnStep->rpcData->chi2;
     rpcStepDoF = pFcnStep->rpcData->totNumHits;
     }*/

    if (!m_pStau->noIDTrack())
    {
        if (eTech == MuGirlNS::ALL_TECHS || eTech == MuGirlNS::TILECAL_TECH)
        {
            pFcnStep->tileCalData = new TileCalStepData();
            m_pStau->pStauTileCal()->initStepData(pFcnStep->tileCalData, beta, tTrack);
            m_pStau->pStauTileCal()->processTileCalWithTTrack(tTrack, pFcnStep->tileCalData, beta);
            tileCalStepChi2 = pFcnStep->tileCalData->chi2;
            tileCalStepDoF = pFcnStep->tileCalData->numOfCells;
        }
    }
    //}

    double fcnStepChi2 = mdtStepChi2 + rpcStepChi2 + tileCalStepChi2 + gfStepChi2 + tgcStepChi2;
    double fcnStepDoF = mdtStepDoF + rpcStepDoF + tileCalStepDoF + gfStepDoF + tgcStepDoF;

    pFcnStep->chi2 = (fcnStepDoF != 0 ? fcnStepChi2 / fcnStepDoF : 1000);
    pFcnStep->dof = fcnStepDoF;
    pFcnStep->isBarrel = (m_pStau->pStauRPC()->hasHits() ? true : false);

    std::cout << " E123: chi2 " << fcnStepChi2 << " dof: " << fcnStepDoF << std::endl;
    std::cout << " E123: pFcn: " << pFcnStep->chi2 << " dof " << pFcnStep->dof << std::endl;
    m_pStau->fcnStepsData()->push_back(pFcnStep);

    return pFcnStep->chi2;
}

//==================
bool MuGirlNS::StauCosmicFcn::selectRange(double& min, double& max)
{
    double value = (min < m_pStau->lowerTTrackLimit()) ? m_pStau->lowerTTrackLimit() : min;
    double epsilon = 0.0001;
    double increment = 2;
    while (value < m_pStau->upperTTrackLimit() + epsilon)
    {
        value += increment;
        function(value, MuGirlNS::ALL_TECHS);
    }

    return m_pStau->selectRangeTTrack(min, max);
}

void MuGirlNS::StauCosmicFcn::singleTTrack(double tTrack)
{
    function(tTrack, MuGirlNS::ALL_TECHS);
}
