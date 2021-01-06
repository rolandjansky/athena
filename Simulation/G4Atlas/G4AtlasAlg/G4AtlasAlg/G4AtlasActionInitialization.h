/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4ATLASACTIONINITIALIZATION_H
#define G4ATLASALG_G4ATLASACTIONINITIALIZATION_H 1

#include "G4VUserActionInitialization.hh"
#include "G4AtlasInterfaces/IUserActionSvc.h"

/// Action initialization class.
///

class G4AtlasActionInitialization : public G4VUserActionInitialization
{
public:
  G4AtlasActionInitialization(G4UA::IUserActionSvc* userActionSvc);
  virtual ~G4AtlasActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  G4UA::IUserActionSvc* m_userActionSvc{};
};

#endif //G4ATLASALG_G4ATLASACTIONINITIALIZATION_H
