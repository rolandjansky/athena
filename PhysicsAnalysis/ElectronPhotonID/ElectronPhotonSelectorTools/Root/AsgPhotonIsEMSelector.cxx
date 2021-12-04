/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
   @class AsgPhotonIsEMSelector
   @brief Photon isEM selector
   @author Jovan Mitrevski Karsten Koeneke
   @date   Dec 2011 - Fab 2012
   11-MAR-2014 convert to ASG tool
*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "AsgTools/CurrentContext.h"
#include "EGSelectorConfigurationMapping.h"
#include "EgammaAnalysisHelpers/AsgEGammaConfigHelper.h"
#include "ElectronPhotonSelectorTools/EGammaAmbiguityTool.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"
#include "PathResolver/PathResolver.h"
#include "TEnv.h"
#include "TPhotonIsEMSelector.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

//=============================================================================
// Standard constructor
//=============================================================================
AsgPhotonIsEMSelector::AsgPhotonIsEMSelector(const std::string& myname)
  : AsgTool(myname)
  , m_configFile("")
  , m_rootTool(nullptr)
{

  m_rootTool = new Root::TPhotonIsEMSelector(myname.c_str());

  declareProperty("WorkingPoint", m_WorkingPoint = "", "The Working Point");
  declareProperty("ConfigFile",
                  m_configFile = "",
                  "The config file to use (if not setting cuts one by one)");

  // Name of the PID
  declareProperty(
    "isEMMask",
    m_rootTool->m_isEMMask =
      egammaPID::EgPidUndefined, // All pass by default, if not specified
    "The mask to use");

  // boolean to force to test converted photon hypothesis
  declareProperty("ForceConvertedPhotonPID",
                  m_rootTool->m_forceConvertedPhotonPID = false,
                  "boolean to force to test converted photon hypothesis");

  // boolean to force to test non converted photon hypothesis
  declareProperty("ForceNonConvertedPhotonPID",
                  m_rootTool->m_forceNonConvertedPhotonPID = false,
                  "boolean to force to test non converted photon hypothesis");

  // new cuts on f3
  // Use f3core instead of f3
  declareProperty("useF3core", m_useF3core = false, "Cut on f3 or f3core?");

  // for the trigger needs:
  declareProperty("caloOnly",
                  m_caloOnly = false,
                  "Flag to tell the tool if its a calo only cutbase");
  declareProperty("trigEtTh", m_trigEtTh = -999., "Trigger threshold");
}

// =================================================================
AsgPhotonIsEMSelector::~AsgPhotonIsEMSelector()
{
  delete m_rootTool;
}

// =================================================================
StatusCode
AsgPhotonIsEMSelector::initialize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS;

  if (!m_WorkingPoint.empty()) {
    m_configFile = AsgConfigHelper::findConfigFile(
      m_WorkingPoint, EgammaSelectors::PhotonCutPointToConfFile);
    m_rootTool->m_isEMMask = AsgConfigHelper::findMask(
      m_WorkingPoint, EgammaSelectors::PhotonCutPointToMask);
  }

  std::string filename = PathResolverFindCalibFile(m_configFile);
  if (filename.empty()) {
    ATH_MSG_ERROR("Could not locate " << m_configFile);
    sc = StatusCode::FAILURE;
    return sc;
  }
  ATH_MSG_INFO("Configfile to use  " << m_configFile);
  TEnv env;
  env.ReadFile(filename.c_str(), kEnvLocal);
  ///------- Read in the TEnv config ------///
  // Override the mask via the config only if it is not set
  if (m_rootTool->m_isEMMask == egammaPID::EgPidUndefined) {
    unsigned int mask(
      env.GetValue("isEMMask", static_cast<int>(egammaPID::EgPidUndefined)));
    m_rootTool->m_isEMMask = mask;
  }
  ///------- Use helpers to read in the cut arrays ------///
  m_rootTool->m_cutBinEta_photonsNonConverted =
    AsgConfigHelper::HelperFloat("CutBinEta_photonsNonConverted", env);
  m_rootTool->m_cutBinEnergy_photonsNonConverted =
    AsgConfigHelper::HelperFloat("CutBinEnergy_photonsNonConverted", env);
  m_rootTool->m_e277_photonsNonConverted =
    AsgConfigHelper::HelperFloat("e277_photonsNonConverted", env);
  m_rootTool->m_cutHadLeakage_photonsNonConverted =
    AsgConfigHelper::HelperFloat("CutHadLeakage_photonsNonConverted", env);
  m_rootTool->m_Reta37_photonsNonConverted =
    AsgConfigHelper::HelperFloat("Reta37_photonsNonConverted", env);
  m_rootTool->m_Rphi33_photonsNonConverted =
    AsgConfigHelper::HelperFloat("Rphi33_photonsNonConverted", env);
  m_rootTool->m_weta2_photonsNonConverted =
    AsgConfigHelper::HelperFloat("weta2_photonsNonConverted", env);
  m_rootTool->m_cutBinEtaStrips_photonsNonConverted =
    AsgConfigHelper::HelperFloat("CutBinEtaStrips_photonsNonConverted", env);
  m_rootTool->m_cutBinEnergyStrips_photonsNonConverted =
    AsgConfigHelper::HelperFloat("CutBinEnergyStrips_photonsNonConverted", env);
  m_rootTool->m_f1_photonsNonConverted =
    AsgConfigHelper::HelperFloat("f1_photonsNonConverted", env);
  m_rootTool->m_deltae_photonsNonConverted =
    AsgConfigHelper::HelperFloat("deltae_photonsNonConverted", env);
  m_rootTool->m_DEmaxs1_photonsNonConverted =
    AsgConfigHelper::HelperFloat("DEmaxs1_photonsNonConverted", env);
  m_rootTool->m_wtot_photonsNonConverted =
    AsgConfigHelper::HelperFloat("wtot_photonsNonConverted", env);
  m_rootTool->m_fracm_photonsNonConverted =
    AsgConfigHelper::HelperFloat("fracm_photonsNonConverted", env);
  m_rootTool->m_w1_photonsNonConverted =
    AsgConfigHelper::HelperFloat("w1_photonsNonConverted", env);
  m_rootTool->m_cutF3_photonsNonConverted =
    AsgConfigHelper::HelperFloat("CutF3_photonsNonConverted", env);
  m_rootTool->m_cutBinEta_photonsConverted =
    AsgConfigHelper::HelperFloat("CutBinEta_photonsConverted", env);
  m_rootTool->m_cutBinEnergy_photonsConverted =
    AsgConfigHelper::HelperFloat("CutBinEnergy_photonsConverted", env);
  m_rootTool->m_e277_photonsConverted =
    AsgConfigHelper::HelperFloat("e277_photonsConverted", env);
  m_rootTool->m_cutHadLeakage_photonsConverted =
    AsgConfigHelper::HelperFloat("CutHadLeakage_photonsConverted", env);
  m_rootTool->m_Reta37_photonsConverted =
    AsgConfigHelper::HelperFloat("Reta37_photonsConverted", env);
  m_rootTool->m_Rphi33_photonsConverted =
    AsgConfigHelper::HelperFloat("Rphi33_photonsConverted", env);
  m_rootTool->m_weta2_photonsConverted =
    AsgConfigHelper::HelperFloat("weta2_photonsConverted", env);
  m_rootTool->m_cutBinEtaStrips_photonsConverted =
    AsgConfigHelper::HelperFloat("CutBinEtaStrips_photonsConverted", env);
  m_rootTool->m_cutBinEnergyStrips_photonsConverted =
    AsgConfigHelper::HelperFloat("CutBinEnergyStrips_photonsConverted", env);
  m_rootTool->m_f1_photonsConverted =
    AsgConfigHelper::HelperFloat("f1_photonsConverted", env);
  m_rootTool->m_deltae_photonsConverted =
    AsgConfigHelper::HelperFloat("deltae_photonsConverted", env);
  m_rootTool->m_DEmaxs1_photonsConverted =
    AsgConfigHelper::HelperFloat("DEmaxs1_photonsConverted", env);
  m_rootTool->m_wtot_photonsConverted =
    AsgConfigHelper::HelperFloat("wtot_photonsConverted", env);
  m_rootTool->m_fracm_photonsConverted =
    AsgConfigHelper::HelperFloat("fracm_photonsConverted", env);
  m_rootTool->m_w1_photonsConverted =
    AsgConfigHelper::HelperFloat("w1_photonsConverted", env);
  m_rootTool->m_cutminEp_photonsConverted =
    AsgConfigHelper::HelperFloat("CutminEp_photonsConverted", env);
  m_rootTool->m_cutmaxEp_photonsConverted =
    AsgConfigHelper::HelperFloat("CutmaxEp_photonsConverted", env);
  m_rootTool->m_cutF3_photonsConverted =
    AsgConfigHelper::HelperFloat("CutF3_photonsConverted", env);

  ATH_MSG_INFO("operating point : " << this->getOperatingPointName());
  // Get the message level and set the underlying ROOT tool message level
  // accordingly
  m_rootTool->msg().setLevel(this->msg().level());

  // We need to initialize the underlying ROOT TSelectorTool
  if (m_rootTool->initialize().isFailure()) {
    ATH_MSG_ERROR("Could not initialize the TPhotonIsEMSelector!");
    sc = StatusCode::FAILURE;
    return sc;
  }

  return sc;
}

//=============================================================================
// return the accept info object
//=============================================================================

const asg::AcceptInfo&
AsgPhotonIsEMSelector::getAcceptInfo() const
{
  return m_rootTool->getAcceptInfo();
}

//=============================================================================
// The main accept method: the actual cuts are applied here
//=============================================================================
asg::AcceptData
AsgPhotonIsEMSelector::accept(const xAOD::IParticle* part) const
{
  return AsgPhotonIsEMSelector::accept(Gaudi::Hive::currentContext(), part);
}

asg::AcceptData
AsgPhotonIsEMSelector::accept(const EventContext& ctx,
                              const xAOD::IParticle* part) const
{

  if (part->type() == xAOD::Type::Photon ||
      part->type() == xAOD::Type::Electron) {
    return accept(ctx, static_cast<const xAOD::Egamma*>(part));
  }

  ATH_MSG_ERROR(
    "AsgElectronIsEMSelector::could not convert argument to Photon/Electron");
  return m_rootTool->accept();
}
asg::AcceptData
AsgPhotonIsEMSelector::accept(const EventContext& ctx,
                              const xAOD::Egamma* eg) const
{

  if (eg) {
    unsigned int isEM = ~0;
    StatusCode sc = execute(ctx, eg, isEM);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("could not calculate isEM");
      return m_rootTool->accept();
    }
    return m_rootTool->fillAccept(isEM);
  }

  ATH_MSG_ERROR("AsgElectronIsEMSelector::accept was given a bad argument");
  return m_rootTool->accept();
}
asg::AcceptData
AsgPhotonIsEMSelector::accept(const EventContext& ctx,
                              const xAOD::Photon* ph) const
{
  return accept(ctx, static_cast<const xAOD::Egamma*>(ph));
}
asg::AcceptData
AsgPhotonIsEMSelector::accept(const EventContext& ctx,
                              const xAOD::Electron* el) const
{
  return accept(ctx, static_cast<const xAOD::Egamma*>(el));
}

//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string
AsgPhotonIsEMSelector::getOperatingPointName() const
{
  //
  // For Loose, Medium and Tight ignore if the difference is in bit 23
  if (m_rootTool->m_isEMMask == egammaPID::PhotonLoose ||
      (egammaPID::PhotonLoose ^ m_rootTool->m_isEMMask) ==
        (0x1 << egammaPID::AmbiguityResolution_Photon)) {
    return "Loose";
  }
  if (m_rootTool->m_isEMMask == egammaPID::PhotonMedium ||
      (egammaPID::PhotonMedium ^ m_rootTool->m_isEMMask) ==
        (0x1 << egammaPID::AmbiguityResolution_Photon)) {
    return "Medium";
  }
  if (m_rootTool->m_isEMMask == egammaPID::PhotonTight ||
      (egammaPID::PhotonTight ^ m_rootTool->m_isEMMask) ==
        (0x1 << egammaPID::AmbiguityResolution_Photon)) {
    return "Tight";
  }
  if (m_rootTool->m_isEMMask == egammaPID::PhotonLooseEF) {
    return "LooseEF";
  }
  if (m_rootTool->m_isEMMask == egammaPID::PhotonMediumEF) {
    return "MediumEF";
  }
  if (m_rootTool->m_isEMMask == 0) {
    return "No cuts applied";
  }

  ATH_MSG_ERROR("Didn't recognize the given operating point with mask: "
                << m_rootTool->m_isEMMask);
  return "";
}

// A simple execute command wrapper
// ==============================================================
StatusCode
AsgPhotonIsEMSelector::execute(const EventContext& ctx,
                               const xAOD::Egamma* eg,
                               unsigned int& isEM) const
{
  //
  // Particle identification for photons based on cuts
  // trigEtTh : threshold in ET as applied by trigger
  //
  (void)ctx;

  // initialisation
  isEM = 0;

  // protection against null pointer
  if (eg == nullptr) {
    ATH_MSG_ERROR("eg == 0");
    // if object is bad then use the bit for "bad eta"
    isEM = (0x1 << egammaPID::ClusterEtaRange_Photon);
    return StatusCode::SUCCESS;
  }

  // protection against bad clusters
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if (cluster == nullptr) {
    ATH_MSG_ERROR("exiting because cluster is NULL " << cluster);
    // if object is bad then use the bit for "bad eta"
    isEM = (0x1 << egammaPID::ClusterEtaRange_Photon);
    return StatusCode::SUCCESS;
  }

  // Fill variables
  // eta position in second sampling
  const float eta2 = fabsf(cluster->etaBE(2));
  // transverse energy in calorimeter (using eta position in second sampling)
  const double energy = cluster->e();
  double et = 0.;
  if (eta2 < 999.) {
    const double cosheta = cosh(eta2);
    et = (cosheta != 0.) ? energy / cosheta : 0.;
  }

  // variables based on HCAL
  // transverse energy in 1st scintillator of hadronic calorimeter/ET
  float Rhad1 = eg->showerShapeValue(xAOD::EgammaParameters::Rhad1);
  // transverse energy in hadronic calorimeter/ET
  float Rhad = eg->showerShapeValue(xAOD::EgammaParameters::Rhad);

  // variables based on S2 of EM CAL
  // E(7*7) in 2nd sampling
  float e277 = eg->showerShapeValue(xAOD::EgammaParameters::e277);
  // E(3*7)/E(7*7) in 2nd sampling
  float Reta = eg->showerShapeValue(xAOD::EgammaParameters::Reta);
  // E(3*3)/E(3*7) in 2nd sampling
  float Rphi = eg->showerShapeValue(xAOD::EgammaParameters::Rphi);
  // shower width in 2nd sampling
  float weta2c = eg->showerShapeValue(xAOD::EgammaParameters::weta2);

  // variables based on S1 of EM CAL
  // fraction of energy reconstructed in the 1st sampling
  float f1 = eg->showerShapeValue(xAOD::EgammaParameters::f1);
  // shower width in 3 strips in 1st sampling
  float weta1c = eg->showerShapeValue(xAOD::EgammaParameters::weta1);
  // (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
  float Eratio = eg->showerShapeValue(xAOD::EgammaParameters::Eratio);
  // E(2nd max)-E(min) in strips
  float DeltaE = eg->showerShapeValue(xAOD::EgammaParameters::DeltaE);
  // total shower width in 1st sampling
  float wtot = eg->showerShapeValue(xAOD::EgammaParameters::wtots1);
  // E(+/-3)-E(+/-1)/E(+/-1)
  float fracm = eg->showerShapeValue(xAOD::EgammaParameters::fracs1);

  float f3(0);
  if (m_useF3core) {
    f3 = eg->showerShapeValue(xAOD::EgammaParameters::f3core);
  } else {
    f3 = eg->showerShapeValue(xAOD::EgammaParameters::f3);
  }

  // cut on E/p
  double ep = 1.0; // default passes

  if (m_caloOnly) {
    ATH_MSG_DEBUG("Doing CaloCutsOnly");
  } else {
    if (xAOD::EgammaHelpers::isConvertedPhoton(
          eg)) // returns false if not photon or no conversion
    {
      const xAOD::Photon* ph = static_cast<const xAOD::Photon*>(eg);
      float p = xAOD::EgammaHelpers::momentumAtVertex(ph).mag();
      if (p != 0.) {
        ep = energy / p;
      } else {
        ep = 9999999.;
      }
    }
  }

  // modifiy et when dealing with trigger
  // to be sure that it will take the correct bin (VD)
  if (m_trigEtTh > 0)
    et = m_trigEtTh * 1.01;

  // apply calorimeter selection for photons
  isEM = m_rootTool->calcIsEm(eta2,
                              et,
                              Rhad1,
                              Rhad,
                              e277,
                              Reta,
                              Rphi,
                              weta2c,
                              f1,
                              Eratio,
                              DeltaE,
                              weta1c,
                              wtot,
                              fracm,
                              f3,
                              ep,
                              xAOD::EgammaHelpers::isConvertedPhoton(eg));

  // Add ambiguity resolution cut for photon (vs electron)
  // to reproduce release 21.2 ambiguity tool configuration
  static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
  int AmbiguityType = acc(*eg);
  if (eg->author() == xAOD::EgammaParameters::AuthorAmbiguous &&
      AmbiguityType == xAOD::AmbiguityTool::ambiguousNoInnermost) {
    isEM |= (0x1 << egammaPID::AmbiguityResolution_Photon);
  }

  return StatusCode::SUCCESS;
}

