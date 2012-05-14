/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef APWeightEntry_h
#define APWeightEntry_h

#include "TObject.h"

class TH1F;

class APWeightEntry : public TObject {

public:
  APWeightEntry();                                                               /*!< Default constructor. */
  APWeightEntry(unsigned int val_denominator, unsigned int val_numerator, double scale, bool isTrig = false); /*!< Constructor, takes n_denom, n_num, the relative scale of the used samples or an additional user scale and a bool if the entry is a trigger weight. */
  virtual ~APWeightEntry();                                                      /*!< Default destructor. */

  void ReadEfficiency(double efficiency, double err_low, double err_high);       /*!< Read efficiencies and upper/lower uncertainty (if numerator/denominator not applicable (e.g. Bootstrap)). */

  unsigned int GetValDenominator();                                              /*!< Get value of original denominator. */
  unsigned int GetValNumerator();                                                /*!< Get value of original numerator. */
  double GetExpectancy();                                                        /*!< Get Expectancy value of efficiency/weight. */
  double GetVariance();                                                          /*!< Get Variance of efficiency/weight (classical binomial/poisson model). */
  double GetStatUncertLow();                                                     /*!< Get lower bound of asymmetric statistical uncertainty. */
  double GetStatUncertHigh();                                                    /*!< Get upper bound of asymmetric statistical uncertainty. */
  double GetSysUncert();                                                         /*!< Get absolute systematic uncertainty value of efficiency/weight. */
  double GetSysUncert2();                                                        /*!< Get absolute systematic uncertainty squared value of efficiency/weight. */
  double GetRandom();                                                            /*!< Get random number from PDF. */
  TH1F* GetPDF();                                                                /*!< Returns the calculated PDF. */

  void SetSystUncert(double rel_uncert);                                         /*!< Set the relative (!) systematic uncertainty for the efficiency/weight. */
    
private:
  void _CreateHist();                                                            /*!< Creates a TH1F instance from the arrays if necessary. */
  void _ComputeCum();                                                            /*!< Calculates the cumulative function of the pdf if necessary. */

  unsigned int _val_denominator;                                                 /*!< Holds the value of original denominator. */
  unsigned int _val_numerator;                                                   /*!< Holds the value of original numerator. */
  double _expectancy_val;                                                        /*!< Holds the Expectancy value of efficiency/weight. */
  double _variance;                                                              /*!< Holds Variance of efficiency/weight (classical binomial/poisson model). */
  double _stat_uncert_low;                                                       /*!< Holds lower bound of asymmetric statistical uncertainty. */
  double _stat_uncert_high;                                                      /*!< Holds upper bound of asymmetric statistical uncertainty. */
  double _sys_uncert;                                                            /*!< Holds absolute systematic uncertainty value of efficiency/weight. */
  double _sys_uncert2;                                                           /*!< Holds absolute systematic uncertainty squared value of efficiency/weight. */

  bool _is_nan;                                                                  /*!< Flag, set to true if denominator is zero. */
  double _integral;                                                              /*!< Holds the integral of the probability distribution. */
  double *_pdf, *_bins, *_cumul;                                                 /*!< Histograms to hold the probability distribution and the cumulative distribution. */
  TH1F *_hist;                                                                   /*!< Holds the TH1F instance from the arrays if computed. */
#ifndef SUSY_ATHENA
  ClassDef(APWeightEntry,1)                                                      /*!< ClassDef for ROOTCINT dictionary. */
#endif
};

#endif
