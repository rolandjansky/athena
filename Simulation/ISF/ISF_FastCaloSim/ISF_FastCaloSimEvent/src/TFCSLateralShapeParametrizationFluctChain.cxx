/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationFluctChain.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

#include "TMath.h"

//=============================================
//======= TFCSLateralShapeParametrizationFluctChain =========
//=============================================

TFCSLateralShapeParametrizationFluctChain::TFCSLateralShapeParametrizationFluctChain(const char* name, const char* title):TFCSLateralShapeParametrizationHitChain(name,title)
{
}

TFCSLateralShapeParametrizationFluctChain::TFCSLateralShapeParametrizationFluctChain(TFCSLateralShapeParametrizationHitBase* hitsim):TFCSLateralShapeParametrizationHitChain(hitsim)
{
}

FCSReturnCode TFCSLateralShapeParametrizationFluctChain::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  // Call get_number_of_hits() only once, as it could contain a random number
  float sigma2  = get_sigma2_fluctuation(simulstate, truth, extrapol);
  if (sigma2 >= 1000) {
    ATH_MSG_ERROR("TFCSLateralShapeParametrizationHitChain::simulate(): fluctuation of hits could not be calculated");
    return FCSFatal;
  }

  //Limit to relative precision of 10^-4=0.01%. ATLAS calorimeter is ~0.1% at best
  if(sigma2<1e-8) sigma2=1e-8; 

  float Elayer=simulstate.E(calosample());
  int nhit = 1.0 / sigma2;

  //Make a good guess of the needed hit energy, assuming all hits would have the same energy
  float Eavghit=Elayer/nhit;
  float sumEhit=0;
  float error2_sumEhit=0;
  float error2=1000;

  bool debug = msgLvl(MSG::DEBUG);
  if (debug) {
    ATH_MSG_DEBUG("E("<<calosample()<<")="<<Elayer<<" sigma2="<<sigma2);
  }

  int ihit=0;
  int ifail=0;
  int itotalfail=0;
  TFCSLateralShapeParametrizationHitBase::Hit hit;
  hit.reset_center();
  do {
    hit.reset();
    hit.E()=Eavghit;
    bool failed=false;
    for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
      if (debug) {
        if (ihit < 2) hitsim->setLevel(MSG::DEBUG);
        else hitsim->setLevel(MSG::INFO);
      }

      FCSReturnCode status = hitsim->simulate_hit(hit, simulstate, truth, extrapol);

      if (status == FCSSuccess) continue;
      if (status == FCSFatal) return FCSFatal;
      failed=true;
      ++ifail;
      ++itotalfail;
      break;
    }
    if(!failed) {
      ifail=0;
      ++ihit;
      float Ehit=hit.E();
      sumEhit+=Ehit;
      error2_sumEhit+=Ehit*Ehit;
      if(sumEhit>0) error2=error2_sumEhit/(sumEhit*sumEhit);
    } else {
      if (ifail >= FCS_RETRY_COUNT) {
        ATH_MSG_ERROR("TFCSLateralShapeParametrizationFluctChain::simulate(): simulate_hit call failed after " << FCS_RETRY_COUNT << "retries");
      }
      if(ifail >= FCS_RETRY_COUNT*FCS_RETRY_COUNT) {
        return FCSFatal;
      }
    }
  } while (error2>sigma2);

  if (debug) {
    ATH_MSG_DEBUG("E("<<calosample()<<")="<<Elayer<<" sumE="<<sumEhit<<"+-"<<TMath::Sqrt(error2_sumEhit)<<" ~ "<<TMath::Sqrt(error2_sumEhit)/sumEhit*100<<"% rel error^2="<<error2<<" sigma^2="<<sigma2<<" ~ "<<TMath::Sqrt(sigma2)*100<<"% hits="<<ihit<<" fail="<<itotalfail);
  }

  return FCSSuccess;
}

