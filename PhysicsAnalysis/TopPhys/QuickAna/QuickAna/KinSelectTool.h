/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__KIN_SELECT_TOOL_H
#define QUICK_ANA__KIN_SELECT_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>
#include <QuickAna/SelectionCut.h>
#include <QuickAna/SelectionData.h>

namespace ana
{
  /// TODO: needs documentation
  class KinSelectTool : public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (KinSelectTool, ana::IAnaTool)


    /// \copydoc IAnaTool::setObjectType
  public:
    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) override;


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    KinSelectTool (const std::string& name);


    /// \brief standard destructor
    ///
    /// This is explicitly defined, to break include dependencies
    /// \par Guarantee
    ///   no-fail
  public:
    ~KinSelectTool ();


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
  public:
    StatusCode initialize() override;


    /// \copydoc IAnaTool::useConfiguration
  public:
    virtual StatusCode
    useConfiguration (const InternalConfiguration& configuration)
      override;


    /// returns: the processing step we are working on
    /// guarantee: no-fail
  public:
    virtual AnalysisStep step () const override;


    /// \copydoc IAnaTool::inputTypes
  public:
    virtual unsigned inputTypes () const override;


    /// \copydoc IAnaTool::outputTypes
  public:
    virtual unsigned outputTypes () const override;


    /// effects: apply this analysis tool to the objects
    /// guarantee: basic
    /// failures: tool dependent
  public:
    virtual StatusCode execute (IEventObjects& objects) override;


    /// description: the object type we work on
  public:
    ObjectType m_objectType;

    /// description: the selection string
  public:
    std::string m_selection;

    /// description: the actual object selector
  private:
    std::unique_ptr<KinObjectSelect> m_selectObject;

    /// \brief the selection data
  private:
    SelectionData m_selectionData;

    /// \brief the selection cut
  private:
    SelectionCut cut_kin_select;
  };
}

#endif
