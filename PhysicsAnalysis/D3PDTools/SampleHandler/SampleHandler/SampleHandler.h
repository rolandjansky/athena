/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_SAMPLE_HANDLER_HH
#define SAMPLE_HANDLER_SAMPLE_HANDLER_HH

//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>
#include <SampleHandler/SamplePtr.h>

#include <memory>
#include <iosfwd>
#include <map>
#include <vector>
#include <TObject.h>

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
  /// \relates SampleHandler
  std::string dbg (const SampleHandler& obj, unsigned verbosity = 0);


  /// \brief standard swap
  ///
  /// \par Guarantee
  ///   no-fail
  /// \relates SampleHandler
  void swap (SampleHandler& a, SampleHandler& b);



  /// \brief A class that manages a list of Sample objects.
  ///
  /// This class is typically used either to hold a list of all
  /// samples known to the analyzer or to hold a list of samples
  /// produced by a search operation on another SampleHandler.  It is
  /// also extensively used to manage the inputs and outputs of
  /// EventLoop.
  class SampleHandler : public TObject
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


    /// \brief standard constructor
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    SampleHandler ();


    /// \brief standard copy constructor
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    SampleHandler (const SampleHandler& that);


    /// \brief standard destructor
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    ~SampleHandler ();


    /// \brief standard assignment operator
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    SampleHandler& operator = (const SampleHandler& that);


    /// \brief add a sample to the handler
    ///
    /// \par Guarantee
    ///   basic, sample is released if this is the last copy
    /// \par Failures
    ///   out of memory I\n
    ///   sample of same name already in use
    /// \pre sample != 0
  public:
    void add (Sample *sample);


    /// \brief add a sample to the handler
    ///
    /// \par Guarantee
    ///   basic, sample is released
    /// \par Failures
    ///   out of memory I\n
    ///   sample of same name already in use
    /// \pre !sample.empty()
    /// \pre !sample->name().empty()
  public:
    void add (std::unique_ptr<Sample> sample);


    /// \brief add a sample to the handler
    ///
    /// \par Guarantee
    ///   basic, sample is released if this is the last copy
    /// \par Failures
    ///   out of memory I\n
    ///   sample of same name already in use
    /// \pre !sample.empty()
    /// \pre !sample->name().empty()
  public:
    void add (const SamplePtr& sample);


    /// \brief add all samples from the given SampleHandler to this one
    ///
    /// \par Guarantee
    ///   basic, some samples may be added
    /// \par Failures
    ///   out of memory II\n
    ///   sample of same name already in use
    /// \pre this != &sh
  public:
    void add (const SampleHandler& sh);


    /// \brief add all samples from the given SampleHandler to this
    /// one, with prefix prepended to their name
    /// \par Guarantee
    ///   basic, some samples may be added
    /// \par Failures
    ///   out of memory II\n
    ///   sample of same name already in use
    /// \pre this != &sh
  public:
    void addWithPrefix (const SampleHandler& sh, const std::string& prefix);


    /// \brief remove the given sample
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   sample not in SampleHandler
  public:
    void remove (const std::string& name);


    /// \brief remove the given sample
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   sample not in SampleHandler
    /// \pre sample != 0
  public:
    void remove (Sample *sample);


    /// \brief get the sample with the given name
    ///
    /// \return the sample with the given name, or NULL if the sample
    ///   can not be found
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    Sample *get (const std::string& name) const;


    /// \brief find all samples which have at least one of the given
    ///   tags.
    ///
    /// \return a SampleHandler with all found samples
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    SampleHandler find (const TagList& tags) const;


    /// \brief find all samples which have at least one of the given
    ///   tags.
    ///
    /// this version uses a comma separated tag list instead of a
    /// TagList object.
    /// \return a SampleHandler with all found samples
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    SampleHandler find (const std::string& tags) const;


    /// \brief find a sample by the name in the source sample
    ///   handler
    ///
    ///  this means it needs to match the content of the meta-data
    ///  field "nc_source" or if that is not present by its own name.
    /// \return the sample or NULL if no sample was found
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   more than one sample found
  public:
    Sample *findBySource (const std::string& name) const;


    /// \brief find samples by pattern on the name
    ///
    /// \param pattern the regular expression that's matched against
    ///   the name
    /// \return a sample handler with the new samples
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    SampleHandler findByName (const std::string& pattern) const;


    /// \brief print the debugging output to the screen
    ///
    /// \par Guarantee
    ///   basic, may print partially
    /// \par Failures
    ///   out of memory II\n
    ///   stream errors
  public:
    void print () const;


    /// \brief print the debugging output to the screen
    ///
    /// \par Guarantee
    ///   basic, may print partially
    /// \par Failures
    ///   out of memory II\n
    ///   stream errors
    /// \par Rationale
    ///   added a second print method for python
  public:
    void printContent () const;


    /// \brief save the list of samples to the given directory
    ///
    /// \par Guarantee
    ///   basic, may save partially
    /// \par Failures
    ///   out of memory II\n
    ///   stream errors
    /// \par Rationale
    ///   Due to some ancient design decision we store a SampleHandler
    ///   as a directory with one root file per Sample, instead of
    ///   storing them all in a single root file
  public:
    void save (const std::string& directory) const;


    /// \brief load all the samples from the given directory
    ///
    /// \par Guarantee
    ///   basic, may load partially
    /// \par Failures
    ///   out of memory III\n
    ///   duplicate sample
    /// \par Rationale
    ///   Due to some ancient design decision we store a SampleHandler
    ///   as a directory with one root file per Sample, instead of
    ///   storing them all in a single root file
  public:
    void load (const std::string& directory);


    /// \brief update all file references starting with from to to
    ///
    /// \param from the original path to the data area
    /// \param to the new path to the data area
    /// \par Guarantee
    ///   basic, may update partially
    /// \par Failures
    ///   out of memory II
    /// \pre !from.empty()
    /// \post !to.empty()
    /// \par Rationale
    ///   this is needed when moving data around without
    ///   recreating the samples from scratch
  public:
    void updateLocation (const std::string& from, const std::string& to);


    /// \brief fetch the meta-data for all samples that are also in
    ///   the source sample handler.
    ///
    /// samples are identified by name in the source.  the name is
    /// either taken from the meta-data field "nc_source" or (if not
    /// available) by the name of the sample.  fields that have a name
    /// that starts with "nc_" are not copied.
    /// \par Guarantee
    ///   basic, may only fetch some meta-data
    /// \par Failures
    ///   out of memory II
  public:
    void fetch (const SampleHandler& source);


    /// \brief fetch the meta-data for all that samples that don't
    ///   have it in this sample.
    ///
    /// this ignores any meta-data without name and any samples not
    /// present in source.
    /// \par Guarantee
    ///   basic, may only fetch some meta-data
    /// \par Failures
    ///   out of memory II
  public:
    void fetchDefaults (const SampleHandler& source);


    /// \brief whether we have all samples from the source
    ///   SampleHandler.
    ///
    /// samples are identified by name in the source.  the name is
    /// either taken from the meta-data field "nc_source" or (if not
    /// available) by the name of the sample.
    /// \param source the source SampleHandler
    /// \return whether all samples in the source sample handler are
    ///   contained in this sample.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    bool check_complete (const SampleHandler& source) const;


    /// \brief set the meta-data double with the given name for all
    ///   samples.
    ///
    /// \param name the name of the meta-data
    /// \param value the value to which it is set
    /// \par Guarantee
    ///   basic, may clear old entry without adding new one or may not
    ///   do it for all samples
    /// \par Failures
    ///   out of memory II
    /// \warning: this is a one-time effect, if you add more samples
    ///   they will not get their meta-data set.
  public:
    void setMetaDouble (const std::string& name, double value);


    /// \brief set the meta-data string with the given name for all
    ///   samples.
    ///
    /// \param name the name of the meta-data
    /// \param value the value to which it is set
    /// \par Guarantee
    ///   basic, may clear old entry without adding new one or may not
    ///   do it for all samples
    /// \par Failures
    ///   out of memory II
    /// \warning: this is a one-time effect, if you add more samples
    ///   they will not get their meta-data set.
  public:
    void setMetaString (const std::string& name, const std::string& value);


    /// \brief set the meta-data double with the given name for
    ///   samples matching a pattern.
    ///
    /// \param pattern the regular expression against which the sample
    ///   name is matched
    /// \param name the name of the meta-data
    /// \param value the value to which it is set
    /// \par Guarantee
    ///   basic, may clear old entry without adding new one or may not
    ///   do it for all samples
    /// \par Failures
    ///   out of memory II
    /// \warning: this is a one-time effect, if you add more samples
    ///   they will not get their meta-data set.
  public:
    void setMetaDouble (const std::string& pattern, const std::string& name,
			double value);


    /// \brief set the meta-data double with the given name for
    ///   samples matching a pattern.
    ///
    /// \param pattern the regular expression against which the sample
    ///   name is matched
    /// \param name the name of the meta-data
    /// \param value the value to which it is set
    /// \par Guarantee
    ///   basic, may clear old entry without adding new one or may not
    ///   do it for all samples
    /// \par Failures
    ///   out of memory II
    /// \warning: this is a one-time effect, if you add more samples
    ///   they will not get their meta-data set.
  public:
    void setMetaString (const std::string& pattern, const std::string& name,
			const std::string& value);


    /// \brief the iterator to use
  public:
    typedef std::vector<Sample*>::const_iterator iterator;


    /// \brief the begin iterator to use
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    iterator begin () const;


    /// \brief the end iterator to use
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    iterator end () const;


    /// \brief the number of samples contained
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    std::size_t size () const;


    /// \brief the sample with the given index
    ///
    /// \pre index < size()
    /// \par Guarantee
    ///   no-fail
  public:
    Sample *operator [] (std::size_t index) const;


    /// \brief the sample with the given index
    ///
    /// \pre index < size()
    /// \par Guarantee
    ///   no-fail
  public:
    Sample *at (std::size_t index) const;



    //
    // private interface
    //

    friend void swap (SampleHandler& a, SampleHandler& b);

    /// \brief the list of samples managed
  private:
    std::vector<SH::Sample*> m_samples;

    /// \brief the iterator for \ref m_samples
  private:
    typedef std::vector<SH::Sample*>::const_iterator SamplesIter;

    /// \brief the mutable iterator for \ref m_samples
  private:
    typedef std::vector<SH::Sample*>::iterator SamplesMIter;

    /// \brief the list of samples by name
  private:
    std::map<std::string,SH::SamplePtr> m_named;

    /// \brief the iterator for \ref m_named
  private:
    typedef std::map<std::string,SH::SamplePtr>::const_iterator NamedIter;

    /// \brief the mutable iterator for \ref m_named
  private:
    typedef std::map<std::string,SH::SamplePtr>::iterator NamedMIter;

    ClassDef (SampleHandler, 1)
  };
}

#endif
