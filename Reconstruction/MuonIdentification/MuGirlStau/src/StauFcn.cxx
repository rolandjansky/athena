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
#include "CxxUtils/make_unique.h"
//================ Constructor =================================================

MuGirlNS::StauFcn::StauFcn(StauTool* pStauTool, MsgStream& log) :
        m_pStau(pStauTool), m_log(log)
{
}

//================ Destructor =================================================

MuGirlNS::StauFcn::~StauFcn()
{
}

//================ The goal function =================================================

double MuGirlNS::StauFcn::function(double currentBeta, StauTechnology eTech)
{
    LOG_VERBOSE << "technology " << technologyName(eTech) << endreq;

    //allocate memory for the data of a new step
    FcnStepData* pFcnStep = new FcnStepData();
    pFcnStep->isBarrel = 0.;
    pFcnStep->beta = currentBeta;
    pFcnStep->chi2 = 0.;
    pFcnStep->mdtData = NULL;
    pFcnStep->rpcData = NULL;
    pFcnStep->tileCalData = NULL;
    pFcnStep->tgcData = NULL;

    bool doMdt = (eTech == MuGirlNS::AOD_TECHS || eTech == MuGirlNS::MDT_TECH
            || eTech == MuGirlNS::MS_TECHS || eTech == MuGirlNS::ALL_TECHS);
    bool doMdtt = (eTech == MuGirlNS::MDTT_TECH || eTech == MuGirlNS::ALLT_TECHS);
    bool doRpc = ((eTech == MuGirlNS::AOD_TECHS && m_pStau->doRpc()) || eTech == MuGirlNS::RPC_TECH
            || eTech == MuGirlNS::MS_TECHS || eTech == MuGirlNS::ALL_TECHS
            || eTech == MuGirlNS::ALLT_TECHS);
    bool doTile = (eTech == MuGirlNS::TILECAL_TECH || eTech == MuGirlNS::ALL_TECHS
            || eTech == MuGirlNS::ALLT_TECHS);

    LOG_VERBOSE << "doMdt=" << doMdt
                << " doMdtt=" << doMdtt
                << " doRpc=" << doRpc
                << " doTile=" << doTile
                << endreq;

    double fcnStepChi2 = 0;
    double fcnStepDoF = 0;

    if (doMdt)
    {
        pFcnStep->mdtData = new MdtStepData();
        m_pStau->pStauMDT()->initStepData(pFcnStep->mdtData, currentBeta);
        m_pStau->pStauMDT()->processMdtWithBeta(currentBeta, pFcnStep->mdtData);
	fcnStepChi2 += pFcnStep->mdtData->chi2;
        fcnStepDoF += pFcnStep->mdtData->dof;
    }

    if (doRpc)
    {
        pFcnStep->rpcData = new RpcStepData();
        m_pStau->pStauRPC()->initStepData(pFcnStep->rpcData, currentBeta);
        m_pStau->pStauRPC()->processRpcWithBeta(currentBeta, pFcnStep->rpcData);
	fcnStepChi2 += pFcnStep->rpcData->chi2;
        fcnStepDoF += pFcnStep->rpcData->dof;
    }

    if (!m_pStau->noIDTrack() && doTile)
    {
        pFcnStep->tileCalData = new TileCalStepData();
        m_pStau->pStauTileCal()->initStepData(pFcnStep->tileCalData, currentBeta);
        m_pStau->pStauTileCal()->processTileCalWithBeta(currentBeta, pFcnStep->tileCalData);
	fcnStepChi2 += pFcnStep->tileCalData->chi2;
	fcnStepDoF += pFcnStep->tileCalData->dof;
    }

    if (doMdtt)
    {
      pFcnStep->mdttData = CxxUtils::make_unique<MdttStepData>();
      m_pStau->pStauMDTT()->initStepData(pFcnStep->mdttData.get(), currentBeta);
      m_pStau->pStauMDTT()->processWithBeta(currentBeta, pFcnStep->mdttData.get());
      fcnStepChi2 += pFcnStep->mdttData->chi2;
      fcnStepDoF += pFcnStep->mdttData->dof;
    }

//    if(NULL!=pFcnStep->tileCalData) std::cout << "shikma:: tile dof " << tileCalStepDoF << " chi2 " << tileCalStepChi2 << " numOfCells " << pFcnStep->tileCalData->numOfCells   << std::endl;

    fcnStepDoF -= 1;

    pFcnStep->chi2 = (fcnStepDoF != 0 ? fcnStepChi2 : 1000);
    pFcnStep->dof = fcnStepDoF;
    pFcnStep->isBarrel = (m_pStau->pStauRPC()->hasHits() ? true : false);
    m_pStau->fcnStepsData()->push_back(pFcnStep);

    return pFcnStep->chi2;
}

//==================
bool MuGirlNS::StauFcn::selectRange(double& min, double& max)
{
    double value = 0.;
    double epsilon = 0.0001;
    do
    {
        if (m_pStau->lowerBetaLimit() - epsilon < value
                && m_pStau->upperBetaLimit() + epsilon > value) function(value, MuGirlNS::MS_TECHS);
        value += 0.1;
    } while (value < m_pStau->upperBetaLimit() + epsilon);

    return m_pStau->selectRange(min, max);
}

void MuGirlNS::StauFcn::singleBeta(double beta)
{
    function(beta, MuGirlNS::ALL_TECHS);
}
