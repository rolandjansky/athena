/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMABASETOOL_H
#define EGAMMATOOLS_EGAMMABASETOOL_H

/**
  @class  egammaBaseTool
          Base Tool for all egamma tool that require 
          an egamma object as its input.  Hence the specific versions
          will have access to all the framework services in addition
          of being passed an 'egamma' object.  The derived 
          tool must implement an execute(egamma*) method.
  @author F. Derue, A. Kaczmarska
*/

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgTool.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODEgamma/EgammaFwd.h"
 

class egammaBaseTool : public AthAlgTool, virtual public IegammaBaseTool
{

 public:

  /** @brief Default constructor*/
  egammaBaseTool(const std::string& name, const std::string& description, 
		 const IInterface* parent);
  /** @brief destructor*/
  virtual ~egammaBaseTool();

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  virtual StatusCode execute(xAOD::Egamma* eg); 
  /** @brief execute method for methods working on containers*/
  virtual StatusCode contExecute(); 
 
};

#endif // EGAMMATOOLS_EGAMMABASETOOL_H
