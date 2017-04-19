/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__TRIGGER_TOOL_H
#define QUICK_ANA__TRIGGER_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/xAODInclude.h>
#include <AsgTools/ToolHandle.h>




namespace Trig{
  class TrigDecisionTool;
  class IMatchingTool;
}

namespace TrigConf {class ITrigConfigTool;}

// Scale factor clases
namespace CP {class IMuonTriggerScaleFactors;}
class IAsgElectronEfficiencyCorrectionTool;

namespace ana
{
  /// TODO: needs documentation
  class TriggerTool : public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (TriggerTool, ana::IAnaTool)

  public:

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    TriggerTool (const std::string& name);


    /// \copydoc IAnaTool::useInitialConfiguration
    virtual StatusCode
    useInitialConfiguration (const InternalConfiguration& conf) override;


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;


    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) override;



    /// returns: the processing step we are working on
    /// guarantee: no-fail
    virtual AnalysisStep step () const override;


    /// \copydoc IAnaTool::inputTypes
    virtual unsigned inputTypes () const override;


    /// \copydoc IAnaTool::outputTypes
    virtual unsigned outputTypes () const override;


    /// effects: apply this analysis tool to the objects
    /// guarantee: basic
    /// failures: tool dependent
    virtual StatusCode execute (IEventObjects& objects) override;

    /// Enum to describe year configuration
    enum TrigYear {
      Only2015,
      Only2016,
      Only2015_2016
    };

  protected:

    /// \brief print overlap removal debugging information

  private:
    std::string m_trigList;
    std::string m_groupName;
    std::string m_muon_trig_str;
    bool m_matchEl;
    bool m_matchMu;
    /// Is using AF2?
    bool m_isAF2;
    /// Is using data?
    bool m_isData;
    /// Which muon working point?
    std::string m_muon_wp;
    /// Which electron working point?
    std::string m_ele_wp;
    /// Which muon isolation working point?
    std::string m_muon_iso_wp;
    /// Which electron isolation working point?
    std::string m_ele_iso_wp;
    /// Do we have a reasonable electron setup?
    bool m_isEleToolSetup;

    /// The pT threshold of the trigger.  Muon tools only want
    //  to be sent muons at least above a certain pT
    double m_TriggerPtThreshold;

    // For holding the year configuration
    //  Note: enum doesn't compile in athena; int doesn't run in RootCore
    int m_year;

    asg::AnaToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
    asg::AnaToolHandle<TrigConf::ITrigConfigTool> m_trigconfigTool;
    // Muon Trigger Matching
    asg::AnaToolHandle<Trig::IMatchingTool> m_triggerMatching;
    // Muon trigger scale factors
    asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> m_mu_trig_sf2015;
    asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> m_mu_trig_sf2016;
    // Electron trigger scale factor
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_el_trig_sf;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_el_trig_eff;

    bool isTrigMatched (std::vector<const xAOD::IParticle*> myParticles, std::string trigger);

    asg::AnaToolHandle<CP::IMuonTriggerScaleFactors>& muonSFToolForThisYear(const int runNumber);

  };
}

#endif
