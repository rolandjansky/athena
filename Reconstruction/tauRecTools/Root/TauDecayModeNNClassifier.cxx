/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/TauDecayModeNNClassifier.h"

// helper function include(s)
#include "PathResolver/PathResolver.h"

// standard library include(s)
#include <functional>
#include <algorithm>
#include <fstream>

using PFOPtr = const xAOD::PFO *;
using TrkPtr = const xAOD::TauTrack *;
using PFOAttributes = xAOD::PFODetails::PFOAttributes;
using DMVar = tauRecTools::TauDecayModeNNVariable;
using DMHelper = tauRecTools::TauDecayModeNNHelper;
using ValueMap = std::map<std::string, double>;
using VectorMap = std::map<std::string, std::vector<double>>;
using InputMap = std::map<std::string, ValueMap>;
using InputSequenceMap = std::map<std::string, VectorMap>;

TauDecayModeNNClassifier::TauDecayModeNNClassifier(const std::string &name)
    : TauRecToolBase(name)
{
  declareProperty("OutputName", m_outputName = "NNDecayMode");
  declareProperty("ProbPrefix", m_probPrefix = "NNDecayModeProb_");
  declareProperty("WeightFile", m_weightFile = "");
  declareProperty("MaxChargedPFOs", m_maxChargedPFOs = 3);
  declareProperty("MaxNeutralPFOs", m_maxNeutralPFOs = 8);
  declareProperty("MaxShotPFOs", m_maxShotPFOs = 6);
  declareProperty("MaxConvTracks", m_maxConvTracks = 4);
  declareProperty("NeutralPFOPtCut", m_neutralPFOPtCut = 1.5);
  declareProperty("EnsureTrackConsistency", m_ensureTrackConsistency = true);
  declareProperty("DecorateProb", m_decorateProb = true);
}

TauDecayModeNNClassifier::~TauDecayModeNNClassifier()
{
}

StatusCode TauDecayModeNNClassifier::initialize()
{
  ATH_MSG_INFO("Initializing TauDecayModeNNClassifier");

  // find input JSON file
  std::string weightFile = find_file(m_weightFile);
  if (weightFile.empty())
  {
    ATH_MSG_ERROR("Could not find network weights: " << m_weightFile);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Loaded network configuration from: " << weightFile);

  // load lwt graph configuration
  std::ifstream inputFile(weightFile);
  lwt::GraphConfig lwtGraphConfig;
  try
  {
    lwtGraphConfig = lwt::parse_json_graph(inputFile);
  }
  catch (const std::logic_error &e)
  {
    ATH_MSG_ERROR("Error parsing network config: " << e.what());
    return StatusCode::FAILURE;
  }

  // configure neural network
  try
  {
    m_lwtGraph = std::make_unique<lwt::LightweightGraph>(lwtGraphConfig, lwtGraphConfig.outputs.cbegin()->first);
  }
  catch (const lwt::NNConfigurationException &e)
  {
    ATH_MSG_ERROR("Error configuring network: " << e.what());
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TauDecayModeNNClassifier::execute(xAOD::TauJet &xTau) const
{
  // inputs
  // ------
  // m_inputMap will not hold any information,
  // but it is required by the lwtnn API.
  //
  InputMap inputMapDummy;
  InputSequenceMap inputSeqMap;
  std::set<std::string> branches = {"ChargedPFO", "NeutralPFO", "ShotPFO", "ConvTrack"};
  DMHelper::initMapKeys(inputSeqMap, branches);

  ATH_CHECK(getInputs(xTau, inputSeqMap));

  // output
  // ------
  ValueMap outputs;

  // inference
  // ---------
  try
  {
    outputs = m_lwtGraph->compute(inputMapDummy, inputSeqMap);
  }
  catch (const std::exception &e)
  {
    ATH_MSG_ERROR("Error evaluating the network: " << e.what());
    return StatusCode::FAILURE;
  }

  // Results
  // -------
  // Decay modes are "1p0n", "1p1n", "1pXn", "3p0n", "3pXn",
  // here they are encoded as 0, 1, 2, 3, 4
  //
  std::array<float, DMVar::nClasses> probs;
  // the prefix to match to output name in the json weight file
  std::string prefix = "c_";
  for (std::size_t i = 0; i < probs.size(); ++i)
  {
    probs[i] = outputs.at(prefix + DMVar::sModeNames[i]);
  }

  // Determine decay mode from classification results
  // If requested: ensures consistency between reconstructed number of tracks and decay mode
  // For non 1 / 3-track taus, classification is performed by maximum mode probability
  //
  std::array<float, DMVar::nClasses>::const_iterator itMax;
  if (m_ensureTrackConsistency && xTau.nTracks() == 1)
  {
    // maximum probability of "1p0n", "1p1n", "1pXn"
    itMax = std::max_element(probs.cbegin(), probs.cbegin() + 3);
  }
  else if (m_ensureTrackConsistency && xTau.nTracks() == 3)
  {
    // maximum probability of "3p0n", "3pXn"
    itMax = std::max_element(probs.cbegin() + 3, probs.cend());
  }
  else
  {
    // maximum probability of all
    itMax = std::max_element(probs.cbegin(), probs.cend());
  }

  const SG::AuxElement::Accessor<int> accDecayMode(m_outputName);
  accDecayMode(xTau) = std::distance(probs.cbegin(), itMax);

  if (m_decorateProb)
  {
    for (std::size_t i = 0; i < probs.size(); ++i)
    {
      const std::string probName = m_probPrefix + DMVar::sModeNames[i];
      const SG::AuxElement::Accessor<float> accProb(probName);
      accProb(xTau) = probs[i];
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TauDecayModeNNClassifier::finalize()
{
  m_lwtGraph.reset(nullptr);

  return StatusCode::SUCCESS;
}

StatusCode TauDecayModeNNClassifier::getInputs(const xAOD::TauJet &xTau, InputSequenceMap &inputSeqMap) const
{
  std::vector<PFOPtr> vChargedPFOs;
  std::vector<PFOPtr> vNeutralPFOs;
  std::vector<PFOPtr> vShotPFOs;
  std::vector<TrkPtr> vConvTracks;

  // set objects
  // -----------

  // charged PFOs
  for (std::size_t i = 0; i < xTau.nChargedPFOs(); ++i)
  {
    const auto pfo = xTau.chargedPFO(i);
    vChargedPFOs.push_back(pfo);
  }

  // neutral PFOs
  for (std::size_t i = 0; i < xTau.nNeutralPFOs(); ++i)
  {
    const auto pfo = xTau.neutralPFO(i);
    // Apply pt threshold
    if (pfo->pt() < m_neutralPFOPtCut * 1e3)
      continue;
    vNeutralPFOs.push_back(pfo);
  }

  // shot PFOs
  for (std::size_t i = 0; i < xTau.nShotPFOs(); ++i)
  {
    const auto pfo = xTau.shotPFO(i);
    // skip PFOs without photons
    int nPhotons{-1};
    try
    {
      nPhotons = DMVar::pfoAttr<int>(pfo, PFOAttributes::tauShots_nPhotons);
    }
    catch (const std::exception &e)
    {
      ATH_MSG_ERROR("Error retrieving tauShots_nPhotons: " << e.what());
      return StatusCode::FAILURE;
    }
    if (nPhotons < 1)
      continue;
    vShotPFOs.push_back(pfo);
  }

  // conversion tracks
  vConvTracks = xTau.tracks(xAOD::TauJetParameters::TauTrackFlag::classifiedConversion);

  DMHelper::sortAndKeep<PFOPtr>(vChargedPFOs, m_maxChargedPFOs);
  DMHelper::sortAndKeep<PFOPtr>(vNeutralPFOs, m_maxNeutralPFOs);
  DMHelper::sortAndKeep<PFOPtr>(vShotPFOs, m_maxShotPFOs);
  DMHelper::sortAndKeep<TrkPtr>(vConvTracks, m_maxConvTracks);

  // set variables
  // -------------

  // tau variables
  const TLorentzVector &tau_p4 = xTau.p4(xAOD::TauJetParameters::TauCalibType::IntermediateAxis);

  // pair: (1st) the value, (2nd) successfully retrieved
  std::pair<float, bool> tau_etaTrkECal{0., false};
  std::pair<float, bool> tau_phiTrkECal{0., false};
  if (xTau.nTracks() > 0)
  {
    TrkPtr trk = xTau.track(0);
    if (!trk->detail(xAOD::TauJetParameters::CaloSamplingPhiEM, tau_phiTrkECal.first))
    {
      ATH_MSG_WARNING("Failed to retrieve extrapolated track phi in ECal");
    }
    else
    {
      tau_phiTrkECal.second = true;
    }
    if (!trk->detail(xAOD::TauJetParameters::CaloSamplingEtaEM, tau_etaTrkECal.first))
    {
      ATH_MSG_WARNING("Failed to retrieve extrapolated track eta in ECal");
    }
    else
    {
      tau_etaTrkECal.second = true;
    }
  }

  // a function to set the common 4-momentum variables, this is needed for all later
  auto setCommonP4Vars = [&tau_p4, &tau_etaTrkECal, &tau_phiTrkECal](VectorMap &in_seq_map, const TLorentzVector &obj_p4) {
    in_seq_map["dphiECal"].push_back(DMVar::deltaPhiECal(obj_p4, tau_phiTrkECal));
    in_seq_map["detaECal"].push_back(DMVar::deltaEtaECal(obj_p4, tau_etaTrkECal));
    in_seq_map["dphi"].push_back(DMVar::deltaPhi(obj_p4, tau_p4));
    in_seq_map["deta"].push_back(DMVar::deltaEta(obj_p4, tau_p4));
    in_seq_map["pt_log"].push_back(DMHelper::Log10Robust(obj_p4.Pt()));
    in_seq_map["jetpt_log"].push_back(DMHelper::Log10Robust(tau_p4.Pt()));
  };

  // a function to set the neutral pfo variables
  auto setNeutralPFOVars = [](VectorMap &in_seq_map, const PFOPtr &pfo) {
    // get the attributes of a given PFO object
    auto getAttr = std::bind(DMVar::pfoAttr<float>, pfo, std::placeholders::_1);
    auto getAttrInt = std::bind(DMVar::pfoAttr<int>, pfo, std::placeholders::_1);

    in_seq_map["FIRST_ETA"].push_back(getAttr(PFOAttributes::cellBased_FIRST_ETA));
    in_seq_map["SECOND_R_log"].push_back(DMHelper::Log10Robust(getAttr(PFOAttributes::cellBased_SECOND_R), 1e-3f));
    in_seq_map["DELTA_THETA"].push_back(getAttr(PFOAttributes::cellBased_DELTA_THETA));
    in_seq_map["CENTER_LAMBDA_log"].push_back(DMHelper::Log10Robust(getAttr(PFOAttributes::cellBased_CENTER_LAMBDA), 1e-3f));
    in_seq_map["LONGITUDINAL"].push_back(getAttr(PFOAttributes::cellBased_LONGITUDINAL));
    in_seq_map["ENG_FRAC_CORE"].push_back(getAttr(PFOAttributes::cellBased_ENG_FRAC_CORE));
    in_seq_map["SECOND_ENG_DENS_log"].push_back(DMHelper::Log10Robust(getAttr(PFOAttributes::cellBased_SECOND_ENG_DENS), 1e-6f));
    in_seq_map["NPosECells_EM1"].push_back(getAttrInt(PFOAttributes::cellBased_NPosECells_EM1));
    in_seq_map["NPosECells_EM2"].push_back(getAttrInt(PFOAttributes::cellBased_NPosECells_EM2));
    in_seq_map["EM1CoreFrac"].push_back(getAttr(PFOAttributes::cellBased_EM1CoreFrac));
    in_seq_map["firstEtaWRTClusterPosition_EM1"].push_back(getAttr(PFOAttributes::cellBased_firstEtaWRTClusterPosition_EM1));
    in_seq_map["secondEtaWRTClusterPosition_EM1_log"].push_back(DMHelper::Log10Robust(getAttr(PFOAttributes::cellBased_secondEtaWRTClusterPosition_EM1), 1e-6f));
    in_seq_map["secondEtaWRTClusterPosition_EM2_log"].push_back(DMHelper::Log10Robust(getAttr(PFOAttributes::cellBased_secondEtaWRTClusterPosition_EM2), 1e-6f));
    in_seq_map["ptSubRatio_logabs"].push_back(DMHelper::Log10Robust(TMath::Abs(DMVar::ptSubRatio(pfo)), 1e-6f));
    in_seq_map["energyfrac_EM2"].push_back(DMVar::energyFracEM2(pfo, getAttr(PFOAttributes::cellBased_energy_EM2)));
  };

  // set Charged PFOs variables
  VectorMap &chrg_map = inputSeqMap.at("ChargedPFO");
  DMHelper::initMapKeys(chrg_map, DMVar::sCommonP4Vars);
  for (const auto &pfo : vChargedPFOs)
  {
    setCommonP4Vars(chrg_map, pfo->p4());
  }

  // set Neutral PFOs variables
  VectorMap &neut_map = inputSeqMap.at("NeutralPFO");
  DMHelper::initMapKeys(neut_map, DMVar::sCommonP4Vars);
  DMHelper::initMapKeys(neut_map, DMVar::sNeutralPFOVars);
  for (const auto &pfo : vNeutralPFOs)
  {
    setCommonP4Vars(neut_map, pfo->p4());
    try
    {
      setNeutralPFOVars(neut_map, pfo);
    }
    catch (const std::exception &e)
    {
      ATH_MSG_ERROR("Error setting neutral PFO variables: " << e.what());
      return StatusCode::FAILURE;
    }
  }

  // set Shot PFOs variables
  VectorMap &shot_map = inputSeqMap.at("ShotPFO");
  DMHelper::initMapKeys(shot_map, DMVar::sCommonP4Vars);
  for (const auto &pfo : vShotPFOs)
  {
    setCommonP4Vars(shot_map, pfo->p4());
  }

  // set Conversion tracks variables
  VectorMap &conv_map = inputSeqMap.at("ConvTrack");
  DMHelper::initMapKeys(conv_map, DMVar::sCommonP4Vars);
  for (const auto &trk : vConvTracks)
  {
    setCommonP4Vars(conv_map, trk->p4());
  }

  return StatusCode::SUCCESS;
}

// Helper functions
namespace tauRecTools
{
  const std::set<std::string> TauDecayModeNNVariable::sCommonP4Vars = {
      "dphiECal", "detaECal", "dphi", "deta", "pt_log", "jetpt_log"};

  const std::set<std::string> TauDecayModeNNVariable::sNeutralPFOVars = {
      "FIRST_ETA", "SECOND_R_log", "DELTA_THETA", "CENTER_LAMBDA_log", "LONGITUDINAL", "ENG_FRAC_CORE",
      "SECOND_ENG_DENS_log", "NPosECells_EM1", "NPosECells_EM2", "EM1CoreFrac", "firstEtaWRTClusterPosition_EM1",
      "secondEtaWRTClusterPosition_EM1_log", "secondEtaWRTClusterPosition_EM2_log", "ptSubRatio_logabs", "energyfrac_EM2"};

  const std::array<std::string, TauDecayModeNNVariable::nClasses> TauDecayModeNNVariable::sModeNames = {
      "1p0n", "1p1n", "1pXn", "3p0n", "3pXn"};

  float TauDecayModeNNVariable::deltaPhi(const TLorentzVector &p4, const TLorentzVector &p4_tau)
  {
    return p4_tau.DeltaPhi(p4);
  }

  float TauDecayModeNNVariable::deltaEta(const TLorentzVector &p4, const TLorentzVector &p4_tau)
  {
    return p4.Eta() - p4_tau.Eta();
  }

  float TauDecayModeNNVariable::deltaPhiECal(const TLorentzVector &p4, const std::pair<float, bool> &tau_phiTrkECal)
  {
    // if not retrieved, then set to 0. (mean value)
    return tau_phiTrkECal.second ? TVector2::Phi_mpi_pi(p4.Phi() - tau_phiTrkECal.first) : 0.0f;
  }

  float TauDecayModeNNVariable::deltaEtaECal(const TLorentzVector &p4, const std::pair<float, bool> &tau_etaTrkECal)
  {
    // if not retrieved, then set to 0. (mean value)
    return tau_etaTrkECal.second ? p4.Eta() - tau_etaTrkECal.first : 0.0f;
  }

  template <typename T>
  T TauDecayModeNNVariable::pfoAttr(const PFOPtr pfo, const PFOAttributes &attr)
  {
    T val{static_cast<T>(0)};
    if (!pfo->attribute(attr, val))
    {
      throw std::runtime_error("Can not retrieve PFO attribute!");
    }
    return val;
  }

  float TauDecayModeNNVariable::ptSubRatio(const PFOPtr pfo)
  {
    float clus0pt = pfo->cluster(0)->pt();
    return clus0pt > 0.0f ? (clus0pt - pfo->pt()) / clus0pt : 0.0f;
  }

  float TauDecayModeNNVariable::energyFracEM2(const PFOPtr pfo, float energy_em2)
  {
    float clus0e = pfo->cluster(0)->e();
    return clus0e > 0.0f ? energy_em2 / clus0e : 0.0f;
  }

  float TauDecayModeNNHelper::Log10Robust(const float val, const float min_val)
  {
    return TMath::Log10(std::max(val, min_val));
  }

  template <typename T>
  void TauDecayModeNNHelper::sortAndKeep(std::vector<T> &vec, const std::size_t n_obj)
  {
    auto cmp_pt = [](const T lhs, const T rhs) { return lhs->pt() > rhs->pt(); };
    std::sort(vec.begin(), vec.end(), cmp_pt);
    if (vec.size() > n_obj)
    {
      vec.erase(vec.begin() + n_obj, vec.end());
    }
  }

  template <typename T>
  void TauDecayModeNNHelper::initMapKeys(std::map<std::string, T> &empty_map,
                                         const std::set<std::string> &keys)
  {
    // T can be any type
    for (const auto &key : keys)
    {
      empty_map[key];
    }
  }
} // namespace tauRecTools
