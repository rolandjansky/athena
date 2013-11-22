/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL include(s):
#include <stdexcept>
#include <sstream>
#include <iostream>

// Local include(s):
#include "TrigMuonEfficiency/LeptonTriggerSF.h"
#include "ElectronEfficiencyCorrection/TElectronEfficiencyCorrectionTool.h"


// ROOT include(s):
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TDirectory.h"
#include "TLorentzVector.h"
#include "TH2.h"

//
// Constant declaration(s):
//
static const double commonSystMTSG = 0.01;
static const double muon_barrel_endcap_boundary = 1.05;
static const double electron_maximum_eta = 2.47;
static const double GeVtoMeV = 1000.;



/// Helper macro for printing nice error messages
/**
 * This macro is used to print some pretty error messages that specify
 * the file name and line number. This can be very useful when debugging
 * problems.
 *
 * It can be placed into a ROOT print function like this:
 *
 * <code>
 *  ::Error( "ThisFunction", ERROR_MESSAGE( "My message with an int %i" ), myInt );
 * </code>
 *
 * @param MESSAGE The message to "make pretty"
 */
#define ERROR_MESSAGE( MESSAGE )                \
   "%s:%i " MESSAGE, __FILE__, __LINE__



/*=================================================================* 
 * constructor/destructor
 *=================================================================*/
LeptonTriggerSF::LeptonTriggerSF(const std::string& directory,
                                 const std::string& directoryElectron,
                                 const std::string& electronFileVersion)
  : m_year(2011),
    m_efficiencyMap(),
    m_phiBoundaryBarrel(0.0),
    m_phiBoundaryEndcap(0.0),
    m_IsAFII(false)
{
  m_electronFileVersion = electronFileVersion;
  initialise(directory, "", std::vector<std::string>(), directoryElectron);
}



LeptonTriggerSF::LeptonTriggerSF(const int year,
                                 const std::string& directory,
                                 const std::string& fileName,
                                 const std::string& directoryElectron,
                                 const std::string& electronFileVersion)
  : m_year(year),
    m_efficiencyMap(),
    m_phiBoundaryBarrel(0.0),
    m_phiBoundaryEndcap(0.0),
    m_IsAFII(false)
{
  m_electronFileVersion = electronFileVersion;
  initialise(directory, fileName, std::vector<std::string>(), directoryElectron);
}



LeptonTriggerSF::LeptonTriggerSF(const int year,
                                 const std::vector<std::string>& qualities,
                                 const std::string& directory,
                                 const std::string& fileName,
                                 const std::string& directoryElectron,
                                 const std::string& electronFileVersion)
  : m_year(year),
    m_efficiencyMap(),
    m_phiBoundaryBarrel(0.0),
    m_phiBoundaryEndcap(0.0),
    m_IsAFII(false)
{
  m_electronFileVersion = electronFileVersion;
  initialise(directory, fileName, qualities, directoryElectron);
}



LeptonTriggerSF::~LeptonTriggerSF()
{
  for (EfficiencyMap::const_iterator cit = m_efficiencyMap.begin();
       cit != m_efficiencyMap.end();
       cit++) {
    delete cit->second;
  }

  for (ElectronEfficiencyToolMap::const_iterator cit = m_electronSF.begin();
       cit != m_electronSF.end();
       cit++) {
    delete cit->second;
  }
}



/*=================================================================* 
 * public methods
 *=================================================================*/
std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const int runNumber,
                              const bool useGeV,
                              const std::vector<TLorentzVector>& muons, 
                              const MuonQuality muonQuality,
                              const int systematicVariation,
                              TrigMuonEff::Configuration* config) const
{
  std::vector< MuonQuality > muonsQuality(muons.size(), muonQuality);
  std::vector<TLorentzVector> electrons;
  std::vector<ElectronQuality> electronsQuality;

  TrigMuonEff::Configuration configuration;
  configuration.runNumber = runNumber;
  configuration.useGeV = useGeV;
  configuration.setByUser = false;
  configuration.isAFII = m_IsAFII;
  configuration.systematics = systematicVariation;

  if (config) {
    configuration.setByUser = true;
    configuration.trigger = config->trigger;
    configuration.period = config->period;
    configuration.binning = config->binning;
  }

  return GetTriggerSF(configuration, muons, muonsQuality, electrons, electronsQuality);
}



std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const int runNumber,
                              const bool useGeV,
                              const std::vector<TLorentzVector>& muons,
                              const std::vector<MuonQuality>& muonsQuality,
                              const int systematicVariation,
                              TrigMuonEff::Configuration* config) const
{
  std::vector<TLorentzVector> electrons;
  std::vector<ElectronQuality> electronsQuality;

  TrigMuonEff::Configuration configuration;
  configuration.runNumber = runNumber;
  configuration.useGeV = useGeV;
  configuration.setByUser = false;
  configuration.isAFII = m_IsAFII;
  configuration.systematics = systematicVariation;

  if (config) {
    configuration.setByUser = true;
    configuration.trigger = config->trigger;
    configuration.period = config->period;
    configuration.binning = config->binning;
  }

  return GetTriggerSF(configuration, muons, muonsQuality, electrons, electronsQuality);
}



std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const int runNumber,
                              const bool useGeV,
                              const std::vector<TLorentzVector>& muons,
                              const std::vector<MuonQuality>& muonsQuality,
                              const std::string& trigger,
                              const int systematicVariation,
                              TrigMuonEff::Configuration* config) const
{
  TrigMuonEff::Configuration configuration;
  configuration.runNumber = runNumber;
  configuration.useGeV = useGeV;
  configuration.systematics = systematicVariation;
  configuration.setByUser = false;
  configuration.isAFII = m_IsAFII;

  if (config) {
    configuration.setByUser = true;
    configuration.trigger = config->trigger;
    configuration.period = config->period;
    configuration.binning = config->binning;
  }

  return GetTriggerSF(configuration, muons, muonsQuality, trigger);
}

std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const int runNumber,
                              const bool useGeV,
                              const std::vector<TLorentzVector>& electrons,
                              const std::vector<ElectronQuality>& electronsQuality,
                              const std::string& trigger,
                              const int systematicVariation,
                              TrigMuonEff::Configuration* config) const
{
  TrigMuonEff::Configuration configuration;
  configuration.runNumber = runNumber;
  configuration.useGeV = useGeV;
  configuration.systematics = systematicVariation;
  configuration.setByUser = false;
  configuration.isAFII = m_IsAFII;

  if (config) {
    configuration.setByUser = true;
    configuration.trigger = config->trigger;
    configuration.period = config->period;
    configuration.binning = config->binning;
  }

  return GetTriggerSF(configuration, electrons, electronsQuality, trigger);
}



std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const int runNumber,
                              const bool useGeV,
                              const std::vector<TLorentzVector>& muons,
                              const MuonQuality muonQuality,
                              const std::vector<TLorentzVector>& electrons,
                              const ElectronQuality electronQuality,
                              const int systematicVariation,
                              TrigMuonEff::Configuration* config) const
{
  std::vector<MuonQuality> muonsQuality(muons.size(), muonQuality);
  std::vector<ElectronQuality> electronsQuality(electrons.size(), electronQuality);

  TrigMuonEff::Configuration configuration;
  configuration.runNumber = runNumber;
  configuration.useGeV = useGeV;
  configuration.setByUser = false;
  configuration.isAFII = m_IsAFII;
  configuration.systematics = systematicVariation;

  if (config) {
    configuration.setByUser = true;
    configuration.trigger = config->trigger;
    configuration.period = config->period;
    configuration.binning = config->binning;
  }

  return GetTriggerSF(configuration, muons, muonsQuality, electrons, electronsQuality);
}



std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const int runNumber,
                              const bool useGeV,
                              const std::vector<TLorentzVector>& muons,
                              const std::vector<MuonQuality>& muonsQuality,
                              const std::vector<TLorentzVector>& electrons,
                              const std::vector<ElectronQuality>& electronsQuality,
                              const int systematicVariation,
                              TrigMuonEff::Configuration* config) const
{
  TrigMuonEff::Configuration configuration;
  configuration.runNumber = runNumber;
  configuration.useGeV = useGeV;
  configuration.systematics = systematicVariation;
  configuration.setByUser = false;
  configuration.isAFII = m_IsAFII;

  if (config) {
    configuration.setByUser = true;
    configuration.trigger = config->trigger;
    configuration.period = config->period;
    configuration.binning = config->binning;
  }

  return GetTriggerSF(configuration, muons, muonsQuality, electrons, electronsQuality);
}


std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const int runNumber,
                              const bool useGeV,
                              const std::vector<TLorentzVector>& muons,
                              const std::vector<MuonQuality>& muonsQuality,
                              const std::vector<TLorentzVector>& electrons,
                              const std::vector<ElectronQuality>& electronsQuality,
			      const std::string& trigger,
                              const int systematicVariation,
                              TrigMuonEff::Configuration* config) const
{
  TrigMuonEff::Configuration configuration;
  configuration.runNumber = runNumber;
  configuration.useGeV = useGeV;
  configuration.systematics = systematicVariation;
  configuration.setByUser = false;
  configuration.isAFII = m_IsAFII;
  configuration.trigger = trigger;

  if (config) {
    configuration.setByUser = true;
    configuration.trigger = config->trigger;
    configuration.period = config->period;
    configuration.binning = config->binning;
  }

  return GetTriggerSF(configuration, muons, muonsQuality, electrons, electronsQuality);
}


std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const TrigMuonEff::Configuration    & configuration,
                              const std::vector< TLorentzVector > & muons,
                              const std::vector< MuonQuality >    & muonsQuality,
                              const std::vector< TLorentzVector > & electrons,
                              const std::vector< ElectronQuality >& electronsQuality) const
{
  // Get the offline muon and electron thresholds:
  const std::pair< double, double > threshold = getThresholds(configuration);

  double rate_not_fired_data = 1.;
  double rate_not_fired_mc   = 1.;

  // needed for uncertainty calculation
  double inv_sq_eff_data    = 0.;
  double inv_sq_eff_mc      = 0.;
  double inv_sq_eff_data_el = 0.;
  double inv_sq_eff_mc_el   = 0.;
  double inv_sq_eff_data_mu = 0.;
  double inv_sq_eff_mc_mu   = 0.;

  DileptonTriggerName dilep_name = kNoTrig;
  const bool is_e12Tvh_medium1_mu8 = (configuration.trigger.find("e12Tvh_medium1_mu8") != std::string::npos);
  if(is_e12Tvh_medium1_mu8) dilep_name = EF_e12Tvh_medium1_mu8;
  for (size_t ielec = 0; ielec < electrons.size(); ++ielec) {

    const int set_data = getElectronQuality(configuration.runNumber, electronsQuality.at(ielec), true,  (dilep_name!=kNoTrig), dilep_name);
    const int set_mc   = getElectronQuality(configuration.runNumber, electronsQuality.at(ielec), false, (dilep_name!=kNoTrig), dilep_name);

    double eff_data = 0., eff_mc = 0.;    
    double err_data = 0., err_mc = 0.;    

    if (((electrons.at(ielec)).Pt() < threshold.second) or
        (fabs((electrons.at(ielec)).Eta()) >= electron_maximum_eta)) {

      eff_data = 0.;
      eff_mc   = 0.;
      err_data = 0.;
      err_mc   = 0.;

    } else {

      // get efficiency from data
      std::pair<double, double> rc;
      rc = ElEff_Data(electrons.at(ielec), set_mc, set_data, m_year, configuration);
      eff_data = rc.first;
      err_data = rc.second;

      // get efficiency from MC
      rc = ElEff_MC(electrons.at(ielec), set_mc, m_year, configuration);
      eff_mc = rc.first;
      err_mc = rc.second;

    }
    if (eff_data==1.0) eff_data=1.0-1.0E-10; // to avoid division by 0 
    if (eff_mc==1.0) eff_mc=1.0-1.0E-10; // to avoid division by 0 

    rate_not_fired_data *= (1. - eff_data);
    rate_not_fired_mc   *= (1. - eff_mc);

    // needed for uncertainty calculation
    if (eff_data) {
      const double c = err_data/( 1. - eff_data );
      inv_sq_eff_data += c*c;
      inv_sq_eff_data_el += c*c; // contribution only due to electron trigger
    }

    if (eff_mc) {
      const double c = err_mc/( 1. - eff_mc );
      inv_sq_eff_mc += c*c;
      inv_sq_eff_mc_el += c*c; // contribution only due to electron trigger
    }
  }

  for (size_t imuon = 0; imuon < muons.size(); ++imuon) {

    double eff_data = 0., eff_mc = 0.;
    double err_data = 0., err_mc = 0.;

    if ((muons.at(imuon)).Pt() < threshold.first) {

      eff_data = 0.;
      eff_mc   = 0.;
      err_data = 0.;
      err_mc   = 0.;

    } else {

      if (not configuration.setByUser) {
        if (not setConfiguration(const_cast<TrigMuonEff::Configuration&>(configuration))) {
          return std::make_pair( 0., 0. );
        }
      }


      if (muonsQuality.at(imuon) == CaloMuon) continue;
      std::string muon_trigger_name = "";
      if(is_e12Tvh_medium1_mu8) muon_trigger_name = "mu8";
      // get efficiency from data
      configuration.isData = true;
      std::pair<double, double> rc;
      rc = getMuonEfficiency(configuration, muons.at(imuon), muonsQuality.at(imuon), muon_trigger_name);
      eff_data = rc.first;
      err_data = rc.second;

      // get efficiency from MC
      configuration.isData = false;
      rc = getMuonEfficiency(configuration, muons.at(imuon), muonsQuality.at(imuon), muon_trigger_name);
      eff_mc = rc.first;
      err_mc = rc.second;

    }

    rate_not_fired_data *= ( 1. - eff_data );
    rate_not_fired_mc   *= ( 1. - eff_mc );

    // needed for uncertainty calculation
    if (eff_data) {
      const double c = err_data/( 1. - eff_data );
      inv_sq_eff_data += c*c;
      inv_sq_eff_data_mu += c*c; // contribution only due to muon trigger
    }

    if (eff_mc) {
      const double c = err_mc/( 1. - eff_mc );
      inv_sq_eff_mc += c*c;
      inv_sq_eff_mc_mu += c*c; // contribution only due to muon trigger
    }
  }

  double event_SF = 1.;
  double event_SF_err = 0.;
  double event_SF_err_el = 0.;
  double event_SF_err_mu = 0.;

  // prevent events with no triggered electrons or muons
  if ((electrons.size() or muons.size()) and
      (fabs(1. - rate_not_fired_mc) > 0.0001)) {

    event_SF        = ( 1. - rate_not_fired_data ) / ( 1. - rate_not_fired_mc );

    event_SF_err    = getSfError( rate_not_fired_data, rate_not_fired_mc,
                                  inv_sq_eff_data, inv_sq_eff_mc );
    event_SF_err_el = getSfError( rate_not_fired_data, rate_not_fired_mc,
                                  inv_sq_eff_data_el, inv_sq_eff_mc_el );
    event_SF_err_mu = getSfError( rate_not_fired_data, rate_not_fired_mc,
                                    inv_sq_eff_data_mu, inv_sq_eff_mc_mu );
  }
  

  switch (configuration.systematics) {
    case noVariation:
    case statisticalOnlyMuon:
      return std::make_pair( event_SF, event_SF_err ); 

    case plusOneSigma:
      return std::make_pair( event_SF + event_SF_err, event_SF_err );

    case minusOneSigma:
      return std::make_pair( event_SF - event_SF_err, event_SF_err ); 

    case plusOneSigmaElectron:
      return std::make_pair( event_SF + event_SF_err_el, event_SF_err ); 

    case minusOneSigmaElectron:
      return std::make_pair( event_SF - event_SF_err_el, event_SF_err ); 

    case plusOneSigmaMuon:
      return std::make_pair( event_SF + event_SF_err_mu, event_SF_err ); 

    case minusOneSigmaMuon:
      return std::make_pair( event_SF - event_SF_err_mu, event_SF_err ); 

    default:
      ::Warning("LeptonTriggerSF::GetTriggerSF",
                "unknown systematics option '%i' given: no systematic variation used", configuration.systematics);
      return std::make_pair( event_SF, event_SF_err ); 
  }
}



std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const TrigMuonEff::Configuration& configuration,
                              const std::vector<TLorentzVector>& muons,
                              const std::vector<MuonQuality>& muonsQuality,
                              const std::string& trigger) const
{
  if (muons.size() != 2) {
    ::Fatal("LeptonTriggerSF::GetTriggerSF",
            ERROR_MESSAGE("Currently dimuon trigger chains only implemented for events with exactly 2 muons."));
    throw std::runtime_error("Muon vector has wrong size");
  }

  double eff_data = 0., eff_mc = 0.;
  double eff_data_err = 0., eff_mc_err = 0.;
    
  if (not configuration.setByUser) {
    if (not setConfiguration(const_cast<TrigMuonEff::Configuration&>(configuration))) {
      return std::make_pair( 0., 0. );
    }
  }

  const bool is_mu24i_tight_or_mu36_tight = (trigger.find("mu24i_tight_or_mu36_tight") != std::string::npos);
  const bool is_mu18_tight_mu8_EFFS = (trigger.find("mu18_tight_mu8_EFFS") != std::string::npos);
  const bool is_2mu13 = (trigger.find("2mu13") != std::string::npos);

  // get efficiency from data
  configuration.isData = true;
  if ((is_mu24i_tight_or_mu36_tight and is_mu18_tight_mu8_EFFS) or
      (is_mu24i_tight_or_mu36_tight and is_2mu13)) { // single OR dimuon
    eff_data = getSingleOrDimuonEfficiency(configuration, muons, muonsQuality, trigger).first;    
    eff_data_err = getSingleOrDimuonEfficiency(configuration, muons, muonsQuality, trigger).second;    

  } else if (is_mu18_tight_mu8_EFFS or is_2mu13) {
    eff_data = getDimuonEfficiency(configuration, muons, muonsQuality, trigger).first; // dimuon only
    eff_data_err = getDimuonEfficiency(configuration, muons, muonsQuality, trigger).second;

  } else {
    ::Fatal("LeptonTriggerSF::GetTriggerSF",
            ERROR_MESSAGE("unknown trigger combination: %s"), trigger.c_str());
    throw std::runtime_error("unknown trigger combination");
  }

  /// get efficiency from MC
  configuration.isData = false;
  if ((is_mu24i_tight_or_mu36_tight and is_mu18_tight_mu8_EFFS) or
      (is_mu24i_tight_or_mu36_tight and is_2mu13)) { // single OR dimuon
    eff_mc = getSingleOrDimuonEfficiency(configuration, muons, muonsQuality, trigger).first;
    eff_mc_err = getSingleOrDimuonEfficiency(configuration, muons, muonsQuality, trigger).second;

  } else if (is_mu18_tight_mu8_EFFS or is_2mu13) {
    eff_mc = getDimuonEfficiency(configuration, muons, muonsQuality, trigger).first; // dimuon only     
    eff_mc_err = getDimuonEfficiency(configuration, muons, muonsQuality, trigger).second;
  }

  double event_SF = 1.;
  double event_SF_err = 0.;
    
  // prevent events with no triggered muons
  if (( muons.size()) and (fabs(eff_mc) > 0.0001)){
    const double d = eff_data;
    const double de = eff_data_err;
    const double m = eff_mc;
    const double me = eff_mc_err;
    event_SF = d / m;
    event_SF_err = sqrt( ( de*de / m*m ) + ( d*d*me*me / m*m*m*m ) );
  }
  /*std::cout << "eff_data = " << eff_data << ", eff_mc = " << eff_mc << std::endl;
  std::cout << "Muon 1: pT = " << muons[0].Pt() << ", eta = " << muons[0].Eta() << ", phi = " << muons[0].Phi() << std::endl;
  std::cout << "Muon 2: pT = " << muons[1].Pt() << ", eta = " << muons[1].Eta() << ", phi = " << muons[1].Phi() << std::endl;*/

  switch (configuration.systematics) {
  case noVariation:
  case statisticalOnlyMuon:
    return std::make_pair( event_SF, event_SF_err ); 

  case plusOneSigma:
    return std::make_pair( event_SF + event_SF_err, event_SF_err );

  case minusOneSigma:
    return std::make_pair( event_SF - event_SF_err, event_SF_err ); 

  default:
    ::Warning("LeptonTriggerSF::GetTriggerSF",
	      "unknown systematics option '%i' given: no systematic variation used", configuration.systematics);
    return std::make_pair( event_SF, event_SF_err ); 
  }
}

std::pair<double, double>
LeptonTriggerSF::GetTriggerSF(const TrigMuonEff::Configuration& configuration,
                              const std::vector<TLorentzVector>& electrons,
                              const std::vector<ElectronQuality>& electronsQuality,
                              const std::string& trigger) const
{
  if (electrons.size() != 2) {
    ::Fatal("LeptonTriggerSF::GetTriggerSF",
            ERROR_MESSAGE("Currently dielectron trigger chains only implemented for events with exactly 2 electrons."));
    throw std::runtime_error("Electron vector has wrong size");
  }

  double eff_data = 0., eff_mc = 0.;
  double eff_data_err = 0., eff_mc_err = 0.;
    
  if (not configuration.setByUser) {
    if (not setConfiguration(const_cast<TrigMuonEff::Configuration&>(configuration))) {
      return std::make_pair( 0., 0. );
    }
  }

  const bool is_e24vhi_medium1_or_e60_medium1 = (trigger.find("e24vhi_medium1_or_e60_medium1") != std::string::npos);
  const bool is_2e12Tvh_loose1 = (trigger.find("2e12Tvh_loose1") != std::string::npos);

  // get efficiency from data
  configuration.isData = true;
  if ((is_e24vhi_medium1_or_e60_medium1 and is_2e12Tvh_loose1)) { // single OR dielectron
    eff_data = getSingleOrDielectronEfficiency(configuration, electrons, electronsQuality, trigger).first;     
    eff_data_err = getSingleOrDielectronEfficiency(configuration, electrons, electronsQuality, trigger).second;     

  } else if (is_2e12Tvh_loose1) { // dielectron only
    eff_data = getDielectronEfficiency(configuration, electrons, electronsQuality, trigger).first;
    eff_data_err = getDielectronEfficiency(configuration, electrons, electronsQuality, trigger).second;

  } else {
    ::Fatal("LeptonTriggerSF::GetTriggerSF",
            ERROR_MESSAGE("unknown trigger combination: %s"), trigger.c_str());
    throw std::runtime_error("unknown trigger combination");
  }

  /// get efficiency from MC
  configuration.isData = false;
  if ((is_e24vhi_medium1_or_e60_medium1 and is_2e12Tvh_loose1)) { // single OR dielectron
    eff_mc = getSingleOrDielectronEfficiency(configuration, electrons, electronsQuality, trigger).first;
    eff_mc_err = getSingleOrDielectronEfficiency(configuration, electrons, electronsQuality, trigger).second;

  } else if (is_2e12Tvh_loose1) { // dimuon only     
    eff_mc = getDielectronEfficiency(configuration, electrons, electronsQuality, trigger).first;
    eff_mc_err = getDielectronEfficiency(configuration, electrons, electronsQuality, trigger).second;
  }

  double event_SF = 1.;
  double event_SF_err = 0.;
    
  // prevent events with no triggered electrons
  if (( electrons.size()) and (fabs(eff_mc) > 0.0001)){
    const double d = eff_data;
    const double de = eff_data_err;
    const double m = eff_mc;
    const double me = eff_mc_err;
    event_SF = d / m;
    event_SF_err = sqrt( ( de*de / m*m ) + ( d*d*me*me / m*m*m*m ) );
  }
  /*std::cout << "eff_data = " << eff_data << ", eff_mc = " << eff_mc << std::endl;
  std::cout << "Electron 1: pT = " << electrons[0].Pt() << ", eta = " << electrons[0].Eta() << ", phi = " << electrons[0].Phi() << std::endl;
  std::cout << "Electron 2: pT = " << electrons[1].Pt() << ", eta = " << electrons[1].Eta() << ", phi = " << electrons[1].Phi() << std::endl;*/

  switch (configuration.systematics) {
  case noVariation:
    return std::make_pair( event_SF, event_SF_err ); 
    
  case plusOneSigma:
    return std::make_pair( event_SF + event_SF_err, event_SF_err );
    
  case minusOneSigma:
    return std::make_pair( event_SF - event_SF_err, event_SF_err ); 
    
  default:
    ::Warning("LeptonTriggerSF::GetTriggerSF",
	      "unknown systematics option '%i' given: no systematic variation used", configuration.systematics);
    return std::make_pair( event_SF, event_SF_err ); 
  }
}



std::pair<double, double>
LeptonTriggerSF::getMuonEfficiency(const TrigMuonEff::Configuration& configuration,
                                   const TLorentzVector& muon,
                                   const MuonQuality muonQuality,
                                   const std::string& trigger) const
{
  if (muonQuality == CaloMuon) return std::make_pair(0.0, 0.0);

  const double mu_eta = muon.Eta();
  const double mu_phi = fixPhiRange(muon);

  const std::string type = (configuration.isData ? "_data" : "_mc");
  const std::string region = ((fabs(mu_eta) < muon_barrel_endcap_boundary) ? "_barrel" : "_endcap" );
  const std::string quality = getMuonQuality(muonQuality);
  const std::string chain = (trigger.empty() ? configuration.trigger : trigger);

  const std::string histname = "_MuonTrigEff_" + configuration.period + ((m_year > 2011) ? chain + "_" : "") + quality +
                               "_EtaPhi_" + configuration.binning + region + type;

  const EfficiencyMap* map = &m_efficiencyMap;

  if (configuration.replicaIndex >= 0) {
    if (configuration.replicaIndex < (int) m_efficiencyMapReplicaArray.size()) {
      map = &m_efficiencyMapReplicaArray.at(configuration.replicaIndex);

    } else {
      ::Fatal("LeptonTriggerSF::getMuonEfficiency",
              ERROR_MESSAGE("index for replicated histograms is out of range. %d"), configuration.replicaIndex);
      throw std::runtime_error( "Internal coding error detected" );
    }
  }

  EfficiencyMap::const_iterator cit = map->find(histname);
  if (cit == map->end()) {
    ::Fatal("LeptonTriggerSF::getMuonEfficiency",
            ERROR_MESSAGE("Could not find what you are looking for in the efficiency map. \"%s\" "
                          "This is either a bug or you are not using the right .root file, please double check."),
                          histname.c_str() );
    throw std::runtime_error( "Internal coding error detected" );
  }

  const int bin = (cit->second)->FindFixBin(mu_eta, mu_phi);
  const double efficiency = (cit->second)->GetBinContent(bin);
  const double statisticalError = (cit->second)->GetBinError(bin);

  const double systematicError = commonSystMTSG * efficiency;
  double uncertainty = 0;
  switch (configuration.systematics) {
    case statisticalOnlyMuon:
      uncertainty = statisticalError;
      break;

    case systematicOnlyMuon:
      uncertainty = systematicError;
      break;

    default:
      uncertainty = sqrt(statisticalError*statisticalError + systematicError*systematicError);
      //std::cout << "Error: " << efficiency << " Stat error: " << statisticalError << " SYst: " << systematicError << std::endl;
      break;
  };

  return std::make_pair(efficiency, uncertainty);
}



std::pair<double, double>
LeptonTriggerSF::getDimuonEfficiency(const TrigMuonEff::Configuration& configuration,
                                     const std::vector<TLorentzVector>& muons,
                                     const std::vector<MuonQuality>& muonsQuality,
                                     const std::string& chain) const
{
  DileptonThresholds thresholds;
  getDileptonThresholds(configuration, thresholds);

  double threshold_leg1 = 0.;
  double threshold_leg2 = 0.;

  if (chain.find("mu18_tight_mu8_EFFS") != std::string::npos) {
    threshold_leg1 = thresholds.mu18_tight;
    threshold_leg2 = thresholds.mu8_EFFS;

  } else if (chain.find("2mu13") != std::string::npos) {
    threshold_leg1 = thresholds.mu13;
    threshold_leg2 = thresholds.mu13;

  } else {
    ::Error("LeptonTriggerSF::getDimuonEfficiency",
      ERROR_MESSAGE("Invalid dimuon or combination of single and dimuon trigger chain name given"));
  }

  double eff_mu1_leg1 = 0.;
  double eff_mu2_leg1 = 0.;
  double eff_mu1_leg2 = 0.;
  double eff_mu2_leg2 = 0.;
  double eff_mu1_bothlegs = 0.;
  double eff_mu2_bothlegs = 0.;
  double eff_mu1_leg1_err = 0.;
  double eff_mu2_leg1_err = 0.;
  double eff_mu1_leg2_err = 0.;
  double eff_mu2_leg2_err = 0.;
  double eff_mu1_bothlegs_err = 0.;
  double eff_mu2_bothlegs_err = 0.;

  DileptonTrigger dimuon;
  getDileptonLegs(chain, dimuon);

  enum {
    Leg1 = 0,
    Leg2 = 1
  };

  if ((muons.at(Leg1)).Pt() >= threshold_leg1) {
    eff_mu1_leg1 = getMuonEfficiency(configuration, muons.at(Leg1), muonsQuality.at(Leg1), dimuon.leg1).first;
    eff_mu1_leg1_err = getMuonEfficiency(configuration, muons.at(Leg1), muonsQuality.at(Leg1), dimuon.leg1).second;
    eff_mu1_bothlegs = getMuonEfficiency(configuration, muons.at(Leg1), muonsQuality.at(Leg1), dimuon.bothLegs).first;
    eff_mu1_bothlegs_err = getMuonEfficiency(configuration, muons.at(Leg1), muonsQuality.at(Leg1), dimuon.bothLegs).second;
  }

  if ((muons.at(Leg2)).Pt() >= threshold_leg1) {
    eff_mu2_leg1 = getMuonEfficiency(configuration, muons.at(Leg2), muonsQuality.at(Leg2), dimuon.leg1).first;
    eff_mu2_leg1_err = getMuonEfficiency(configuration, muons.at(Leg2), muonsQuality.at(Leg2), dimuon.leg1).second;
    eff_mu2_bothlegs = getMuonEfficiency(configuration, muons.at(Leg2), muonsQuality.at(Leg2), dimuon.bothLegs).first;
    eff_mu2_bothlegs_err = getMuonEfficiency(configuration, muons.at(Leg2), muonsQuality.at(Leg2), dimuon.bothLegs).second;
  }

  if ((muons.at(Leg1)).Pt() >= threshold_leg2){
    eff_mu1_leg2 = getMuonEfficiency(configuration, muons.at(Leg1), muonsQuality.at(Leg1), dimuon.leg2).first;
    eff_mu1_leg2_err = getMuonEfficiency(configuration, muons.at(Leg1), muonsQuality.at(Leg1), dimuon.leg2).second;
  }
  if ((muons.at(Leg2)).Pt() >= threshold_leg2){ 
    eff_mu2_leg2 = getMuonEfficiency(configuration, muons.at(Leg2), muonsQuality.at(Leg2), dimuon.leg2).first;
    eff_mu2_leg2_err = getMuonEfficiency(configuration, muons.at(Leg2), muonsQuality.at(Leg2), dimuon.leg2).second;
  }

  /*if(configuration.isData){
    std::cout << "DATA: di_eff_mu1 = " << eff_mu1_leg1 << std::endl;
    std::cout << "DATA: di_eff_mu2 = " << eff_mu2_leg1 << std::endl;
  } else{
    std::cout << "MC: di_eff_mu1 = " << eff_mu1_leg1 << std::endl;
    std::cout << "MC: di_eff_mu2 = " << eff_mu2_leg1 << std::endl;
    }*/

  const double thedieff = eff_mu1_leg1*eff_mu2_leg2 + eff_mu2_leg1*eff_mu1_leg2 - eff_mu1_bothlegs*eff_mu2_bothlegs;
  const double thedieff_err = getDiError(eff_mu1_leg1, eff_mu1_leg1_err,
					 eff_mu2_leg1, eff_mu2_leg1_err,
					 eff_mu1_leg2, eff_mu1_leg2_err,
					 eff_mu2_leg2, eff_mu2_leg2_err,
					 eff_mu1_bothlegs, eff_mu1_bothlegs_err,
					 eff_mu2_bothlegs, eff_mu2_bothlegs_err);

  return std::make_pair(thedieff, thedieff_err);

}

std::pair<double, double>
LeptonTriggerSF::getDielectronEfficiency(const TrigMuonEff::Configuration& configuration,
                                     const std::vector<TLorentzVector>& electrons,
                                     const std::vector<ElectronQuality>& electronsQuality,
                                     const std::string& chain) const
{
  DileptonThresholds thresholds;
  getDileptonThresholds(configuration, thresholds);

  double threshold_leg1 = 0.;
  double threshold_leg2 = 0.;
  DileptonTriggerName dilep_name = kNoTrig;
  if (chain.find("2e12Tvh_loose1") != std::string::npos) {
    threshold_leg1 = thresholds.e12Tvh_loose1;
    threshold_leg2 = thresholds.e12Tvh_loose1;
    dilep_name = EF_2e12Tvh_loose1;
  } else {
    ::Error("LeptonTriggerSF::getDielectronEfficiency",
      ERROR_MESSAGE("Invalid dielectron or combination of single and dielectron trigger chain name given"));
  }

  double eff_el1_leg1 = 0.;
  double eff_el2_leg1 = 0.;
  double eff_el1_leg2 = 0.;
  double eff_el2_leg2 = 0.;
  double eff_el1_bothlegs = 0.;
  double eff_el2_bothlegs = 0.;
  double eff_el1_leg1_err = 0.;
  double eff_el2_leg1_err = 0.;
  double eff_el1_leg2_err = 0.;
  double eff_el2_leg2_err = 0.;
  double eff_el1_bothlegs_err = 0.;
  double eff_el2_bothlegs_err = 0.;

  DileptonTrigger dielectron;
  getDileptonLegs(chain, dielectron);

  enum {
    Leg1 = 0,
    Leg2 = 1
  };

  const int set_data_leg1 = getElectronQuality(configuration.runNumber, electronsQuality.at(Leg1), true,  true, dilep_name);
  const int set_mc_leg1   = getElectronQuality(configuration.runNumber, electronsQuality.at(Leg1), false, true, dilep_name);
  const int set_data_leg2 = getElectronQuality(configuration.runNumber, electronsQuality.at(Leg2), true,  true, dilep_name);
  const int set_mc_leg2   = getElectronQuality(configuration.runNumber, electronsQuality.at(Leg2), false, true, dilep_name);

  if(configuration.isData){
    if ((electrons.at(Leg1)).Pt() >= threshold_leg1) {
      // these two lines are the same for the electron case for now, but coded so that implementation of asymmetric dielectron trigger will be easier
      eff_el1_leg1 = ElEff_Data(electrons.at(Leg1), set_mc_leg1, set_data_leg1, m_year, configuration).first;
      eff_el1_leg1_err = ElEff_Data(electrons.at(Leg1), set_mc_leg1, set_data_leg1, m_year, configuration).second;
      eff_el1_bothlegs = ElEff_Data(electrons.at(Leg1), set_mc_leg1, set_data_leg1, m_year, configuration).first;
      eff_el1_bothlegs_err = ElEff_Data(electrons.at(Leg1), set_mc_leg1, set_data_leg1, m_year, configuration).second;
    }

    if ((electrons.at(Leg2)).Pt() >= threshold_leg1) {
      eff_el2_leg1 = ElEff_Data(electrons.at(Leg2), set_mc_leg2, set_data_leg2, m_year, configuration).first;
      eff_el2_leg1_err = ElEff_Data(electrons.at(Leg2), set_mc_leg2, set_data_leg2, m_year, configuration).second;
      eff_el2_bothlegs = ElEff_Data(electrons.at(Leg2), set_mc_leg2, set_data_leg2, m_year, configuration).first;
      eff_el2_bothlegs_err = ElEff_Data(electrons.at(Leg2), set_mc_leg2, set_data_leg2, m_year, configuration).second;
    }

    if ((electrons.at(Leg1)).Pt() >= threshold_leg2){
      eff_el1_leg2 = ElEff_Data(electrons.at(Leg1), set_mc_leg1, set_data_leg1, m_year, configuration).first;
      eff_el1_leg2_err = ElEff_Data(electrons.at(Leg1), set_mc_leg1, set_data_leg1, m_year, configuration).second;
    }
    if ((electrons.at(Leg2)).Pt() >= threshold_leg2){
      eff_el2_leg2 = ElEff_Data(electrons.at(Leg2), set_mc_leg2, set_data_leg2, m_year, configuration).first;
      eff_el2_leg2_err = ElEff_Data(electrons.at(Leg2), set_mc_leg2, set_data_leg2, m_year, configuration).second;
    }
  } else{
    if ((electrons.at(Leg1)).Pt() >= threshold_leg1) {
      eff_el1_leg1 = ElEff_MC(electrons.at(Leg1), set_mc_leg1, m_year, configuration).first;
      eff_el1_leg1_err = ElEff_MC(electrons.at(Leg1), set_mc_leg1, m_year, configuration).second;
      eff_el1_bothlegs = ElEff_MC(electrons.at(Leg1), set_mc_leg1, m_year, configuration).first;
      eff_el1_bothlegs_err = ElEff_MC(electrons.at(Leg1), set_mc_leg1, m_year, configuration).second;
    }

    if ((electrons.at(Leg2)).Pt() >= threshold_leg1) {
      eff_el2_leg1 = ElEff_MC(electrons.at(Leg2), set_mc_leg2, m_year, configuration).first;
      eff_el2_leg1_err = ElEff_MC(electrons.at(Leg2), set_mc_leg2, m_year, configuration).second;
      eff_el2_bothlegs = ElEff_MC(electrons.at(Leg2), set_mc_leg2, m_year, configuration).first;
      eff_el2_bothlegs_err = ElEff_MC(electrons.at(Leg2), set_mc_leg2, m_year, configuration).second;
    }

    if ((electrons.at(Leg1)).Pt() >= threshold_leg2){
      eff_el1_leg2 = ElEff_MC(electrons.at(Leg1), set_mc_leg1, m_year, configuration).first;
      eff_el1_leg2_err = ElEff_MC(electrons.at(Leg1), set_mc_leg1, m_year, configuration).second;
    }
    if ((electrons.at(Leg2)).Pt() >= threshold_leg2){
      eff_el2_leg2 = ElEff_MC(electrons.at(Leg2), set_mc_leg2, m_year, configuration).first;
      eff_el2_leg2_err = ElEff_MC(electrons.at(Leg2), set_mc_leg2, m_year, configuration).second;
    }
  }

  /*if(configuration.isData){
    std::cout << "DATA: di_eff_el1 = " << eff_el1_leg1 << std::endl;
    std::cout << "DATA: di_eff_el2 = " << eff_el2_leg1 << std::endl;
  } else{
    std::cout << "MC: di_eff_el1 = " << eff_el1_leg1 << std::endl;
    std::cout << "MC: di_eff_el2 = " << eff_el2_leg1 << std::endl;
    }*/

  const double thedieff = eff_el1_leg1*eff_el2_leg2 + eff_el2_leg1*eff_el1_leg2 - eff_el1_bothlegs*eff_el2_bothlegs;
  const double thedieff_err = getDiError(eff_el1_leg1, eff_el1_leg1_err,
					 eff_el2_leg1, eff_el2_leg1_err,
					 eff_el1_leg2, eff_el1_leg2_err,
					 eff_el2_leg2, eff_el2_leg2_err,
					 eff_el1_bothlegs, eff_el1_bothlegs_err,
					 eff_el2_bothlegs, eff_el2_bothlegs_err);

  return std::make_pair(thedieff, thedieff_err);

}


std::pair<double, double>
LeptonTriggerSF::getSingleOrDimuonEfficiency(const TrigMuonEff::Configuration& config,
                                             const std::vector<TLorentzVector>& muons,
                                             const std::vector<MuonQuality>& muonsQuality,
                                             const std::string& chain) const
{
  if (config.trigger.compare("mu24i_tight_or_EF_mu36_tight") != 0) {
    ::Fatal("LeptonTriggerSF::getSingleOrDimuonEfficiency",
            ERROR_MESSAGE("Currently dimuon trigger only with (mu24i_tight_or_mu_36_tight) trigger is supported: %s"),
            config.trigger.c_str());
    throw std::runtime_error("Currently dimuon trigger only with (mu24i_tight_or_mu_36_tight) trigger is supported");
  }

  const std::pair<double, double> threshold = getThresholds(config);

  DileptonThresholds thresholds;
  getDileptonThresholds(config, thresholds);

  double threshold_leg1 = 0.;
  double threshold_leg2 = 0.;

  if (chain.find("mu18_tight_mu8_EFFS") != std::string::npos) {
    threshold_leg1 = thresholds.mu18_tight;
    threshold_leg2 = thresholds.mu8_EFFS;

  } else if (chain.find("2mu13") != std::string::npos) {
    threshold_leg1 = thresholds.mu13;
    threshold_leg2 = thresholds.mu13;

  } else {
    ::Error("LeptonTriggerSF::getDimuonEfficiency",
      ERROR_MESSAGE("Invalid dimuon or combination of single and dimuon trigger chain name given"));
  }

  double single_eff_tmp = 0.;
  double dimuon_eff_tmp = 0.;
  double overlap_term = 0.;

  double mu1_eff = getMuonEfficiency(config, muons.at(0), muonsQuality.at(0)).first;
  double mu1_eff_err = getMuonEfficiency(config, muons.at(0), muonsQuality.at(0)).second;
  double mu2_eff = getMuonEfficiency(config, muons.at(1), muonsQuality.at(1)).first;
  double mu2_eff_err = getMuonEfficiency(config, muons.at(1), muonsQuality.at(1)).second;
  
  const bool is_mu18_tight_mu8_EFFS = (chain.find("mu18_tight_mu8_EFFS") != std::string::npos);
  const bool is_2mu13 = (chain.find("2mu13") != std::string::npos);

  if (is_mu18_tight_mu8_EFFS) {
    dimuon_eff_tmp = getDimuonEfficiency(config, muons, muonsQuality, "mu18_tight_mu8_EFFS").first;

  } else if (is_2mu13) {
    dimuon_eff_tmp = getDimuonEfficiency(config, muons, muonsQuality, "2mu13").first;

  } else {
    ::Error("LeptonTriggerSF::getSingleOrDimuonEfficiency",
      ERROR_MESSAGE("Invalid dimuon chain name given: %s"), chain.c_str());
    return std::make_pair(-1., -1.);
  }

  const bool is_mu1_above_threshold = ((muons.at(0)).Pt() >= threshold.first);
  const bool is_mu2_above_threshold = ((muons.at(1)).Pt() >= threshold.first);

  // all above threshold
  //if (is_mu1_above_threshold and is_mu2_above_threshold) {
  //
  //  if (is_mu18_tight_mu8_EFFS) {
  //    overlap_term = getMuonEfficiency(config, muons.at(0), muonsQuality.at(0), "mu18_tight").first*
  //                   getMuonEfficiency(config, muons.at(1), muonsQuality.at(1), "mu18_tight").first; // this still needs to be checked!
  //
  //  } else {
  //    // check whether mu13 or mu24i_tight_or_mu36_tight is more efficient
  //    double higher_mu1 = 0;
  //    double higher_mu2 = 0;
  //    double eff = getMuonEfficiency(config, muons.at(0), muonsQuality.at(0), "mu13").first;
  //    higher_mu1 = (mu1_eff > eff) ? mu1_eff : eff;
  //
  //    eff = getMuonEfficiency(config, muons.at(1), muonsQuality.at(1), "mu13").first;
  //    higher_mu2 = (mu2_eff > eff) ? mu2_eff : eff;
  //
  //    overlap_term = higher_mu1*higher_mu2;
  //  }
  //
  //// one above, one below threshold
  //} else if (((not is_mu1_above_threshold) and is_mu2_above_threshold) or
  //           ((not is_mu2_above_threshold) and is_mu1_above_threshold)) {
  //  if (is_mu1_above_threshold){
  //    mu2_eff = 0.;
  //    mu2_eff_err = 0.;
  //  }
  //  if (is_mu2_above_threshold){
  //    mu1_eff = 0.;
  //    mu1_eff_err = 0.;
  //  }
  //  if (chain.find("mu18_tight_mu8_EFFS") != std::string::npos ) {
  //    overlap_term = getMuonEfficiency(config, muons.at(0), muonsQuality.at(0), "mu18_tight").first*
  //                   getMuonEfficiency(config, muons.at(1), muonsQuality.at(1), "mu18_tight").first; // this still needs to be checked!
  //  } else {
  //    overlap_term = single_eff_tmp*dimuon_eff_tmp;
  //  }
  //}

  single_eff_tmp = mu1_eff + mu2_eff - mu1_eff*mu2_eff;
    
  /*if(config.isData){
    std::cout << "DATA: single_eff = " << single_eff_tmp << ", di_eff = " << dimuon_eff_tmp << ", overlap_term = " << overlap_term << std::endl;
    std::cout << "DATA: single_eff_mu1 = " << mu1_eff << " +- " << getMuonEfficiency(config, muons[0], muonq[0]).second << std::endl;
    std::cout << "DATA: single_eff_mu2 = " << mu2_eff << " +- " << getMuonEfficiency(config, muons[1], muonq[1]).second << std::endl;
  } else{
    std::cout << "MC: single_eff = " << single_eff_tmp << ", di_eff = " << dimuon_eff_tmp << ", overlap_term = " << overlap_term << std::endl;
    std::cout << "MC: single_eff_mu1 = " << mu1_eff << " +- " << getMuonEfficiency(config, muons[0], muonq[0]).second << std::endl;
    std::cout << "MC: single_eff_mu2 = " << mu2_eff << " +- " << getMuonEfficiency(config, muons[1], muonq[1]).second << std::endl;
    }*/

  const double thesingleordieff = single_eff_tmp + dimuon_eff_tmp - overlap_term;
  double thesingleordieff_err = 0.;

  DileptonTrigger dimuon;
  getDileptonLegs(chain, dimuon);
  
  enum {
    Leg1 = 0,
    Leg2 = 1
  };
  
  if (chain.find("mu18_tight_mu8_EFFS") != std::string::npos ){

    std::pair<double, double> mu1_leg1_pair, mu2_leg1_pair, mu1_leg2_pair, mu2_leg2_pair;

    if ((muons.at(Leg1)).Pt() >= threshold_leg1) {
      mu1_leg1_pair = getMuonEfficiency(config, muons.at(Leg1), muonsQuality.at(Leg1), dimuon.leg1);
    }
    if ((muons.at(Leg2)).Pt() >= threshold_leg1) {
      mu2_leg1_pair = getMuonEfficiency(config, muons.at(Leg2), muonsQuality.at(Leg2), dimuon.leg1);
    }
    
    if ((muons.at(Leg1)).Pt() >= threshold_leg2){
      mu1_leg2_pair = getMuonEfficiency(config, muons.at(Leg1), muonsQuality.at(Leg1), dimuon.leg2);
    }
    if ((muons.at(Leg2)).Pt() >= threshold_leg2){ 
      mu2_leg2_pair = getMuonEfficiency(config, muons.at(Leg2), muonsQuality.at(Leg2), dimuon.leg2);
    }
    
    //const std::pair<double, double> mu1_leg1_pair = getMuonEfficiency(config, muons.at(0), muonsQuality.at(0), "mu18_tight");
    //const std::pair<double, double> mu2_leg1_pair = getMuonEfficiency(config, muons.at(1), muonsQuality.at(1), "mu18_tight");
    //const std::pair<double, double> mu1_leg2_pair = getMuonEfficiency(config, muons.at(0), muonsQuality.at(0), "mu8_EFFS");
    //const std::pair<double, double> mu2_leg2_pair = getMuonEfficiency(config, muons.at(1), muonsQuality.at(1), "mu8_EFFS");
    thesingleordieff_err = getSingleOrDiError_asym(mu1_eff, mu1_eff_err, 
						   mu2_eff, mu2_eff_err,
						   mu1_leg1_pair.first, mu1_leg1_pair.second,
						   mu2_leg1_pair.first, mu2_leg1_pair.second,
						   mu1_leg2_pair.first, mu1_leg2_pair.second,
						   mu2_leg2_pair.first, mu2_leg2_pair.second,
						   mu1_leg1_pair.first, mu1_leg1_pair.second, // should be eff(bothlegs) but approximated to mu18_tight for now
						   mu2_leg1_pair.first, mu2_leg1_pair.second); // should be eff(bothlegs) but approximated to mu18_tight for now
  } else{
    std::pair<double, double> mu1_pair, mu2_pair;
    if (is_mu1_above_threshold){
      mu1_pair  = getMuonEfficiency(config, muons.at(0), muonsQuality.at(0), "mu13");
    }
    if(is_mu2_above_threshold){
      mu2_pair = getMuonEfficiency(config, muons.at(1), muonsQuality.at(1), "mu13");
    }
    thesingleordieff_err = getSingleOrDiError_sym(mu1_eff, mu1_eff_err, 
						  mu2_eff, mu2_eff_err, 
						  mu1_pair.first, mu1_pair.second,
						  mu2_pair.first, mu2_pair.second,
						  is_mu1_above_threshold, is_mu2_above_threshold);
  }

  return std::make_pair(thesingleordieff, thesingleordieff_err);

}

std::pair<double, double>
LeptonTriggerSF::getSingleOrDielectronEfficiency(const TrigMuonEff::Configuration& config,
                                             const std::vector<TLorentzVector>& electrons,
                                             const std::vector<ElectronQuality>& electronsQuality,
                                             const std::string& chain) const
{
  if (!(chain.find("e24vhi_medium1_or_e60_medium1") != std::string::npos)) {
    ::Fatal("LeptonTriggerSF::getSingleOrDielectronEfficiency",
            ERROR_MESSAGE("Currently dielectron trigger only with (e24vhi_medium1_or_e60_medium1) trigger is supported."));
    throw std::runtime_error("Currently dielectron trigger only with (e24vhi_medium1_or_e60_medium1) trigger is supported");
  }

  const std::pair<double, double> threshold_singleLep = getThresholds(config);

  DileptonThresholds thresholds;
  getDileptonThresholds(config, thresholds);

  double single_eff_tmp = 0.;
  double dielectron_eff_tmp = 0.;
  double overlap_term = 0.;

  // Set single lepton trigger efficiencies and pt plateau
  const int set_data_el1 = getElectronQuality(config.runNumber, electronsQuality.at(0), true,  false, kNoTrig);
  const int set_mc_el1   = getElectronQuality(config.runNumber, electronsQuality.at(0), false, false, kNoTrig);
  const int set_data_el2 = getElectronQuality(config.runNumber, electronsQuality.at(1), true,  false, kNoTrig);
  const int set_mc_el2   = getElectronQuality(config.runNumber, electronsQuality.at(1), false, false, kNoTrig);
  double el1_eff = 0.;
  double el1_eff_err = 0.;
  double el2_eff = 0.;
  double el2_eff_err = 0.;
  const bool is_el1_above_threshold = ((electrons.at(0)).Pt() >= threshold_singleLep.first);
  const bool is_el2_above_threshold = ((electrons.at(1)).Pt() >= threshold_singleLep.first);

  if(config.isData){
    if(is_el1_above_threshold){
      el1_eff = ElEff_Data(electrons.at(0), set_mc_el1, set_data_el1, m_year, config).first;
      el1_eff_err = ElEff_Data(electrons.at(0), set_mc_el1, set_data_el1, m_year, config).second;
    }
    if(is_el2_above_threshold){
      el2_eff = ElEff_Data(electrons.at(1), set_mc_el2, set_data_el2, m_year, config).first;
      el2_eff_err = ElEff_Data(electrons.at(1), set_mc_el2, set_data_el2, m_year, config).second;
    }
  } else{
    if(is_el1_above_threshold){
      el1_eff = ElEff_MC(electrons.at(0), set_mc_el1, m_year, config).first;
      el1_eff_err = ElEff_MC(electrons.at(0), set_mc_el1, m_year, config).second;
    }
    if(is_el2_above_threshold){
      el2_eff = ElEff_MC(electrons.at(1), set_mc_el2, m_year, config).first;
      el2_eff_err = ElEff_MC(electrons.at(1), set_mc_el2, m_year, config).second;
    }
  }
  
  single_eff_tmp = el1_eff + el2_eff - el1_eff*el2_eff;

  const bool is_2e12Tvh_loose1 = (chain.find("2e12Tvh_loose1") != std::string::npos);
  DileptonTriggerName dilep_name = kNoTrig;
  if (is_2e12Tvh_loose1) {
    dielectron_eff_tmp = getDielectronEfficiency(config, electrons, electronsQuality, "2e12Tvh_loose1").first;
    dilep_name = EF_2e12Tvh_loose1;
  } else {
    ::Error("LeptonTriggerSF::getSingleOrDielectronEfficiency",
      ERROR_MESSAGE("Invalid dielectron chain name given: %s"), chain.c_str());
    return std::make_pair(-1., -1);
  }

  const int set_data_el1_low = getElectronQuality(config.runNumber, electronsQuality.at(0), true,  true, dilep_name);
  const int set_mc_el1_low   = getElectronQuality(config.runNumber, electronsQuality.at(0), false, true, dilep_name);
  const int set_data_el2_low = getElectronQuality(config.runNumber, electronsQuality.at(1), true,  true, dilep_name);
  const int set_mc_el2_low   = getElectronQuality(config.runNumber, electronsQuality.at(1), false, true, dilep_name);
  
  //if(thresholds.e12Tvh_loose1>electrons.at(0).Pt()){
  //  if(config.isData){
  //    eff = ElEff_Data(electrons.at(0), set_mc_el1_low, set_data_el1_low, m_year, config).first;
  //  } else{
  //    eff = ElEff_MC(electrons.at(0), set_mc_el1_low, m_year, config).first;
  //  }
  //}
  //higher_el1 = (el1_eff > eff) ? el1_eff : eff;
  //if(thresholds.e12Tvh_loose1>electrons.at(1).Pt()){
  //  if(config.isData){
  //    eff = ElEff_Data(electrons.at(1), set_mc_el2_low, set_data_el2_low, m_year, config).first;
  //  } else{
  //    eff = ElEff_MC(electrons.at(1), set_mc_el2_low, m_year, config).first;
  //  }
  //}

  // all above threshold
  //if (is_el1_above_threshold and is_el2_above_threshold) {
  //
  //  // check whether e12Tvh_loose1 or e24vhi_medium1_or_e60_medium1 is more efficient
  //  double higher_el1 = 0;
  //  double higher_el2 = 0;
  //  double eff = 0.;
  //  if(thresholds.e12Tvh_loose1>electrons.at(0).Pt()){
  //    if(config.isData){
  //	eff = ElEff_Data(electrons.at(0), set_mc_el1_low, set_data_el1_low, m_year, config).first;
  //    } else{
  //	eff = ElEff_MC(electrons.at(0), set_mc_el1_low, m_year, config).first;
  //    }
  //  }
  //  higher_el1 = (el1_eff > eff) ? el1_eff : eff;
  //  if(thresholds.e12Tvh_loose1>electrons.at(1).Pt()){
  //    if(config.isData){
  //	eff = ElEff_Data(electrons.at(1), set_mc_el2_low, set_data_el2_low, m_year, config).first;
  //    } else{
  //	eff = ElEff_MC(electrons.at(1), set_mc_el2_low, m_year, config).first;
  //    }
  //  }
  //  higher_el2 = (el2_eff > eff) ? el2_eff : eff;
  //  
  //  overlap_term = higher_el1*higher_el2;
  //  
  //// one above, one below threshold
  //} else if (((not is_el1_above_threshold) and is_el2_above_threshold) or
  //           ((not is_el2_above_threshold) and is_el1_above_threshold)) {
  //  if (is_el1_above_threshold){
  //    el2_eff = 0.;
  //    el2_eff_err = 0.;
  //  }
  //  if (is_el2_above_threshold){
  //    el1_eff = 0.;
  //    el1_eff_err = 0.;
  //  }
  //  overlap_term = single_eff_tmp*dielectron_eff_tmp;
  //}

  overlap_term = single_eff_tmp*dielectron_eff_tmp;

  const double thesingleordieff = single_eff_tmp + dielectron_eff_tmp - overlap_term;
  double thesingleordieff_err = 0.;
  std::pair<double, double> el1_pair = std::make_pair(0., 0.);
  std::pair<double, double> el2_pair = std::make_pair(0., 0.);
  if(config.isData){
    if(thresholds.e12Tvh_loose1<electrons.at(0).Pt())
      el1_pair = ElEff_Data(electrons.at(0), set_mc_el1_low, set_data_el1_low, m_year, config);
    if(thresholds.e12Tvh_loose1<electrons.at(1).Pt())
      el2_pair = ElEff_Data(electrons.at(1), set_mc_el2_low, set_data_el2_low, m_year, config);
  } else{
    if(thresholds.e12Tvh_loose1<electrons.at(0).Pt())
      el1_pair = ElEff_MC(electrons.at(0), set_mc_el1_low, m_year, config);
    if(thresholds.e12Tvh_loose1<electrons.at(1).Pt())
      el2_pair = ElEff_MC(electrons.at(1), set_mc_el2_low, m_year, config);
  }
  thesingleordieff_err = getSingleOrDiError_sym(el1_eff, el1_eff_err, 
						el2_eff, el2_eff_err, 
						el1_pair.first, el1_pair.second,
						el2_pair.first, el2_pair.second,
						is_el1_above_threshold, is_el2_above_threshold);
  
  return std::make_pair(thesingleordieff, thesingleordieff_err);

}


std::pair<double, double>
LeptonTriggerSF::getElectronSF(const TLorentzVector& electron,
                               const int combination,
                               const int /* year : not needed for ElectronEfficiencyCorrection */,
                               const TrigMuonEff::Configuration& config) const
{
  const double eta = electron.Eta();
  double pt = electron.Pt();
  if (config.useGeV) pt *= GeVtoMeV;

  if (not m_electronSF[combination]) throw std::runtime_error("getElectronSF: m_electronSF[..] is not initialised for the given combination.");
  const Root::TResult& rc = m_electronSF[combination]->calculate(config.isAFII ? PATCore::ParticleDataType::Fast : PATCore::ParticleDataType::Full, config.runNumber, eta, pt);

  return std::pair<double, double>(rc.getScaleFactor(), rc.getTotalUncertainty());
}



std::pair<double, double>
LeptonTriggerSF::ElEff_Data(const TLorentzVector& electron,
                            const int combinationSF,
                            const int combinationEfficiency,
                            const int year,
                            const TrigMuonEff::Configuration& config) const
{
  std::pair<double, double> sf = getElectronSF(electron, combinationSF, year, config);
  std::pair<double, double> mcEfficiency = getElectronSF(electron, combinationEfficiency, year, config);

  const double efficiency = sf.first * mcEfficiency.first;
  const double par1 = sf.second / sf.first;
  const double par2 = mcEfficiency.second / mcEfficiency.first;
  const double error = efficiency * sqrt(par1*par1 + par2*par2);

  return std::make_pair(efficiency, error);
}



void
LeptonTriggerSF::createMcReplicaMuon(const int n,
                                     const std::string& trigger)
{
  const bool filter = not trigger.empty();
  m_efficiencyMapReplicaArray.clear();

  // create n replica maps
  for (int ii = 0; ii < n; ii++) {
    EfficiencyMap map;
    for (EfficiencyMap::const_iterator cit = m_efficiencyMap.begin();
        cit != m_efficiencyMap.end();
        cit++) {
      const std::string& name = cit->first;
      TH2* th2 = cit->second;

      if (filter and (name.find(trigger) == std::string::npos)) continue;
      if (not th2) {
        ::Fatal("LeptonTriggerSF::createMcReplicaMuon",
                ERROR_MESSAGE("An internal coding error detected"));
        throw std::runtime_error("An internal coding error detected");
      }

      TH2* hcopy = dynamic_cast<TH2*>(th2->Clone());
      if (not hcopy) {
        ::Fatal("LeptonTriggerSF::createMcReplicaMuon",
                ERROR_MESSAGE("Couldn't make copy of histogram \"%s\""), name.c_str());
        throw std::runtime_error(("Couldn't make copy of histogram " + name ).c_str());
      }

      std::pair<EfficiencyMap::iterator, bool> rc = map.insert(EfficiencyPair(name, hcopy));
      if (not rc.second) {
        ::Fatal("LeptonTriggerSF::createMcReplicaMuon",
                ERROR_MESSAGE("histogram \"%s\" duplicated"), name.c_str());
        throw std::runtime_error(("histogram duplication: " + name).c_str());
      }
    }
    m_efficiencyMapReplicaArray.push_back(map);
  }


  // fill n replica maps
  for (EfficiencyMap::const_iterator cit = m_efficiencyMap.begin();
      cit != m_efficiencyMap.end();
      cit++) {

    const std::string& name = cit->first;
    TH2* th2 = cit->second;

    if (filter and (name.find(trigger) == std::string::npos)) continue;

    if (not th2) {
      ::Fatal("LeptonTriggerSF::createMcReplicaMuon",
              ERROR_MESSAGE("An internal coding error detected"));
      throw std::runtime_error("An internal coding error detected");
    }


    for (int ii = 1; ii <= th2->GetNbinsX(); ii++) {
      for (int jj = 1; jj <= th2->GetNbinsY(); jj++) {
        const double efficiency = th2->GetBinContent(ii, jj);
        const double statisticalError = th2->GetBinError(ii, jj);
        TRandom random = TRandom();

        for (int kk = 0; kk < n; kk++) {
          EfficiencyMap &map = m_efficiencyMapReplicaArray.at(kk);
          EfficiencyMap::const_iterator itr = map.find(name);
          if (itr == map.end()) {
            ::Fatal("LeptonTriggerSF::createMcReplicaMuon",
                    ERROR_MESSAGE("Could not find what you are looking for in the efficiency map. \"%s\" "
                                  "This is either a bug or you are not using the right .root file, please double check."),
                                  name.c_str());
            throw std::runtime_error("Internal coding error detected");
          }
          const double v = random.Gaus(efficiency, statisticalError);
          (itr->second)->SetBinContent(ii, jj, v);
        }
      }
    }
  }
}



std::pair<double, double>
LeptonTriggerSF::getSfReplicaMuon(const int runNumber,
                                  const bool useGeV,
                                  const std::vector<TLorentzVector>& muons,
                                  const std::vector<MuonQuality>& muonsQuality,
                                  const int index,
                                  TrigMuonEff::Configuration* config) const
{
  std::vector<TLorentzVector> electrons;
  std::vector<ElectronQuality> electronsQuality;

  TrigMuonEff::Configuration configuration;
  configuration.runNumber = runNumber;
  configuration.useGeV = useGeV;
  configuration.setByUser = false;
  configuration.replicaIndex = index;
  configuration.isAFII = m_IsAFII;

  if (config) {
    configuration.setByUser = true;
    configuration.trigger = config->trigger;
    configuration.period = config->period;
    configuration.binning = config->binning;
  }

  return GetTriggerSF(configuration, muons, muonsQuality, electrons, electronsQuality);
}



/*=================================================================* 
 * private methods
 *=================================================================*/
void
LeptonTriggerSF::prepareForElectronSF(const std::string& directory)
{
  std::string path = directory;
  if (path.empty()) {
    const char *tmparea = gSystem->Getenv("TestArea");
    if (tmparea) {
      path = std::string(tmparea) + "/InstallArea/XML/ElectronEfficiencyCorrection/";
      ::Info("LeptonTriggerSF::prepareForElectronSF", "Using default directory: %s", path.c_str());

    } else {
      ::Warning("LeptonTriggerSF::prepareForElectronSF",
                "You are not running in Athena but also did not set a directory to the root file." );
    }

  } else {
    ::Info("LeptonTriggerSF::prepareForElectronSF", "Using user defined directory: %s", path.c_str());
  }


  m_fileNameMap.clear();
  // 2011 Trigger
  if (m_electronFileVersion=="rel17p0.v02"){
    // old v01 version since we don't have for now a v02 version !!!
    m_fileNameMap[27] = path + "/" + "efficiencySF.e20_medium.Loose.2011.7TeV.rel17p0.v01.root";
    m_fileNameMap[20] = path + "/" + "efficiency.e20_medium.Loose.2011.7TeV.rel17p0.v01.root";
    
    m_fileNameMap[8]  = path + "/" + "efficiencySF.e20_medium.Medium.2011.7TeV.rel17p0.v01.root";
    m_fileNameMap[9]  = path + "/" + "efficiency.e20_medium.Medium.2011.7TeV.rel17p0.v01.root";
    
    m_fileNameMap[10] = path + "/" + "efficiencySF.e20_medium.Tight.2011.7TeV.rel17p0.v01.root";
    m_fileNameMap[11] = path + "/" + "efficiency.e20_medium.Tight.2011.7TeV.rel17p0.v01.root";
    
    m_fileNameMap[28] = path + "/" + "efficiencySF.e22_medium1.Loose.2011.7TeV.rel17p0.v01.root";
    m_fileNameMap[21] = path + "/" + "efficiency.e22_medium1.Loose.2011.7TeV.rel17p0.v01.root";
    
    m_fileNameMap[12] = path + "/" + "efficiencySF.e22_medium.Medium.2011.7TeV.rel17p0.v01.root";
    m_fileNameMap[13] = path + "/" + "efficiency.e22_medium.Medium.2011.7TeV.rel17p0.v01.root";
    
    m_fileNameMap[14] = path + "/" + "efficiencySF.e22_medium.Tight.2011.7TeV.rel17p0.v01.root";
    m_fileNameMap[15] = path + "/" + "efficiency.e22_medium.Tight.2011.7TeV.rel17p0.v01.root";
    
    m_fileNameMap[29] = path + "/" + "efficiencySF.e22vh_medium1.Loose.2011.7TeV.rel17p0.v01.root";
    m_fileNameMap[22] = path + "/" + "efficiency.e22vh_medium1.Loose.2011.7TeV.rel17p0.v01.root";
    
    m_fileNameMap[16] = path + "/" + "efficiencySF.e22vh_medium1.Medium.2011.7TeV.rel17p0.v01.root";
    m_fileNameMap[17] = path + "/" + "efficiency.e22vh_medium1.Medium.2011.7TeV.rel17p0.v01.root";
    
    m_fileNameMap[18] = path + "/" + "efficiencySF.e22vh_medium1.Tight.2011.7TeV.rel17p0.v01.root";
    m_fileNameMap[19] = path + "/" + "efficiency.e22vh_medium1.Tight.2011.7TeV.rel17p0.v01.root";
    
  }else{
    m_fileNameMap[27] = path + "/" + "efficiencySF.e20_medium.Loose.2011.7TeV." + m_electronFileVersion + ".root";
    m_fileNameMap[20] = path + "/" + "efficiency.e20_medium.Loose.2011.7TeV." + m_electronFileVersion + ".root";
    
    m_fileNameMap[8]  = path + "/" + "efficiencySF.e20_medium.Medium.2011.7TeV." + m_electronFileVersion + ".root";
    m_fileNameMap[9]  = path + "/" + "efficiency.e20_medium.Medium.2011.7TeV." + m_electronFileVersion + ".root";
    
    m_fileNameMap[10] = path + "/" + "efficiencySF.e20_medium.Tight.2011.7TeV." + m_electronFileVersion + ".root";
    m_fileNameMap[11] = path + "/" + "efficiency.e20_medium.Tight.2011.7TeV." + m_electronFileVersion + ".root";
    
    m_fileNameMap[28] = path + "/" + "efficiencySF.e22_medium1.Loose.2011.7TeV." + m_electronFileVersion + ".root";
    m_fileNameMap[21] = path + "/" + "efficiency.e22_medium1.Loose.2011.7TeV." + m_electronFileVersion + ".root";
    
    m_fileNameMap[12] = path + "/" + "efficiencySF.e22_medium.Medium.2011.7TeV." + m_electronFileVersion + ".root";
    m_fileNameMap[13] = path + "/" + "efficiency.e22_medium.Medium.2011.7TeV." + m_electronFileVersion + ".root";
    
    m_fileNameMap[14] = path + "/" + "efficiencySF.e22_medium.Tight.2011.7TeV." + m_electronFileVersion + ".root";
    m_fileNameMap[15] = path + "/" + "efficiency.e22_medium.Tight.2011.7TeV." + m_electronFileVersion + ".root";
    
    m_fileNameMap[29] = path + "/" + "efficiencySF.e22vh_medium1.Loose.2011.7TeV." + m_electronFileVersion + ".root";
    m_fileNameMap[22] = path + "/" + "efficiency.e22vh_medium1.Loose.2011.7TeV." + m_electronFileVersion + ".root";
    
    m_fileNameMap[16] = path + "/" + "efficiencySF.e22vh_medium1.Medium.2011.7TeV." + m_electronFileVersion + ".root";
    m_fileNameMap[17] = path + "/" + "efficiency.e22vh_medium1.Medium.2011.7TeV." + m_electronFileVersion + ".root";
    
    m_fileNameMap[18] = path + "/" + "efficiencySF.e22vh_medium1.Tight.2011.7TeV." + m_electronFileVersion + ".root";
    m_fileNameMap[19] = path + "/" + "efficiency.e22vh_medium1.Tight.2011.7TeV." + m_electronFileVersion + ".root";
  }

  // 2012 Trigger
  m_fileNameMap[31] = path + "/" + "efficiencySF.e24vhi_medium1_e60_medium1.Loose.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[32] = path + "/" + "efficiency.e24vhi_medium1_e60_medium1.Loose.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[33] = path + "/" + "efficiencySF.e24vhi_medium1_e60_medium1.Medium.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[34] = path + "/" + "efficiency.e24vhi_medium1_e60_medium1.Medium.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[35] = path + "/" + "efficiencySF.e24vhi_medium1_e60_medium1.Tight.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[36] = path + "/" + "efficiency.e24vhi_medium1_e60_medium1.Tight.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[37] = path + "/" + "efficiencySF.e24vhi_medium1_e60_medium1.Multilepton.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[38] = path + "/" + "efficiency.e24vhi_medium1_e60_medium1.Multilepton.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[39] = path + "/" + "efficiencySF.e12Tvh_loose1.Loose.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[40] = path + "/" + "efficiency.e12Tvh_loose1.Loose.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[41] = path + "/" + "efficiencySF.e12Tvh_loose1.Medium.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[42] = path + "/" + "efficiency.e12Tvh_loose1.Medium.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[43] = path + "/" + "efficiencySF.e12Tvh_loose1.Tight.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[44] = path + "/" + "efficiency.e12Tvh_loose1.Tight.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[45] = path + "/" + "efficiencySF.e12Tvh_loose1.Multilepton.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[46] = path + "/" + "efficiency.e12Tvh_loose1.Multilepton.2012.8TeV." + m_electronFileVersion + ".root";

  // new egamma likelihood ID

  // LLH Loose
  m_fileNameMap[50] = path + "/" + "efficiencySF.e24vhi_medium1_e60_medium1.LooseLLH.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[51] = path + "/" + "efficiency.e24vhi_medium1_e60_medium1.LooseLLH.2012.8TeV." + m_electronFileVersion + ".root";
 
  m_fileNameMap[52] = path + "/" + "efficiencySF.e12Tvh_loose1.LooseLLH.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[53] = path + "/" + "efficiency.e12Tvh_loose1.LooseLLH.2012.8TeV." + m_electronFileVersion + ".root";

  // LLH VeryTight
  m_fileNameMap[54] = path + "/" + "efficiencySF.e24vhi_medium1_e60_medium1.VeryTightLLH.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[55] = path + "/" + "efficiency.e24vhi_medium1_e60_medium1.VeryTightLLH.2012.8TeV." + m_electronFileVersion + ".root";
 
  m_fileNameMap[56] = path + "/" + "efficiencySF.e12Tvh_loose1.VeryTightLLH.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[57] = path + "/" + "efficiency.e12Tvh_loose1.VeryTightLLH.2012.8TeV." + m_electronFileVersion + ".root";

  // the medium1 e12Tvh_medium1
  m_fileNameMap[70] = path + "/" + "efficiencySF.e12Tvh_medium1.Loose.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[71] = path + "/" + "efficiency.e12Tvh_medium1.Loose.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[72] = path + "/" + "efficiencySF.e12Tvh_medium1.Medium.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[73] = path + "/" + "efficiency.e12Tvh_medium1.Medium.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[74] = path + "/" + "efficiencySF.e12Tvh_medium1.Tight.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[75] = path + "/" + "efficiency.e12Tvh_medium1.Tight.2012.8TeV." + m_electronFileVersion + ".root";

  m_fileNameMap[76] = path + "/" + "efficiencySF.e12Tvh_medium1.Multilepton.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[77] = path + "/" + "efficiency.e12Tvh_medium1.Multilepton.2012.8TeV." + m_electronFileVersion + ".root";

  // LLH Loose e12Tvh_medium1
  m_fileNameMap[78] = path + "/" + "efficiencySF.e12Tvh_medium1.LooseLLH.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[79] = path + "/" + "efficiency.e12Tvh_medium1.LooseLLH.2012.8TeV." + m_electronFileVersion + ".root";

  // LLH VeryTight e12Tvh_medium1
  m_fileNameMap[80] = path + "/" + "efficiencySF.e12Tvh_medium1.VeryTightLLH.2012.8TeV." + m_electronFileVersion + ".root";
  m_fileNameMap[81] = path + "/" + "efficiency.e12Tvh_medium1.VeryTightLLH.2012.8TeV." + m_electronFileVersion + ".root";

  m_electronSF.clear();

}

std::string
LeptonTriggerSF::getPathName(const std::string& directory,
                             const std::string& fileName) const
{
  //
  // construct a path to a root file
  //
  std::string path = directory;
  if (path.empty()) { // default to InstallArea/share for the files if running in Athena
    const char *tmparea = gSystem->Getenv("TestArea");
    if (tmparea) {
      path = std::string(tmparea) + "/InstallArea/share/";
      ::Info("LeptonTriggerSF::getPathName", "Using default directory: %s", path.c_str());

    } else {
      ::Warning("LeptonTriggerSF::getPathName",
                "You are not running in Athena but also did not set a directory to the root file." );
    }

  } else {
    ::Info("LeptonTriggerSF::getPathName", "Using user defined directory: %s", path.c_str());
  }


  std::string name = fileName;
  if (fileName.empty()) {
    if (m_year == 2011) {
      name = "muon_trigger_sf_mc11c.root";
      ::Info("LeptonTriggerSF::getPathName", "Using default file for 2011: %s", name.c_str());

    } else if (m_year == 2012) {
      name = "muon_trigger_sf_2012_AtoL.p1328.root";
      ::Info("LeptonTriggerSF::getPathName", "Using default file for 2012: %s", name.c_str());

    } else {
      ::Error("LeptonTriggerSF::getPathName",
              "unknown year specified. cannot define default file.");
      throw std::runtime_error("Unknown year: cannot define default file.");
    }

  } else {
    ::Info("LeptonTriggerSF::getPathName", "Using user defined file name: %s", name.c_str());
  }

  return path + "/" + name;
}

void
LeptonTriggerSF::setStorage(Storage& storage) const
{
  //
  // Constants used in the file reading:
  //
  static const std::string type_[] = {"data", "mc"};
  static const std::vector<std::string> type(type_, type_ + sizeof(type_)/sizeof(std::string));

  static const std::string region_[]  = {"barrel", "endcap"};
  static const std::vector<std::string> region(region_, region_ + sizeof(region_)/sizeof(std::string));


  static std::vector<std::string> quality2011, quality2012;
  if ((not m_qualities.empty()) and ((m_qualities.size() != 2) and (m_qualities.size() != 1))) {
    ::Fatal("LeptonTriggerSF::setStorage",
            ERROR_MESSAGE("If giving a muon qualities vector, it needs to be of size 2."));
    throw std::runtime_error("Muon qualities vector has wrong size ");

  } else if ((m_qualities.size() == 2) or (m_qualities.size() == 1)) {
    quality2011 = m_qualities;
    quality2012 = m_qualities;

  } else {
    // If multiple instances of the tool are initialized within the same
    // process, we have to be careful not to fill these vectors multiple
    // times:
    quality2011.clear();
    quality2012.clear();
    quality2011.push_back("loose");
    quality2011.push_back("combined");
    quality2012.push_back("loose");
    quality2012.push_back("combined");
  }


  // 2011
  static const std::string bins2011_[]    = {"fine", "coarse"};
  static const std::vector<std::string> bins2011(bins2011_, bins2011_ + sizeof(bins2011_)/sizeof(std::string));

  static const std::string trigger2011_[] = {"mu18_MG", "mu18_MG_medium", "mu18_MG_medium",
                                             "mu18_MG_medium", "mu18_MG_medium", "mu18_MG_medium", "mu18_MG_medium", "mu18_MG_medium"};
  static const std::vector<std::string> trigger2011(trigger2011_, trigger2011_ + sizeof(trigger2011_)/sizeof(std::string));

  static const std::string period2011_[] = {"BtoI", "JtoMwoL3toL4", "L3toL4",
                                            "J", "K", "JtoK", "JtoM", "LtoM"};
  static const std::vector<std::string> period2011(period2011_, period2011_ + sizeof(period2011_)/sizeof(std::string));


  // 2012
  static const std::string bins2012_[]    = {"fine", "coarse"};
  static const std::vector<std::string> bins2012(bins2012_, bins2012_ + sizeof(bins2012_)/sizeof(std::string));

  static const std::string trigger2012_[] = {"mu24i_tight_or_EF_mu36_tight", "mu24i_tight_or_EF_mu36_tight", "mu24i_tight_or_EF_mu36_tight", "mu24i_tight_or_EF_mu36_tight", "mu24i_tight_or_EF_mu36_tight",
                                             "mu24i_tight", "mu24i_tight", "mu24i_tight", "mu24i_tight", "mu24i_tight",
                                             "mu18_tight", "mu18_tight", "mu18_tight", "mu18_tight", "mu18_tight",
                                             "mu36_tight", "mu36_tight", "mu36_tight", "mu36_tight", "mu36_tight",
                                             "mu8_EFFS", "mu8_EFFS", "mu8_EFFS", "mu8_EFFS", "mu8_EFFS",
                                             "mu8", "mu8", "mu8", "mu8", "mu8",
                                             "mu13", "mu13", "mu13", "mu13", "mu13"};
  static const std::vector<std::string> trigger2012(trigger2012_, trigger2012_ + sizeof(trigger2012_)/sizeof(std::string));

  static const std::string period2012_[] = {"A", "B", "CtoE", "G", "HtoL",
                                            "A", "B", "CtoE", "G", "HtoL",
                                            "A", "B", "CtoE", "G", "HtoL",
                                            "A", "B", "CtoE", "G", "HtoL",
                                            "A", "B", "CtoE", "G", "HtoL",
                                            "A", "B", "CtoE", "G", "HtoL",
                                            "A", "B", "CtoE", "G", "HtoL"};
  static const std::vector<std::string> period2012(period2012_, period2012_ + sizeof(period2012_)/sizeof(std::string));


  storage.type = &type;
  storage.region = &region;
  switch (m_year) {
    case 2011:
      storage.trigger = &trigger2011;
      storage.period = &period2011;
      storage.quality = &quality2011;
      storage.bins = &bins2011;
      break;

    case 2012:
      storage.trigger = &trigger2012;
      storage.period = &period2012;
      storage.quality = &quality2012;
      storage.bins = &bins2012;
      break;

    default:
      ::Fatal("LeptonTriggerSF::setStorage",
              ERROR_MESSAGE("Unknown year: %d"), m_year);
      throw std::runtime_error("unknown year");
      break;
  }

  if (storage.period->size() != storage.trigger->size()) {
    ::Fatal("LeptonTriggerSF::setStorage",
            ERROR_MESSAGE("size of period and trigger is different: check configuration"));
    throw std::runtime_error("size of period and trigger is different: check configuration");
  }

  ::Info("LeptonTriggerSF::setStorage",
         "Year = %d, nperiods = %u", m_year, (unsigned int)storage.period->size());
  ::Info("LeptonTriggerSF::setStorage",
         "nquality = %u, nbins = %u, ntype = %u, nregion = %u",
         (unsigned int)storage.quality->size(), (unsigned int)storage.bins->size(), (unsigned int)storage.type->size(), (unsigned int)storage.region->size());

  return;
}



TDirectory*
LeptonTriggerSF::getTemporaryDirectory(void) const
{
  //
  // Create a unique directory in memory to hold the histograms:
  //
  gROOT->cd();
  TDirectory* tempDir = 0;
  int counter = 0;
  while (not tempDir) {
    // First, let's find a directory name that doesn't exist yet:
    std::stringstream dirname;
    dirname << "LeptonTriggerSFTempDir_%i" << counter;
    if (gROOT->GetDirectory((dirname.str()).c_str())) {
      ++counter;
      continue;
    }
    // Let's try to make this directory:
    tempDir = gROOT->mkdir((dirname.str()).c_str());
    if (not tempDir) {
      ::Fatal("LeptonTriggerSF::getTemporaryDirectory",
              ERROR_MESSAGE("Temporary directory could not be created"));
      throw std::runtime_error("Temporary directory could not be created");
    }
  }

  return tempDir;
}



void
LeptonTriggerSF::initialise(const std::string& directory,
                            const std::string& fileName,
                            const std::vector<std::string>& qualities,
                            const std::string& directoryElectron)
{
  prepareForElectronSF(directoryElectron);

  m_max_period = TrigMuonEff::period_undefined;

  m_qualities = qualities;

  // Remember which TDirectory the system is in at the beginning of the method:
  TDirectory* origDir = gDirectory;

  // set parameters for reading a root file
  Storage storage;
  setStorage(storage);

  // Open the efficiency file:
  const std::string filePath = getPathName(directory, fileName);
  TFile* file = TFile::Open(filePath.c_str());

  if (not file) {
    ::Fatal("LeptonTriggerSF::initialise", ERROR_MESSAGE("Couldn't open file \"%s\""), filePath.c_str());
    throw std::runtime_error(("Couldn't open file \"" + filePath + "\". input root files are available at "
                              "https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TrigMuonEfficiency "
                              "or in /afs/cern.ch/user/t/takashi/public/SF2012/").c_str());
  }

  ::Info("LeptonTriggerSF::initialise", "File \"%s\" opened", file->GetName());


  // Create a unique directory in memory to hold the histograms:
  TDirectory* tempDir = getTemporaryDirectory();
  tempDir->cd();

  //
  // Read in the histograms:
  //
  for (size_t iqu = 0; iqu < storage.quality->size(); ++iqu) {
    for (size_t ibins = 0; ibins < storage.bins->size(); ++ibins) {
      for (size_t iperiod = 0; iperiod < storage.period->size(); ++iperiod) {
        for (size_t iregion = 0; iregion < storage.region->size(); ++iregion) {
          for (size_t itype = 0; itype < storage.type->size(); ++itype) {

            // Construct the histogram name:
            std::string histname = ("_MuonTrigEff_" + storage.period->at(iperiod) + "_" +
                                    ((m_year > 2011) ? storage.trigger->at(iperiod) + "_" : "") +
                                    storage.quality->at(iqu) +
                                    "_EtaPhi_" +
                                    storage.bins->at(ibins) + "_" +
                                    storage.region->at(iregion) + "_" +
                                    storage.type->at(itype));
            // Try to access the histogram:
            std::string path = (storage.quality->at(iqu) + "/" +
                                storage.bins->at(ibins) + "/" +
                                storage.trigger->at(iperiod) + "_" + storage.period->at(iperiod) + "/" +
                                "etaphi_" + storage.region->at(iregion) + "_eff_" + storage.type->at(itype) + "_period" +
                                storage.period->at(iperiod) + "_EF_" + storage.trigger->at(iperiod));
            TH2* hist = dynamic_cast<TH2*>(file->Get(path.c_str()));
            if (not hist) {
              ::Warning("LeptonTriggerSF::initialise",
                        "%s not available in %s", path.c_str(), filePath.c_str());
              continue;
            }
            // Make a copy of the histogram into the in-memory directory:
            TH2* hcopy = dynamic_cast<TH2*>(hist->Clone());
            if (not hcopy) {
              ::Fatal("LeptonTriggerSF::initialise",
                      ERROR_MESSAGE("Couldn't make copy of histogram \"%s\""), histname.c_str());
              throw std::runtime_error(("Couldn't make copy of histogram " + histname ).c_str());
            }

            // Now, remember this histogram:
            // std::cout << "DEBUG: " << path << " = " << histname << "\n";
            std::pair<EfficiencyMap::iterator, bool> rc = m_efficiencyMap.insert(EfficiencyPair(histname, hcopy));
            if (not rc.second) {
              ::Fatal("LeptonTriggerSF::initialise",
                      ERROR_MESSAGE("histogram \"%s\" duplicated"), histname.c_str());
              throw std::runtime_error(("histogram duplication: " + histname).c_str());
            }
          }
        }
      }
    }
  }


  //
  // Set the phi boundaries:
  //
  if (m_year == 2011) {
    setPhiBoundary2011(storage.period);

  } else {
    setPhiBoundary(file);
  }

  // Now we can close the file:
  file->Close();
  delete file;

  // Return to the directory we were in before the function call:
  origDir->cd();

  ::Info("LeptonTriggerSF::initialise",
         "Initialization successful");

  return;
}

std::string
LeptonTriggerSF::getMuonQuality(const MuonQuality quality)
{
  std::string name;
  switch (quality) {
    case loose:
      name = "loose";
      break;

    case combined:
      name = "combined";
      break;

    default:
      ::Error("LeptonTriggerSF::getMuonQuality",
              ERROR_MESSAGE("Unknown muon quality (%i) received; Using loose muon as default"),
              quality );
      name = "loose";
   }

   return name;
}

int
LeptonTriggerSF::getElectronQuality(const int runNumber,
                                    const ElectronQuality quality,
                                    const bool isSF,
                                    const bool isditrig,
				    const DileptonTriggerName dilepName) const
{
  int combination = 0;

  if (runNumber <= 186755) { // e20_medium
    switch (quality) {
      case loosepp:
        combination = isSF ? 27 : 20;

        initialiseElectronTool(27);
        initialiseElectronTool(20);
        break;

      case mediumpp:
        combination = isSF ? 8 : 9;

        initialiseElectronTool(8);
        initialiseElectronTool(9);
        break;

      case tightpp:
        combination = isSF ? 10 : 11;

        initialiseElectronTool(10);
        initialiseElectronTool(11);
        break;

      default:
        ::Error("LeptonTriggerSF::getElectronQuality",
                ERROR_MESSAGE("Unknown electron quality (%i) received; Using loose++ electron as default" ),
                quality );
        combination = isSF ? 27 : 20;

        initialiseElectronTool(27);
        initialiseElectronTool(20);
    }
      
  } else if (runNumber <= 187815) { // e22_medium
    switch (quality) {
      case loosepp:
        combination = isSF ? 28 : 21;

        initialiseElectronTool(28);
        initialiseElectronTool(21);
        break;

      case mediumpp:
        combination = isSF ? 12 : 13;

        initialiseElectronTool(12);
        initialiseElectronTool(13);
        break;

      case tightpp:
        combination = isSF ? 14 : 15;

        initialiseElectronTool(14);
        initialiseElectronTool(15);
        break;

      default:
        ::Error("LeptonTriggerSF::getElectronQuality",
                ERROR_MESSAGE("Unknown electron quality (%i) received; Using loose++ electron as default" ),
                quality );
        combination = isSF ? 28 : 21;

        initialiseElectronTool(28);
        initialiseElectronTool(21);
    }

  } else if (runNumber <= 191933 ) { // e22vh_medium1
    switch (quality) {
      case loosepp:
        combination = isSF ? 29 : 22;

        initialiseElectronTool(29);
        initialiseElectronTool(22);
        break;

      case mediumpp:
        combination = isSF ? 16 : 17;

        initialiseElectronTool(16);
        initialiseElectronTool(17);
        break;

      case tightpp:
        combination = isSF ? 18 : 19;

        initialiseElectronTool(18);
        initialiseElectronTool(19);
        break;

      default:
        ::Error("LeptonTriggerSF::getElectronQuality",
                ERROR_MESSAGE("Unknown electron quality (%i) received; Using loose++ electron as default" ),
                quality );
        combination = isSF ? 29 : 22;

        initialiseElectronTool(29);
        initialiseElectronTool(22);
    }

  } else if (runNumber <=  9999999) { 
    if(!isditrig){ // e24vhi_medium1 || e60_medium1
      switch (quality) {
      case loosepp:
        combination = isSF ? 31 : 32;
	
        initialiseElectronTool(31);
        initialiseElectronTool(32);
        break;
	
      case mediumpp:
        combination = isSF ? 33 : 34;
	
        initialiseElectronTool(33);
        initialiseElectronTool(34);
        break;

      case tightpp:
        combination = isSF ? 35 : 36;

        initialiseElectronTool(35);
        initialiseElectronTool(36);
        break;

      case ML:
        combination = isSF ? 37 : 38;

        initialiseElectronTool(37);
        initialiseElectronTool(38);
        break;

      case LooseLLH:
        combination = isSF ? 50 : 51;

        initialiseElectronTool(50);
        initialiseElectronTool(51);
        break;

      case VeryTightLLH:
        combination = isSF ? 54 : 55;

        initialiseElectronTool(54);
        initialiseElectronTool(55);
        break;

      default:
        ::Error("LeptonTriggerSF::getElectronQuality",
                ERROR_MESSAGE("Unknown electron quality (%i) received; Using loose++ electron as default" ),
                quality );
        combination = isSF ? 31 : 32;

        initialiseElectronTool(31);
        initialiseElectronTool(32);
      }
    }  else if(dilepName==EF_e12Tvh_medium1_mu8){ // e12Tvh_medium1
      switch (quality) {
      case loosepp:
        combination = isSF ? 70 : 71;
	
        initialiseElectronTool(70);
        initialiseElectronTool(71);
        break;
	
      case mediumpp:
        combination = isSF ? 72 : 73;
	
        initialiseElectronTool(72);
        initialiseElectronTool(73);
        break;

      case tightpp:
        combination = isSF ? 74 : 75;

        initialiseElectronTool(74);
        initialiseElectronTool(75);
        break;

      case ML:
        combination = isSF ? 78 : 79;

        initialiseElectronTool(78);
        initialiseElectronTool(79);
        break;

      case LooseLLH:
        combination = isSF ? 80 : 81;

        initialiseElectronTool(80);
        initialiseElectronTool(81);
        break;

      case VeryTightLLH:
        combination = isSF ? 82 : 83;

        initialiseElectronTool(82);
        initialiseElectronTool(83);
        break;

      default:
        ::Error("LeptonTriggerSF::getElectronQuality",
                ERROR_MESSAGE("Unknown electron quality (%i) received; Using loose++ electron as default" ),
                quality );
        combination = isSF ? 70 : 71;

        initialiseElectronTool(70);
        initialiseElectronTool(71);
      } 
    } else { //else if(dilepName==EF_2e12Tvh_loose1){ // e12Tvh_loose1 -- default for backward compatibility
      
      switch (quality) {
      case loosepp:
	combination = isSF ? 39 : 40;
	
	initialiseElectronTool(39);
	initialiseElectronTool(40);
	break;
	
      case mediumpp:
	combination = isSF ? 41 : 42;
	
	initialiseElectronTool(41);
	initialiseElectronTool(42);
	break;
	
      case tightpp:
	combination = isSF ? 43 : 44;
	
	initialiseElectronTool(43);
	initialiseElectronTool(44);
	break;
	  
      case ML:
	combination = isSF ? 45 : 46;
	
	initialiseElectronTool(45);
	initialiseElectronTool(46);
	break;
	
      case LooseLLH:
	combination = isSF ? 52 : 53;
	
	initialiseElectronTool(52);
	initialiseElectronTool(53);
	break;
	
      case VeryTightLLH:
	  combination = isSF ? 56 : 57;
	
	initialiseElectronTool(56);
	initialiseElectronTool(57);
	break;
	
      default:
	::Error("LeptonTriggerSF::getElectronQuality",
		ERROR_MESSAGE("Unknown electron quality (%i) received; Using loose++ electron as default" ),
		quality );
	combination = isSF ? 39 : 40;
	
	initialiseElectronTool(39);
	initialiseElectronTool(40);
      } 
    }
  } else {
    ::Error("LeptonTriggerSF::getElectronQuality",
	    ERROR_MESSAGE( "Bad run number (%i) received" ), runNumber);
  }
  
  return combination;
}




std::pair<double, double>
LeptonTriggerSF::getThresholds(const TrigMuonEff::Configuration& config)
{
  std::pair<double, double> result(20., 21.);  // (muon, electron)

  if (config.runNumber <= 186755) {
    result.second = 21.;

  } else if (config.runNumber <= 191933) {
    result.second = 23.;

  } else if ((200804 <= config.runNumber) and (config.runNumber <= 9999999)) {
    result.first = 25.;  // muon
    result.second = 25.; // electron

    if (config.trigger.find("e12Tvh_medium1_mu8") != std::string::npos){
      result.first = 10.;  // muon
      result.second = 14.; // electron
    }

  } else {
    ::Error("LeptonTriggerSF::getThresholds",
            ERROR_MESSAGE( "Unknown run number (%i) received" ), config.runNumber);
    return result;
  }

  if (not config.useGeV ) {
    result.first  *= GeVtoMeV;
    result.second *= GeVtoMeV;
  }

  return result;
}



void
LeptonTriggerSF::getDileptonThresholds(const TrigMuonEff::Configuration& config,
                                     DileptonThresholds& thresholds)
{
  thresholds.mu18_tight = 20;
  thresholds.mu8_EFFS = 10;
  thresholds.mu13 = 15;
  thresholds.e12Tvh_loose1 = 14;

  if (not config.useGeV) {
    thresholds.mu18_tight *= GeVtoMeV;
    thresholds.mu8_EFFS *= GeVtoMeV;
    thresholds.mu13 *= GeVtoMeV;
    thresholds.e12Tvh_loose1 *= GeVtoMeV;
  }
}



TrigMuonEff::DataPeriod
LeptonTriggerSF::getDataPeriod(int runNumber)
{
  if (runNumber < 178044) return TrigMuonEff::period_undefined;
  if (runNumber <= 186493) return TrigMuonEff::period_2011_BtoI;
  if ((runNumber <= 189090) or
      (runNumber >= 189639 and runNumber <= 191933)) return TrigMuonEff::period_2011_JtoMwithoutL3L4;
  if ((runNumber >= 189184) and (runNumber <= 189610)) return TrigMuonEff::period_2011_L3toL4;
  if ((runNumber >= 200804) and (runNumber <= 201556)) return TrigMuonEff::period_2012_A;
  if ((runNumber >= 202660) and (runNumber <= 205113)) return TrigMuonEff::period_2012_B;
  if ((runNumber >= 206248) and (runNumber <= 210308)) return TrigMuonEff::period_2012_CtoE;
  if ((runNumber >= 211522) and (runNumber <= 212272)) return TrigMuonEff::period_2012_G;
  if ((runNumber >= 212619) and (runNumber <= 215643)) return TrigMuonEff::period_2012_HtoL;

  ::Error("LeptonTriggerSF::getDataPeriod",
          ERROR_MESSAGE( "Can't associate run number (%i) to a period" ), runNumber);
  return TrigMuonEff::period_undefined;
}



double
LeptonTriggerSF::getSfError(const double a,
                            const double b,
                            const double c,
                            const double d)
{
  const double a2 = a*a;
  const double b2 = b*b;
  const double p1 = (1 - a);
  const double p2 = (1 - b);
  const double p12 = p1*p1;
  const double p22 = p2*p2;
  const double p24 = p22*p22;

  return sqrt( ( a2 / p22 ) * c + ( ( b2 * p12 ) / p24 ) * d );
}


double
LeptonTriggerSF::getDiError(const double a1, const double a2,
			    const double b1, const double b2,
			    const double c1, const double c2,
			    const double d1, const double d2,
			    const double e1, const double e2,
			    const double f1, const double f2)
{
  const double T1 = ( d1 * a2 ) * ( d1 * a2 );
  const double T2 = ( a1 * d2 ) * ( a1 * d2 );
  const double T3 = ( c1 * b2 ) * ( c1 * b2 );
  const double T4 = ( b1 * c2 ) * ( b1 * c2 );
  const double T5 = ( f1 * e2 ) * ( f1 * e2 );
  const double T6 = ( e1 * f2 ) * ( e1 * f2 );

  return sqrt( T1 + T2 + T3 + T4 + T5 + T6 );
}

double
LeptonTriggerSF::getSingleOrDiError_asym(const double a1, const double a2, // lepton 1, single trigger eff+unc
					 const double b1, const double b2, // lepton 2, single trigger eff+unc
					 const double c1, const double c2, // lepton 1, single trigger leg1 for dilepton eff+unc
					 const double d1, const double d2, // lepton 2, single trigger leg1 for dilepton eff+unc
					 const double e1, const double e2, // lepton 1, single trigger leg2 for dilepton eff+unc
					 const double f1, const double f2, // lepton 2, single trigger leg2 for dilepton eff+unc
					 const double g1, const double g2, // lepton 1, single trigger leg1 and leg2 for dilepton eff+unc
					 const double h1, const double h2) // lepton 2, single trigger leg1 and leg2 for dilepton eff+unc
{
  const double eff_single = a1+b1 -a1*b1;
  //const double eff_double = c1*f1+d1*e1 - c1*f1*d1*e1;
  //const double T1 = ( (1.0-b1) * (1.0-eff_double) * a2 )*( (1.0-b1) * (1.0-eff_double) * a2 ) + ( (1.0-a1) * (1.0-eff_double) * b2 )*( (1.0-a1) * (1.0-eff_double) * b2 ); // vary the single trigger uncertainties
  ////const double T2 = ( ( (d1+f1) * c2) * ( (d1+f1) * c2) ) + ( ( (c1+e1) * d2) * ( (c1+e1) * d2) );
  //const double T2  =  ( (f1*(1.0-d1*e1)) * (1.0 - eff_single) * c2)* ( (f1*(1.0-d1*e1)) * (1.0 - eff_single) * c2) +  ((c1*(1.0-e1*d1)) * (1.0 - eff_single) * f2)*((c1*(1.0-e1*d1)) * (1.0 - eff_single) * f2);
  //const double T2b =  ( (d1*(1.0-c1*f1)) * (1.0 - eff_single) * e2)*( (d1*(1.0-c1*f1)) * (1.0 - eff_single) * e2) +  ((e1*(1.0-c1*f1)) * (1.0 - eff_single) * d2)* ((e1*(1.0-c1*f1)) * (1.0 - eff_single) * d2);
  ////const double T3 = d1*e2*d1*e2 + c1*f2*c1*f2;
  ////const double T4 = h1*g2*h1*g2 + g1*h2*g1*h2;
  //		      
  ////std::cout << "OR error: " << T1 << " T2: " << T2 << " T3: " << T3 << " T4: " << T4 << std::endl;
  //std::cout << "a2: " << a2 << " b2: " << b2 << " c2: " << c2 << " d2: " << d2 << std::endl;
  //std::cout << "e2: " << e2 << " f2: " << f2 << " g2: " << g2 << " h2: " << h2 << std::endl;
  //std::cout << "a1: " << a1 << " b1: " << b1 << " c1: " << c1 << " d1: " << d1 << std::endl;
  //std::cout << "e1: " << e1 << " f1: " << f1 << " g1: " << g1 << " h1: " << h1 << std::endl;
  //std::cout <<"error: " << sqrt( T1 + T2 + T2b) << std::endl;
  //return sqrt( T1 + T2 + T2b);

  // correlate the lead lepton 
  //double T1 = a2*(1-b1)+c2*f1*(1.0-d1*e1)-(a2*(1-b1)*eff_double+c2*eff_single*f1*(1.0-d1*e1));
  //T1*=T1; // treat mu18 and mu24i/mu36 as correlated
  //double T2 = b2*(1-a1)+d2*e1*(1.0-c1*f1)-(b2*(1-a1)*eff_double+d2*eff_single*e1*(1.0-c1*f1));
  //T2*=T2; // treat mu18 and mu24i/mu36 as correlated
  double T1 = c2*(1.0-d1);
  T1*=T1;
  double T2 = d2*(1.0-c1);
  T2*=T2;
  double T3 = e2*d1*(1.0-c1*f1)*(1.0-eff_single); 
  T3*=T3; // treat mu8 and mu24i/mu36 as uncorrelated
  double T4 = f2*c1*(1.0-d1*e1)*(1.0-eff_single); 
  T4*=T4; // treat mu8 and mu24i/mu36 as uncorrelated

  //std::cout << "OR error: " << T1 << " T2: " << T2 << " T3: " << T3 << " T4: " << T4 << std::endl;
  //std::cout <<"error: " << sqrt( T1 + T2 + T3 + T4) << std::endl;
  return sqrt( T1 + T2 + T3 + T4);
}

double
LeptonTriggerSF::getSingleOrDiError_sym(const double a1, const double a2, // lepton 1, single trigger eff+unc
					const double b1, const double b2, // lepton 2, single trigger eff+unc
					const double c1, const double c2, // lepton 1, single trigger leg for dilepton eff+unc
					const double d1, const double d2, // lepton 2, single trigger leg for dilepton eff+unc
					bool ispT1above, bool ispT2above)
{

  //double a1=0.0, a2=0.0, b1=0.0, b2=0.0, c1=0.0, c2=0.0, d1=0.0, d2=0.0; 
  //if(ispT1above){
  //  a1=ai1; a2=ai2; c1=ci1; c2=ci2;
  //}
  //if(ispT2above){
  //  b1=bi1; b2=bi2; d1=di1; d2=di2;
  //}

  const double single_eff = a1+b1-a1*b1;
  const double dil_eff    = c1*d1;

  double T1 = a2*(1.0-b1)*(1.0-dil_eff) + c2*d1*(1.0-single_eff);
  T1*=T1;
  double T2 = b2*(1.0-a1)*(1.0-dil_eff) + d2*c1*(1.0-single_eff);
  T2*=T2;

  //std::cout << "a1: " << a1 << " a2: " << a2 << " b1: " << b1 << " b2: " << b2
  //	    << " c1: " << c1 << " c2: " << c2 << " d1: " << d1 << " d2: " << d2
  //	    << " T1: " << T1 << " T2: " << T2 << std::endl;
  //
  return sqrt( T1 + T2 );
  //if(ispT1above and ispT2above){
  //  if(a1 > c1){
  //    if(b1 > d1){
  //	const double T1 = ( (1-b1-b1) * a2 ) * ( (1-b1-b1) * a2 );
  //	const double T2 = ( (1-a1-a1) * b2 ) * ( (1-a1-a1) * b2 );
  //	const double T3 = ( d1*c2 ) * ( d1*c2 );
  //	const double T4 = ( c1*d2 ) * ( c1*d2 );
  //	return sqrt( T1 + T2 + T3 + T4 );
  //    } else{
  //	const double T1 = ( (1-b1-d1) * a2 ) * ( (1-b1-d1) * a2 );
  //	const double T2 = ( (1-a1) * b2 ) * ( (1-a1) * b2 );
  //	const double T3 = ( d1*c2 ) * ( d1*c2 );
  //	const double T4 = ( (c1-a1) * d2 ) * ( (c1-a1) * d2 );
  //	return sqrt( T1 + T2 + T3 + T4 );
  //    }
  //  } else{
  //    if(b1 > d1){
  //	const double T1 = ( (1-b1) * a2 ) * ( (1-b1) * a2 );
  //	const double T2 = ( (1-a1-c1) * b2 ) * ( (1-a1-c1) * b2 );
  //	const double T3 = ( (d1-b1) * c2 ) * ( (d1-b1) * c2 );
  //	const double T4 = ( c1*d2 ) * ( c1*d2 );
  //	return sqrt( T1 + T2 + T3 + T4 );
  //    } else{
  //	const double T1 = ( (1-b1) * a2 ) * ( (1-b1) * a2 );
  //	const double T2 = ( (1-a1) * b2 ) * ( (1-a1) * b2 );
  //	return sqrt( T1 + T2 );
  //    }
  //  }
  //} else if( ((not ispT1above) and ispT2above) or ((not ispT2above) and ispT1above) ){
  //  const double T1 = ( (1-b1) * ( 1-(c1 * d1) ) * a2 ) * ( (1-b1) * ( 1-(c1 * d1) ) * a2 );
  //  const double T2 = ( (1-a1) * ( 1-(c1 * d1) ) * b2 ) * ( (1-a1) * ( 1-(c1 * d1) ) * b2 );
  //  const double T3 = ( d1 * ( 1-a1-b1+(a1*b1) ) * c2) * ( d1 * ( 1-a1-b1+(a1*b1) ) * c2);
  //  const double T4 = ( c1 * ( 1-a1-b1+(a1*b1) ) * d2) * ( c1 * ( 1-a1-b1+(a1*b1) ) * d2);
  //  return sqrt( T1 + T2 + T3 + T4 );
  //} else{
  //  return 0.;
  //}
}


void
LeptonTriggerSF::getDileptonLegs(const std::string& chain,
				 DileptonTrigger& dilepton)
{

  if (chain.find("mu18_tight_mu8_EFFS") != std::string::npos) {
    dilepton.leg1 = "mu18_tight";
    dilepton.leg2 = "mu8_EFFS";
    dilepton.bothLegs = "mu18_tight"; // for now, eventually should be mu18i_tight_or_mu8_EFFS

  } else if (chain.find("2mu13") != std::string::npos) {
    dilepton.leg1 = "mu13";
    dilepton.leg2 = "mu13";
    dilepton.bothLegs = "mu13";

  } else if (chain.find("2e12Tvh_loose1") != std::string::npos) {
    dilepton.leg1 = "e12Tvh_loose1";
    dilepton.leg2 = "e12Tvh_loose1";
    dilepton.bothLegs = "e12Tvh_loose1";

  } else if (chain.find("e12Tvh_medium1_mu8") != std::string::npos) {
    dilepton.leg1 = "e12Tvh_medium1";
    dilepton.leg2 = "mu8";
    dilepton.bothLegs = "e12Tvh_medium1"; // should be an OR. Not implemented yet

  } else {
    dilepton.leg1 = "";
    dilepton.leg2 = "";
    dilepton.bothLegs = "";
    ::Error( "LeptonTriggerSF::getDileptonLegs",
      ERROR_MESSAGE("Invalid dilepton or combination of single and dilepton trigger chain name given"));
  }
}


bool
LeptonTriggerSF::setConfiguration(TrigMuonEff::Configuration& config,
                                  TrigMuonEff::DataPeriod period) const
{
  if (period == TrigMuonEff::period_undefined) {
    period = getDataPeriod(config.runNumber);
    if (m_max_period != TrigMuonEff::period_undefined) {
      if (period > m_max_period) {
        static bool hasWarned = false;
        if (not hasWarned) {
          ::Warning("LeptonTriggerSF::period out of range:", "setting it to %d", period);
          hasWarned = true;
        }
        period = m_max_period;
      }
    }

    if (period == TrigMuonEff::period_undefined) {
      ::Error( "LeptonTriggerSF::setConfiguration",
      ERROR_MESSAGE("RunNumber is not in 2011 or 2012 dataset. No scale factors calculated. "
                    "Please use RunNumber between 178044-191933 for 2011 data, or "
                    "above 200804 (upper run number limit removed for now) for 2012 data. %d" ), config.runNumber );
      return false;
    }
  }

  switch (period) {
    case TrigMuonEff::period_2011_BtoI:
      config.period = "BtoI_";
      config.binning = "fine";
      break;

    case TrigMuonEff::period_2011_JtoMwithoutL3L4:
      config.period = "JtoMwoL3toL4_";
      config.binning = "fine";
      break;

    case TrigMuonEff::period_2011_L3toL4:
      config.period = "L3toL4_";
      config.binning = "coarse";
      break;

    case TrigMuonEff::per2012A:
      config.period = "A_";
      config.binning = "fine";
      config.trigger = "mu24i_tight_or_EF_mu36_tight";
      break;

    case TrigMuonEff::per2012B:
      config.period = "B_";
      config.binning = "fine";
      config.trigger = "mu24i_tight_or_EF_mu36_tight";
      break;

    case TrigMuonEff::per2012CtoE:
      config.period = "CtoE_";
      config.binning = "fine";
      config.trigger = "mu24i_tight_or_EF_mu36_tight";
      break;

    case TrigMuonEff::per2012G:
      config.period = "G_";
      config.binning = "fine";
      config.trigger = "mu24i_tight_or_EF_mu36_tight";
      break;

    case TrigMuonEff::per2012HtoL:
      config.period = "HtoL_";
      config.binning = "fine";
      config.trigger = "mu24i_tight_or_EF_mu36_tight";
      break;

    // speical cases
    case TrigMuonEff::per2011J:
      config.period = "J_";
      config.binning = "coarse";
      break;

    case TrigMuonEff::per2011K:
      config.period = "K_";
      config.binning = "coarse";
      break;

   case TrigMuonEff::per2011J_K:
      config.period = "JtoK_";
      config.binning = "coarse";
      break;

   case TrigMuonEff::per2011J_M:
      config.period = "JtoM_";
      config.binning = "coarse";
      break;

   case TrigMuonEff::per2011L_M:
      config.period = "LtoM_";
      config.binning = "coarse";
      break;

    default:
      ::Error("LeptonTriggerSF::setConfiguration",
      ERROR_MESSAGE("Unknown data period (%i) requested"), period);
      return false;
   }

   return true;
}



void
LeptonTriggerSF::setPhiBoundary2011(const std::vector<std::string> *period)
{
  m_phiBoundaryBarrel = M_PI;
  m_phiBoundaryEndcap = M_PI;

  EfficiencyMap::const_iterator itr = m_efficiencyMap.begin();
  EfficiencyMap::const_iterator end = m_efficiencyMap.end();

  for (; itr != end; ++itr) {
    if (not itr->second) {
      ::Fatal("LeptonTriggerSF::setPhiBoundary2011",
              ERROR_MESSAGE("An internal coding error detected"));
      throw std::runtime_error("An internal coding error detected");
    }

    // setting lower phi to account for unusual binning
    std::string string = "_MuonTrigEff_" + period->at(0) + "_loose_EtaPhi_fine_barrel_data";
    if (itr->first.find( string ) != std::string::npos) {
      m_phiBoundaryBarrel = itr->second->GetYaxis()->GetXmin();
    } 
    string = "_MuonTrigEff_" + period->at(0) + "_loose_EtaPhi_fine_endcap_data";
    if (itr->first.find(string) != std::string::npos) {
      m_phiBoundaryEndcap = itr->second->GetYaxis()->GetXmin();
    }
    if (m_phiBoundaryBarrel < -M_PI) m_phiBoundaryBarrel += 2. * M_PI;
    if (m_phiBoundaryEndcap < -M_PI) m_phiBoundaryEndcap += 2. * M_PI;
 }

 return;

}



void
LeptonTriggerSF::setPhiBoundary(TFile* file)
{
  gROOT->cd();

  m_phiBoundaryBarrel = M_PI;
  m_phiBoundaryEndcap = M_PI;

  TH2* hist = 0;

  const char* barrel = "binning_barrel";
  hist = dynamic_cast< TH2* >(file->Get(barrel));
  if (not hist) {
    ::Warning("LeptonTriggerSF::setPhiBoundary", "%s not available", barrel);

  } else {
    m_phiBoundaryBarrel = hist->GetYaxis()->GetXmin();
  }

  const char* endcap = "binning_endcap";
  hist = dynamic_cast< TH2* >(file->Get(endcap));
  if (not hist) {
    ::Warning("LeptonTriggerSF::setPhiBoundary", "%s not available", endcap);

  } else {
    m_phiBoundaryEndcap = hist->GetYaxis()->GetXmin();
  }


  if (m_phiBoundaryBarrel < -M_PI) m_phiBoundaryBarrel += 2. * M_PI;
  if (m_phiBoundaryEndcap < -M_PI) m_phiBoundaryEndcap += 2. * M_PI;

  return;
}



double
LeptonTriggerSF::fixPhiRange(const TLorentzVector& muon) const
{
  double phi = muon.Phi();

  if (phi > M_PI) {
    ::Warning("LeptonTriggerSF::fixPhiRange",
              "Muon phi %4.2f > pi! Using (phi-2*pi)", phi);
    phi -= 2 * M_PI;

  } else if (phi < -M_PI) {
    ::Warning("LeptonTriggerSF::fixPhiRange",
              "Muon phi %4.2f < -pi! Using (phi+2*pi)", phi);
    phi += 2 * M_PI;
  }


  if (fabs(muon.Eta()) < muon_barrel_endcap_boundary) {  // barrel
    if (m_phiBoundaryBarrel < 0.) {
      if (phi < m_phiBoundaryBarrel) {
        phi += 2 * M_PI;
      }
    } else {
      if (phi >= m_phiBoundaryBarrel) {
        phi -= 2 * M_PI;
      }
    }
  } else {  // endcap
    if (m_phiBoundaryEndcap < 0.) {
      if (phi < m_phiBoundaryEndcap) {
        phi += 2 * M_PI;
      }
    } else {
      if (phi >= m_phiBoundaryEndcap ) {
        phi -= 2 * M_PI;
      }
    }
  }

  return phi;
}



void
LeptonTriggerSF::initialiseElectronTool(const int key) const
{
  if (m_electronSF.find(key) == m_electronSF.end()) {
    Root::TElectronEfficiencyCorrectionTool *tool = new Root::TElectronEfficiencyCorrectionTool;
    if (m_fileNameMap.find(key) == m_fileNameMap.end()) throw std::runtime_error("initialiseElectronTool: key error");
    tool->addFileName(m_fileNameMap[key]);
    tool->initialize();
    m_electronSF.insert(ElectronEfficiencyToolPair(key, tool));
  }
}
// eof
