// emacs this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef APPLYJETCALIBRATION_APPLYJETCALI_H
#define APPLYJETCALIBRATION_APPLYJETCALI_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"

#include "ApplyJetCalibration/ApplyJetCalibration.h"
#include <string>

class Jet;

class ApplyJetCalib : 
  public asg::AsgTool ,
  virtual public IJetModifier 
{
  ASG_TOOL_CLASS(ApplyJetCalib, IJetModifier)
public:
  ApplyJetCalib( const std::string& s1 ) ;  


  virtual StatusCode initialize()  ;

  // virtual StatusCode initializeEvent()  ;
  virtual int modify(xAOD::JetContainer& jets) const ;



protected:

  // virtual bool compute_corrections(const Jet* jet_in );
  // virtual bool correct_4mom(Jet* jet_in);

  JetAnalysisCalib::JetCalibrationTool *m_applyJES;


  std::string m_algoType;
  std::string m_configfile;
  std::string m_rhoKey;
  bool m_isData;

  int m_npv;
  double m_mu;
  double m_rho;

  bool m_onlyOffset;
};
#endif
