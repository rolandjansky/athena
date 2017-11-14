/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_USERACTIONTOOLBASE_H
#define G4ATLASTOOLS_USERACTIONTOOLBASE_H

// System includes
#include <memory>

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"

// G4Atlas includes
#include "G4AtlasInterfaces/IUserActionTool.h"

// Local includes
#include "ThreadSpecificUserAction.h"


namespace G4UA
{

  /// @class UserActionToolBase
  /// @brief abstract template utility base-class for G4 user-action tools.
  ///
  /// The template parameter of this class should be the concrete action type.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  template <class ActionType>
  class UserActionToolBase : public extends<AthAlgTool, IUserActionTool>
  {

    public:

      /// Standard constructor
      UserActionToolBase(const std::string& type, const std::string& name,
                         const IInterface* parent)
        : base_class(type, name, parent)
      {}

      /// Make the action and push onto the lists
      virtual std::unique_ptr<ActionType>
      makeAndFillAction(G4AtlasUserActions& actionLists) = 0;

      /// Fill the user action lists
      virtual StatusCode
      fillUserAction(G4AtlasUserActions& actionLists) override final
      {
        auto myAction = makeAndFillAction(actionLists);
        if(myAction == nullptr) {
          ATH_MSG_ERROR( "Failed to construct user action in " << name() );
          return StatusCode::FAILURE;
        }
        m_actions.set( std::move(myAction) );
        return StatusCode::SUCCESS;
      }

    protected:

      /// Thread-specific storage of the user action
      ThreadSpecificUserAction<ActionType> m_actions;

  }; // UserActionToolBase

} // namespace G4UA

#endif // G4ATLASTOOLS_USERACTIONTOOLBASE_H
