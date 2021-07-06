/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Asg wrapper around the MissingMassCalculator
// author Quentin Buat <quentin.buat@no.spam.cern.ch>
#ifndef DITAUMASSTOOLS_IMISSINGMASSTOOL_H
#define DITAUMASSTOOLS_IMISSINGMASSTOOL_H


#include "PATInterfaces/CorrectionCode.h"
#include "AsgTools/IAsgTool.h"

// ROOT includes
#include "TLorentzVector.h"
#include "TVector2.h"


// EDM Include
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingET.h"
#include "xAODBase/IParticle.h"

// fwd declarations
class TLorentzVector;
class TVector2;

class IMissingMassTool : public virtual asg::IAsgTool

{

  // Declare the interface that the class provides 
  ASG_TOOL_INTERFACE(IMissingMassTool)

  public:

  /// virtual destructor
  virtual ~IMissingMassTool() {};
  
// generic method
  virtual CP::CorrectionCode  apply(const xAOD::EventInfo& ei,
				    const xAOD::IParticle* part1,
				    const xAOD::IParticle* part2,
				    const xAOD::MissingET* met,
				    const int & njets)=0;
  
  virtual void calculate(const xAOD::EventInfo & ei, 
			 const TLorentzVector & vis_tau1,
			 const TLorentzVector & vis_tau2,
			 const int & tau1_decay_type,
			 const int & tau2_decay_type,
			 const xAOD::MissingET & met,
			 const int & njets)=0;


  virtual double GetFitStatus(const int method)=0;
  virtual double GetFittedMass(const int method)=0;
  virtual double GetFittedMassErrorUp(int method)=0;
  virtual double GetFittedMassErrorLow(int method)=0;
  virtual TLorentzVector GetResonanceVec(int method) = 0;
  virtual TVector2 GetFittedMetVec(int method) = 0;
  virtual TLorentzVector GetNeutrino4vec(int method, int index) = 0;
  virtual TLorentzVector GetTau4vec(int method, int index) = 0;
  virtual int GetNNoSol()=0;
  virtual int GetNMetroReject()=0;
  virtual int GetNSol()=0;

};
#endif
