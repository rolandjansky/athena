///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  enum SoftTermParams{ Random=0,
		       PthardParam=1,
		       TSTParam=2
  };
}// end met namespace

class IMETSignificance :  virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMETSignificance)

public:


  ///////////////////////////////////////////////////////////////////
  // Variance MET  rebuilding
  ///////////////////////////////////////////////////////////////////

  // Convenience methods that creates MET terms internally
  // Version with single soft term
  virtual StatusCode varianceMET(xAOD::MissingETContainer* metCont, std::string jetTermName, std::string softTermName, std::string totalMETName)=0;
    
  virtual StatusCode RotateToPhi(float phi) = 0;

  ///////////////////////////////////////////////////////////////////
  // Additional utility commands
  ///////////////////////////////////////////////////////////////////

  virtual double GetMETOverSqrtSumET() const = 0 ; 
  virtual double GetMETOverSqrtHT   () const = 0 ; 
  virtual double GetSignificance()     const = 0 ;
  virtual double GetSigDirectional()   const = 0 ;
  virtual double GetRho()              const = 0 ;
  virtual double GetVarL()             const = 0 ;
  virtual double GetVarT()             const = 0 ;

};

#endif
