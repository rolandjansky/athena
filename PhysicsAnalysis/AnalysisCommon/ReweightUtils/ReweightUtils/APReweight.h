/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APReweight
 *
 * Class to store 1D parameterized weight maps
 *
 * Stores a 1D parameterization of APWeightEntries from numerator and
 * denominator histograms or from an efficiency histogram and uncertainties.
 *
 * @author fabian.Kohn@cern.ch
 **/
/////////////////////////////////////////////////////////////////////////////

#ifndef APReweight_h
#define APReweight_h

#include "APReweightBase.h"
#include <string>
#include <vector>

class TTree;
class TH1;
class TH1D;
class TAxis;
class APWeightEntry;

class APReweight : public APReweightBase {

public:
  APReweight();                                                                  /*!< Default constructor. */
  APReweight(TTree* denominator, std::string denominator_branch, TTree* numerator, std::string numerator_branch, unsigned int n_bins, double x_min, double x_max, bool isTrig = false); /*!< Constructor for whole branches (numerator/denominator) to read in. */
  APReweight(std::vector< double > denominator, std::vector< double > numerator, unsigned int n_bins, double x_min, double x_max, bool isTrig = false); /*!< Constructor for vectors (numerator/denominator) to read in. */
  APReweight(TH1* denominator_in, TH1* numerator_in, bool isTrig = false);       /*!< Constructor for histograms (numerator/denominator) to read in. */
  virtual ~APReweight();							 /*!< Default destructor. */

  void ReadEfficiency(TH1* efficiency_in, TH1* err_low_in, TH1* err_high_in = 0);/*!< Read efficiencies and upper/lower uncertainty (if numerator/denominator not applicable (e.g. Bootstrap)). */
  
  APWeightEntry* GetBinWeight(unsigned int bin) const;                           /*!< Get Weight entry for a given bin number. */
  APWeightEntry* GetWeight(double value) const;                                  /*!< Get Weight entry for a given value. */
   
  double GetSampleScale() const;                                                 /*!< Get scale factor that was calculated from sample sizes upon instantiation. */
  unsigned int NBins() const;                                                    /*!< Get amount of bins. */
  unsigned int GetBin(double value) const;                                       /*!< Get bin number that corresponds to a given value. */
  
  TH1D* GetDenominatorHist() const;                                              /*!< Get original denominator histogram. */
  TH1D* GetNumeratorHist() const;                                                /*!< Get original numerator histogram. */
  
  void SetSystUncert(double rel_uncert);                                         /*!< Set the global relative (!) systematic uncertainty of all efficiencies/weights. */
  void SetQuietMode(bool isQuiet = true);                                        /*!< Sets the flag to turn off messages. */

  ClassDef(APReweight, 1)

private:
  TH1D* _denominator_hist;                                                       /*!< Holds the original denominator histogram. */
  TH1D* _numerator_hist;                                                         /*!< Holds the original numerator histogram. */
  std::vector< APWeightEntry* > _weights;                                        /*!< Holds all weight entries. */
  unsigned int _n_bins;                                                          /*!< Holds the amount of bins. */
  TAxis *_axis;                                                                  /*!< Holds the axis of the APReweight instance (from input histograms). */
};

#endif
