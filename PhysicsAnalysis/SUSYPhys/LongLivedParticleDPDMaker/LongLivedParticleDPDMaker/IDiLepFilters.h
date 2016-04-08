/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// IDiLepFilters.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Dominik Krauss (krauss@mpp.mpg.de)

#ifndef DERIVATIONFRAMEWORK_IDILEPFILTERS_H
#define DERIVATIONFRAMEWORK_IDILEPFILTERS_H 1

// Athena
#include "GaudiKernel/IAlgTool.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"

// declaration of interface ID
static const InterfaceID IID_DerivationFramework_IDiLepFilters("DerivationFramework::IDiLepFilters", 1, 0);

namespace DerivationFramework
{
  class IDiLepFilters : virtual public IAlgTool
  {
    public:
      static const InterfaceID& interfaceID() { return IID_DerivationFramework_IDiLepFilters; }

      virtual bool GetTriggers() = 0;

      virtual bool PassSiEl(const xAOD::Electron& el) const = 0;
      virtual bool PassSiPhX(const xAOD::Photon& ph, const xAOD::Electron& el) const = 0;
      virtual bool PassSiPhX(const xAOD::Photon& ph1, const xAOD::Photon& ph2) const = 0;
      virtual bool PassSiPhX(const xAOD::Photon& ph, const xAOD::Muon& mu) const = 0;
      virtual bool PassSiMu(const xAOD::Muon& mu) const = 0;

      virtual bool PassDiEl(const xAOD::Electron& el1, const xAOD::Electron& el2) const = 0;
      virtual bool PassDiPh(const xAOD::Photon& ph1, const xAOD::Photon& ph2) const = 0;
      virtual bool PassDiElPh(const xAOD::Electron& el, const xAOD::Photon& ph) const = 0;
      virtual bool PassDiLoElPh(const xAOD::Electron& el, const xAOD::Photon& ph) const = 0;
  };
}

#endif
