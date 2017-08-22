/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_StepNtuple_H
#define G4DEBUGGINGTOOLS_StepNtuple_H


#include "GaudiKernel/NTuple.h"

#include <string>
#include <vector>


#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserRunAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA{
  
  
  class StepNtuple : public AthMessaging,
                     public G4UserEventAction,
                     public G4UserSteppingAction,
                     public G4UserRunAction
  {
    /// simple struct to hold step information
    struct stepdata{
      float time, code, dep, x, y, z;
    };
    
  public:
    /// ctor
    StepNtuple();

    /// the hooks for G4 UA handling
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void UserSteppingAction(const G4Step*) override;
    virtual void BeginOfRunAction(const G4Run*) override;

  private:

    /// holds data extracted from steps
    std::vector<stepdata> eventSteps;
    
    /// handles for ntuple writing
    NTuple::Item<long>     m_nsteps;
    NTuple::Array<float>   m_pdgcode;
    NTuple::Array<float>   m_step_x;
    NTuple::Array<float>   m_step_y;
    NTuple::Array<float>   m_step_z;
    NTuple::Array<float>   m_time;
    NTuple::Array<float>   m_dep;
    
    
  }; // class StepNtuple
  
  
} // namespace G4UA 



#endif // G4DEBUGGINGTOOLS_StepNtuple_H
