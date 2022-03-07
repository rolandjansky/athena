/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PATINTERFACES_CORRECTIONCODE_H
#define PATINTERFACES_CORRECTIONCODE_H

#include "CxxUtils/nodiscard.h"

namespace CP {

   /// Return value from object correction CP tools
   ///
   /// This class needs to be used in CP tools implementing object corrections
   /// when they implement the interface described in:
   /// https://cds.cern.ch/record/1667206
   /// In short, all such CP tools should implement the two following functions:
   ///
   /// <code>
   /// CorrectionCode Tool::applyCorrection( xAODObjectType& inputObject );<br/>
   /// CorrectionCode Tool::correctedCopy( const xAODObjectType& inputObject,<br/>
   ///                                     xAODObjectType*& outputObject );
   /// </code>
   ///
   /// @author Nils Krumnack <krumnack@iastate.edu>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 719663 $
   /// $Date: 2016-01-25 21:27:50 +0100 (Mon, 25 Jan 2016) $
   ///
   class ATLAS_NODISCARD CorrectionCode {

   public:
      /// Possible values for the correction code
      enum ErrorCode {
         Error = 0, ///< Some error happened during the object correction
         OutOfValidityRange = 1, ///< Input object is out of validity range
         Ok = 2 ///< The correction was done successfully
      };

      /// Constructor with a correction code
      CorrectionCode( ErrorCode code = Ok ) noexcept : m_code( code ) { }

      /// The code stored internally
      ErrorCode code() const noexcept {return m_code;}

      /// Automatic conversion to the enumeration value
      operator ErrorCode() const noexcept { return code(); }

      /// Ordering operator. To make it possible to use this type as an
      /// STL container key
      bool operator < ( const CorrectionCode& rhs ) const noexcept {
         return m_code < rhs.m_code;
      }

      /// Ignore the correction code
      void ignore() const noexcept { }

      /// Older functions for backward compatibility with the
      /// pre-nondiscard version of this class in which we tracked
      /// in the object itself whether the content was checked.  These
      /// may go away at some point.
      void setChecked() const noexcept { }
      static void enableFailure() noexcept {}
      static void disableFailure() noexcept {}

   private:
      ErrorCode m_code; ///< The stored correction code

   }; // class CorrectionCode

} // namespace CP

namespace asg
{
  /// \brief this is an internal traits class for status codes used by
  /// the ANA_CHECK* macros
  template<typename T> struct CheckHelper;

  template<> struct CheckHelper<CP::CorrectionCode>
  {
    /// \brief whether the status code reports a success
    static inline bool isSuccess (const CP::CorrectionCode& sc) {
      return sc == CP::CorrectionCode::Ok; }

    /// \brief whether the status code reports out of validity
    static inline bool isOutOfValidityRange (const CP::CorrectionCode& sc) {
      return sc == CP::CorrectionCode::OutOfValidityRange; }

    /// \brief produce a status code to report success
    static inline CP::CorrectionCode successCode () {
      return CP::CorrectionCode::Ok;}

    /// \brief produce a status code to report failure
    static inline CP::CorrectionCode failureCode () {
      return CP::CorrectionCode::Error;}
  };
}

#endif // not PATINTERFACES_CORRECTIONCODE_H
