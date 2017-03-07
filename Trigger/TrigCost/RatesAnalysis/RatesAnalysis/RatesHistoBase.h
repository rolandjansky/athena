#ifndef RATESANALYSIS_RATESHISTOBASE_H
#define RATESANALYSIS_RATESHISTOBASE_H 1

#include "TH1.h"
#include "TString.h"

#include "GaudiKernel/MsgStream.h"

#include <string>
#include <unordered_map>
#include <set>
#include <sstream>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <memory>

/**
* Extrapolation strategy to apply to each emulated trigger.
* @see RatesAnalysisAlg::setTargetLumi
*/
enum ExtrapStrat_t{
  kLINEAR, //!< Scale trigger linearly with luminosity
  kEXPO_MU, //!< Scale trigger linearly in bunches and exponentially in mu. Exponential modifier factor is to be provided.
  kBUNCH_SCALING, //!< Scale trigger linearly but only in the number of bunches
  kMU_SCALING, //!< Scale trigger linearly but only in the change in <mu>
  kNONE //!< Do not scale this trigger for changes in luminosity
};

/**
 * @brief Structure to hold per-event weights for distribution
 */
struct WeightingValuesSummary_t {
  // Event property weights
  double   m_enhancedBiasWeight; //!< A property of the event derived from online enhanced bias prescales
  double   m_eventMu; //!< The actual number of interactions in the event
  double   m_eventLumi; //!< The instantaneous lumi in cm-2s-1
  bool     m_isUnbiased; //!< If the event was taken online with a RD trigger
  uint32_t m_distanceInTrain; //!< How far into the bunch train the event was, in bunch crossings 
  double   m_eventLiveTime; //!< How much wall-time at P1 did this event represent
  // Lumi weights
  double   m_bunchFactor; //!< What weight needs to be applied to extrapolate rates linear in number of bunches
  double   m_muFactor; //!< What weight needs to be applied to extrapolate rates linear in mu
  double   m_linearLumiFactor; //!< What weight needs to be applied to extrapolate rates linear in mu and bunches 
  double   m_expoMuFactor; //!< What weight needs to be applied to extrapolate rates linear in bunches and exponential in mu.
  const double m_noScaling = 1.; //!< Weight for no scaling.
  const std::string print() {
    std::stringstream ss;
    ss << "WeightSummary: wEB:" << m_enhancedBiasWeight << " mu:" << m_eventMu << " lumi:" << m_eventLumi;
    ss << " isUnbiased:" << m_isUnbiased << " distanceInTrain:" << m_distanceInTrain;
    ss << " liveTime:" << m_eventLiveTime;
    ss << " lwBunch:" << m_bunchFactor << " lwMu:" << m_muFactor;
    ss << " lwExpoMu:" << m_expoMuFactor << " lwLin:" << m_linearLumiFactor;
    return ss.str();
  } 
};

/**
 * @brief Basic base class for any common functionality between RatesTrigger and RatesGroup
 * This means that everyone has access to the same histograms
 */
class RatesHistoBase {
 public:

  RatesHistoBase(const std::string& name, const MsgStream& log, const bool doHistograms = true);
  virtual ~RatesHistoBase();

  TH1D* getMuHist() const; //!< @return histogram pointer or nullptr and an error
  TH1D* getTrainHist() const; //!< @return histogram pointer or nullptr and an error
  virtual void normaliseHist(const double ratesDenominator); //!< Normalise to walltime to get rate.
  bool doHistograms() const { return m_doHistograms; } //!< If histogramming was enabled in this rates object

  static bool isZero(double v) { return fabs(v) < 1e-10; } //<! Helper fn

 protected:

  bool m_doHistograms; //!< If histogramming is switched on
  TH1D* m_rateVsMu; //!< Histogram of rate as a fn. of the input event's mu
  TH1D* m_rateVsTrain; //!< Histogram of rate as a fn. of position in bunch train
  static uint32_t m_histoID; //!< Give every histo a unique name using this

  mutable MsgStream m_log; //!< For ATHENA messaging
};

#endif //> !RATESANALYSIS_RATESHISTOBASE_H
