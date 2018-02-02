/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthBase_MCTruthSteppingActionTool_h
#define MCTruthBase_MCTruthSteppingActionTool_h

// Local includes
#include "MCTruthSteppingAction.h"

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

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
  class MCTruthSteppingActionTool : public UserActionToolBase<MCTruthSteppingAction>
  {

    public:

      /// Standard constructor
      MCTruthSteppingActionTool(const std::string& type, const std::string& name,
                                const IInterface* parent);

      /// Initialize the tool
      virtual StatusCode initialize() override final;

    protected:

      /// Setup the user action for current thread
      virtual std::unique_ptr<MCTruthSteppingAction>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      /// Map of volume name to output collection name
      std::map<std::string, std::string> m_volumeCollectionMap;

  }; // class MCTruthSteppingActionTool

} // namespace G4UA

#endif
