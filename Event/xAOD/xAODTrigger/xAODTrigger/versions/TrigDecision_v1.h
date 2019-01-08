// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecision_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGDECISION_V1_H
#define XAODTRIGGER_VERSIONS_TRIGDECISION_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "AthContainers/AuxElement.h"

// TrigCompositeUtils::DecisionID
#include "xAODTrigger/TrigComposite.h"

namespace xAOD {

   /// Interface to the raw trigger decision information of the event
   ///
   /// This interface is used to access the raw trigger decision information
   /// in the event. It should only be used directly by experts, regular users
   /// should access the trigger decision information rather through the
   /// TrigDecisionTool.
   ///
   /// Note that the HLT information of the class is a bit redundant. The
   /// decision can also be derived from the other 4 bitsets that are stored.
   /// The reason for having it saved separately is to allow us to slim away
   /// the detailed bits during analysis, while still keeping the trigger
   /// decision needed for the physics analysis.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class TrigDecision_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TrigDecision_v1();

      /// @name Trigger configuration key accessors
      /// @{

      /// Get the Super Master Key describing this object
      uint32_t smk() const;
      /// Set the Super Master Key describing this object
      void setSMK( uint32_t value );

      /// @}

      /// @name Level-1 result accessors
      /// @{

      /// Get the bunch group code of the current event
      char bgCode() const;
      /// Set the bunch group code of the current event
      void setBGCode( char value );

      /// Get the Trigger After Veto bits
      const std::vector< uint32_t >& tav() const;
      /// Set the Trigger After Veto bits
      void setTAV( const std::vector< uint32_t >& value );

      /// Get the Trigger After Prescale bits
      const std::vector< uint32_t >& tap() const;
      /// Set the Trigger After Prescale bits
      void setTAP( const std::vector< uint32_t >& value );

      /// Get the Trigger Before Prescale bits
      const std::vector< uint32_t >& tbp() const;
      /// Set the Trigger Before Prescale bits
      void setTBP( const std::vector< uint32_t >& value );

      /// @}

      /// @name HLT result accessors
      /// @{

      /// Get a summary of all errors that happened during the LVL2 execution
      uint32_t lvl2ErrorBits() const;
      /// Set a summary of all errors that happened during the LVL2 execution
      void setLVL2ErrorBits( uint32_t value );

      /// Get a summary of all errors that happened during the EF execution
      uint32_t efErrorBits() const;
      /// Set a summary of all errors that happened during the EF execution
      void setEFErrorBits( uint32_t value );

      /// Is the LVL2 result truncated?
      bool lvl2Truncated() const;
      /// Set whether the LVL2 result is truncated
      void setLVL2Truncated( bool value );

      /// Is the EF result truncated?
      bool efTruncated() const;
      /// Set whether the EF result is truncated
      void setEFTruncated( bool value );

      /// Get the LVL2 physics decision bits
      const std::vector< uint32_t >& lvl2PassedPhysics() const;
      /// Set the LVL2 physics decision bits
      void setLVL2PassedPhysics( const std::vector< uint32_t >& value );

      /// Get the EF physics decision bits
      const std::vector< uint32_t >& efPassedPhysics() const;
      /// Set the EF physics decision bits
      void setEFPassedPhysics( const std::vector< uint32_t >& value );

      /// Get the LVL2 passed-raw bits
      const std::vector< uint32_t >& lvl2PassedRaw() const;
      /// Set the LVL2 passed-raw bits
      void setLVL2PassedRaw( const std::vector< uint32_t >& value );

      /// Get the EF passed-raw bits
      const std::vector< uint32_t >& efPassedRaw() const;
      /// Set the EF passed-raw bits
      void setEFPassedRaw( const std::vector< uint32_t >& value );

      /// Get the LVL2 pass-through bits
      const std::vector< uint32_t >& lvl2PassedThrough() const;
      /// Set the LVL2 pass-through bits
      void setLVL2PassedThrough( const std::vector< uint32_t >& value );

      /// Get the EF pass-through bits
      const std::vector< uint32_t >& efPassedThrough() const;
      /// Set the EF pass-through bits
      void setEFPassedThrough( const std::vector< uint32_t >& value );

      /// Get the LVL2 prescaled bits
      const std::vector< uint32_t >& lvl2Prescaled() const;
      /// Set the LVL2 prescaled bits
      void setLVL2Prescaled( const std::vector< uint32_t >& value );

      /// Get the EF prescaled bits
      const std::vector< uint32_t >& efPrescaled() const;
      /// Set the EF prescaled bits
      void setEFPrescaled( const std::vector< uint32_t >& value );

      /// Get the LVL2 resurrected bits
      const std::vector< uint32_t >& lvl2Resurrected() const;
      /// Set the LVL2 resurrected bits
      void setLVL2Resurrected( const std::vector< uint32_t >& value );

      /// Get the EF resurrected bits
      const std::vector< uint32_t >& efResurrected() const;
      /// Set the EF resurrected bits
      void setEFResurrected( const std::vector< uint32_t >& value );

      /// @}

   }; // class TrigDecision_v1

   std::ostream& operator<<(std::ostream& s, const xAOD::TrigDecision_v1& td);

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigDecision_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_TRIGDECISION_V1_H
