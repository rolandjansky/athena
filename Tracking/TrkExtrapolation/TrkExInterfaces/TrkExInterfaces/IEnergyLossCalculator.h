/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IEnergyLossCalculator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IENERGYLOSSCALCULATOR_H
#define TRKEXINTERFACES_IENERGYLOSSCALCULATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExUtils/ElossInputBase.h"
// STL
#include <utility>

namespace Trk {  

  class EnergyLoss;

  /** Interface ID for IEnergyLossCalculator*/  
  static const InterfaceID IID_IEnergyLossCalculator("IEnergyLossCalculator", 1, 0);
  
  /**@class IEnergyLossCalculator
     
     Interface class IEnergyLossCalculator
     
     @author David.Lopez@cern.ch
    */
  class IEnergyLossCalculator : virtual public IAlgTool {
      
     public:

       /**Virtual destructor*/
       virtual ~IEnergyLossCalculator(){}
       
       /** AlgTool and IAlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IEnergyLossCalculator; }
       
      /** deltaE calculation. It returns an EnergyLoss
        The sign of the deltaE in the EnergyLoss returned
	depends on the given propagation direction. Since the calculation can be
	very complex, any extra parameters required in the calculation should be
	given through a class inheriting from the abstract class ElossInputBase.
	Dynamic casting will be required inside each time. extraParams is not owned
	by this class. It is given as a pointer so that it can be defaulted to 0.

        Units: [MeV]
        */
      virtual EnergyLoss* energyLoss(double dX0,
				     double p,
				     PropDirection dir=alongMomentum,
				     ParticleHypothesis particle=pion,
				     ElossInputBase* extraParams = 0) const = 0;  

       /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps */
       virtual void validationAction() const {}

  };

} // end of namespace

#endif // TRKEXINTERFACES_IENERGYLOSSCALCULATOR_H
