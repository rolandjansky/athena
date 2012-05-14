/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APEvtWeight
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

#ifndef APEvtWeight_h
#define APEvtWeight_h

#include "Rtypes.h"
#include <vector>

class APWeightEntry;

class APEvtWeight {

public:
  APEvtWeight();                                                                 /*!< Default constructor. */
  virtual ~APEvtWeight();                                                        /*!< Default destructor. */
  void AddWeightToEvt(APWeightEntry* weight);                                    /*!< Adds a weight to the sum of weights. */

  double GetWeight();                                                            /*!< Returns the event weight. */
  double GetStdDev();                                                            /*!< Returns the standard deviation. */
  double GetSysUncert();                                                         /*!< Returns the systematic uncertainty (from systematics assigned to weights). */

  unsigned long NEntries();                                                      /*!< Returns the unweighted number of entries. */

  const friend APEvtWeight operator&&(const APEvtWeight& a_in, const APEvtWeight& b_in); /*!< Operator implementing logical AND */
  const friend APEvtWeight operator||(const APEvtWeight& a_in, const APEvtWeight& b_in); /*!< Operator implementing logical OR */
  
protected:
  void Compute();                                                                /*!< Calculates the event weight for the current entries. */
  std::vector< double > _current_evt_weights;                                    /*!< Holds the expectancy values of the weights in the current event. */
  std::vector< double > _current_evt_variances;                                  /*!< Holds the variances of the weights in the current event. */
  std::vector< double > _current_evt_sysuncert2s;                                /*!< Holds the squared systematic uncertainties of the weights in the current event. */
  unsigned long int _n_entries;                                                  /*!< Holds the original amount of unweighted counts ("sum of 1's"). */
  double _k_evt_weight;                                                          /*!< Holds the event weight. */
  double _variance;                                                              /*!< Holds the variance. */
  double _variance_sys;                                                          /*!< Holds the systematic variance (from systematics assigned to weights). */
  bool isComputed;                                                               /*!< Flag if calculation has already been performed for current set of input weights. */

#ifndef SUSY_ATHENA
  ClassDef(APEvtWeight,1)                                                        /*!< ClassDef for ROOTCINT dictionary. */
#endif
};

#endif
