/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTNOISE_H
#define TRTNOISE_H

#include <vector>
#include <map>
#include <set>

#include "TRTDigit.h"

#include "GaudiKernel/ServiceHandle.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

class TRTDigCondBase;
class TRTElectronicsProcessing;
class TRTElectronicsNoise;
class IAtRndmGenSvc;
#include "AthenaKernel/MsgStreamMember.h"
#include "CLHEP/Random/RandomEngine.h"
class Identifier;
class TRT_ID;

namespace InDetDD {
  class TRT_DetectorManager;
}

class TRTDigSettings;

/**
 * Simulation of noise hits in the TRT. @n
 * For method, please see Thomas Kittelmanns thesis
 *
 */
class TRTNoise {
public:
  /**
   * Constructor
   */
  TRTNoise( const TRTDigSettings*,
	    const InDetDD::TRT_DetectorManager*,
	    ServiceHandle <IAtRndmGenSvc> atRndmGenSvc,
	    TRTDigCondBase* digcond,
	    TRTElectronicsProcessing * ep,
	    TRTElectronicsNoise * electronicsnoise,
	    const TRT_ID* trt_id,
	    int UseGasMix,
	    ServiceHandle<ITRT_StrawStatusSummarySvc> sumSvc
	  );

  /**
   * Append noise digits to list of digits from proper hits. @n
   * We are here dealing with unhit straws and noise digits are added from a
   * pool of cached digits (@c m_digitPool).
   * @param digitVect:  vector of @e digits this event.
   *                    At input, contains digits from hits.
   *                    At output, noise digits have been added.
   * @param sim_hitids: list of IDs of hit straws.
   *                    This to make sure we do not add
   *                    noise digit to already hit straw.
   */
  void appendPureNoiseToProperDigits( std::vector<TRTDigit>& digitVect,
				      const std::set<int>& sim_hitids) ;


  void appendCrossTalkNoiseToProperDigits(std::vector<TRTDigit>& digitVect,
					  const std::set<Identifier>& simhitsIdentifiers,
					  ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc);

  void sortDigits(std::vector<TRTDigit>& digitVect);

  ~TRTNoise();

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

 private:

  const TRTDigSettings* m_settings;
  const InDetDD::TRT_DetectorManager* m_detmgr;

  TRTDigCondBase * m_pDigConditions;
  TRTElectronicsProcessing * m_pElectronicsProcessing;
  TRTElectronicsNoise * m_pElectronicsNoise;
  CLHEP::HepRandomEngine* m_noise_randengine;

  //---  Initialization:

  /**
   * Initialize thresholds and noise amplitudes. @n
   * This is the main initialization method for the noise treatment. @n
   * Strategy:
   * - First we create a lookup table so that we can, based on the
   *   noiselevel_i (i=straw_id), calculate for each straw: (LT/NA)_i
   *   (NA=Noise Amplitude).
   * - We also know for each straw their relative noise amplitude, r_i.
   * - C.f. Thomas Kittelmanns thesis, one finds that the lowthreshold and
   *   noise amplitude for each straw is given by the formulas: @n
   *   LT_i = (LT/NA)_i * r_i * k   and   NA_i = r_i * k,
   *      with k = < LT_i > / <(LT/NA)_i * r_i>
   *
   * Steps:
   * -# Create lookup table for mapping: noiselevel to LT/NA
   * -# Find constant: k=< LT_i > / <(LT/NA)_i * r_i>
   * -# Calculate and set actual values of LT_i and NA_i for each straw.
   * -# Call method @c ProduceNoiseDigitPool to produce pool of pure noise
   *   digits
   */
  void InitThresholdsAndNoiseAmplitudes_and_ProduceNoiseDigitPool();

  /**
   * Produce pool of pure noise digits (for simulation of noise in unhit
   * straws) and store them in @c m_digitPool. @n
   * The procedure uses @c TRTElectronicsProcessing::ProcessDeposits(), but
   * with no input deposits. This way, we get the noise contribution only.
   * The noise signals are thus originating from the cached vectors
   * @c TRTElectronicsNoise::m_cachedFastNoiseAfterSignalShaping[]
   * and
   * @c TRTElectronicsNoise::m_cachedSlowNoiseAfterSignalShaping[]
   * @param lowthresholds: low threshold settings, all straws
   * @param noiseamp:      noise amplitudes, all straws
   */
  void ProduceNoiseDigitPool( const std::vector<float>& lowthresholds,
			      const std::vector<float>& noiseamps,
			      const std::vector<int>& strawType
			    );

  const TRT_ID* m_id_helper;
  /** Length of noise digit pool @c m_digitPool */
  unsigned int m_digitPoolLength;
  /** Pointer into noise digit pool @c m_digitPool */
  unsigned int m_digitPoolLength_nextaccessindex;
  /** Pool of noise digits for noise in unhit straws */
  std::vector<unsigned int> m_digitPool;

  double simulateNoiseFrequency( const unsigned int& totalNumberOfTests,
				 const double& electronicsNoiseAmplitude,
				 double lowthreshold = -1.0 );

  /**
   * Return @a y value corresponding to input @a x value from LUT
   * @return y: Output @a y value
   * @param x:         Input @a x value
   * @param y_given_x: Look-up-table: gives @a y values for uniformly
   *                   distributed @a x values
   * @param min_x:     lowest tabulated @a x value
   * @param max_x:     highest tabulated @a x value
   */
  float useLookupTable(const float& x,
		       const std::vector<float>& y_given_x,
		       const float & min_x,
		       const float & max_x);

  /**
   * Invert look-up-table: Go from tabulated @a y values vs. equidistantly
   * spaced @a x values to tabulated @a x values vs. equidistantly spaced
   * @a y values.
   * @param y_given_x: @a y given @a x   (input)
   * @param min_x:     lower @a x value  (input)
   * @param max_x:     higher @a x value (input)
   * @param x_given_y: @a x given @a y   (output)
   * @param min_y:     lower @a y value  (output)
   * @param max_y:     higher @a y value (output)
   */
  void makeInvertedLookupTable( const std::vector<float>& y_given_x,
  				const float & min_x,
  				const float & max_x,
  				std::vector<float>& x_given_y,
  				float & min_y,
  				float & max_y );
  /**
   * Refined noise treatment by allowing for event-by-event fluctuations in
   * the low threshold settings. This is done by smearing of a LUT giving noise
   * fractions as function of LT/NA values (see TK thesis).
   *
   * What this method effectively does, is to smear the x-axis in a LUT
   * with a Gaussian with a given relative width.
   * @param nl_given_lt2na: The (inverted) LUT of noise level vs. LT/NA
   *                        values (input and output)
   * @param min_lt2na: lower LT/NA value (input)
   * @param max_lt2na: higher LT/NA value (input)
   * @param relativeLTfluct: relative event-by-event fluctuations in low
   *                         threshold level (input)
   * @param new_min_lt2na: lower LT/NA value after smearing (output)
   * @param new_max_lt2na: higher LT/NA value after smearing (output)
   * @param number_new_bins: LUT is expanded to this number of bins (input)
   */
  void evolve_LT2AmpVsNL_to_include_LTfluct( std::vector<float>& nl_given_lt2na,
  					     const float & min_lt2na,
  					     const float & max_lt2na,
  					     const float relativeLTFluct,
  					     float & new_min_lt2na,
  					     float & new_max_lt2na,
  					     const unsigned int& number_new_bins );
  mutable Athena::MsgStreamMember m_msg;

  Identifier getStrawIdentifier (int hitID);

  // The straw's gas mix: 1=Xe, 2=Kr, 3=Ar
  int StrawGasType(Identifier TRT_Identifier);

  unsigned int getRegion(int hitID,const TRT_ID* trt_id);
  int m_UseGasMix;
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_sumSvc; // need for Argon

};

#endif
