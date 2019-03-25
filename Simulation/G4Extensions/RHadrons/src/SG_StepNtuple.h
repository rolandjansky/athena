/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SG_StepNtuple_H
#define SG_StepNtuple_H

#include <set>

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthMessaging.h"

#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"

namespace G4UA
{

  class SG_StepNtuple : public AthMessaging, public G4UserRunAction,
                        public G4UserEventAction, public G4UserSteppingAction
  {

  public:

    SG_StepNtuple(const std::vector<int>&);

    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void UserSteppingAction(const G4Step*) override;

  private:

    bool isSUSYParticle(const int id) const;

    NTuple::Item<long>     m_nsteps, m_evtid;
    NTuple::Array<int>     m_pdg, m_charge, m_baryon;
    NTuple::Array<float>   m_x1, m_y1, m_z1, m_t1, m_pt1;
    NTuple::Array<float>   m_x2, m_y2, m_z2, m_t2, m_pt2;
    NTuple::Array<float>   m_minA, m_v2, m_vthresh, m_vbelowthresh;
    NTuple::Array<float>   m_dep, m_mass;
    NTuple::Array<float>   m_ke1, m_ke2;
    NTuple::Array<int>   m_rh, m_rhid, m_step;
    std::set<int> m_rhs;//rhadron pdgid's
    long m_nevents = 0;
    long m_rhadronIndex = 0;

  }; // class SG_StepNtuple

} // namespace G4UA

#endif
