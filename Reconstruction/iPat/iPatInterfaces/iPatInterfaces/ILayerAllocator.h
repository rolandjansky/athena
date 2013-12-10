/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISiLayerAllocator.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ILAYERALLOCATOR_H
# define IPATINTERFACES_ILAYERALLOCATOR_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"

class SiliconDetector;
class SiliconLayer;

/** Interface ID for ILayerAllocator*/  
static const InterfaceID IID_ILayerAllocator("ILayerAllocator", 1, 0);
  
/**@class ILayerAllocator

Abstract base class for LayerAllocator AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class ILayerAllocator: virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~ILayerAllocator() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ILayerAllocator; }

    /**ILayerAllocator interface:
       access to silicon detectors */
    virtual const std::vector<SiliconDetector*>&	detectors(void) const = 0;

    /**ILayerAllocator interface:
       access to layers forming pixel/sct barrel/endcap */
    virtual const std::vector<SiliconLayer*>&		pixelBarrel(void) const = 0;
    virtual const std::vector<SiliconLayer*>&		pixelEndcap(void) const = 0;
    virtual const std::vector<SiliconLayer*>&		sctBarrel(void) const = 0;
    virtual const std::vector<SiliconLayer*>&		sctEndcap(void) const = 0;
    virtual const std::vector<SiliconLayer*>&		supportCylinders(void) const = 0;

    /**ILayerAllocator interface:
       refresh cached data whenever conditions change */
    virtual void					refresh (void) = 0;

    /**ILayerAllocator interface:
       layers intersected by a straight line in rz-projection */
    virtual std::vector<SiliconLayer*>*			siliconLayers (double r0,
								       double z0,
								       double cotTheta) const = 0;
};

#endif // IPATINTERFACES_ILAYERALLOCATOR_H
