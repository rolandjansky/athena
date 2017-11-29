/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__FASTIDKILLERTOOL_H
#define G4USERACTIONS_G4UA__FASTIDKILLERTOOL_H

#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/FastIDKiller.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class FastIDKillerTool: public ActionToolBaseReport<FastIDKiller>,
                          public IBeginRunActionTool,
                          public ISteppingActionTool
  {

    public:

      /// Standard constructor
      FastIDKillerTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

      virtual IBeginRunAction* getBeginRunAction() override final
      { return static_cast<IBeginRunAction*>( getAction() ); }

      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }

      virtual StatusCode finalize() override;

    protected:

      virtual std::unique_ptr<FastIDKiller> makeAction() override final;

    private:

      FastIDKiller::Config m_config;

  }; // class FastIDKillerTool

} // namespace G4UA

#endif
