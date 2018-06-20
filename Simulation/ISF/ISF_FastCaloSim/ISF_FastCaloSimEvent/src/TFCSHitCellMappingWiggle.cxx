/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSHitCellMappingWiggle.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunctionInt32Histogram.h"
#include "TRandom3.h"
#include "TVector2.h"
#include "TMath.h"

//=============================================
//======= TFCSHitCellMappingWiggle =========
//=============================================

TFCSHitCellMappingWiggle::TFCSHitCellMappingWiggle(const char* name, const char* title, ICaloGeometry* geo) : TFCSHitCellMapping(name,title,geo)
{
}

TFCSHitCellMappingWiggle::~TFCSHitCellMappingWiggle()
{
  if(!m_function) delete m_function;
}

void TFCSHitCellMappingWiggle::initialize(TFCS1DFunction* func)
{
  if(!func) return;
  if(!m_function) delete m_function;
  m_function=func;
}

void TFCSHitCellMappingWiggle::initialize(TH1* histogram,float xscale)
{
  if(!histogram) return;
  TFCS1DFunctionInt32Histogram* func=new TFCS1DFunctionInt32Histogram(histogram);
  if(xscale!=1) {
    for(auto& ele : func->get_HistoBordersx()) ele*=xscale;
  }
  initialize(func);
}

void TFCSHitCellMappingWiggle::simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  if(!m_function) return;
  
  double rnd = 1-gRandom->Rndm();
  /* 
  double wiggle;
  // CODE ONLY NEEDED FOR ONE SIDES FUNCTION. CHANGED TO TWO SIDED FUNCTION FOR NOW
  if(rnd>=0.5) { //do negative wiggle
    rnd-=0.5;
    rnd*=2;
    wiggle=-1;
  } else { //do positive wiggle
    rnd*=2;
    wiggle=+1;
  }
  */
  
  double wiggle=m_function->rnd_to_fct(rnd);

  ATH_MSG_DEBUG("HIT: E="<<hit.E()<<" cs="<<calosample()<<" eta="<<hit.eta()<<" phi="<<hit.phi()<<" wiggle="<<wiggle);

  double hit_phi_shifted=hit.phi()+wiggle;
  hit.phi()=TVector2::Phi_mpi_pi(hit_phi_shifted);

  TFCSHitCellMapping::simulate_hit(hit,simulstate,truth,extrapol);
}
