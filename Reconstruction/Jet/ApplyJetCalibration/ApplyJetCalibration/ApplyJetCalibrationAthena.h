// emacs this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef APPLYJETCALIBRATION_APPLYJETCALIBRATIONATHENA_H
#define APPLYJETCALIBRATION_APPLYJETCALIBRATIONATHENA_H

#include "JetCalibTools/JetCalibratorAthena.h"
#include "ApplyJetCalibration/ApplyJetCalibration.h"
#include <string>

class Jet;

class ApplyJetCalibrationAthena : public JetCalibratorAthena
{
public:
  ApplyJetCalibrationAthena( const std::string& s1, const std::string& s2, const IInterface* i ) ;  
  virtual ~ApplyJetCalibrationAthena(){}


  virtual StatusCode initialize()  ;

  virtual StatusCode initializeEvent()  ;



protected:

  virtual bool compute_corrections(const Jet* jet_in );
  virtual bool correct_4mom(Jet* jet_in);

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
