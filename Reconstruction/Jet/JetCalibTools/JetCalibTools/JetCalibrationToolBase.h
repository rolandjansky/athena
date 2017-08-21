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

//JetInterface includes
#include "JetInterface/IJetModifier.h"
#include "JetInterface/ISingleJetModifier.h"

//Package includes
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibUtils.h"

class JetCalibrationToolBase
  : public asg::AsgTool,
    public CP::CorrectionTool< xAOD::JetContainer >,
    virtual public ::IJetCalibrationTool,
    virtual public ::IJetModifier,
    virtual public ::ISingleJetModifier
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

  ///Implement IJetCalibrationTool interface :
  virtual StatusCode applyCalibration(xAOD::Jet& jet) const;

  virtual CP::CorrectionCode applyCorrection(xAOD::Jet&) { return CP::CorrectionCode::Ok; }

  virtual CP::CorrectionCode calibratedCopy(const xAOD::Jet& inputJet, xAOD::Jet*& outputJet)  {
    return CP::CorrectionTool< xAOD::JetContainer >::correctedCopy( inputJet, outputJet );
  }

  virtual CP::CorrectionCode correctedCopy(const xAOD::Jet& inputJet, xAOD::Jet*& outputJet) {
    return calibratedCopy( inputJet, outputJet );
  }

  virtual int modify(xAOD::JetContainer&) const { return 0; }
  virtual int modifyJet(xAOD::Jet&) const { return 0; }

  virtual void setUnitsGeV(bool useGeV) { if (useGeV) m_GeV=1; else m_GeV=1000; }
  
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
