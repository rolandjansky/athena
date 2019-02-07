/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTElectronicsNoise.h"

#include "CLHEP/Units/SystemOfUnits.h"

// For the Athena-based random numbers.
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"

#include "TRTDigSettings.h"

#include <cmath>
#include <cstdlib>

//_____________________________________________________________________________
TRTElectronicsNoise::TRTElectronicsNoise(const TRTDigSettings* digset,
					 CLHEP::HepRandomEngine *elecNoiseRndmEngine )
  : m_settings(digset),
    m_msg("TRTElectronicsNoise")
{

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TRTElectronicsNoise::Constructor begin"  << endmsg;

  //Need to initialize the signal shaping first as it is used in tabulateNoiseSignalShape()!
  this->InitializeNoiseShaping();
  m_fractionOfSlowNoise = m_settings->slowPeriodicNoisePulseFraction();
  this->tabulateNoiseSignalShape();
  this->reinitElectronicsNoise(200, elecNoiseRndmEngine);
  const double slowPeriod(m_settings->slowPeriodicNoisePulseDistance());

  //Must be rounded to nearest multiple of binwidth... (fixme - from options)
  const double binwidth(m_settings->timeInterval()/m_settings->numberOfBins());
  m_nbins_periodic = static_cast<int>(slowPeriod/binwidth + 0.5);

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TRTElectronicsNoise::Constructor done"   << endmsg;

}

//_____________________________________________________________________________
TRTElectronicsNoise::~TRTElectronicsNoise(){}

//_____________________________________________________________________________
void TRTElectronicsNoise::getSamplesOfMaxLTOverNoiseAmp(std::vector<float>& maxLTOverNoiseAmp,
							unsigned long nsamplings, CLHEP::HepRandomEngine* rndmEngine) {

  // Note: The offset structure is not the same as in
  // addElectronicsNoise(), but that is OK, since it is not the exact
  // time-structure we are looking for here - but only the distribution
  // of the highest amplitude.

  maxLTOverNoiseAmp.resize(nsamplings);

  reinitElectronicsNoise(500, rndmEngine);
  unsigned int index         = m_noiseSignalShape.size();
  unsigned int nbinsinperiod = m_settings->numberOfBins();
  unsigned int maxindex      = m_cachedFastNoiseAfterSignalShaping.size() - nbinsinperiod;

  for (unsigned int i = 0; i < nsamplings; ++i) {
    maxLTOverNoiseAmp[i] = getMax(index, index, nbinsinperiod );
    index += nbinsinperiod;
    if ( index  > maxindex ) {
      reinitElectronicsNoise(500, rndmEngine);
      index = m_noiseSignalShape.size();
    }
  }
}

//_____________________________________________________________________________
double TRTElectronicsNoise::getMax(unsigned int firstbinslowsignal,
				   unsigned int firstbinfastsignal,
				   const unsigned int& binsinwindow )
{

  // This method assumes that firstbinslowsignal + binsinwindow doesn't
  // exceed the cached array sizes (and same for fastsignal).

  double max = -99999.0;
  unsigned int lastslowbinplusone = firstbinslowsignal + binsinwindow;

  while( firstbinslowsignal < lastslowbinplusone ) {
    double totalsig =
      m_cachedFastNoiseAfterSignalShaping[firstbinfastsignal++] +
      m_cachedSlowNoiseAfterSignalShaping[firstbinslowsignal++];
    if ( max<totalsig ) max = totalsig;
  };

  return max;
}

//_____________________________________________________________________________
void TRTElectronicsNoise::reinitElectronicsNoise(const unsigned int& numberOfDigitLengths /*number of 75ns timeslices*/,
                                                 CLHEP::HepRandomEngine* rndmEngine)
{
  //This method gives the actual physics shape!
  //Model parameters:

  const double fastPeriod = m_settings->fastElectronicsNoisePulseDistance();
  //Must be rounded to nearest multiple of binwidth... (fixme - from options)

  const double slowPeriod = m_settings->slowPeriodicNoisePulseDistance();
  //Must be rounded to nearest multiple of binwidth... (fixme - from options)

  //For consistency we start the arrays a little earlier than the bins we use -
  //the time back from where the signal shaping could carry a pulse:
  unsigned int nbins = m_noiseSignalShape.size() + numberOfDigitLengths * m_settings->numberOfBins();

  m_cachedFastNoiseAfterSignalShaping.resize(nbins);
  m_cachedSlowNoiseAfterSignalShaping.resize(nbins);

  m_tmpArray.resize(nbins);

  const double invbinwidth(m_settings->numberOfBins() / m_settings->timeInterval());
  double timeOfNextPulse;
  unsigned int binindex;

  //### Fast signal:
  double fractionOfFastNoise = 1.0-m_fractionOfSlowNoise;

  //### First we produce the noise as it is BEFORE signal shaping:
  for (unsigned int i(0); i < nbins; ++i) {
    m_tmpArray[i] = 0.;
    m_cachedFastNoiseAfterSignalShaping[i] = 0.;
  }

  timeOfNextPulse = 0.5 * fastPeriod;
  while ( true ) {
    binindex = static_cast<unsigned int>(timeOfNextPulse*invbinwidth);
    if (binindex >= nbins) break;
    m_tmpArray[binindex] += CLHEP::RandGaussZiggurat::shoot(rndmEngine, 0., 1.);
    timeOfNextPulse += fastPeriod;
  };

  //### Then (Anatolis) signal shaping:
  for (unsigned int i(0); i < nbins; ++i) {
    unsigned int limit(i+m_noiseSignalShape.size());
    if ( limit > nbins ) limit = nbins;
    for (unsigned int j(i); j < limit; ++j) {
      m_cachedFastNoiseAfterSignalShaping[j] +=
	m_tmpArray[i] * m_noiseSignalShape[j-i] * fractionOfFastNoise;
    };
  };

  //### Slow periodic signal
  for (unsigned int i(0); i < nbins; ++i) {
    m_tmpArray[i] = 0.;
    m_cachedSlowNoiseAfterSignalShaping[i] = 0.;
  }

  //### First we produce the noise as it is BEFORE signal shaping:
  timeOfNextPulse = 0.5 * slowPeriod;
  while (true) {
    binindex = static_cast<unsigned int>(timeOfNextPulse*invbinwidth);
    if (binindex >= nbins) break;
    m_tmpArray[binindex] += CLHEP::RandGaussZiggurat::shoot(rndmEngine, 0., 1.);
    timeOfNextPulse += slowPeriod;
  };

  //### Then (Anatolis) signal shaping:
  for (unsigned int i(0); i < nbins; ++i) {
    unsigned int limit(i+m_noiseSignalShape.size());
    if (limit > nbins) limit = nbins;
    for (unsigned int j(i); j < limit; ++j) {
      m_cachedSlowNoiseAfterSignalShaping[j] +=
	m_tmpArray[i] * m_noiseSignalShape[j-i] *
	m_fractionOfSlowNoise;
    };
  };

}

//_____________________________________________________________________________
void TRTElectronicsNoise::tabulateNoiseSignalShape() {

  m_noiseSignalShape.resize(0);

  const double binwidth(m_settings->timeInterval()/m_settings->numberOfBins());

  //Tabulate:
  double shapemax = 0;
  for ( double time(0.5*binwidth); time < m_settings->timeInterval(); time += binwidth ) {
    m_noiseSignalShape.push_back(this->NoiseShape(time));
    shapemax = std::max(shapemax,std::abs(m_noiseSignalShape.back()));
  }

  //Cut of trailing zeroes:
  unsigned int noiseshapebins(m_noiseSignalShape.size()-1);
  for ( ;noiseshapebins>0;--noiseshapebins) {
    if (m_noiseSignalShape.at(noiseshapebins-1)>0.001*shapemax) break;
  }
  m_noiseSignalShape.resize(noiseshapebins);

  //Normalize:
  for (unsigned int i(0); i < m_noiseSignalShape.size(); ++i) {
    m_noiseSignalShape[i] /= shapemax;
  };

}

//_____________________________________________________________________________
void TRTElectronicsNoise::addElectronicsNoise(std::vector<double>& signal,
					      const double& noiseamplitude,
                                              CLHEP::HepRandomEngine *rndmEngine) {

  // complain if uninitialized? (fixme)

  // We "smear" the 25ns signal (simulating the fact that different
  // straws have different offsets):
  //
  // Effectively, in 64% of the cases we choose the shift randomly in
  // the interval [-1.56ns,3.9ns], and in the remaining 36% we choose
  // the shift randomly in [4.7ns,17.97ns]. This scheme seems to
  // reproduce the features of data well.

  static const unsigned int n_slowperiodic_shift = 26;
  static const int slowperiodic_constshift       = -2;
  static const double slowperiodic_shift_prob_comul[n_slowperiodic_shift] =
    {0.08, 0.16, 0.24, 0.32, 0.40, 0.48, 0.56, 0.64, 0.66, 0.68,
     0.70, 0.72, 0.74, 0.76, 0.78, 0.80, 0.82, 0.84, 0.86, 0.88,
     0.90, 0.92, 0.94, 0.96, 0.98, 1.00};

  //Find array offset for fast signal:
  const unsigned int nsignalbins(signal.size());
  const unsigned int offset_fast(CLHEP::RandFlat::shootInt(rndmEngine, m_cachedFastNoiseAfterSignalShaping.size()-nsignalbins));

  //Find array offset for slow periodic signal:
  int offset_slowperiodic(CLHEP::RandFlat::shootInt(rndmEngine,
                          m_cachedSlowNoiseAfterSignalShaping.size()
                          - nsignalbins-n_slowperiodic_shift
                          - slowperiodic_constshift));

  offset_slowperiodic -= ( offset_slowperiodic % m_nbins_periodic );
  offset_slowperiodic -= slowperiodic_constshift;

  const double rand(CLHEP::RandFlat::shoot(rndmEngine, 0., 1.));
  for (unsigned int i(0); i < n_slowperiodic_shift; ++i) {
    if ( rand < slowperiodic_shift_prob_comul[i] ) {
      offset_slowperiodic -= i;
      break;
    };
  };

  //Fix for rare case when offset becomes negative
  if (offset_slowperiodic<0)
    offset_slowperiodic += (((-offset_slowperiodic)%m_nbins_periodic)+1)*m_nbins_periodic;

  //Add the two components to the signal:
  for ( unsigned int i(0); i<nsignalbins; ++i) {
    signal[i] += noiseamplitude *
      ( m_cachedFastNoiseAfterSignalShaping[offset_fast + i] +
	m_cachedSlowNoiseAfterSignalShaping[offset_slowperiodic + i] );
  };
}

//_____________________________________________________________________________
void TRTElectronicsNoise::InitializeNoiseShaping() {

  // For now we hardcode the noise shape parameters here:
  // (I am not sure they make much sense in the DB in any case - a simple version flag should suffice).
  // According to Anatoli, this shape can be the same for Xe, Kr and Ar.
  m_noisepars1.clear();
  m_noisepars1.push_back(263.1021);//N
  m_noisepars1.push_back(4.611810);//mu
  m_noisepars1.push_back(8.496722);//sigma
  m_noisepars1.push_back(-226.9187);//a
  m_noisepars1.push_back(-15.15887);//+bt
  m_noisepars1.push_back(2.833467);//+ct**2
  m_noisepars1.push_back(-.8981638E-01);//+dt**3
  m_noisepars2.clear();
  m_noisepars2.push_back(-131.7723);//N
  m_noisepars2.push_back(12.94000);//mu
  m_noisepars2.push_back(8.107412);//sigma
  m_noisepars2.push_back(276.7422);//a
  m_noisepars2.push_back(-6.767590);//+bt
  m_noisepars2.push_back(-.3214721);//+ct**2
  m_noisepars2.push_back(.8512509E-02);//+dt**3

}

//___________________________________________________________________
double TRTElectronicsNoise::NoiseShape(const double& time) const {

  //convert to nanoseconds:
  const double time_ns(time/CLHEP::nanosecond);

  if ( time_ns<0 || time_ns > 32 ) return 0;

  double tmp(0);
  for (unsigned int i(6); i>=4; --i) {
    if (time_ns<15.5)
      tmp = (tmp + m_noisepars1[i])*time_ns;
    else
      tmp = (tmp + m_noisepars2[i])*time_ns;
  };

  if (time_ns<15.5)
    tmp += m_noisepars1[0]*exp(-0.5*((time_ns-m_noisepars1[1])/m_noisepars1[2])
			       *((time_ns-m_noisepars1[1])/m_noisepars1[2])) + m_noisepars1[3];
  else
    tmp += m_noisepars2[0]*exp(-0.5*((time_ns-m_noisepars2[1])/m_noisepars2[2])
			       *((time_ns-m_noisepars2[1])/m_noisepars2[2])) + m_noisepars2[3];

  tmp *= 0.001;

  return tmp;

}
