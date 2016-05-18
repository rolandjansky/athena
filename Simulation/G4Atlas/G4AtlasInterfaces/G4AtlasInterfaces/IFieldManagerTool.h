/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasInterfaces_IFieldManagerTool_H
#define G4AtlasInterfaces_IFieldManagerTool_H

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class G4FieldManager;

/** @class IFieldManagerTool IFieldManagerTool.h "G4AtlasInterfaces/IFieldManagerTool.h"
 *
 *  Abstract interface to a field manager tool.
 *
 *  @author ADA
 *  @date   2015-11-17
 */

class IFieldManagerTool : virtual public IAlgTool
{
  public:

    /// Gaudi interface ID
    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_IFieldManagerTool("IFieldManagerTool", 1, 0);
      return IID_IFieldManagerTool;
    }

    /// Initialize a field manager, field, and stepper
    virtual StatusCode initializeField() = 0;
};

#endif
