/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JepCmxMonitorAlgorithm.h"

JepCmxMonitorAlgorithm::JepCmxMonitorAlgorithm(const std::string &name, ISvcLocator *pSvcLocator)
    : AthMonitorAlgorithm(name, pSvcLocator)
{
}

StatusCode JepCmxMonitorAlgorithm::initialize()
{

  ATH_MSG_DEBUG("JepCmxMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name " << m_packageName);

  // container names
  ATH_MSG_DEBUG("m_CMXJetTobLocation " << m_CMXJetTobLocation);
  ATH_MSG_DEBUG("m_CMXJetHitsLocation " << m_CMXJetHitsLocation);
  ATH_MSG_DEBUG("m_CMXEtSumsLocation " << m_CMXEtSumsLocation);
  ATH_MSG_DEBUG("m_CMXRoILocation " << m_CMXJetHitsLocation);

  // steering parameters
  ATH_MSG_DEBUG("m_crates" << m_crates);
  ATH_MSG_DEBUG("m_modules" << m_modules);
  ATH_MSG_DEBUG("m_tobsPerJem" << m_tobsPerJem);
  ATH_MSG_DEBUG("m_maxTobsPerCmx" << m_maxTobsPerCmx);

  // we initialise all the containers that we need
  ATH_CHECK(m_CMXJetTobLocation.initialize());
  ATH_CHECK(m_CMXJetHitsLocation.initialize());
  ATH_CHECK(m_CMXEtSumsLocation.initialize());
  ATH_CHECK(m_CMXRoILocation.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode JepCmxMonitorAlgorithm::fillHistograms(const EventContext &ctx) const
{

  ATH_MSG_DEBUG("JepCmxMonitorAlgorithm::fillHistograms");

  // total error summary
  auto cmxErrorSummary = Monitored::Scalar<int>("cmxErrorSummary", 0);

  // triggered slice numbers
  int j_num_slice = -1;
  int e_num_slice = -1;

  // Error vector for global overview
  std::vector<int> overview(2);

  using LVL1::DataError;

  // =========================================================================
  // ================= Container: CMX Jet TOBs ===============================
  // =========================================================================

  // retrieve CMX Jet TOBs from Storegate
  SG::ReadHandle<xAOD::CMXJetTobContainer> CMXJetTob(m_CMXJetTobLocation, ctx);
  if (!CMXJetTob.isValid())
  {
    ATH_MSG_ERROR("No CMX JetTob found in TES at " << m_CMXJetTobLocation);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("--------------  CMX Jet TOBs ---------------");

  auto jetTobEnergyLg = Monitored::Scalar<int>("jetTobEnergyLg", 0.);
  auto jetTobEnergySm = Monitored::Scalar<int>("jetTobEnergySm", 0.);
  auto jetTobHitmap_x = Monitored::Scalar<int>("jetTobHitmap_x", 0.);
  auto jetTobHitmap_y = Monitored::Scalar<int>("jetTobHitmap_y", 0.);

  auto tobsPerJem = Monitored::Scalar<int>("tobsPerJem", 0.);
  auto tobsPerCmx = Monitored::Scalar<int>("tobsPerCmx", 0.);

  auto threshParityTobJem = Monitored::Scalar<int>("threshParityTobJem", 0.);
  auto threshParityTobCrate = Monitored::Scalar<int>("threshParityTobCrate", 0.);

  std::vector<int> tobCount(m_crates * m_modules);
  std::vector<int> cmxCount(m_crates);

  // Step over all cells
  xAOD::CMXJetTobContainer::const_iterator it_CMXJetTob = (*CMXJetTob).begin();
  xAOD::CMXJetTobContainer::const_iterator it_CMXJetTob_end = (*CMXJetTob).end();
  for (; it_CMXJetTob != it_CMXJetTob_end; ++it_CMXJetTob)
  {
    const int crate = (*it_CMXJetTob)->crate();
    const int jem = (*it_CMXJetTob)->jem();
    const int frame = (*it_CMXJetTob)->frame();
    const int location = (*it_CMXJetTob)->location();
    const int energyLg = (*it_CMXJetTob)->energyLarge();
    const int energySm = (*it_CMXJetTob)->energySmall();
    const int error = (*it_CMXJetTob)->error();
    const int x = crate * m_modules + jem;
    const int y = frame * 4 + location;

    if (energyLg) {
      jetTobEnergyLg = energyLg;
      fill(m_packageName, jetTobEnergyLg);
      jetTobHitmap_x = x;
      jetTobHitmap_y = y;
      fill(m_packageName, jetTobHitmap_x, jetTobHitmap_y);
    }
    if (energySm) {
      jetTobEnergySm = energySm;
      fill(m_packageName, jetTobEnergySm);
    }
    if (error) {
      const LVL1::DataError err(error);
      int parity = err.get(LVL1::DataError::Parity);
      if (parity) {
        threshParityTobJem = jem;
        threshParityTobCrate = crate;
        fill(m_packageName, threshParityTobJem, threshParityTobCrate);
        cmxErrorSummary = JetParity;
        fill(m_packageName, cmxErrorSummary);
        overview[crate] |= (1 << JetParity);
      }
    }
    if (energyLg || energySm || error) {
      ++tobCount[x];
      ++cmxCount[crate];
    }
  }

  // step over crates
  for (int crate = 0; crate < m_crates; ++crate) {
    for (int jem = 0; jem < m_modules; ++jem) {
      int val = tobCount[crate * m_modules + jem];
      if (val) {
        if (val > m_tobsPerJem)
          val = m_tobsPerJem + 1;
        tobsPerJem = val;
        fill(m_packageName, tobsPerJem);
      }
    }
    int val = cmxCount[crate];
    if (val) {
      if (val >= m_maxTobsPerCmx)
       val = m_maxTobsPerCmx - 1;
      tobsPerCmx = val;
      fill(m_packageName, tobsPerCmx);
    }
  }

  // =========================================================================
  // ================= Container: CMX Jet Hits ===============================
  // =========================================================================

  // retrieve CMX Jet Hits from Storegate
  SG::ReadHandle<xAOD::CMXJetHitsContainer> CMXJetHits(m_CMXJetHitsLocation, ctx);
  if (!CMXJetHits.isValid()) {
    ATH_MSG_ERROR("No CMX JetHits found in TES at " << m_CMXJetHitsLocation);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("--------------  CMX Jet Hits ---------------");

  auto threshTotalMainVals = Monitored::Scalar<int>("threshTotalMainVals", 0.);
  auto threshTotalMainHits = Monitored::Scalar<int>("threshTotalMainHits", 0.);

  auto threshRoiOverflow = Monitored::Scalar<int>("threshRoiOverflow", 0.);
  auto topoDataChecksum = Monitored::Scalar<int>("topoDataChecksum", 0.);
  auto topoTobsPerJem = Monitored::Scalar<int>("topoTobsPerJem", 0.);
  auto topoTobsPerCmx = Monitored::Scalar<int>("topoTobsPerCmx", 0.);

  auto topoJemOccMap_mod = Monitored::Scalar<int>("topoJemOccMap_mod", 0.);
  auto topoJemOccMap_cra = Monitored::Scalar<int>("topoJemOccMap_cra", 0.);
  auto topoJemOccMap_hit = Monitored::Scalar<int>("topoJemOccMap_hit", 0.);

  auto topoJemOccCount_mod = Monitored::Scalar<int>("topoJemOccCount_mod", 0.);
  auto topoJemOccCount_cra = Monitored::Scalar<int>("topoJemOccCount_cra", 0.);
  auto topoJemOccCount_hit = Monitored::Scalar<int>("topoJemOccCount_hit", 0.);

  auto cmxThreshStatusErr_bit = Monitored::Scalar<int>("cmxThreshStatusErr_bit", 0.);
  auto cmxThreshStatusErr_crate = Monitored::Scalar<int>("cmxThreshStatusErr_crate", 0.);

  auto cmxThreshParityErr = Monitored::Scalar<int>("cmxThreshParityErr", 0.);

  cmxCount.assign(m_crates, 0);

  // Step over all cells
  xAOD::CMXJetHitsContainer::const_iterator it_CMXJetHits = (*CMXJetHits).begin();
  xAOD::CMXJetHitsContainer::const_iterator it_CMXJetHitsEnd = (*CMXJetHits).end();
  for (; it_CMXJetHits != it_CMXJetHitsEnd; ++it_CMXJetHits) {
    const int crate = (*it_CMXJetHits)->crate();
    const int source = (*it_CMXJetHits)->sourceComponent();
    const unsigned int hits0 = (*it_CMXJetHits)->hits0();
    const unsigned int hits1 = (*it_CMXJetHits)->hits1();
    const DataError err0((*it_CMXJetHits)->error0());
    const DataError err1((*it_CMXJetHits)->error1());

    // -----------------------------------------------------------------------
    // --------- Histos with distribution of CMX Hit Multiplicities ----------
    // -----------------------------------------------------------------------

    if (source == xAOD::CMXJetHits::TOTAL_MAIN) {
      fillThresholds(threshTotalMainVals, threshTotalMainHits, hits0, 5, 3);
      fillThresholds(threshTotalMainVals, threshTotalMainHits, hits1, 5, 3, 5);
    }
    // RoI overflow
    if (crate == m_crates - 1 && err0.get(DataError::Overflow)) {
      const int x = (source == xAOD::CMXJetHits::REMOTE_MAIN)
                        ? 0
                    : (source == xAOD::CMXJetHits::LOCAL_MAIN)
                        ? 1
                    : (source == xAOD::CMXJetHits::TOTAL_MAIN) ? 2
                                                               : 3;
      if (x < 3) {
        threshRoiOverflow = x;
        fill(m_packageName, threshRoiOverflow);
      }
    }

    // -----------------------------------------------------------------------
    // ------------------- Histos with Topo information ----------------------
    // -----------------------------------------------------------------------

    else if (source == xAOD::CMXJetHits::TOPO_CHECKSUM) {
      if (hits0) {
        topoDataChecksum = crate;
        fill(m_packageName, topoDataChecksum);
      }
    }
    else if (source == xAOD::CMXJetHits::TOPO_OCCUPANCY_MAP) {
      if (hits0)
      {
        fillThresholdsVsY(topoJemOccMap_mod, topoJemOccMap_cra, topoJemOccMap_hit,
                          hits0, crate, m_modules, 1);
      }
    }
    else if (source == xAOD::CMXJetHits::TOPO_OCCUPANCY_COUNTS) {
      if (hits0)
      {
        fillThresholdsVsY(topoJemOccCount_mod, topoJemOccCount_cra, topoJemOccCount_hit,
                          hits0, crate, m_modules / 2, 3);
        for (int mod = 0; mod < m_modules / 2; ++mod) {
          const int val = (hits0 >> (mod * 3)) & 0x7;
          if (val) {
            topoTobsPerJem = val;
            fill(m_packageName, topoTobsPerJem);
            cmxCount[crate] += val;
          }
        }
      }
      if (hits1) {
        fillThresholdsVsY(topoJemOccCount_mod, topoJemOccCount_cra, topoJemOccCount_hit,
                          hits1, crate, m_modules / 2, 3, m_modules / 2);
        for (int mod = 0; mod < m_modules / 2; ++mod) {
          const int val = (hits1 >> (mod * 3)) & 0x7;
          if (val) {
            topoTobsPerJem = val;
            fill(m_packageName, topoTobsPerJem);
            cmxCount[crate] += val;
          }
        }
      }
    }

    // -----------------------------------------------------------------------
    // ----------------- Histos with SubStatus Word errors -------------------
    // -----------------------------------------------------------------------

    if (j_num_slice < 0) j_num_slice = (*it_CMXJetHits)->peak();

    // Error summary plots
    // substatus word
    const int status = (err0.error() >> LVL1::DataError::GLinkParity) & 0xff;
    if (status) {
      for (int bit = 0; bit < 8; ++bit) {
        if ((status >> bit) & 0x1) {
          cmxThreshStatusErr_bit = bit;
          cmxThreshStatusErr_crate = crate;
          fill(m_packageName, cmxThreshStatusErr_bit, cmxThreshStatusErr_crate);
        }
      }
      cmxErrorSummary = JetStatus;
      fill(m_packageName, cmxErrorSummary);
      overview[crate] |= (1 << JetStatus);
    }

    if (source == xAOD::CMXJetHits::REMOTE_MAIN ||
        source == xAOD::CMXJetHits::REMOTE_FORWARD)
    {
      // Parity
      const int p0 = err0.get(DataError::Parity);
      const int p1 = err1.get(DataError::Parity);
      if (p0 || p1) {
        if (p0) { // cable 0/phase 0 or cable 1/phase 0
          const int xpos = (source == xAOD::CMXJetHits::REMOTE_MAIN) ? 0 : 2;
          cmxThreshParityErr = xpos;
          fill(m_packageName, cmxThreshParityErr);
        }
        if (p1) { // cable 0/phase 1 or cable 1/phase 1
          const int xpos = (source == xAOD::CMXJetHits::REMOTE_MAIN) ? 1 : 3;
          cmxThreshParityErr = xpos;
          fill(m_packageName, cmxThreshParityErr);
        }
        cmxErrorSummary = JetParity;
        fill(m_packageName, cmxErrorSummary);
        overview[crate] |= (1 << JetParity);
      }
    }
  }

  for (int crate = 0; crate < m_crates; ++crate) {
    int val = cmxCount[crate];
    if (val) {
      if (val >= m_maxTobsPerCmx)
        val = m_maxTobsPerCmx - 1;
      topoTobsPerCmx = val;
      fill(m_packageName, topoTobsPerCmx);
    }
  }

  // =========================================================================
  // ================= Container: CMX Et Sums ================================
  // =========================================================================

  // retrieve CMX Jet TOBs from Storegate
  SG::ReadHandle<xAOD::CMXEtSumsContainer> CMXEtSums(m_CMXEtSumsLocation, ctx);
  if (!CMXEtSums.isValid()) {
    ATH_MSG_ERROR("No CMXEtSums found in TES at " << m_CMXEtSumsLocation);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("-------------- CMX Et Sums ---------------");

  auto eSubSumsEx = Monitored::Scalar<int>("eSubSumsEx", 0.);
  auto eSubSumsEy = Monitored::Scalar<int>("eSubSumsEy", 0.);
  auto eSubSumsEt = Monitored::Scalar<int>("eSubSumsEt", 0.);

  auto cmxTotalEx = Monitored::Scalar<int>("cmxTotalEx", 0.);
  auto cmxTotalEy = Monitored::Scalar<int>("cmxTotalEy", 0.);
  auto cmxTotalEt = Monitored::Scalar<int>("cmxTotalEt", 0.);
  auto cmxTotalExRes = Monitored::Scalar<int>("cmxTotalExRes", 0.);
  auto cmxTotalEyRes = Monitored::Scalar<int>("cmxTotalEyRes", 0.);
  auto cmxTotalEtRes = Monitored::Scalar<int>("cmxTotalEtRes", 0.);

  auto cmxMissingEt_vals = Monitored::Scalar<int>("cmxMissingEt_vals", 0.);
  auto cmxMissingEt_hits = Monitored::Scalar<int>("cmxMissingEt_hits", 0.);
  auto cmxMissingEtSig_vals = Monitored::Scalar<int>("cmxMissingEtSig_vals", 0.);
  auto cmxMissingEtSig_hits = Monitored::Scalar<int>("cmxMissingEtSig_hits", 0.);
  auto cmxMissingEtHitsRes_vals = Monitored::Scalar<int>("cmxMissingEtHitsRes_vals", 0.);
  auto cmxMissingEtHitsRes_hits = Monitored::Scalar<int>("cmxMissingEtHitsRes_hits", 0.);

  auto cmxSumEt_vals = Monitored::Scalar<int>("cmxSumEt_vals", 0.);
  auto cmxSumEt_hits = Monitored::Scalar<int>("cmxSumEt_hits", 0.);
  auto cmxSumEtHitsWt_vals = Monitored::Scalar<int>("cmxSumEtHitsWt_vals", 0.);
  auto cmxSumEtHitsWt_hits = Monitored::Scalar<int>("cmxSumEtHitsWt_hits", 0.);

  auto cmxTriggeredSlice = Monitored::Scalar<int>("cmxTriggeredSlice", 0.);

  auto cmxEnergyStatus_bit = Monitored::Scalar<int>("cmxEnergyStatus_bit", 0.);
  auto cmxEnergyStatus_cra = Monitored::Scalar<int>("cmxEnergyStatus_cra", 0.);

  auto cmxEnergyParityModx = Monitored::Scalar<int>("cmxEnergyParityModx", 0.);
  auto cmxEnergyParityMody = Monitored::Scalar<int>("cmxEnergyParityMody", 0.);

  auto cmxParityRem = Monitored::Scalar<int>("cmxParityRem", 0.);

  auto cmxEnergyOverflowx = Monitored::Scalar<int>("cmxEnergyOverflowx", 0.);
  auto cmxEnergyOverflowy = Monitored::Scalar<int>("cmxEnergyOverflowy", 0.);
  auto cmxEnergyOverflowz = Monitored::Scalar<int>("cmxEnergyOverflowz", 0.);

  auto cmxEnergyOverflowResx = Monitored::Scalar<int>("cmxEnergyOverflowResx", 0.);
  auto cmxEnergyOverflowResy = Monitored::Scalar<int>("cmxEnergyOverflowResy", 0.);
  auto cmxEnergyOverflowResz = Monitored::Scalar<int>("cmxEnergyOverflowResz", 0.);

  // Step over all cells
  xAOD::CMXEtSumsContainer::const_iterator it_CMXEtSums = CMXEtSums->begin();
  xAOD::CMXEtSumsContainer::const_iterator it_CMXEtSums_end = CMXEtSums->end();
  for (; it_CMXEtSums != it_CMXEtSums_end; ++it_CMXEtSums) {
    const int crate = (*it_CMXEtSums)->crate();
    const int source = (*it_CMXEtSums)->sourceComponent();
    const unsigned int rawEx = (*it_CMXEtSums)->ex();
    const unsigned int rawEy = (*it_CMXEtSums)->ey();
    const unsigned int rawEt = (*it_CMXEtSums)->et();
    const int exError = (*it_CMXEtSums)->exError();
    const int eyError = (*it_CMXEtSums)->eyError();
    const int etError = (*it_CMXEtSums)->etError();

    // -----------------------------------------------------------------------
    // -------------- Histos with distribution of JEM Energies ---------------
    // -----------------------------------------------------------------------
    // JEM energy sums
    if (source < m_modules) {
      const int ex = rawEx;
      const int ey = rawEy;
      const int et = rawEt;

      if (ex) {
        eSubSumsEx = ex;
        fill(m_packageName, eSubSumsEx);
      }
      if (ey) {
        eSubSumsEy = ey;
        fill(m_packageName, eSubSumsEy);
      }
      if (et) {
        eSubSumsEt = et;
        fill(m_packageName, eSubSumsEt);
      }
    }

    // -----------------------------------------------------------------------
    // ---------- Histos with distribution of total Energy per system --------
    // -----------------------------------------------------------------------
    // total energy sums
    else if (source == xAOD::CMXEtSums::TOTAL_STANDARD ||
             source == xAOD::CMXEtSums::TOTAL_RESTRICTED) {
      // Use CrateEnergy object to decode 15-bit twos-complement format
      LVL1::CrateEnergy cen(crate, rawEt, rawEx, rawEy, etError & 0x1, exError & 0x1, eyError & 0x1);
      const int ex = cen.ex();
      const int ey = cen.ey();
      const int et = rawEt;

      if (source == xAOD::CMXEtSums::TOTAL_STANDARD) {
        if (ex && !cen.exOverflow()) {
          cmxTotalEx = ex;
          fill(m_packageName, cmxTotalEx);
        }
        if (ey && !cen.eyOverflow()) {
          cmxTotalEy = ey;
          fill(m_packageName, cmxTotalEy);
        }
        if (et && !cen.etOverflow()) {
          cmxTotalEt = et;
          fill(m_packageName, cmxTotalEt);
        }
      }
      else {
        if (ex && !cen.exOverflow()) {
          cmxTotalExRes = ex;
          fill(m_packageName, cmxTotalExRes);
        }
        if (ey && !cen.eyOverflow()) {
          cmxTotalEyRes = ey;
          fill(m_packageName, cmxTotalEyRes);
        }
        if (et && !cen.etOverflow()) {
          cmxTotalEtRes = et;
          fill(m_packageName, cmxTotalEtRes);
        }
      }
    }
    // MissingEt/SumEt/MissingEtSig Hitmaps
    else if (source == xAOD::CMXEtSums::MISSING_ET_STANDARD ||
             source == xAOD::CMXEtSums::SUM_ET_STANDARD ||
             source == xAOD::CMXEtSums::MISSING_ET_SIG_STANDARD) {
      const int nHits = 8;
      if (source == xAOD::CMXEtSums::MISSING_ET_STANDARD) {
        fillThresholds(cmxMissingEt_vals, cmxMissingEt_hits, rawEt, nHits, 1);
      }
      else if (source == xAOD::CMXEtSums::SUM_ET_STANDARD) {
        fillThresholds(cmxSumEt_vals, cmxSumEt_hits, rawEt, nHits, 1);
      }
      else if (source == xAOD::CMXEtSums::MISSING_ET_SIG_STANDARD) {
        fillThresholds(cmxMissingEtSig_vals, cmxMissingEtSig_hits, rawEt, nHits, 1);
      }
    }
    else if (source == xAOD::CMXEtSums::MISSING_ET_RESTRICTED ||
             source == xAOD::CMXEtSums::SUM_ET_RESTRICTED) {
      const int nHits = 8;
      if (source == xAOD::CMXEtSums::MISSING_ET_RESTRICTED) {
        fillThresholds(cmxMissingEtHitsRes_vals, cmxMissingEtHitsRes_hits, rawEt, nHits, 1);
      }
      else fillThresholds(cmxSumEtHitsWt_vals, cmxSumEtHitsWt_hits, rawEt, nHits, 1);
    }

    if (e_num_slice < 0) {
      e_num_slice = (*it_CMXEtSums)->peak();
      if (j_num_slice >= 0) {
        cmxTriggeredSlice = std::abs(e_num_slice - j_num_slice);
        fill(m_packageName, cmxTriggeredSlice);
      }
    }

    // -----------------------------------------------------------------------
    // --------------- Histos with SubStatus Word errors ---------------------
    // -----------------------------------------------------------------------
    //Error summary plots
    //substatus word
    const DataError exerr(exError);
    const DataError eyerr(eyError);
    const DataError eterr(etError);
    const int status = (eterr.error() >> LVL1::DataError::GLinkParity) & 0xff;
    if (status) {
      for (int bit = 0; bit < 8; ++bit) {
        if ((status >> bit) & 0x1) {
          cmxEnergyStatus_bit = bit;
          cmxEnergyStatus_cra = crate;
          fill(m_packageName, cmxEnergyStatus_bit, cmxEnergyStatus_cra);
        }
      }
      cmxErrorSummary = JetParity;
      fill(m_packageName, cmxErrorSummary);
      overview[crate] |= (1 << EnergyStatus);
    }

    if (source < m_modules || source == xAOD::CMXEtSums::REMOTE_STANDARD ||
        source == xAOD::CMXEtSums::REMOTE_RESTRICTED) {
      // Parity
      const int p0 = exerr.get(DataError::Parity);
      const int p1 = eyerr.get(DataError::Parity);
      const int p2 = eterr.get(DataError::Parity);
      if (p0 || p1 || p2) {
        // backplane parity
        if (source < m_modules) {
          const int xpos = crate * m_modules + source;
          cmxEnergyParityModx = xpos;
          if (p0) cmxEnergyParityMody = 0.;
          if (p1) cmxEnergyParityMody = 1.;
          if (p2) cmxEnergyParityMody = 2.;
          fill(m_packageName, cmxEnergyParityModx, cmxEnergyParityMody);
        }
        // cable transmission parity
        else if (source == xAOD::CMXEtSums::REMOTE_STANDARD) {
          if (p0) cmxParityRem = 0.; // cable 0, phase 0
          if (p1) cmxParityRem = 1.; // cable 0, phase 1
        }
        else {
          if (p0) cmxParityRem = 2.; // cable 1, phase 0
          if (p1) cmxParityRem = 3.; // cable 1, phase 1
        }
        fill(m_packageName, cmxParityRem);
        cmxErrorSummary = EnergyParity;
        fill(m_packageName, cmxErrorSummary);
        overview[crate] |= (1 << EnergyParity);
      }
    }

    // Overflow - not an error, plot rate
    if (crate == 1)  {
      if (source == xAOD::CMXEtSums::REMOTE_STANDARD ||
          source == xAOD::CMXEtSums::LOCAL_STANDARD ||
          source == xAOD::CMXEtSums::TOTAL_STANDARD) {
        const double ypos = (source == xAOD::CMXEtSums::REMOTE_STANDARD)  ? 0.
                            : (source == xAOD::CMXEtSums::LOCAL_STANDARD) ? 1.
                                                                          : 2.;
        cmxEnergyOverflowy = ypos;

        cmxEnergyOverflowx = 0.;
        cmxEnergyOverflowz = exerr.get(DataError::Overflow);
        fill(m_packageName, cmxEnergyOverflowx, cmxEnergyOverflowy, cmxEnergyOverflowz);

        cmxEnergyOverflowx = 1.;
        cmxEnergyOverflowz = eyerr.get(DataError::Overflow);
        fill(m_packageName, cmxEnergyOverflowx, cmxEnergyOverflowy, cmxEnergyOverflowz);

        cmxEnergyOverflowx = 2.;
        cmxEnergyOverflowz = eterr.get(DataError::Overflow);
        fill(m_packageName, cmxEnergyOverflowx, cmxEnergyOverflowy, cmxEnergyOverflowz);
      }
      else if (source == xAOD::CMXEtSums::REMOTE_RESTRICTED ||
               source == xAOD::CMXEtSums::LOCAL_RESTRICTED ||
               source == xAOD::CMXEtSums::TOTAL_RESTRICTED) {
        const double ypos = (source == xAOD::CMXEtSums::REMOTE_RESTRICTED)  ? 0.
                            : (source == xAOD::CMXEtSums::LOCAL_RESTRICTED) ? 1.
                                                                            : 2.;
        cmxEnergyOverflowResy = ypos;

        cmxEnergyOverflowResx = 0.;
        cmxEnergyOverflowResz = exerr.get(DataError::Overflow);
        fill(m_packageName, cmxEnergyOverflowResx, cmxEnergyOverflowResy, cmxEnergyOverflowResz);

        cmxEnergyOverflowResx = 1.;
        cmxEnergyOverflowResz = eyerr.get(DataError::Overflow);
        fill(m_packageName, cmxEnergyOverflowResx, cmxEnergyOverflowResy, cmxEnergyOverflowResz);

        cmxEnergyOverflowResx = 2.;
        cmxEnergyOverflowResz = eterr.get(DataError::Overflow);
        fill(m_packageName, cmxEnergyOverflowResx, cmxEnergyOverflowResy, cmxEnergyOverflowResz);
      }
    }
  }

  // =========================================================================
  // ================= Container: CMX RoI ====================================
  // =========================================================================

  // retrieve RoI information from Storegate
  SG::ReadHandle<xAOD::CMXRoIContainer> CRCon(m_CMXRoILocation, ctx);
  if (!CRCon.isValid()) {
    ATH_MSG_ERROR("No CMX RoI found in TES at " << m_CMXRoILocation);
    return StatusCode::FAILURE;
  }

  const xAOD::CMXRoI *CR = *CRCon->begin();

  ATH_MSG_DEBUG("-------------- CMX RoI ---------------");

  auto roiSumEtHits_var = Monitored::Scalar<int>("roiSumEtHits_var", 0.);
  auto roiSumEtHits_hit = Monitored::Scalar<int>("roiSumEtHits_hit", 0.);

  auto roiMissingEtHits_var = Monitored::Scalar<int>("roiMissingEtHits_var", 0.);
  auto roiMissingEtHits_hit = Monitored::Scalar<int>("roiMissingEtHits_hit", 0.);

  auto roiMissingEtSigHits_var = Monitored::Scalar<int>("roiMissingEtSigHits_var");
  auto roiMissingEtSigHits_hit = Monitored::Scalar<int>("roiMissingEtSigHits_hit");

  auto roiSumEtHitsMask_var = Monitored::Scalar<int>("roiSumEtHitsMask_var");
  auto roiSumEtHitsMask_hit = Monitored::Scalar<int>("roiSumEtHitsMask_hit");

  auto roiMissingEtHitsMask_var = Monitored::Scalar<int>("roiMissingEtHitsMask_var");
  auto roiMissingEtHitsMask_hit = Monitored::Scalar<int>("roiMissingEtHitsMask_hit");

  auto cmxRoiEx = Monitored::Scalar<int>("cmxRoiEx", 0.);
  auto cmxRoiEy = Monitored::Scalar<int>("cmxRoiEy", 0.);
  auto cmxRoiEt = Monitored::Scalar<int>("cmxRoiEt", 0.);

  auto cmxRoiExMask = Monitored::Scalar<int>("cmxRoiExMask", 0.);
  auto cmxRoiEyMask = Monitored::Scalar<int>("cmxRoiEyMask", 0.);
  auto cmxRoiEtMask = Monitored::Scalar<int>("cmxRoiEtMask", 0.);

  // -------------------------------------------------------------------------
  // -------------- Histos filled with CMX RoI information -------------------
  // -------------------------------------------------------------------------

  const int rawEx = CR->ex();
  const int rawEy = CR->ey();
  const int et = CR->et();
  const int exError = CR->exError();
  const int eyError = CR->eyError();
  const int etError = CR->etError();
  const int sumEtHits = CR->sumEtHits();
  const int missingEtHits = CR->missingEtHits();
  const int missingEtSigHits = CR->missingEtSigHits();
  const int rawExM = CR->ex(xAOD::CMXRoI::MASKED);
  const int rawEyM = CR->ey(xAOD::CMXRoI::MASKED);
  const int etM = CR->et(xAOD::CMXRoI::MASKED);
  const int exErrorM = CR->exError(xAOD::CMXRoI::MASKED);
  const int eyErrorM = CR->eyError(xAOD::CMXRoI::MASKED);
  const int etErrorM = CR->etError(xAOD::CMXRoI::MASKED);
  const int sumEtHitsM = CR->sumEtHits(xAOD::CMXRoI::MASKED);
  const int missingEtHitsM = CR->missingEtHits(xAOD::CMXRoI::MASKED);

  fillThresholds(roiSumEtHits_var, roiSumEtHits_hit, sumEtHits, 8, 1);
  fillThresholds(roiMissingEtHits_var, roiMissingEtHits_hit, missingEtHits, 8, 1);
  fillThresholds(roiMissingEtSigHits_var, roiMissingEtSigHits_hit, missingEtSigHits, 8, 1);
  fillThresholds(roiSumEtHitsMask_var, roiSumEtHitsMask_hit, sumEtHitsM, 8, 1);
  fillThresholds(roiMissingEtHitsMask_var, roiMissingEtHitsMask_hit, missingEtHitsM, 8, 1);

  // Use CrateEnergy object to decode 15-bit twos-complement format
  LVL1::CrateEnergy cen(1, et, rawEx, rawEy, etError & 0x1, exError & 0x1, eyError & 0x1);
  const int ex = cen.ex();
  const int ey = cen.ey();

  if (ex && !cen.exOverflow()) {
    cmxRoiEx = ex;
    fill(m_packageName, cmxRoiEx);
  }
  if (ey && !cen.eyOverflow()) {
    cmxRoiEy = ey;
    fill(m_packageName, cmxRoiEy);
  }
  if (et && !cen.etOverflow()) {
    cmxRoiEt = et;
    fill(m_packageName, cmxRoiEt);
  }

  LVL1::CrateEnergy cen2(1, etM, rawExM, rawEyM, etErrorM & 0x1, exErrorM & 0x1, eyErrorM & 0x1);
  const int exM = cen2.ex();
  const int eyM = cen2.ey();

  if (exM && !cen2.exOverflow()) {
    cmxRoiExMask = exM;
    fill(m_packageName, cmxRoiExMask);
  }
  if (eyM && !cen2.eyOverflow()) {
    cmxRoiEyMask = eyM;
    fill(m_packageName, cmxRoiEyMask);
  }
  if (etM && !cen2.etOverflow()) {
    cmxRoiEtMask = etM;
    fill(m_packageName, cmxRoiEtMask);
  }

  return StatusCode::SUCCESS;
}

void JepCmxMonitorAlgorithm::fillThresholds(Monitored::Scalar<int> &monVar,
                                            Monitored::Scalar<int> &hitVar,
                                            int val, int nThresh,
                                            int nBits, int offset) const {
  const int mask = (1 << nBits) - 1;
  for (int thr = 0; thr < nThresh; ++thr) {
    const int hit = (val >> (nBits * thr)) & mask;
    if (hit) {
      monVar = thr + offset;
      hitVar = hit;
      fill(m_packageName, monVar, hitVar);
    }
  }
}

void JepCmxMonitorAlgorithm::fillThresholdsVsY(Monitored::Scalar<int> &monVarX,
                                               Monitored::Scalar<int> &monVarY,
                                               Monitored::Scalar<int> &hitVar,
                                               int val, int y, int nThresh,
                                               int nBits, int offset) const {
  const int mask = (1 << nBits) - 1;
  for (int thr = 0; thr < nThresh; ++thr) {
    const int hit = (val >> (nBits * thr)) & mask;
    if (hit) {
      monVarX = thr + offset;
      monVarY = y;
      hitVar = hit;
      fill(m_packageName, monVarX, monVarY, hitVar);
    }
  }
}
