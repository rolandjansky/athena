/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthBase_MCTruthSteppingActionTool_h
#define MCTruthBase_MCTruthSteppingActionTool_h

// Local includes
#include "MCTruthSteppingAction.h"

// Infrastructure includes
#include "G4AtlasTools/ActionToolBase.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"

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
                                    public IBeginEventActionTool,
                                    public ISteppingActionTool
  {

    public:

      /// Standard constructor
      MCTruthSteppingActionTool(const std::string& type, const std::string& name,
                                const IInterface* parent);

      /// Initialize the tool
      virtual StatusCode initialize() override final;

      /// Retrieve the begin-event action
      virtual IBeginEventAction* getBeginEventAction() override final
      { return static_cast<IBeginEventAction*>( getAction() ); }

      /// Retrieve the stepping action
      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }

    private:

      /// Create an action for the current thread
      virtual std::unique_ptr<MCTruthSteppingAction> makeAction() override final;

      /// Map of volume name to output collection name
      std::map<std::string, std::string> m_volumeCollectionMap;

  }; // class MCTruthSteppingActionTool

} // namespace G4UA

#endif
