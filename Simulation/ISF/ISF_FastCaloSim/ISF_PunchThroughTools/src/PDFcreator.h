/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PDFcreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PUNCHTHROUGHTOOLS_SRC_PDFCREATOR_H
#define ISF_PUNCHTHROUGHTOOLS_SRC_PDFCREATOR_H

// Athena Base
#include "AthenaKernel/IAtRndmGenSvc.h"

// ROOT includes
#include "TFile.h"
#include "TF1.h"
#include "TH2F.h"

namespace ISF
{
  /** @class PDFcreator

      Creates random numbers with a distribution given as ROOT TF1.
      The TF1 function parameters will be retrieved from a histogram given by addPar.

      @author  Elmar Ritsch <Elmar.Ritsch@cern.ch>
  */

  class PDFcreator
  {

  public:
    /** construct the class with a given TF1 and a random engine */
    PDFcreator(TF1 *pdf, CLHEP::HepRandomEngine *engine):m_randomEngine(engine), m_pdf(pdf), m_randmin(0), m_randmax(0) {} ;

    ~PDFcreator() { };

    /** all following is used to set up the class */
    void addPar(TH1 *hist) { m_par.push_back(hist); };             //!< get the function's parameter from the given histogram
    void setRange( TH1 *min, TH1 *max) { m_randmin = min; m_randmax = max; }; //!< get the function's range from the given histograms

    /** get the random value with this methode, by providing the input parameters */
    double getRand( std::vector<double> inputPar, bool discrete = false, double randMin = 0., double randMax = 0.);

  private:
    CLHEP::HepRandomEngine             *m_randomEngine;       //!< Random Engine
    TF1                                *m_pdf;                //!< the probability density function
    TH1                                *m_randmin;            //!< the fit-functions minimum values
    TH1                                *m_randmax;            //!< the fit-functions maximum values
    std::vector<TH1*>                   m_par;                /*!< the 'histograms' which hold the parameters
                                                                for the above PDF */
  };
}

#endif
