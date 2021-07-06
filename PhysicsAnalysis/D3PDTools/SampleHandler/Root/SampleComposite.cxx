/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/SampleComposite.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/SamplePtr.h>

//
// method implementations
//

ClassImp (SH::SampleComposite)

namespace SH
{
  void SampleComposite ::
  testInvariant () const
  {
    for (SamplesIter sample = m_samples.begin(),
	   end = m_samples.end(); sample != end; ++ sample)
    {
      RCU_INVARIANT (!sample->empty());
    };
  }



  SampleComposite ::
  SampleComposite ()
    : Sample ("unnamed")
  {
    RCU_NEW_INVARIANT (this);
  }



  SampleComposite ::
  SampleComposite (const std::string& name)
    : Sample (name)
  {
    RCU_NEW_INVARIANT (this);
  }



  void SampleComposite ::
  add (const SamplePtr& sample)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (!sample.empty());
    if (contains (sample->name()))
      RCU_THROW_MSG ("trying to add sample " + sample->name() + " to sample " + name() + ", which already contains a sample " + sample->name());
    m_samples.push_back (sample);
  }



  std::size_t SampleComposite ::
  getNumFiles () const
  {
    RCU_READ_INVARIANT (this);
    RCU_THROW_MSG ("Sample::numFiles not supported for SampleComposite");
    return 0; // compiler dummy
  }



  std::string SampleComposite ::
  getFileName (const std::size_t /*index*/) const
  {
    RCU_READ_INVARIANT (this);
    RCU_THROW_MSG ("Sample::fileName not supported for SampleComposite");
    return ""; // compiler dummy
  }



  std::vector<std::string> SampleComposite ::
  doMakeFileList () const
  {
    RCU_READ_INVARIANT (this);

    std::vector<std::string> result;
    for (SamplesIter sample = m_samples.begin(),
	   end = m_samples.end(); sample != end; ++ sample)
    {
      std::vector<std::string> subresult = (*sample)->makeFileList();
      result.insert (result.end(), subresult.begin(), subresult.end());
    }
    return result;
  }



  SamplePtr SampleComposite ::
  doMakeLocal () const
  {
    RCU_READ_INVARIANT (this);
    RCU_THROW_MSG ("Sample::makeLocal not supported for SampleComposite");
    return 0; // compiler dummy
  }



  void SampleComposite ::
  doUpdateLocation (const std::string& from, const std::string& to)
  {
    RCU_READ_INVARIANT (this);
    for (SamplesIter sample = m_samples.begin(),
	   end = m_samples.end(); sample != end; ++ sample)
    {
      (*sample)->updateLocation (from, to);
    }
  }



  bool SampleComposite ::
  getContains (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    for (SamplesIter sample = m_samples.begin(),
	   end = m_samples.end(); sample != end; ++ sample)
    {
      if ((*sample)->contains (name))
	return true;
    };
    return false;
  }



  void SampleComposite ::
  doAddSamples (SampleHandler& result)
  {
    RCU_READ_INVARIANT (this);
    for (SamplesIter sample = m_samples.begin(),
	   end = m_samples.end(); sample != end; ++ sample)
    {
      (*sample)->addSamples (result);
    };
  }
}
