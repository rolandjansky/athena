/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_META_HH
#define SAMPLE_HANDLER_META_HH

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <string>
#include <TNamed.h>

namespace SH
{
  /// \brief the debugging info of this object
  ///
  /// \return a string representation of the object content
  /// \param obj the object to be presented
  /// \param verbosity the level of verbosity (higher number -> more
  ///   information)
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   out of memory II
  /// \relates Meta
  std::string dbg (const Meta& obj, unsigned verbosity = 0);



  /// \brief A base class for classes that implement
  /// arbitrary meta-data.
  class Meta : public TNamed
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard destructor
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~Meta ();



    //
    // protected interface
    //

    /// \brief standard constructor
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  protected:
    Meta (const std::string& name);



    //
    // private interface
    //

    ClassDef(Meta, 1);
  };
}

#endif
