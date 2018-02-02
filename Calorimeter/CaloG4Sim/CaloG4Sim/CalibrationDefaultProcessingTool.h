/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOG4SIM_G4UA_CALOG4_CALIBRATIONDEFAULTPROCESSINGTOOL_H
#define CALOG4SIM_G4UA_CALOG4_CALIBRATIONDEFAULTPROCESSINGTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "CaloG4Sim/CalibrationDefaultProcessing.h"

namespace G4UA
{

  namespace CaloG4
  {

    /// @class CalibrationDefaultProcessingTool
    /// @brief Tool which manages CalibrationDefaultProcessing
    ///
    /// @author Andrea Di Simone
    ///
    class CalibrationDefaultProcessingTool:
      public UserActionToolBase<CalibrationDefaultProcessing>
    {

    public:
      /// Standard constructor
      CalibrationDefaultProcessingTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent);
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<CalibrationDefaultProcessing>
      makeAndFillAction(G4AtlasUserActions&) override final;
    private:
      /// python configuration
      CalibrationDefaultProcessing::Config m_config;
    }; // class CalibrationDefaultProcessingTool

  } // namespace CaloG4

} // namespace G4UA

#endif
