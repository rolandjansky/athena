/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_Digitization/HGTD_TimingResolution.h
 *
 * @author Tao Wang <tao.wang@cern.ch>
 *
 * @date October, 2021
 *
 * @brief Smearing LGAD timing based on integrated luminosity and radius
 */

#ifndef HGTD_TIMINGRESOLUTION_H
#define HGTD_TIMINGRESOLUTION_H

#include <map>
#include <string>
#include <vector>

#include "CLHEP/Random/RandomEngine.h"

class HGTD_TimingResolution {
public:
  HGTD_TimingResolution(float integratedLumi = 0, float Rmin = 120.,
                        float Rmax = 670., float z = 3500.);

  // Initialize and check
  void init();
  bool radiusInRange(float) const;
  bool etaInRange(float);

  // methods
  float hitTimingResolution(float radius) const;

  bool changeResolution2(float integratedLumi);
  float resolution(float) const;
  float gain(float);
  float sensorResolution() const;

  float translateR2Eta(float);
  float translateEta2R(float);

  void radToResolution();
  void radToGain();
  void ComputeDose(float lumi);
  void print();

  /** Return simulated CFD time */
  float calculateTime(const float t, const float E, float r, CLHEP::HepRandomEngine* rndm_engine) const;

  constexpr static float HGTDInitialLuminosity = 0.0;
  constexpr static float HGTDIntermediateLuminosity_pre_Replacement = 2000.0;
  constexpr static float HGTDIntermediateLuminosity_post_Replacement = 2001.0;
  constexpr static float HGTDFinalLuminosity = 4000.0;

private:
  std::string m_version;

  float m_Rmin;
  float m_Rmax;

  float m_z;
  float m_integratedLumi;

  float m_sensorResolution;

  std::vector<int> m_radii;
  // Inner ring is replaced every 1 ab-1, middle ring replaced every 2 ab-1,
  // outer ring will be used till 4000 fb-1 Dose radius relation for inner ring
  // at 1000 fb-1 integrated luminosity
  std::vector<float> m_doseInner1000;
  std::vector<float> m_doseMiddle2000;
  std::vector<float> m_doseOuter4000;
  std::vector<float> m_dose;
  std::vector<float> m_resolution;
  std::vector<float> m_gain;

  std::vector<std::pair<float, float>> m_doseResolution;
  std::vector<std::pair<float, float>> m_doseGain;

  float m_sensorNoiseFactor =
      0.0229; // factor use to simulate the electronic noise (1mm*1mm)

  float m_cfdThreshold = 0.5; // fraction of max amplitude at which the time is
                               // extracted (default: 50%)

  float mutable m_pulseWaveform[400] = {0}; // Signal Pulse

  /** Simulate a new pulse that can be acess using the PulseShape method */
  void simulatePulse(CLHEP::HepRandomEngine* rndm_engine) const;

  /** Calculate the pulse as a vector of float (400 points) */
  void calculatePulse(std::map<int, std::pair<float, float>> &pulsebin,
                      float t, float E, float *max, CLHEP::HepRandomEngine* rndm_engine) const;


  // std::unique_ptr<HGTD_TimingResolution> m_timingResolution;
};

#endif
