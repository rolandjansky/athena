/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CTP/RandomTrigger.h"

#include <iostream>
#include <assert.h>

// gaudi includes
#include "GaudiKernel/StatusCode.h"

// random number generator
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

LVL1CTP::RandomTrigger::RandomTrigger() :
   SpecialTrigger(),
   m_randEngine(0) 
{
   Init();
}

LVL1CTP::RandomTrigger::RandomTrigger(unsigned int rand_number, const unsigned int aRate,unsigned int ctpVersionNumber, CLHEP::HepRandomEngine* randEng) :
   SpecialTrigger(TrigConf::L1DataDef::RNDM, rand_number, ctpVersionNumber, aRate),
   m_randEngine(randEng)
{
   Init();
}

LVL1CTP::RandomTrigger::~RandomTrigger()
{}

void
LVL1CTP::RandomTrigger::Init() {
   if (m_randEngine != 0) return;
   std::cout << "Random engine not set up." << std::endl;
   assert(0);
}

StatusCode
LVL1CTP::RandomTrigger::execute(int /*bcid=1*/) {
   // get next random number    
    m_counter = CLHEP::RandFlat::shoot(m_randEngine);
    return StatusCode::SUCCESS;
}

bool
LVL1CTP::RandomTrigger::evaluate() const {
   return (m_counter >= (1.-1./this->rate()));
}
