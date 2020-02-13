/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationFluctChain.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

#include "CLHEP/Random/RandGauss.h"

#include "TMath.h"

//=============================================
//======= TFCSLateralShapeParametrizationFluctChain =========
//=============================================

TFCSLateralShapeParametrizationFluctChain::TFCSLateralShapeParametrizationFluctChain(const char* name, const char* title, float RMS):TFCSLateralShapeParametrizationHitChain(name,title), m_RMS(RMS)
{
}

TFCSLateralShapeParametrizationFluctChain::TFCSLateralShapeParametrizationFluctChain(TFCSLateralShapeParametrizationHitBase* hitsim):TFCSLateralShapeParametrizationHitChain(hitsim)
{
}

float TFCSLateralShapeParametrizationFluctChain::get_E_hit(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  const float sigma2 = get_sigma2_fluctuation(simulstate,truth,extrapol);
  const int sample = calosample();
  if(sigma2<=0 || sample<0) return -1.;
  const float maxWeight = getMaxWeight();
  
  if(maxWeight>0) return simulstate.E(sample)*sigma2/maxWeight;
  else return simulstate.E(sample)*sigma2;
  
}

FCSReturnCode TFCSLateralShapeParametrizationFluctChain::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  // Call get_number_of_hits() only once, as it could contain a random number
  float sigma2  = get_sigma2_fluctuation(simulstate, truth, extrapol);
  if (sigma2 >= s_max_sigma2_fluctuation) {
    ATH_MSG_ERROR("TFCSLateralShapeParametrizationHitChain::simulate(): fluctuation of hits could not be calculated");
    return FCSFatal;
  }

  //Limit to relative precision of 10^-4=0.01%. ATLAS calorimeter is ~0.1% at best
  if(sigma2<1e-8) sigma2=1e-8; 

  const float Elayer=simulstate.E(calosample());

  //Make a good guess of the needed hit energy, assuming all hits would have the same energy
  const float Eavghit=get_E_hit(simulstate,truth,extrapol);
  const float Eavghit_tenth=Eavghit/10;
  float sumEhit=0;
  float error2_sumEhit=0;
  float error2=2*s_max_sigma2_fluctuation;

  const bool debug = msgLvl(MSG::DEBUG);
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
    //hit.E()=Eavghit;
    do {
      hit.E()=CLHEP::RandGauss::shoot(simulstate.randomEngine(), Eavghit, m_RMS*Eavghit);
    } while (hit.E()<Eavghit_tenth);
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
      const float Ehit=hit.E();
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

void TFCSLateralShapeParametrizationFluctChain::Print(Option_t *option) const
{
  TString opt(option);
  bool shortprint=opt.Index("short")>=0;
  bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  TString optprint=opt;optprint.ReplaceAll("short","");
  TFCSLateralShapeParametrizationHitChain::Print(option);

  if(longprint) ATH_MSG_INFO(optprint <<"  hit energy fluctuation RMS="<<m_RMS);
}
