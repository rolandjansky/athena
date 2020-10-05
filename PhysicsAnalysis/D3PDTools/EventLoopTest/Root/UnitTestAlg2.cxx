/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoopTest/UnitTestAlg2.h>

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

ClassImp (EL::UnitTestAlg2)

namespace EL
{
  void UnitTestAlg2 ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  UnitTestAlg2 ::
  UnitTestAlg2 (const std::string& name,
                ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator),
      makeOutput (true),
      m_name ("el_n"),
      m_value (0),// m_hist (0),
      m_tree (0),
      m_hasInitialize (false)
  {
    declareProperty ("property", m_property, "test property");
    declareProperty ("string_property", m_string_property, "test string property");
    declareProperty ("toolHandle", m_toolHandle, "ToolHandle property");

    RCU_NEW_INVARIANT (this);
  }



  ::StatusCode UnitTestAlg2 ::
  initialize ()
  {
    RCU_CHANGE_INVARIANT (this);

    getCallbacks()->Fill (CB_INITIALIZE);

    RCU_ASSERT_SOFT (!m_hasInitialize);

    ANA_CHECK (book (TH1F ((m_name + "2_2").c_str(), m_name.c_str(), 50, 0, 50)));
    ANA_CHECK (book (TH1F ("file_executes_2", "file executes", 1, 0, 1)));

    ANA_CHECK (book (TH1F ("test_property", "test_property", 1, 0, 1)));
    hist("test_property")->Fill (0.5, m_property);

    // if (wk()->metaData()->castDouble ("jobOpt") != 42)
    //   RCU_THROW_MSG ("failed to read meta-data from job options");

    // if (wk()->metaData()->castString ("mymeta") != "test")
    //   RCU_THROW_MSG ("failed to read meta-data from worker");

    wk()->addOutput (/*m_hist = */new TH1F (m_name.c_str(), m_name.c_str(),
					50, 0, 50));
    if (makeOutput)
    {
      TFile *file = wk()->getOutputFile ("out");
      m_tree = new TTree ("tree2", "test output");
      m_tree->SetDirectory (file);
      m_tree->Branch (m_name.c_str(), &m_value, (m_name + "/I").c_str());
    }
    m_hasInitialize = true;
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode UnitTestAlg2 ::
  execute ()
  {
    RCU_CHANGE_INVARIANT (this);

    getCallbacks()->Fill (CB_EXECUTE);

    RCU_ASSERT_SOFT (m_hasInitialize);

    RCU_ASSERT_SOFT (wk()->tree()->GetEntries() > wk()->treeEntry());
    // RCU_ASSERT_SOFT (m_fileName == wk()->inputFile()->GetName());

    RCU_ASSERT (wk()->tree() != 0);
    m_branch = wk()->tree()->GetBranch (m_name.c_str());
    if (m_branch == 0)
      RCU_THROW_MSG ("failed to find branch " + m_name);
    m_branch->SetAddress (&m_value);

    RCU_ASSERT (m_branch != nullptr);
    RCU_ASSERT (wk() != nullptr);
    m_branch->GetEntry (wk()->treeEntry());
    hist(m_name)->Fill (m_value);
    if (makeOutput)
      m_tree->Fill ();

    msg().setLevel (MSG::INFO);
    if (++ m_calls < 3)
      ATH_MSG_INFO ("message test");

    return ::StatusCode::SUCCESS;
  }



  ::StatusCode UnitTestAlg2 ::
  finalize ()
  {
    RCU_CHANGE_INVARIANT (this);

    getCallbacks()->Fill (CB_FINALIZE);

    RCU_ASSERT_SOFT (m_hasInitialize);
    wk()->addOutput (new TH1F ("beta/dir/hist2", "directory test", 10, 0, 10));
    wk()->addOutputList ("alpha2", new TObjString ("alpha2"));
    return ::StatusCode::SUCCESS;
  }



  TH1 *UnitTestAlg2 ::
  getCallbacks ()
  {
    if (m_callbacks == nullptr)
    {
      m_callbacks = new TH1F ("callbacks2", "callbacks", CB_FINALIZE + 1,
			      0, CB_FINALIZE + 1);
      wk()->addOutput (m_callbacks);
    }
    return m_callbacks;
  }
}
