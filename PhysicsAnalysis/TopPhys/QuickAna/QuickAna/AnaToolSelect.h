/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__ANA_TOOL_SELECT_H
#define QUICK_ANA__ANA_TOOL_SELECT_H


#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>
#include <QuickAna/SelectionData.h>

namespace ana
{
  /// \brief The base class for tools implementing basic object
  /// selection.
  template<class XAODContainer>
  class AnaToolSelect : virtual public IAnaTool, public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (AnaToolSelect, ana::IAnaTool)


    ///\brief the container type we are operating on
  public:
    typedef XAODContainer ContainerType;


    ///\brief the object type we are operating on
  public:
    typedef typename XAODContainer::base_value_type XAODObject;


    /// \copydoc IAnaTool::setObjectType
  public:
    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) override;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaToolSelect (const std::string& name);


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


    /// \brief run the selection on this object
    /// \param object the object to evaluate
    /// \param[out] accept whether the object passes object selection
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool dependent
  public:
    virtual StatusCode
    selectObject (XAODObject& object) = 0;


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

    /// \brief our working point, or the empty string if this is the
    /// only working point
  private:
    std::string m_workingPoint;

    /// \brief the selection data
  private:
    SelectionData m_selection;
  };
}

#include <QuickAna/AnaToolSelect.icc>

#endif
