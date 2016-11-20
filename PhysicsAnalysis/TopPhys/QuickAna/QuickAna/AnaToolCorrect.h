/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__ANA_TOOL_CORRECT_H
#define QUICK_ANA__ANA_TOOL_CORRECT_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>
#include <QuickAna/SelectionData.h>

namespace ana
{
  /// \brief The base class for tools implementing basic object
  /// corrections.
  template<class XAODContainer>
  class AnaToolCorrect : virtual public IAnaTool, public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (AnaToolCorrect, ana::IAnaTool)


    ///\brief the container type we are operating on
  public:
    typedef XAODContainer ContainerType;


    ///\brief the object type we are operating on
  public:
    typedef typename XAODContainer::base_value_type XAODObject;


    /// \brief standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    AnaToolCorrect (const std::string& name);


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


    /// \brief run the calibration on this object
    /// \param object the object to evaluate
    /// \param[out] accept whether the object passes basic object selection
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool dependent
  public:
    virtual StatusCode correctObject (XAODObject& object) = 0;


    /// \brief add a cut with the given name and selection step
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    /// \pre !isInitialized()
  public:
    void registerCut (SelectionStep step, const std::string& name,
		      SelectionCut& cut);



    //
    // private interface
    //

    /// \brief the type of object we work on
  private:
    ObjectType m_type;

    /// \brief the selection data
  private:
    SelectionData m_selection;
  };
}

#include <QuickAna/AnaToolCorrect.icc>

#endif
