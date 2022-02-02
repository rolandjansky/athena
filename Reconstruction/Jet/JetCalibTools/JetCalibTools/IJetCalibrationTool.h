/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetCalibrationTool.h

//////////////////////////////////////////////////////////
/// class IJetCalibrationTool
/// 
/// Interface for the tool that applies jet calibrations.
///
//////////////////////////////////////////////////////////

#ifndef JETCALIBTOOL_IJETCALIBRATIONTOOL_H
#define JETCALIBTOOL_IJETCALIBRATIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "JetInterface/IJetModifier.h"

//EDM includes
#include "xAODJet/Jet.h"

//Package includes
#include "JetCalibTools/JetCalibUtils.h"

class IJetCalibrationTool : virtual public IJetModifier {

  ASG_TOOL_INTERFACE( IJetCalibrationTool )

public:

  /// Initialize the tool.
  virtual StatusCode initializeTool(const std::string& name) = 0;

  /// Apply calibration to a jet container (for `IJetModifier` interface).
  virtual StatusCode modify(xAOD::JetContainer& jets) const override final {return applyCalibration(jets);}

  /// Apply calibration to a jet container.
  virtual StatusCode applyCalibration(xAOD::JetContainer& jets) const = 0;

  // Retrieve pTmax from in situ corrections
  virtual VecD retrieveEtaIntercalPtMax(){VecD tmp; return tmp;}
  virtual VecD retrieveAbsoluteInsituPtMax(){VecD tmp; return tmp;}

  // Get the nominal resolution
  virtual StatusCode getNominalResolutionData(const xAOD::Jet&, double&) const { return StatusCode::FAILURE; }
  virtual StatusCode getNominalResolutionMC(  const xAOD::Jet&, double&) const { return StatusCode::FAILURE; }

};

#endif
