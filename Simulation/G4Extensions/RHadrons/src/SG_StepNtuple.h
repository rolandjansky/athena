/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SG_StepNtuple_H
#define SG_StepNtuple_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>
#include "GaudiKernel/NTuple.h"
#include <vector>
#include <set>

class SG_StepNtuple final: public UserActionBase {

public:
  SG_StepNtuple(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent), nevents(0), rhid(0) {}
  virtual void BeginOfEvent(const G4Event*)  override;
  virtual void EndOfEvent(const G4Event*) override;
  virtual void Step(const G4Step*) override;
    virtual StatusCode initialize() override;
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;


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
  std::set<int> rhs;//rhadron pdgid's
  long nevents, rhid;
};
#endif
