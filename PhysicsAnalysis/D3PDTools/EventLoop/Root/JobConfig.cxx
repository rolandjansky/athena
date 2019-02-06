/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/JobConfig.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

ClassImp (EL::JobConfig)

namespace EL
{
  void JobConfig :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  JobConfig :: 
  JobConfig () noexcept
  {
    RCU_NEW_INVARIANT (this);
  }



  JobConfig ::
  JobConfig (const JobConfig& that)
    : TObject (that),
      m_algorithmCount (that.m_algorithmCount)
  {
    RCU_READ_INVARIANT (&that);

    for (const auto& algorithm : that.m_algorithms)
    {
      if (algorithm != nullptr)
      {
        std::unique_ptr<Algorithm> myalgorithm
          (dynamic_cast<Algorithm*>(algorithm->Clone ()));
        if (myalgorithm == nullptr)
          RCU_THROW_MSG ("failed to clone algorithm " + std::string (algorithm->GetName()));
        m_algorithms.push_back (std::move (myalgorithm));
      } else
      {
        m_algorithms.emplace_back (nullptr);
      }
    }

    RCU_NEW_INVARIANT (this);
  }



  JobConfig ::
  JobConfig (JobConfig&& that) noexcept
    : JobConfig ()
  {
    that.swap (*this);

    // no invariant used
  }



  JobConfig ::
  ~JobConfig () noexcept
  {
    RCU_DESTROY_INVARIANT (this);

    // not actually doing anything here, but have to make the
    // destructor explicit to break include dependencies.
  }



  JobConfig& JobConfig ::
  operator = (const JobConfig& that)
  {
    // no invariant used
    JobConfig (that).swap (*this);
    return *this;
  }



  JobConfig& JobConfig ::
  operator = (JobConfig&& that) noexcept
  {
    // no invariant used
    that.swap (*this);
    return *this;
  }



  void JobConfig ::
  swap (JobConfig& that) noexcept
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_CHANGE_INVARIANT (&that);
    std::swap (m_algorithmCount, that.m_algorithmCount);
    m_algorithms.swap (that.m_algorithms);
  }



  ::StatusCode JobConfig ::
  addAlgorithm (std::unique_ptr<Algorithm> val_algorithm)
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (val_algorithm != nullptr);

    if (getAlgorithm (val_algorithm->GetName()) != nullptr)
    {
      ANA_MSG_ERROR ("can't have two algorithms with the same name: " << val_algorithm->GetName());
      return ::StatusCode::FAILURE;
    }

    m_algorithms.push_back (std::move (val_algorithm));
    ++ m_algorithmCount;
    return ::StatusCode::SUCCESS;
  }



  const Algorithm *JobConfig ::
  getAlgorithm (const std::string& name) const noexcept
  {
    RCU_READ_INVARIANT (this);
    for (const auto& algorithm : m_algorithms)
    {
      if (algorithm != nullptr && algorithm->GetName() == name)
        return algorithm.get();
    }
    return nullptr;
  }



  std::vector<std::unique_ptr<EL::Algorithm> > JobConfig ::
  extractAlgorithms ()
  {
    RCU_CHANGE_INVARIANT (this);
    if (m_algorithmCount != m_algorithms.size())
      RCU_THROW_MSG ("algorithm count missmatch.  streaming error?");
    for (const auto& algorithm : m_algorithms)
    {
      if (algorithm == nullptr)
        RCU_THROW_MSG ("algorithm null.  streaming error?");
    }
    m_algorithmCount = 0;
    return std::move (m_algorithms);
  }



  std::size_t JobConfig ::
  numberOfAlgorithms () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_algorithms.size();
  }
}
