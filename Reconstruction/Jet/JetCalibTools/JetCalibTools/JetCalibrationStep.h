// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/*
JetCalibrationStep
A base class for individual pieces of the jet calibration sequence

Author: Bill Balunas (balunas@cern.ch)
*/

#ifndef JETCALIBTOOLS_JETCALIBRATIONSTEP_H
#define JETCALIBTOOLS_JETCALIBRATIONSTEP_H 1

#include "xAODJet/JetContainer.h"
#include "JetCalibTools/JetEventInfo.h"
#include "AsgMessaging/AsgMessaging.h"
#include "AsgMessaging/Check.h"
#include "AsgMessaging/StatusCode.h"

class JetCalibrationStep : public asg::AsgMessaging
{ 

 public:

  JetCalibrationStep(const char* name = "JetCalibrationStep");
  virtual ~JetCalibrationStep(); 

  virtual StatusCode initialize() = 0;
  virtual StatusCode calibrate(xAOD::Jet&, JetEventInfo&) const = 0;

  virtual void setUnitsGeV(bool useGeV) { if (useGeV) m_GeV=1; else m_GeV=1000; }

  virtual StatusCode getNominalResolutionData(const xAOD::Jet&, double&) const;
  virtual StatusCode getNominalResolutionMC(const xAOD::Jet&, double&) const;

protected:

  virtual StatusCode setStartP4(xAOD::Jet& jet) const;

protected:
  double m_GeV;
  std::string m_jetStartScale;
  std::string m_name;

}; 

#endif
