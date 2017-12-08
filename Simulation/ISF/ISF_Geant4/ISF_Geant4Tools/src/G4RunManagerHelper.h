/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4RUNMANAGERHELPER_H
#define ISF_GEANT4TOOLS_G4RUNMANAGERHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ISF_Geant4Tools/IG4RunManagerHelper.h"

#include "ISF_Geant4Tools/G4AtlasRunManager.h"

namespace iGeant4 {

  class G4RunManagerHelper: virtual public ISF::IG4RunManagerHelper,  public AthAlgTool {

  public:
    G4RunManagerHelper(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);
    virtual ~G4RunManagerHelper();

    StatusCode initialize();
    StatusCode finalize();

    G4AtlasRunManager* g4RunManager() const;
    G4RunManager*      fastG4RunManager() const;

  private:

    mutable G4AtlasRunManager* m_g4RunManager;
    mutable G4RunManager*      m_fastG4RunManager;

  };

}

#endif // ISF_GEANT4TOOLS_G4RUNMANAGERHELPER_H
