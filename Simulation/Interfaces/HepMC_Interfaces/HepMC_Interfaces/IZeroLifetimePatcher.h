/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPMC_INTERFACES_IZEROLIFETIMEPATCHER_H
#define HEPMC_INTERFACES_IZEROLIFETIMEPATCHER_H 1

// Gaudi
// framework includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

namespace HepMC {
    class GenEvent;
}

namespace Simulation {

  /**
   @class IZeroLifetimePatcher

   Interface for a service which works around the issue of zero-lifetime particles in Geant4.

   */

  class IZeroLifetimePatcher : virtual public IInterface {
     public:

       /** Virtual destructor */
       virtual ~IZeroLifetimePatcher(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IZeroLifetimePatcher, 1, 0);

       /** Applies the workaround for zero-lifetime particles to the GenEvent */
       virtual StatusCode applyWorkaround(HepMC::GenEvent& ge) const = 0;

       /** Removes the workaround for zero-lifetime particles from the GenEvent */
       virtual StatusCode removeWorkaround(HepMC::GenEvent& ge) const = 0;

  };

} // end of namespace

#endif // HEPMC_INTERFACES_IZEROLIFETIMEPATCHER_H
