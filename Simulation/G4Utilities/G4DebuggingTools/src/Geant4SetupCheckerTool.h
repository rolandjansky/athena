/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__GEANT4SETUPCHECKERTOOL_H
#define G4DEBUGGINGTOOLS_G4UA__GEANT4SETUPCHECKERTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "Geant4SetupChecker.h"

namespace G4UA
{

  /// @class Geant4SetupCheckerTool
  /// @brief Tool which manages the Geant4SetupChecker
  ///
  /// @author Zach Marshall
  ///
  class Geant4SetupCheckerTool : public UserActionToolBase<Geant4SetupChecker>
  {

    public:
      /// Standard constructor
      Geant4SetupCheckerTool(const std::string& type, const std::string& name,const IInterface* parent);
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<Geant4SetupChecker>
      makeAndFillAction(G4AtlasUserActions&) override final;
    private:
      /// File location for reference file
      std::string m_file_location;
      /// Test the setup or write a reference file?
      bool m_test;
  }; // class Geant4SetupCheckerTool

} // namespace G4UA

#endif
