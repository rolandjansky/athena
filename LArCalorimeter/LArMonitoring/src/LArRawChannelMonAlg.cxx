/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawChannelMonAlg.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODEventInfo/EventInfo.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnlID_Exception.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRecEvent/LArEventBitInfo.h"
#include "AthenaKernel/Units.h"
#include "StoreGate/ReadCondHandle.h"
#include "CaloConditions/CaloNoise.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <complex>

using namespace std::complex_literals;

namespace {

enum : int8_t {
  EMBA,
  EMBC,
  EMECA,
  EMECC,
  HECA,
  HECC,
  FCALA,
  FCALC,
  NDETECTORS,
  UNDEF = -1
};

constexpr unsigned numberOfSlotsPerFeedthrough(int8_t det) {
  bool b{det == ::EMBA || det == ::EMBC};
  return b ? 14 : 15;
}

constexpr std::array<int8_t, 8> partitions() {
  return {::EMBA, ::EMBC, ::EMECA, ::EMECC,
          ::HECA, ::HECC, ::FCALA, ::FCALC};
}

} // namespace


LArRawChannelMonAlg::LArRawChannelMonAlg(const std::string &name,
                                         ISvcLocator *pSvcLocator)
    : AthMonitorAlgorithm(name, pSvcLocator) {
  using T = decltype(m_det_to_nchannels);
  static_assert(std::tuple_size_v<T> >= ::NDETECTORS);
  using U = decltype(m_monitoring_tool_index);
  static_assert(std::tuple_size_v<U> >= ::NDETECTORS);
}


LArRawChannelMonAlg::~LArRawChannelMonAlg() {}


StatusCode LArRawChannelMonAlg::initialize()
{
  ATH_MSG_DEBUG("===> start " << name() << "::initialize <=== ");

  ATH_CHECK(m_EventInfoKey.initialize());
  ATH_CHECK(m_LArRawChannel_container_key.initialize());
  ATH_CHECK(detStore()->retrieve(m_lar_online_id_ptr, "LArOnlineID"));
  ATH_CHECK(detStore()->retrieve(m_calo_id_mgr_ptr));
  ATH_CHECK(m_bcContKey.initialize());
  ATH_CHECK(m_bcMask.buildBitMask(m_problemsToMask, msg()));
  ATH_CHECK(m_atlasReady_tools.retrieve());
  ATH_CHECK(m_noiseKey.initialize());
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_caloMgrKey.initialize());

  auto get_detector = [&](auto hwid) {
    const bool sideA = m_lar_online_id_ptr->pos_neg(hwid);
    if (m_lar_online_id_ptr->isEMBchannel(hwid))
      return sideA ? ::EMBA : ::EMBC;
    else if (m_lar_online_id_ptr->isEMECchannel(hwid))
      return sideA ? ::EMECA : ::EMECC;
    else if (m_lar_online_id_ptr->isHECchannel(hwid))
      return sideA ? ::HECA : ::HECC;
    else if (m_lar_online_id_ptr->isFCALchannel(hwid))
      return sideA ? ::FCALA : ::FCALC;
    return ::UNDEF; };

  // Create FEB hash -> Detector map
  std::size_t feb_hash_max = m_lar_online_id_ptr->febHashMax();
  m_feb_hash_to_detector.resize(feb_hash_max, ::UNDEF);
  auto end_feb = m_lar_online_id_ptr->feb_end();
  for (auto itr = m_lar_online_id_ptr->feb_begin(); itr != end_feb; ++itr) {
    IdentifierHash feb_hash = m_lar_online_id_ptr->feb_Hash(*itr);
    auto det = get_detector(*itr);
    if (feb_hash < m_feb_hash_to_detector.size()) {
      m_feb_hash_to_detector.at(feb_hash) = det;
    } else {
      ATH_MSG_WARNING("FEB hash out of range, ignored.");
    }
  }

  // Count number of channels in each detector ---
  std::fill(m_det_to_nchannels.begin(), m_det_to_nchannels.end(), 0);
  auto citr = m_lar_online_id_ptr->channel_begin();
  auto citr_end = m_lar_online_id_ptr->channel_end();
  for (; citr != citr_end; ++citr) {
    // TODO: skip unconnected/masked channels, but these may depend on IOV...
    auto det = get_detector(*citr);
    if (det != ::UNDEF)
      m_det_to_nchannels[det] += 1;
  }

  std::vector<std::string> det2str(::NDETECTORS);
  det2str[::EMBA] = "EMBA";
  det2str[::EMBC] = "EMBC";
  det2str[::EMECA] = "EMECA";
  det2str[::EMECC] = "EMECC";
  det2str[::HECA] = "HECA";
  det2str[::HECC] = "HECC";
  det2str[::FCALA] = "FCalA";
  det2str[::FCALC] = "FCalC";

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("Number of channels in detectors: ");
    for (int8_t det : partitions()) {
      auto n = m_det_to_nchannels[det];
      ATH_MSG_DEBUG(det2str[det] << " has " << n << "channels ");
    }
  }

  m_noise_streams_set.insert(m_noise_streams.begin(), m_noise_streams.end());

  auto toolmap = Monitored::buildToolMap<int>(
      m_tools, "LArRawChannelMon", det2str);
  for (int8_t det : partitions()) {
    m_monitoring_tool_index[det] = toolmap.at(det2str[det]);
  }

  ATH_MSG_DEBUG("===> end " << name() << "::initialize, "
                "will now initialize base class  <=== ");
  return AthMonitorAlgorithm::initialize();
}


StatusCode LArRawChannelMonAlg::fillHistograms(const EventContext &ctx) const
{
  ATH_MSG_DEBUG("===> start " << name() << "::fillHistograms() <=== ");

  // Retrieve raw channels
  SG::ReadHandle<LArRawChannelContainer> raw_channels{
      m_LArRawChannel_container_key, ctx};
  if (!raw_channels.isValid()) {
    ATH_MSG_WARNING("Cannot retrieve LArRawChannelContainer with key: "
                    << m_LArRawChannel_container_key.key());
    return StatusCode::SUCCESS;
  }

  const bool is_atlas_ready = std::all_of(
      m_atlasReady_tools.begin(),
      m_atlasReady_tools.end(),
      [](auto &f) { return f->accept(); });

  SG::ReadHandle<xAOD::EventInfo> event_info{GetEventInfo(ctx)};
  int bcid{0}, lumi_block{0};
  bool larNoisyROAlg_flag{false};
  bool larNoisyROAlgInTimeW_flag{false};
  bool larNoisyROAlg_W_flag{false};
  bool noisy_event{false};
  if (event_info.isValid()) {
    auto checkEventFlag = [&](auto bitinfo, const char *txt) {
      bool flag = event_info->isEventFlagBitSet(
        xAOD::EventInfo::LAr, bitinfo);
      if (flag) ATH_MSG_DEBUG(" !!! Noisy event found " << txt << " !!!");
      return flag; };
    larNoisyROAlg_flag = checkEventFlag(
        LArEventBitInfo::BADFEBS, "from LArNoisyROAlg");
    larNoisyROAlg_W_flag = checkEventFlag(
        LArEventBitInfo::BADFEBS_W, "from LArNoisyROAlg_W");
    larNoisyROAlgInTimeW_flag = checkEventFlag(
        3, "by LArNoisyROAlg in Time window of 500ms");
    bcid = event_info->bcid();
    lumi_block = event_info->lumiBlock();

    const auto &tags = event_info->streamTags();
    auto inSet = [&](auto &x){ return m_noise_streams_set.count(x.name()); };
    noisy_event = m_noise_streams_set.empty()
        || std::any_of(tags.begin(), tags.end(), inSet);
  } else {
    ATH_MSG_DEBUG("Cannot retrieve EventInfo");
  }

  std::array<uint32_t, ::NDETECTORS> det_n_noisy_channels{};
  std::array<uint32_t, ::NDETECTORS> det_n_noisy_channels_Neg{};
  std::array<uint32_t, ::NDETECTORS> det_n_badQ_channels{};
  using wsum_t = std::complex<double>;
  wsum_t event_mean_time{};
  std::array<wsum_t, ::NDETECTORS> mean_detector_times;
  std::vector<wsum_t> mean_feb_times(m_feb_hash_to_detector.size(), 0.);
  std::array<double, ::NDETECTORS> per_detector_total_energy{};
  int8_t lastdet{::UNDEF};
  ToolHandle<GenericMonitoringTool> monitoring{nullptr};
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrH{m_caloMgrKey, ctx};
  SG::ReadCondHandle<CaloNoise> noiseH{m_noiseKey, ctx};
  SG::ReadCondHandle<LArBadChannelCont> bcContH{m_bcContKey, ctx};
  SG::ReadCondHandle<LArOnOffIdMapping> cablingH{m_cablingKey, ctx};
  ATH_CHECK(caloMgrH.isValid());
  ATH_CHECK(noiseH.isValid());
  ATH_CHECK(bcContH.isValid());
  ATH_CHECK(cablingH.isValid());

  Monitored::Scalar<int> dqm_superslot{"S", -1};
  Monitored::Scalar<int> dqm_channel{"C", -1};
  Monitored::Scalar<float> dqm_posn{"posn", 0};
  Monitored::Scalar<float> dqm_negn{"negn", 0};
  Monitored::Scalar<float> dqm_qual{"Q4k", 0};
  Monitored::Scalar<float> dqm_energy{"E", 0};
  Monitored::Scalar<int> dqm_gain{"G", 0};
  Monitored::Scalar<bool> dqmf_occ{"occ", false};
  Monitored::Scalar<bool> dqmf_sig{"sig", false};

  for (const LArRawChannel &chan : *raw_channels) {
    HWIdentifier hardware_id{chan.hardwareID()};
    Identifier offline_id{0};
    HWIdentifier feb_id{0};
    IdentifierHash feb_hash{0};
    int channel{-1};
    int slot_number{-1}, feedthrough_number{-1};
    int8_t det{::UNDEF};
    float energy, time, noise, significance;
    int gain{-1};
    bool bad_quality;

    try {
      offline_id = cablingH->cnvToIdentifier(hardware_id);
      const CaloDetDescrElement *dde = caloMgrH->get_element(offline_id);
      // Skip unconnected or masked channels ---
      if (!dde) continue;
      if (m_bcMask.cellShouldBeMasked(*bcContH, hardware_id)) continue;
      // Monitor properly reconstructed channels only:
      // - provenance&0x00ff == 0x00a5:
      //   raw channels from OFC iteration, all calib constants found in DB
      // - provenance&0x1000 == 0x1000:
      //   raw channels from DSP. If no constant loaded in DSP, energy==0
      if (!m_db_and_ofc_only
          && (chan.provenance() & 0x00ff) != 0x00A5
          && (chan.provenance() & 0x1000) != 0x1000) {
        continue;
      }

      feb_id = m_lar_online_id_ptr->feb_Id(hardware_id);
      feb_hash = m_lar_online_id_ptr->feb_Hash(feb_id);
      det = m_feb_hash_to_detector.at(feb_hash);
      if (det != lastdet) {
        if (det >= 0 && det < ::NDETECTORS) {
          monitoring = m_tools[m_monitoring_tool_index[det]];
        } else {
          monitoring = nullptr;
        }
        lastdet = det;
      }

      slot_number = m_lar_online_id_ptr->slot(hardware_id);
      feedthrough_number = m_lar_online_id_ptr->feedthrough(hardware_id);
      channel = m_lar_online_id_ptr->channel(hardware_id);
      energy = chan.energy() * Athena::Units::MeV;    // fixed in MeV by DSP
      time = chan.time() * Athena::Units::picosecond; // fixed in ps by DSP
      gain = chan.gain();
      noise = noiseH->getNoise(offline_id, gain);
      significance = energy / noise;
      bad_quality = (energy > 0.1 * Athena::Units::MeV)
        && (chan.quality() > m_quality_threshold);
    }
    catch (const LArOnlID_Exception &) {
      continue; // skip this channel
    }
    catch (const std::out_of_range &err) {
      ATH_MSG_WARNING("FEB hash out of range. Detector undefined"
                      << err.what());
      continue; // skip this channel
    }
    catch (const LArID_Exception &) {
      ATH_MSG_WARNING("channel offline id undefined ... skipping");
      continue; // skip this channel
    }

    // Fill per-detector histograms ---
    if (m_monitor_detectors && monitoring) {
      bool noisy_pos{significance > m_pos_noise_thresholds[det]};
      bool noisy_neg{-significance > m_neg_noise_thresholds[det]};
      per_detector_total_energy[det] += energy;
      det_n_noisy_channels[det] += noisy_pos;
      det_n_noisy_channels_Neg[det] += noisy_neg;
      det_n_badQ_channels[det] += bad_quality;
      dqm_superslot = feedthrough_number * ::numberOfSlotsPerFeedthrough(det)
                      + slot_number;
      dqm_channel = channel;
      dqmf_occ = noisy_event && (energy > m_occupancy_thresholds[det]);
      dqmf_sig = noisy_event && (energy > m_signal_thresholds[det])
                 && is_atlas_ready && !larNoisyROAlgInTimeW_flag;
      dqm_energy = energy;
      dqm_gain = gain;
      dqm_posn = 100 * (noisy_event && !larNoisyROAlgInTimeW_flag
                        && noisy_pos && is_atlas_ready);
      dqm_negn = 100 * (noisy_event && !larNoisyROAlgInTimeW_flag
                        && noisy_neg && is_atlas_ready);
      dqm_qual = 100 * (bad_quality && is_atlas_ready
                        && !larNoisyROAlgInTimeW_flag);
      fill(monitoring, dqm_superslot, dqm_channel,
           dqmf_occ, dqmf_sig, dqm_energy, dqm_gain,
           dqm_posn, dqm_negn, dqm_qual);

      if (m_monitor_time && significance > m_time_threshold) {
        // The time resolution is \sigma_t = \frac{a}{E/\sigma_{E}} \oplus b
        // where a and b depend on the channel position (partition + layer);
        // in practice a = 30ns and b = 1ns are assumed.
        if (significance != 0.) {
          double weight = 1. + 900. / std::pow(significance, 2);
          // <t> = ( \sum w*t ) / ( \sum w )
          // -> num. & den. are accumulated separately using a complex variable
          wsum_t datapoint{weight * (double(time) + 1i)};
          event_mean_time += datapoint;
          mean_detector_times[det] += datapoint;
          mean_feb_times[feb_hash] += datapoint;
        }
      }
    }
  }

  Monitored::Scalar<int> dqm_lb{"lb", lumi_block};
  Monitored::Scalar<int> dqm_bcid{"bc", bcid};
  Monitored::Scalar<int> dqm_qnChan{"nQ4k", 0};
  Monitored::Scalar<float> dqm_percent_noisy{"%noisy", 0};
  Monitored::Scalar<float> dqm_percent_neg_noisy{"%noisy_neg", 0};
  Monitored::Scalar<bool> dqmf_burst{"burst", false};
  Monitored::Scalar<bool> dqmf_burst_timevetoed{"burst_quietW", false};
  Monitored::Scalar<float> dqm_total_energy{"detE", 0};
  Monitored::Scalar<bool> dqmf_qburst{"qburst", false};
  Monitored::Scalar<bool> dqmf_noNoisyRO{"quiet", !larNoisyROAlg_flag};
  Monitored::Scalar<bool> dqmf_noNoisyRO_W{"quietW", !larNoisyROAlg_W_flag};
  Monitored::Scalar<bool> dqmf_noNoisyRO_ITW{
      "quietITW", !larNoisyROAlgInTimeW_flag};
  for (int8_t det : ::partitions()) {
    float scaling = 100.f / m_det_to_nchannels[det];
    float percent_noisy = scaling * det_n_noisy_channels[det];
    float percent_bad_quality = scaling * det_n_badQ_channels[det];
    dqmf_burst = percent_noisy > m_noise_burst_percent_thresholds[det];
    dqmf_burst_timevetoed = dqmf_burst && !larNoisyROAlgInTimeW_flag;
    dqm_percent_noisy = percent_noisy;
    dqm_percent_neg_noisy = scaling * det_n_noisy_channels_Neg[det];
    dqm_qnChan = det_n_badQ_channels[det];
    dqmf_qburst = percent_bad_quality > m_noise_burst_percent_thresholds[det];
    dqm_total_energy = per_detector_total_energy[det];

    if (m_monitor_burst) {
      fill(m_tools[m_monitoring_tool_index[det]], dqm_lb, dqm_bcid,
           dqm_percent_noisy, dqm_percent_neg_noisy,
           dqmf_noNoisyRO, dqmf_noNoisyRO_W, dqmf_noNoisyRO_ITW,
           dqmf_burst, dqmf_burst_timevetoed, dqmf_qburst,
           dqm_qnChan);
    }
    if (m_monitor_signal) {
      fill(m_tools[m_monitoring_tool_index[det]], dqm_lb, dqm_bcid,
           dqmf_noNoisyRO_W, dqm_total_energy);
    }
  }

  Monitored::Scalar<float> dqm_time{"T", 0};
  if (m_monitor_burst && m_monitor_time) {
    // sum w*t and sum w are accumulated respectively in Re(z) and Im(z)
    double t{event_mean_time.real() / event_mean_time.imag()};
    for (uint32_t h = 0; h < mean_feb_times.size(); ++h) {
      wsum_t w{mean_feb_times[h]};
      if (w.imag() == 0.) continue;
      int8_t det = m_feb_hash_to_detector.at(h);
      double t_feb = w.real() / w.imag();
      dqm_time = (t_feb - t) / Athena::Units::nanosecond;
      fill(m_tools[m_monitoring_tool_index[det]], dqm_time);
    }
  }

  return StatusCode::SUCCESS;
}
