/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISiliconLayerAssociator.h, (c) ATLAS iPatRec
//
// associate clusters within a layer given SiliconLayer predictions 
//
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_SILICONLAYERASSOCIATOR_H
# define IPATINTERFACES_SILICONLAYERASSOCIATOR_H

#include <list>
#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

class LayerPrediction;
class SiliconClusterMap;

/** Interface ID for ILayerAllocator*/  
static const InterfaceID IID_ISiliconLayerAssociator("ISiliconLayerAssociator", 1, 0);
  
/**@class ISiliconLayerAssociator

Abstract base class for SiliconLayerAssociator AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class ISiliconLayerAssociator: virtual public IAlgTool
{
public:
    
    /**Virtual destructor*/
    virtual ~ISiliconLayerAssociator() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ISiliconLayerAssociator; }

    /**ILayerAllocator interface:
       clear internal state */
    virtual void   	clear (void) = 0;
    
    /**ILayerAllocator interface:
       associate clusters from the SiliconClusterMap given predictions within a SiliconLayer */
    virtual void   	associateClusters (
	const SiliconClusterMap* 			clusterMap,
	const Amg::Vector3D			     	position,
	const Amg::Vector3D			     	transverseDirection,
	double						rPhiWidth,
	double						rZWidth,
	std::list<LayerPrediction*>::const_iterator	begin,
	std::list<LayerPrediction*>::const_iterator	end) = 0;
    

};

#endif // IPATINTERFACES_SILICONLAYERASSOCIATOR_H

