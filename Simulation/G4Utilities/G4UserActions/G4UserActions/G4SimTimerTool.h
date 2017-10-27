/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA_G4SIMTIMERTOOL_H
#define G4USERACTIONS_G4UA_G4SIMTIMERTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "G4UserActions/G4SimTimer.h"

namespace G4UA
{

  /// @class G4SimTimerTool
  /// @brief Tool which manages the G4SimTimer action.
  ///
  /// Create the G4SimTimer for each worker thread and merges timing results in
  /// finalization.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class G4SimTimerTool : public ActionToolBaseReport<G4SimTimer>,
                         public IBeginEventActionTool, public IEndEventActionTool
  {

    public:

      /// Standard constructor
      G4SimTimerTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

      /// Temporary, just for debugging
      virtual StatusCode initialize() override;

      /// Finalize and merge results from all worker threads
      virtual StatusCode finalize() override;

      /// Retrieve the begin-event action interface
      virtual IBeginEventAction* getBeginEventAction() override final
      { return static_cast<IBeginEventAction*>( getAction() ); }

      /// Retreive the end-event action interface
      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }

    protected:

      /// Create aciton for this thread
      virtual std::unique_ptr<G4SimTimer> makeAction() override final;

  }; // class G4SimTimerTool

} // namespace G4UA

#endif
