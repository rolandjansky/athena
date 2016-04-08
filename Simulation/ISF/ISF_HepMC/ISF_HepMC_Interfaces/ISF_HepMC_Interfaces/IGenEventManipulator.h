/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IGenEventManipulator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_INTERFACES_IGENEVENTMANIPULATOR_H
#define ISF_HEPMC_INTERFACES_IGENEVENTMANIPULATOR_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace HepMC {
    class GenEvent;
} 
 
namespace ISF {
 
  static const InterfaceID IID_IGenEventManipulator("IGenEventManipulator", 1, 0);
   
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

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IGenEventManipulator; }
       
       /** Returns the Particle Stack, should register truth */
       virtual StatusCode manipulate(HepMC::GenEvent& ge) = 0;
       
  };

} // end of namespace

#endif // ISF_HEPMC_INTERFACES_IGENEVENTMANIPULATOR_H
