/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APReweightND
 *
 * Class to store n-dimensionally parameterized weight maps
 *
 * Stores a n-dimensional parameterization of APWeightEntries from numerator
 * and denominator histograms or from an efficiency histogram and uncertainties.
 *
 * @author fabian.Kohn@cern.ch
 **/
/////////////////////////////////////////////////////////////////////////////

#ifndef APReweightND_h
#define APReweightND_h

#include "APReweightBase.h"
#include <vector>
#include <map>

class TTree;
class THnSparse;
class TAxis;
class APWeightEntry;

class APReweightND : public APReweightBase {

public:
  APReweightND();                                                                /*!< Default constructor. */
  APReweightND(THnSparse* denominator_in, THnSparse* numerator_in, bool isTrig = false); /*!< Constructor for histograms (numerator/denominator) to read in. */
  virtual ~APReweightND();                                                       /*!< Default destructor. */

  void ReadEfficiency(THnSparse* efficiency_in, THnSparse* err_low_in, THnSparse* err_high_in = 0); /*!< Read efficiencies and upper/lower uncertainty (if numerator/denominator not applicable (e.g. Bootstrap)). */

  APWeightEntry* GetBinWeight(const int bin[]);                                  /*!< Get Weight entry for a given n-tuple of bin numbers. */
  APWeightEntry* GetWeight(double value[]);                                      /*!< Get Weight entry for a given n-tuple of values. */
   
  double GetSampleScale() const;                                                 /*!< Get scale factor that was calculated from sample sizes upon instantiation. */
  unsigned int NBins() const;                                                    /*!< Get amount of bins. */
  
  THnSparse* GetDenominatorHist() const;                                         /*!< Get original denominator histogram. */
  THnSparse* GetNumeratorHist() const;                                           /*!< Get original numerator histogram. */

  void SetSystUncert(double rel_uncert);                                         /*!< Set the global relative (!) systematic uncertainty of all efficiencies/weights. */
  void SetQuietMode(bool isQuiet = true);                                        /*!< Sets the flag to turn off messages. */
 
  ClassDef(APReweightND, 1)

private:
  unsigned int _n_dim;                                                           /*!< Holds the amount of dimensions. */
  THnSparse* _denominator_hist;                                                  /*!< Holds the original denominator histogram. */
  THnSparse* _numerator_hist;                                                    /*!< Holds the original numerator histogram. */
  std::map< std::vector<int> , APWeightEntry* > _weights;                        /*!< Holds all weight entries. */
  std::vector< TAxis* > _axes;                                                   /*!< Holds all axes of the APReweightND instance (from input histograms). */
};

#endif
