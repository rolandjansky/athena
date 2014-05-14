/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerAttemptsCalculcator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETY_LAYERATTEMPTSCALCULATOR_H
#define TRKGEOMETY_LAYERATTEMPTSCALCULATOR_H

#include "TrkVolumes/BoundarySurfaceFace.h"

namespace Trk {

  
  /** @class LayerAttemptsCalculator
    
      Simple helper class that allows to define different number
      of layer attempts in the extrapolation before throwing a veto
      
      @author Andreas.Salzburger@cern.ch
    */ 
    class LayerAttemptsCalculator {
      public:
       virtual ~LayerAttemptsCalculator(){}  
          
       /** The calculator interface */
       virtual unsigned int layerAttempts(BoundarySurfaceFace bsFace = Trk::undefinedFace) const = 0;
       
       /** The max attempts */
       virtual unsigned int maxLayerAttempts() const = 0;

    };

}

#endif
