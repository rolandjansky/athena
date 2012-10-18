/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APWeightHist
 *
 * Extended histogramming class.
 *
 * Extended histogramming class (inheriting from TH1D) which allows
 * propagation of uncertainties on the actual weights used for filling.
 *
 * @author fabian.Kohn@cern.ch
 **/
/////////////////////////////////////////////////////////////////////////////

#ifndef APWeightHist_h
#define APWeightHist_h
#include "TH1D.h"
#include <string>

class APWeightEntry;
class TGraphErrors;
class TGraphAsymmErrors;

class APWeightHist : public TH1D {

public:
  APWeightHist(const char *name,const char *title, const int n_bins, const float x_min, const float x_max); /*!< Constructor which takes histo title, amount of bins and the range and optionally the preicision to use for the calculation of the uncertainty. */
  APWeightHist();                                                                /*!< Default constructor. */
  ~APWeightHist();                                                               /*!< Default destructor. */
  using TH1D::Fill;                                                              /*!< Overloads TH1D's Fill method. */
  int Fill(const double value, APWeightEntry* weight);                           /*!< Adds a weighted value to the calculation. */
  TGraphAsymmErrors* GetGraphStatUncert(bool autocompute = true);                /*!< Extracts the histogram with statistical uncertainties. */
  TGraphErrors* GetGraphSystUncert(bool simple = true, bool autocompute = true); /*!< Extracts the histogram with systematic uncertainties. */
  void ComputeGraph(const int prec = 250);                                       /*!< Computes the resulting graph from all added ntuples and calculates the uncertainties for all bins. */
  TH1D* GetBinPDF(unsigned int bin, bool autocompute = true);                    /* Retrieves the PDF of a bin (starting with bin=1 in accordance with the TH1 classes, over- & underflow bins not accessible) */
  
  ClassDef(APWeightHist,1);                                                      /*!< ClassDef for ROOTCINT dictionary. */

private:
  double _computed_entries;                                                      /*!< Flag to store information about the status of the computation. */
  std::vector< std::vector< APWeightEntry* > > _binned_weights;                  /*!< Holds all filled weights weights as pointers. */
  std::vector< TH1D* > _bin_dists;                                               /*!< Holds the PDFs for the individual bins. */
  std::vector< double > fSumSys2;                                                /*!< Holds the variances of systematic uncertainties for the individual bins. */
  TGraphAsymmErrors* _graph_stat;                                                /*!< Holds the histogram with statistical uncertainties. */
  TGraphErrors* _graph_syst;                                                     /*!< Holds the histogram with systematic uncertainties. */
  int _prec;                                                                     /*!< Holds the calculation precision (= amount of histograms & binning factor). */

};

#endif
