/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA_LOOPERKILLERTOOL_H
#define G4USERACTIONS_G4UA_LOOPERKILLERTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "G4UserActions/LooperKiller.h"

namespace G4UA
{

  /// @class LooperKillerTool
  /// @brief Tool which manages the LooperKiller action.
  ///
  /// Create the LooperKiller for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class LooperKillerTool : public ActionToolBaseReport<LooperKiller>,
                           public ISteppingActionTool
  {

    public:

      /// Standard constructor
      LooperKillerTool(const std::string& type, const std::string& name,
		       const IInterface* parent);


      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;

      /// Retrieve the begin-event action interface
      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }

    protected:

      /// Create action for this thread
      virtual std::unique_ptr<LooperKiller> makeAction() override final;

    private:
      /// Configuration parameters
      G4UA::LooperKiller::Config m_config;

  }; // class LooperKillerTool

} // namespace G4UA

#endif
