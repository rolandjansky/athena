/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/FatJetTool.h>

#include <JetCalibTools/IJetCalibrationTool.h>
#include <JetResolution/IJERSmearingTool.h>
#include <JetResolution/IJERTool.h>
#include <JetSelectorTools/JetCleaningTool.h>
#include <JetUncertainties/JetUncertaintiesTool.h>
#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/InternalConfiguration.h>



// For calibration tool
#define FAT_JET_JET_COLLECTION "AntiKt10LCTopo"
#define FAT_JET_CALIB_CONFIG_FILE "JES_Full2012dataset_Rscan_June2014.config"
#define FAT_JET_CALIB_SEQUENCE "EtaJES" //?

//// For uncertainties tool
#define FAT_JET_JET_DEFINITION "AntiKt10LCTopoTrimmedPtFrac5SmallR30"
#define FAT_JET_MC_TYPE "MC12a" // Or b, c also available?
#define FAT_JET_UNCERT_CONFIG_FILE "UJ_2012/JMS2012_Jul242014_Combined.config"


//// For resolution tool
// TODO: Will put these in once CP Tools are figured out
#define FAT_JET_PLOT_FILE_NAME "JetResolution/JERProviderPlots_2012.root"
#define FAT_JET_COLLECTION_NAME "AntiKt10LCTopoJets"
#define FAT_JET_BEAM_ENERGY "8TeV"
#define FAT_JET_SIMULATION_TYPE "FullSim"

//// For selection
#define FAT_JET_MAX_ABS_ETA 2.5
#define FAT_JET_MIN_PT 50e3

//// For b-tagging efficiency tool
//#define FAT_JET_TAGGER_NAME "MV1"
//#define FAT_JET_OPERATING_POINT "0_7892"
//#define FAT_JET_JET_AUTHOR "AntiKt4TopoLCJVF0_5"
//#define FAT_JET_SCALE_FACTOR_FILE_NAME "dev/xAODBTaggingEfficiency/2014-Winter-8TeV-MC12-CDI.root"


//
// method implementations
//

namespace ana
{
  FatJetToolCorrect ::
  FatJetToolCorrect (const std::string& name)
    : AsgTool (name), AnaToolCorrect<xAOD::JetContainer> (name),
      m_calibration_tool ("JetCalibrationTool", this),
      m_uncertainties_tool ("JetUncertaintiesTool", this),
      m_resolution_tool ("JERTool", this),
      m_smearing_tool ("JERSmearingTool", this)
  {
  }



  StatusCode FatJetToolCorrect ::
  initialize()
  {
    /*
    // TODO: Utilize default properties where possible!
    if (m_calibration_tool.empty())
    {
      AsgTool *ptr = nullptr;
      QUICK_ANA_MAKE_TOOL2 (m_calibration_tool, ptr, JetCalibrationTool, name() + "calibration");

      ATH_CHECK (ptr->setProperty( "JetCollection", FAT_JET_JET_COLLECTION));

      // for DC14 fullsim
      ATH_CHECK (ptr->setProperty( "ConfigFile", FAT_JET_CALIB_CONFIG_FILE));
      ATH_CHECK (ptr->setProperty( "CalibSequence", FAT_JET_CALIB_SEQUENCE));
      ATH_CHECK (ptr->setProperty( "IsData", false));
      ATH_CHECK (m_calibration_tool->initialize());
    }

    if (m_uncertainties_tool.empty())
    {
      AsgTool *ptr = nullptr;
      QUICK_ANA_MAKE_TOOL2 (m_uncertainties_tool, ptr, JetUncertaintiesTool, name() + "_uncertainties");
      ATH_CHECK (ptr->setProperty("JetDefinition", FAT_JET_JET_DEFINITION));
      ATH_CHECK (ptr->setProperty("MCType", FAT_JET_MC_TYPE));
      ATH_CHECK (ptr->setProperty("ConfigFile", FAT_JET_UNCERT_CONFIG_FILE));
      ATH_CHECK (m_uncertainties_tool->initialize());
    }
    registerTool (&*m_uncertainties_tool);

    // TODO: CP Tools don't appear to support fat jets yet
    if (m_resolution_tool.empty())
    {
      JERTool *ptr =
        new JERTool (name() + "_resolution");
      m_resolution_tool = ptr;
      ATH_CHECK (ptr->setProperty("PlotFileName", FAT_JET_PLOT_FILE_NAME));
      ATH_CHECK (ptr->setProperty("CollectionName", FAT_JET_COLLECTION_NAME));
      ATH_CHECK (ptr->setProperty("BeamEnergy", FAT_JET_BEAM_ENERGY));
      // TODO: need a way to properly identify and deal with fastsim samples.
      ATH_CHECK (ptr->setProperty("SimulationType", FAT_JET_SIMULATION_TYPE));
      ATH_CHECK (m_resolution_tool->initialize());
    }

    if (m_smearing_tool.empty())
    {
      JERSmearingTool *ptr =
        new JERSmearingTool (name() + "_smearing");
      m_smearing_tool = ptr;
      ptr->setJERTool(&*m_resolution_tool);
      ATH_CHECK (m_smearing_tool->initialize());
    }
    registerTool (&*m_smearing_tool);
    */

    return StatusCode::SUCCESS;
  }



  StatusCode FatJetToolCorrect ::
  correctObject (xAOD::Jet& /*jet*/)
  {
    /*
    // jet calibration
    QA_CHECK_CUT (cut_calibration_tool, m_calibration_tool->applyCorrection (jet));

    // jet energy scale uncertainties
    QA_CHECK_CUT (cut_uncertainties_tool, m_uncertainties_tool->applyCorrection (jet));

    // jet resolution smearing
    QA_CHECK_CUT (cut_smearing_tool, m_smearing_tool->applyCorrection (jet));
    */

    return StatusCode::SUCCESS;
  }



  FatJetToolSelect ::
  FatJetToolSelect (const std::string& name)
    : AsgTool (name), AnaToolSelect<xAOD::JetContainer> (name)
  {
  }



  StatusCode FatJetToolSelect ::
  initialize()
  {
    registerCut (SelectionStep::MET, "eta", cut_eta);
    registerCut (SelectionStep::MET, "pt", cut_pt);

    return StatusCode::SUCCESS;
  }


  StatusCode FatJetToolSelect ::
  selectObject (xAOD::Jet& jet)
  {
    // TODO: implement cuts like pt eta cuts.
    // However, it seems that all objects that are not accepted get a weight=0
    // And, there should be CP jet selection tools for this, right?

    cut_eta.setPassedIf (fabs (jet.eta()) < FAT_JET_MAX_ABS_ETA);
    cut_pt.setPassedIf (jet.pt() > FAT_JET_MIN_PT);

    return StatusCode::SUCCESS;
  }



  FatJetToolWeight ::
  FatJetToolWeight (const std::string& name)
    : AsgTool (name), AnaToolWeight<xAOD::JetContainer> (name)
  {
  }



  StatusCode FatJetToolWeight ::
  initialize()
  {
    /* TODO: The file in xAODBTaggingEfficiency/ has nothing for fat jets
    if (m_btagging_eff_tool.empty())
    {
      AsgTool *ptr = nullptr;
      QUICK_ANA_MAKE_TOOL2 (m_btagging_eff_tool, ptr, BTaggingEfficiencyTool,
                            name() + "_btagging_eff");
      ATH_CHECK (ptr->setProperty("TaggerName", FAT_JET_TAGGER_NAME));
      ATH_CHECK (ptr->setProperty("OperatingPoint", FAT_JET_OPERATING_POINT));
      ATH_CHECK (ptr->setProperty("JetAuthor", FAT_JET_JET_AUTHOR));
      ATH_CHECK (ptr->setProperty("ScaleFactorFileName", FAT_JET_SCALE_FACTOR_FILE_NAME));
      //ATH_CHECK (ptr->setProperty("EfficiencyFileName", ""));
      //ATH_CHECK (ptr->setProperty("SystematicStrategy", "Envelope"));

      ATH_CHECK (ptr->setProperty( "ConeFlavourLabel", true));
      ATH_CHECK (m_btagging_eff_tool->initialize());
    }
    */

    return StatusCode::SUCCESS;
  }


  StatusCode FatJetToolWeight ::
  objectWeight (const xAOD::Jet& jet, float& weight)
  {
    // rationale: this is temporary, until the problems with
    // BTaggingEfficiencyTool are worked out
    if (std::fabs(jet.eta()) > FAT_JET_MAX_ABS_ETA) {
      // OutOfValidityRange
      weight = 1;
      return StatusCode::SUCCESS;
    }
    /*

    QA_CHECK_WEIGHT (weight, m_btagging_eff_tool->getScaleFactor (jet, weight));
    */

    return StatusCode::SUCCESS;
  }
}

namespace
{
  StatusCode makeFatJetTool (ana::DefinitionArgs& args)
  {
    args.add (std::unique_ptr<ana::IAnaTool>
      (new ana::AnaToolRetrieve (args.prefix() + "_retrieve", FAT_JET_COLLECTION_NAME)));
    args.add (std::unique_ptr<ana::FatJetToolCorrect>
      (new ana::FatJetToolCorrect (args.prefix() + "_correct")));
    args.add (std::unique_ptr<ana::FatJetToolSelect>
      (new ana::FatJetToolSelect (args.prefix() + "_select")));
    if (args.configuration()->isData() == false)
    {
      args.add (std::unique_ptr<ana::FatJetToolWeight>
		(new ana::FatJetToolWeight (args.prefix() + "_weight")));
    }
    return StatusCode::SUCCESS;
  }
}

QUICK_ANA_JET_DEFINITION_MAKER ("antikt10", makeFatJetTool (args))
