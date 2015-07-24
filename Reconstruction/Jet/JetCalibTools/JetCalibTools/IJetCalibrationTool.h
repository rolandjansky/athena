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

#include "PATInterfaces/CorrectionCode.h"
#include "AsgTools/IAsgTool.h"
#include "JetInterface/IJetModifier.h"
#include "JetInterface/ISingleJetModifier.h"

//EDM includes
#include "xAODJet/Jet.h"

//Package includes
#include "JetCalibTools/JetEventInfo.h"

class IJetCalibrationTool : virtual public asg::IAsgTool {

  ASG_TOOL_INTERFACE( IJetCalibrationTool )

public:

  /// Initialize the tool.
  virtual StatusCode initializeTool(const std::string& name) = 0;

  /// Apply calibration to a jet
  virtual StatusCode applyCalibration(xAOD::Jet& jet) const = 0;

  /// ASG interface to apply calibartaion to a jet.
  virtual CP::CorrectionCode applyCorrection(xAOD::Jet& jet) = 0;

  /// Make a deep copy of the jet and the calibrate the copy.
  virtual CP::CorrectionCode calibratedCopy(const xAOD::Jet& jet, xAOD::Jet*& jetReceivingCalibration) = 0;

  /// ASG interface for copy plus cliabrate.
  virtual CP::CorrectionCode correctedCopy(const xAOD::Jet& jet, xAOD::Jet*& jetReceivingCalibration) = 0;

  /// Jet modifier interface to calibrate all jets in a container.
  virtual int modify(xAOD::JetContainer& jets) const = 0;

  /// Jet modifier interface to calibrate a single jet.
  virtual int modifyJet(xAOD::Jet& jet) const = 0;

};

#endif
