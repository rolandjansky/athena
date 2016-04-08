// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigDecisionTool.h 582206 2014-02-07 16:47:44Z krasznaa $
#ifndef TRIGDECISIONINTERFACE_ITRIGDECISIONTOOL_H
#define TRIGDECISIONINTERFACE_ITRIGDECISIONTOOL_H

// System include(s):
#include <string>
#include <vector>

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

/// ID of the ITrigDecisionTool interface
static const InterfaceID IID_ITrigDecisionTool( "ITrigDecisionTool", 1, 0 );

/**
 * \brief Abstract interface for obsolete version of the TrigDecisionTool.
 * \warning This is obsolete interface! Use the one below (in the Trig namesapce).
 *
 * Unfortunately some monitoring code still seems to use this interface...
 */
class ITrigDecisionTool : virtual public IAlgTool {

public:
   /// Function declaring the interface defined by this class
   static const InterfaceID& interfaceID() { return IID_ITrigDecisionTool; }

   /// @name Deprecated functions
   /// @{

   virtual bool isPassed( const std::string& chain_name ) const
      __attribute__ ((deprecated)) = 0;
   virtual bool isPassedThrough( const std::string& chain ) const
      __attribute__ ((deprecated)) = 0; 
   virtual bool isConfigured( const std::string& chain_name ) const
      __attribute__ ((deprecated)) = 0; 
   virtual bool isGroupPassed( const std::string& group_name ) const
      __attribute__ ((deprecated)) = 0; 
   virtual bool isStreamPassed( const std::string& stream_name ) const
      __attribute__ ((deprecated)) = 0;
   virtual int getFullChainPrescaleFactor( const std::string& chain_name ) const
      __attribute__ ((deprecated)) = 0;  
   virtual std::vector< std::string > getConfiguredChainNames() const
      __attribute__ ((deprecated)) = 0;      
   virtual std::map< std::string, std::vector< std::string > >
   getConfiguredGroups() const __attribute__ ((deprecated)) = 0;  
   virtual std::map< std::string, std::vector< std::string > >
   getConfiguredStreams() const __attribute__ ((deprecated)) = 0;

   /// @}

}; // class ITrigDecisionTool

/// ID of the Trig::ITrigDecisionTool interface
static const InterfaceID
IID_Trig_ITrigDecisionTool( "Trig::ITrigDecisionTool", 1, 0 );

namespace Trig {

   /**
    * @brief Basic interface for the TrigDecisionTool
    *
    * It is used by some packages that are not in AtlasTrigger or higher.
    *
    * $Revision: 582206 $
    * $Date: 2014-02-07 17:47:44 +0100 (Fri, 07 Feb 2014) $
    **/
   class ITrigDecisionTool : virtual public IAlgTool {

   public:
      /// Function declaring the interface defined by this class
      static const InterfaceID& interfaceID() {
         return IID_Trig_ITrigDecisionTool;
      }

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
