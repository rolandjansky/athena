/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISiDetectorLocator.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ISIDETECTORLOCATOR_H
# define IPATINTERFACES_ISIDETECTORLOCATOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "GaudiKernel/IAlgTool.h"

class Identifier;
class IdentifierHash;
class SiliconDetector;

/** Interface ID for ISiDetectorLocator*/  
static const InterfaceID IID_ISiDetectorLocator("ISiDetectorLocator", 1, 0);
  
/**@class ISiDetectorLocator

Abstract base class for SiDetectorLocator AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class ISiDetectorLocator: virtual public IAlgTool
{
public:
 
    /**Virtual destructor*/
    virtual ~ISiDetectorLocator() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ISiDetectorLocator; }

    /**ISiDetectorLocator interface:
       const iterators to access pixel/SCT vectors  */
    virtual std::vector<SiliconDetector*>::const_iterator	beginPixel (void) const = 0; 
    virtual std::vector<SiliconDetector*>::const_iterator	beginSCT (void) const = 0; 
    virtual std::vector<SiliconDetector*>::const_iterator	endPixel (void) const = 0;
    virtual std::vector<SiliconDetector*>::const_iterator	endSCT (void) const = 0;
    
    /**ISiDetectorLocator interface:
       pixel detector look-up by identifier  */
    virtual const SiliconDetector&			pixelDetector (IdentifierHash hashId) const = 0;
    virtual IdentifierHash				pixelHash (const Identifier& id) const = 0;

    /**ISiDetectorLocator interface:
       sct detector look-up by identifier  */
    virtual const SiliconDetector&			SCT_Detector (IdentifierHash hashId) const = 0;
    virtual IdentifierHash				SCT_Hash (const Identifier& id) const = 0;
 
};

#endif // IPATINTERFACES_ISIDETECTORLOCATOR_H

