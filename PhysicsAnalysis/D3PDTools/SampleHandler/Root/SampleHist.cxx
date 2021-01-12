/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include <SampleHandler/SampleHist.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/RootUtils.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/SamplePtr.h>
#include <TFile.h>
#include <memory>

//
// method implementations
//

ClassImp (SH::SampleHist)

namespace SH
{
  void SampleHist ::
  testInvariant () const
  {
    RCU_INVARIANT (!m_file.empty() || name() == "unnamed");    
  }



  SampleHist ::
  SampleHist ()
    : Sample ("unnamed")
  {
    RCU_NEW_INVARIANT (this);
  }



  SampleHist ::
  SampleHist (const std::string& name, const std::string& file)
    : Sample (name), m_file (file)
  {
    RCU_NEW_INVARIANT (this);
  }



  std::size_t SampleHist ::
  getNumFiles () const
  {
    RCU_READ_INVARIANT (this);
    return 1;
  }



  std::string SampleHist ::
  getFileName (const std::size_t
#ifndef NDEBUG
	       index
#endif
	       ) const
  {
    RCU_READ_INVARIANT (this);

    RCU_INVARIANT (index == static_cast<std::size_t>(0));
    return m_file;
  }



  SamplePtr SampleHist ::
  doMakeLocal () const
  {
    RCU_READ_INVARIANT (this);
    RCU_THROW_MSG ("Sample::makeLocal not supported for SampleHist");
    return SamplePtr (); // compiler dummy
  }



  std::vector<std::string> SampleHist ::
  doMakeFileList () const
  {
    RCU_READ_INVARIANT (this);

    std::vector<std::string> result;
    result.push_back (m_file);
    return result;
  }



  void SampleHist ::
  doUpdateLocation (const std::string& from, const std::string& to)
  {
    RCU_READ_INVARIANT (this);
    if (strncmp (m_file.c_str(), from.c_str(), from.size()) == 0)
      m_file = to + "/" + m_file.substr (from.size());
  }



  TObject *SampleHist ::
  doReadHist (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);

    std::unique_ptr<TFile> file (TFile::Open (m_file.c_str(), "READ"));
    if (file.get() == 0)
      RCU_THROW_MSG ("could not open file " + m_file);
    TObject *object = file->Get (name.c_str());
    if (object != 0)
      RCU::SetDirectory (object, 0);
    return object;
  }
}
