/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_SAMPLE_HH
#define SAMPLE_HANDLER_SAMPLE_HH

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <string>
#include <vector>
#include <TObject.h>
#include <SampleHandler/TagList.h>

class TChain;
class TCollection;
class TDSet;

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
  /// \relates Sample
  std::string dbg (const Sample& obj, unsigned verbosity = 0);



  /// \brief a base class that manages a set of files belonging to a
  /// particular data set and the associated meta-data.
  ///
  /// While for most people this is essentially just a fancy file
  /// list, it hides a lot of flexibility.  Besides a simple file
  /// list, this could also be a sample on the grid, or on tape
  /// storage that first needs to be pre-staged before it can be used.
  class Sample : public TObject
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
    virtual ~Sample ();


    /// \brief the name of the sample we are using
    ///
    /// \par Guarantee
    ///   no-fail
    const std::string& name () const;

    /// \brief set the value of \ref name
    ///
    /// \par Guarantee
    ///   no-fail / strong
    /// \par Failures
    ///   out of memory II
    /// \par Failures
    ///   Sample already owned by SampleHandler
    /// \par Rationale
    ///   setting the sample name can be beneficial, if the
    ///   sample auto-discovery set a duplicate (or otherwise
    ///   unsuitable) name.
    /// \warning this must be done before adding it to the
    ///   SampleHandler, or alternatively a clone must be made that
    ///   can then be added to a new SampleHandler.
  public:
    void name (std::string val_name);


    /// \brief the number of files in the sample
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    std::size_t numFiles () const;


    /// \brief the name of the file with the given index
    ///
    /// \param index the index of the file
    /// \par Guarantee
    ///   no-fail
    /// \pre index < numFiles()
    /// \post !result.empty()
    /// \warning this is meant more for debugging and may or may not do
    ///   what you want, e.g. if this is a grid dataset the file may
    ///   not be directly accessible.
  public:
    std::string fileName (std::size_t index) const;


    /// \brief Make this a local sample, i.e. one which can be
    ///   directly opened with TFile::Open
    ///
    /// \return
    ///   the local sample, or "this" if this already is a
    ///   local sample
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory III\n
    ///   can not make local sample
  public:
    SamplePtr makeLocal () const;


    /// \brief the tag list we are using
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    const TagList& tags () const;


    /// \brief set the content of tags()
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void tags (const TagList& tags);


    /// \brief add a tag to the content of tags()
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void addTag (const std::string& tag);


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


    /// \brief the meta-information for this sample
    ///
    /// \par Guarantee
    ///   no-fail
    /// \invariant meta != 0
  public:
    MetaObject *meta ();


    /// \copydoc meta()
  public:
    const MetaObject *meta () const;


    /// \brief make a list of all files, prestaging them if necessary
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory III\n
    ///   file catalogue failures\n
    ///   prestaging failures
  public:
    std::vector<std::string> makeFileList () const;


    /// \brief create a TChain object, containing all these files
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   chain making not supported
    /// \post result != 0
  public:
    TChain *makeTChain () const;


    /// \brief create a TDSet object, containing all these files
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    /// \par Failures
    ///   dataset making not supported
    /// \post result != 0
  public:
    TDSet *makeTDSet () const;


    /// \brief read an object from a histogram file
    ///
    /// \par Returns
    ///   the object read or NULL if it was not found
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   i/o errors
  public:
    TObject *readHist (const std::string& name) const;


    /// \brief whether this sample contains a sample of the given name
    ///
    /// \return whether this sample contains a sample of the given
    ///   name (or whether the sample itself has the given name)
    /// \par Guarantee
    ///   no-fail
    /// \par Rationale
    ///   this is used with composite sample to make sure we don't
    ///   create ring like structures that would lead to endless
    ///   recursion.  the reason to check by name, not by address is
    ///   that we will probably want to add these samples to a sample
    ///   handler at some point.
  public:
    bool contains (const std::string& name) const;


    /// \brief add all samples this sample corresponds to to the
    ///   given sample handler
    ///
    /// \par Guarantee
    ///   basic, may only add some
    /// \par Failures
    ///   out of memory II
    /// \par Rationale this is used with composite samples to find the
    ///   actual list of samples to run on.
  public:
    void addSamples (SampleHandler& result);


    /// \brief print the debugging output to the screen
    ///
    /// \par Guarantee
    ///   basic, may print partially
    /// \par Failures
    ///   out of memory II
    /// \par Failures
    ///   stream errors
  public:
    void print () const;


    /// \copydoc print()
    /// \par Rationale
    ///   added a second print method for python where print is a keyword
  public:
    void printContent () const;


    /// \brief get the number of entries
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   i/o errors
  public:
    Long64_t getNumEntries () const;



    //
    // legacy interface
    //

    /// \brief get the meta-data list
    ///
    /// \par Guarantee
    ///   no-fail
    /// \post result != 0
    /// \deprecated use meta() instead, or (preferably) access of
    ///   meta-data by name
  public:
    [[deprecated("use meta() instead, or (preferably) access of meta-data by name")]]
    TCollection *metaDataList ();


    /// \copydoc metaDataList()
  public:
    [[deprecated("use meta() instead, or (preferably) access of meta-data by name")]]
    const TCollection *metaDataList () const;


    /// \copydoc MetaObject::remove
  public:
    [[deprecated("use meta() instead")]]
    void removeMeta (const std::string& name);


    /// \copydoc MetaObject::addReplace
  public:
    [[deprecated("use meta() instead")]]
    void addReplaceMeta (TNamed *meta_swallow);


    /// \copydoc MetaObject::get
  public:
    [[deprecated("use meta() instead")]]
    TObject *getMeta (const std::string& name);


    /// \copydoc MetaObject::get
  public:
    [[deprecated("use meta() instead")]]
    const TObject *getMeta (const std::string& name) const;


    /// \copydoc MetaObject::getDouble
  public:
    [[deprecated("use meta() instead")]]
    double getMetaDouble (const std::string& name, double def_val = 0) const;


    /// \copydoc MetaObject::getString
  public:
    [[deprecated("use meta() instead")]]
    std::string getMetaString (const std::string& name, const std::string& def_val = "") const;


    /// \copydoc MetaObject::setDouble
  public:
    [[deprecated("use meta() instead")]]
    void setMetaDouble (const std::string& name, double value);


    /// \copydoc MetaObject::setString
  public:
    [[deprecated("use meta() instead")]]
    void setMetaString (const std::string& name, const std::string& value);


    /// \copydoc MetaObject::fetch
  public:
    [[deprecated("use meta() instead")]]
    void fetchMeta (const Sample& source);



    //
    // protected interface
    //

    /// \brief standard constructor
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   low level errors II
  protected:
    Sample (const std::string& name);



    //
    // virtual interface
    //

    /// \copydoc numFiles
    /// \par Rationale
    ///   the virtual part of Sample::numFiles()
  protected:
    virtual std::size_t getNumFiles () const = 0;


    /// \copydoc fileName
    /// \par Rationale
    ///   the virtual part of Sample::fileName()
  protected:
    virtual std::string getFileName (std::size_t index) const = 0;


    /// \copydoc makeLocal
    /// \par Rationale
    ///   the virtual part of SH::Sample::makeLocal()
  protected:
    virtual SamplePtr doMakeLocal () const = 0;


    /// \copydoc makeFileList
    /// \par Rationale
    ///   the virtual part of \ref Sample::makeFileList
  protected:
    virtual std::vector<std::string> doMakeFileList () const = 0;


    /// \copydoc updateLocation
    /// \par Rationale
    ///   the virtual part of SH::Sample::updateLocation()
  protected:
    virtual void
    doUpdateLocation (const std::string& from, const std::string& to);


    /// \copydoc readHist
    /// \par Rationale
    ///   the virtual part of SH::Sample::readHist()
  protected:
    virtual TObject *doReadHist (const std::string& name) const;


    /// \copydoc contains
    /// \par Rationale
    ///   the virtual part of SH::Sample::contains()
  protected:
    virtual bool getContains (const std::string& name) const;


    /// \copydoc addSamples
    /// \par Rationale
    ///   the virtual part of SH::Sample::addSamples()
  protected:
    virtual void doAddSamples (SampleHandler& result);



    //
    // friend interface for SamplePtr
    //

    friend class SamplePtr;
  private:

    /// \brief increase the reference count by one
    ///
    /// \par Guarantee
    ///   no-fail
    void alloc () const;


    /// \brief decrease the reference count by one
    ///
    /// \par Side Effects
    ///   release this sample if that was the last reference
    /// \par Guarantee
    ///   no-fail
    /// \pre reference count > 0
    void release () const;



    //
    // private interface
    //

    /// \copybrief name \sa name
  private:
    std::string m_name;

    /// \copybrief tags \sa tags
  private:
    TagList m_tags;

    /// \copybrief meta \sa meta
  private:
    MetaObject *m_meta;

    /// \brief the reference count
  private:
    mutable unsigned m_references; //!

    /// \par Rationale
    ///   hiding this to avoid slicing
  private:
    Sample (const Sample& that);

    /// \par Rationale
    ///   hiding this to avoid slicing
  private:
    Sample& operator = (const Sample& that);

    ClassDef(Sample, 2);
  };
}

#endif
