/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISiLayerNumberAllocator.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ILAYERNUMBERALLOCATOR_H
# define IPATINTERFACES_ILAYERNUMBERALLOCATOR_H

#include "GaudiKernel/IAlgTool.h"

/** Interface ID for ILayerNumberAllocator*/  
static const InterfaceID IID_ILayerNumberAllocator("ILayerNumberAllocator", 1, 0);
  
/**@class ILayerNumberAllocator

Abstract base class for LayerNumberAllocator AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class ILayerNumberAllocator: virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~ILayerNumberAllocator() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ILayerNumberAllocator; }

    /**ILayerNumberAllocator interface:
       allocate layer number according to rz coordinate  */
    virtual int			layerNumber (double r, double z) const = 0;
    
    /**ILayerNumberAllocator interface:
       print layer boundary table summarizing arrangement of silicon layers wrt vertex region  */
    virtual void		print (void) const = 0;

};   

#endif // IPATINTERFACES_ILAYERNUMBERALLOCATOR_H
