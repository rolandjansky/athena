/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_STATUS_CODE_HH
#define EVENT_LOOP_STATUS_CODE_HH

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a class that describes the status of a
/// function call.  The interface provided in this module is intended
/// for the general user.  The module is considered to be in the
/// pre-alpha stage.



#include <EventLoop/Global.h>

namespace EL
{
  class StatusCode
  {
    unsigned long m_code;
  public:
    enum {
      FAILURE = 0,
      SUCCESS = 1
      //RECOVERABLE = 2
    };

    /// Constructor.
    StatusCode(): m_code(SUCCESS) {}
    StatusCode( unsigned long code) : m_code(code){};
    bool operator == (unsigned long code) const {return m_code == code;};
    bool operator != (unsigned long code) const {return m_code != code;};
  };
}

namespace asg
{
  /// \brief this is an internal traits class for status codes used by
  /// the ANA_CHECK* macros
  template<typename T> struct CheckHelper;

  template<> struct CheckHelper<EL::StatusCode>
  {
    /// \brief whether the status code reports a success
    static inline bool isSuccess (const EL::StatusCode& sc) {
      return sc == EL::StatusCode::SUCCESS; }

    /// \brief produce a status code to report success
    static inline EL::StatusCode successCode () {
      return EL::StatusCode::SUCCESS;}

    /// \brief produce a status code to report failure
    static inline EL::StatusCode failureCode () {
      return EL::StatusCode::FAILURE;}
  };
}

#endif
