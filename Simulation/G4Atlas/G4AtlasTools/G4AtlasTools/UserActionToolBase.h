/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_USERACTIONTOOLBASE_H
#define G4ATLASTOOLS_USERACTIONTOOLBASE_H

// System includes
#include <memory>

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"

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
  class UserActionToolBase : public AthAlgTool
  {

    public:

      /// Standard constructor
      UserActionToolBase(const std::string& type, const std::string& name,
                         const IInterface* parent)
        : AthAlgTool(type, name, parent)
      {}

    protected:

      /// Thread-specific storage of the user action
      ThreadSpecificUserAction<ActionType> m_actions;

  }; // UserActionToolBase

} // namespace G4UA

#endif // G4ATLASTOOLS_USERACTIONTOOLBASE_H
