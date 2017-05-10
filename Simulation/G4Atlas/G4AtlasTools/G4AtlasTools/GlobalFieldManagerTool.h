/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_GlobalFieldManagerTool_H
#define G4ATLASTOOLS_GlobalFieldManagerTool_H

// Local includes
#include "G4AtlasTools/G4FieldManagerToolBase.h"


/** @class GlobalFieldManagerTool GlobalFieldManagerTool.h "G4AtlasTools/GlobalFieldManagerTool.h"
 *
 *  @brief Tool for setting up a detector-global magnetic field manager.
 *
 *  @author Andrea Dell'Acqua
 *  @date   2016-03-25
 */
class GlobalFieldManagerTool : public G4FieldManagerToolBase
{

  public:

    /// Standard constructor
    GlobalFieldManagerTool(const std::string& type, const std::string& name,
                           const IInterface* parent);

    /// Empty destructor
    ~GlobalFieldManagerTool(){};

    /// Initialize a field manager
    StatusCode initializeField() override final;

  protected:

};

#endif // G4ATLASTOOLS_GlobalFieldManagerTool_H
