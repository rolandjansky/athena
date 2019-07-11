/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__FASTIDKILLERTOOL_H
#define G4USERACTIONS_G4UA__FASTIDKILLERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "FastIDKiller.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class FastIDKillerTool: public UserActionToolBase<FastIDKiller>
  {

    public:

      /// Standard constructor
      FastIDKillerTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

      virtual StatusCode finalize() override;

    protected:

      /// Create the action for the current thread
      virtual std::unique_ptr<FastIDKiller>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      FastIDKiller::Config m_config;

  }; // class FastIDKillerTool

} // namespace G4UA

#endif
