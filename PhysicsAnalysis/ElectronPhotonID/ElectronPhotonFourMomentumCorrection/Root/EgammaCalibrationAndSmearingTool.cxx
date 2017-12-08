/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <utility>

#include <boost/format.hpp>

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODEventInfo/EventInfo.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"
#include "CxxUtils/make_unique.h"
#include "xAODMetaData/FileMetaData.h"
#ifndef ROOTCORE
#include <boost/algorithm/string.hpp>
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

// internal (old) tool
#include "ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h"

#include "egammaMVACalib/egammaMVATool.h"
#include "egammaLayerRecalibTool/egammaLayerRecalibTool.h"
#include "ElectronPhotonFourMomentumCorrection/GainTool.h"

#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"


namespace CP {

const double GeV = 1000.;

std::unique_ptr<egGain::GainTool> gainToolFactory(egEnergyCorr::ESModel model)
{
  switch (model)
  {
    case egEnergyCorr::es2011d:
    case egEnergyCorr::es2011dMedium:
    case egEnergyCorr::es2011dTight:
    case egEnergyCorr::es2012c:
    case egEnergyCorr::es2012XX:
    case egEnergyCorr::es2015PRE:
    case egEnergyCorr::es2015cPRE:
    case egEnergyCorr::es2015PRE_res_improved:
    case egEnergyCorr::es2015cPRE_res_improved:
    case egEnergyCorr::es2015c_summer:
    case egEnergyCorr::es2015_day0_3percent:
    case egEnergyCorr::es2016PRE:
    {
      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      return CxxUtils::make_unique<egGain::GainTool>(gain_filename1, gain_filename2);
    }
    case egEnergyCorr::es2017:
      return nullptr;
    default:
      return nullptr;
    }
}

std::unique_ptr<egammaMVATool> egammaMVAToolFactory(egEnergyCorr::ESModel model)
{
    std::string folder;
    switch (model)
    {
        case egEnergyCorr::es2011d:
        case egEnergyCorr::es2011dMedium:
        case egEnergyCorr::es2011dTight:
          folder = "egammaMVACalib/v1";
          break;
        case egEnergyCorr::es2012c:
          folder = "egammaMVACalib/v1";
          break;
        case egEnergyCorr::es2012XX:
        case egEnergyCorr::es2015PRE:
        case egEnergyCorr::es2015PRE_res_improved:
        case egEnergyCorr::es2015_day0_3percent:
          folder = "egammaMVACalib/offline/v3";
          break;
        case egEnergyCorr::es2015cPRE:
        case egEnergyCorr::es2015cPRE_res_improved:
          folder = "egammaMVACalib/offline/v3_E4crack_bis";
          break;
        case egEnergyCorr::es2015c_summer:
        case egEnergyCorr::es2016PRE:
        case egEnergyCorr::es2017:
          folder = "egammaMVACalib/offline/v4.0";
          break;
        default: folder = "";
    }

    if (not folder.empty()) {
      auto tool = CxxUtils::make_unique<egammaMVATool>("EgammaMVATool");
      tool->setProperty("folder", folder).ignore();
      return tool;
    }

    else { return nullptr; }
}


std::unique_ptr<egammaLayerRecalibTool> egammaLayerRecalibToolFactory(egEnergyCorr::ESModel model)
{
  std::string tune = "";
  switch (model)
  {
    case egEnergyCorr::es2011d:
    case egEnergyCorr::es2011dMedium:
    case egEnergyCorr::es2011dTight:
      tune = "2011_alt_with_layer2";
      break;
    case egEnergyCorr::es2012c:
    case egEnergyCorr::es2012XX:
    case egEnergyCorr::es2015PRE:
    case egEnergyCorr::es2015cPRE:
    case egEnergyCorr::es2015PRE_res_improved:
    case egEnergyCorr::es2015cPRE_res_improved:
    case egEnergyCorr::es2015_day0_3percent:
    case egEnergyCorr::es2015c_summer:
    case egEnergyCorr::es2016PRE:
    case egEnergyCorr::es2017:
      tune = "2012_alt_with_layer2";
      break;
    default:
      return nullptr;
  }
  return CxxUtils::make_unique<egammaLayerRecalibTool>(tune);
}

bool use_intermodule_correction(egEnergyCorr::ESModel model)
{
  switch (model)
  {
    case egEnergyCorr::es2010:
    case egEnergyCorr::es2011c:
    case egEnergyCorr::es2011d:
    case egEnergyCorr::es2011dMedium:
    case egEnergyCorr::es2011dTight:
    case egEnergyCorr::es2012a:
      return false;
    case egEnergyCorr::es2012c:
    case egEnergyCorr::es2012cMedium:
    case egEnergyCorr::es2012cTight:
    case egEnergyCorr::es2015_day0_3percent:
    case egEnergyCorr::es2012XX:
    case egEnergyCorr::es2015PRE:
    case egEnergyCorr::es2015cPRE:
    case egEnergyCorr::es2015PRE_res_improved:
    case egEnergyCorr::es2015cPRE_res_improved:
    case egEnergyCorr::es2015c_summer:
    case egEnergyCorr::es2016PRE:
    case egEnergyCorr::es2017:
      return true;
    case egEnergyCorr::UNDEFINED:  // TODO: find better logic
      return false;
  }
  assert(false);
  return false;
}

bool use_phi_uniform_correction(egEnergyCorr::ESModel model)
{
  return use_intermodule_correction(model);  // they are equal
}

bool is_run2(egEnergyCorr::ESModel model)
{
  switch (model) {
    case egEnergyCorr::es2010:
    case egEnergyCorr::es2011c:
    case egEnergyCorr::es2011d:
    case egEnergyCorr::es2011dMedium:
    case egEnergyCorr::es2011dTight:
    case egEnergyCorr::es2012a:
    case egEnergyCorr::es2012c:
    case egEnergyCorr::es2012cMedium:
    case egEnergyCorr::es2012cTight:
      return false;
    case egEnergyCorr::es2015_day0_3percent:
    case egEnergyCorr::es2012XX:
    case egEnergyCorr::es2015PRE:
    case egEnergyCorr::es2015cPRE:
    case egEnergyCorr::es2015PRE_res_improved:
    case egEnergyCorr::es2015cPRE_res_improved:
    case egEnergyCorr::es2015c_summer:
    case egEnergyCorr::es2016PRE:
    case egEnergyCorr::es2017:
      return true;
    case egEnergyCorr::UNDEFINED:  // TODO: find better logic
      return false;
    }
    assert(false);
    return false;
}


EgammaCalibrationAndSmearingTool::EgammaCalibrationAndSmearingTool(const std::string& name)
  : asg::AsgMetadataTool(name),
    m_TESModel(egEnergyCorr::UNDEFINED),
    m_TResolutionType(egEnergyCorr::Resolution::SigmaEff90),
    m_currentScaleVariation_MC(egEnergyCorr::Scale::None),
    m_currentScaleVariation_data(egEnergyCorr::Scale::Nominal),
    m_currentResolutionVariation_MC(egEnergyCorr::Resolution::Nominal),
    m_currentResolutionVariation_data(egEnergyCorr::Resolution::None),
    m_set_seed_function([](const EgammaCalibrationAndSmearingTool&,
                           const xAOD::Egamma& egamma,
                           const xAOD::EventInfo& ei) {
			  // avoid 0 as result, see https://root.cern.ch/root/html/TRandom3.html#TRandom3:SetSeed
			  return 1 + static_cast<RandomNumber>(std::abs(egamma.phi()) * 1E6 + std::abs(egamma.eta()) * 1E3 + ei.eventNumber()); })
{
  declareProperty("ESModel", m_ESModel = "");
  declareProperty("decorrelationModel", m_decorrelation_model_name = "");
  declareProperty("decorrelationModelScale", m_decorrelation_model_scale_name = "");
  declareProperty("decorrelationModelResolution", m_decorrelation_model_resolution_name = "");
  declareProperty("ResolutionType", m_ResolutionType = "SigmaEff90");
  declareProperty("varSF", m_varSF = 1.0);
  declareProperty("doScaleCorrection", m_doScaleCorrection = AUTO);
  declareProperty("doSmearing", m_doSmearing = AUTO);
  declareProperty("useLayerCorrection", m_useLayerCorrection = AUTO);
  declareProperty("usePSCorrection", m_usePSCorrection = AUTO);
  declareProperty("useS12Correction", m_useS12Correction = AUTO);
  declareProperty("useLayer2Recalibration", m_useLayer2Recalibration = AUTO);
  declareProperty("useIntermoduleCorrection", m_useIntermoduleCorrection = AUTO);
  declareProperty("usePhiUniformCorrection", m_usePhiUniformCorrection = AUTO);
  declareProperty("useGainCorrection", m_useGainCorrection = AUTO);
  declareProperty("autoReseed", m_auto_reseed = true);
  declareProperty("MVAfolder", m_MVAfolder = "");
  declareProperty("layerRecalibrationTune", m_layer_recalibration_tune = "");
  declareProperty("useEPCombination", m_use_ep_combination = false);
  declareProperty("useMVACalibration", m_use_mva_calibration = AUTO);
  declareProperty("use_full_statistical_error", m_use_full_statistical_error=false);
  declareProperty("use_temp_correction201215", m_use_temp_correction201215=AUTO);
  declareProperty("use_uA2MeV_2015_first2weeks_correction", m_use_uA2MeV_2015_first2weeks_correction=AUTO);
  declareProperty("randomRunNumber", m_user_random_run_number=0);
  // this is the user input, it is never changed by the tool. The tool uses m_simulation.
  declareProperty("useAFII", m_use_AFII = AUTO, "This will be set automatically for you if using athena, (int)0=full sim, (int)1=fast sim");
  m_use_mapping_correction = false;
}

EgammaCalibrationAndSmearingTool::~EgammaCalibrationAndSmearingTool() {
  ATH_MSG_DEBUG("destructor");
  delete m_mva_tool;
  delete m_layer_recalibration_tool;
  delete m_gain_tool;
}

StatusCode EgammaCalibrationAndSmearingTool::initialize() {
  ATH_MSG_INFO("Initialization");

  if (m_ESModel == "es2015XX") { ATH_MSG_ERROR("es2015XX is deprecated. Use es2015PRE"); }

  if (m_ESModel == "es2010") { m_TESModel = egEnergyCorr::es2010; }           // legacy
  else if (m_ESModel == "es2011c") { m_TESModel = egEnergyCorr::es2011c; }    // mc11c : faulty G4; old geometry
  else if (m_ESModel == "es2011d") { m_TESModel = egEnergyCorr::es2011d; }    // mc11d : corrected G4; new geometry == final Run1 scheme
  else if (m_ESModel == "es2012a") { m_TESModel = egEnergyCorr::es2012a; }    // mc12a : "crude" G4 fix; old geometry
  else if (m_ESModel == "es2012c") { m_TESModel = egEnergyCorr::es2012c; }    // mc12c : corrected G4; new geometry == final Run1 scheme
  else if (m_ESModel == "es2012XX") { m_TESModel = egEnergyCorr::es2012XX; }
  else if (m_ESModel == "es2015PRE") { m_TESModel = egEnergyCorr::es2015PRE; }
  else if (m_ESModel == "es2015PRE_res_improved") { m_TESModel = egEnergyCorr::es2015PRE_res_improved; }
  else if (m_ESModel == "es2015cPRE") { m_TESModel = egEnergyCorr::es2015cPRE; }
  else if (m_ESModel == "es2015cPRE_res_improved") { m_TESModel = egEnergyCorr::es2015cPRE_res_improved; }
  else if (m_ESModel == "es2015c_summer") { m_TESModel = egEnergyCorr::es2015c_summer; }
  else if (m_ESModel == "es2016PRE") { m_TESModel = egEnergyCorr::es2016PRE; }
  else if (m_ESModel == "es2016data_mc15c") { m_TESModel = egEnergyCorr::es2017; }
  else if (m_ESModel.empty()) {
    ATH_MSG_ERROR("you must set ESModel property");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_ERROR("Cannot understand model " << m_ESModel);
    return StatusCode::FAILURE;
  }

  if (m_ResolutionType == "Gaussian") { m_TResolutionType = egEnergyCorr::Resolution::Gaussian; }
  else if (m_ResolutionType == "SigmaEff80") { m_TResolutionType = egEnergyCorr::Resolution::SigmaEff80; }
  else if (m_ResolutionType == "SigmaEff90") { m_TResolutionType = egEnergyCorr::Resolution::SigmaEff90; }
  else {
    ATH_MSG_ERROR("Cannot understand resolution " << m_ResolutionType);
    return StatusCode::FAILURE;
  }

  if (m_use_AFII == 1) { m_simulation = PATCore::ParticleDataType::Fast; }
  else if (m_use_AFII == 0) { m_simulation = PATCore::ParticleDataType::Full; }
  // this is needed for tests where only applyCorrection is called without beginInputFile or beginEvent
  else if (m_use_AFII == AUTO) { m_simulation = PATCore::ParticleDataType::Full; }

  // configure decorrelation model, translate string property to internal class enum
  /*    S R SR
    0.  0 0 0     WARNING Full, Full (this is the default without configuration)
    1.  0 0 1     SR
    2.  0 1 0     FATAL
    3.  0 1 1     WARNING SR then R
    4.  1 0 0     FATAL
    5.  1 0 1     WARNING SR then S
    6.  1 1 0     S, R
    7.  1 1 1     FATAL
  */
  if (m_decorrelation_model_name.empty() and
      m_decorrelation_model_scale_name.empty() and
      m_decorrelation_model_resolution_name.empty()) {
    // case 0
    ATH_MSG_WARNING("no decorrelation model specified, assuming full model");
    m_decorrelation_model_scale = ScaleDecorrelation::FULL;
    m_decorrelation_model_resolution = ResolutionDecorrelation::FULL;
    m_decorrelation_model_name = "FULL_v1";
  }
  else if (not m_decorrelation_model_name.empty() and
           not m_decorrelation_model_scale_name.empty() and
           not m_decorrelation_model_resolution_name.empty()) {
    // case 7
    ATH_MSG_FATAL("too many flags for the decorrelation model");
    return StatusCode::FAILURE;
  }
  else {
    // set scale decorrelation model
    if (not m_decorrelation_model_scale_name.empty()) {  // case 4, 5, 6, (7)
      if (not m_decorrelation_model_name.empty()) { ATH_MSG_WARNING("flag decorrelation model ignored for scale decorrelation model"); } // case 5
      if (m_decorrelation_model_scale_name == "1NP_v1") m_decorrelation_model_scale = ScaleDecorrelation::ONENP;
      else if (m_decorrelation_model_scale_name == "FULL_ETACORRELATED_v1") m_decorrelation_model_scale = ScaleDecorrelation::FULL_ETA_CORRELATED;
      else if (m_decorrelation_model_scale_name == "1NPCOR_PLUS_UNCOR") m_decorrelation_model_scale = ScaleDecorrelation::ONENP_PLUS_UNCONR;
      else if (m_decorrelation_model_scale_name == "FULL_v1") m_decorrelation_model_scale = ScaleDecorrelation::FULL;
      else {
        ATH_MSG_FATAL("cannot understand the scale decorrelation model '" << m_decorrelation_model_scale_name << "'(typo?)");
        return StatusCode::FAILURE;
       }
    }
    else if (not m_decorrelation_model_name.empty()) { // case 1, 3
      if (m_decorrelation_model_name == "1NP_v1") m_decorrelation_model_scale = ScaleDecorrelation::ONENP;
      else if (m_decorrelation_model_name == "FULL_ETACORRELATED_v1") m_decorrelation_model_scale = ScaleDecorrelation::FULL_ETA_CORRELATED;
      else if (m_decorrelation_model_name == "1NPCOR_PLUS_UNCOR") m_decorrelation_model_scale = ScaleDecorrelation::ONENP_PLUS_UNCONR;
      else if (m_decorrelation_model_name == "FULL_v1") m_decorrelation_model_scale = ScaleDecorrelation::FULL;
      else {
        ATH_MSG_FATAL("cannot understand the decorrelation model '" << m_decorrelation_model_name << "'(typo?)");
        return StatusCode::FAILURE;
       }
    }
    else {  // case 2, (7)
      ATH_MSG_FATAL("not information how to initialize the scale decorrelation model");
      return StatusCode::FAILURE;
    }

    // set resolution decorralation model
    if (not m_decorrelation_model_resolution_name.empty()) { // case 2, 3, 6, (7)
      if (not m_decorrelation_model_name.empty()) { ATH_MSG_WARNING("flag decorrelation model ignored for resolution decorrelation model"); } // case 3
      if (m_decorrelation_model_resolution_name == "1NP_v1") m_decorrelation_model_resolution = ResolutionDecorrelation::ONENP;
      else if (m_decorrelation_model_resolution_name == "FULL_v1") m_decorrelation_model_resolution = ResolutionDecorrelation::FULL;
      else {
        ATH_MSG_FATAL("cannot understand the resolution decorrelation model '" << m_decorrelation_model_resolution_name << "'(typo?)");
        return StatusCode::FAILURE;
       }
    }
    else if (not m_decorrelation_model_name.empty()) { // case 1, 5
      if (m_decorrelation_model_name == "1NP_v1") m_decorrelation_model_resolution = ResolutionDecorrelation::ONENP;
      else if (m_decorrelation_model_name == "FULL_ETACORRELATED_v1") m_decorrelation_model_resolution = ResolutionDecorrelation::FULL;
      else if (m_decorrelation_model_name == "1NPCOR_PLUS_UNCOR") m_decorrelation_model_resolution = ResolutionDecorrelation::ONENP;
      else if (m_decorrelation_model_name == "FULL_v1") m_decorrelation_model_resolution = ResolutionDecorrelation::FULL;
      else {
        ATH_MSG_FATAL("cannot understand the decorrelation model '" << m_decorrelation_model_name << "'(typo?)");
        return StatusCode::FAILURE;
       }
    }
  }


  // create correction tool
  ATH_MSG_DEBUG("creating internal correction tool");
  m_rootTool.reset(new AtlasRoot::egammaEnergyCorrectionTool());
  if (!m_rootTool) {
    ATH_MSG_ERROR("Cannot initialize underlying tool");
    return StatusCode::FAILURE;
  }
  m_rootTool->setESModel(m_TESModel);

  m_rootTool->msg().setLevel(this->msg().level());
  m_rootTool->initialize();


  // configure MVA tool
  if (m_use_mva_calibration != 0)
  {
    ATH_MSG_DEBUG("creating MVA calibration tool (if needed)");
    if (m_MVAfolder == "")  {  // automatically configure MVA tool
      m_mva_tool = egammaMVAToolFactory(m_TESModel).release();
      if (!m_mva_tool) { ATH_MSG_INFO("not using MVA calibration"); }
    }
    else {
      m_mva_tool = new egammaMVATool("egammaMVATool");
      ATH_CHECK(m_mva_tool->setProperty("folder", m_MVAfolder));
    }
    if (m_mva_tool) {
      m_mva_tool->msg().setLevel(this->msg().level());
      ATH_CHECK(m_mva_tool->initialize());
    }
  }

  // configure layer recalibration tool
  ATH_MSG_DEBUG("initializing layer recalibration tool (if needed)");
  if (m_layer_recalibration_tune == "") { // automatically configure layer recalibration tool
    m_layer_recalibration_tool = egammaLayerRecalibToolFactory(m_TESModel).release();
    if (!m_layer_recalibration_tool) { ATH_MSG_INFO("not using layer recalibration"); }
  }
  else {
    m_layer_recalibration_tool = new egammaLayerRecalibTool(m_layer_recalibration_tune);
  }
  if (m_layer_recalibration_tool) { m_layer_recalibration_tool->msg().setLevel(this->msg().level()); }

  if (m_use_temp_correction201215 != AUTO) m_rootTool->use_temp_correction201215(m_use_temp_correction201215);
  if (m_use_uA2MeV_2015_first2weeks_correction != AUTO) m_rootTool->use_uA2MeV_2015_first2weeks_correction(m_use_uA2MeV_2015_first2weeks_correction);
  if (not m_use_full_statistical_error and m_decorrelation_model_scale == ScaleDecorrelation::FULL) { m_rootTool->useStatErrorScaling(true); }

  if (m_use_ep_combination) {
    ATH_MSG_ERROR("ep combination not supported yet");
    throw std::runtime_error("ep combination not supported yet");
  }

  if (m_useIntermoduleCorrection == AUTO) { m_useIntermoduleCorrection = use_intermodule_correction(m_TESModel); }
  if (m_usePhiUniformCorrection == AUTO) { m_usePhiUniformCorrection = use_phi_uniform_correction(m_TESModel); }
  m_use_mapping_correction = not is_run2(m_TESModel);
  if (m_useGainCorrection == AUTO) {
    ATH_MSG_DEBUG("initializing gain tool");
    m_gain_tool = gainToolFactory(m_TESModel).release();
    m_useGainCorrection = bool(m_gain_tool);
  }
  else if (m_useGainCorrection == 1) {
    m_useGainCorrection = 0;
    ATH_MSG_ERROR("cannot instantiate gain tool for this model (you can only disable the gain tool, but not enable it)");
  }


  ATH_MSG_INFO("ESModel: " << m_ESModel);
  ATH_MSG_INFO("ResolutionType: " << m_ResolutionType);
  ATH_MSG_INFO("layer correction = " << m_useLayerCorrection);
  ATH_MSG_INFO("PS correction = " << m_usePSCorrection);
  ATH_MSG_INFO("S12 correction = " << m_useS12Correction);
  ATH_MSG_INFO("layer2 recalibration = " << m_useLayer2Recalibration);
  ATH_MSG_INFO("intermodule correction = " << m_useIntermoduleCorrection);
  ATH_MSG_INFO("phi uniformity correction = " << m_usePhiUniformCorrection);
  ATH_MSG_INFO("gain correction = " << m_useGainCorrection);
  ATH_MSG_INFO("smearing = " << m_doSmearing);
  ATH_MSG_INFO("insitu scales = " << m_doScaleCorrection);
  ATH_MSG_INFO("ep combination = " << m_use_ep_combination);
  ATH_MSG_INFO("use MVA calibration = " << m_use_mva_calibration);
  ATH_MSG_INFO("use temperature correction 2015 = " << m_use_temp_correction201215);
  ATH_MSG_INFO("use uA2MeV correction 2015 1/2 week = " << m_use_uA2MeV_2015_first2weeks_correction);
  //ATH_MSG_INFO("use AFII = " << m_use_AFII); ... print in beginInputFile now, since that's where we check it's value

  setupSystematics();

  applySystematicVariation(CP::SystematicSet()).ignore();   // this set the flags for the internal tool without systematics
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if ( registry.registerSystematics( *this ) != CP::SystematicCode::Ok ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}


StatusCode EgammaCalibrationAndSmearingTool::get_simflavour_from_metadata(PATCore::ParticleDataType::DataType& result) const
{
  // adapted from https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/AnalysisCommon/CPAnalysisExamples/trunk/Root/MetadataToolExample.cxx
#ifndef ROOTCORE
  std::string dataType("");
  ATH_CHECK(AthAnalysisHelper::retrieveMetadata("/TagInfo", "project_name", dataType, inputMetaStore()));
  // TODO: evaluate to set isMC as a state of the tool
  if (not(dataType == "IS_SIMULATION")) {
    result = PATCore::ParticleDataType::Data;
    return StatusCode::SUCCESS;
  }

  // Determine Fast/FullSim
  std::string simType("");
  ATH_CHECK(AthAnalysisHelper::retrieveMetadata("/Simulation/Parameters", "SimulationFlavour", simType, inputMetaStore()));
  boost::to_upper(simType);
  result = (simType.find("ATLFASTII")==std::string::npos) ?  PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Fast;
  return StatusCode::SUCCESS;
#else
  //here's how things will work dual use, when file metadata is available in files
  if (inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
    const xAOD::FileMetaData* fmd = 0;
    ATH_CHECK(inputMetaStore()->retrieve(fmd, "FileMetaData"));

    std::string simType("");
    const bool s = fmd->value(xAOD::FileMetaData::simFlavour, simType);
    if (!s) {
      ATH_MSG_DEBUG("no sim flavour from metadata: must be data");
      result = PATCore::ParticleDataType::Data;
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_DEBUG("sim type = " + simType);
      result = simType == "FullSim" ? PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Fast;
      return StatusCode::SUCCESS;
    }
  }
  else {  // no metadata in the file
    ATH_MSG_DEBUG("no metadata found in the file");
    return StatusCode::FAILURE;
  }
#endif
}

StatusCode EgammaCalibrationAndSmearingTool::beginInputFile()
{
  // if the user has set a preference (m_use_AFII != AUTO) set it
  if (m_use_AFII == 0) { m_simulation = PATCore::ParticleDataType::Full; }
  else if (m_use_AFII == 1) { m_simulation = PATCore::ParticleDataType::Fast; }

  PATCore::ParticleDataType::DataType data_flavour_metadata;
  const StatusCode status_metadata = get_simflavour_from_metadata(data_flavour_metadata);
  if (status_metadata == StatusCode::SUCCESS) {
    m_metadata_retrieved = true;
    ATH_MSG_DEBUG("metadata from new file: " << (data_flavour_metadata == PATCore::ParticleDataType::Data ? "data" : (data_flavour_metadata == PATCore::ParticleDataType::Full ? "full simulation" : "fast simulation")));

    if (data_flavour_metadata != PATCore::ParticleDataType::Data) {
      if (m_use_AFII == AUTO) { m_simulation = data_flavour_metadata; }
      else { // user set a preference
        // check if the preference is consistent and warning
        if (m_use_AFII == 1 and data_flavour_metadata == PATCore::ParticleDataType::Full) {
          ATH_MSG_WARNING("data is full sim, but you asked for AFII");
        }
        else if (m_use_AFII == 0 and data_flavour_metadata == PATCore::ParticleDataType::Fast) {
          ATH_MSG_WARNING("data is fast sim, but you asked for full sim");
        }
      }
    }
  }
  else { // not able to retrieve metadata
    m_metadata_retrieved = false;

    if (m_use_AFII == AUTO) {
      ATH_MSG_WARNING("not able to retrieve metadata and use_AFII not specified -> set simulation flavour to full simulation");
      // do not error since it can be real data, but we don't know, need to check later (beginEvent)
    }
  }

//  return status_metadata;  // since several times it is not possible to retrieve metadata
  return StatusCode::SUCCESS;
}

StatusCode EgammaCalibrationAndSmearingTool::endInputFile() {
  m_metadata_retrieved = false;
  return StatusCode::SUCCESS;
}

StatusCode EgammaCalibrationAndSmearingTool::beginEvent() {
  if (m_metadata_retrieved) return StatusCode::SUCCESS;

  //determine MC/Data from evtInfo ... this will work for both athena and eventloop
  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK(evtStore()->retrieve(evtInfo, "EventInfo"));
  if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
    // redundant, already done in beginInputFile
    if (m_use_AFII == 1) { m_simulation = PATCore::ParticleDataType::Fast; }
    else if (m_use_AFII == 0) { m_simulation = PATCore::ParticleDataType::Full; }
    else { // AUTO
      // do not warning since it is annoying for every event, the warning is in beginInputFile
      m_simulation = PATCore::ParticleDataType::Full;
    }
  }
  return StatusCode::SUCCESS;
}


void EgammaCalibrationAndSmearingTool::setRandomSeed(unsigned seed) {
  m_rootTool->setRandomSeed(seed);
}

PATCore::ParticleType::Type EgammaCalibrationAndSmearingTool::xAOD2ptype(const xAOD::Egamma& particle) const
{
  auto ptype = PATCore::ParticleType::Electron;
  //no ForwardElectron ptype: consider them as Electron
  if (xAOD::EgammaHelpers::isElectron(&particle) || particle.author() == xAOD::EgammaParameters::AuthorFwdElectron) { ptype = PATCore::ParticleType::Electron; }
  else if (xAOD::EgammaHelpers::isPhoton(&particle)) {
    if (xAOD::EgammaHelpers::isConvertedPhoton(&particle)) { ptype = PATCore::ParticleType::ConvertedPhoton; }
    else { ptype = PATCore::ParticleType::UnconvertedPhoton; }
  }
  else {
    ATH_MSG_ERROR("particle is not electron of photon");
    throw std::runtime_error("particle is not electron or photon");
  }
  return ptype;
}

double EgammaCalibrationAndSmearingTool::getResolution(const xAOD::Egamma& particle, bool withCT) const
{
  const auto ptype = xAOD2ptype(particle);
  const auto cl_etaCalo = xAOD::get_eta_calo(*particle.caloCluster(), particle.author());

  return m_rootTool->resolution(particle.e(), particle.caloCluster()->eta(),
                                cl_etaCalo, ptype,
                                withCT, false); // TODO: always for full simulation
}

double
EgammaCalibrationAndSmearingTool::resolution(double energy, double cl_eta, double cl_etaCalo,
                                             PATCore::ParticleType::Type ptype, bool withCT) const
{
  return m_rootTool->resolution(energy, cl_eta, cl_etaCalo, ptype, withCT, false);
}


CP::CorrectionCode EgammaCalibrationAndSmearingTool::applyCorrection(xAOD::Egamma & input)
{
  // Retrieve the event information:
  const xAOD::EventInfo* event_info = 0;
  if (evtStore()->retrieve(event_info, "EventInfo").isFailure()) {
    ATH_MSG_ERROR("No EventInfo object could be retrieved");
    return CP::CorrectionCode::Error;
  }
  return applyCorrection(input, *event_info);
}


CP::CorrectionCode EgammaCalibrationAndSmearingTool::correctedCopy(const xAOD::Electron& input, xAOD::Electron*& output)
{
  // A sanity check:
  if (output) ATH_MSG_WARNING( "Non-null pointer received. " "There's a possible memory leak!" );

  output = new xAOD::Electron();
  output->makePrivateStore(input);
  return applyCorrection(*output);
}

CP::CorrectionCode EgammaCalibrationAndSmearingTool::correctedCopy(const xAOD::Photon& input, xAOD::Photon*& output)
{
  // A sanity check:
  if (output) ATH_MSG_WARNING( "Non-null pointer received. " "There's a possible memory leak!" );

  output = new xAOD::Photon();
  output->makePrivateStore(input);
  return applyCorrection(*output);
}

double EgammaCalibrationAndSmearingTool::getEnergy(const xAOD::Photon& input)
{
  xAOD::Photon* new_particle = nullptr;
  correctedCopy(input, new_particle);
  const double e = new_particle->e();
  delete new_particle;
  return e;
}

double EgammaCalibrationAndSmearingTool::getEnergy(const xAOD::Electron& input)
{
  xAOD::Electron* new_particle = nullptr;
  correctedCopy(input, new_particle);
  const double e = new_particle->e();
  delete new_particle;
  return e;
}


CP::CorrectionCode EgammaCalibrationAndSmearingTool::applyCorrection(xAOD::Egamma & input, const xAOD::EventInfo& event_info)
{
  PATCore::ParticleDataType::DataType dataType = (event_info.eventType(xAOD::EventInfo::IS_SIMULATION)) ? m_simulation : PATCore::ParticleDataType::Data;

  if (event_info.eventType(xAOD::EventInfo::IS_SIMULATION) and m_auto_reseed) {
    setRandomSeed(m_set_seed_function(*this, input, event_info));
  }

  if (dataType == PATCore::ParticleDataType::Data and m_layer_recalibration_tool) {
    // if data apply energy recalibration
    ATH_MSG_DEBUG("applying energy recalibration before E0|E1|E2|E3 = "
                  << input.caloCluster()->energyBE(0) << "|"
                  << input.caloCluster()->energyBE(1) << "|"
                  << input.caloCluster()->energyBE(2) << "|"
                  << input.caloCluster()->energyBE(3));
    const CP::CorrectionCode status_layer_recalibration = m_layer_recalibration_tool->applyCorrection(input, event_info);
    if (status_layer_recalibration == CP::CorrectionCode::Error) { return CP::CorrectionCode::Error; }
    ATH_MSG_DEBUG("eta|phi = " << input.eta() << "|" << input.phi());
    if (status_layer_recalibration == CP::CorrectionCode::Ok) {
      ATH_MSG_DEBUG("decoration E0|E1|E2|E3 = "
                    << input.caloCluster()->auxdataConst<double>("correctedcl_Es0") << "|"
                    << input.caloCluster()->auxdataConst<double>("correctedcl_Es1") << "|"
                    << input.caloCluster()->auxdataConst<double>("correctedcl_Es2") << "|"
                    << input.caloCluster()->auxdataConst<double>("correctedcl_Es3") << "|");
      if (input.caloCluster()->auxdataConst<double>("correctedcl_Es2") == 0 and input.caloCluster()->auxdataConst<double>("correctedcl_Es1") == 0 and
          input.caloCluster()->auxdataConst<double>("correctedcl_Es3") == 0 and input.caloCluster()->auxdataConst<double>("correctedcl_Es0") == 0 and
          (std::abs(input.eta()) < 1.37 or (std::abs(input.eta()) > 1.55 and std::abs(input.eta()) < 2.47)))
      {
        ATH_MSG_WARNING("all layer energies are zero");
      }
    }
  }

  double energy = 0.;
  // apply MVA calibration
  if (m_mva_tool) {
    if (input.author() != xAOD::EgammaParameters::AuthorFwdElectron) {    // do not apply MVA calibration to fwd electrons
      energy = m_mva_tool->getEnergy(input.caloCluster(), &input);
    }
    else { energy = input.e(); }
    ATH_MSG_DEBUG("energy after MVA calibration = " << boost::format("%.2f") % energy);
  }
  else { energy = input.e(); }

  if ( m_TESModel == egEnergyCorr::es2011c ) {
    // Crack calibation correction for es2011c (calibration hits calibration)
    const auto ptype = xAOD2ptype(input);
    const double etaden = ptype == PATCore::ParticleType::Electron ? static_cast<xAOD::Electron&>(input).trackParticle()->eta() : input.caloCluster()->eta();
    energy *= m_rootTool->applyMCCalibration( input.caloCluster()->eta(), energy / cosh(etaden), ptype);
    ATH_MSG_DEBUG("energy after crack calibration es2011c = " << boost::format("%.2f") % energy);
  }

  // apply uniformity correction
  if (dataType == PATCore::ParticleDataType::Data and m_useIntermoduleCorrection) {
    energy = intermodule_correction(energy, input.caloCluster()->phi(), input.caloCluster()->eta());
    ATH_MSG_DEBUG("energy after intermodule correction = " << boost::format("%.2f") % energy);
  }

  if (dataType == PATCore::ParticleDataType::Data and m_usePhiUniformCorrection) {
    energy *= correction_phi_unif(xAOD::get_eta_calo(*input.caloCluster(), input.author(), false),
                                  xAOD::get_phi_calo(*input.caloCluster(), input.author(), false));
    ATH_MSG_DEBUG("energy after uniformity correction = " << boost::format("%.2f") % energy);
  }

  // apply gain correction
  if (dataType == PATCore::ParticleDataType::Data and m_gain_tool)
  {
    const auto cl_eta = input.caloCluster()->eta();
    const auto es2 = input.caloCluster()->isAvailable<double>("correctedcl_Es2") ? input.caloCluster()->auxdataConst<double>("correctedcl_Es2") : input.caloCluster()->energyBE(2);
    if (!(std::abs(cl_eta) < 1.52 and std::abs(cl_eta) > 1.37) and std::abs(cl_eta) < 2.4)
    energy = m_gain_tool->CorrectionGainTool(cl_eta, energy / GeV, es2 / GeV, xAOD2ptype(input)); // cl_eta ok, TODO: check corrected E2
    ATH_MSG_DEBUG("energy after gain correction = " << boost::format("%.2f") % energy);
  }

  const double eraw = ((input.caloCluster()->isAvailable<double>("correctedcl_Es0") ? input.caloCluster()->auxdataConst<double>("correctedcl_Es0") : input.caloCluster()->energyBE(0)) +
                       (input.caloCluster()->isAvailable<double>("correctedcl_Es1") ? input.caloCluster()->auxdataConst<double>("correctedcl_Es1") : input.caloCluster()->energyBE(1)) +
                       (input.caloCluster()->isAvailable<double>("correctedcl_Es2") ? input.caloCluster()->auxdataConst<double>("correctedcl_Es2") : input.caloCluster()->energyBE(2)) +
                       (input.caloCluster()->isAvailable<double>("correctedcl_Es3") ? input.caloCluster()->auxdataConst<double>("correctedcl_Es3") : input.caloCluster()->energyBE(3)));


  unsigned int runNumber_for_tool = 0;
  if (dataType == PATCore::ParticleDataType::Data) runNumber_for_tool = event_info.runNumber();
  else {
    if (m_user_random_run_number == 0) {
      static const SG::AuxElement::Accessor<unsigned int> randomrunnumber_getter("RandomRunNumber");
      if (randomrunnumber_getter.isAvailable(event_info)) { runNumber_for_tool = randomrunnumber_getter(event_info); }
      else {
	ATH_MSG_ERROR("Pileup tool not run before using ElectronPhotonFourMomentumCorrection! Assuming it is 2016. If you "
		      "want to force a specific period set the property randomRunNumber of the tool, e.g. in the job option: "
		      "tool.randomRunNumber = 123456 or "
		      "tool.randomRunNumber = EgammaCalibrationAndSmearingToolRunNumbersExample.run_2016");
	runNumber_for_tool = EgammaCalibPeriodRunNumbersExample::run_2016;
      }
    }
    else {
      runNumber_for_tool = m_user_random_run_number;
    }
  }

  if (dataType == PATCore::ParticleDataType::Fast) ATH_MSG_DEBUG("is fast");
  else if (dataType == PATCore::ParticleDataType::Full) ATH_MSG_DEBUG("is full");
  else if (dataType == PATCore::ParticleDataType::Data) ATH_MSG_DEBUG("is data");

  // apply scale factors or systematics
  energy = m_rootTool->getCorrectedEnergy(
             runNumber_for_tool,
             dataType,
             xAOD2ptype(input),
             input.caloCluster()->eta(),
             xAOD::get_eta_calo(*input.caloCluster(), input.author(), false),
             energy,
             input.caloCluster()->isAvailable<double>("correctedcl_Es2") ? input.caloCluster()->auxdataConst<double>("correctedcl_Es2") : input.caloCluster()->energyBE(2),
             eraw,
             oldtool_scale_flag_this_event(input, event_info),
             oldtool_resolution_flag_this_event(input, event_info),
             m_TResolutionType,
	           m_varSF);

  ATH_MSG_DEBUG("energy after scale/systematic correction = " << boost::format("%.2f") % energy);

  // TODO: this check should be done before systematics variations
  const double new_energy2 = energy * energy;
  const double m2 = input.m() * input.m();
  const double p2 = new_energy2 > m2 ? new_energy2 - m2 : 0.;
  input.setPt(sqrt(p2) / cosh(input.eta()));
  ATH_MSG_DEBUG("after setting pt, energy = " << input.e());
  return CP::CorrectionCode::Ok;
}


double EgammaCalibrationAndSmearingTool::getEnergy(xAOD::Egamma* p, const xAOD::EventInfo* event_info)
{
  applyCorrection(*p, *event_info);
  ATH_MSG_DEBUG("returning " << p->e());
  return p->e();
}


egEnergyCorr::Scale::Variation EgammaCalibrationAndSmearingTool::oldtool_scale_flag_this_event(const xAOD::Egamma& p, const xAOD::EventInfo& event_info) const
{
  if (!event_info.eventType(xAOD::EventInfo::IS_SIMULATION)) return m_currentScaleVariation_data;
  if (m_currentScalePredicate(p)) return m_currentScaleVariation_MC;
  else return egEnergyCorr::Scale::None;
}

egEnergyCorr::Resolution::Variation EgammaCalibrationAndSmearingTool::oldtool_resolution_flag_this_event(const xAOD::Egamma&, const xAOD::EventInfo& event_info) const
{
  return event_info.eventType(xAOD::EventInfo::IS_SIMULATION) ? m_currentResolutionVariation_MC : m_currentResolutionVariation_data;
}

double EgammaCalibrationAndSmearingTool::getElectronMomentum(const xAOD::Electron *el, const xAOD::EventInfo* event_info)
{
  PATCore::ParticleDataType::DataType dataType = (event_info->eventType(xAOD::EventInfo::IS_SIMULATION)) ? m_simulation : PATCore::ParticleDataType::Data;

  const xAOD::TrackParticle* eTrack = el->trackParticle();

  // track momentum and eta
  const float el_tracketa = eTrack->eta();
  const float el_trackmomentum = eTrack->pt() * cosh(el->eta());

  return m_rootTool->getCorrectedMomentum(dataType,
					  PATCore::ParticleType::Electron,
					  el_trackmomentum,
					  el_tracketa,
					  oldtool_scale_flag_this_event(*el, *event_info),
					  m_varSF);
}

bool EgammaCalibrationAndSmearingTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const {
  CP::SystematicSet sys = affectingSystematics();
  return sys.find( systematic ) != sys.end();
}

CP::SystematicSet EgammaCalibrationAndSmearingTool::affectingSystematics() const {
  CP::SystematicSet affecting_systematics;
  for (const auto it : m_syst_description) { affecting_systematics.insert(it.first); }
  for (const auto it : m_syst_description_resolution) { affecting_systematics.insert(it.first); }

  return affecting_systematics;
}

void EgammaCalibrationAndSmearingTool::setupSystematics() {
  const EgammaPredicate always = [](const xAOD::Egamma&) { return true; };

  if (m_decorrelation_model_scale == ScaleDecorrelation::ONENP) {
    // TODO: independet implementation of ALL UP looping on all the variations
    m_syst_description[CP::SystematicVariation("EG_SCALE_ALL", +1)] = SysInfo{always, egEnergyCorr::Scale::AllUp};
    m_syst_description[CP::SystematicVariation("EG_SCALE_ALL", -1)] = SysInfo{always, egEnergyCorr::Scale::AllDown};
  }
  else if (m_decorrelation_model_scale == ScaleDecorrelation::FULL_ETA_CORRELATED) {
    // all the physical effects separately, considered as fully correlated in eta

    // common systematics for all the esmodels
    #define SYSMACRO(name, fullcorrelated, decorrelation, flagup, flagdown)                \
      m_syst_description[CP::SystematicVariation(#name, +1)] = SysInfo{always, flagup};    \
      m_syst_description[CP::SystematicVariation(#name, -1)] = SysInfo{always, flagdown};
    #include "ElectronPhotonFourMomentumCorrection/systematics.def"
    #undef SYSMACRO

    // Zee stat is not included in the macro list, add by hand
    m_syst_description[CP::SystematicVariation("EG_SCALE_ZEESTAT", +1)] = SysInfo{always, egEnergyCorr::Scale::ZeeStatUp};
    m_syst_description[CP::SystematicVariation("EG_SCALE_ZEESTAT", -1)] = SysInfo{always, egEnergyCorr::Scale::ZeeStatDown};

    // additional systematics for S12 run2
    if (m_TESModel == egEnergyCorr::es2015PRE_res_improved or m_TESModel == egEnergyCorr::es2015PRE or
        m_TESModel == egEnergyCorr::es2015cPRE or m_TESModel == egEnergyCorr::es2015c_summer or
        m_TESModel == egEnergyCorr::es2016PRE or m_TESModel == egEnergyCorr::es2017) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARCALIB_EXTRA2015PRE", +1)] = SysInfo{always, egEnergyCorr::Scale::LArCalibExtra2015PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARCALIB_EXTRA2015PRE", -1)] = SysInfo{always, egEnergyCorr::Scale::LArCalibExtra2015PreDown};
    }

    // additional systematics for temperature run1->run2
    if (m_TESModel == egEnergyCorr::es2015PRE_res_improved or m_TESModel == egEnergyCorr::es2015PRE or
        m_TESModel == egEnergyCorr::es2015cPRE or m_TESModel == egEnergyCorr::es2015c_summer or
        m_TESModel == egEnergyCorr::es2016PRE) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2015PRE", +1)] = SysInfo{always, egEnergyCorr::Scale::LArTemperature2015PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2015PRE", -1)] = SysInfo{always, egEnergyCorr::Scale::LArTemperature2015PreDown};
    }

    // additional systematic for S12 last eta bin run2
    if (m_TESModel == egEnergyCorr::es2017) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_S12EXTRALASTETABINRUN2", +1)] = SysInfo{always, egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Up};
      m_syst_description[CP::SystematicVariation("EG_SCALE_S12EXTRALASTETABINRUN2", -1)] = SysInfo{always, egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Down};
    }

    // additional systematic for PP0 region
    if (m_TESModel == egEnergyCorr::es2017) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_MATPP0", +1)] = SysInfo{always, egEnergyCorr::Scale::MatPP0Up};
      m_syst_description[CP::SystematicVariation("EG_SCALE_MATPP0", -1)] = SysInfo{always, egEnergyCorr::Scale::MatPP0Down};
    }

    // systematic related to wtots1
    if (m_TESModel == egEnergyCorr::es2017) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_WTOTS1", +1)] = SysInfo{always, egEnergyCorr::Scale::Wtots1Up};
      m_syst_description[CP::SystematicVariation("EG_SCALE_WTOTS1", -1)] = SysInfo{always, egEnergyCorr::Scale::Wtots1Down};
    }

    // systematic for the scintillators
    if (m_TESModel == egEnergyCorr::es2015cPRE or m_TESModel == egEnergyCorr::es2015c_summer or m_TESModel == egEnergyCorr::es2016PRE or m_TESModel == egEnergyCorr::es2017) {
      // scintillator systematics
      m_syst_description[CP::SystematicVariation("EG_SCALE_E4SCINTILLATOR", +1)] = SysInfo{always, egEnergyCorr::Scale::E4ScintillatorUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_E4SCINTILLATOR", -1)] = SysInfo{always, egEnergyCorr::Scale::E4ScintillatorDown};
    }

    // additional systematic for temperature 2015->2016
    if (m_TESModel == egEnergyCorr::es2016PRE) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2016PRE", +1)] = SysInfo{always, egEnergyCorr::Scale::LArTemperature2016PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2016PRE", -1)] = SysInfo{always, egEnergyCorr::Scale::LArTemperature2016PreDown};
    }
  }
  else if (m_decorrelation_model_scale == ScaleDecorrelation::ONENP_PLUS_UNCONR) {
      // qsum of all variations correlated 8/13 TeV + uncorrelated (additional systematics for 2015PRE or 2016)
      // all the physical effects separately, considered as fully correlated in eta
      // TODO: fix for es2017
      #define SYSMACRO(name, fullcorrelated, decorrelation, flagup, flagdown)                \
        m_syst_description[CP::SystematicVariation(#name, +1)] = SysInfo{always, flagup};    \
        m_syst_description[CP::SystematicVariation(#name, -1)] = SysInfo{always, flagdown};
      #include "ElectronPhotonFourMomentumCorrection/systematics_1NPCOR_PLUS_UNCOR.def"
      #undef SYSMACRO
  }
  else if (m_decorrelation_model_scale == ScaleDecorrelation::FULL) {
    typedef std::vector<std::pair<double, double>> pairvector;
    const pairvector decorrelation_bins_BE = {{0., 1.45}, {1.52, 2.5}};
    const std::vector<double> decorrelation_edges_TWELVE = {0., 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4};
    const std::vector<double> decorrelation_edges_MODULE = {0., 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.37, 1.52, 1.8};
    const std::vector<double> decorrelation_edges_MATERIAL = {0.0, 1.1, 1.5, 2.1, 2.5};

    #define SYSMACRO(name, fullcorrelated, decorrelation, flagup, flagdown)                \
    if (bool(fullcorrelated)) {                                                            \
      m_syst_description[CP::SystematicVariation(#name, +1)] = SysInfo{always, flagup};    \
      m_syst_description[CP::SystematicVariation(#name, -1)] = SysInfo{always, flagdown};  \
    }                                                                                      \
    else {                                                                                 \
      int i = 0;                                                                           \
      for (const auto& p : AbsEtaCaloPredicatesFactory(decorrelation)) {                       \
        m_syst_description[CP::SystematicVariation(#name "__ETABIN" + std::to_string(i), +1)] = SysInfo{p, flagup};    \
        m_syst_description[CP::SystematicVariation(#name "__ETABIN" + std::to_string(i), -1)] = SysInfo{p, flagdown};  \
        i += 1;                                                                            \
      }                                                                                    \
    }
    #include "ElectronPhotonFourMomentumCorrection/systematics.def"
    #undef SYSMACRO

    if (m_use_full_statistical_error) {
      // statistical error, decorrelate in *all* the bins
      int i = 0;
      const TAxis& axis_statistical_error(m_rootTool->get_ZeeStat_eta_axis());
      for (int ibin = 1; ibin <= axis_statistical_error.GetNbins(); ++ibin) {
        auto p = AbsEtaCaloPredicateFactory(axis_statistical_error.GetBinLowEdge(ibin),
        axis_statistical_error.GetBinLowEdge(ibin + 1));
        m_syst_description[CP::SystematicVariation("EG_SCALE_ZEESTAT__ETABIN" + std::to_string(i), +1)] = SysInfo{p, egEnergyCorr::Scale::ZeeStatUp};
        m_syst_description[CP::SystematicVariation("EG_SCALE_ZEESTAT__ETABIN" + std::to_string(i), -1)] = SysInfo{p, egEnergyCorr::Scale::ZeeStatDown};
        ++i;
      }
    }
    else {
      // return 1 variation only, fully correlated in eta, equal to the correct value
      // but scaled by sqrt(number of bins)
      // the scaling is done by the old tool
      m_syst_description[CP::SystematicVariation("EG_SCALE_ZEESTAT", +1)] = SysInfo{always, egEnergyCorr::Scale::ZeeStatUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_ZEESTAT", -1)] = SysInfo{always, egEnergyCorr::Scale::ZeeStatDown};
    }

    // additional systematics for S12 run2
    if (m_TESModel == egEnergyCorr::es2015PRE_res_improved or m_TESModel == egEnergyCorr::es2015PRE or
        m_TESModel == egEnergyCorr::es2015cPRE or m_TESModel == egEnergyCorr::es2015c_summer or
        m_TESModel == egEnergyCorr::es2016PRE or m_TESModel == egEnergyCorr::es2017) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARCALIB_EXTRA2015PRE__ETABIN0", +1)] = SysInfo{AbsEtaCaloPredicateFactory({0, 1.45}), egEnergyCorr::Scale::LArCalibExtra2015PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARCALIB_EXTRA2015PRE__ETABIN0", -1)] = SysInfo{AbsEtaCaloPredicateFactory({0, 1.45}), egEnergyCorr::Scale::LArCalibExtra2015PreDown};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARCALIB_EXTRA2015PRE__ETABIN1", +1)] = SysInfo{AbsEtaCaloPredicateFactory({1.45, 2.47}), egEnergyCorr::Scale::LArCalibExtra2015PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARCALIB_EXTRA2015PRE__ETABIN1", -1)] = SysInfo{AbsEtaCaloPredicateFactory({1.45, 2.47}), egEnergyCorr::Scale::LArCalibExtra2015PreDown};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARCALIB_EXTRA2015PRE__ETABIN2", +1)] = SysInfo{AbsEtaCaloPredicateFactory({2.47, 3.2}), egEnergyCorr::Scale::LArCalibExtra2015PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARCALIB_EXTRA2015PRE__ETABIN2", -1)] = SysInfo{AbsEtaCaloPredicateFactory({2.47, 3.2}), egEnergyCorr::Scale::LArCalibExtra2015PreDown};
    }

    // additional systematics for temperature run1->run2
    if (m_TESModel == egEnergyCorr::es2015PRE_res_improved or m_TESModel == egEnergyCorr::es2015PRE or
        m_TESModel == egEnergyCorr::es2015cPRE or m_TESModel == egEnergyCorr::es2015c_summer or
        m_TESModel == egEnergyCorr::es2016PRE) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2015PRE__ETABIN0", +1)] = SysInfo{AbsEtaCaloPredicateFactory(decorrelation_bins_BE[0]), egEnergyCorr::Scale::LArTemperature2015PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2015PRE__ETABIN0", -1)] = SysInfo{AbsEtaCaloPredicateFactory(decorrelation_bins_BE[0]), egEnergyCorr::Scale::LArTemperature2015PreDown};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2015PRE__ETABIN1", +1)] = SysInfo{AbsEtaCaloPredicateFactory(decorrelation_bins_BE[1]), egEnergyCorr::Scale::LArTemperature2015PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2015PRE__ETABIN1", -1)] = SysInfo{AbsEtaCaloPredicateFactory(decorrelation_bins_BE[1]), egEnergyCorr::Scale::LArTemperature2015PreDown};
    }

    // additional systematic for S12 last eta bin run2
    if (m_TESModel == egEnergyCorr::es2017) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_S12EXTRALASTETABINRUN2", +1)] = SysInfo{always, egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Up};
      m_syst_description[CP::SystematicVariation("EG_SCALE_S12EXTRALASTETABINRUN2", -1)] = SysInfo{always, egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Down};
    }

    // additional systematic for PP0 region
    if (m_TESModel == egEnergyCorr::es2017) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_MATPP0__ETABIN0", +1)] = SysInfo{AbsEtaCaloPredicateFactory(0, 1.5), egEnergyCorr::Scale::MatPP0Up};
      m_syst_description[CP::SystematicVariation("EG_SCALE_MATPP0__ETABIN1", +1)] = SysInfo{AbsEtaCaloPredicateFactory(1.5, 2.5), egEnergyCorr::Scale::MatPP0Up};
      m_syst_description[CP::SystematicVariation("EG_SCALE_MATPP0__ETABIN0", -1)] = SysInfo{AbsEtaCaloPredicateFactory(0, 1.5), egEnergyCorr::Scale::MatPP0Down};
      m_syst_description[CP::SystematicVariation("EG_SCALE_MATPP0__ETABIN1", -1)] = SysInfo{AbsEtaCaloPredicateFactory(1.5, 2.5), egEnergyCorr::Scale::MatPP0Down};
    }

    // systematic related to wtots1
    if (m_TESModel == egEnergyCorr::es2017) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_WTOTS1", +1)] = SysInfo{always, egEnergyCorr::Scale::Wtots1Up};
      m_syst_description[CP::SystematicVariation("EG_SCALE_WTOTS1", -1)] = SysInfo{always, egEnergyCorr::Scale::Wtots1Down};
    }

    // systematic for the scintillators
    if (m_TESModel == egEnergyCorr::es2015cPRE or m_TESModel == egEnergyCorr::es2015c_summer or m_TESModel == egEnergyCorr::es2016PRE or m_TESModel == egEnergyCorr::es2017) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_E4SCINTILLATOR__ETABIN0", +1)] = SysInfo{AbsEtaCaloPredicateFactory(1.4, 1.46), egEnergyCorr::Scale::E4ScintillatorUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_E4SCINTILLATOR__ETABIN1", +1)] = SysInfo{AbsEtaCaloPredicateFactory(1.46, 1.52), egEnergyCorr::Scale::E4ScintillatorUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_E4SCINTILLATOR__ETABIN2", +1)] = SysInfo{AbsEtaCaloPredicateFactory(1.52, 1.6), egEnergyCorr::Scale::E4ScintillatorUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_E4SCINTILLATOR__ETABIN0", -1)] = SysInfo{AbsEtaCaloPredicateFactory(1.4, 1.46), egEnergyCorr::Scale::E4ScintillatorDown};
      m_syst_description[CP::SystematicVariation("EG_SCALE_E4SCINTILLATOR__ETABIN1", -1)] = SysInfo{AbsEtaCaloPredicateFactory(1.46, 1.52), egEnergyCorr::Scale::E4ScintillatorDown};
      m_syst_description[CP::SystematicVariation("EG_SCALE_E4SCINTILLATOR__ETABIN2", -1)] = SysInfo{AbsEtaCaloPredicateFactory(1.52, 1.6), egEnergyCorr::Scale::E4ScintillatorDown};

    }

    // additional systematic for temperature 2015->2016
    if (m_TESModel == egEnergyCorr::es2016PRE) {
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2016PRE__ETABIN0", +1)] = SysInfo{AbsEtaCaloPredicateFactory(decorrelation_bins_BE[0]), egEnergyCorr::Scale::LArTemperature2016PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2016PRE__ETABIN1", +1)] = SysInfo{AbsEtaCaloPredicateFactory(decorrelation_bins_BE[1]), egEnergyCorr::Scale::LArTemperature2016PreUp};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2016PRE__ETABIN0", -1)] = SysInfo{AbsEtaCaloPredicateFactory(decorrelation_bins_BE[0]), egEnergyCorr::Scale::LArTemperature2016PreDown};
      m_syst_description[CP::SystematicVariation("EG_SCALE_LARTEMPERATURE_EXTRA2016PRE__ETABIN1", -1)] = SysInfo{AbsEtaCaloPredicateFactory(decorrelation_bins_BE[1]), egEnergyCorr::Scale::LArTemperature2016PreDown};
    }

  }
  else {
    ATH_MSG_FATAL("scale decorrelation model invalid");
  }

  // resolution systematics
  if (m_decorrelation_model_resolution == ResolutionDecorrelation::ONENP) {
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_ALL", +1)] = egEnergyCorr::Resolution::AllUp;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_ALL", -1)] = egEnergyCorr::Resolution::AllDown;
  }
  else if (m_decorrelation_model_resolution == ResolutionDecorrelation::FULL){
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_ZSMEARING", +1)] = egEnergyCorr::Resolution::ZSmearingUp;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_ZSMEARING", -1)] = egEnergyCorr::Resolution::ZSmearingDown;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_SAMPLINGTERM", +1)] = egEnergyCorr::Resolution::SamplingTermUp;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_SAMPLINGTERM", -1)] = egEnergyCorr::Resolution::SamplingTermDown;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALID", +1)] = egEnergyCorr::Resolution::MaterialIDUp;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALID", -1)] = egEnergyCorr::Resolution::MaterialIDDown;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALCALO", +1)] = egEnergyCorr::Resolution::MaterialCaloUp;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALCALO", -1)] = egEnergyCorr::Resolution::MaterialCaloDown;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALGAP", +1)] = egEnergyCorr::Resolution::MaterialGapUp;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALGAP", -1)] = egEnergyCorr::Resolution::MaterialGapDown;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALCRYO", +1)] = egEnergyCorr::Resolution::MaterialCryoUp;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALCRYO", -1)] = egEnergyCorr::Resolution::MaterialCryoDown;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_PILEUP", +1)] = egEnergyCorr::Resolution::PileUpUp;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_PILEUP", -1)] = egEnergyCorr::Resolution::PileUpDown;
    if (m_TESModel == egEnergyCorr::es2017) {
      m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALIBL", +1)] = egEnergyCorr::Resolution::MaterialIBLUp;
      m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALIBL", -1)] = egEnergyCorr::Resolution::MaterialIBLDown;
      m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALPP0", +1)] = egEnergyCorr::Resolution::MaterialPP0Up;
      m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_MATERIALPP0", -1)] = egEnergyCorr::Resolution::MaterialPP0Down;
    }
  }
  else {
    ATH_MSG_FATAL("resolution decorrelation model invalid");
  }

  // ep combination systematics
  if (m_use_ep_combination) {
    m_syst_description[CP::SystematicVariation("EL_SCALE_MOMENTUM", +1)] = SysInfo{always, egEnergyCorr::Scale::MomentumUp};
    m_syst_description[CP::SystematicVariation("EL_SCALE_MOMENTUM", -1)] = SysInfo{always, egEnergyCorr::Scale::MomentumDown};
  }
}

CP::SystematicSet EgammaCalibrationAndSmearingTool::recommendedSystematics() const {
  return affectingSystematics();
}

CP::SystematicCode EgammaCalibrationAndSmearingTool::applySystematicVariation(const CP::SystematicSet& systConfig) {

  // set the nominal one (no systematics)
  m_currentScaleVariation_MC = egEnergyCorr::Scale::None;
  m_currentScaleVariation_data = m_doScaleCorrection ? egEnergyCorr::Scale::Nominal : egEnergyCorr::Scale::None;
  m_currentResolutionVariation_MC = m_doSmearing ? egEnergyCorr::Resolution::Nominal : egEnergyCorr::Resolution::None;
  m_currentResolutionVariation_data = egEnergyCorr::Resolution::None;
  m_currentScalePredicate = [](const xAOD::Egamma&) { return true; };

  if (systConfig.size() == 0) return CP::SystematicCode::Ok;

  // the following code allows only ONE systematic variation at a time (1 for scale, 1 for resolution)

  bool first_scale = true;
  bool first_resolution = true;
  for (const auto& it : systConfig) {
    const auto found_scale = m_syst_description.find(it);
    if (found_scale != m_syst_description.end()) {
      if (not first_scale) {
        ATH_MSG_ERROR("multiple scale variations not supported");
        throw std::runtime_error("multiple scale variations not supported");
      }
      first_scale = false;
      m_currentScaleVariation_MC = found_scale->second.effect;
      m_currentScalePredicate = found_scale->second.predicate;
    }

    const auto found_resolution = m_syst_description_resolution.find(it);
    if (found_resolution != m_syst_description_resolution.end()) {
      if (not first_resolution) {
        ATH_MSG_ERROR("multiple resolution variations not supported");
        throw std::runtime_error("multiple resolution variations not supported");
      }
      first_resolution = false;
      m_currentResolutionVariation_MC = found_resolution->second;
    }
  }

  return CP::SystematicCode::Ok;
}

double EgammaCalibrationAndSmearingTool::intermodule_correction(double Ecl,  double phi, double eta) const
{
  double Ecl_corr = 0.;
  int DivInt = 0;
  double pi = 3.1415926535897932384626433832795 ;

  //  Definitions of module folding into four quarters (top, left, bottom and right)

  DivInt =  (int) (phi / ((2 * pi) / 16.));
  double phi_mod = phi - DivInt * (2 * pi / 16.);


  //  Centring on the intermodule --> phi_mod will now be in [0,0.4]
  if (phi_mod < 0) phi_mod += pi / 8.;

  //  The correction concerns only the barrel
  if(fabs(eta) <= 1.4){

    //  Top quarter
    if(phi < (3 * pi) / 4. && phi >= pi / 4.){
Ecl_corr = Ecl / (1-0.131 * ((1 / (1 + exp((phi_mod-0.2) * 199.08))) * (1 / (1 + exp((phi_mod-0.2) * (-130.36))))));
    }

    //  Right quarter
    if(phi < pi / 4. && phi >= -pi / 4.){
Ecl_corr = Ecl / (1-0.0879 * ((1 / (1 + exp((phi_mod-0.2) * 221.01))) * (1 / (1 + exp((phi_mod-0.2) * (-149.51))))));
    }
    //  Bottom quarter
    if(phi < -pi / 4. && phi >= (-3 * pi) / 4.){
Ecl_corr = Ecl / (1-0.0605 * ((1 / (1 + exp((phi_mod-0.2) * 281.37))) * (1 / (1 + exp((phi_mod-0.2) * (-170.29))))));
    }
    //  Left quarter
    if((phi < (-3 * pi) / 4.) || (phi >= (3 * pi) / 4.)){
Ecl_corr = Ecl / (1-0.102 * ((1 / (1 + exp((phi_mod-0.2) * 235.37))) * (1 / (1 + exp((phi_mod-0.2) * (-219.04))))));
    }
  }

  //  No correction for the EC
  else{
    Ecl_corr = Ecl;
  }

  return Ecl_corr;

}



double EgammaCalibrationAndSmearingTool::correction_phi_unif(double eta, double phi) const
{
  const double PI = 3.141592653589793;  // TODO: move to M_PI from cmath with #define _USE_MATH_DEFINES
  double Fcorr = 1.0;

  if (m_use_mapping_correction) {
    // wrong mapping HV -> sectors in run1
    if (eta < -0.4 && eta > -0.6) {
      if (phi < (14 * PI / 32.) && phi > (13 * PI / 32.)) { Fcorr += 0.035; }
      else if (phi < (13 * PI / 32.) && phi > (12 * PI / 32.)) { Fcorr -= 0.035; }
    }
  }

  if (eta < 0.6 && eta > 0.4) {
    if (phi < 0 && phi > (-2 * PI / 32.)) { Fcorr = 1.028; }
    else if (phi < (-4 * 2 * PI / 32.) && phi > (-5 * 2 * PI / 32.)) { Fcorr = 1.044; }
  }

  else if (eta < 0.8 && eta > 0.6) {
    if (phi < (7 * 2 * PI / 32.) && phi > (6 * 2 * PI / 32.)) { Fcorr = 1.022; }
  }

  else if (eta < 1.4 && eta > 1.2) {
    if (phi < (-11 * 2 * PI / 32.) && phi > (-12 * 2 * PI / 32.)) { Fcorr = 1.038; }
  }

  else if (eta < 2.0 && eta > 1.9 ) {
    if (phi < (10 * 2 * PI / 32.) && phi > (9 * 2 * PI / 32.)) { Fcorr = 1.029; }
  }

  else if(eta < -1.2 && eta > -1.4) {
    if (phi < (-4 * 2 * PI / 32.) && phi > (-5 * 2 * PI / 32.)) { Fcorr = 1.048; }
    else if (phi < (-6 * 2 * PI / 32.) && phi > (-7 * 2 * PI / 32.)) { Fcorr = 1.048; }
  }

  else if (eta < -1.6 && eta > -1.8 ) {
    if (phi < (9 * 2 * PI / 32.) && phi > (8 * 2 * PI / 32.)) { Fcorr = 1.024; }
  }

  else if(eta < -2.3 && eta > -2.5) {
    if (phi < (-8 * 2 * PI / 32.) && phi > (-9 * 2 * PI / 32.)) { Fcorr = 1.037; }
    else if (phi < (5 * 2 * PI / 32.) && phi > (4 * 2 * PI / 32.)) { Fcorr = 1.031; }
    else if (phi < (9 * 2 * PI / 32.) && phi > (8 * 2 * PI / 32.)) { Fcorr = 1.040; }
    else if (phi < (10 * 2 * PI / 32.) && phi > (9 * 2 * PI / 32.)) { Fcorr = 1.030; }
    else if (phi < (11 * 2 * PI / 32.) && phi > (10 * 2 * PI / 32.)) { Fcorr = 1.020; }
  }

  return Fcorr;
}



} // namespace CP
