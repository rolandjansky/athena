/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__DISK_LIST_H
#define SAMPLE_HANDLER__DISK_LIST_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <string>

namespace SH
{
  /// \brief an interface for listing directory contents, locally or
  ///   on a file server
  ///
  /// \par Rationale:
  ///   this interface was introduced to allow the various data
  ///   discovery methods to work with the various kinds of file
  ///   servers without having to specialize them all individually.
  class DiskList
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    ///
    /// \par Guarantee
    ///    no-fail
  public:
    void testInvariant () const;


    /// \brief standard destructor
    ///
    /// \par Guarantee
    ///   no-fail
    /// \par Rationale
    ///   virtual destructor for base class
  public:
    virtual ~DiskList ();


    /// \brief get the next list entry
    ///
    /// \return whether we found another entry
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   i/o errors
  public:
    bool next ();


    /// \brief the path for the current entry.
    ///
    /// \return the path for the current entry
    /// \pre (soft) next() has been called successfully
    /// \par Guarantee
    ///  strong
    /// failures: out of memory III
  public:
    std::string path () const;


    /// \brief the filename for the current entry
    ///
    /// \return the filename for the current entry
    /// \pre (soft) next() has been called successfully
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory III
  public:
    std::string fileName () const;


    /// \brief make a new list object for the sub-directory
    ///
    /// \return a new list object for the sub-directory, or \c NULL if
    ///   it is not a directory
    /// \pre (soft) next() has been called successfully
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory III
  public:
    DiskList *openDir () const;


    /// \brief the base path for the directory listed
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    std::string dirname () const;



    //
    // protected interface
    //

    /// \brief standard constructor
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  protected:
    DiskList ();



    //
    // virtual interface
    //

    /// \copydoc next()
    /// \par Rationale
    ///   the virtual part of DiskList::next()
  protected:
    virtual bool getNext () = 0;


    /// \copydoc path()
    /// \par Rationale
    ///   the virtual part of DiskList::path()
  protected:
    virtual std::string getPath () const = 0;


    /// \copydoc openDir()
    /// \par Rationale
    ///   the virtual part of DiskList::openDir()
  protected:
    virtual DiskList *doOpenDir () const = 0;


    /// \copydoc dirname()
    /// \par Rationale
    ///   the virtual part of DiskList::dirname()
  protected:
    virtual std::string getDirname () const = 0;



    //
    // private interface
    //

    /// \brief the current state
  private:
    enum State
    {
      S_BLANK,  ///< just initialized
      S_VALID,  ///< holding a valid entry
      S_DONE,   ///< finished reading entries
      S_BROKEN  ///< an error occured
    };
    /// \copydoc State
    State m_state;
  };
}

#endif
