///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMETAssocToolBase.h 
// Header file for interface IMETToolBase
//
// This is the base class for all tools that calculate MET Terms
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
	
#ifndef IMETAssocToolBase_H
#define IMETAssocToolBase_H

#include "AsgTools/IAsgTool.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
	
class IMETAssocToolBase : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMETAssocToolBase)
	
  public:

  virtual StatusCode execute(xAOD::MissingETContainer*,xAOD::MissingETAssociationMap*) const = 0;
  
};
	
#endif
