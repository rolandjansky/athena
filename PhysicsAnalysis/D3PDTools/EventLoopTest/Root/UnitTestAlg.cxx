/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include <EventLoopTest/UnitTestAlg.h>

#include <EventLoop/Job.h>
#include <EventLoop/OutputStream.h>
#include <EventLoop/Worker.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>
#include <TObjString.h>

#include <AsgTools/MsgStream.h>
#include <AsgTools/MsgStreamMacros.h>

//
// method implementations
//

ClassImp (EL::UnitTestAlg)

namespace EL
{
  void UnitTestAlg ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  UnitTestAlg ::
  UnitTestAlg (const std::string& branchName)
    : makeOutput (true),
      m_name (branchName),
      m_branch (0),
      m_value (0),// m_hist (0),
      m_tree (0),
      m_hasInitialize (false),
      m_hasHistInitialize (false)
  {
    RCU_NEW_INVARIANT (this);
  }



  StatusCode UnitTestAlg ::
  setupJob (Job& job)
  {
    RCU_CHANGE_INVARIANT (this);

    if (makeOutput)
    {
      OutputStream out ("out");
      job.outputAdd (out);
    }
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlg ::
  changeInput (bool firstFile)
  {
    RCU_CHANGE_INVARIANT (this);

    RCU_ASSERT (wk()->tree() != 0);
    m_branch = wk()->tree()->GetBranch (m_name.c_str());
    if (m_branch == 0)
      RCU_THROW_MSG ("failed to find branch " + m_name);
    m_branch->SetAddress (&m_value);
    RCU_ASSERT_SOFT (firstFile == m_fileName.empty());
    m_fileName = wk()->inputFile()->GetName();
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlg ::
  histInitialize ()
  {
    RCU_CHANGE_INVARIANT (this);

    RCU_ASSERT_SOFT (!m_hasHistInitialize);

    book (TH1F ((m_name + "2").c_str(), m_name.c_str(), 50, 0, 50));
    book (TH1F ("file_executes", "file executes", 1, 0, 1));
    m_hasHistInitialize = true;
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlg ::
  initialize ()
  {
    RCU_CHANGE_INVARIANT (this);

    RCU_ASSERT_SOFT (m_hasHistInitialize);
    RCU_ASSERT_SOFT (!m_hasInitialize);

    RCU_ASSERT_SOFT (wk()->tree()->GetEntries() > wk()->treeEntry());
    RCU_ASSERT_SOFT (m_fileName == wk()->inputFile()->GetName());

    if (wk()->metaData()->castDouble ("jobOpt") != 42)
      RCU_THROW_MSG ("failed to read meta-data from job options");

    if (wk()->metaData()->castString ("mymeta") != "test")
      RCU_THROW_MSG ("failed to read meta-data from worker");

    wk()->addOutput (/*m_hist = */new TH1F (m_name.c_str(), m_name.c_str(),
					50, 0, 50));
    if (makeOutput)
    {
      TFile *file = wk()->getOutputFile ("out");
      m_tree = new TTree ("tree", "test output");
      m_tree->SetDirectory (file);
      m_tree->Branch (m_name.c_str(), &m_value, (m_name + "/I").c_str());
    }
    m_hasInitialize = true;
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlg ::
  fileExecute ()
  {
    RCU_CHANGE_INVARIANT (this);

    RCU_ASSERT_SOFT (m_hasHistInitialize);
    hist ("file_executes")->Fill (0);
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlg ::
  execute ()
  {
    RCU_CHANGE_INVARIANT (this);

    RCU_ASSERT_SOFT (m_hasInitialize);

    m_branch->GetEntry (wk()->treeEntry());
    hist(m_name)->Fill (m_value);
    if (makeOutput)
      m_tree->Fill ();

    setMsgLevel (MSG::INFO);
    if (++ m_calls < 3)
      ATH_MSG_INFO ("message test");

    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlg ::
  finalize ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_ASSERT_SOFT (m_hasInitialize);
    wk()->addOutput (new TH1F ("beta/dir/hist", "directory test", 10, 0, 10));
    wk()->addOutputList ("alpha", new TObjString ("alpha"));
    return StatusCode::SUCCESS;
  }



  StatusCode UnitTestAlg ::
  histFinalize ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_ASSERT_SOFT (m_hasHistInitialize);
    wk()->addOutput (new TH1F ("beta/dir/hist", "directory test", 10, 0, 10));
    wk()->addOutputList ("alpha", new TObjString ("alpha"));
    return StatusCode::SUCCESS;
  }
}
