/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSParametrizationBase_h
#define ISF_FASTCALOSIMEVENT_TFCSParametrizationBase_h

#include <TNamed.h>
#include <set>
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

class TFCSParametrizationBase:public TNamed {
public:
  TFCSParametrizationBase(const char* name=0, const char* title=0);

  virtual bool is_match_pdgid(int /*id*/) const {return false;};
  virtual bool is_match_Ekin(float /*Ekin*/) const {return false;};
  virtual bool is_match_eta(float /*eta*/) const {return false;};

  virtual bool is_match_Ekin_bin(int /*Ekin_bin*/) const {return false;};
  virtual bool is_match_calosample(int /*calosample*/) const {return false;};

  virtual const std::set< int > &pdgid() const {return m_no_pdgid;};
  virtual double Ekin_nominal() const {return 0;};
  virtual double Ekin_min() const {return 0;};
  virtual double Ekin_max() const {return 0;};
  virtual double eta_nominal() const {return 100;};
  virtual double eta_min() const {return 100;};
  virtual double eta_max() const {return 100;};

  // Do some simulation. Result should be returned in simulstate
  // Simulate all energies in calo layers for energy parametrizations
  // Simulate one HIT for later shape parametrizations (TO BE DISCUSSED!)
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  void Print(Option_t *option = "") const;
private:
  static std::set< int > m_no_pdgid;

  ClassDef(TFCSParametrizationBase,1)  //TFCSParametrizationBase
};

#endif
