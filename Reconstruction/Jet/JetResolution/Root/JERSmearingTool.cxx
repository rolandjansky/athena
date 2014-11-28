/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System includes
#include <cmath>

// Local includes
#include "JetResolution/JERSmearingTool.h"

// Other includes
#include "PATInterfaces/SystematicRegistry.h"

//-----------------------------------------------------------------------------
// Standard ASG tool constructor
//-----------------------------------------------------------------------------
JERSmearingTool::JERSmearingTool(const std::string& name)
  : asg::AsgTool(name),
    m_jerTool("JERTool/JERTool"),
    m_userSeed(0),
    m_jerSys("JER", 1),
    m_applyJERSyst(false)
{
  declareProperty("JERTool", m_jerTool);
  declareProperty("ApplyNominalSmearing", m_applyNominalSmearing = false,
                  "If set to false, smearing only applied for JER sys");
}

//-----------------------------------------------------------------------------
// Copy constructor for reflex in Athena.
// Implement a shallow copy for now. Come back to this later.
//-----------------------------------------------------------------------------
JERSmearingTool::JERSmearingTool(const JERSmearingTool& other)
  : asg::AsgTool(other.name() + "_copy"),
    m_jerTool(other.m_jerTool),
    m_applyNominalSmearing(other.m_applyNominalSmearing),
    m_rand(other.m_rand),
    m_userSeed(other.m_userSeed),
    m_jerSys(other.m_jerSys),
    m_applyJERSyst(other.m_applyJERSyst)
{}

//-----------------------------------------------------------------------------
// Initialize the tool
//-----------------------------------------------------------------------------
StatusCode JERSmearingTool::initialize()
{
  // Greet the user
  ATH_MSG_INFO("Initializing JER smearing tool");

  // Try to retrieve the JERTool
  if(m_jerTool.retrieve() != StatusCode::SUCCESS){
    ATH_MSG_ERROR("Cannot retrieve the JER tool!");
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
  if(m_applyJERSyst || m_applyNominalSmearing){
    // TODO: change these methods to return a StatusCode
    double smear = m_applyJERSyst ? getSmearingFactorSys(&jet)
                                  : getSmearingFactorNom(&jet);
    ATH_MSG_DEBUG("Applying smearing factor: " << smear);
    // Sanity check
    if(smear < 0) ATH_MSG_WARNING("Negative smear factor: " << smear);
    // No non-const accessor, so I need to copy the p4 to modify it
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
  // For now, support only 1-sigma variation of the resolution
  CP::SystematicSet result;
  result.insert(m_jerSys);
  return result;
}
//-----------------------------------------------------------------------------
CP::SystematicSet JERSmearingTool::recommendedSystematics() const
{ return affectingSystematics(); }

//-----------------------------------------------------------------------------
// Apply systematic configuration
//-----------------------------------------------------------------------------
CP::SystematicCode JERSmearingTool::applySystematicVariation
(const CP::SystematicSet& systematics)
{
  // As a first pass, just check to see if the JER systematic is present
  if(systematics.find(m_jerSys) != systematics.end()){
    m_applyJERSyst = true;
  }
  else m_applyJERSyst = false;
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
  ATH_MSG_DEBUG("Retrieved nominal jet resolution values: MC " << resMC
                << " Data " << resData << " SmearSigma " << sigma);
  return getSmearingFactor(jet, sigma);
}

//-----------------------------------------------------------------------------
// Get random smearing factor for evaluating jet energy resolution
// systematic uncertainty
//-----------------------------------------------------------------------------
double JERSmearingTool::getSmearingFactorSys(const xAOD::Jet* jet)
{
  double resMC = m_jerTool->getRelResolutionMC(jet);
  double resSys = resMC + m_jerTool->getUncertainty(jet);
  double sigma = sqrt(resSys*resSys - resMC*resMC);
  ATH_MSG_DEBUG("Retrieved systematic jet resolution values: MC " << resMC
                << " SysUp " << resSys << " SmearSigma " << sigma);
  return getSmearingFactor(jet, sigma);
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
    ATH_MSG_DEBUG("Negative smear factor, " << smear << ", after "
                  << tries << " tries.");
    smear = m_rand.Gaus(1.0, sigma);
    tries++;
  }

  // If smear factor is still negative, something is wrong.
  if(smear <= 0){
    ATH_MSG_WARNING("Negative smear factor, " << smear << ", after "
                    << tries << " tries.");
    ATH_MSG_WARNING("Please contact the JER tool developers");
  }

  return smear;
}
