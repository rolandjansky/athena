//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/AnaAlgorithmWrapper.h>

#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

ClassImp (EL::AnaAlgorithmWrapper)

namespace EL
{
  void AnaAlgorithmWrapper :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  AnaAlgorithmWrapper :: 
  AnaAlgorithmWrapper (const AnaAlgorithmConfig& val_config)
    : m_config (val_config)
  {
    SetName (m_config.name().c_str());

    RCU_NEW_INVARIANT (this);
  }



  StatusCode AnaAlgorithmWrapper ::
  setupJob (Job& job)
  {
    RCU_CHANGE_INVARIANT (this);
    if (m_config.useXAODs())
      job.useXAOD ();
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  histInitialize ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_READ_INVARIANT (this);
    ANA_CHECK (m_config.makeAlgorithm (m_algorithm));
    m_algorithm->setHistogramWorker (wk ());
    m_algorithm->setFilterWorker (wk ());
    m_algorithm->setWk (wk ());
    ANA_CHECK (m_algorithm->sysInitialize());
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  initialize ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_READ_INVARIANT (this);
    if (m_config.useXAODs())
      m_algorithm->setEvtStore (evtStore ());
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  execute ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_READ_INVARIANT (this);
    ANA_CHECK (m_algorithm->sysExecute());
    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithmWrapper ::
  histFinalize ()
  {
    ANA_CHECK_SET_TYPE (EL::StatusCode);
    RCU_READ_INVARIANT (this);
    ANA_CHECK (m_algorithm->sysFinalize());
    return StatusCode::SUCCESS;
  }
}
