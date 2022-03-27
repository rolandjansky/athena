/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_Digitization/src/HGTD_TimingResolution.cxx
 *
 * @author Tao Wang <tao.wang@cern.ch>
 *
 * @date October, 2021
 */

#include "HGTD_TimingResolution.h"

#include "CLHEP/Random/RandGaussZiggurat.h"
#include "TMath.h"
#include <cmath>
#include <iostream>

namespace {
inline float remainder_1(float a, int b) {
  float remainderValue = std::fmod(a, b);
  if (remainderValue != 0. || a == 0.) {
    return remainderValue;
  } else {
    return static_cast<float>(b);
  }
}

inline float exp16(float x) {
  x = 1.0 + x / 16.0;
  return std::pow(x, 16);
}
} // namespace

HGTD_TimingResolution::HGTD_TimingResolution(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
        : AthAlgTool(type, name, parent) {

  m_version = "HGTD Timing three-ring layout";

  // Contain also the peripheral electronics region, i.e. (R>640 mm)
  m_radii = {120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230,
             240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350,
             360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470,
             480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590,
             600, 610, 620, 630, 640, 650, 660, 670, 680, 690};

  m_doseInner1000 = {1.995, 1.953, 1.787, 1.687, 1.608, 1.518,
                     1.448, 1.425, 1.354, 1.328, 1.275};

  m_doseMiddle2000 = {2.44,  2.355, 2.26,  2.204, 2.137, 2.081, 2.015, 1.96,
                      1.878, 1.87,  1.809, 1.732, 1.722, 1.63,  1.588, 1.57,
                      1.509, 1.464, 1.44,  1.38,  1.333, 1.321, 1.284, 1.271};

  m_doseOuter4000 = {2.458, 2.4,   2.382, 2.362, 2.266, 2.207, 2.122, 2.067,
                     2.046, 1.973, 1.94,  1.9,   1.869, 1.83,  1.711, 1.701,
                     1.685, 1.615, 1.626, 1.565, 1.53,  1.499, 1.429};

  m_doseResolution = {{0.01, 0.025}, {0.1, 0.031}, {0.3, 0.035}, {0.6, 0.040},
                      {1.0, 0.046},  {3.0, 0.065}, {6.0, 0.065}};

  m_doseGain = {{0.01, 39}, {0.1, 23}, {0.3, 21}, {0.6, 19},
                {1.0, 10},  {3.0, 5},  {6.0, 4}};

  // resolution in ns
  m_sensorResolution = std::sqrt((0.025 * 0.025) - (0.010 * 0.010));

}

StatusCode HGTD_TimingResolution::initialize() {

  if (m_radii.size() != (m_doseInner1000.size() + m_doseMiddle2000.size() +
                         m_doseOuter4000.size())) {
    ATH_MSG_FATAL("ERROR while initializing the HGTD timing resolution. Vector of different size.");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(setIntegratedLuminosity(0.));

  return StatusCode::SUCCESS;
}

StatusCode HGTD_TimingResolution::setIntegratedLuminosity(float integratedLumi) {
  m_integratedLumi = integratedLumi;

  m_dose.clear();
  m_resolution.clear();
  m_gain.clear();
  computeDose(m_integratedLumi);
  radToResolution();
  radToGain();

  return StatusCode::SUCCESS;
}

// get the timing resolution for a hit at a radius R (unit: mm)
float HGTD_TimingResolution::hitTimingResolution(float radius) const {

  if (!radiusInRange(radius))
    return 99999.;
  float sigmaT = 9999.;
  if (m_integratedLumi == 0)
    sigmaT = m_resolution[0];
  else
    sigmaT = resolution(std::fabs(radius));

  // include the effect of electronics as 25ps added in quadrature
  float sigmaT2 = sigmaT * sigmaT + m_sensorResolution * m_sensorResolution;
  return std::sqrt(sigmaT2);
}

// returns the per Hit resolution for the final performance
float HGTD_TimingResolution::resolution(float radius) const {
  float resolution = 9999.;

  if (radius < m_radii[0]) {
    resolution = m_resolution[0];
    return resolution;
  }

  for (unsigned int i = 1; i < m_radii.size(); i++) {

    if (radius < m_radii[i]) {
      resolution =
          m_resolution[i - 1] +
          (((radius - m_radii[i - 1]) / (m_radii[i] - m_radii[i - 1])) *
           (m_resolution[i] - m_resolution[i - 1]));
      return resolution;
    }
  }

  resolution = m_resolution[m_resolution.size() - 1];
  return resolution;
}

float HGTD_TimingResolution::gain(float radius) const {
  float gain = 1.;

  if (radius < m_radii[0]) {
    gain = m_gain[0];
    return gain;
  }

  for (unsigned int i = 1; i < m_radii.size(); i++) {

    if (radius < m_radii[i]) {
      gain = m_gain[i - 1] +
             (((radius - m_radii[i - 1]) / (m_radii[i] - m_radii[i - 1])) *
              (m_gain[i] - m_gain[i - 1]));
      return gain;
    }
  }

  gain = m_gain[m_gain.size() - 1];
  return gain;
}

inline float HGTD_TimingResolution::sensorResolution() const {
  return m_sensorResolution;
}

// check if the request is in range:
bool HGTD_TimingResolution::radiusInRange(float radius) const {
  return (std::fabs(radius) >= m_Rmin && std::fabs(radius) <= m_Rmax);
}

// check if the request is in range:
bool HGTD_TimingResolution::etaInRange(float eta) const {
  float radius = translateEta2R(eta);
  return radiusInRange(radius);
}

// translate radius to eta
float HGTD_TimingResolution::translateR2Eta(float radius) const {
  return std::fabs(
      -std::log(std::tan(std::atan(std::fabs(radius) / m_z) / 2.)));
}

// translate eta to radius
float HGTD_TimingResolution::translateEta2R(float eta) const {
  return std::fabs(std::tan(std::atan(std::exp(-eta)) * 2.) * m_z);
}

void HGTD_TimingResolution::computeDose(float lumi) {
  for (unsigned int i = 0; i < m_doseInner1000.size(); ++i) {
    m_dose.push_back(remainder_1(lumi, 1000) / 1000 * m_doseInner1000[i]);
  }
  for (unsigned int i = 0; i < m_doseMiddle2000.size(); ++i) {
    m_dose.push_back(remainder_1(lumi, 2000) / 2000 * m_doseMiddle2000[i]);
  }
  for (unsigned int i = 0; i < m_doseOuter4000.size(); ++i) {
    m_dose.push_back(lumi / 4000 * m_doseOuter4000[i]);
  }
}

void HGTD_TimingResolution::radToResolution() {

  for (unsigned int i = 0; i < m_dose.size(); i++) {
    if (m_dose[i] < m_doseResolution[0].first) {
      m_resolution.push_back(m_doseResolution[0].second);
      continue;
    }
    for (unsigned int j = 1; j < m_doseResolution.size(); j++) {
      if (m_dose[i] < m_doseResolution[j].first) {
        float res =
            m_doseResolution[j - 1].second +
            (((m_dose[i] - m_doseResolution[j - 1].first) /
              (m_doseResolution[j].first - m_doseResolution[j - 1].first)) *
             (m_doseResolution[j].second - m_doseResolution[j - 1].second));
        m_resolution.push_back(res);
        break;
      }

      else if (j == m_doseResolution.size() - 1) {
        m_resolution.push_back(
            m_doseResolution[m_doseResolution.size() - 1].second);
      }
    }
  }
}

void HGTD_TimingResolution::radToGain() {
  for (unsigned int i = 0; i < m_dose.size(); i++) {
    if (m_dose[i] < m_doseGain[0].first) {
      m_gain.push_back(m_doseGain[0].second);
      continue;
    }
    for (unsigned int j = 1; j < m_doseGain.size(); j++) {
      if (m_dose[i] < m_doseGain[j].first) {
        float gain = m_doseGain[j - 1].second +
                     (((m_dose[i] - m_doseGain[j - 1].first) /
                       (m_doseGain[j].first - m_doseGain[j - 1].first)) *
                      (m_doseGain[j].second - m_doseGain[j - 1].second));
        m_gain.push_back(gain);
        break;
      }

      else if (j == m_doseGain.size() - 1) {
        m_gain.push_back(m_doseGain[m_doseGain.size() - 1].second);
      }
    }
  }
}

void HGTD_TimingResolution::print() {
  ATH_MSG_VERBOSE("HGTD_TimingResolution version: " << m_version);
  ATH_MSG_VERBOSE("Integrated Lumi: " << m_integratedLumi);
  ATH_MSG_VERBOSE("Rmin = " << m_Rmin);
  ATH_MSG_VERBOSE("Rmax = " << m_Rmax);
  ATH_MSG_VERBOSE("Z    = " << m_z);

  ATH_MSG_VERBOSE("");
  for (unsigned int i = 0; i < m_resolution.size(); i++) {
    ATH_MSG_VERBOSE("Resolution r = " << m_radii[i] << " mm " << hitTimingResolution(m_radii[i]) << " ps");
    ATH_MSG_VERBOSE("Gain r = " << m_radii[i] << " mm " << gain(m_radii[i]));
  }
}

void HGTD_TimingResolution::simulatePulse(
    CLHEP::HepRandomEngine *rndm_engine) const {

  float pulseParameter[4] = {0};
  pulseParameter[0] = 0.036; // Width (scale) parameter of Landau density
  pulseParameter[1] =
      5.754; // Most Probable (MP, location) parameter of Landau density
  pulseParameter[3] = CLHEP::RandGaussZiggurat::shoot(
      rndm_engine, 0.318,
      m_sensorNoiseFactor -
          0.1 * m_sensorNoiseFactor); // Width (sigma) of convoluted Gaussian
                                      // function
  pulseParameter[2] =
      0.4785 + 1.4196 * pulseParameter[3]; // Total area (integral -inf to inf,
                                           // normalization constant)

  // Convoluted Landau and Gaussian Fitting Function
  // Adapted to simulate a pulse in an HGTD pad

  // Numeric constants
  float invsq2pi = 1 / sqrt(2 * M_PI); // 0.3989422804014;   // (2 pi)^(-1/2)
  float mpshift = -0.22278298;         // Landau maximum location

  // Variables
  float mpc = pulseParameter[1] - mpshift * pulseParameter[0];
  float step = 0.03;
  float step_par = step / (sqrt(2) * pulseParameter[3]);
  float f_weight = pulseParameter[2] * step * invsq2pi / pulseParameter[3];
  float fland;
  float p_time;

  // Convolution integral of Landau and Gaussian by sum
  for (int i = 412; i < 527; i++) {

    fland = TMath::Landau(3.5 + (i * 0.005), mpc, pulseParameter[0]) /
            pulseParameter[0];
    p_time = (1.5 + (i % 6 - i) * 0.005) / (sqrt(2) * pulseParameter[3]);

    for (int point = i % 6; point < 400; point += 6) {
      p_time += step_par;
      m_pulseWaveform[point] += fland * exp16(-p_time * p_time);
    }
  }
  for (int point = 0; point < 400; point++) {
    m_pulseWaveform[point] = f_weight * m_pulseWaveform[point];
  }
}

void HGTD_TimingResolution::calculatePulse(
    std::map<int, std::pair<float, float>> &pulsebin, const float t,
    const float E, float *max, CLHEP::HepRandomEngine *rndm_engine) const {

  int timebin = 0;
  float energy = 0;
  float time = 0;

  for (int point = 0; point < 400; point++) {

    energy = m_pulseWaveform[point] * E +
             CLHEP::RandGaussZiggurat::shoot(rndm_engine, 0, 0.015) * E;
    time = t + point * 0.005;
    timebin = (int)(t / 0.005) + point;

    auto pulse = pulsebin.find(timebin);
    if (pulse == pulsebin.end()) {
      pulsebin.insert({timebin, {time * energy, energy}});
      pulse = pulsebin.find(timebin);
    } else {
      (*pulse).second = {(*pulse).second.first + time * energy,
                         (*pulse).second.second + energy};
    }
    if (max[0] < (*pulse).second.second) {
      max[0] = (*pulse).second.second;
      max[1] = (*pulse).second.first / max[0];
    }
  }
  return;
}

float HGTD_TimingResolution::calculateTime(
    const float t, const float E, const float r,
    CLHEP::HepRandomEngine *rndm_engine) const {
  std::map<int, std::pair<float, float>> pulseBins;
  float max_hit[4] = {0};
  simulatePulse(rndm_engine);
  calculatePulse(pulseBins, t, E, max_hit, rndm_engine);
  for (auto &pulse : pulseBins) {
    pulse.second.first = pulse.second.first / pulse.second.second;
    // We look the the time when E=Emax/2 to get the time
    if ((max_hit[1] > pulse.second.first) &&
        (max_hit[0] * m_cfdThreshold < pulse.second.second) &&
        (max_hit[3] > pulse.second.first || max_hit[3] == 0)) {
      max_hit[2] = pulse.second.second; // Energy when E=Emax*m_cfd
      max_hit[3] = pulse.second.first;  // Time when E=Emax*m_cfd
    }
  }
  float electronicRes = hitTimingResolution(r);
  electronicRes = sqrt(electronicRes * electronicRes -
                       sensorResolution() * sensorResolution());
  float cfdTime =
      max_hit[3] + CLHEP::RandGaussZiggurat::shoot(
                       rndm_engine, 0, electronicRes); // adds electric noise
  return cfdTime;
}
