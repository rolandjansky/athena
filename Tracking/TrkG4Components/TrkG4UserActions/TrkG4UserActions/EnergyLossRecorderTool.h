/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__ENERGYLOSSRECORDERTOOL_H
#define TRKG4USERACTIONS_G4UA__ENERGYLOSSRECORDERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "TrkG4UserActions/EnergyLossRecorder.h"

namespace Trk
{
  class IPositionMomentumWriter;
}

namespace G4UA
{

  /// @class EnergyLossRecorderTool
  /// @brief A class to manage EnergyLossRecorder actions
  ///
  /// creates one instance of the action per thread
  ///
  /// @author Andrea Di Simone

  class EnergyLossRecorderTool : public UserActionToolBase<EnergyLossRecorder>
  {

  public:
    /// standard tool ctor
    EnergyLossRecorderTool(const std::string& type, const std::string& name,
                           const IInterface* parent);
    /// Athena initialize method
    virtual StatusCode initialize() override final;
  protected:
    /// creates the instance for this thread
    virtual std::unique_ptr<EnergyLossRecorder>
    makeAndFillAction(G4AtlasUserActions&) override final;
  private:
    /// stores the config obtained from the python side
    EnergyLossRecorder::Config m_config;
    ToolHandle<Trk::IPositionMomentumWriter> m_pmWriter;
  }; // class EnergyLossRecorderTool

} // namespace G4UA

#endif
