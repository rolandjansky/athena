/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__RADIATIONMAPSMAKERTOOL_H 
#define G4USERACTIONS_G4UA__RADIATIONMAPSMAKERTOOL_H 

#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/RadiationMapsMaker.h"

namespace G4UA
{ 

  /// 	
  /// @class RadiationMapsMakerTool
  /// @brief Tool which manages the RadiationMapsMaker action.
  ///
  /// Create the RadiationMapsMaker  for each worker thread
  ///
  /// @author Sven Menke (based on Andrea Di Simone's FluxRecorderTool)
  ///

class RadiationMapsMakerTool: 
  public ActionToolBaseReport<RadiationMapsMaker>,
  public IG4RunActionTool,  
  public IG4SteppingActionTool
  {

  public:
    /// standard tool ctor
    RadiationMapsMakerTool(const std::string& type, 
			   const std::string& name,
			   const IInterface* parent);

    /// Initialize configurable properties 
    virtual StatusCode initialize() override final;

    /// Finalize and merge results from all threads
    virtual StatusCode finalize() override final;

    /// retrieves the run action
    virtual G4UserRunAction* getRunAction() override final
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// retrieves the stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final
    { return static_cast<G4UserSteppingAction*>( getAction() ); }

  protected:
    /// create action for this thread
    virtual std::unique_ptr<RadiationMapsMaker> makeAction() override final;

  private:
    /// Output Filename for the Radiation Maps
    std::string m_radMapsFileName;
    /// Radiation Map ranges and granularities
    RadiationMapsMaker::Config m_config;

  }; // class RadiationMapsMakerTool
 
} // namespace G4UA 
#endif
