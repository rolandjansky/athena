/*                                                                                                                      
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration                                               
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_SIMCONTROLLER_H
#define TRIGT1MUCTPIPHASE1_SIMCONTROLLER_H

/*
class description
*/

namespace LVL1MUONIF {
  class Lvl1MuCTPIInputPhase1;
}

namespace LVL1MUCTPIPHASE1 {

  class GenTestPattern;

  class SimController
  {
    
  public:
    
    SimController();
    ~SimController();
    
    void genRandomSimData();
    void run();
    
  private:
  
    LVL1MUONIF::Lvl1MuCTPIInputPhase1* m_muctpiInput;
    GenTestPattern* m_patGen;

  };

}

#endif // TRIGT1MUCTPIPHASE1_SIMCONTROLLER_H
