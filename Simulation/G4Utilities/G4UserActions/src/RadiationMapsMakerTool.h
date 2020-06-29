/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__RADIATIONMAPSMAKERTOOL_H 
#define G4USERACTIONS_G4UA__RADIATIONMAPSMAKERTOOL_H 

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "RadiationMapsMaker.h"

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
  public UserActionToolBase<RadiationMapsMaker>
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

  protected:

    /// create action for this thread
    virtual std::unique_ptr<RadiationMapsMaker> makeAndFillAction(G4AtlasUserActions&) override final;

  private:
    /// Output Filename for the Radiation Maps
    std::string m_radMapsFileName;
    /// Radiation Map ranges and granularities
    RadiationMapsMaker::Config m_config;

  }; // class RadiationMapsMakerTool
 
} // namespace G4UA 
#endif
