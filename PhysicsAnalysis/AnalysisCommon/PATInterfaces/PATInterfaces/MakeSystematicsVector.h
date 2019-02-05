/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef PATINTERFACES_MAKE_SYSTEMATICS_VECTOR_H
#define PATINTERFACES_MAKE_SYSTEMATICS_VECTOR_H

#include <PATInterfaces/Global.h>

#include <PATInterfaces/SystematicSet.h>
#include <map>
#include <string>
#include <vector>

namespace CP
{
  /// \brief This class handles turning the list of systematics into
  /// the actual list of nuisance parameter points to evaluate.
  ///
  /// This is meant as a placeholder for a generic tool to be
  /// developed by the statistics forum (or as a prototype for it).
  ///
  /// For now I decided to keep it as a single class, but there are
  /// other options, e.g. the member \ref
  /// MakeSystematicsVector::Config could be made a class that the
  /// user configures directly and then passes in.  However, for now
  /// this approach seems better, as it hides some of the mechanics
  /// from the user and gives me more freedom on the backend side.

  class MakeSystematicsVector
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard default constructor
    /// \par Guarantee
    ///   no-fail
  public:
    MakeSystematicsVector ();


    /// \brief the list of nuisance parameter points generated with
    /// the given label
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   unknown label
    /// \pre calculate() has been called
  public:
    const std::vector<SystematicSet>&
    result (const std::string& label) const;


    /// \brief fill in \ref result
    /// \param sysList the list of systematics to use, usually the
    ///   list of recommended systematics from your CP tools,
    ///   framework or the systematics registry
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   configuration errors
  public:
    void calc (const SystematicSet& sysList);


    /// \brief finish configuration for this group and add a new one
    /// \param val_label the label for the new group
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void addGroup (const std::string& val_label);


    /// \brief set the pattern for the current group
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void setPattern (const std::string& val_pattern);


    /// \brief set the number of sigmas to vary this group by
    ///
    /// Normally we are using just +/-1 sigma variations, but if the
    /// systematics are very small that can get lost in the
    /// statistical jitter.  For those cases it is better to do a
    /// multi-sigma variation and then scale it back to +/-1 sigma
    /// thereby reducing the statistical jitter introduced into the
    /// systematic.  A traditional scaling factor for these cases is
    /// to scale by five sigma.
    ///
    /// Please note that if you do this, you normally only want to do
    /// this for small systematics for which statistical jitter is an
    /// issue.  For large systematics there is a legitimate concern
    /// that a 5 sigma variation won't be 5 times the size of a 1
    /// sigma variation, introducing a different kind of bias.  To
    /// that end, if you use this, you should normally put the small
    /// systematics into a separate group from your regular
    /// systematics (which then also makes it easier for you to know
    /// which one to scale down).
    /// \par Guarantee
    ///   no-fail
    /// \pre val_sigma > 0
  public:
    void setSigma (float val_sigma);


    /// \brief set the number of toys to run for this group
    ///
    /// This is a specialized mechanism pioneered for the e/gamma and
    /// muon scale factors.  Instead of evaluating a large number of
    /// systematics separately, it allows to vary all of them at the
    /// same time repeatedly, so that instead of hundreds of
    /// systematic variations you only have to perform maybe 10 or 20
    /// "toy" variations.  You then just take the spread of the
    /// variations as the overall systematic uncertainty from the
    /// toys.  There are reports that compared to the "old" method
    /// this can yield a factor five reduction in systematic
    /// uncertainty without increasing the number of systematic
    /// variations to evaluate.
    ///
    /// Please note that this approach requires a more expert handling
    /// than "regular" systematics.  The main point here is that the
    /// "toy" variations need a different post-processing than regular
    /// variations, i.e. you need to look at the spread between the
    /// output histograms for the different "toys" and use that to
    /// construct the combined systematic.  The e/gamma group
    /// currently (Oct 15) provides a tool for that.  For Bayesian
    /// marginalization you may alternatively consider to integrate
    /// the "toy" variations directly into the integral over nuisance
    /// parameter space to extract more information than you could
    /// with a single systematic.
    ///
    /// A general concern for all methods that reduce the number of
    /// nuisance parameters (including the "toy" approach) is if you
    /// use profiling and are able to constrain the "toy" systematic,
    /// in which case we generally assume that such an approach is
    /// invalid.  Similar precautions should be taken when used with
    /// Bayesian marginalization.  This is typically of little
    /// practical concern as long as the systematic in question is
    /// small, but it is something that you should check for and that
    /// you should include in your supporting documentation.
    ///
    /// The "toy" approach to systematics evaluation introduces an
    /// additional statistical jitter into your systematics, due to
    /// the sampling fluctuations of your "toy" variations.  Naturally
    /// this uncertainty decreases if you use a larger number of "toy"
    /// variations.  We currently (Oct 15) provide no recommendations
    /// for evaluating the size of that uncertainty or for determining
    /// whether your chosen number of "toys" is sufficient.  Anecdotal
    /// evidence from the e/gamma group suggests that for evaluating
    /// their scale factor systematic as little as 10 or 20 "toys" may
    /// be sufficient.  However, this will vary depending on your
    /// analysis and the systematics you use the "toys" for.
    ///
    /// If you decide to go for a large number of "toy" variations it
    /// may be better to go for a "traditional" evaluation of your
    /// systematics instead, as that can cut down on the
    /// aforementioned statistical jitter by interpolating between the
    /// variations.  The number of variations needed for the
    /// traditional approach to be better (or even feasible) will
    /// depend on the interpolation algorithm used and is likely to
    /// improve as we implement better interpolation algorithms.


    /// \par Guarantee
    ///   no-fail
  public:
    void setToys (unsigned val_toys);


    /// \brief set this group as the default, i.e. the group
    /// containing the nominal variation
    /// \par Guarantee
    ///   no-fail
  public:
    void useForNominal ();



    //
    // private interface
    //

    /// \brief the value of \ref result
  private:
    std::map<std::string,std::vector<SystematicSet>> m_result;


    /// \brief the configuration for the given group
  private:
    struct GroupConfig
    {
      /// \brief the label for this group
    public:
      std::string label;

      /// \brief the value set by \ref setPattern
    public:
      std::string pattern;

      /// \brief the value set by \ref setSigma
    public:
      float sigma = 1;

      /// \brief the value set by \ref setToys, or 0 if this isn't
      /// used with toys
    public:
      unsigned toys = 0;
    };

    /// \brief the configuration on a per-group basis
  private:
    std::vector<GroupConfig> m_config;


    /// \brief the group for which \ref useForNominal was set
  private:
    std::string m_useForNominal;


    /// \brief make the list of base systematics for \ref calc
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  private:
    std::vector<std::map<std::string,std::vector<SystematicVariation> >>
    calcBaseSys (const SystematicSet& sysList);
  };
}

#endif
