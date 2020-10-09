// Dear emacs, this is -*- c++ -*-
// $Id: SystematicCode.h 717600 2016-01-13 17:48:13Z krumnack $
#ifndef PATINTERFACES_SYSTEMATICCODE_H
#define PATINTERFACES_SYSTEMATICCODE_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.

namespace CP {

   /// Return value from CP tools supporting systematic variations
   ///
   /// This class needs to be used in CP tools that support systematic
   /// variations, and implement the interface described in:
   /// https://cds.cern.ch/record/1667206
   /// In short, all such CP tools should implement the two following functions:
   ///
   /// <code>
   /// SystematicCode Tool::applySystematicVariation(<br/>
   ///                 const std::set< SystematicVariation >& systConfig );<br/>
   /// bool Tool::isAffectedBySystematic( const SystematicVariation& s ) const;<br/>
   /// std::set< SystematicVariation > Tool::affectingSystematics(<br/>
   ///          const std::set< SystematicVariation >& inputSystList ) const;
   /// </code>
   ///
   /// @author Nils Krumnack <krumnack@iastate.edu>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 717600 $
   /// $Date: 2016-01-13 18:48:13 +0100 (Wed, 13 Jan 2016) $
   ///
   class [[nodiscard]] SystematicCode {

   public:
      /// Possible values for the systematic code
      enum ESysCode {
         Unsupported = 0, ///< The requested systematic is not supported
         Ok = 1 ///< The requested systematic will be applied
      };

      /// Constructor with the systematic code
      SystematicCode( ESysCode code );
      /// Copy constructor
      SystematicCode( const SystematicCode& parent );
      /// Destructor
      ~SystematicCode();

      /// Assignment operator
      SystematicCode& operator= ( const SystematicCode& rhs );

      /// The code stored internally
      ESysCode code() const;

      /// Automatic conversion to the enumeration value
      operator ESysCode() const { return code(); }

      /// Ordering operator. To make it possible to use this type as an
      /// STL container key
      bool operator < ( const SystematicCode& rhs ) const {
         return m_code < rhs.m_code;
      }

      /// Mark the correction code as checked, ignoring its value
      void setChecked() const { m_checked = true; }
      /// Ignore the correction code, marking it as checked
      void ignore() const { setChecked(); }

      /// Enable failure (with a backtrace) on an unchecked systematic code
      static void enableFailure();
      /// Disable failure (no backtrace) on an unchecked systematic code
      static void disableFailure();

   private:
      ESysCode m_code; ///< The stored systematic code
      mutable bool m_checked; ///< Checked status of the object

   }; // class SystematicCode

} // namespace CP

namespace asg
{
  template<typename T> struct CheckHelper;

  template<> struct CheckHelper<CP::SystematicCode>
  {
    /// \brief whether the status code reports a success
    static inline bool isSuccess (const CP::SystematicCode& sc) {
      return sc == CP::SystematicCode::Ok; }

    /// \brief produce a status code to report success
    static inline CP::SystematicCode successCode () {
      return CP::SystematicCode::Ok;}

    /// \brief produce a status code to report failure
    static inline CP::SystematicCode failureCode () {
      return CP::SystematicCode::Unsupported;}
  };
}

#endif // PATINTERFACES_SYSTEMATICCODE_H
