/*                                                                                                                      
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration                                               
*/

// First the corresponding header.
#include "TrigT1MuctpiPhase1/SimController.h"


// The headers from other ATLAS packages,
// from most to least dependent.
#include "TrigT1MuctpiPhase1/GenTestPattern.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"

// Headers from external packages.


// System headers.
#include <iostream>

namespace LVL1MUCTPIPHASE1 {

  SimController::SimController() :
    m_muctpiInput(new LVL1MUONIF::Lvl1MuCTPIInputPhase1()),
    m_patGen(new GenTestPattern(m_muctpiInput))
    
  {
  }
  
  SimController::~SimController()
  {
    delete m_muctpiInput;
    delete m_patGen;
  }

  void SimController::genRandomSimData()
  {
    m_patGen->generateEvent(0, 0x3);
  }

  void SimController::run()
  {
    std::cout << *m_muctpiInput << std::endl;
  }


}

