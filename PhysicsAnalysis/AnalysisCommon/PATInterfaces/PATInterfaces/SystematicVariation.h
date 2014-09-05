#ifndef PATINTERFACES_SYSTEMATIC_VARIATION_H
#define PATINTERFACES_SYSTEMATIC_VARIATION_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module implements a class that manages the various
// representations of a single systematic variation.  The interface
// provided in this module is intended for the general user.  The
// module is considered to be in the pre-alpha stage.

// The current implementation isn't particularly efficient, in fact it
// is terribly inefficient, as most of the accessors involve a lot of
// string operations.  However, the current accessors should allow for
// more efficient implementations if needed.  And this class is likely
// only to be used during job startup, meaning an inefficient
// implementation is likely to have little to no effect on overall job
// execution time.  At the same time objects of this type are likely
// to be kept around for the entire runtime of the job, meaning the
// current small memory footprint should have advantages.



#include <PATInterfaces/Global.h>

#include <set>
#include <string>

namespace CP
{
  /// rationale: standard comparison operator for use by std::set
  bool operator < (const SystematicVariation& a, const SystematicVariation& b);

  bool operator == (const SystematicVariation& a, const SystematicVariation& b);

  class SystematicVariation
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard default constructor
    /// guarantee: no-fail
  public:
    SystematicVariation ();


    /// effects: constructor from the given members
    /// guarantee: strong
    /// failures: out of memory II
  public:
    SystematicVariation (const std::string& val_name);
    SystematicVariation (const std::string& val_basename,
			 const std::string& val_subvariation);
    SystematicVariation (const std::string& val_basename,
			 float val_parameter);


    /// effects: constructor for indicating continuous systematics
    /// guarantee: strong
    /// failures: out of memory II
  public:
    enum CONTINUOUS_ARG {CONTINUOUS};
    SystematicVariation (const std::string& val_basename, CONTINUOUS_ARG);


    /// effects: this constructor picks the systematic from the set of
    ///   systematics based on the basename.  if multiple systematics
    ///   with the same basename are in the set, it picks one of them.
    ///   if no systematic has this basename it is initialized to the
    ///   empty systematic.
    /// guarantee: strong
    /// failures: out of memory II
    /// rationale: this is meant to be used inside CP tools, so they
    ///   can more easily pick up systematics they care about (in
    ///   certain situations).
  public:
    SystematicVariation (const std::set<SystematicVariation>& systematics,
			 std::string val_basename);


    /// returns: whether this is an empty systematic, i.e. whether we
    ///   can safely ignore it
    /// guarantee: no-fail
    /// rationale: there are a number of cases in which a systematic
    ///   may become initialized to nothing, e.g. when choosing a 0
    ///   parameter.  this is an easy way to check for it.
  public:
    bool empty () const;


    /// description: the full systematics name, for use in strings,
    ///   etc.
    /// guarantee: no-fail
  public:
    const std::string& name () const;


    /// description: the base name, i.e. without the part that
    ///   specifies by how many sigmas we varied
    /// guarantee: strong
    /// failures: out of memory II
  public:
    std::string basename () const;


    /// description: the part of the name that indicates by how many
    ///   sigmas we varied
    /// guarantee: strong
    /// failures: out of memory II
    /// warning: an empty subvariation is *not* the same as requesting
    ///   the nuisance parameter to be 0.  it is a systematic that
    ///   doesn't have multiple subvariations
  public:
    std::string subvariation () const;


    /// description: the numeric parameter contained in the
    ///   subvariation(), or 0 if the subvariation can't be parsed
    ///   into a number
    /// guarantee: no-fail
  public:
    float parameter () const;


    //
    // private interface
    //

    /// description: members directly corresponding to accessors
  private:
    std::string m_name;
  };
}

#endif
