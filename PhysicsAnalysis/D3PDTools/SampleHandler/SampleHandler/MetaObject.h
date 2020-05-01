/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_META_OBJECT_HH
#define SAMPLE_HANDLER_META_OBJECT_HH

//          - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>
#include <RootCoreUtils/Assert.h>

#include <string>
#include <TCollection.h>

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
  /// \relates MetaObject
  std::string dbg (const MetaObject& obj, unsigned verbosity = 0);


  /// \brief standard swap
  ///
  /// \par Guarantee
  ///   no-fail
  /// \relates MetaObject
  void swap (MetaObject& a, MetaObject& b);



  /// \brief A class that manages meta-data to be associated with an
  /// object.
  ///
  /// Originally this was just to be used for meta-data of a sample,
  /// but it is now also used for specifying the options for an
  /// EventLoop job.
  class MetaObject : public TCollection
  {
    //
    // public interface
    //

    /// \brief the different modes for casts
  public:
    enum CastMode
    {
      /// \brief cast and throw if the cast fails (not if the input is
      /// not found)
      CAST_ERROR_THROW,

      /// \brief cast and return the default value if the cast fails
      CAST_ERROR_DEFAULT,

      /// \brief do not cast and throw if the input has the wrong type
      /// (not if the input is not found)
      CAST_NOCAST_THROW,

      /// \brief cast and return the default value if the input has
      /// the wrong type
      CAST_NOCAST_DEFAULT
    };



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
    MetaObject ();


    /// \brief standard copy constructor
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    MetaObject (const MetaObject& that);


    /// \brief standard destructor
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~MetaObject ();


    /// \brief standard assignment operator
    ///
    /// \par Returns
    ///   *this
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    MetaObject& operator = (const MetaObject& that);


    /// \brief standard swap
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    void swap (MetaObject& that);


    /// \brief remove all meta-information with the given name
    ///
    /// \param name the meta-data name to remove
    /// \par Guarantee
    ///   basic, may only delete some entries
    /// \par Failures
    ///   out of memory II
  public:
    void remove (const std::string& name);


    /// \brief add a meta-data object and remove any existing
    ///   meta-data with the same name
    ///
    /// \param meta_swallow the meta-object to add
    /// \pre meta_swallow != 0
    /// \par Guarantee
    ///   basic, may delete some entries without adding the
    ///   new one
    /// \par Failures
    ///   out of memory II
  public:
    void addReplace (TNamed *meta_swallow);


    /// \brief the meta-data object with the given name
    ///
    /// \return the meta-data object with the given name, or NULL if
    ///   there is no such entry
    /// \param name the name of the meta-data object
    /// \par Guarantee
    ///   no-fail
  public:
    TObject *get (const std::string& name);
    const TObject *get (const std::string& name) const;


    /// \brief the meta-data double with the given name
    ///
    /// \return the meta-data with the given name, or def_value if
    ///   that meta-data is not available or does not have the right
    ///   type
    /// \param name the name of the meta-data object
    /// \param dev_val the value to return if no meta-data object of
    ///   the given name exists, or if it does not have the right type
    /// \par Guarantee
    ///   no-fail
  public:
    [[deprecated("please use castDouble instead")]]
    double getDouble (const std::string& name, double def_val = 0) const;

    /// \brief the meta-data string with the given name
    ///
    ///
    /// \return the meta-data with the given name, or def_value if
    ///   that meta-data is not available or does not have the right
    ///   type
    /// \param name the name of the meta-data object
    /// \param dev_val the value to return if no meta-data object of
    ///   the given name exists, or if it does not have the right type
    /// \par Guarantee
    ///   no-fail
    /// \par Rationale
    ///   I return the string by value, because returning by
    ///   reference seems to break the python binding.
  public:
    [[deprecated("please use castString instead")]]
    std::string getString (const std::string& name, const std::string& def_val = "") const;


    /// \brief the meta-data element with the given name
    ///
    /// \return the meta-data with the given name, or def_value if
    ///   that meta-data is not available or does not have the right
    ///   type
    /// \param name the name of the meta-data object
    /// \param dev_val the value to return if no meta-data object of
    ///   the given name exists, or if it does not have the right type
    /// \par Guarantee
    ///   no-fail
  public:
    template<class T> T castT(const std::string& name, T def_val,
			      CastMode mode) const;


    /// \brief the meta-data double with the given name
    ///
    /// \return the meta-data with the given name, or def_value if
    ///   that meta-data is not available or does not have the right
    ///   type
    /// \param name the name of the meta-data object
    /// \param dev_val the value to return if no meta-data object of
    ///   the given name exists, or if it does not have the right type
    /// \par Guarantee
    ///   no-fail
  public:
    double castDouble (const std::string& name, double def_val = 0,
		       CastMode mode = CAST_ERROR_THROW) const;

    /// \brief the meta-data string with the given name
    ///
    ///
    /// \return the meta-data with the given name, or def_value if
    ///   that meta-data is not available or does not have the right
    ///   type
    /// \param name the name of the meta-data object
    /// \param dev_val the value to return if no meta-data object of
    ///   the given name exists, or if it does not have the right type
    /// \par Guarantee
    ///   no-fail
    /// \par Rationale
    ///   I return the string by value, because returning by
    ///   reference seems to break the python binding.
  public:
    std::string castString (const std::string& name, const std::string& def_val = "",
			    CastMode mode = CAST_ERROR_THROW) const;


    /// \brief the meta-data integer with the given name
    ///
    /// \return the meta-data with the given name, or def_value if
    ///   that meta-data is not available or does not have the right
    ///   type
    /// \param name the name of the meta-data object
    /// \param dev_val the value to return if no meta-data object of
    ///   the given name exists, or if it does not have the right type
    /// \par Guarantee
    ///   no-fail
  public:
    int castInteger (const std::string& name, int def_val = 0,
		     CastMode mode = CAST_ERROR_THROW) const;


    /// \brief the meta-data boolean with the given name
    ///
    /// \return the meta-data with the given name, or def_value if
    ///   that meta-data is not available or does not have the right
    ///   type
    /// \param name the name of the meta-data object
    /// \param dev_val the value to return if no meta-data object of
    ///   the given name exists, or if it does not have the right type
    /// \par Guarantee
    ///   no-fail
  public:
    bool castBool (const std::string& name, bool def_val = false,
		   CastMode mode = CAST_ERROR_THROW) const;
    
    /// \brief set the meta-data double with the given name
    ///
    /// \param name the name of the meta-data
    /// \param value the value to which it is set
    /// \par Guarantee
    ///   basic, may clear old entry without adding new one
    /// \par Failures
    ///   out of memory II
  public:
    void setDouble (const std::string& name, double value);

    /// \brief set the meta-data string with the given name
    ///
    /// \param name the name of the meta-data
    /// \param value the value to which it is set
    /// \par Guarantee
    ///   basic, may clear old entry without adding new one
    /// \par Failures
    ///   out of memory II
  public:
    void setString (const std::string& name, const std::string& value);

    /// \brief set the meta-data integer with the given name
    ///
    /// \param name the name of the meta-data
    /// \param value the value to which it is set
    /// \par Guarantee
    ///   basic, may clear old entry without adding new one
    /// \par Failures
    ///   out of memory II
  public:
    void setInteger (const std::string& name, int value);

    /// \brief set the meta-data boolean with the given name
    ///
    /// \param name the name of the meta-data
    /// \param value the value to which it is set
    /// \par Guarantee
    ///   basic, may clear old entry without adding new one
    /// \par Failures
    ///   out of memory II
  public:
    void setBool (const std::string& name, bool value);

    /// \brief set the meta-data element with the given name
    ///
    /// \param name the name of the meta-data
    /// \param value the value to which it is set
    /// \par Guarantee
    ///   basic, may clear old entry without adding new one
    /// \par Failures
    ///   out of memory II
  public:
    template<class T> void setT(const std::string& name, T value){
      RCU_CHANGE_INVARIANT (this);
      addReplace (new MetaData<T> (name, value));
    }

    /// \brief fetch the meta-data from the given sample.
    ///
    /// fields that have a name that starts with "nc_" are not copied.
    ///
    /// \par Guarantee
    ///   basic, may only fetch some meta-data
    /// \par Failures
    ///   out of memory II
    /// \par Rationale this is used by EventLoop, to copy the
    ///   meta-data from the input samples to the output samples.  the
    ///   prefix "nc_" is used to indicate meta-data entries that only
    ///   apply to the current sample, or are options to the specific
    ///   EventLoop job.
  public:
    void fetch (const MetaObject& source);


    /// \brief fetch the meta-data from the given sample not present
    ///   in this sample.  ignores any meta-data without name.
    ///
    /// \par Guarantee
    ///   basic, may only fetch some meta-data
    /// \par Failures
    ///   out of memory II
  public:
    void fetchDefaults (const MetaObject& source);


    /// \brief fetch the meta-data from a given string
    ///
    /// individual pieces of data may be given as
    ///   key=value, key='value', key="value"
    /// spurious spaces will be ignored, 
    /// commas inside values are not allowed
    ///
    /// \par Guarantee
    ///   weak, may throw runtime exceptions if string cannot be parsed
    /// \par Failures
    ///   malformed input
  public:
    void fetchFromString(const std::string& source);


    /// \brief dump the meta-data to a string
    ///
    ///  string will be suitable for fetchFromString
    /// \par Guarantee
    ///   strong
    /// \par Failues
    ///   no-fail
  public:
    std::string dumpToString();


    //
    // method inherited from TCollection
    //

    /// \brief add another object to the collection
    ///
    /// \pre meta_swallow != 0
    /// \par Guarantee
    ///   basic, argument is always swallowed
    /// \par Failures
    ///   out of memory II
  public:
    virtual void Add (TObject *meta_swallow);


    /// \brief clear out the content of the collection
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    virtual void Clear (Option_t *option = "");


    /// \brief clear out the content of the collection
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    virtual void Delete (Option_t *option = "");


    /// \brief return the number of entries of this collection
    ///
    /// \par Guarantee
    ///   no-fail
  public:
    virtual Int_t GetEntries () const;


    /// \brief return pointer to pointer of object meta
    ///
    /// \pre meta != 0
    /// \post result != 0
    /// \post *result == meta
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   meta not in collection
  public:
    virtual TObject **GetObjectRef (const TObject *meta) const;


    /// \brief make a new iterator for this collection
    ///
    /// \post result != 0
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    virtual TIterator *MakeIterator (Bool_t dir = kIterForward) const;


    /// \brief remove the given object from the list
    ///
    /// \pre meta != 0
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   meta not in list
  public:
    virtual TObject *Remove (TObject *meta);

    //
    // private interface
    //

    /// \brief the actual meta-data list
  private:
    TList *m_dataList;

    ClassDef(MetaObject, 1);
  };
}

#endif
