///////////////////////// -*- C++ -*- /////////////////////////////
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

class IMETSignificance :  virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMETSignificance)

public:


  ///////////////////////////////////////////////////////////////////
  // Variance MET  rebuilding
  ///////////////////////////////////////////////////////////////////

  // Convenience methods that creates MET terms internally
  // Version with single soft term
    virtual StatusCode varianceMET(xAOD::MissingETContainer* metCont)=0;
    

  ///////////////////////////////////////////////////////////////////
  // Additional utility commands
  ///////////////////////////////////////////////////////////////////

};

#endif
