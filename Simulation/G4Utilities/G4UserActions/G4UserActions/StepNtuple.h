/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StepNtuple_H
#define StepNtuple_H

#include "FadsActions/UserAction.h"
#include <string>
#include "GaudiKernel/NTuple.h"
#include <vector>

struct stepinfo{

  float time, code, dep, x, y, z;

};

class StepNtuple: public FADS::UserAction {

public:
	StepNtuple(std::string s): FADS::UserAction(s) {}
	void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);
        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);
        void SteppingAction(const G4Step*);
	std::vector<stepinfo> eventSteps;

private:

	NTuple::Item<long>     m_nsteps;
	NTuple::Array<float>   m_pdgcode;
	NTuple::Array<float>   m_step_x;  
	NTuple::Array<float>   m_step_y;  
	NTuple::Array<float>   m_step_z;
	NTuple::Array<float>   m_time;
	NTuple::Array<float>   m_dep;  

};

#endif
