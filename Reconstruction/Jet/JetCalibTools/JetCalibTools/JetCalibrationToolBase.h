///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCalibrationToolBase.h 
// Header file for class JetCalibrationToolBase
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETCALIBTOOLS_JETCALIBRATIONTOOLBASE_H
#define JETCALIBTOOLS_JETCALIBRATIONTOOLBASE_H 1

//Interface includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "PATInterfaces/CorrectionTool.h"

//EDM includes
#include "xAODJet/JetContainer.h"

// ROOT includes
#include <TSystem.h>
#include <TEnv.h>
#include <TLorentzVector.h>

//Package includes
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetEventInfo.h"

class JetCalibrationToolBase
  : public asg::AsgTool,
    virtual public ::IJetCalibrationTool
{ 

  ASG_TOOL_CLASS( JetCalibrationToolBase, IJetCalibrationTool )

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public:
  friend class JetCalibrationTool;

  // Default constructor: 
  JetCalibrationToolBase();

  /// Constructor with parameters: 
  JetCalibrationToolBase(const std::string& name);

  /// Destructor: 
  virtual ~JetCalibrationToolBase(); 

  //default initializeTool()
  virtual StatusCode initialize() { return StatusCode::FAILURE; }

  virtual StatusCode applyCalibration(xAOD::JetContainer&) const { return StatusCode::SUCCESS; }

  virtual void setUnitsGeV(bool useGeV) { if (useGeV) m_GeV=1; else m_GeV=1000; }

  // Retrieve pTmax from in situ corrections
  virtual VecD retrieveEtaIntercalPtMax(){VecD tmp;return tmp;}
  virtual VecD retrieveAbsoluteInsituPtMax(){VecD tmp;return tmp;}
  
  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
protected:
  /// This is where the actual calibration code goes.
  virtual StatusCode calibrateImpl(xAOD::Jet&, JetEventInfo&) const;

  virtual TString FindFile(TString filename);

  virtual StatusCode setStartP4(xAOD::Jet& jet) const;

protected:
  static double m_GeV;
  TString m_jetStartScale;

private:
  std::string m_jetAlgo;
  std::string m_config;
  std::string m_calibSeq;
  std::string m_calibAreaTag;
  bool m_isData;
  std::string m_dir;

  TString m_basePath;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !JETCALIBTOOLS_JETCALIBRATIONTOOLBASE_H
