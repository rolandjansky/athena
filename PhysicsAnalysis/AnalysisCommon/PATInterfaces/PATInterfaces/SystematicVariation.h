/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef PATINTERFACES_SYSTEMATIC_VARIATION_H
#define PATINTERFACES_SYSTEMATIC_VARIATION_H

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

#include <iosfwd>
#include <set>
#include <string>

/// rationale: standard comparison operator for use by std::set
//bool operator < (const CP::SystematicVariation& a, const CP::SystematicVariation& b);
//bool operator == (const CP::SystematicVariation& a, const CP::SystematicVariation& b);


namespace CP
{

  // Pulling global ops into CP. Not ideal.
  using ::operator<;
  using ::operator==;

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
#ifndef __MAKECINT__
#ifndef __ROOTCLING__
    SystematicVariation (const std::string& val_basename, CONTINUOUS_ARG);
#endif // not __ROOTCLING__
#endif // not __MAKECINT__


    /// \brief constructor for toy systematics
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    static SystematicVariation
    makeToyVariation (const std::string& basename,
		      unsigned toyIndex, float toyScale);


    /// \brief constructor for toy systematics ensemble
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    static SystematicVariation
    makeToyEnsemble (const std::string& basename);


    /// \brief constructor for continuous systematics ensemble
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    static SystematicVariation
    makeContinuousEnsemble (const std::string& basename);


    /// Named constructor for continuous systematics.
    ///
    /// rationale: Introduced because the enum-based constructor was
    ///   getting confused with the float one above in PyROOT.
  public:
    static SystematicVariation makeContinuous (const std::string& basename);


    // TODO: re-evaluate the usefulness of this constructor
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
    SystematicVariation (const std::set<CP::SystematicVariation>& systematics,
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


    /// \brief unpack the toy variation
    /// \return the index of the toy, and the scale
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   parse errors\n
    ///   not a toy variation
  public:
    std::pair<unsigned,float> getToyVariation () const;


    /// \brief whether this represents a toy ensemble
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    bool isToyEnsemble () const;


    /// \brief whether this represents a continuous ensemble
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    bool isContinuousEnsemble () const;


    /// \brief whether this represents any form of ensemble
    ///
    /// this can be used to check whether this is a valid variation to
    /// pass into \ref ISystematicsTool::applySystematicVariation
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    bool isEnsemble () const;


    /// \brief whether this is an ensemble that contains the given
    /// systematic variation
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    bool ensembleContains (const SystematicVariation& sys) const;


    /// \brief whether this represents a toy variation
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    bool isToyVariation () const;


    /// \brief whether this represents a continuous variation
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    bool isContinuousVariation () const;




    //
    // private interface
    //

    /// description: members directly corresponding to accessors
  private:
    std::string m_name;
  };

  std::ostream& operator << (std::ostream& str, const CP::SystematicVariation& obj);
}

#endif
