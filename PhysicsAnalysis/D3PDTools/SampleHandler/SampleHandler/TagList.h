/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_TAG_LIST_HH
#define SAMPLE_HANDLER_TAG_LIST_HH

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.




#include <SampleHandler/Global.h>

#include <set>
#include <string>

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
  /// \relates TagList
  std::string dbg (const TagList& obj, unsigned verbosity = 0);



  /// \brief A class that manages a list of tags for identifying
  /// samples.
  ///
  /// The idea is to use the marker to group samples according to
  /// their type.  This is a bit of an old mechanism, and personally I
  /// prefer to rely on meta-data instead.
  class TagList
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


    /// \brief standard default constructor
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    TagList ();


    /// \brief create a tag-list from a string
    ///
    /// param tags the given list of tags
    /// param separator the character by which tags are separated in
    ///   the list
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit TagList (const std::string& tags, char separator = ':');


    /// \brief whether we have the given tag
    ///
    /// \return whether we have the given tag
    /// \pre !tag.empty()
    /// \par Guarantee
    ///   no-fail
  public:
    bool has (const std::string& tag) const;


    /// \brief the iterator to use
  public:
    typedef std::set<std::string>::const_iterator iterator;


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


    /// \brief join all the tags together into a string
    ///
    /// \return the joined tags
    /// \param separator the separator to use
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   separator used within tag
  public:
    std::string join (char separator = ':') const;


    /// \brief add a tag to the list
    ///
    /// \param tag the tag to add
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void add (const std::string& tag);



    //
    // private interface
    //

    /// \brief the list of tags
  private:
    std::set<std::string> m_tags;

    /// \brief the iterator for \ref m_tags
  private:
    typedef std::set<std::string>::const_iterator TagsIter;
  };
}

#endif
