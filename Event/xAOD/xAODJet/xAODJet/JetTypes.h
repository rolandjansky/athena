// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_JETTYPES_H
#define XAODJET_JETTYPES_H

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {

  /// Base 4 Momentum type for Jet
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> >
  JetFourMom_t;

  
  enum JetConstitScale {
    UncalibratedJetConstituent      =  0,
    CalibratedJetConstituent        =  1,
  };
  
  enum JetScale {

    JetEMScaleMomentum = 0,
    JetConstitScaleMomentum = 1,
    JetAssignedScaleMomentum =2,
    
    //EMSCALE = JET_USING_UNCALIBRATED_CONSTIT, //  compat with run1
    //CONSTITUENTSCALE = JEt_USING_JETFINDING_CONSTIT //  compat with run1
  };



}

#endif
