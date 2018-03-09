/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__HIPLARVOLUMEACCEPTTOOL_H
#define G4USERACTIONS_G4UA__HIPLARVOLUMEACCEPTTOOL_H

#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/HIPLArVolumeAccept.h"

namespace G4UA
{

  /// Tool which manages the HIPLArVolumeAccept action
  ///
  class HIPLArVolumeAcceptTool : public ActionToolBaseReport<HIPLArVolumeAccept>,
                                 public IG4SteppingActionTool,
                                 public IG4EventActionTool
  {

    public:

      /// Standard constructor
      HIPLArVolumeAcceptTool(const std::string& type, const std::string& name,
                             const IInterface* parent);

      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }

      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }

      virtual StatusCode finalize() override;

    protected:

      virtual std::unique_ptr<HIPLArVolumeAccept> makeAction() override final;

    //private:

    //  HIPLArVolumeAccept::Report m_report;

  }; // class HIPLArVolumeAcceptTool

} // namespace G4UA

#endif
