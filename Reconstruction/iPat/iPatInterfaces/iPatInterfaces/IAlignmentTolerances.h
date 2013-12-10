/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IAlignmentTolerances.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_IALIGNMENTTOLERANCES_H
# define IPATINTERFACES_IALIGNMENTTOLERANCES_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/IAlgTool.h"

/** Interface ID for IAlignmentTolerances*/  
static const InterfaceID IID_IAlignmentTolerances("IAlignmentTolerances", 1, 0);
  
/**@class IAlignmentTolerances

Abstract base class for AlignmentTolerances AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class IAlignmentTolerances: virtual public IAlgTool
{
public:
 
    /**Virtual destructor*/
    virtual ~IAlignmentTolerances() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IAlignmentTolerances; }

    /**IAlignmentTolerances interface:
       set alignment tolerances onto SiliconDetectors  */
    virtual void		setPixelDetectors (void) = 0;
    virtual void		setSCT_Detectors (void) = 0;

};

#endif // IPATINTERFACES_IALIGNMENTTOLERANCES_H

