///////////////////////// -*- C++ -*- /////////////////////////////
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// IMETSignificance.h
// Header file for interface IMETSignificance
//
// This is the tool that builds the MET significance at analysis level
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Francavilla
///////////////////////////////////////////////////////////////////

#ifndef METINTERFACE_IMETSIGNIFICANCE_H
#define METINTERFACE_IMETSIGNIFICANCE_H

#include "AsgTools/IAsgTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODJet/JetContainer.h"

namespace met {
  // Soft term parameterizations
  enum SoftTermParams{
    Random      = 0,
		PthardParam = 1,
		TSTParam    = 2
  };
  enum METSignificanceResoTerms{
    ResoNone = 0,
		ResoJet  = 1,
    ResoSoft = 2,
		ResoEle  = 3,
		ResoMuo  = 4,
		ResoPho  = 5,
		ResoTau  = 6
  };
}

class IMETSignificance :  virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMETSignificance)

public:


  ///////////////////////////////////////////////////////////////////
  // Variance MET  rebuilding
  ///////////////////////////////////////////////////////////////////

  // Convenience methods that creates MET terms internally
  // Version with single soft term
  virtual StatusCode varianceMET(xAOD::MissingETContainer* metCont, float avgmu, std::string jetTermName, std::string softTermName, std::string totalMETName) = 0;
    
  // Rotates the phi direction of the object resolutions & recomputes the MET significance
  virtual StatusCode RotateToPhi(float phi) = 0;

  // Subtracts the vector lambda from the MET & recomputes the MET signficance in new MET - lambda direction
  virtual StatusCode SetLambda(const float px, const float py, const bool GeV=true) = 0;

  ///////////////////////////////////////////////////////////////////
  // Additional utility commands
  ///////////////////////////////////////////////////////////////////

  virtual double GetMETOverSqrtSumET()       const = 0 ; 
  virtual double GetMETOverSqrtHT   ()       const = 0 ; 
  virtual double GetSignificance()           const = 0 ;
  virtual double GetSigDirectional()         const = 0 ;
  virtual double GetRho()                    const = 0 ;
  virtual double GetVarL()                   const = 0 ;
  virtual double GetVarT()                   const = 0 ;
  virtual double GetTermVarL(const int term) const = 0 ;
  virtual double GetTermVarT(const int term) const = 0 ;

};

#endif
