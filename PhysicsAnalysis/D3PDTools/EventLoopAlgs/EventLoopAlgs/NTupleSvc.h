/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_ALGS__NTUPLE_SVC_H
#define EVENT_LOOP_ALGS__NTUPLE_SVC_H

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines an algorithm that writes out an n-tuple with
/// various services to make skimming easier.  The interface provided
/// in this module is intended for the general user.  The module is
/// considered to be in the pre-alpha stage.



#include <EventLoopAlgs/Global.h>

#include <set>
#include <EventLoop/Algorithm.h>

class TBranch;
class TFile;
class TTree;

namespace EL
{
  /// effects: get the skimming algorithm for the given output for
  ///   this worker
  /// guarantee: strong
  /// failures: formula service not configured
  /// requires: worker != 0
  /// requires: !outputStream.empty()
  /// postcondition: result != 0
  NTupleSvc *getNTupleSvc (IWorker *worker,
			   const std::string& outputStream,
			   const std::string& treeName = "");



  class NTupleSvc : public EL::Algorithm
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory I
    /// requires: !val_outputName.empty()
  public:
    NTupleSvc (const std::string& val_outputName = "output");


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~NTupleSvc ();


    /// effects: add another branch to copy.  allows use of regular
    ///   expressions
    /// guarantee: strong
    /// failures: out of memory II
  public:
    void copyBranch (const std::string& name);


    /// effects: read a list of branches to copy from a file.  allows
    ///   use of regular expressions inside the file.  leading and
    ///   trailing spaces are ignored.  lines starting with # are
    ///   ignored.
    /// guarantee: strong
    /// failures: out of memory III
    /// failures: i/o errors
  public:
    void copyBranchList (const std::string& fileName);


    /// effects: add a float from the whiteboard
    /// guarantee: strong
    /// failures: out of memory II
  public:
    void addWhiteFloat (const std::string& varName);


    /// effects: add an array from the whiteboard
    /// guarantee: strong
    /// failures: out of memory II
  public:
    void addWhiteArray (const std::string& varName);


    /// returns: the tree we are writing out
    /// guarantee: no-fail
    /// requires(soft): initialize() has been called
    /// postcondition: result != 0
  public:
    TTree *tree () const;


    /// returns: whether the current event will be taken
    /// guarantee: no-fail
  public:
    bool getFilterPassed () const;


    /// effects: take the current event if passed is set, otherwise
    ///   perform a no-op
    /// guarantee: no-fail
  public:
    void setFilterPassed (bool passed = true);


    /// description: the name of the tree, or the empty string to use
    ///   the name of the input tree
    /// guarantee: no-fail / strong
    /// failures: out of memory II
  public:
    const std::string& treeName () const;
    void treeName (const std::string& val_treeName);


    //
    // interface inherited from Algorithm
    //

    /// effects: return the name of this algorithm
    /// guarantee: no-fail
  public:
    virtual const char *GetName () const;


    /// effects: give the algorithm a chance to intialize the job with
    ///   anything this algorithm needs.  this method is automatically
    ///   called before the algorithm is actually added to the job.
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is currently used to give algorithms a chance
    ///   to register their output datasets, but can also be used for
    ///   other stuff.
  private:
    virtual StatusCode setupJob (Job& job);


    /// effects: do all changes to work with a new input file,
    ///   e.g. set new branch addresses.  if firstFile is set, this
    ///   method is called just before init() is called
    /// guarantee: basic
    /// failures: algorithm dependent
  private:
    virtual StatusCode changeInput (bool firstFile);


    /// effects: do everything that needs to be done before running
    ///   the algorithm, e.g. create output n-tuples and histograms.
    ///   this method is called only once right after
    ///   changeInput(true) is called
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: in principle all this work could be done on
    ///   changeInput(true).  However, providing this method should
    ///   make it easier for the user to set up all his outputs and to
    ///   do so only once.
  private:
    virtual StatusCode initialize ();


    /// effects: process the next event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: the virtual part of Algorithm::process
  private:
    virtual StatusCode execute ();


    /// effects: do the post-processing for the event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: the virtual part of Algorithm::process
    /// rationale: this is mainly used for specialized services that
    ///   need to get input from subsequent algorithms before filling
    ///   their event data
  private:
    virtual StatusCode postExecute ();


    /// returns: whether this algorithm has the given name
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is to allow an algorithm to be known by
    ///   multiple names.  this is needed for NTupleSvc, so that it
    ///   can be located with and without the output tree name.
  private:
    virtual bool hasName (const std::string& name) const;



    //
    // private interface
    //

    /// description: the name of the output stream
  private:
    std::string m_outputName;

    /// description: the name of the output tree
  private:
    std::string m_treeName;

    /// description: the title of the output tree
  private:
    std::string m_tree_title;

    /// description: the list of branches we copy
  private:
    std::set<std::string> m_copyBranches;
    typedef std::set<std::string>::iterator m_copyBranchesIter;

    /// description: the list of whiteboard floats we copy
  private:
    std::set<std::string> m_whiteFloat;
    typedef std::set<std::string>::iterator m_whiteFloatIter;

    /// description: the list of whiteboard arrays we copy
  private:
    std::set<std::string> m_whiteArray;
    typedef std::set<std::string>::iterator m_whiteArrayIter;

    /// description: the file we are writing out
  private:
    TFile *m_file; //!

    /// description: the tree we are writing out
  private:
    TTree *m_tree; //!

    /// description: whether we initialized the tree we are writing out
  private:
    bool m_initialized; //!

    /// description: whether we take the current event
  private:
    bool m_taken; //!

    /// description: whether our output branches are properly
    ///   connected to the input tree
  private:
    bool m_connected; //!

    /// description: the white board service, if we are using it
  private:
    WhiteBoardSvc *m_whiteboard; //!

    /// description: the information we need for copying branches
  private:
    struct CopyInfo
    {
      /// description: the name of the branch
    public:
      std::string name;

      /// description: the branch in the source tree
    public:
      TBranch *source;

      /// description: the branch in the output tree
    public:
      TBranch *target;

      /// description: the buffer used if none is set
    public:
      std::vector<char> buffer;
    };
    std::vector<CopyInfo> m_copyInfo; //!
    typedef std::vector<CopyInfo>::iterator copyInfoMIter;

    /// description: the information we need for whiteboard variables
  private:
    struct WhiteInfo
    {
      /// description: the name of the branch
    public:
      std::string name;

      /// description: whether this is an array branch
    public:
      bool array;

      /// description: the buffer we are writing
    public:
      std::vector<float> buffer;

      /// description: a pointer to the buffer
      /// rationale: this is used when writing arrays
    public:
      std::vector<float> *pointer;
    };
    std::vector<WhiteInfo> m_whiteInfo; //!
    typedef std::vector<WhiteInfo>::iterator whiteInfoMIter;


    /// effects: create all the branches when called for the first time
    /// guarantee: basic, may create some
    /// failures: out of memory III
    /// failures: branch not found
  private:
    void initBranches ();


    /// effects: find the branches in the input tree
    /// guarantee: basic, may create some
    /// failures: out of memory III
    /// failures: branch not found
  private:
    void findBranches (std::set<std::string>& branchList);


    /// effects: create the branch in the output tree
    /// guarantee: basic, may create some
    /// failures: out of memory II
    /// failures: branch not found
    /// failures: branch not copyable
    /// rationale: it is safe to call this function for branches that
    ///   are already configured, since this function can call itself
    ///   recursively.
  private:
    void initOutput (const std::string& branchName);


    /// effects: create the branches in the output tree
    /// guarantee: basic, may create some
    /// failures: out of memory II
    /// failures: branch not found
    /// failures: branch not copyable
  private:
    void initOutput (const std::set<std::string>& branchList);


    /// effects: copy the input branches to the output branches
    /// guarantee: basic, may copy some
    /// failures: out of memory III
  private:
    void copyInput ();


    ClassDef(NTupleSvc, 1);
  };
}

#endif
