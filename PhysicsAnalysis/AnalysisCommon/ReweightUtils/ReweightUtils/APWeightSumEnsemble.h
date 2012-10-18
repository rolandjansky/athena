/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APWeightSumEnsemble
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

#ifndef APWeightSumEnsemble_h
#define APWeightSumEnsemble_h

#include "APWeightSum.h"
#include <vector>

class TH1F;
class TVirtualFitter;
class APWeightEntry;

class APWeightSumEnsemble : public APWeightSum {
  
public:
  APWeightSumEnsemble();                                                         /*!< Default constructor. */
  virtual ~APWeightSumEnsemble();                                                /*!< Default destructor. */
  void SetRelPrecision(double rel_prec);                                         /*!< Sets the relative precision of the gaussian fit to stop the ensemble test at. */
  void AddWeightToEvt(APWeightEntry* weight);                                    /*!< Adds a weight to the sum of weights. */
  void FinishEvt(double ext_weight = 1.0);                                       /*!< Finishes the current event and calculates the event weight. Takes an optional external weight factor (e.g. mc weight). */

  double GetQuantile(const double prob);                                         /*!< Returns the quantlile for p=prob for the distribution. (Invokes Ensemble Test) */
  double GetRandom();                                                            /*!< Returns a random value according to the pdf. (Invokes Ensemble Test) */

  double GetEnsemblePDFMode();                                                   /*!< Returns the Mode (= sum of weights from PDF). (Invokes Ensemble Test) */
  double GetEnsemblePDFStdDev();                                                 /*!< Returns the standard deviation from PDF . (Invokes Ensemble Test) */

  TH1F *GetPDF();                                                                /*!< Returns the calculated PDF. (Invokes Ensemble Test) */
 
  ClassDef(APWeightSumEnsemble,1)

private:
  void Compute();                                                                /*!< Performs the ensemble test to model final PDF. */
  std::vector< std::vector< APWeightEntry* > > _weight_vector;                   /*!< Holds the weight objects (dim2) for each event (dim1). */
  std::vector< APWeightEntry* > _current_evt_pdfs;                               /*!< Holds the weight objects for the current event. */
  std::vector< double > _ext_weights;                                            /*!< Holds the external event weight provided when finishing the respective events. */
  double _rel_prec;                                                              /*!< Holds the relative precision of the gaussian fit to stop the ensemble test at. */
  TH1F *_pdf;                                                                    /*!< Holds the TH1F instance from the arrays if computed. */
  double _ensemble_mode;                                                         /*!< Holds the Mode (= sum of weights from PDF) from ensemble test. */
  double _ensemble_sigma;                                                        /*!< Holds the standard deviation from ensemble test. */
  bool _ensembleTest_done;                                                       /*!< Flag if the ensemble test has been performed with the current set of weights. */
};

#endif
