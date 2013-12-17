/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_RUNALGS_H
#define ANP_RUNALGS_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : RunAlgs
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 * This class creates, configures and executes a list of AlgEvent algorithms.
 * 
 **********************************************************************************/

// C/C++
#include <vector>

// Local
#include "TrigCostBase/AlgEvent.h"

namespace Anp
{
  class RunAlgs: public AlgEvent {
  public:
    
    RunAlgs();
    virtual ~RunAlgs();
    
    void Config(const Anp::Registry &reg);
    
    void Signal(const Anp::Registry &reg, const std::string &signal);

    void Save(TDirectory *);

    bool Init();

    void Exec();

    void Done();
    
  private:
    
    typedef std::vector<Anp::Handle<Anp::AlgEvent> > AlgVec;

  private:

    bool                      fDebug;       // Print some debug info
    AlgVec                    fAlg;         // List of algorithms to be executed
  };
}

#endif
