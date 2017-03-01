#ifndef RATESANALYSIS_RATESHISTOBASE_H
#define RATESANALYSIS_RATESHISTOBASE_H 1

#include <string>
#include <unordered_map>
#include <set>
#include <sstream>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <memory>

#include <TH1.h>
#include <TString.h>

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
 * Structure to hold per-event weights for distribution
 */
struct WeightingValuesSummary_t {
  // Event property weights
  double   m_enhancedBiasWeight;
  double   m_eventMu;
  double   m_eventLumi;
  bool     m_isUnbiased;
  uint32_t m_distanceInTrain;
  double   m_eventLiveTime;
  // Lumi weights
  double   m_bunchFactor;
  double   m_muFactor;
  double   m_linearLumiFactor;
  double   m_expoMuFactor;
  const double m_noScaling = 1.;
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
 * Basic base class for any common functionality between RatesTrigger and RatesGroup 
 */
class RatesHistoBase {
 public:

  RatesHistoBase(const std::string& name, const bool doHistograms = true);
  ~RatesHistoBase();

  TH1D* getMuHist() const;
  TH1D* getTrainHist() const;
  virtual void normaliseHist(const double ratesDenominator);
  bool doHistograms() const { return m_doHistograms; }

  static bool isZero(double v) { return fabs(v) < 1e-10; }

 protected:

  bool m_doHistograms;
  TH1D* m_rateVsMu;  
  TH1D* m_rateVsTrain;
  static uint32_t m_histoID;
};

#endif //> !RATESANALYSIS_RATESHISTOBASE_H
