/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__ANA_TOOL_SELECT_SPREAD_H
#define QUICK_ANA__ANA_TOOL_SELECT_SPREAD_H


#include <QuickAna/Global.h>

#include <AthContainers/AuxElement.h>
#include <QuickAna/AnaTool.h>

namespace ana
{
  /// \brief The tool class that takes care of copying the global
  /// selection flags to the individual working points.
  ///
  /// this is used twice (when using multiple working points), first
  /// to copy the selection flags from the working point independent
  /// tools to the different working points.  then again after the
  /// overlap removal tool it copies over the objects that fail the
  /// overlap removal.

  class AnaToolSelectSpread : virtual public IAnaTool, public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (AnaToolSelectSpread, ana::IAnaTool)


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaToolSelectSpread (const std::string& name);


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaToolSelectSpread (const std::string& name, const std::vector<std::string>& workingPoints, bool first);


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


    /// \brief the list of working points to spread
  public:
    std::vector<std::string> m_workingPoints;



    //
    // private interface
    //

    /// \brief the type of object we work on
  private:
    ObjectType m_type;

    /// \brief whether we are spreading for the first time,
    /// i.e. whether we create the decorations without looking at them
  private:
    bool m_first;

    /// \brief the data for the different selection steps
  private:
    struct Data
    {
      /// \brief the field to write out the selection
    public:
      SG::AuxElement::ConstAccessor<SelectType> select;

      /// \brief the input fields to combine
    public:
      std::vector<SG::AuxElement::Accessor<SelectType>> outputSelect;

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
