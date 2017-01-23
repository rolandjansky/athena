/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Includes
//

#include <QuickAna/PhotonTool.h>

#include <ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h>
#include <ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h>
#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>

#include <PhotonEfficiencyCorrection/AsgPhotonEfficiencyCorrectionTool.h>
#include <ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool.h>
#include <IsolationSelection/IsolationSelectionTool.h>
#include <IsolationCorrections/IsolationCorrectionTool.h>
#include <xAODTracking/TrackParticlexAODHelpers.h>

// The photon tools are apparently lazy?
#include <PathResolver/PathResolver.h>

//
// Method implementations
//
namespace ana
{
  // const float GeV = 1000.;

  // Constructor
  PhotonToolCorrect ::
  PhotonToolCorrect (const std::string& name)
    : AsgTool (name), AnaToolCorrect<xAOD::PhotonContainer> (name),
      m_calibration ("EgammaCalibrationAndSmearingTool", this),
      m_fudgeMCTool ("ElectronPhotonShowerShapeFudgeTool", this),
      m_isoCorrTool ("IsolationCorrectionTool", this)
  {
    declareProperty("IsAF2", m_isAF2=false);
    declareProperty("IsData", m_isData=false);
  }


  // Initialization
  StatusCode PhotonToolCorrect ::
  initialize()
  {
    ATH_MSG_INFO("initialize");
    ATH_MSG_WARNING("This tool currently skips calibration on author 128 " <<
                    "photons to avoid errors");
    // Initialize the calibration tool
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_calibration, CP::EgammaCalibrationAndSmearingTool));
    ATH_CHECK (m_calibration.setProperty ("ESModel", "es2016PRE"));
    ATH_CHECK (m_calibration.setProperty ("useAFII", m_isAF2?1:0));
    ATH_CHECK (m_calibration.setProperty ("decorrelationModel", "1NP_v1"));
    ATH_CHECK (m_calibration.initialize());
    registerTool (&*m_calibration);

    // Initialize Fudge Tool
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_fudgeMCTool, ElectronPhotonShowerShapeFudgeTool));
    ATH_CHECK (m_fudgeMCTool.setProperty ("Preselection", 16)); // 16 == MC15
    ATH_CHECK (m_fudgeMCTool.initialize());

    // Initialize isolation correction tool
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_isoCorrTool, CP::IsolationCorrectionTool));
    ATH_CHECK( m_isoCorrTool.setProperty( "IsMC", !m_isData) );
    ATH_CHECK( m_isoCorrTool.setProperty( "AFII_corr", m_isAF2) );
    ATH_CHECK( m_isoCorrTool.initialize() );

    // Set the cut steps
    registerCut (SelectionStep::MET, "fudge_tool", cut_fudge_tool);
    registerCut (SelectionStep::MET, "calibration_tool", cut_calibration_tool);
    registerCut (SelectionStep::MET, "iso_corr_tool", cut_isoCorr_tool);

    return StatusCode::SUCCESS;
  }


  // Object correction
  StatusCode PhotonToolCorrect ::
  correctObject (xAOD::Photon& photon)
  {
    if ((photon.author() & xAOD::EgammaParameters::AuthorPhoton) || (photon.author() & xAOD::EgammaParameters::AuthorAmbiguous)){
      QA_CHECK_CUT (cut_calibration_tool, m_calibration->applyCorrection(photon));
    }

    // Do *not* apply smearing to AF2:
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaIdentificationRun2
    if (m_isAF2 || m_isData){
      cut_fudge_tool.setPassedIf(true);
    } else {
      QA_CHECK_CUT (cut_fudge_tool, m_fudgeMCTool->applyCorrection(photon));
    }

    // Correct the isolation
    QA_CHECK_CUT (cut_isoCorr_tool, m_isoCorrTool->applyCorrection(photon));

    return StatusCode::SUCCESS;
  }


  // Constructor
  PhotonToolSelect ::
  PhotonToolSelect (const std::string& name)
    : AsgTool (name), AnaToolSelect<xAOD::PhotonContainer> (name),
      quality(egammaPID::PhotonTight),
      m_selection ("PhotonIsEMSelector", this),
      m_isolationTool ("IsolationSelectionTool", this),
      m_isolationOn(false)

  {
    declareProperty( "IsolationWP", m_isolationWP = "", "Working point to use" );
  }


  // Initialization
  StatusCode PhotonToolSelect ::
  initialize()
  {
    ATH_MSG_DEBUG("initialize");

    // Initalize Photon Selector
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_selection, AsgPhotonIsEMSelector));
    ATH_CHECK (m_selection.setProperty("isEMMask",quality));
    ATH_CHECK (m_selection.setProperty("ConfigFile",
      "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMTightSelectorCutDefs.conf"));
    ATH_CHECK (m_selection.initialize());

    registerCut (SelectionStep::MET, "quality", cut_OQ);
    registerCut (SelectionStep::MET, "author", cut_author);
    registerCut (SelectionStep::MET, "cleaning", cut_cleaning);
    registerCut (SelectionStep::MET, "selection", cut_selection);
    registerCut (SelectionStep::MET, "isolationTool", cut_isolationTool);
    ATH_MSG_INFO("initialize");
    if (!(m_isolationWP==""))
    {
      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_isolationTool, CP::IsolationSelectionTool));
      ATH_CHECK (m_isolationTool.setProperty("PhotonWP", m_isolationWP));
      ATH_CHECK (m_isolationTool.initialize());
      registerTool(&*m_isolationTool);
      m_isolationOn = true;
    }

    return StatusCode::SUCCESS;
  }


  // Object selection
  StatusCode PhotonToolSelect ::
  selectObject (xAOD::Photon& photon)
  {
    // Object quality
    cut_OQ.setPassedIf (photon.isGoodOQ(xAOD::EgammaParameters::BADCLUSPHOTON));

    // Photon author requirement; see:
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaIdentificationRun2
    cut_author.setPassedIf( (photon.author() & xAOD::EgammaParameters::AuthorPhoton) ||
                            (photon.author() & xAOD::EgammaParameters::AuthorAmbiguous) );

    // Photon cleaning from the same TWiki
    // Damn... These should really be in a CP tool!
    cut_cleaning.setPassedIf( !( (photon.OQ()&134217728)!=0 &&
                               (photon.showerShapeValue(xAOD::EgammaParameters::Reta)>0.98 ||
                                photon.showerShapeValue(xAOD::EgammaParameters::Rphi)>1.0 ||
                                (photon.OQ()&67108864)!=0) ) );

    // Using MC15/R20 selections
    cut_selection.setPassedIf (m_selection->accept (&photon));

    // Apply isolation tool
    if (m_isolationOn) {
      cut_isolationTool.setPassedIf (m_isolationTool->accept(photon));
    }

    // Photon eta should be calculated as
    // input.caloCluster()->etaBE(2)
    // according to :
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaIdentificationRun2#Fiducial_region_and_Calorimeter
    // We should really try to find a way to do this
    //  Only needed in the case of loose ID!

    return StatusCode::SUCCESS;
  }



  // Constructor
  PhotonToolWeight ::
  PhotonToolWeight (const std::string& name)
    : AsgTool (name), AnaToolWeight<xAOD::PhotonContainer> (name),
      m_efficiencyTool ("IAsgPhotonEfficiencyCorrectionTool_eff", this),
      m_isoSFTool ("IAsgPhotonEfficiencyCorrectionTool_iso", this),
    m_doIsolation (false)
  {
    declareProperty("IsAF2", m_isAF2=false);
    declareProperty( "IsolationWP", m_isolationWP = "", "Working point to use" );
  }


  // Initialization
  StatusCode PhotonToolWeight ::
  initialize()
  {
    ATH_MSG_INFO("initialize");

    // Initialize the AsgPhotonEfficiencyCorrectionTool.
    ATH_CHECK (ASG_MAKE_ANA_TOOL (m_efficiencyTool, AsgPhotonEfficiencyCorrectionTool));
    // Reco scale factors - currently need to set this file by hand
    std::string file_unc = PathResolverFindCalibFile(m_isAF2?
          "PhotonEfficiencyCorrection/v1/efficiencySF.offline.Tight.2015.13TeV.rel20.AFII.unc.v01.root":
          "PhotonEfficiencyCorrection/v1/efficiencySF.offline.Tight.2015.13TeV.rel20.unc.v02.root");
    std::string file_con = PathResolverFindCalibFile(m_isAF2?
          "PhotonEfficiencyCorrection/v1/efficiencySF.offline.Tight.2015.13TeV.rel20.AFII.con.v01.root":
          "PhotonEfficiencyCorrection/v1/efficiencySF.offline.Tight.2015.13TeV.rel20.con.v02.root");

    ATH_CHECK( m_efficiencyTool.setProperty("CorrectionFileNameConv",file_con) );
    ATH_CHECK( m_efficiencyTool.setProperty("CorrectionFileNameUnconv",file_unc) );
    ATH_CHECK( m_efficiencyTool.setProperty("ForceDataType", 1 + (m_isAF2?2:0)) );
    ATH_CHECK( m_efficiencyTool.initialize() );
    registerTool(&*m_efficiencyTool);

    if (m_isolationWP == "FixedCutTight" || m_isolationWP == "FixedCutLoose")
    {
      // Initialize the AsgPhotonEfficiencyCorrectionTool.
      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_isoSFTool, AsgPhotonEfficiencyCorrectionTool));
      std::string file_unc_iso = PathResolverFindCalibFile("PhotonEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v1/isolation/efficiencySF.Isolation.isol"
             + m_isolationWP + ".2016.13TeV.rel20.7.25ns.unc.v01.root");
      std::string file_con_iso = PathResolverFindCalibFile("PhotonEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v1/isolation/efficiencySF.Isolation.isol"
             + m_isolationWP + ".2016.13TeV.rel20.7.25ns.con.v01.root");
      ATH_CHECK( m_isoSFTool.setProperty("CorrectionFileNameConv", file_con_iso) );
      ATH_CHECK( m_isoSFTool.setProperty("CorrectionFileNameUnconv", file_unc_iso) );
      ATH_CHECK( m_isoSFTool.setProperty("ForceDataType", 1 + (m_isAF2?2:0)) );
      ATH_CHECK( m_isoSFTool.initialize() );
      registerTool(&*m_isoSFTool);
      m_doIsolation = true;
    }

    return StatusCode::SUCCESS;
  }


  // Object weighting
  StatusCode PhotonToolWeight ::
  objectWeight (const xAOD::Photon& photon, float& weight)
  {
    // Photon efficiency SF
    weight = 1;

    double eff_weight=1.;

    // Reconstruction efficiency scale factor
    if (m_efficiencyTool->getEfficiencyScaleFactor(photon,eff_weight) ==
        CP::CorrectionCode::Ok)
      weight *= eff_weight;

    if (m_doIsolation){
      double iso_weight=1.;
      if (m_isoSFTool->getEfficiencyScaleFactor(photon,iso_weight) ==
          CP::CorrectionCode::Ok)
        weight *= iso_weight;
    }

    return StatusCode::SUCCESS;
  }



  StatusCode makePhotonTool (DefinitionArgs& args, unsigned int quality,
                             std::string isolationWP )
  {
    using namespace msgObjectDefinition;

    const std::string inputContainer = "Photons";
    const bool isAFII = args.configuration()->isAFII();
    const bool isData = args.configuration()->isData();

    // Retrieval tool
    args.add (std::unique_ptr<ana::IAnaTool>
      (new AnaToolRetrieve (args.prefix() + "_retrieve", inputContainer)));
    // Correction tool
    std::unique_ptr<PhotonToolCorrect> correctTool
      (new PhotonToolCorrect (args.prefix() + "_correct"));
    ANA_CHECK
      ( correctTool->setProperty("IsAF2", isAFII) );
    ANA_CHECK
      ( correctTool->setProperty("IsData", isData) );
    args.add( std::move(correctTool) );
    // Selection tool
    {
      std::unique_ptr<PhotonToolSelect> tool
        (new PhotonToolSelect (args.prefix() + "_select"));
      tool->quality = quality;
      ANA_CHECK (tool->setProperty("IsolationWP", isolationWP));
      args.add (std::move (tool));
    }
    if (!isData)
    {
      // Weight tool
      std::unique_ptr<PhotonToolWeight> weightTool
        (new PhotonToolWeight (args.prefix() + "_weight"));
      ANA_CHECK
        ( weightTool->setProperty("IsAF2", isAFII) );
      ANA_CHECK
        ( weightTool->setProperty("IsolationWP", isolationWP));
      args.add ( std::move(weightTool) );
    }
    return StatusCode::SUCCESS;
  }



  QUICK_ANA_PHOTON_DEFINITION_MAKER ("tight", makePhotonTool (args,egammaPID::PhotonTight,""))
  QUICK_ANA_PHOTON_DEFINITION_MAKER ("loose", makePhotonTool (args,egammaPID::PhotonLoose,""))

  QUICK_ANA_PHOTON_DEFINITION_MAKER ("tight_FixedCutTight", makePhotonTool (args,egammaPID::PhotonTight,"FixedCutTight"))




}
