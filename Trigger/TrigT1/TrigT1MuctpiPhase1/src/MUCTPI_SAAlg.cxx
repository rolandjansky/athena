/*                                                                                                                      
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration                                               
*/

// First the corresponding header.
#include "TrigT1MuctpiPhase1/MUCTPI_SAAlg.h"


// The headers from other ATLAS packages,
// from most to least dependent.
#include "TrigT1MuctpiPhase1/SimController.h"

// Headers from external packages.


// System headers.
#include <iostream>

namespace LVL1MUCTPIPHASE1 {

  MUCTPI_SAAlg::MUCTPI_SAAlg() :
    m_controller(new SimController())
  {
    
  }
  
  MUCTPI_SAAlg::~MUCTPI_SAAlg()
  {
    delete m_controller;
  }
  
  void MUCTPI_SAAlg::initialize()
  {
  }
  
  void MUCTPI_SAAlg::execute()
  {
    for (int i=0;i<m_nSimEvents;i++)
    {
      std::cout << "ON EVENT " << i+1 << " / " << m_nSimEvents << std::endl;
      if (m_runSimData) m_controller->genRandomSimData();
      m_controller->run();
    }
  }

  void MUCTPI_SAAlg::finalize()
  {

  }

}
