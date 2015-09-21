/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <utility>

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODEventInfo/EventInfo.h"
#include "PATInterfaces/SystematicRegistry.h"

// internal (old) tool
#include "ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h"

#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"

typedef AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation ParticleInformation;

namespace CP {

EgammaCalibrationAndSmearingTool::EgammaCalibrationAndSmearingTool(const std::string& name)
  : asg::AsgTool(name),
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
  declareProperty("decorrelationModel", m_decorrelation_model_name = "FULL_v1");
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
  declareProperty("useEPCombination", m_use_ep_combination = false);
  declareProperty("useMVACalibration", m_use_mva_calibration = AUTO);
  declareProperty("use_full_statistical_error", m_use_full_statistical_error=false);
  declareProperty("use_temp_correction201215", m_use_temp_correction201215=AUTO);
  declareProperty("use_uA2MeV_2015_first2weeks_correction", m_use_uA2MeV_2015_first2weeks_correction=AUTO);
  declareProperty("useAFII", m_use_AFII=false);
}

StatusCode EgammaCalibrationAndSmearingTool::initialize() {
  ATH_MSG_INFO("Initialization");

  // remove following line after July
  if (m_ESModel == "es2015XX") { ATH_MSG_WARNING("es2015XX will be removed. Use es2015PRE"); }

  if (m_ESModel == "es2010") { m_TESModel = egEnergyCorr::es2010; }           // legacy
  else if (m_ESModel == "es2011c") { m_TESModel = egEnergyCorr::es2011c; }    // mc11c : faulty G4; old geometry
  else if (m_ESModel == "es2011d") { m_TESModel = egEnergyCorr::es2011d; }    // mc11d : corrected G4; new geometry == final Run1 scheme
  else if (m_ESModel == "es2012a") { m_TESModel = egEnergyCorr::es2012a; }    // mc12a : "crude" G4 fix; old geometry
  else if (m_ESModel == "es2012c") { m_TESModel = egEnergyCorr::es2012c; }    // mc12c : corrected G4; new geometry == final Run1 scheme
  else if (m_ESModel == "es2012XX") { m_TESModel = egEnergyCorr::es2012XX; }
  else if (m_ESModel == "es2015PRE" or m_ESModel == "es2015XX") { m_TESModel = egEnergyCorr::es2015PRE; }
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

  if (m_use_AFII) { m_simulation = PATCore::ParticleDataType::Fast; }
  else { m_simulation = PATCore::ParticleDataType::Full; }

  // create the root underlying tool
  m_rootTool.reset(new AtlasRoot::egammaEnergyCorrectionTool());
  if (!m_rootTool) {
    ATH_MSG_ERROR("Cannot initialize underlying tool");
    return StatusCode::FAILURE;
  }

  // configure the underlying tool
  m_rootTool->setESModel(m_TESModel);

  m_rootTool->initialize();

  if (m_MVAfolder != "") m_rootTool->setMVAfolder(m_MVAfolder);
  if (m_useLayerCorrection != AUTO) m_rootTool->useLayerCorrection(m_useLayerCorrection);
  if (m_usePSCorrection != AUTO) m_rootTool->applyPSCorrection(m_usePSCorrection);
  if (m_useS12Correction != AUTO) m_rootTool->applyS12Correction(m_useS12Correction);
  if (m_useLayer2Recalibration != AUTO) m_rootTool->useLayer2Recalibration(m_useLayer2Recalibration);
  if (m_useIntermoduleCorrection != AUTO) m_rootTool->useIntermoduleCorrection(m_useIntermoduleCorrection);
  if (m_usePhiUniformCorrection != AUTO) m_rootTool->usePhiUniformCorrection(m_usePhiUniformCorrection);
  if (m_useGainCorrection != AUTO) m_rootTool->useGainCorrection(m_useGainCorrection);
  if (m_use_mva_calibration != AUTO) m_rootTool->useMVA(m_use_mva_calibration);
  if (m_use_temp_correction201215 != AUTO) m_rootTool->use_temp_correction201215(m_use_temp_correction201215);
  if (m_use_uA2MeV_2015_first2weeks_correction != AUTO) m_rootTool->use_uA2MeV_2015_first2weeks_correction(m_use_uA2MeV_2015_first2weeks_correction);
  if (not m_use_full_statistical_error and m_decorrelation_model_name == "FULL_v1") { m_rootTool->useStatErrorScaling(true); }

  if (m_use_ep_combination) {
    ATH_MSG_ERROR("ep combination not supported yet");
    throw std::runtime_error("ep combination not supported yet");
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
  ATH_MSG_INFO("use AFII = " << m_use_AFII);

  setupSystematics();

  applySystematicVariation(CP::SystematicSet()).ignore();   // this set the flags for the internal tool without systematics
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if ( registry.registerSystematics( *this ) != CP::SystematicCode::Ok ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

StatusCode EgammaCalibrationAndSmearingTool::finalize() {
  ATH_MSG_INFO("finalize");

  return StatusCode::SUCCESS;
}

void EgammaCalibrationAndSmearingTool::setRandomSeed(unsigned seed) {
  m_rootTool->setRandomSeed(seed);
}

PATCore::ParticleType::Type EgammaCalibrationAndSmearingTool::xAOD2ptype(const xAOD::Egamma& particle) const
{
  auto ptype = PATCore::ParticleType::Electron;
  if (xAOD::EgammaHelpers::isElectron(&particle)) { ptype = PATCore::ParticleType::Electron; }
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
  const auto cl_etaCalo = xAOD::get_eta_calo(*particle.caloCluster());

  return m_rootTool->resolution(particle.e(), particle.caloCluster()->eta(),
                                cl_etaCalo, ptype,
                                withCT, false); // TODO: always for full simulation
}

double
EgammaCalibrationAndSmearingTool::resolution(double energy, double cl_eta, double cl_etaCalo,
                                             PATCore::ParticleType::Type ptype) const
{
  return m_rootTool->resolution(energy, cl_eta, cl_etaCalo, ptype, false, false); // TODO: false argument
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


CP::CorrectionCode EgammaCalibrationAndSmearingTool::applyCorrection(xAOD::Egamma & input, const xAOD::EventInfo& event_info)
{
  if (event_info.eventType(xAOD::EventInfo::IS_SIMULATION) and m_auto_reseed) {
    setRandomSeed(m_set_seed_function(*this, input, event_info));
  }

  double new_energy = 0;

  if (const xAOD::Electron *el = dynamic_cast<const xAOD::Electron*>(&input))
  {
    new_energy = getElectronEnergy(el, &event_info);
  }
  else if (const xAOD::Photon *ph = dynamic_cast<const xAOD::Photon*>(&input))
  {
    new_energy = getPhotonEnergy(ph, &event_info);
  }
  else {
    ATH_MSG_ERROR("particle is not electron or photon, cannot calibrate");
    return CP::CorrectionCode::Error;
  }

  ATH_MSG_DEBUG("new energy " << new_energy);
  // TODO: this check should be done before systematics variations
  const double new_energy2 = new_energy * new_energy;
  const double m2 = input.m() * input.m();
  const double p2 = new_energy2 > m2 ? new_energy2 - m2 : 0.;
  input.setPt(sqrt(p2) / cosh(input.eta()));
  return CP::CorrectionCode::Ok;
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

// this function is not used, but useful for testing
double EgammaCalibrationAndSmearingTool::getEnergy(const xAOD::Egamma* p, const xAOD::EventInfo* event_info)
{
  const xAOD::Electron *el = dynamic_cast<const xAOD::Electron*>(p);
  if (el) { return getElectronEnergy(el, event_info); }

  const xAOD::Photon *ph = dynamic_cast<const xAOD::Photon*>(p);
  if (ph) { return getPhotonEnergy(ph, event_info); }

  ATH_MSG_ERROR("particle is not electron of photon");
  return -999;
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

double EgammaCalibrationAndSmearingTool::getElectronEnergy(const xAOD::Electron *el, const xAOD::EventInfo* event_info)
{
  const unsigned int runnumber = event_info->runNumber();
  PATCore::ParticleDataType::DataType dataType = (event_info->eventType(xAOD::EventInfo::IS_SIMULATION)) ? m_simulation : PATCore::ParticleDataType::Data;

  const xAOD::CaloCluster* eCluster = el->caloCluster();
  if (!eCluster) {
    ATH_MSG_ERROR("electron without CaloCluster");
    return -1;
  }

  const float m_el_rawcl_Es0 = eCluster->energyBE(0);
  const float m_el_rawcl_Es1 = eCluster->energyBE(1);
  const float m_el_rawcl_Es2 = eCluster->energyBE(2);
  const float m_el_rawcl_Es3 = eCluster->energyBE(3);
  const float m_el_cl_eta = eCluster->eta();
  const float m_el_cl_phi = eCluster->phi();
  const float m_el_cl_E = eCluster->e();
  const xAOD::TrackParticle* eTrack = el->trackParticle();
  if (not eTrack) {
    ATH_MSG_ERROR("cannot find track from electron");
    throw std::runtime_error("cannot find track from electron");
  }
  const float m_el_tracketa = eTrack->eta();     // TODO: why not electron->eta() ?

  const float m_el_cl_etaCalo = xAOD::get_eta_calo(*eCluster, false);
  const float m_el_cl_phiCalo = xAOD::get_phi_calo(*eCluster, false);


  return m_rootTool->getCorrectedEnergy(runnumber,
					dataType,
					ParticleInformation(m_el_rawcl_Es0,
							    m_el_rawcl_Es1,
							    m_el_rawcl_Es2,
							    m_el_rawcl_Es3,
							    m_el_cl_eta,
							    m_el_cl_phi,
							    m_el_tracketa,
							    m_el_cl_E,
							    m_el_cl_etaCalo,
							    m_el_cl_phiCalo),
          oldtool_scale_flag_this_event(*el, *event_info),
					oldtool_resolution_flag_this_event(*el, *event_info),
					m_TResolutionType,
					m_varSF);

}

double EgammaCalibrationAndSmearingTool::getPhotonEnergy(const xAOD::Photon *ph,const xAOD::EventInfo* event_info)
{
  const unsigned int runnumber = event_info->runNumber();
  PATCore::ParticleDataType::DataType dataType = (event_info->eventType(xAOD::EventInfo::IS_SIMULATION)) ? m_simulation : PATCore::ParticleDataType::Data;

  const xAOD::CaloCluster* eCluster = ph->caloCluster();
  if (!eCluster) {
    ATH_MSG_ERROR("photon without CaloCluster");
    return -1;
  }
  const float m_ph_rawcl_Es0 = eCluster->energyBE(0);
  const float m_ph_rawcl_Es1 = eCluster->energyBE(1);
  const float m_ph_rawcl_Es2 = eCluster->energyBE(2);
  const float m_ph_rawcl_Es3 = eCluster->energyBE(3);
  const float m_ph_cl_eta = eCluster->eta();
  const float m_ph_cl_phi = eCluster->phi();
  const float m_ph_cl_E = eCluster->e();
  const float m_ph_cl_etaCalo = xAOD::get_eta_calo(*eCluster, false);
  const float m_ph_cl_phiCalo = xAOD::get_phi_calo(*eCluster, false);

  // initialize conversion variables as for unconv
  float m_ph_ptconv = 0.;
  float m_ph_pt1conv = 0.;
  float m_ph_pt2conv = 0.;
  int m_ph_convtrk1nPixHits = 0;
  int m_ph_convtrk1nSCTHits = 0;
  int m_ph_convtrk2nPixHits = 0;
  int m_ph_convtrk2nSCTHits = 0;
  float m_ph_Rconv = 0.;
  //float m_ph_rawcl_calibHitsShowerDepth = -999.;

  // conversion
  const xAOD::Vertex* phVertex = ph->vertex();
  if (phVertex) {
    const Amg::Vector3D pos = phVertex->position();
    m_ph_Rconv = static_cast<float>(hypot(pos.x(), pos.y()));

    const xAOD::TrackParticle* tp0 = phVertex->trackParticle(0);
    const xAOD::TrackParticle* tp1 = phVertex->trackParticle(1);

    TLorentzVector sum;
    if (tp0) {
      sum += tp0->p4();
      uint8_t hits;
      tp0->summaryValue(hits, xAOD::numberOfPixelHits);
      m_ph_convtrk1nPixHits = hits;
      tp0->summaryValue(hits, xAOD::numberOfSCTHits);
      m_ph_convtrk1nSCTHits = hits;

      m_ph_pt1conv = static_cast<float>(tp0->pt());
    }

    if (tp1) {
      sum += tp1->p4();
      uint8_t hits;
      tp1->summaryValue(hits, xAOD::numberOfPixelHits);
      m_ph_convtrk2nPixHits = hits;
      tp1->summaryValue(hits, xAOD::numberOfSCTHits);
      m_ph_convtrk2nSCTHits = hits;

      m_ph_pt2conv = static_cast<float>(tp1->pt());
    }

    m_ph_ptconv = sum.Perp();
  }

  return m_rootTool->getCorrectedEnergy(runnumber,
					dataType,
					ParticleInformation(m_ph_rawcl_Es0,
							    m_ph_rawcl_Es1,
							    m_ph_rawcl_Es2,
							    m_ph_rawcl_Es3,
							    m_ph_cl_eta,
							    m_ph_cl_phi,
							    m_ph_cl_E,
							    m_ph_cl_etaCalo,
							    m_ph_cl_phiCalo,
							    m_ph_ptconv,
							    m_ph_pt1conv,
							    m_ph_pt2conv,
							    m_ph_convtrk1nPixHits,
							    m_ph_convtrk1nSCTHits,
							    m_ph_convtrk2nPixHits,
							    m_ph_convtrk2nSCTHits,
							    m_ph_Rconv),
					oldtool_scale_flag_this_event(*ph, *event_info),
					oldtool_resolution_flag_this_event(*ph, *event_info),
					m_TResolutionType,
					m_varSF);

}

double EgammaCalibrationAndSmearingTool::getElectronMomentum(const xAOD::Electron *el, const xAOD::EventInfo* event_info)
{
  PATCore::ParticleDataType::DataType dataType = (event_info->eventType(xAOD::EventInfo::IS_SIMULATION)) ? m_simulation : PATCore::ParticleDataType::Data;

  const xAOD::TrackParticle* eTrack = el->trackParticle();

  // track momentum and eta
  const float m_el_tracketa = eTrack->eta();
  const float m_el_trackmomentum = eTrack->pt() * cosh(el->eta());

  return m_rootTool->getCorrectedMomentum(dataType,
					  PATCore::ParticleType::Electron,
					  m_el_trackmomentum,
					  m_el_tracketa,
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

  if (m_decorrelation_model_name == "1NP_v1") {
    // TODO: independet implementation of ALL UP looping on all the variations
    m_syst_description[CP::SystematicVariation("EG_SCALE_ALL", +1)] = SysInfo{always, egEnergyCorr::Scale::AllUp};
    m_syst_description[CP::SystematicVariation("EG_SCALE_ALL", -1)] = SysInfo{always, egEnergyCorr::Scale::AllDown};
  }
  else if (m_decorrelation_model_name == "FULL_ETACORRELATED_v1") {
    // all the physical effects separately, considered as fully correlated in eta
    #define SYSMACRO(name, fullcorrelated, decorrelation, flagup, flagdown)                \
      m_syst_description[CP::SystematicVariation(#name, +1)] = SysInfo{always, flagup};    \
      m_syst_description[CP::SystematicVariation(#name, -1)] = SysInfo{always, flagdown};
    #include "ElectronPhotonFourMomentumCorrection/systematics.def"
    #undef SYSMACRO
    // Zee stat is not included in the macro list, add by hand
    m_syst_description[CP::SystematicVariation("EG_SCALE_ZEESTAT", +1)] = SysInfo{always, egEnergyCorr::Scale::ZeeStatUp};
    m_syst_description[CP::SystematicVariation("EG_SCALE_ZEESTAT", -1)] = SysInfo{always, egEnergyCorr::Scale::ZeeStatDown};
  }
  else if (m_decorrelation_model_name == "FULL_v1") {
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
  }
  else {
    ATH_MSG_ERROR("decorrelation model invalid");
  }

  // resolution systematics
  if (m_decorrelation_model_name == "1NP_v1") {
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_ALL", +1)] = egEnergyCorr::Resolution::AllUp;
    m_syst_description_resolution[CP::SystematicVariation("EG_RESOLUTION_ALL", -1)] = egEnergyCorr::Resolution::AllDown;
  }
  else {
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


} // namespace CP
