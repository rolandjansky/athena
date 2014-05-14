/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderLayerAttemptsCalculator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETY_CYLINDERLAYERATTEMPTSCALCULATOR_H
#define TRKGEOMETY_CYLINDERLAYERATTEMPTSCALCULATOR_H

#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkGeometry/LayerAttemptsCalculator.h"

namespace Trk {

  
  /** @class CylinderLayerAttemptsCalculator
    
      Simple helper class for CylinderLayer static array

      @author Andreas.Salzburger@cern.ch
    */ 
    
    class CylinderLayerAttemptsCalculator : public LayerAttemptsCalculator {
      public:
        /** Constructor */
        CylinderLayerAttemptsCalculator(unsigned int faceAttempts, unsigned int maxAttempts) :
          m_faceAttempts(faceAttempts),
          m_maxLayerAttempts(maxAttempts)
        {}

        /** one single interface method */
        unsigned int layerAttempts(BoundarySurfaceFace bsFace = Trk::undefinedFace) const;

        /** The max attempts */
        unsigned int maxLayerAttempts() const;

      private:
        unsigned int m_faceAttempts;     //!< number of face attampts
        unsigned int m_maxLayerAttempts; //!< the max attempt number

    };

    inline unsigned int CylinderLayerAttemptsCalculator::layerAttempts(BoundarySurfaceFace bsFace) const {
        // if you go through the Inner/Outer cover only one attempt is allowed
        if (bsFace == tubeInnerCover || bsFace == tubeOuterCover) return m_faceAttempts;
        // allow the maximum
        return m_maxLayerAttempts;
    }
     
    inline unsigned int CylinderLayerAttemptsCalculator::maxLayerAttempts() const 
    {
        return m_maxLayerAttempts;
    }
}

#endif
