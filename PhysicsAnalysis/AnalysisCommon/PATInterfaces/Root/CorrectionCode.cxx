// $Id: CorrectionCode.cxx 600738 2014-06-07 14:34:05Z krasznaa $

// System include(s):
#include <iostream>
#include <cstdlib>

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"

namespace {

   /// Private class for keeping track of unchecked correction codes
   class UncheckedCounter {

   public:
      /// Default constructor
      UncheckedCounter() : m_error( 0 ), m_oov( 0 ), m_ok( 0 ) {}
      /// Destructor
      ~UncheckedCounter() {
         // If we need to destruct the object, then there must have been
         // some unchecked correction codes.
         std::cerr << "Warning in <CP::CorrectionCode>:" << std::endl;
         std::cerr << "Warning in <CP::CorrectionCode>: "
                   << "Unchecked correction codes encountered during the job"
                   << std::endl;
         if( m_error ) {
            std::cerr << "Error in   <CP::CorrectionCode>: "
                      << "Number of unchecked errors: "
                      << m_error << std::endl;
         }
         if( m_oov ) {
            std::cerr << "Warning in <CP::CorrectionCode>: "
                      << "Number of unchecked out-of-validity-ranges: "
                      << m_oov << std::endl;
         }
         if( m_ok ) {
            std::cerr << "Warning in <CP::CorrectionCode>: "
                      << "Number of unchecked okays: "
                      << m_ok << std::endl;
         }
         // Let him/her know how to look up the unchecked codes in the easiest
         // way:
         std::cerr << "Warning in <CP::CorrectionCode>: "
                   << "To fail on an unchecked code, call "
                   << "CP::CorrectionCode::enableFailure() at the job's start"
                   << std::endl;
         std::cerr << "Warning in <CP::CorrectionCode>:" << std::endl;
      }

      std::atomic<int> m_error; ///< Unchecked Error codes
      std::atomic<int> m_oov; ///< Unchecked OutOfValidityRange codes
      std::atomic<int> m_ok; ///< Unchecked Ok codes

   }; // class UncheckedCounter

} // private namespace

namespace CP {

   /// Application-wide setup of whether to fail on unchecked correction codes
   ///
   /// By default applications will not fail on an uncecked return code, they
   /// will just keep track of how many return codes were left unchecked by
   /// the user.
   ///
   static std::atomic<bool> s_failure = false;

   CorrectionCode::CorrectionCode( ErrorCode code )
      : m_code( code ), m_checked( false ) {

   }

   CorrectionCode::CorrectionCode( const CorrectionCode& parent )
      : m_code( parent.m_code ), m_checked( false ) {

      // The parent doesn't have to be checked anymore, whatever
      // its status was:
      parent.m_checked = true;
   }

   CorrectionCode::~CorrectionCode() {

      if( ! m_checked ) {
         // If we are supposed to fail, let's fail right away:
         if( s_failure ) {
            std::cerr << "Fatal in <CP::CorrectionCode::~CorrectionCode> "
                      << "Unchecked correction code encountered" << std::endl;
            std::abort();
         }
         // Global variable for keeping track of unchecked correction codes.
         // It gets deleted only at the end of the process.
         static ::UncheckedCounter s_counter;
         if( m_code == Error ) {
            s_counter.m_error += 1;
         } else if( m_code == OutOfValidityRange ) {
            s_counter.m_oov += 1;
         } else if( m_code == Ok ) {
            s_counter.m_ok += 1;
         } else {
            std::cerr << "Fatal in <CP::CorrectionCode::~CorrectionCode> "
                      << "Unknown correction code encountered" << std::endl;
            std::abort();
         }
      }
   }

   CorrectionCode& CorrectionCode::operator= ( const CorrectionCode& rhs ) {

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

   CorrectionCode::ErrorCode CorrectionCode::code() const {

      setChecked();
      return m_code;
   }

   void CorrectionCode::enableFailure() {

      s_failure = true;
      return;
   }

   void CorrectionCode::disableFailure() {

      s_failure = false;
      return;
   }

} // namespace CP
