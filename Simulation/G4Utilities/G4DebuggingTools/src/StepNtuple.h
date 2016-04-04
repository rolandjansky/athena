/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_StepNtuple_H
#define G4DEBUGGINGTOOLS_StepNtuple_H

#include "G4AtlasTools/UserActionBase.h"

#include "GaudiKernel/NTuple.h"

#include <string>
#include <vector>

struct stepinfo{
  float time, code, dep, x, y, z;
};

class StepNtuple final:  public UserActionBase {

  public:
   StepNtuple(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent){}
   virtual void BeginOfEvent(const G4Event*) override;
   virtual void EndOfEvent(const G4Event*) override;

   virtual void Step(const G4Step*) override;
   std::vector<stepinfo> eventSteps;

   virtual StatusCode queryInterface(const InterfaceID&, void**) override;
   virtual StatusCode initialize();

  private:
   NTuple::Item<long>     m_nsteps;
   NTuple::Array<float>   m_pdgcode;
   NTuple::Array<float>   m_step_x;
   NTuple::Array<float>   m_step_y;
   NTuple::Array<float>   m_step_z;
   NTuple::Array<float>   m_time;
   NTuple::Array<float>   m_dep;

};

#endif // G4DEBUGGINGTOOLS_StepNtuple_H
