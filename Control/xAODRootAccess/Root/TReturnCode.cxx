/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TReturnCode.cxx 598368 2014-05-22 10:23:54Z krasznaa $

// System include(s):
#include <iostream>

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "xAODRootAccess/tools/TReturnCode.h"
#include "xAODRootAccess/tools/Message.h"

namespace {

   /// Class used behind the scenes to keep track of the unchecked return codes
   ///
   /// One object of this type is created, just in case there are unchecked
   /// return codes encountered during the job. This single object is finally
   /// deleted at the end of the job, printing some complaints for the user.
   ///
   class TUncheckedCounter {

   public:
      /// Default constructor
      TUncheckedCounter() : m_uncheckedSuccess( 0 ), m_uncheckedFailure( 0 ) {}
      /// Destructor
      ~TUncheckedCounter() {
         // If we need to destruct the object, then there must have been
         // some unchecked return codes. Notice that we can't use the ROOT
         // print functions here, as by the time this object gets deleted, the
         // ROOT infrastructure may already be offline...
         std::cerr << "Warning in <xAOD::TReturnCode>:" << std::endl;
         std::cerr << "Warning in <xAOD::TReturnCode>: "
                   << "Unchecked return codes encountered during the job"
                   << std::endl;
         if( m_uncheckedSuccess ) {
            std::cerr << "Warning in <xAOD::TReturnCode>: "
                      << "Number of unchecked successes: "
                      << m_uncheckedSuccess << std::endl;
         }
         if( m_uncheckedFailure ) {
            std::cerr << "Error in   <xAOD::TReturnCode>: "
                      << "Number of unchecked failures: "
                      << m_uncheckedFailure << std::endl;
         }
         if( m_uncheckedRecoverable ) {
            std::cerr << "Warning in <xAOD::TReturnCode>: "
                      << "Number of unchecked recoverables: "
                      << m_uncheckedRecoverable << std::endl;
         }
         // Let him/her know how to look up the unchecked codes in the easiest
         // way:
         std::cerr << "Warning in <xAOD::TReturnCode>: "
                   << "To fail on an unchecked code, call "
                   << "xAOD::TReturnCode::enableFailure() at the job's start"
                   << std::endl;
         std::cerr << "Warning in <xAOD::TReturnCode>:" << std::endl;
      }

      /// Number of unchecked successful return codes
      int m_uncheckedSuccess;
      /// Number of unchecked failure return codes
      int m_uncheckedFailure;
      /// Number of unchecked recoverable return codes
      int m_uncheckedRecoverable;
   };

} // private namespace

namespace xAOD {

   /// Application-wide setup of whether to fail on unchecked return codes.
   ///
   /// By default applications will not fail on an uncecked return code, they
   /// will just keep track of how many return codes were left unchecked by
   /// the user.
   ///
   static bool s_failure = false;

   /// This constructor is called implicitly in 99% of all the use cases,
   /// by the code just returning an enumeration value from a function.
   /// Which gets converted into an xAOD::TReturnCode object automatically
   /// by the compiler.
   ///
   /// @param code The return code of the function
   ///
   TReturnCode::TReturnCode( EReturnCode code )
      : m_code( code ), m_checked( false ) {

   }

   /// The copy constructor makes sure that when we pass around
   /// return codes by value, it would be enough to just check the
   /// value of the last object in the chain. The objects that get
   /// copied can get destructed without them complaining that they
   /// were not checked.
   ///
   /// Even if the parent object was checked, the copied object will
   /// need to be checked as well. This is how we make sure that an expression
   /// like:
   ///
   /// <code>
   ///    xAOD::TReturnCode rc = bla();<br/>
   ///    if( rc.isFailure() ) {<br/>
   ///       std::cout << "Error" << std::endl;<br/>
   ///       return rc;<br/>
   ///    }
   /// </code>
   ///
   /// would work as expected.
   ///
   /// @param parent The parent object that needs to be copied
   ///
   TReturnCode::TReturnCode( const TReturnCode& parent )
      : m_code( parent.m_code ), m_checked( false ) {

      // The parent doesn't have to be checked anymore, whatever
      // its status was:
      parent.m_checked = true;
   }

   /// The destructor does the heavy lifting of the class. If the object
   /// is still unchecked at the time that it's destructed, it will either
   /// crash the application, or just register the missing check in the
   /// global registry.
   ///
   TReturnCode::~TReturnCode() {

      if( ! m_checked ) {
         // If we are supposed to fail, let's fail right away:
         if( s_failure ) {
            ::Fatal( "xAOD::TReturnCode::~TReturnCode",
                     XAOD_MESSAGE( "Unchecked return code encountered" ) );
         }
         // Global variable for keeping track of unchecked return codes.
         // It gets deleted only at the end of the process.
         static ::TUncheckedCounter s_counter;
         if( m_code == kSuccess ) {
            s_counter.m_uncheckedSuccess += 1;
         } else if( m_code == kFailure ) {
            s_counter.m_uncheckedFailure += 1;
         } else if( m_code == kRecoverable ) {
            s_counter.m_uncheckedRecoverable += 1;
         } else {
            ::Fatal( "xAOD::TReturnCode::~TReturnCode",
                     XAOD_MESSAGE( "Unknown return code encountered" ) );
         }
      }
   }

   TReturnCode& TReturnCode::operator= ( const TReturnCode& rhs ) {

      // Check if anything needs to be done:
      if( this == &rhs ) return *this;

      // Do the deed:
      m_code = rhs.m_code;
      m_checked = false;

      // The copied object doesn't have to be checked anymore, whatever
      // its status was:
      rhs.m_checked = true;

      // Return this object:
      return *this;
   }

   TReturnCode::EReturnCode TReturnCode::code() const {

      m_checked = true;
      return m_code;
   }

   bool TReturnCode::isSuccess() const {

      m_checked = true;
      return ( m_code == kSuccess );
   }

   bool TReturnCode::isFailure() const {

      m_checked = true;
      return ( m_code == kFailure );
   }

   bool TReturnCode::isRecoverable() const {

      m_checked = true;
      return ( m_code == kRecoverable );
   }

   void TReturnCode::enableFailure() {

      s_failure = true;
      return;
   }

   void TReturnCode::disableFailure() {

      s_failure = false;
      return;
   }

} // namespace xAOD
