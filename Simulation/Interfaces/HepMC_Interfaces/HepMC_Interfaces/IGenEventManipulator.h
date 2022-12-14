/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPMC_INTERFACES_IGENEVENTMANIPULATOR_H
#define HEPMC_INTERFACES_IGENEVENTMANIPULATOR_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "AtlasHepMC/GenEvent_fwd.h"

class EventContext;

namespace Simulation {

  /**
   @class IGenEventManipulator

   Interface definition for the HepMC::GenEvent manipulator,
   the provided GenEvent is hereby changed, i.e. geometrical transformations are applied to
   all contained vertices.

   @author Andreas.Salzburger -at- cern.ch
   */

  class IGenEventManipulator : virtual public IAlgTool {
     public:

       /** Virtual destructor */
       virtual ~IGenEventManipulator(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IGenEventManipulator, 1, 0);

       /** Applies any required transformations to the GenEvent */
    virtual StatusCode manipulate(HepMC::GenEvent& ge, const EventContext& ctx) const = 0;

  };

} // end of namespace

#endif // HEPMC_INTERFACES_IGENEVENTMANIPULATOR_H
