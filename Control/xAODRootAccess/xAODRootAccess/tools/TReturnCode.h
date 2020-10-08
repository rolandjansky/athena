// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TReturnCode.h 598368 2014-05-22 10:23:54Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TRETURNCODE_H
#define XAODROOTACCESS_TOOLS_TRETURNCODE_H


namespace xAOD {

   /// A light-weight version of StatusCode for the xAODRootAccess package
   ///
   /// In order to force users to check return codes from the classes of
   /// xAODRootAccess (especially xAOD::TEvent), the functions now return
   /// such objects instead of booleans. The user must check the value of
   /// the returned object in all cases, using one of the member functions
   /// of the class.
   ///
   /// The code then keeps track internally of unchecked return codes, and
   /// complains at the end of the job about them.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 598368 $
   /// $Date: 2014-05-22 12:23:54 +0200 (Thu, 22 May 2014) $
   ///
   class [[nodiscard]] TReturnCode {

   public:
      /// Enumeration listing the possible return codes
      enum EReturnCode {
         kFailure     = 0, ///< The function call failed
         kSuccess     = 1, ///< The function call succeeded
         /// The function call didn't succeed, but there's no fatal failure
         /// either
         kRecoverable = 2
      };

      /// Default constructor
      TReturnCode() : TReturnCode(kSuccess) {}
      /// Constructor with a return code value
      TReturnCode( EReturnCode code );
      /// Copy constructor
      TReturnCode( const TReturnCode& parent );
      /// Destructor
      ~TReturnCode();

      /// Assignment operator
      TReturnCode& operator= ( const TReturnCode& rhs );

      /// The code stored internally
      EReturnCode code() const;

      /// Function checking if the function call succeeded
      bool isSuccess() const;
      /// Function checking if the function call failed
      bool isFailure() const;
      /// Function checking if the function call produced a recoverable issue
      bool isRecoverable() const;

      /// Automatic conversion operator to a boolean
      operator bool() const { return isSuccess(); }

      /// Mark the return code as checked, ignoring it thereby
      void setChecked() const { m_checked = true; }
      /// Ignore the return code, marking it as checked
      void ignore() const { setChecked(); }

      /// Enable failure (with a backtrace) on an unchecked return code
      static void enableFailure();
      /// Disable failure (no backtrace) on an unchecked return code
      static void disableFailure();

   private:
      /// The code returned by a function
      EReturnCode m_code;
      /// Internal status flag of whether the code was checked by the user
      mutable bool m_checked;

   }; // class TReturnCode

} // namespace xAOD

namespace asg
{
  /// \brief this is an internal traits class for status codes used by
  /// the ANA_CHECK* macros
  template<typename T> struct CheckHelper;

  template<> struct CheckHelper<xAOD::TReturnCode>
  {
    /// \brief whether the status code reports a success
    static inline bool isSuccess (const xAOD::TReturnCode& sc) {
      return sc.isSuccess(); }

    /// \brief produce a status code to report success
    static inline decltype (xAOD::TReturnCode::kSuccess) successCode () {
      return xAOD::TReturnCode::kSuccess;}

    /// \brief produce a status code to report failure
    static inline decltype (xAOD::TReturnCode::kFailure) failureCode () {
      return xAOD::TReturnCode::kFailure;}
  };
}

#endif // XAODROOTACCESS_TOOLS_TRETURNCODE_H
