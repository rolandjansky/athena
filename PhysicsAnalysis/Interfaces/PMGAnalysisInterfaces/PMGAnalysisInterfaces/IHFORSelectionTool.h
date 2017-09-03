/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HFORTOOLS_IHFORSELECTIONTOOL_H
#define HFORTOOLS_IHFORSELECTIONTOOL_H 1

#include "AsgTools/IAsgTool.h"

#include "PMGAnalysisInterfaces/HFORTypes.h"

class IHFORSelectionTool : public virtual asg::IAsgTool {
   public:
      ASG_TOOL_INTERFACE( IHFORSelectionTool ) //declares the interface to athena

      //This method will signal the event to be kept (true) or to be removed (false)
      //based on the overlap removal result
      virtual bool         isSelected() = 0 ;

      //Helper to get back the current sample type
      virtual HFORType     getSampleType() = 0 ;

      //Helper to get back the name of the current sample
      //(light, bb, cc, c or unknown)
      virtual std::string  getSampleName() = 0 ;

      // Return a string with the LAST type of event classification
      //(bb, cc, c, light, kill or unknown)
      virtual HFORType     getDecisionType() = 0 ;

};

#endif //> !HFORTOOLS_IHFORSELECTIONTOOL_H
