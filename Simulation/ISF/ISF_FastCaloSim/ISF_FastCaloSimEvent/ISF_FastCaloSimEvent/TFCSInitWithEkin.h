/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSInitWithEkin_h
#define ISF_FASTCALOSIMEVENT_TFCSInitWithEkin_h

#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"

class TFCSInitWithEkin:public TFCSParametrizationBase {
public:
  TFCSInitWithEkin(const char* name=nullptr, const char* title=nullptr);

  virtual bool is_match_Ekin(float /*Ekin*/) const {return true;};
  virtual bool is_match_eta(float /*eta*/) const {return true;};

  virtual bool is_match_Ekin_bin(int /*Ekin_bin*/) const {return true;};
  virtual bool is_match_calosample(int /*calosample*/) const {return true;};

  virtual bool is_match_all_Ekin() const {return true;};
  virtual bool is_match_all_eta() const {return true;};
  virtual bool is_match_all_Ekin_bin() const {return true;};
  virtual bool is_match_all_calosample() const {return true;};

  // Initialize simulstate with the kinetic energy Ekin from truth
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
private:

  ClassDef(TFCSInitWithEkin,1)  //TFCSInitWithEkin
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSInitWithEkin+;
#endif

#endif
