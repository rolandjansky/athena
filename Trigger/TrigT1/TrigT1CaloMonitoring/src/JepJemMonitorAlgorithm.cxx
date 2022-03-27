/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JepJemMonitorAlgorithm.h"

JepJemMonitorAlgorithm::JepJemMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator),
    m_phiScaleJE(16./M_PI)
{
}

StatusCode JepJemMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("JepJemMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);

  // container names
  ATH_MSG_DEBUG("m_JetElementLocation " << m_JetElementLocation);
  ATH_MSG_DEBUG("m_JEMEtSumsLocation " << m_JEMEtSumsLocation);
  ATH_MSG_DEBUG("m_JEMRoILocation " << m_JEMRoILocation);

  // steering parameters
  ATH_MSG_DEBUG("m_maxSlices" << m_maxSlices);
  ATH_MSG_DEBUG("m_crates" << m_crates);
  ATH_MSG_DEBUG("m_modules" << m_modules);
  ATH_MSG_DEBUG("m_tobsPerJEM" << m_tobsPerJEM);

  // we initialise all the containers that we need
  ATH_CHECK(m_JetElementLocation.initialize());
  ATH_CHECK(m_JEMEtSumsLocation.initialize());
  ATH_CHECK(m_JEMRoILocation.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode JepJemMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG("JepJemMonitorAlgorithm::fillHistograms");

  // ===============================
  // RETRIEVE AND SETUP JET ELEMENTS
  // ===============================

  // Retrieve jet elements from SG
  SG::ReadHandle<xAOD::JetElementContainer> jetElements(m_JetElementLocation, ctx);
  if (!jetElements.isValid()) {
    ATH_MSG_ERROR("No JetElements found in TES at " << m_JetElementLocation);
    return StatusCode::FAILURE;
  }

  // 1D EM
  auto etJepJem_em  = Monitored::Scalar<int>("etJepJemJE_em", 0.);
  auto em_weight    = Monitored::Scalar<int>("em_weight", 0.);
  auto etaJepJem_em = Monitored::Scalar<double>("etaJepJemJE_em", 0.);
  auto phiJepJem_em = Monitored::Scalar<double>("phiJepJemJE_em", 0.);
  // 1D HAD
  auto etJepJem_had  = Monitored::Scalar<int>("etJepJemJE_had", 0.);
  auto had_weight    = Monitored::Scalar<int>("had_weight", 0.);
  auto etaJepJem_had = Monitored::Scalar<double> ("etaJepJemJE_had", 0.);
  auto phiJepJem_had = Monitored::Scalar<double> ("phiJepJemJE_had", 0.);
  // 2D EM
  auto etaScaledJepJem_em = Monitored::Scalar<double>("etaScaledJepJemJE_em", 0.);
  auto phiScaledJepJem_em = Monitored::Scalar<double>("phiScaledJepJemJE_em", 0.);
  // 2D HAD
  auto etaScaledJepJem_had = Monitored::Scalar<double>("etaScaledJepJemJE_had", 0.);
  auto phiScaledJepJem_had = Monitored::Scalar<double>("phiScaledJepJemJE_had", 0.);

  // triggered slice number
  auto jem_1d_triggeredSlice_num = Monitored::Scalar<int>("jem_1d_TriggeredSlice_num", 0.);

  // hitmaps per time slice in eta and phi
  std::vector<Monitored::Scalar<double>> jem_em_2d_eta_jetEl_HitMapSlice;
  std::vector<Monitored::Scalar<double>> jem_em_2d_phi_jetEl_HitMapSlice;
  std::vector<Monitored::Scalar<double>> jem_had_2d_eta_jetEl_HitMapSlice;
  std::vector<Monitored::Scalar<double>> jem_had_2d_phi_jetEl_HitMapSlice;
  std::stringstream buffer;
  for (int i=0; i < m_maxSlices; i++) {
    buffer.str("");
    buffer << i;
    jem_em_2d_eta_jetEl_HitMapSlice.push_back(Monitored::Scalar<double>("etaSliceJepJemJE_em_num" + buffer.str(), 0.));
    jem_em_2d_phi_jetEl_HitMapSlice.push_back(Monitored::Scalar<double>("phiSliceJepJemJE_em_num" + buffer.str(), 0.));
    jem_had_2d_eta_jetEl_HitMapSlice.push_back(Monitored::Scalar<double>("etaSliceJepJemJE_had_num" + buffer.str(), 0.));
    jem_had_2d_phi_jetEl_HitMapSlice.push_back(Monitored::Scalar<double>("phiSliceJepJemJE_had_num" + buffer.str(), 0.));
  }

  // error variables
  auto parityErrorJepJE_em    = Monitored::Scalar<bool>("parityErrorJepJE_em", 0.);
  auto parityErrorJepJE_had   = Monitored::Scalar<bool>("parityErrorJepJE_had", 0.);
  auto linkDownErrorJepJE_em  = Monitored::Scalar<bool>("linkDownErrorJepJE_em", 0.);
  auto linkDownErrorJepJE_had = Monitored::Scalar<bool>("linkDownErrorJepJE_had", 0.);

  // error summaries
  auto jemErrorSummary = Monitored::Scalar<int>("jemErrorSummary", 0);
  auto GLinkParityError = Monitored::Scalar<int>("GLinkParityError", 0);
  auto jemLoc = Monitored::Scalar<int>("jemLoc", 0);

  // Error vector for global overview
  std::vector<int> overview(2);

  using LVL1::DataError;

  // =====================
  // Stepping over JE cells
  // =====================
  LVL1::CoordToHardware ToHW;
  xAOD::JetElementContainer::const_iterator jeIterator = (*jetElements).begin();
  xAOD::JetElementContainer::const_iterator jeIteratorEnd = (*jetElements).end();
  for (; jeIterator != jeIteratorEnd; ++jeIterator)
  {
    const xAOD::JetElement *je = *jeIterator;
    const int emEnergy = je->emJetElementET();
    const int hadEnergy = je->hadJetElementET();
    const double eta = je->eta();
    const double phi = je->phi();
    const int slice_n = je->peak();
    const LVL1::Coordinate coord(phi, eta);
    const int crate = ToHW.jepCrate(coord);
    const int module = ToHW.jepModule(coord);
    const int cord = ToHW.jepCoordinateWord(coord);

    ATH_MSG_DEBUG("JE has coords (eta,phi): " << eta << ", " << phi
                  << " and energies (Em,Had): " << emEnergy << ", "
                  << hadEnergy << " HW Crate:" << crate
                  << " Module: " << module << " " << cord);

    // fill kinematic variables
    if (emEnergy > 0) {
      etJepJem_em = emEnergy;
      fill(m_packageName, etJepJem_em);
      em_weight = emEnergy;
      fill(m_packageName, em_weight);
      etaJepJem_em = eta;
      fill(m_packageName, etaJepJem_em);
      fillJEMPhiMon(eta, phi, phiJepJem_em);
      fillJEMEtaVsPhiMon(eta, phi, etaScaledJepJem_em, phiScaledJepJem_em, em_weight);
    }
    if (hadEnergy > 0) {
      etJepJem_had = hadEnergy;
      fill(m_packageName, etJepJem_had);
      had_weight = hadEnergy;
      fill(m_packageName, had_weight);
      etaJepJem_had = eta;
      fill(m_packageName, etaJepJem_had);
      fillJEMPhiMon(eta, phi, phiJepJem_had);
      fillJEMEtaVsPhiMon(eta, phi, etaScaledJepJem_had, phiScaledJepJem_had, had_weight);
    }

    // number of triggered slice
    jem_1d_triggeredSlice_num = slice_n;
    fill(m_packageName, jem_1d_triggeredSlice_num);

    // HitMaps per time slice
    const std::vector<uint16_t> &emEnergyVec(je->emJetElementETVec());
    const std::vector<uint16_t> &hadEnergyVec(je->hadJetElementETVec());
    const int slicesEm = emEnergyVec.size();
    const int slicesHad = hadEnergyVec.size();
    for (int i = 0; i < m_maxSlices; i++) {
      if (i < slicesEm && emEnergyVec[i] > 0) {
        fillJEMEtaVsPhiMon(eta, phi, jem_em_2d_eta_jetEl_HitMapSlice[i], jem_em_2d_phi_jetEl_HitMapSlice[i]);
      }
      if (i < slicesHad && hadEnergyVec[i] > 0) {
        fillJEMEtaVsPhiMon(eta, phi, jem_had_2d_eta_jetEl_HitMapSlice[i], jem_had_2d_phi_jetEl_HitMapSlice[i]);
      }
    }

    // ERRORS
    const LVL1::DataError err(je->emJetElementError());
    const LVL1::DataError haderr(je->hadJetElementError());

    const int ypos = crate * 16 + module;

    bool emParityError = false;
    bool hadParityError = false;
    bool emLinkDownError = false;
    bool hadLinkDownError = false;
    // EM Parity
    if (err.get(DataError::Parity)) {
      emParityError = true;
      jemErrorSummary = EMParity;
      fill(m_packageName, jemErrorSummary);
      overview[crate] |= (1 << EMParity);
    }
    // HAD Parity
    if (haderr.get(DataError::Parity)) {
      hadParityError = true;
      jemErrorSummary = HadParity;
      fill(m_packageName, jemErrorSummary);
      overview[crate] |= (1 << HadParity);
    }
    // PPM Link down: em.
    if (err.get(DataError::LinkDown)) {
      emLinkDownError = true;
      jemErrorSummary = EMLink;
      fill(m_packageName, jemErrorSummary);
      overview[crate] |= (1 << EMLink);
    }
    // PPM Link down: had.
    if (haderr.get(DataError::LinkDown)) {
      hadLinkDownError = true;
      jemErrorSummary = HadLink;
      fill(m_packageName, jemErrorSummary);
      overview[crate] |= (1 << HadLink);
    }
    parityErrorJepJE_em = emParityError;
    fill(m_packageName, parityErrorJepJE_em);
    parityErrorJepJE_had = hadParityError;
    fill(m_packageName, parityErrorJepJE_had);
    linkDownErrorJepJE_em = emLinkDownError;
    fill(m_packageName, linkDownErrorJepJE_em);
    linkDownErrorJepJE_had = hadLinkDownError;
    fill(m_packageName, linkDownErrorJepJE_had);

    // Errors from substatus word from ROD: JEM
    const int status = (err.error() >> LVL1::DataError::GLinkParity) & 0xff;
    if (status) {
      jemLoc = ypos;
      for (int bit = 0; bit < 8; ++bit) {
        if ((status >> bit) & 0x1) {
          GLinkParityError = bit;
          fill(m_packageName, GLinkParityError, jemLoc);
        };
      }
      jemErrorSummary = JEMStatus;
      fill(m_packageName, jemErrorSummary);
      overview[crate] |= (1 << JEMStatus);
    }
  } // iterator

  // ===============
  // For JEM Et Sums
  // ===============

  // Retrieve Et sums from SG
  SG::ReadHandle<xAOD::JEMEtSumsContainer> JEMEtSums(m_JEMEtSumsLocation, ctx);
  if (!JEMEtSums.isValid()) {
    ATH_MSG_ERROR("No JEMEtSums found in TES at " << m_JEMEtSumsLocation);
    return StatusCode::FAILURE;
  }

  auto etSumJE_Ex = Monitored::Scalar<int>("JEMEtSums_Ex", 0.);
  auto etSumJE_Ey = Monitored::Scalar<int>("JEMEtSums_Ey", 0.);
  auto etSumJE_Et = Monitored::Scalar<int>("JEMEtSums_Et", 0.);

  ATH_MSG_DEBUG("-------------- JEM Et Sums ---------------");

  xAOD::JEMEtSumsContainer::const_iterator sumsIterator = (*JEMEtSums).begin();
  xAOD::JEMEtSumsContainer::const_iterator sumsIteratorEnd = (*JEMEtSums).end();
  for (; sumsIterator != sumsIteratorEnd; ++sumsIterator) {
    const xAOD::JEMEtSums* sums = *sumsIterator;
    const int ex = sums->ex();
    const int ey = sums->ey();
    const int et = sums->et();

    etSumJE_Ex = ex;
    etSumJE_Ey = ey;
    etSumJE_Et = et;

    if (ex != 0) fill(m_packageName, etSumJE_Ex);
    if (ey != 0) fill(m_packageName, etSumJE_Ey);
    if (et != 0) fill(m_packageName, etSumJE_Et);

    ATH_MSG_DEBUG(" JEMEtSums Crate: " << sums->crate()
                      << "  Module: "  << sums->module()
                      << "   Ex: "     <<  ex
                      << "   Ey: "     <<  ey
                      << "   Et: "     <<  et);
  }

  // ==================
  // ==== JEM ROIS ====
  // ==================

  // Retrieve JEM TOB RoIs from SG
  SG::ReadHandle<xAOD::JEMTobRoIContainer> JEMRoIs(m_JEMRoILocation, ctx);
  if (!JEMRoIs.isValid()) {
    ATH_MSG_ERROR("No JEM RoIs found in TES at " << m_JEMRoILocation);
    return StatusCode::FAILURE;
  }

  auto jemRoiEnergyLg = Monitored::Scalar<int>("jemRoiEnergyLg", 0.);
  auto jemRoiEnergySm = Monitored::Scalar<int>("jemRoiEnergySm", 0.);
  auto jemRoiTOBsPerJEM = Monitored::Scalar<int>("jemRoiTOBsPerJEM", 0.);
  auto jemRoiEta = Monitored::Scalar<double>("jemRoiEta", 0.);
  auto jemRoiPhi = Monitored::Scalar<double>("jemRoiPhi", 0.);
  auto jemRoiEtaWeighted = Monitored::Scalar<double>("jemRoiEtaWeighted", 0.);
  auto jemRoiPhiWeighted = Monitored::Scalar<double>("jemRoiPhiWeighted", 0.);
  auto jemRoiEnergyWeight = Monitored::Scalar<int>("jemRoiEnergyWeight", 0.);

  ATH_MSG_DEBUG("-------------- JEM RoIs ---------------");

  xAOD::JEMTobRoIContainer::const_iterator roiIterator = (*JEMRoIs).begin();
  xAOD::JEMTobRoIContainer::const_iterator roiIteratorEnd = (*JEMRoIs).end();
  const int vecSize = m_crates *m_modules;
  std::vector<int> tobCount(vecSize);
  for (; roiIterator != roiIteratorEnd; ++roiIterator) {
    const xAOD::JEMTobRoI* roi = *roiIterator;
    const int crate = roi->crate();
    const int module = roi->jem();
    const int energyLg = roi->energyLarge();
    const int energySm = roi->energySmall();
    LVL1::JEPRoIDecoder decoder;
    const LVL1::CoordinateRange coordRange = decoder.coordinate(roi->roiWord());
    const double eta = coordRange.eta();
    const double phi = coordRange.phi();

    if (energyLg) {
      jemRoiEnergyLg = energyLg;
      fill(m_packageName, jemRoiEnergyLg);
      jemRoiEnergyWeight = energyLg;
      fill(m_packageName, jemRoiEnergyWeight);
      fillJEMRoIEtaVsPhiMon(eta, phi, jemRoiEta, jemRoiPhi);
      fillJEMRoIEtaVsPhiMon(eta, phi, jemRoiEtaWeighted, jemRoiPhiWeighted, jemRoiEnergyWeight);
    }
    if (energySm) {
      jemRoiEnergySm = energySm;
      fill(m_packageName, jemRoiEnergySm);
    }
    ++tobCount[crate * m_modules + module];
  }

  for (int crate = 0; crate < m_crates; ++crate) {
    for (int jem = 0; jem < m_modules; ++jem) {
      int val = tobCount[crate * m_modules + jem];
      if (val) {
        if (val > m_tobsPerJEM) val = m_tobsPerJEM + 1;
        jemRoiTOBsPerJEM = val;
        fill(m_packageName, jemRoiTOBsPerJEM);
      }
    }
  }

  return StatusCode::SUCCESS;
}

void JepJemMonitorAlgorithm::fillJEMPhiMon(double eta, double phi,
                                           Monitored::Scalar<double> &phiMonitoredScalar
                                           ) const
{
  const double halfBin = 1. / (2. * m_phiScaleJE);
  if (eta < -3.2 || eta > 3.2)
  {
    // Fill two bins for FCAL
    phiMonitoredScalar = phi + halfBin;
    fill(m_packageName, phiMonitoredScalar);
    phiMonitoredScalar = phi - halfBin;
    fill(m_packageName, phiMonitoredScalar);
  }
  else {
    phiMonitoredScalar = phi;
    fill(m_packageName, phiMonitoredScalar);
  }
}

void JepJemMonitorAlgorithm::fillJEMEtaVsPhiMon(double eta, double phi,
                                                Monitored::Scalar<double> &etaMonitoredScalar,
                                                Monitored::Scalar<double> &phiMonitoredScalar,
                                                Monitored::Scalar<int> &weight
                                                ) const
{
  double phiMod = phi * m_phiScaleJE;
  double etaMod = eta;
  const double absEta = std::abs(eta);
  if (absEta > 2.4) {
    int offset = 1;
    if      (absEta > 3.2) offset = 4;
    else if (absEta > 2.9) offset = 3;
    else if (absEta > 2.7) offset = 2;
    etaMod = 2.3 + 0.2 * offset;
    if (eta < 0.) etaMod = -etaMod;
  }
  etaMonitoredScalar = etaMod;
  if (eta < -3.2 || eta > 3.2) {
    // Fill two bins for FCAL
    phiMod = std::floor(phiMod / 2) * 2. + 1.;
    phiMonitoredScalar = phiMod + 0.5;
    fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar, weight);
    phiMonitoredScalar = phiMod - 0.5;
    fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar, weight);
  }
  else {
    phiMonitoredScalar = phiMod;
    fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar, weight);
  }
}

void JepJemMonitorAlgorithm::fillJEMEtaVsPhiMon(double eta, double phi,
                                                Monitored::Scalar<double> &etaMonitoredScalar,
                                                Monitored::Scalar<double> &phiMonitoredScalar
                                                ) const
{
  double phiMod = phi * m_phiScaleJE;
  double etaMod = eta;
  const double absEta = std::abs(eta);
  if (absEta > 2.4) {
    int offset = 1;
    if      (absEta > 3.2) offset = 4;
    else if (absEta > 2.9) offset = 3;
    else if (absEta > 2.7) offset = 2;
    etaMod = 2.3 + 0.2 * offset;
    if (eta < 0.) etaMod = -etaMod;
  }
  etaMonitoredScalar = etaMod;
  if (eta < -3.2 || eta > 3.2) {
    // Fill two bins for FCAL
    phiMod = std::floor(phiMod / 2) * 2. + 1.;
    phiMonitoredScalar = phiMod + 0.5;
    fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar);
    phiMonitoredScalar = phiMod - 0.5;
    fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar);
  }
  else {
    phiMonitoredScalar = phiMod;
    fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar);
  }
}

void JepJemMonitorAlgorithm::fillJEMRoIEtaVsPhiMon(double eta, double phi,
                                                   Monitored::Scalar<double> &etaMonitoredScalar,
                                                   Monitored::Scalar<double> &phiMonitoredScalar,
                                                   Monitored::Scalar<int> &weight
                                                   ) const
{
  const double phiMod = phi * m_phiScaleJE - 0.5;
  phiMonitoredScalar = phiMod;
  double etaMod = eta;
  const double absEta = std::abs(eta);
  if (absEta > 2.3) {
    int offset = 1;
    if      (absEta > 4.0)  offset = 5;
    else if (absEta > 3.05) offset = 4;
    else if (absEta > 2.8)  offset = 3;
    else if (absEta > 2.55) offset = 2;
    etaMod = 2.2 + 0.2 * offset;
    if (eta < 0.) etaMod = -etaMod;
  }
  const double etaShift = 0.1;
  // JEPRoIDecoder returns eta=3.9 for both of the two forwardmost bins
  if (eta > 3.8 && eta < 4.0) {
    etaMonitoredScalar = 3.2 - etaShift;
    fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar, weight);
  }
  etaMonitoredScalar = etaMod - etaShift;
  fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar, weight);
}

void JepJemMonitorAlgorithm::fillJEMRoIEtaVsPhiMon(double eta, double phi,
                                                   Monitored::Scalar<double> &etaMonitoredScalar,
                                                   Monitored::Scalar<double> &phiMonitoredScalar
                                                   ) const
{
  const double phiMod = phi * m_phiScaleJE - 0.5;
  phiMonitoredScalar = phiMod;
  double etaMod = eta;
  const double absEta = std::abs(eta);
  if (absEta > 2.3) {
    int offset = 1;
    if      (absEta > 4.0)  offset = 5;
    else if (absEta > 3.05) offset = 4;
    else if (absEta > 2.8)  offset = 3;
    else if (absEta > 2.55) offset = 2;
    etaMod = 2.2 + 0.2 * offset;
    if (eta < 0.) etaMod = -etaMod;
  }
  const double etaShift = 0.1;
  // JEPRoIDecoder returns eta=3.9 for both of the two forwardmost bins
  if (eta > 3.8 && eta < 4.0) {
    etaMonitoredScalar = 3.2 - etaShift;
    fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar);
  }
  etaMonitoredScalar = etaMod - etaShift;
  fill(m_packageName, etaMonitoredScalar, phiMonitoredScalar);
}
