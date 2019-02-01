/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__ANA_TOOL_H
#define QUICK_ANA__ANA_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <AsgTools/AsgTool.h>
#include <QuickAna/IAnaTool.h>

namespace ana
{
  /// \brief The base class for all the tools constituting object
  /// definitions.
  /// \par Rationale
  ///   Most object definitions should derive from this class instead
  ///   of from IAnaTool directly, since this class contains quite a
  ///   bit of boilerplate code.
  class AnaTool : virtual public IAnaTool, virtual public asg::AsgTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (AnaTool, ana::IAnaTool)


    /// \brief test the invariant of this container
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaTool (const std::string& name);



    //
    // protected interface
    //

    /// \brief register the given systematics tool
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
    /// \par Rationale
    ///   for now this is mainly used for managing the
    ///   systematics, but at a later stage there may be other uses
    ///   for this as well.
  protected:
    void registerTool (CP::ISystematicsTool *tool);

    /// \copydoc registerTool(CP::ISystematicsTool*)
    /// \par Rationale
    ///   there are two versions here, so that it can be
    ///   called directly both with tools that implement
    ///   ISystematicsTool and those that do not need it.
    /// \sa registerTool(CP::ISystematicsTool*)
  protected:
    void registerTool (IAsgTool *tool);



    //
    // inherited interface
    //

  public:
    virtual bool
    isAffectedBySystematic (const CP::SystematicVariation& systematic) const
      override;

  public:
    virtual CP::SystematicSet
    affectingSystematics () const override;

  public:
    virtual CP::SystematicSet
    recommendedSystematics () const override;

  public:
    virtual CP::SystematicCode
    applySystematicVariation (const CP::SystematicSet& systConfig) override;

  public:
    virtual void
    fillEventDataSource (EventData& event) const override;

  public:
    virtual StatusCode
    useConfiguration (const InternalConfiguration& configuration)
      override;

  public:
    virtual StatusCode
    getInitialConfiguration (InternalConfiguration& conf) override;

  public:
    virtual StatusCode
    useInitialConfiguration (const InternalConfiguration& conf) override;



    //
    // private interface
    //

    /// \brief the list of registered systematics tools
  private:
    std::vector<CP::ISystematicsTool*> m_systematicsTools;
  };
}

#endif
