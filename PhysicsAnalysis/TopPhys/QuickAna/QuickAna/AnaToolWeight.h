/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__ANA_TOOL_WEIGHT_H
#define QUICK_ANA__ANA_TOOL_WEIGHT_H


#include "AthContainers/AuxElement.h"

#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>

namespace ana
{
  /// \brief The base class for tools implementing basic object
  /// weights.
  template<class XAODContainer>
  class AnaToolWeight : virtual public IAnaTool, public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (AnaToolWeight, ana::IAnaTool)


    ///\brief the container type we are operating on
  public:
    typedef XAODContainer ContainerType;


    ///\brief the object type we are operating on
  public:
    typedef typename XAODContainer::base_value_type XAODObject;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaToolWeight (const std::string& name);


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


    /// \copydoc IAnaTool::execute
  public:
    virtual StatusCode execute (IEventObjects& objects) override;


    /// \brief calculate the weight for this event
    /// \param object the object to evaluate
    /// \param[out] weight the calculated weight
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool dependent
    /// \post result > 0
  public:
    virtual StatusCode objectWeight (const XAODObject& object, float& weight) = 0;


    //
    // private interface
    //

    /// \brief the type of object we work on
  private:
    ObjectType m_type;

    /// \brief the field to read in the selection
  private:
    SG::AuxElement::ConstAccessor<SelectType> m_select;

    /// \brief the field to write out the weight
  private:
    SG::AuxElement::Accessor<float> m_weight;
  };
}

#include <QuickAna/AnaToolWeight.icc>

#endif
