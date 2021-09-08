/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_UNIT_TEST_ALG6_H
#define EVENT_LOOP_UNIT_TEST_ALG6_H

#include <EventLoopTest/Global.h>

#include <AnaAlgorithm/AnaReentrantAlgorithm.h>
#include <EventLoopTest/IUnitTestTool.h>
#include <AsgTools/ToolHandle.h>

namespace EL
{
  class UnitTestAlg6 final : public AnaReentrantAlgorithm
  {
  public:
    UnitTestAlg6 (const std::string& name, ISvcLocator* pSvcLocator);

  public:
    virtual ::StatusCode initialize () override;

  public:
    virtual ::StatusCode execute (const EventContext& ctx) const override;

  public:
    virtual ::StatusCode finalize () override;


    /// \brief the float property our owner sets
  public:
    float m_property = 0;

    /// \brief the string property our owner sets
  public:
    std::string m_string_property;

    /// \brief the tool handle property our owner sets
  public:
    ToolHandle<IUnitTestTool> m_toolHandle {"", this};

    /// \brief whether initialize has been called
  public:
    bool m_wasInitialized = false;
  };
}

#endif
