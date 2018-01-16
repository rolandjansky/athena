/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthBase_MCTruthSteppingActionTool_h
#define MCTruthBase_MCTruthSteppingActionTool_h

// Local includes
#include "MCTruthSteppingAction.h"

// Infrastructure includes
#include "G4AtlasTools/ActionToolBase.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"

// STL includes
#include <string>
#include <map>

namespace G4UA
{

  /// @class MCTruthSteppingActionTool
  /// @brief A tool for managing the MCTruthSteppingAction
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class MCTruthSteppingActionTool : public ActionToolBase<MCTruthSteppingAction>,
                                    public IG4EventActionTool,
                                    public IG4SteppingActionTool
  {

    public:

      /// Standard constructor
      MCTruthSteppingActionTool(const std::string& type, const std::string& name,
                                const IInterface* parent);

      /// Initialize the tool
      virtual StatusCode initialize() override final;

      /// Retrieve the begin-event action
      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }

      /// Retrieve the stepping action
      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }

    private:

      /// Create an action for the current thread
      virtual std::unique_ptr<MCTruthSteppingAction> makeAction() override final;

      /// Map of volume name to output collection name
      std::map<std::string, std::string> m_volumeCollectionMap;

  }; // class MCTruthSteppingActionTool

} // namespace G4UA

#endif
