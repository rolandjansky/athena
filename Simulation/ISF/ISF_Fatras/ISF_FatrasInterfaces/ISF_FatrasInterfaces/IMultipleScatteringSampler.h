/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMultipleScatteringSampler.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_IMULTIPLESCATTERINGSAMPLER_H
#define ISF_FATRASINTERFACES_IMULTIPLESCATTERINGSAMPLER_H

// Gaudi 
#include "GaudiKernel/IAlgTool.h"

// Trk
#include "TrkEventPrimitives/ParticleHypothesis.h"

namespace Trk {
  class MaterialProperties;
}

namespace iFatras {
 
  /**@class IMultipleScatteringSampler
     
     Interface class IMultipleScatteringSampler
     
     @author Noemi.Calace@cern.ch, Andreas.Salzburger@cern.ch
  */
  
  class IMultipleScatteringSampler : virtual public IAlgTool {

  public:
    /**Virtual destructor*/
    virtual ~IMultipleScatteringSampler(){}
    
    /// Creates the InterfaceID and interfaceID() method
    DeclareInterfaceID(IMultipleScatteringSampler, 1, 0);
    
    virtual double simTheta (const Trk::MaterialProperties& mat,
			     double momentum,
			     double pathcorrection,
			     Trk::ParticleHypothesis particle = Trk::pion,
			     double deltaE=0.) const = 0;
    
  };

}

#endif // ISF_FATRASINTERFACES_IMULTIPLESCATTERINGSAMPLER_H
