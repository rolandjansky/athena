/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__GEANT4SETUPCHECKERTOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__GEANT4SETUPCHECKERTOOL_H 
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "Geant4SetupChecker.h"

namespace G4UA{ 

/// @class Geant4SetupCheckerTool
/// @brief Tool which manages the Geant4SetupChecker
///
/// @author Zach Marshall
///
  

  class Geant4SetupCheckerTool: 
  public ActionToolBase<Geant4SetupChecker>,
    public IG4RunActionTool
    {
      
    public:
      /// Standard constructor
      Geant4SetupCheckerTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// Retrieve the BoE action
      virtual G4UserRunAction* getRunAction() override final
      { return static_cast<G4UserRunAction*>( getAction() ); }
      /// Gaudi interface management
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<Geant4SetupChecker> makeAction() override final;
    private:
      /// File location for reference file
      std::string m_file_location;
      /// Test the setup or write a reference file?
      bool m_test;
    }; // class Geant4SetupCheckerTool
  
  
} // namespace G4UA 
#endif
