/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__ANA_TOOL_SELECT_COMBINE_H
#define QUICK_ANA__ANA_TOOL_SELECT_COMBINE_H

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
  /// \brief The tool class that merges the selection flags from the
  /// different workings points into a single one

  class AnaToolSelectCombine : virtual public IAnaTool, public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (AnaToolSelectCombine, ana::IAnaTool)


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaToolSelectCombine (const std::string& name);


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaToolSelectCombine (const std::string& name, const std::vector<std::string>& workingPoints);


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


    /// \brief the list of working points to combine
  public:
    std::vector<std::string> m_workingPoints;



    //
    // private interface
    //

    /// \brief the type of object we work on
  private:
    ObjectType m_type;

    /// \brief the data for the different selection steps
  private:
    struct Data
    {
      /// \brief the field to write out the selection
    public:
      SG::AuxElement::Accessor<SelectType> select;

      /// \brief the input fields to combine
    public:
      std::vector<SG::AuxElement::ConstAccessor<SelectType>> inputSelect;

      Data (const InternalConfiguration& configuration,
	    const std::vector<std::string>& workingPoints,
	    SelectionStep step);
    };

    /// \brief the different data values
  private:
    std::vector<Data> m_data;
  };
}

#endif
