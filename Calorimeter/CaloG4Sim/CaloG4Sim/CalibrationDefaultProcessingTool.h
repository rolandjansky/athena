/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOG4SIM_G4UA_CALOG4_CALIBRATIONDEFAULTPROCESSINGTOOL_H
#define CALOG4SIM_G4UA_CALOG4_CALIBRATIONDEFAULTPROCESSINGTOOL_H
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "CaloG4Sim/CalibrationDefaultProcessing.h"

namespace G4UA{

  namespace CaloG4 {

    /// @class CalibrationDefaultProcessingTool
    /// @brief Tool which manages  CalibrationDefaultProcessing
    ///
    /// @author Andrea Di Simone
    ///

    class CalibrationDefaultProcessingTool:
      public ActionToolBase<CalibrationDefaultProcessing>,
      public IG4EventActionTool, public IG4SteppingActionTool
    {

    public:
      /// Standard constructor
      CalibrationDefaultProcessingTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// Retrieve the BoE action
      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }
      /// Retrieve the stepping action
      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }
      /// Gaudi interface management
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<CalibrationDefaultProcessing> makeAction() override final;
    private:
      /// python configuration
      CalibrationDefaultProcessing::Config m_config;
    }; // class CalibrationDefaultProcessingTool

  }// namespace CaloG4

} // namespace G4UA
#endif
