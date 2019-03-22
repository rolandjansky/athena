/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZeroLifetimePositioner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BEAMEFFECTS_ZEROLIFETIMEPOSITIONER_H
#define BEAMEFFECTS_ZEROLIFETIMEPOSITIONER_H 1

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "HepMC_Interfaces/IZeroLifetimePatcher.h"
#include <vector>

namespace Simulation {

  /** @class ZeroLifetimePositioner

      This tool works around the issue of zero-lifetime particles in Geant4.

     */
  class ZeroLifetimePositioner : public extends<AthService, Simulation::IZeroLifetimePatcher> {

    public:
      /** Constructor with parameters */
      ZeroLifetimePositioner( const std::string& name, ISvcLocator* pSvcLocator );

      /** Athena algtool's Hooks */
      StatusCode  initialize() override final;
      StatusCode  finalize() override final;

      /** Applies the workaround for zero-lifetime particles to the GenEvent */
      virtual StatusCode applyWorkaround(HepMC::GenEvent& ge) const override final;

      /** Removes the workaround for zero-lifetime particles from the GenEvent */
      virtual StatusCode removeWorkaround(HepMC::GenEvent& ge) const override final;

    private:
      StatusCode manipulate(HepMC::GenEvent& ge, bool applyPatch, bool removePatch) const;
      bool m_applyPatch{false};
      bool m_removePatch{false};
      std::vector<int> m_pdgCodesToCheck{511};
  };

}

#endif //> !BEAMEFFECTS_ZEROLIFETIMEPOSITIONER_H
