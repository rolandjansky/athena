/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2018 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Markus Prim                                              *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/

#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtSemiLeptonicVectorAmp.hh"
#include "EvtGenBase/EvtSemiLeptonicScalarAmp.hh"
#include <string>

//#include "generators/evtgen/EvtGenModelRegister.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtBCL.h"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtBCLFF.h"




/** register the model in EvtGen */
//  B2_EVTGEN_REGISTER_MODEL(EvtBCL);

EvtBCL::EvtBCL() : m_bclmodel(nullptr), m_calcamp(nullptr) {}

EvtBCL::~EvtBCL()
{
  delete m_bclmodel;
  m_bclmodel = nullptr;
  delete m_calcamp;
  m_calcamp = nullptr;
}

std::string EvtBCL::getName()
{
  return "BCL";
}

EvtDecayBase* EvtBCL::clone()
{
  return new EvtBCL;
}

void EvtBCL::decay(EvtParticle* p)
{
  p->initializePhaseSpace(getNDaug(), getDaugs());
  m_calcamp->CalcAmp(p, _amp2, m_bclmodel);
}


void EvtBCL::initProbMax()
{
  
  EvtId parnum, mesnum, lnum, nunum;
  
  parnum = getParentId();
  mesnum = getDaug(0);
  lnum = getDaug(1);
  nunum = getDaug(2);
  
  double mymaxprob = m_calcamp->CalcMaxProb(parnum, mesnum, lnum, nunum, m_bclmodel);

  setProbMax(mymaxprob);
}


void EvtBCL::init()
{

  checkNDaug(3);

  //We expect the parent to be a scalar
  //and the daughters to be X lepton neutrino

  checkSpinParent(EvtSpinType::SCALAR);
  checkSpinDaughter(1, EvtSpinType::DIRAC);
  checkSpinDaughter(2, EvtSpinType::NEUTRINO);
  
  EvtSpinType::spintype mesontype = EvtPDL::getSpinType(getDaug(0));
  
  m_bclmodel = new EvtBCLFF(getNArg(), getArgs());
  
  if (mesontype == EvtSpinType::SCALAR) {
    m_calcamp = new EvtSemiLeptonicScalarAmp;
  }
  if (mesontype == EvtSpinType::VECTOR) {
    m_calcamp = new EvtSemiLeptonicVectorAmp;
  }
  // Tensor Meson implementation is possible here.
  
}



