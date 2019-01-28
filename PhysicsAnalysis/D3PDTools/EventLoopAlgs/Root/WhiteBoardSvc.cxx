/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//
//

#include <EventLoopAlgs/WhiteBoardSvc.h>

#include <memory>
#include <RootCoreUtils/Assert.h>
#include <EventLoop/IWorker.h>
#include <SampleHandler/MetaData.h>
#include <SampleHandler/MetaVector.h>

//
// method implementations
//

ClassImp (EL::WhiteBoardSvc)

namespace EL
{
  WhiteBoardSvc *getWhiteBoardSvc (IWorker *worker)
  {
    RCU_REQUIRE_SOFT (worker != 0);
    WhiteBoardSvc *const result
      = dynamic_cast<WhiteBoardSvc*>(worker->getAlg ("WhiteBoardSvc"));
    RCU_ASSERT2_SOFT (result != 0, "whiteboard service not found");
    return result;
  }



  void WhiteBoardSvc ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  WhiteBoardSvc ::
  WhiteBoardSvc ()
  {
    RCU_NEW_INVARIANT (this);
  }



  bool WhiteBoardSvc ::
  hasVar (const std::string& name) const
  {
    /// no invariant used
    return doHasVar (name);
  }



  float WhiteBoardSvc ::
  getFloat (const std::string& name) const
  {
    /// no invariant used
    return doGetFloat (name);
  }



  void WhiteBoardSvc ::
  getArray (const std::string& name, std::size_t& size,
	    const float*& values) const
  {
    /// no invariant used
    std::size_t tmpSize = 0;
    const float *tmpValues = 0;
    doGetArray (name, tmpSize, tmpValues);
    size = tmpSize;
    values = tmpValues;
  }



  TObject *WhiteBoardSvc ::
  getTObject (const std::string& name) const
  {
    /// no invariant used
    TObject *result = doGetTObject (name);
    RCU_PROVIDE (result != 0);
    return result;
  }



  void WhiteBoardSvc ::
  setFloat (const std::string& name, float value)
  {
    /// no invariant used
    doSetFloat (name, value);
  }



  void WhiteBoardSvc ::
  setArray (const std::string& name, const std::vector<float>& value)
  {
    /// no invariant used
    doSetArray (name, value.size(), &value[0]);
  }



  void WhiteBoardSvc ::
  setArray (const std::string& name, std::size_t size, const float *values)
  {
    /// no invariant used
    doSetArray (name, size, values);
  }



  void WhiteBoardSvc ::
  setTObject (TObject *object, bool swallow)
  {
    /// no invariant used
    doSetTObject (object, swallow);
  }



  const char *WhiteBoardSvc ::
  GetName () const
  {
    RCU_READ_INVARIANT (this);
    return "WhiteBoardSvc";
  }



  bool WhiteBoardSvc ::
  doHasVar (const std::string& /*name*/) const
  {
    RCU_ASSERT0_NOIMP ("failed to override WhiteBoardSvc::doHasVar");
    return false; // compiler dummy
  }



  float WhiteBoardSvc ::
  doGetFloat (const std::string& name) const
  {
    /// no invariant used

    TObject *const object = getTObject (name);
    RCU_ASSERT (object != 0);
    SH::MetaData<float> *const meta
      = dynamic_cast<SH::MetaData<float>*>(object);
    RCU_ASSERT2_SOFT (meta != 0, ("variable " + name + " not of type float").c_str());
    return meta->value;
  }



  void WhiteBoardSvc ::
  doGetArray (const std::string& name, std::size_t& size,
	      const float*& values) const
  {
    /// no invariant used

    TObject *const object = getTObject (name);
    RCU_ASSERT (object != 0);
    SH::MetaVector<float> *const meta
      = dynamic_cast<SH::MetaVector<float>*>(object);
    RCU_ASSERT2_SOFT (meta != 0, ("variable " + name + " not of type float array").c_str());
    size = meta->value.size();
    values = &meta->value[0];
  }



  TObject *WhiteBoardSvc ::
  doGetTObject (const std::string& /*name*/) const
  {
    RCU_ASSERT0_NOIMP ("function WhiteBoardSvc::doGetTObject not overloaded");
    return 0; // compiler dummy
  }



  void WhiteBoardSvc ::
  doSetFloat (const std::string& name, float value)
  {
    /// no invariant used

    std::auto_ptr<SH::MetaData<float> > meta (new SH::MetaData<float>);
    meta->SetName (name.c_str());
    meta->value = value;
    setTObject (meta.release(), true);
  }



  void WhiteBoardSvc ::
  doSetArray (const std::string& name, std::size_t size, const float *values)
  {
    /// no invariant used

    std::auto_ptr<SH::MetaVector<float> > meta (new SH::MetaVector<float>);
    meta->SetName (name.c_str());
    std::vector<float> (values, values+size).swap (meta->value);
    setTObject (meta.release(), true);
  }



  void WhiteBoardSvc ::
  doSetTObject (TObject *object, bool swallow)
  {
    if (swallow)
      delete object;
    RCU_ASSERT0_NOIMP ("function WhiteBoardSvc::doSetTObject not overloaded");
  }
}
