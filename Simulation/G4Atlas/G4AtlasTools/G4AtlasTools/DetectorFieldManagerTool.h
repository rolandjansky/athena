/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_DetectorFieldManagerTool_H
#define G4ATLASTOOLS_DetectorFieldManagerTool_H

// Geant4 field manger
#include "G4FieldManager.hh"

// Local includes
#include "G4AtlasTools/G4FieldManagerToolBase.h"
#include "G4AtlasTools/ThreadLocalHolder.h"

/** @class DetectorFieldManagerTool DetectorFieldManagerTool.h "G4AtlasTools/DetectorFieldManagerTool.h"
 *
 *  @brief Tool for setting up a volume-local magnetic field manager
 *
 *  @author Andrea Dell'Acqua
 *  @date   2016-03-25
 */
class DetectorFieldManagerTool : public G4FieldManagerToolBase
{

  public:

    /// Standard constructor
    DetectorFieldManagerTool( const std::string& type, const std::string& name,
                              const IInterface* parent );

    /// Empty destructor
    ~DetectorFieldManagerTool(){};

    /// Initialize a field manager
    StatusCode initializeField() override final;

  protected:

    /// List of volumes to assign this field configuration to
    Gaudi::Property<std::vector<std::string> > m_volumeList{this, "LogicalVolumes", {}, "List of volumes to which the field will be applied"};

    /// Option for muons feeling the B-field only
    Gaudi::Property<bool> m_muonOnlyField{this, "MuonOnlyField", false, "Only muons experience the magnetic field"};

    /// My field manager
    thread_utils::ThreadLocalOwner<G4FieldManager> m_fieldMgrHolder;

};

#endif // G4ATLASTOOLS_DetectorFieldManagerTool_H
