/*                                                                                                                      
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration                                               
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_MUCTPI_SAALG_H
#define TRIGT1MUCTPIPHASE1_MUCTPI_SAALG_H

/*
class description
*/

namespace LVL1MUCTPIPHASE1 {

  class SimController;

  class MUCTPI_SAAlg
  {
    
  public:
    
    MUCTPI_SAAlg();
    ~MUCTPI_SAAlg();
    
    void initialize();
    void execute();
    void finalize();

    void setRunSimData(bool flag, int nevents) {m_runSimData=flag;m_nSimEvents=nevents;}
    
  private:

    
    SimController* m_controller;
    bool m_runSimData;
    int m_nSimEvents;

  };
}

#endif // TRIGT1MUCTPIPHASE1_MUCTPI_SAALG_H
