/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APWeightEntry
 *
 * Class to store a single weight entry (one bin).
 *
 * Stores a single binned weight, corresponding to a pair of bins from the 
 * underlying numerator and denominator histograms. Allows access to quantities
 * like central values, uncertainties and PDFs.
 *
 * @author fabian.Kohn@cern.ch
 **/
/////////////////////////////////////////////////////////////////////////////

#ifndef APWeightEntry_h
#define APWeightEntry_h

#include "Rtypes.h"

class TH1F;

class APWeightEntry {

public:
  APWeightEntry();                                                               /*!< Default constructor. */
  APWeightEntry(unsigned int val_denominator, unsigned int val_numerator, double scale, bool isTrig = false); /*!< Constructor, takes n_denom, n_num, the relative scale of the used samples or an additional user scale and a bool if the entry is a trigger weight. */
  virtual ~APWeightEntry();                                                      /*!< Default destructor. */

  void ReadEfficiency(double efficiency, double err_low, double err_high);       /*!< Read efficiencies and upper/lower uncertainty (if numerator/denominator not applicable (e.g. Bootstrap)). */
  void SetCoordinates(std::vector<int> coords, std::vector<int> n_dim_origin);
  
  unsigned int GetValDenominator() const;                                        /*!< Get value of original denominator. */
  unsigned int GetValNumerator() const;                                          /*!< Get value of original numerator. */
  double GetExpectancy() const;                                                  /*!< Get Expectancy value of efficiency/weight. */
  double GetVariance() const;                                                    /*!< Get Variance of efficiency/weight (classical binomial/poisson model). */
  double GetStatUncertLow() const;                                               /*!< Get lower bound of asymmetric statistical uncertainty. */
  double GetStatUncertHigh() const;                                              /*!< Get upper bound of asymmetric statistical uncertainty. */
  double GetSysUncert() const;                                                   /*!< Get absolute systematic uncertainty value of efficiency/weight. */
  double GetSysUncert2() const;                                                  /*!< Get absolute systematic uncertainty squared value of efficiency/weight. */
  double GetRandom();                                                            /*!< Get random number from PDF. */
  TH1F* GetPDF();                                                                /*!< Returns the calculated PDF. */
  unsigned int GetID() const;                                                    /*!< Returns the internal ID (used by APReweight/APReweight2D/APReweight3D/APReweightND). */
  
  std::vector< int > GetCoords() const;                                          /*!< Returns the coordinates of the current entry in the original histogram. */
  std::vector< int > GetOriginalDimensions() const;                              /*!< Returns the dimensions and amounts of bins for each dimension of the original histogram. */
  
  bool IsNaN() const;                                                            /*!< Returns true if instance is NaN. */
  bool IsTrig() const;                                                           /*!< Returns true if instance is trigger based. */
  
  void SetSystUncert(double rel_uncert);                                         /*!< Set the relative (!) systematic uncertainty for the efficiency/weight. */
  void SetID(unsigned int id);                                                   /*!< Set the internal ID (used by APReweight/APReweight2D/APReweight3D/APReweightND). */

private:
  APWeightEntry (const APWeightEntry&);
  APWeightEntry& operator= (const APWeightEntry&);

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

  bool _is_trig;                                                                 /*!< Flag, set to true if weight entry is trigger based. */
  bool _is_nan;                                                                  /*!< Flag, set to true if denominator is zero. */
  unsigned int _ID;                                                              /*!< Holds internal ID (used by APReweight/APReweight2D/APReweight3D/APReweightND). */
  double _integral;                                                              /*!< Holds the integral of the probability distribution. */
  double *_pdf, *_bins, *_cumul;                                                 /*!< Histograms to hold the probability distribution and the cumulative distribution. */
  TH1F *_hist;                                                                   /*!< Holds the TH1F instance from the arrays if computed. */
  std::vector<int> _coords;                                                      /*!< Holds the coordinates of the current entry in the original histogram. */
  std::vector< int > _n_dim_origin;                                              /*!< Holds the amount of dimensions and bins per axis in the original histogram. */
};

#endif
