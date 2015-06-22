/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IEnergyLossSampler.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_IENERGYLOSSSAMPLER_H
#define ISF_FATRASINTERFACES_IENERGYLOSSSAMPLER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

// STL
#include <utility>

namespace Trk {

  class MaterialProperties;
  class EnergyLoss;
  
}

namespace iFatras {

  /** Interface ID for IEnergyLossSampler*/  
  static const InterfaceID IID_IEnergyLossSampler("IEnergyLossSampler", 1, 0);
  
  /**@class IEnergyLossSampler
     
     Interface class IEnergyLossSampler
     
     @author Noemi.Calace@cern.ch, Andreas.Salzburger@cern.ch
  */

  class IEnergyLossSampler : virtual public IAlgTool {
    
  public:
    /**Virtual destructor*/
    virtual ~IEnergyLossSampler(){}
    
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IEnergyLossSampler; }
    
    /** dEdX calculation when providing MaterialProperties,
	a momentum, and a ParicleHypothesis. 
	- Units: [Mev/mm]
    */
    virtual double dEdX(const Trk::MaterialProperties& mat,
			double momentum,
			Trk::ParticleHypothesis particle = Trk::pion) const = 0;
    
    /** deltaE calculation
	using dEdX and integrating along pathlength,
	assuming constant dEdX during for the path.
	- The sign depends on the given propagation direction 
	- mpv steers the most probable energy loss
	- Units: [MeV]
    */
    virtual Trk::EnergyLoss* energyLoss(const Trk::MaterialProperties& mat,
					double momentum,
					double pathcorrection,
					Trk::PropDirection dir=Trk::alongMomentum,
					Trk::ParticleHypothesis particle=Trk::pion,
					bool mpv = false) const = 0;  
    
  };

} // end of namespace

#endif // ISF_FATRASINTERFACES_IENERGYLOSSSAMPLER_H
