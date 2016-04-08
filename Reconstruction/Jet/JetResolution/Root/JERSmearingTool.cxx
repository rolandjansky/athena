/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System includes
#include <cmath>
#include <stdexcept>

// Local includes
#include "JetResolution/JERSmearingTool.h"

// Other includes
#include "PATInterfaces/SystematicRegistry.h"

// Simple configuration
static const CP::SystematicSet simpleSysts = {
  CP::SystematicVariation("JET_JER_SINGLE_NP", 1)
};

// Full configuration
static const CP::SystematicSet fullSysts = {
  CP::SystematicVariation("JET_JER_NP0", 1),
  CP::SystematicVariation("JET_JER_NP0", -1),
  CP::SystematicVariation("JET_JER_NP1", 1),
  CP::SystematicVariation("JET_JER_NP1", -1),
  CP::SystematicVariation("JET_JER_NP2", 1),
  CP::SystematicVariation("JET_JER_NP2", -1),
  CP::SystematicVariation("JET_JER_NP3", 1),
  CP::SystematicVariation("JET_JER_NP3", -1),
  CP::SystematicVariation("JET_JER_NP4", 1),
  CP::SystematicVariation("JET_JER_NP4", -1),
  CP::SystematicVariation("JET_JER_NP5", 1),
  CP::SystematicVariation("JET_JER_NP5", -1),
  CP::SystematicVariation("JET_JER_NP6", 1),
  CP::SystematicVariation("JET_JER_NP6", -1),
  CP::SystematicVariation("JET_JER_NP7", 1),
  CP::SystematicVariation("JET_JER_NP7", -1),
  CP::SystematicVariation("JET_JER_NP8", 1),
  CP::SystematicVariation("JET_JER_NP8", -1),
  CP::SystematicVariation("JET_JER_NOISE_FORWARD", 1),
  CP::SystematicVariation("JET_JER_CROSS_CALIB_FORWARD", 1)
};

//-----------------------------------------------------------------------------
// Standard ASG tool constructor
//-----------------------------------------------------------------------------
JERSmearingTool::JERSmearingTool(const std::string& name)
  : asg::AsgTool(name),
    m_jerTool("JERTool/JERTool"),
    m_userSeed(0),
    m_sysConfig(nullptr)
{
  // Property for setting the tool handle directly
  declareProperty("JERTool", m_jerTool);
  // Temporary workaround for setting the JERTool in PyROOT
  declareProperty("JERToolName", m_jerToolName="");
  // Keeping this here for backwards compatibility,
  // but nominal smearing is actually disabled for now.
  declareProperty("ApplyNominalSmearing", m_applyNominalSmearing = false,
                  "If set to false, smearing only applied for JER sys");
  declareProperty("isMC", m_isMC = true,
                  "set to true if you run on MC, set to false if you run on "
                  "data or pseudo-data");
  // Could use an enum for this setting if preferred
  declareProperty("SystematicMode", m_sysMode = "Simple",
                  "Combined NP (Simple) or full sys breakdown (Full)");
  // Apply nominal sys config
  applySystematicVariation(CP::SystematicSet()).ignore();
}

//-----------------------------------------------------------------------------
// Copy constructor for reflex in Athena.
// Implement a shallow copy for now. Come back to this later.
// TODO: update this with the new data members.
//-----------------------------------------------------------------------------
JERSmearingTool::JERSmearingTool(const JERSmearingTool& other)
  : asg::AsgTool(other.name() + "_copy"),
    m_jerTool(other.m_jerTool),
    m_jerToolName(other.m_jerToolName),
    m_applyNominalSmearing(other.m_applyNominalSmearing),
    m_rand(other.m_rand),
    m_userSeed(other.m_userSeed),
    m_sysConfig(nullptr)
{}

//-----------------------------------------------------------------------------
// Initialize the tool
//-----------------------------------------------------------------------------
StatusCode JERSmearingTool::initialize()
{
  // Greet the user
  ATH_MSG_INFO("Initializing JER smearing tool");

  // Workaround for configuring ToolHandle in PyROOT
  if(!m_jerToolName.empty())
    m_jerTool = ToolHandle<IJERTool>(m_jerToolName);

  // Try to retrieve the JERTool
  if(m_jerTool.retrieve() != StatusCode::SUCCESS){
    ATH_MSG_ERROR("Cannot retrieve the JER tool!");
    return StatusCode::FAILURE;
  }

  // Nominal smearing is disabled for now
  if(m_applyNominalSmearing){
    ATH_MSG_WARNING("Nominal smearing is currently unsupported. Disabling");
    m_applyNominalSmearing = false;
  }

  // Make sure we have a valid systematic mode
  if(m_sysMode != "Simple" && m_sysMode != "Full"){
    ATH_MSG_ERROR("Unsupported SystematicMode: " << m_sysMode);
    ATH_MSG_ERROR("Should be 'Simple' or 'Full'");
    return StatusCode::FAILURE;
  }

  // Add the affecting systematics to the global registry
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if(registry.registerSystematics(*this) != CP::SystematicCode::Ok){
    ATH_MSG_ERROR("Unable to register systematics!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Apply the JER correction on a modifiable jet
//-----------------------------------------------------------------------------
CP::CorrectionCode JERSmearingTool::applyCorrection(xAOD::Jet& jet)
{
  bool applySys = !m_sysConfig->empty();
  if(applySys || m_applyNominalSmearing){
    // TODO: change these methods to return a StatusCode instead?
    // I don't know, exceptions are pretty nice to use (internally only).
    double smear = 0.;
    try {
      smear = applySys ? getSmearingFactorSys(&jet)
                       : getSmearingFactorNom(&jet);
    }
    catch(const std::logic_error& err){
      ATH_MSG_FATAL("Logic error in smearing factor: " << err.what());
      return CP::CorrectionCode::Error;
    }

    // Sanity check
    ATH_MSG_DEBUG("Applying smearing factor: " << smear);
    if(smear < 0) ATH_MSG_WARNING("Negative smear factor: " << smear);

    // No non-const accessor, so I need to copy the p4 to modify it.
    // Maybe we can set pt and E only. It would reduce the number of
    // branches in shallow copies.
    xAOD::JetFourMom_t p4 = jet.jetP4();
    jet.setJetP4(p4*smear);
  }

  return CP::CorrectionCode::Ok;
}

//-----------------------------------------------------------------------------
// Declare affecting systematics
//-----------------------------------------------------------------------------
bool JERSmearingTool::isAffectedBySystematic
(const CP::SystematicVariation& systematic) const
{
  // Using 'find' is sufficient until this tool supports continuous
  // variations, at which point I'll need to use the 'match' method.
  static CP::SystematicSet sys = affectingSystematics();
  return sys.find(systematic) != sys.end();
}
//-----------------------------------------------------------------------------
CP::SystematicSet JERSmearingTool::affectingSystematics() const
{
  CP::SystematicSet result;
  result.insert(simpleSysts);
  result.insert(fullSysts);
  return result;
}
//-----------------------------------------------------------------------------
CP::SystematicSet JERSmearingTool::recommendedSystematics() const
{
  // Currently two possible recommendations: Simple and Full
  if(m_sysMode == "Simple") return simpleSysts;
  if(m_sysMode == "Full") return fullSysts;
  else{
    ATH_MSG_WARNING("Unsupported SystematicMode: " << m_sysMode);
    ATH_MSG_WARNING("Should be 'Simple' or 'Full'");
    ATH_MSG_WARNING("Returning an empty set");
  }
  return CP::SystematicSet();
}

//-----------------------------------------------------------------------------
// Apply systematic configuration
//-----------------------------------------------------------------------------
CP::SystematicCode JERSmearingTool::applySystematicVariation
(const CP::SystematicSet& systematics)
{
  // First check if we already know this systematic configuration.
  // Look for it in our filter map.
  auto sysMapItr = m_sysFilterMap.find(systematics);

  // If this is a new input set, we need to filter it.
  if(sysMapItr == m_sysFilterMap.end()){

    // Filter the input systematics with my affecting systematics.
    // Should be ok to use static here, as the affecting systematics should be
    // the same across any instance of this tool.
    static const CP::SystematicSet affectingSysts = affectingSystematics();
    CP::SystematicSet filteredSysts;
    if( CP::SystematicSet::
        filterForAffectingSystematics(systematics, affectingSysts, filteredSysts) !=
        CP::SystematicCode::Ok ) {
      ATH_MSG_ERROR("Received unsupported systematics: " << systematics.name());
      return CP::SystematicCode::Unsupported;
    }

    // At this point, we can do some additional checks for consistency
    // with the JER functionality. For example, if the tool can only handle
    // one type of systematic at a time, we return an error if the filtered
    // set has more than one item:
    if(filteredSysts.size() > 1){
      ATH_MSG_ERROR("No support for more than one JER sys at a time: " <<
                    filteredSysts.name());
      return CP::SystematicCode::Unsupported;
    }

    // Insert the new filtered set onto our filter map
    sysMapItr = m_sysFilterMap.insert
      (std::make_pair(systematics, filteredSysts)).first;

  }

  // Apply the filtered systematics
  m_sysConfig = &sysMapItr->second;

  return CP::SystematicCode::Ok;
}

//-----------------------------------------------------------------------------
// Get random nominal smearing factor to match the jet energy resolution
// in MC to the resolution in data
//-----------------------------------------------------------------------------
double JERSmearingTool::getSmearingFactorNom(const xAOD::Jet* jet)
{
  double resMC = m_jerTool->getRelResolutionMC(jet);
  double resData = m_jerTool->getRelResolutionData(jet);
  double sigma = resData > resMC ? sqrt(resData*resData - resMC*resMC) : 0.0;
  ATH_MSG_DEBUG("Retrieved nominal jet resolution values: MC " << resMC <<
                " Data " << resData << " SmearSigma " << sigma);
  return getSmearingFactor(jet, sigma);
}

//-----------------------------------------------------------------------------
// Get random smearing factor for evaluating jet energy resolution
// systematic uncertainty
//-----------------------------------------------------------------------------
double JERSmearingTool::getSmearingFactorSys(const xAOD::Jet* jet)
{
  // For now, assuming there is only one (affecting) systematic at a time.
  // If we want to allow simultaneous variations, then the code will need
  // to be updated to apply multiple variations on top of each other.
  if(m_sysConfig->size() != 1)
    throw std::logic_error("More than one affecting systematic received");
  const CP::SystematicVariation& sys = *m_sysConfig->begin();

  // MC resolution
  double resMC = m_jerTool->getRelResolutionMC(jet);
  // Data resolution
  double resData = m_jerTool->getRelResolutionData(jet);

  // The smearing parameter
  double smearingSigma = 0.; double resSys = 0.; double sigmaNP = 0.;

  // All NPs combined in quadrature
  if(sys.basename() == "JET_JER_SINGLE_NP"){
    // Systematic-shifted resolution
    resSys = resMC + m_jerTool->getUncertainty(jet, JER::JER_NP_ALL);
    smearingSigma =  m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
  }

  // NP 0
  else if(sys.basename() == "JET_JER_NP0"){
    // Size and sign of the requested variation.
    float sysVar = sys.parameter();
    sigmaNP = sysVar * m_jerTool->getUncertainty(jet, JER::JER_NP0);

    if (sigmaNP > 0.){
      resSys = resData + sigmaNP;
      smearingSigma = m_isMC ? 0. : sqrt(resSys*resSys - resData*resData);
    }
    else {
      resSys = resMC + fabs(sigmaNP);
      smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
    }
  }

  // NP 1
  else if(sys.basename() == "JET_JER_NP1"){
    float sysVar = sys.parameter();
    sigmaNP = sysVar * m_jerTool->getUncertainty(jet, JER::JER_NP1);

    if (sigmaNP > 0.){
      resSys = resData + sigmaNP;
      smearingSigma = m_isMC ? 0. : sqrt(resSys*resSys - resData*resData);
    }
    else {
      resSys = resMC + fabs(sigmaNP);
      smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
    }
  }

  // NP 2
  else if(sys.basename() == "JET_JER_NP2"){
    float sysVar = sys.parameter();
    sigmaNP = sysVar * m_jerTool->getUncertainty(jet, JER::JER_NP2);

    if (sigmaNP > 0.){
      resSys = resData + sigmaNP;
      smearingSigma = m_isMC ? 0. : sqrt(resSys*resSys - resData*resData);
    }
    else {
      resSys = resMC + fabs(sigmaNP);
      smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
    }
  }

  // NP 3
  else if(sys.basename() == "JET_JER_NP3"){
    float sysVar = sys.parameter();
    sigmaNP = sysVar * m_jerTool->getUncertainty(jet, JER::JER_NP3);

    if (sigmaNP > 0.){
      resSys = resData + sigmaNP;
      smearingSigma = m_isMC ? 0. : sqrt(resSys*resSys - resData*resData);
    }
    else {
      resSys = resMC + fabs(sigmaNP);
      smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
    }
  }

  // NP 4
  else if(sys.basename() == "JET_JER_NP4"){
    float sysVar = sys.parameter();
    sigmaNP = sysVar * m_jerTool->getUncertainty(jet, JER::JER_NP4);

    if (sigmaNP > 0.){
      resSys = resData + sigmaNP;
      smearingSigma = m_isMC ? 0. : sqrt(resSys*resSys - resData*resData);
    }
    else {
      resSys = resMC + fabs(sigmaNP);
      smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
    }
  }

  // NP 5
  else if(sys.basename() == "JET_JER_NP5"){
    float sysVar = sys.parameter();
    sigmaNP = sysVar * m_jerTool->getUncertainty(jet, JER::JER_NP5);

    if (sigmaNP > 0.){
      resSys = resData + sigmaNP;
      smearingSigma = m_isMC ? 0. : sqrt(resSys*resSys - resData*resData);
    }
    else {
      resSys = resMC + fabs(sigmaNP);
      smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
    }
  }

  // NP 6
  else if(sys.basename() == "JET_JER_NP6"){
    float sysVar = sys.parameter();
    sigmaNP = sysVar * m_jerTool->getUncertainty(jet, JER::JER_NP6);

    if (sigmaNP > 0.){
      resSys = resData + sigmaNP;
      smearingSigma = m_isMC ? 0. : sqrt(resSys*resSys - resData*resData);
    }
    else {
      resSys = resMC + fabs(sigmaNP);
      smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
    }
  }

  // NP 7
  else if(sys.basename() == "JET_JER_NP7"){
    float sysVar = sys.parameter();
    sigmaNP = sysVar * m_jerTool->getUncertainty(jet, JER::JER_NP7);

    if (sigmaNP > 0.){
      resSys = resData + sigmaNP;
      smearingSigma = m_isMC ? 0. : sqrt(resSys*resSys - resData*resData);
    }
    else {
      resSys = resMC + fabs(sigmaNP);
      smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
    }
  }

  // NP 8
  else if(sys.basename() == "JET_JER_NP8"){
    float sysVar = sys.parameter();
    sigmaNP = sysVar * m_jerTool->getUncertainty(jet, JER::JER_NP8);

    if (sigmaNP > 0.){
      resSys = resData + sigmaNP;
      smearingSigma = m_isMC ? 0. : sqrt(resSys*resSys - resData*resData);
    }
    else {
      resSys = resMC + fabs(sigmaNP);
      smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0. ;
    }
  }

  // JER_NOISE_FORWARD, Be careful: Noise forward unc is stored as relative;
  // need to use absolute (jerMC*unc)
  else if(sys.basename() == "JET_JER_NOISE_FORWARD"){
    // Systematic-shifted resolution
    resSys = resMC + resMC*m_jerTool->getUncertainty(jet, JER::JER_NOISE_FORWARD);
    smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0.;
  }

  // JER from cross calibration
  else if(sys.basename() == "JET_JER_CROSS_CALIB_FORWARD"){
    // Systematic-shifted resolution
    resSys = resMC + m_jerTool->getUncertainty(jet, JER::JER_CROSS_CALIB_ALL);
    smearingSigma = m_isMC ? sqrt(resSys*resSys - resMC*resMC) : 0.;
  }

  // Whatever else will be expected
  //else if(...)
  //}

  // We should only get to unknown systematic if there's some
  // kind of logic error in the code. E.g., maybe we were supposed
  // to be applying nominal settings in this case.
  else throw std::logic_error("Bad syst config");

  ATH_MSG_DEBUG("sigmaNP:  " << sigmaNP );
  ATH_MSG_DEBUG("Retrieved systematic jet resolution values: MC " << resMC <<
                " Systematic Name " << sys.basename() << " SysUp " << resSys <<
                " SmearSigma " << smearingSigma);

  return getSmearingFactor(jet, smearingSigma);
}

//-----------------------------------------------------------------------------
// Get the random smearing factor for a given sigma
//-----------------------------------------------------------------------------
double JERSmearingTool::getSmearingFactor(const xAOD::Jet* jet, double sigma)
{
  // Set the seed
  long int seed = m_userSeed;
  if(seed == 0) seed = 1.e+5*std::abs(jet->phi());
  m_rand.SetSeed(seed);

  // Calculate the smearing factor
  double smear = m_rand.Gaus(1.0, sigma);
  if(smear > 0) return smear;

  // If smear factor is negative, then retry the calculation
  int tries = 1;
  const int maxTries = 20;
  while(smear <= 0 && tries <= maxTries){
    ATH_MSG_DEBUG("Negative smear factor, " << smear << ", after " <<
                  tries << " tries.");
    smear = m_rand.Gaus(1.0, sigma);
    tries++;
  }

  // If smear factor is still negative, something is wrong.
  if(smear <= 0){
    ATH_MSG_WARNING("Negative smear factor, " << smear << ", after " <<
                    tries << " tries.");
    ATH_MSG_WARNING("Please contact the JER tool developers");
  }

  return smear;
}
