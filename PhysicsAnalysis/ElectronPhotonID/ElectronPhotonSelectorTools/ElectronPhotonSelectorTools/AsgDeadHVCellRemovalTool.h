/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef __ASGDEADHVCELLREMOVALTOOL__
#define __ASGDEADHVCELLREMOVALTOOL__

#include "EgammaAnalysisInterfaces/IAsgDeadHVCellRemovalTool.h"
#include "AsgTools/AsgTool.h"
#include "xAODEgamma/EgammaFwd.h"

class AsgDeadHVCellRemovalTool : public asg::AsgTool,
				 virtual public IAsgDeadHVCellRemovalTool
{
  ASG_TOOL_CLASS2(AsgDeadHVCellRemovalTool, 
		  IAsgDeadHVCellRemovalTool,
		  asg::IAsgTool)

public: 
  /** Standard constructor */
  AsgDeadHVCellRemovalTool(std::string myname);

  /** Standard destructor */
  virtual ~AsgDeadHVCellRemovalTool();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();

  virtual bool accept( const xAOD::Egamma* eg ) const final;

  /** Accept using reference **/
  virtual bool accept( const xAOD::Egamma& part ) const final ;

private:

}; // End: class definition

#endif

