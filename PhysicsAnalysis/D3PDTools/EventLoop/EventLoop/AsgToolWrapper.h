/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP_ASG_TOOL_WRAPPER_H
#define EVENT_LOOP_ASG_TOOL_WRAPPER_H

#include <EventLoop/Global.h>

#include <AsgTools/AsgTool.h>
#include <AsgTools/AsgToolConfig.h>
#include <AnaAlgorithm/IAlgorithmWrapper.h>

namespace EL
{
  /// \brief an \ref IAlgorithmWrapper for \ref AsgTool

  class AsgToolWrapper final : public IAlgorithmWrapper
  {
  public:

    /// Public Members
    /// ==============

    /// \brief test the invariant of this object
    void testInvariant () const;

    /// \brief standard default constructor for serialization
    AsgToolWrapper () {};

    /// \brief standard constructor
    AsgToolWrapper (asg::AsgToolConfig val_config);



    /// Inherited Members
    /// =================

    virtual std::string_view getName () const override;

    virtual bool hasName (const std::string& name) const override;

    virtual std::unique_ptr<IAlgorithmWrapper> makeClone() const override;

    virtual StatusCode initialize (const AlgorithmWorkerData& workerData) override;

    virtual StatusCode execute () override;

    virtual StatusCode finalize () override;

    virtual ::StatusCode fileExecute () override;

    virtual ::StatusCode beginInputFile () override;

    virtual ::StatusCode endInputFile () override;



    /// Private Members
    /// ===============

  private:

    /// \brief the algorithm configuration
    asg::AsgToolConfig m_config;

    /// \brief the actual tool (once instantiated)
    ///
    /// Please note (and do not remove) the comment at the end of the
    /// line.  This specifically excludes this member from
    /// serialization.
    ToolHandle<asg::AsgTool> m_tool; //!

    /// \brief the cleanup pointer for this tool
    ///
    /// Please note (and do not remove) the comment at the end of the
    /// line.  This specifically excludes this member from
    /// serialization.
    std::shared_ptr<void> m_cleanup; //!
  };
}

#endif
