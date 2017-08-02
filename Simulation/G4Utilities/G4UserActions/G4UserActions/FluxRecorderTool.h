/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__FLUXRECORDERTOOL_H
#define G4USERACTIONS_G4UA__FLUXRECORDERTOOL_H

#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndRunActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/FluxRecorder.h"

namespace G4UA
{

  /// 	
  /// @class FluxRecorderTool
  /// @brief Tool which manages the FluxRecorder action.
  ///
  /// Create the FluxRecorder  for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class FluxRecorderTool : public ActionToolBase<FluxRecorder>,
                           public IBeginRunActionTool, public IEndRunActionTool,
                           public IEndEventActionTool, public ISteppingActionTool
  {

    public:
      /// standard tool ctor
      FluxRecorderTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// retrieves the BoR action
      virtual IBeginRunAction* getBeginRunAction() override final
      { return static_cast<IBeginRunAction*>( getAction() ); }
      /// retrieves the EoR action
      virtual IEndRunAction* getEndRunAction() override final
      { return static_cast<IEndRunAction*>( getAction() ); }
      /// retrieves the EoE action
      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }
      /// retrieves the stepping action
      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }
    protected:
      /// create action for this thread
      virtual std::unique_ptr<FluxRecorder> makeAction() override final;
    private:
  }; // class FluxRecorderTool

} // namespace G4UA
#endif
