/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APWeightSum
 *
 * Class to calculate the sum of weights ("weighted counter")
 *
 * Calculates the sum of weights taking into account the
 * underlying asymmetric probability distribution. This is done by
 * modelling the pdf and then extracting the corresponding quantiles.
 *
 * @author fabian.Kohn@cern.ch
 **/
/////////////////////////////////////////////////////////////////////////////

#ifndef APWeightSum_h
#define APWeightSum_h

#include "TNamed.h"
#include "APEvtWeight.h"
#include <vector>

class APWeightEntry;
class APReweightBase;
class THnSparse;

class APWeightSum : public TNamed {
  
public:
  APWeightSum();                                                                 /*!< Default constructor. */
  virtual ~APWeightSum();                                                        /*!< Default destructor. */
  void AddWeightToEvt(APWeightEntry* weight);                                    /*!< Adds a weight to the sum of weights. */
  void FinishEvt(double ext_weight = 1.0);                                       /*!< Finishes the current event and calculates the event weight. Takes an optional external weight factor (e.g. mc weight). */
  void AddEvt(APEvtWeight* evt_weight, double ext_weight = 1.0);                 /*!< Adds an event with an externally calculated EvtWeight object. Takes an optional external weight factor (e.g. mc weight). */
  
  double GetSumW() const;                                                        /*!< Returns the sum of weights. */
  double GetSumW2() const;                                                       /*!< Returns sum of (weights^2). */
  double GetSumWExternal() const;                                                /*!< Returns the sum of weights without taking into account the trigger weighting (external weights only) to allow switching trigger weighting on/off. */
  double GetStdDev();                                                            /*!< Returns the standard deviation. */
  double GetVariance();                                                          /*!< Returns the variance. */
  double GetVarianceNoCorr();                                                    /*!< Returns the variance, assuming no correlations. */
  double GetVarianceFullCorr();                                                  /*!< Returns the variance, assuming full correlation amongst objects. */
  double GetSysUncert() const;                                                   /*!< Returns the systematic uncertainty (from systematics assigned to weights). */

  unsigned long GetKUnweighted() const;                                          /*!< Returns the unweighted sum of entries. */

  THnSparse* GetUncertHistogram( APReweightBase* weighter );                     /*!< Returns THnSparse holding the uncertainties for given APReweightBase instance. */
  std::vector<THnSparse*> GetAllUncertHistograms();                              /*!< Returns vector of THnSparses holding the uncertainties for all APReweight IDs. */

  ClassDef(APWeightSum,1)

protected:
  void Compute();                                                                /*!< Calculates the final uncertainties including correlations. */
  
  std::vector< APWeightEntry* > _current_evt_weights;                            /*!< Holds the expectancy values of the weights in the current event. */
  std::vector< THnSparse* > _linear_uncert;                                      /*!< Holds all histograms for uncertainties. */
  unsigned long int _k_evt_orig;                                                 /*!< Holds the original amount of unweighted counts ("sum of 1's"). */
  double _k_evt_weight;                                                          /*!< Holds the sum of weights. */
  double _k_evt_weight2;                                                         /*!< Holds the sum of squared weights. */
  double _k_evt_weight_external;                                                 /*!< Holds the sum of external weights (no trigger weighting). */
  double _variance;                                                              /*!< Holds the variance. */
  double _variance_nocorr;                                                       /*!< Holds the variance, assuming no correlations. */
  double _variance_fullcorr;                                                     /*!< Holds the variance, assuming full correlation amongst objects. */
  double _variance_sys;                                                          /*!< Holds the systematic variance (from systematics assigned to weights). */
  bool _isComputed;
};

#endif
