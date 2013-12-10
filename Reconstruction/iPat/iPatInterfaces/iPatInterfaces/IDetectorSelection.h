/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IDetectorSelection.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_IDETECTORSELECTION_H
# define IPATINTERFACES_IDETECTORSELECTION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/IAlgTool.h"

/** Interface ID for IDetectorSelection*/  
static const InterfaceID IID_IDetectorSelection("IDetectorSelection", 1, 0);
  
/**@class IDetectorSelection

Abstract base class for DetectorSelection AlgTool
note: this Tool really should be a Service !
     
     
@author Alan.Poppleton@cern.ch
*/

class IDetectorSelection: virtual public IAlgTool
{
public:
 
    /**Virtual destructor*/
    virtual ~IDetectorSelection() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IDetectorSelection; }
  
    /**IDetectorSelection interface:
       methods defining which of the Pixel,SCT and/or TRT detectors are selected for processing */
    virtual bool		processPixels (void) const = 0;
    virtual bool		processSCT (void) const = 0;
    virtual bool		processTRT (void) const = 0;
};

#endif // IPATINTERFACES_IDETECTORSELECTION_H

