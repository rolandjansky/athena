// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigDecisionTool.h 624182 2014-10-25 21:44:42Z lheinric $
#ifndef TRIGDECISIONINTERFACE_ITRIGDECISIONTOOL_H
#define TRIGDECISIONINTERFACE_ITRIGDECISIONTOOL_H

// System include(s):
#include <string>
#include <vector>

// Framework include(s):
#include "AsgTools/IAsgTool.h"

namespace Trig {

   /**
    * @brief Basic interface for the TrigDecisionTool
    *
    * It is used by some packages that are not in AtlasTrigger or higher.
    *
    * $Revision: 624182 $
    * $Date: 2014-10-25 23:44:42 +0200 (Sat, 25 Oct 2014) $
    **/
   class ITrigDecisionTool : virtual public asg::IAsgTool {
     ASG_TOOL_INTERFACE(ITrigDecisionTool)

   public:
      /// Get the decision for a trigger chain
      /**
       * @see Trig::TrigDecisionTool::isPassed
       */
      virtual bool isPassed( const std::string& pattern ) const = 0;
      /// Get the decision for a trigger chain with a given condition
      /**
       * @see Trig::TrigDecisionTool::isPassed
       */
      virtual bool isPassed( const std::string& pattern,
                             unsigned int condition ) const = 0;
      /// Get expert-level information about a given trigger chain
      /**
       * @see Trig::DecisionAccess::isPassedBits
       */
      virtual unsigned int isPassedBits( const std::string& chain ) const = 0;

   }; // class ITrigDecisionTool

} // namespace Trig

#endif // TRIGDECISIONINTERFACE_ITRIGDECISIONTOOL_H
