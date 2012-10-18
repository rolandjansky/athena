/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APReweight2D
 *
 * Class to store 2D parameterized weight maps
 *
 * Stores a 2D parameterization of APWeightEntries from numerator and
 * denominator histograms or from an efficiency histogram and uncertainties.
 *
 * @author fabian.Kohn@cern.ch
 **/
/////////////////////////////////////////////////////////////////////////////

#ifndef APReweight2D_h
#define APReweight2D_h

#include "APReweightBase.h"
#include <vector>

class TTree;
class TH2;
class TH2D;
class TAxis;
class APWeightEntry;

class APReweight2D : public APReweightBase {

public:
  APReweight2D();                                                                /*!< Default constructor. */
  APReweight2D(TH2* denominator_in, TH2* numerator_in, bool isTrig = false);     /*!< Constructor for histograms (numerator/denominator) to read in. */
  virtual ~APReweight2D();							 /*!< Default destructor. */

  void ReadEfficiency(TH2* efficiency_in, TH2* err_low_in, TH2* err_high_in = 0);/*!< Read efficiencies and upper/lower uncertainty (if numerator/denominator not applicable (e.g. Bootstrap)). */
  
  APWeightEntry* GetBinWeight(unsigned int bin_x, unsigned int bin_y) const;     /*!< Get Weight entry for a given pair of bin numbers. */
  APWeightEntry* GetWeight(double value_x, double value_y) const;                /*!< Get Weight entry for a given pair of values. */
   
  double GetSampleScale() const;                                                 /*!< Get scale factor that was calculated from sample sizes upon instantiation. */
  unsigned int NBins() const;                                                    /*!< Get amount of bins. */
  unsigned int GetBinX(double value_x) const;                                    /*!< Get bin number in X that corresponds to a given value. */
  unsigned int GetBinY(double value_y) const;                                    /*!< Get bin number in Y that corresponds to a given value. */
  
  TH2D* GetDenominatorHist() const;                                              /*!< Get original denominator histogram. */
  TH2D* GetNumeratorHist() const;                                                /*!< Get original numerator histogram. */

  void SetSystUncert(double rel_uncert);                                         /*!< Set the global relative (!) systematic uncertainty of all efficiencies/weights. */
  void SetQuietMode(bool isQuiet = true);                                        /*!< Sets the flag to turn off messages. */
  
  ClassDef(APReweight2D,1)

private:
  TH2D* _denominator_hist;                                                       /*!< Holds the original denominator histogram. */
  TH2D* _numerator_hist;                                                         /*!< Holds the original numerator histogram. */
  std::vector< std::vector< APWeightEntry* > > _weights;                         /*!< Holds all weight entries. */
  unsigned int _n_bins_x;                                                        /*!< Holds the amount of bins in X. */
  unsigned int _n_bins_y;                                                        /*!< Holds the amount of bins in Y. */
  TAxis *_axis_x;                                                                /*!< Holds the X axis of the APReweight2D instance (from input histograms). */
  TAxis *_axis_y;                                                                /*!< Holds the Y axis of the APReweight2D instance (from input histograms). */
};

#endif
