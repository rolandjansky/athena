/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__GEANT4SETUPCHECKER_H 
#define G4DEBUGGINGTOOLS_G4UA__GEANT4SETUPCHECKER_H 

#include "G4UserRunAction.hh"
#include <string>

namespace G4UA{ 

class Geant4SetupChecker: 
  public G4UserRunAction
  {
    
  public:
    Geant4SetupChecker(const std::string&,const bool);
    virtual void BeginOfRunAction(const G4Run*) override;
  private:
    /// File location for reference file
    std::string m_file_location;
    /// Do you want to write a reference or read a file to check?
    bool m_test;
  }; // class Geant4SetupChecker


} // namespace G4UA 
#endif
