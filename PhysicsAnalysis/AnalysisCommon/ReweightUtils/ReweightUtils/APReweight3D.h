/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APReweight3D
 *
 * Class to store 3D parameterized weight maps
 *
 * Stores a 3D parameterization of APWeightEntries from numerator and
 * denominator histograms or from an efficiency histogram and uncertainties.
 *
 * @author fabian.Kohn@cern.ch
 **/
/////////////////////////////////////////////////////////////////////////////

#ifndef APReweight3D_h
#define APReweight3D_h

#include "APReweightBase.h"
#include <vector>

class TTree;
class TH3;
class TH3D;
class TAxis;
class APWeightEntry;

class APReweight3D : public APReweightBase {

public:
  APReweight3D();                                                                /*!< Default constructor. */
  APReweight3D(TH3* denominator_in, TH3* numerator_in, bool isTrig = false);     /*!< Constructor for histograms (numerator/denominator) to read in. */
  virtual ~APReweight3D();                                                       /*!< Default destructor. */

  void ReadEfficiency(TH3* efficiency_in, TH3* err_low_in, TH3* err_high_in = 0);/*!< Read efficiencies and upper/lower uncertainty (if numerator/denominator not applicable (e.g. Bootstrap)). */

  APWeightEntry* GetBinWeight(unsigned int bin_x, unsigned int bin_y, unsigned int bin_z) const; /*!< Get Weight entry for a given triplet of bin numbers. */
  APWeightEntry* GetWeight(double value_x, double value_y, double value_z) const;/*!< Get Weight entry for a given triplet of values. */
   
  double GetSampleScale() const;                                                 /*!< Get scale factor that was calculated from sample sizes upon instantiation. */
  unsigned int NBins() const;                                                    /*!< Get amount of bins. */
  unsigned int GetBinX(double value_x) const;                                    /*!< Get bin number in X that corresponds to a given value. */
  unsigned int GetBinY(double value_y) const;                                    /*!< Get bin number in Y that corresponds to a given value. */
  unsigned int GetBinZ(double value_z) const;                                    /*!< Get bin number in Z that corresponds to a given value. */
  
  TH3D* GetDenominatorHist() const;                                              /*!< Get original denominator histogram. */
  TH3D* GetNumeratorHist() const;                                                /*!< Get original numerator histogram. */

  void SetSystUncert(double rel_uncert);                                         /*!< Set the global relative (!) systematic uncertainty of all efficiencies/weights. */
  void SetQuietMode(bool isQuiet = true);                                        /*!< Sets the flag to turn off messages. */
  
  ClassDef(APReweight3D,1)

private:
  TH3D* _denominator_hist;                                                       /*!< Holds the original denominator histogram. */
  TH3D* _numerator_hist;                                                         /*!< Holds the original numerator histogram. */
  std::vector< std::vector< std::vector< APWeightEntry* > > > _weights;          /*!< Holds all weight entries. */
  unsigned int _n_bins_x;                                                        /*!< Holds the amount of bins in X. */
  unsigned int _n_bins_y;                                                        /*!< Holds the amount of bins in Y. */
  unsigned int _n_bins_z;                                                        /*!< Holds the amount of bins in Z. */
  TAxis *_axis_x;                                                                /*!< Holds the X axis of the APReweight3D instance (from input histograms). */
  TAxis *_axis_y;                                                                /*!< Holds the Y axis of the APReweight3D instance (from input histograms). */
  TAxis *_axis_z;                                                                /*!< Holds the Z axis of the APReweight3D instance (from input histograms). */
};

#endif
