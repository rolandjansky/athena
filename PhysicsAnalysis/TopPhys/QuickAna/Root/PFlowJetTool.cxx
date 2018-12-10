/*
  Copyright (C) 2018-2021 CERN for the benefit of the ATLAS collaboration
*/

// Please feel free to contact me (cong.geng@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/PFlowJetTool.h>

#include <JetCalibTools/JetCalibrationTool.h>
#include <JetResolution/JERSmearingTool.h>
#include <JetResolution/JERTool.h>
#include <JetSelectorTools/JetCleaningTool.h>
#include <JetUncertainties/JetUncertaintiesTool.h>
#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>
#include <xAODBTaggingEfficiency/BTaggingEfficiencyTool.h>
#include <xAODBTaggingEfficiency/BTaggingSelectionTool.h>
#include <JetMomentTools/JetVertexTaggerTool.h>
#include <JetJvtEfficiency/JetJvtEfficiency.h>
#include <AthContainers/ConstDataVector.h>

static const float GeV = 1000.;
static const float TeV = 1e6;

// BTag config options currently depend on release
namespace
{
  const char* btagAlgDefault = "MV2c10";
  const std::string bTagCalibFile =
    "xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2018-10-19_v1.root";
  const char *jesFile_pflow = "JES_data2017_2016_2015_Consolidated_PFlow_2018_Rel21.config";
  const char *jesFile_pflow_AFII = "JES_MC16Recommendation_AFII_PFlow_April2018_rel21.config";
  const std::string uncertConfigFile = "rel21/Fall2018/R4_SR_Scenario1.config";
  const char *mcType = "MC16";
}

//
// method implementations
//

namespace ana
{
  PFlowJetToolCorrect ::
  PFlowJetToolCorrect (const std::string& name)
    : AsgTool (name), AnaToolCorrect<xAOD::JetContainer> (name),
      m_calibration_tool ("calibration", this),
      m_uncertainties_tool ("uncertainties", this),
      m_resolution_tool ("resolution", this),
      m_smearing_tool ("smearing", this),
      m_jvt_tool ("jvt", this),
      m_jvtEffTool("jvt_eff", this),
      m_bsel_tool ("btag", this),
      m_bsel_OR_tool ("btag_OR", this)
  {
    declareProperty("EnableBTagging", m_enableBTagging = true);
    declareProperty("BTagger", m_btagger = btagAlgDefault);
    declareProperty("BTagWP", m_btagWP = "FixedCutBEff_77");
    // Set to a large negative number to disable
    // TODO: add a better way to disable
    declareProperty("BTagWP_OR", m_btagWP_OR = "FixedCutBEff_85");
  }



  StatusCode PFlowJetToolCorrect ::
  useInitialConfiguration (const InternalConfiguration& conf)
  {
    ATH_CHECK (AnaTool::useInitialConfiguration (conf));

    m_isData = conf.isData();
    m_isAFII = conf.isAFII();

    m_jetContainer = conf.inputName (OBJECT_PFLOW_JET);
    if (m_jetContainer.empty())
    {
      ATH_MSG_ERROR ("can't apply correction without jet collection name");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode PFlowJetToolCorrect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");

    // Strip off the "Jets" suffix in the jet collection name
    // @TODO: update AnaToolHandle tool creation mechanism
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_calibration_tool, JetCalibrationTool));
    const auto jetCollection = m_jetContainer.substr(0, m_jetContainer.size()-4);
    const std::string configFile = m_isAFII ? jesFile_pflow_AFII : jesFile_pflow;
    const std::string calibSeq = m_isData ? "JetArea_Residual_EtaJES_GSC_Insitu"
                                          : "JetArea_Residual_EtaJES_GSC_Smear";
    ATH_CHECK( m_calibration_tool.setProperty("CalibArea", "00-04-82") );
    ATH_CHECK( m_calibration_tool.setProperty("JetCollection", jetCollection) );
    ATH_CHECK( m_calibration_tool.setProperty("ConfigFile", configFile) );
    ATH_CHECK( m_calibration_tool.setProperty("CalibSequence", calibSeq) );
    ATH_CHECK( m_calibration_tool.setProperty("IsData", m_isData) );
    ATH_CHECK( m_calibration_tool.initialize() );

    // NOTE: using the strongly reduced uncertainty breakdown. JetEtMiss says
    // that analyses MUST evaluate all four strongly reduced scenarios before
    // adopting this one!

    // @TODO: update AnaToolHandle tool creation mechanism
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_uncertainties_tool, JetUncertaintiesTool) );
    ATH_CHECK( m_uncertainties_tool.setProperty("JetDefinition", jetCollection) );
    ATH_CHECK( m_uncertainties_tool.setProperty("MCType", m_isAFII ? "AFII" : mcType) );
    ATH_CHECK( m_uncertainties_tool.setProperty("ConfigFile", uncertConfigFile) );
    ATH_CHECK( m_uncertainties_tool.setProperty("CalibArea", "CalibArea-06") );
    ATH_CHECK( m_uncertainties_tool.initialize() );
    registerTool( &*m_uncertainties_tool);

    // JER MC15 pre-recommendations

    // Specify the JER file - Don't want behavior to change with the same tag of
    // QuickAna just because the package underlying was updated (JER in this case)
    const std::string jerFile =
      "JetResolution/Prerec2015_xCalib_2012JER_ReducedTo9NP_Plots_v2.root";
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_resolution_tool, JERTool));
    ATH_CHECK( m_resolution_tool.setProperty("PlotFileName", jerFile) );
    ATH_CHECK( m_resolution_tool.setProperty("CollectionName", m_jetContainer) );
    ATH_CHECK( m_resolution_tool.initialize() );

    // @TODO: update AnaToolHandle tool creation mechanism
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_smearing_tool, JERSmearingTool) );
    ATH_CHECK( m_smearing_tool.setProperty("isMC", !m_isData) );
    ATH_CHECK( m_smearing_tool.setProperty("ApplyNominalSmearing", false) );
    ATH_CHECK( m_smearing_tool.setProperty("SystematicMode", "Simple") );
    ATH_CHECK( m_smearing_tool.setProperty
                 ("JERTool", m_resolution_tool) );
    ATH_CHECK( m_smearing_tool.initialize() );
    registerTool( &*m_smearing_tool );

    // JVT tool
    const std::string jvtFile = "JetMomentTools/JVTlikelihood_20140805.root";
    // @TODO: update AnaToolHandle tool creation mechanism
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_jvt_tool, JetVertexTaggerTool) );
    ATH_CHECK( m_jvt_tool.setProperty("JVTFileName", jvtFile) );
    ATH_CHECK( m_jvt_tool.initialize() );

    // JVT efficiency SF
    //  From https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JVTCalibration
    // @TODO update tool creation mechanism
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_jvtEffTool, CP::JetJvtEfficiency) );
    // The default working point is the recommended one
    //ATH_CHECK( m_jvtEffTool.setProperty("WorkingPoint","Default") );
    ATH_CHECK( m_jvtEffTool.setProperty("SFFile","JetJvtEfficiency/Moriond2018/JvtSFFile_EMPFlow.root") );
    ATH_CHECK( m_jvtEffTool.initialize() );
    registerTool (&*m_jvtEffTool);

    // b-tagging tools
    if(m_enableBTagging) {
      // @TODO: update AnaToolHandle tool creation mechanism
      ATH_CHECK( ASG_MAKE_ANA_TOOL(m_bsel_tool, BTaggingSelectionTool) );
      ATH_CHECK( m_bsel_tool.setProperty("TaggerName", m_btagger) );
      ATH_CHECK( m_bsel_tool.setProperty("OperatingPoint", m_btagWP) );
      ATH_CHECK( m_bsel_tool.setProperty("JetAuthor", m_jetContainer) );
      ATH_CHECK( m_bsel_tool.setProperty("FlvTagCutDefinitionsFileName", bTagCalibFile) );
      ATH_CHECK( m_bsel_tool.initialize() );

      ATH_CHECK( ASG_MAKE_ANA_TOOL(m_bsel_OR_tool, BTaggingSelectionTool) );
      ATH_CHECK( m_bsel_OR_tool.setProperty("TaggerName", m_btagger) );
      ATH_CHECK( m_bsel_OR_tool.setProperty("OperatingPoint", m_btagWP_OR) );
      ATH_CHECK( m_bsel_OR_tool.setProperty("JetAuthor", m_jetContainer) );
      ATH_CHECK( m_bsel_OR_tool.setProperty("FlvTagCutDefinitionsFileName", bTagCalibFile) );
      ATH_CHECK( m_bsel_OR_tool.initialize() );
    }

    registerCut (SelectionStep::MET, "calibration_tool", cut_calibration_tool);
    registerCut (SelectionStep::MET, "uncertainties_tool", cut_uncertainties_tool);
    registerCut (SelectionStep::MET, "smearing_tool", cut_smearing_tool);

    return StatusCode::SUCCESS;
  }



  StatusCode PFlowJetToolCorrect ::
  correctObject (xAOD::Jet& jet)
  {
    ATH_MSG_DEBUG("correctObject");
    // jet calibration
    QA_CHECK_CUT( cut_calibration_tool, m_calibration_tool->applyCorrection(jet) );

    // jet energy scale uncertainties
    QA_CHECK_CUT( cut_uncertainties_tool, m_uncertainties_tool->applyCorrection(jet) );

    // jet resolution smearing
    QA_CHECK_CUT( cut_smearing_tool, m_smearing_tool->applyCorrection(jet) );

    // JVT Recalculation after calibration
    float jvt = m_jvt_tool->updateJvt(jet);
    // Update "Jvt" of the jet - required by the MET tool
    jet.auxdata<float>("Jvt") = jvt;
    bool jvt_pass = m_jvtEffTool->passesJvtCut(jet);
    jet.auxdata<char>("Jvt_pass") = jvt_pass;

    // B-tagging section
    if (m_enableBTagging)
    {
      // BTagging valid in pt > 20, |eta| < 2.5
      bool inBTagKinRange = jet.pt() > 20.*GeV && std::abs(jet.eta()) < 2.5;

      // B-Jet criteria
      bool isbjet = ( inBTagKinRange && jvt_pass && m_bsel_tool->accept(jet) );
      jet.auxdecor<char>("bjet") = isbjet;

      // Apply the dedicated bjet decoration for overlap removal as well.
      // Working point can be different from standard one.
      bool isbjet_OR = isbjet;
      if (m_btagWP_OR != m_btagWP) {
        isbjet_OR = ( inBTagKinRange && jvt_pass && m_bsel_OR_tool->accept(jet) );
      }
      jet.auxdecor<char>("bjet_OR") = isbjet_OR;

      // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertainties2015Prerec#Tool_requirements_and_assumption
      // Explains that this decoration *must* be called IsBjet for the
      // nuisance parameter breakdown to work.
      // - Why can't we just adopt this as our convention, then??
      jet.auxdecor<char>("IsBjet") = isbjet;
    }

    return StatusCode::SUCCESS;
  }



  PFlowJetToolSelect ::
  PFlowJetToolSelect (const std::string& name)
    : AsgTool (name), AnaToolSelect<xAOD::JetContainer> (name),
      m_jvt_cut_step (SelectionStep::MET)
  {
  }



  StatusCode PFlowJetToolSelect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");

    // For SUSY: Make sure to *not* cut jets before they hit the overlap removal!
    // Just decorate.
    registerCut (m_jvt_cut_step, "jvt", cut_jvt);

    return StatusCode::SUCCESS;
  }



  StatusCode PFlowJetToolSelect ::
  selectObject (xAOD::Jet& jet)
  {
    ATH_MSG_DEBUG("selectObject");

    // Apply the recommended selection
    cut_jvt.setPassedIf ( jet.getAttribute<char>("Jvt_pass") );

    return StatusCode::SUCCESS;
  }



  PFlowJetToolWeight ::
  PFlowJetToolWeight (const std::string& name)
    : AsgTool (name), AnaToolWeight<xAOD::JetContainer> (name),
      m_btagging_eff_tool ("btagging_eff", this),
      m_jvtEffTool("jvt_eff", this),
      m_anaSelect ("ana_select"),
      m_anaWeight ("ana_weight")
  {
    declareProperty("BTagger", m_btagger = btagAlgDefault);
    declareProperty("BTagWP", m_btagWP = "-0_4434");
  }


  unsigned PFlowJetToolWeight ::
  inputTypes () const
  {
    return (1 << OBJECT_EVENTINFO) | (1 << OBJECT_PFLOW_JET);
  }


  unsigned PFlowJetToolWeight ::
  outputTypes () const
  {
    return (1 << OBJECT_EVENTINFO) | (1 << OBJECT_PFLOW_JET);
  }


  StatusCode PFlowJetToolWeight ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");

    // BTag efficiency SF
    // Recommendations come from
    //  https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTagCalib2015

    // @TODO update tool creation mechanism
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_btagging_eff_tool, BTaggingEfficiencyTool) );
    ATH_CHECK( m_btagging_eff_tool.setProperty("TaggerName", m_btagger) );
    // really stupid that these have different formats
    ATH_CHECK( m_btagging_eff_tool.setProperty("OperatingPoint", m_btagWP) );
    ATH_CHECK( m_btagging_eff_tool.setProperty("JetAuthor", "AntiKt4EMPFlowJets") );
    ATH_CHECK( m_btagging_eff_tool.setProperty("ScaleFactorFileName", bTagCalibFile) );
    ATH_CHECK( m_btagging_eff_tool.setProperty("SystematicsStrategy", "Envelope") );
    ATH_CHECK( m_btagging_eff_tool.initialize() );
    // register for systematics
    registerTool (&*m_btagging_eff_tool);

    // JVT efficiency SF
    //  From https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JVTCalibration
    // @TODO update tool creation mechanism
    ATH_CHECK( ASG_MAKE_ANA_TOOL(m_jvtEffTool, CP::JetJvtEfficiency) );
    // The default working point is the recommended one
    //ATH_CHECK( m_jvtEffTool.setProperty("WorkingPoint","Default") );
    ATH_CHECK( m_jvtEffTool.setProperty("SFFile","JetJvtEfficiency/Moriond2018/JvtSFFile_EMPFlow.root") );
    ATH_CHECK( m_jvtEffTool.initialize() );
    registerTool (&*m_jvtEffTool);

    return StatusCode::SUCCESS;
  }


  StatusCode PFlowJetToolWeight ::
  execute (IEventObjects& objects)
  {

    ConstDataVector<xAOD::JetContainer> jvtjets(SG::VIEW_ELEMENTS);
    for (auto object : *objects.pflow_jets())
    {
      float weight = 1;
      if (m_anaSelect (*object))
      {
        ATH_CHECK (this->objectWeight (*object, weight));
        if (!(weight > 0))
        {
          ATH_MSG_WARNING ("object weight of " << weight <<
                             " is not allowed: pt=" << object->pt() <<
                             " eta=" << object->eta() <<
                             " phi=" << object->phi());
          //return StatusCode::FAILURE;
        }
        jvtjets.push_back( object );
      } else
      {
        weight = 1;
      }
      m_anaWeight (*object) = weight;
    }

    float totalSF=1.;

    CP::CorrectionCode ret = m_jvtEffTool->applyAllEfficiencyScaleFactor( jvtjets.asDataVector() , totalSF );

    switch (ret) {
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve SF for jet in SUSYTools_xAOD::JVT_SF" );
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "No valid SF for jet in SUSYTools_xAOD::JVT_SF" );
    default:
      ATH_MSG_VERBOSE( " Retrieve SF for jet container in SUSYTools_xAOD::JVT_SF with value " << totalSF );
    }
    objects.eventinfo()->auxdata<float>("JVT_SF") = totalSF;


    return StatusCode::SUCCESS;
  }


  StatusCode PFlowJetToolWeight ::
  objectWeight (const xAOD::Jet& jet, float& weight)
  {
    ATH_MSG_DEBUG("objectWeight");

    // Apply btag efficiency SF.
    // The btag tool only supports pt > 20 and |eta| < 2.5
    if (jet.pt() > 20.*GeV && jet.pt() <= 1.*TeV && std::abs(jet.eta()) < 2.5)
    {
      // If the jet is b-tagged, use the efficiency weight
      if (jet.auxdecor<char>("IsBjet")) {
        QA_CHECK_WEIGHT( float, weight,
                         m_btagging_eff_tool->getScaleFactor(jet, weight) );
      }
      // If it is not, then use the *inefficiency* weight
      else {
        QA_CHECK_WEIGHT( float, weight,
                         m_btagging_eff_tool->getInefficiencyScaleFactor(jet, weight) );
      }
    }

    return StatusCode::SUCCESS;
  }


  // Maker function for jet tools
  StatusCode makePFlowJetTool (ana::DefinitionArgs& args,
                          const std::string& collection,
                          const SelectionStep& jvt_step,
                          const std::string& btagWP)
  {
    using namespace msgObjectDefinition;

    const bool useBTagging = !btagWP.empty();
    const auto config = args.configuration();

    args.add (std::unique_ptr<ana::IAnaTool>
      (new ana::AnaToolRetrieve (args.prefix() + "_retrieve", collection)));

    std::unique_ptr<ana::PFlowJetToolCorrect> correctTool
      (new ana::PFlowJetToolCorrect (args.prefix() + "_correct"));
    ANA_CHECK( correctTool->setProperty("EnableBTagging", useBTagging) );
    ANA_CHECK( correctTool->setProperty("BTagWP", btagWP) );
    args.add (std::move (correctTool));

    std::unique_ptr<ana::PFlowJetToolSelect> selectTool
      (new ana::PFlowJetToolSelect (args.prefix() + "_select"));
    selectTool->m_jvt_cut_step = jvt_step;
    args.add (std::move (selectTool));

    // Only apply jet weights to MC.
    // Also, at the moment we're only applying btag-related weights, so
    // disable the weight tool if we're not using btagging
    if (config->isData() == false && useBTagging)
    {
      std::unique_ptr<ana::PFlowJetToolWeight> weightTool
        (new ana::PFlowJetToolWeight (args.prefix() + "_weight"));
      ANA_CHECK( weightTool->setProperty("BTagWP", btagWP));
      args.add (std::move (weightTool));
    }
    return StatusCode::SUCCESS;
  }

  QUICK_ANA_JET_DEFINITION_MAKER ("pflow_HZZ",
    makePFlowJetTool (args, "AntiKt4EMPFlowJets", SelectionStep::ANALYSIS,
                      "FixedCutBEff_85"))

}
