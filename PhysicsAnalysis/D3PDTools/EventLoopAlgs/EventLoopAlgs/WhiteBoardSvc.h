/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_ALGS__WHITE_BOARD_SVC_H
#define EVENT_LOOP_ALGS__WHITE_BOARD_SVC_H

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a base class for classes that implement an
/// interface to a white board service.  The interface provided in
/// this module is intended for the general user.  The module is
/// considered to be in the pre-alpha stage.



#include <EventLoopAlgs/Global.h>

#include <EventLoop/Algorithm.h>

namespace EL
{
  /// effects: get the whiteboard service for this worker
  /// guarantee: strong
  /// failures: formula service not configured
  /// requires: worker != 0
  /// requires: !outputStream.empty()
  /// postcondition: result != 0
  WhiteBoardSvc *getWhiteBoardSvc (IWorker *worker);



  class WhiteBoardSvc : public EL::Algorithm
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
    /// rationale: this constructor has to be public to allow root
    ///   streaming.  normally I would have made it protected to
    ///   prevent the user from creating an object of base class type.
  public:
    WhiteBoardSvc ();


    /// returns: whether we have the given variable
    /// guarantee: strong
    /// failures: out of memory II
  public:
    bool hasVar (const std::string& name) const;


    /// returns: the content of the variable, assuming the given type
    /// guarantee: strong
    /// failures: variable not found
    /// failures: variable of wrong type
    /// failures: given type not supported
    /// postcondition(3): result != 0
  public:
    float getFloat (const std::string& name) const;
    void getArray (const std::string& name, std::size_t& size,
		   const float*& values) const;
    TObject *getTObject (const std::string& name) const;


    /// returns: the content of the variable, casted to the given type
    /// guarantee: strong
    /// failures: variable not found
    /// failures: variable of wrong type
    /// failures: TObject type not supported
  public:
    template<class T> T *retrieve (const std::string& name) const;


    /// returns: set/create a variable, assuming the given type
    /// guarantee: strong
    /// failures: given type not supported
    /// failures: variable can not be set
    /// failures: out of memory II
    /// requires(4): TObject != 0
  public:
    void setFloat (const std::string& name, float value);
    void setArray (const std::string& name, const std::vector<float>& value);
    void setArray (const std::string& name, std::size_t size,
		   const float *values);
    void setTObject (TObject *object, bool swallow = true);



    //
    // interface inherited from Algorithm
    //

    /// effects: return the name of this algorithm
    /// guarantee: no-fail
  public:
    virtual const char *GetName () const;



    //
    // virtual interface
    //

    /// returns: whether we have the given variable
    /// guarantee: strong
    /// failures: out of memory II
    /// rationale: the virtual part of WhiteBoardSvc::hasVar
  private:
    virtual bool doHasVar (const std::string& name) const;


    /// returns: the content of the variable, assuming the given type
    /// guarantee: strong
    /// failures: variable not found
    /// failures: variable of wrong type
    /// failures: given type not supported
    /// postcondition(4): result != 0
    /// description: the virtual part of the getters
  private:
    virtual float doGetFloat (const std::string& name) const;
    virtual void doGetArray (const std::string& name, std::size_t& size,
			     const float*& values) const;
    virtual TObject *doGetTObject (const std::string& name) const;


    /// returns: set/create a variable, assuming the given type
    /// guarantee: strong
    /// failures: given type not supported
    /// failures: variable can not be set
    /// failures: out of memory II
    /// requires(3): TObject != 0
    /// description: the virtual part of the setters
  private:
    virtual void doSetFloat (const std::string& name, float value);
    virtual void doSetArray (const std::string& name, std::size_t size,
			     const float *values);
    virtual void doSetTObject (TObject *object, bool swallow);



    //
    // private interface
    //

    ClassDef(WhiteBoardSvc, 1);
  };
}

#include <RootCoreUtils/Assert.h>

namespace EL
{
  //
  // template methods
  //

  template<class T> T *WhiteBoardSvc ::
  retrieve (const std::string& name) const
  {
    /// no invariant used
    T *const result = dynamic_cast<T*>(getTObject (name));
    RCU_ASSERT2_SOFT (result != 0, "object not of correct type");
    return result;
  }
}

#endif
