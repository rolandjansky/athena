// Dear emacs, this is -*- c++ -*-
// $Id: CorrectionCode.h 600738 2014-06-07 14:34:05Z krasznaa $
#ifndef PATINTERFACES_CORRECTIONCODE_H
#define PATINTERFACES_CORRECTIONCODE_H

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.

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
   /// $Revision: 600738 $
   /// $Date: 2014-06-07 16:34:05 +0200 (Sat, 07 Jun 2014) $
   ///
   class CorrectionCode {

   public:
      /// Possible values for the correction code
      enum ErrorCode {
         Error = 0, ///< Some error happened during the object correction
         OutOfValidityRange = 1, ///< Input object is out of validity range
         Ok = 2 ///< The correction was done successfully
      };

      /// Constructor with a correction code
      CorrectionCode( ErrorCode code = Ok );
      /// Copy constructor
      CorrectionCode( const CorrectionCode& parent );
      /// Destructor
      ~CorrectionCode();

      /// Assignment operator
      CorrectionCode& operator= ( const CorrectionCode& rhs );

      /// The code stored internally
      ErrorCode code() const;

      /// Automatic conversion to the enumeration value
      operator ErrorCode() const { return code(); }

      /// Ordering operator. To make it possible to use this type as an
      /// STL container key
      bool operator < ( const CorrectionCode& rhs ) const {
         return m_code < rhs.m_code;
      }

      /// Mark the correction code as checked, ignoring its value
      void setChecked() const { m_checked = true; }
      /// Ignore the correction code, marking it as checked
      void ignore() const { setChecked(); }

      /// Enable failure (with a backtrace) on an unchecked correction code
      static void enableFailure();
      /// Disable failure (no backtrace) on an unchecked correction code
      static void disableFailure();

   private:
      ErrorCode m_code; ///< The stored correction code
      mutable bool m_checked; ///< Checked status of the object

   }; // class CorrectionCode

} // namespace CP

#endif // not PATINTERFACES_CORRECTIONCODE_H
