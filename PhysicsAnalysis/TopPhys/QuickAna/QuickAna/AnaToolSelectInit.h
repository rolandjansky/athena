/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__ANA_TOOL_SELECT_INIT_H
#define QUICK_ANA__ANA_TOOL_SELECT_INIT_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



#include <QuickAna/Global.h>

#include <AthContainers/AuxElement.h>
#include <QuickAna/AnaTool.h>

namespace ana
{
  /// \brief The tool class that takes care of setting the selection
  /// flags to true at the beginning

  class AnaToolSelectInit : virtual public IAnaTool, public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (AnaToolSelectInit, ana::IAnaTool)


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaToolSelectInit (const std::string& name);


    /// \copydoc IAnaTool::setObjectType
  public:
    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) override;


    /// \copydoc IAnaTool::step
  public:
    virtual AnalysisStep step () const override;


    /// \copydoc IAnaTool::inputTypes
  public:
    virtual unsigned inputTypes () const override;


    /// \copydoc IAnaTool::outputTypes
  public:
    virtual unsigned outputTypes () const override;


    /// \copydoc IAnaTool::useConfiguration
  public:
    virtual StatusCode
    useConfiguration (const InternalConfiguration& configuration)
      override;


    /// \copydoc IAnaTool::execute
  public:
    virtual StatusCode execute (IEventObjects& objects) override;



    //
    // private interface
    //

    /// \brief the type of object we work on
  private:
    ObjectType m_type;

    /// \brief the selection fields
  private:
    std::vector<SG::AuxElement::Accessor<SelectType>> m_select;
  };
}

#endif
