/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__HIPLARVOLUMEACCEPTTOOL_H
#define G4USERACTIONS_G4UA__HIPLARVOLUMEACCEPTTOOL_H

#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/HIPLArVolumeAccept.h"

namespace G4UA
{

  /// Tool which manages the HIPLArVolumeAccept action
  ///
  class HIPLArVolumeAcceptTool : public ActionToolBaseReport<HIPLArVolumeAccept>,
                                 public ISteppingActionTool,
                                 public IBeginEventActionTool,
                                 public IEndEventActionTool
  {

    public:

      /// Standard constructor
      HIPLArVolumeAcceptTool(const std::string& type, const std::string& name,
                             const IInterface* parent);

      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }

      virtual IBeginEventAction* getBeginEventAction() override final
      { return static_cast<IBeginEventAction*>( getAction() ); }

      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }

      virtual StatusCode finalize() override;

    protected:

      virtual std::unique_ptr<HIPLArVolumeAccept> makeAction() override final;

    //private:

    //  HIPLArVolumeAccept::Report m_report;

  }; // class HIPLArVolumeAcceptTool

} // namespace G4UA

#endif
