/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_IEVENTCLEANINGTOOL_H
#define ASSOCIATIONUTILS_IEVENTCLEANINGTOOL_H

// Framework includes
#include "AsgTools/IAsgTool.h"

// EDM includes
#include "xAODJet/JetContainer.h"


namespace ECUtils
{

  /// @class IEventCleaningTool
  /// @brief Interface for the event cleaning tool
  ///
  /// @author Julia Gonski <j.gonski@cern.ch>
  ///
  class IEventCleaningTool : public virtual asg::IAsgTool
  {

      /// Declare the interface
      ASG_TOOL_INTERFACE(IEventCleaningTool)

    public:

      /// Top-level method for performing full overlap-removal.
      /// The individual OR methods will be called in the recommended order,
      /// and the considered objects will be decorated with the output result.
      virtual bool
      acceptEvent(const xAOD::JetContainer* jets) const = 0;
  
      virtual int keepJet(const xAOD::Jet& jet) const =0; 
  
}; // class IEventCleaningTool

} // namespace ECUtils

#endif
