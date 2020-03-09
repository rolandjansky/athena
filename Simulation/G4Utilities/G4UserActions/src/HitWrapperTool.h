/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__HITWRAPPERTOOL_H
#define G4USERACTIONS_G4UA__HITWRAPPERTOOL_H

// Infrastructure includes
#include  "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "HitWrapper.h"

namespace G4UA
{

  /// @brief A tool which manages the HitWrapper user action.
  class HitWrapperTool : public UserActionToolBase<HitWrapper>
  {

    public:

      /// Standard constructor
      HitWrapperTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

    protected:

      /// Create the action for the current thread
      virtual std::unique_ptr<HitWrapper>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      HitWrapper::Config m_config;

  }; // class HitWrapperTool

} // namespace G4UA

#endif
