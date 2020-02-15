/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JETINTERFACE_IJETPROVIDER_H
#define JETINTERFACE_IJETPROVIDER_H

///***********************************************
/// IJetProvider is a dual-use tool interface for a tool
/// that builds a new jet collection :
///  - running fastjet
///  - grooming an existing JetContainer
///  - shallow copying an existing JetContainer
///  - ...

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

class IJetProvider : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetProvider)

public:

  /// Destructor.
  virtual ~IJetProvider() { };

  /// Method to build the collection and return it to the caller.
  virtual  xAOD::JetContainer* getJets() const =0;


};

#endif
