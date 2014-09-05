// $Id: SystematicCode.cxx 600738 2014-06-07 14:34:05Z krasznaa $

// System include(s):
#include <iostream>
#include <cstdlib>

// Local include(s):
#include "PATInterfaces/SystematicCode.h"

namespace {

   /// Private class for keeping track of unchecked systematic codes
   class UncheckedCounter {

   public:
      /// Default constructor
      UncheckedCounter() : m_unsupported( 0 ), m_ok( 0 ) {}
      /// Destructor
      ~UncheckedCounter() {
         // If we need to destruct the object, then there must have been
         // some unchecked systematic codes.
         std::cerr << "Warning in <CP::SystematicCode>:" << std::endl;
         std::cerr << "Warning in <CP::SystematicCode>: "
                   << "Unchecked correction codes encountered during the job"
                   << std::endl;
         if( m_unsupported ) {
            std::cerr << "Error in   <CP::SystematicCode>: "
                      << "Number of unchecked unsupporteds: "
                      << m_unsupported << std::endl;
         }
         if( m_ok ) {
            std::cerr << "Warning in <CP::SystematicCode>: "
                      << "Number of unchecked okays: "
                      << m_ok << std::endl;
         }
         // Let him/her know how to look up the unchecked codes in the easiest
         // way:
         std::cerr << "Warning in <CP::SystematicCode>: "
                   << "To fail on an unchecked code, call "
                   << "CP::SystematicCode::enableFailure() at the job's start"
                   << std::endl;
         std::cerr << "Warning in <CP::SystematicCode>:" << std::endl;
      }

      int m_unsupported; ///< Unchecked Unsupported codes
      int m_ok; ///< Unchecked Ok codes

   }; // class UncheckedCounter

} // private namespace

namespace CP {

   /// Application-wide setup of whether to fail on unchecked systematic codes
   ///
   /// By default applications will not fail on an uncecked return code, they
   /// will just keep track of how many return codes were left unchecked by
   /// the user.
   ///
   static bool s_failure = false;

   SystematicCode::SystematicCode( ESysCode code )
      : m_code( code ), m_checked( false ) {

   }

   SystematicCode::SystematicCode( const SystematicCode& parent )
      : m_code( parent.m_code ), m_checked( false ) {

      // The parent doesn't have to be checked anymore, whatever
      // its status was:
      parent.m_checked = true;
   }

   SystematicCode::~SystematicCode() {

      if( ! m_checked ) {
         // If we are supposed to fail, let's fail right away:
         if( s_failure ) {
            std::cerr << "Fatal in <CP::SystematicCode::~SystematicCode> "
                      << "Unchecked systematic code encountered" << std::endl;
            std::abort();
         }
         // Global variable for keeping track of unchecked systematic codes.
         // It gets deleted only at the end of the process.
         static ::UncheckedCounter s_counter;
         if( m_code == Unsupported ) {
            s_counter.m_unsupported += 1;
         } else if( m_code == Ok ) {
            s_counter.m_ok += 1;
         } else {
            std::cerr << "Fatal in <CP::SystematicCode::~SystematicCode> "
                      << "Unknown systematic code encountered" << std::endl;
            std::abort();
         }
      }
   }

   SystematicCode& SystematicCode::operator= ( const SystematicCode& rhs ) {

      // Check if anything needs to be done:
      if( this == &rhs ) {
         return *this;
      }

      // Copy the code:
      m_code = rhs.m_code;

      // Update the internal flags:
      m_checked = false;
      rhs.m_checked = true;

      return *this;
   }

   SystematicCode::ESysCode SystematicCode::code() const {

      setChecked();
      return m_code;
   }

   void SystematicCode::enableFailure() {

      s_failure = true;
      return;
   }

   void SystematicCode::disableFailure() {

      s_failure = false;
      return;
   }

} // namespace CP
