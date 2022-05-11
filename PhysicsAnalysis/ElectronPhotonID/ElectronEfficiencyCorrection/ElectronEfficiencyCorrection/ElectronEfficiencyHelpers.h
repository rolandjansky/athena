/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ELECTRONEFFICIENCYHELPERS__
#define __ELECTRONEFFICIENCYHELPERS__

// Atlas includes
#include "xAODEgamma/Electron.h"
#include "PATInterfaces/CorrectionCode.h"

namespace ElectronEfficiencyHelpers{

  /// Get the charge of the original electron
  CP::CorrectionCode getEleTruthCharge( const xAOD::Electron& ele, int& truthcharge);
  
  /// Return true if it's good ele for charge flip measurements
  CP::CorrectionCode isGoodEle( const xAOD::Electron& ele, bool& goodEle);

  struct ROOT6_NamespaceAutoloadHook{};

}

#endif
