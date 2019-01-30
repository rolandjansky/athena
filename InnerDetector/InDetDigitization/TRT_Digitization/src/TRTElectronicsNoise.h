/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTELECTRONICSNOISE_H
#define TRTELECTRONICSNOISE_H

#include <vector>

#include "AthenaKernel/MsgStreamMember.h"

#include "CLHEP/Random/RandomEngine.h"
class TRTDigSettings;

/**
 * Simulate TRT Electronics Noise @n
 * For description of metod, see Thomas Kittelmanns PhD thesis chapters 9-11.
 */
class TRTElectronicsNoise {

public:
  /**
   * Constructor: Calls tabulateNoiseSignalShape()
   */
  TRTElectronicsNoise( const TRTDigSettings*,
		       CLHEP::HepRandomEngine *rndmEngine );
  /** Destructor */
  ~TRTElectronicsNoise();

  /**
   * From generated (and cached) noise samples, this function returns
   * in vector @c maxLTOverNoiseAmp the maximum of the noise amplitude
   * within each of @c nsamplings 75 ns long samplings.
   *
   * @param maxLTOverNoiseAmp: vector containing max noise signal for each
   *                           of the 75 ns long sampling
   * @param nsamplings:        number of samplings
   */

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

  void getSamplesOfMaxLTOverNoiseAmp(std::vector<float>& maxLTOverNoiseAmp,
				     unsigned long nsamplings,
                                     CLHEP::HepRandomEngine *rndmEngine);

  /**
   * Re-initialize electronics noise table.
   *
   * Fills the cached tables
   * @c m_cachedFastNoiseAfterSignalShaping and
   * @c m_cachedSlowNoiseAfterSignalShaping with
   * @c numberOfDigitLengths 75 ns long noise signals.
   *
   * Procedure (same for fast and slow signals)
   * -# Create periodic noise pulses with Gaussian pulse height centered
   *    around zero and with unit width.
   * -# Shape pulses according to @c noiseSignalShaping into cached tables
   *
   * @param numberOfDigitLengths: number of 75ns timeslices to generate
   *                              noise for
   */
  void reinitElectronicsNoise(const unsigned int& numberOfDigitLengths
			      /*number of 75ns timeslices*/,
                              CLHEP::HepRandomEngine *rndmEngine);

  /** Set electronics noise amplitude */
  void setElectronicsNoiseAmplitude(const double&);

  /**
   * Add electronics noise to simulated signals in hit straws. Analog noise
   * is added to simulated signal after signal shaping.
   * The analog noise is taken from @c m_cachedFastNoiseAfterSignalShaping[]
   * and @c m_cachedSlowNoiseAfterSignalShaping[]
   * @param signal:         tabulated analog shaped signal
   * @param noiseamplitude: noise amplitude
   */
  void addElectronicsNoise(std::vector<double>& signal,
			   const double& noiseamplitude /*= 1.0*/,
                           CLHEP::HepRandomEngine *rndmEngine);

private:

  const TRTDigSettings* m_settings;

  /**
   * Tabulate noise signal shape. Extract signal shape from NoiseShape(time)
   * function and store in table.
   */
  void tabulateNoiseSignalShape();

  /**
   * Get max amplitude of noise signal from weighted sum of fast and slow
   * varying noise in period
   * @return max signal
   * @param firstbinslowsignal: pointer into array (LUT) of slow noise signals
   * @param firstbinfastsignal: pointer into array (LUT) of fast noise signals
   * @param binsinwindow: number of bins in window, where maximum is searched
   */
  double getMax(unsigned int firstbinslowsignal,
		unsigned int firstbinfastsignal,
		const unsigned int& binsinwindow );

  std::vector<double> m_noiseSignalShape; /**< Tabulated noise signal shape */

  /** Cached samples of electronics noise after signal shaping (fast input) */
  std::vector<float> m_cachedFastNoiseAfterSignalShaping;
  /** Cached samples of electronics noise after signal shaping (slow input) */
  std::vector<float> m_cachedSlowNoiseAfterSignalShaping;

  std::vector<float> m_tmpArray;/**< Here to avoid repeated memory allocs */

  double m_fractionOfSlowNoise;
  unsigned int m_nbins_periodic;

  mutable Athena::MsgStreamMember m_msg;

  /**
   * Initialize signal shaping.
   * @note The noise shape parameters are hardcoded here.
   */
  void InitializeNoiseShaping();
  /**
   * Get parameterized signal shape for noise hits
   * @return Signal amplitude
   * @param  time: time after signal arrival
   */
  double NoiseShape(const double& time) const;

  std::vector<double> m_noisepars1;/**< Noise signal params 1 (t<15.5 ns) */
  std::vector<double> m_noisepars2;/**< Noise signal params 2 (t>15.5 ns) */

};

#endif
