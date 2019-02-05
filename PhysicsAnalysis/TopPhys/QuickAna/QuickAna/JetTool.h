/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__JET_TOOL_H
#define QUICK_ANA__JET_TOOL_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <QuickAna/AnaToolCorrect.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/AnaToolSelect.h>
#include <QuickAna/AnaToolWeight.h>
#include <QuickAna/SelectionCut.h>

// Forward declarations
class IJetCalibrationTool;
class ICPJetUncertaintiesTool;
class IJERTool;
class IJERSmearingTool;
class IJetSelector;
class IBTaggingEfficiencyTool;
class IBTaggingSelectionTool;
class IJetUpdateJvt;

namespace CP {
  class IJetJvtEfficiency;
}

namespace ana
{

  /// @class JetToolCorrect
  /// @brief QuickAna tool for applying CP corrections to jets
  ///
  class JetToolCorrect : public AnaToolCorrect<xAOD::JetContainer>
  {

    ASG_TOOL_CLASS (JetToolCorrect, ana::IAnaTool)

  public:
    //
    // public interface
    //

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    JetToolCorrect (const std::string& name);

    /// \copydoc IAnaTool::useInitialConfiguration
    virtual StatusCode
    useInitialConfiguration (const InternalConfiguration& conf) override;

    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;

    /// effects: run the calibration on this event
    /// returns: whether we accepted the event
    /// guarantee: basic
    /// failures: tool dependent
    virtual StatusCode
    correctObject (xAOD::Jet& jet) override;

  private:
    /// @name CP tool handles
    /// @{

    /// description: the calibration tool
    asg::AnaToolHandle<IJetCalibrationTool> m_calibration_tool;

    /// description: the uncertainties tool
    asg::AnaToolHandle<ICPJetUncertaintiesTool> m_uncertainties_tool;

    /// description: the resolution tool
    asg::AnaToolHandle<IJERTool> m_resolution_tool;

    /// description: the smearing tool
    asg::AnaToolHandle<IJERSmearingTool> m_smearing_tool;

    /// description: the JVT recalculation tool
    asg::AnaToolHandle<IJetUpdateJvt> m_jvt_tool;

    /// The JVT efficiency tool
    asg::AnaToolHandle<CP::IJetJvtEfficiency> m_jvtEffTool;

    /// description: the b-tagged jet selection tool
    asg::AnaToolHandle<IBTaggingSelectionTool> m_bsel_tool;
    std::map< int, asg::AnaToolHandle<IBTaggingSelectionTool>> m_bsel_tools;
    asg::AnaToolHandle<IBTaggingSelectionTool> m_bsel_OR_tool;

    /// description: the cleaning tool
    asg::AnaToolHandle<IJetSelector> m_cleaning_tool;

    /// @}

  private:
    /// @name Configurations set from InternalConfiguration
    /// @{

    /// \brief whether we run on data
    bool m_isData = false;
    /// \brief whether we run on AFII MC
    bool m_isAFII = false;
    /// \brief the name of the jet container
    std::string m_jetContainer;

    /// @}

  private:
    /// @name Configuration tool properties
    /// @{

    /// Master switch to enable/disable all b-tagging
    bool m_enableBTagging;
    /// B-tag algorithm
    std::string m_btagger;
    /// B-tag working point
    std::string m_btagWP;
    /// Optional b-tag working point specifically for overlap removal
    std::string m_btagWP_OR;

    /// @}

  private:
    /// @name Cutflow bookkeeping
    /// @{
    SelectionCut cut_calibration_tool;
    SelectionCut cut_uncertainties_tool;
    SelectionCut cut_smearing_tool;
    SelectionCut cut_cleaning_tool;
    /// @}
  };



  /// @class JetToolSelect
  /// @brief QuickAna tool for applying standard selections to jets
  ///
  class JetToolSelect : public AnaToolSelect<xAOD::JetContainer>
  {

    ASG_TOOL_CLASS (JetToolSelect, ana::IAnaTool)

  public:
    //
    // public interface
    //

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    JetToolSelect (const std::string& name);

    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;

    /// returns: whether the object passes object selection
    /// guarantee: basic
    /// failures: definition dependent
    virtual StatusCode
    selectObject (xAOD::Jet& jet) override;

  public:
    /// @name Configurations -- public memebers
    /// @todo stop using public config members.
    /// @{

    ///\brief At what step should the JVT cut be applied?
    SelectionStep m_jvt_cut_step;

    /// @}

  private:
    /// @name Cutflow bookkeeping
    /// @{
    SelectionCut cut_jvt;
    /// @}
  };



  /// @class JetToolWeight
  /// @brief QuickAna tool for applying CP scale factors to jets.
  ///
  class JetToolWeight : public AnaToolWeight<xAOD::JetContainer>
  {

    ASG_TOOL_CLASS (JetToolWeight, ana::IAnaTool)

  public:
    //
    // public interface
    //

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    JetToolWeight (const std::string& name);

    /// \copydoc IAnaTool::inputTypes
    virtual unsigned inputTypes () const override;

    /// \copydoc IAnaTool::outputTypes
    virtual unsigned outputTypes () const override;

    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;

    /// \copydoc IAnaTool::execute
    /// Required to implement an event weight
    virtual StatusCode execute (IEventObjects& objects) override;

    /// effects: calculate the weight for this event
    /// returns: the calculated weight
    /// guarantee: basic
    /// failures: tool dependent
    virtual StatusCode
    objectWeight (const xAOD::Jet& jet, float& weight) override;

  private:
    /// @name CP tool handles
    /// @{

    /// description: the b-tagging efficiency tool
    asg::AnaToolHandle<IBTaggingEfficiencyTool> m_btagging_eff_tool;

    /// The JVT efficiency tool
    asg::AnaToolHandle<CP::IJetJvtEfficiency> m_jvtEffTool;

    /// @}

  private:
    /// @name Configurations -- tool properties
    /// @{
    SG::AuxElement::ConstAccessor<SelectType> m_anaSelect;
    SG::AuxElement::Accessor<float> m_anaWeight;

    /// B-tagger and working point
    std::string m_btagger;
    std::string m_btagWP;

    /// @}
  };



  // Maker function for jet tools
  StatusCode makeJetTool (DefinitionArgs& args,
                          const std::string& collection,
                          const SelectionStep& jvt_step=SelectionStep::MET,
                          const std::string& btagWP="");
}

#endif
