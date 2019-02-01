/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__JOB_CONFIG_H
#define EVENT_LOOP__JOB_CONFIG_H

#include <EventLoop/Global.h>

#include <TObject.h>
#include <memory>
#include <vector>

class StatusCode;

namespace EL
{
  /// \brief the job configuration that is independent of driver and
  /// dataset
  ///
  /// This is a work in progress (03 Feb 18), but essentially the idea
  /// is that this will contain he generic configuration of the user
  /// job (whatever that is), which will then be combined with the
  /// configuration for how it is to be run and with the configuration
  /// for which data to run over.
  ///
  /// For now (03 Feb 18) this is just the list of algorithms we
  /// configured.  That way that code is bundled in a single place and
  /// I can add some extra checks.  There used to be some issues with
  /// the grid driver not correctly detecting when algorithms could
  /// not be loaded, which should be avoided this way.

  class JobConfig final : public TObject
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   no-fail
  public:
    JobConfig () noexcept;


    /// \brief standard copy constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   algorithm clone failures
  public:
    JobConfig (const JobConfig& that);


    /// \brief standard move constructor
    /// \par Guarantee
    ///   no-fail
  public:
    JobConfig (JobConfig&& that) noexcept;


    /// \brief standard destructor
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~JobConfig () noexcept;


    /// \brief standard assignment operator
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   algorithm clone failures
  public:
    JobConfig& operator = (const JobConfig& that);


    /// \brief standard move assignment operator
    /// \par Guarantee
    ///   no-fail
  public:
    JobConfig& operator = (JobConfig&& that) noexcept;


    /// \brief standard swap
    /// \par Guarantee
    ///   no-fail
  public:
    void swap (JobConfig& that) noexcept;


    /// \brief add an algorithm
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   algorithm of same name already present
    /// \pre val_algorithm != nullptr
  public:
    ::StatusCode addAlgorithm (std::unique_ptr<Algorithm> val_algorithm);


    /// \brief get the algorithm with the given name, or nullptr if
    /// there is no algorithm with that name
    /// \par Guarantee
    ///   no-fail
  public:
    const Algorithm *getAlgorithm (const std::string& name) const noexcept;


    /// \brief extract the list of algorithms from this object
    ///
    /// This will check for any possible streaming errors before
    /// extracting.
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   streaming errors
  public:
    std::vector<std::unique_ptr<EL::Algorithm> > extractAlgorithms ();


    /// \brief get the number of algorithms configured
    /// \par Guarantee
    ///   no-fail
  public:
    std::size_t numberOfAlgorithms () const noexcept;



    //
    // private interface
    //

    /// \brief the number of algorithms added
    ///
    /// This is used to double-check that we didn't lose any
    /// algorithms during serialization.
  private:
    std::size_t m_algorithmCount = 0;

    /// \brief the list of algorithms added
    ///
    /// \warn This may contain nullptr if the clone failed (usually
    /// due to missing dictionaries/libraries).  All users of this
    /// member need to check it regularly.
  private:
    std::vector<std::unique_ptr<EL::Algorithm> > m_algorithms;

    ClassDef (JobConfig, 1);
  };
}

#endif
