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
    void setName( std::string PDFname ){ m_name = PDFname; }; //get the pdf's name
    void addToSliceHistMap(std::vector<double> energyEtaMinEtaMax, TH1 *hist) { m_energyEtaMinEtaMax_hists.insert( {energyEtaMinEtaMax, hist} ); }; 
    void addToEnergyEtaRangeHistMap(double energy, std::vector<double> etaMinEtaMax, TH1 *hist) { std::map<std::vector<double>, TH1*> inner; inner.insert(std::make_pair(etaMinEtaMax, hist)); m_energy_etaRange_hists.insert(std::make_pair(energy, inner)); }; //add entry to map linking energy, eta window and histogram

    /** get the random value with this methode, by providing the input parameters */
    double getRand( std::vector<double> inputPar, bool discrete = false, double randMin = 0., double randMax = 0.);
    std::string getName(){return m_name;};
    static bool maxEnergyCompare(const std::pair<std::vector<double>, TH1*>& p1, const std::pair<std::vector<double>, TH1*>& p2){ return p1.first.at(0) < p2.first.at(0); };
    static bool compareEnergy(std::pair< double , std::map< std::vector<double>, TH1*> > map, double value){ return map.first < value; };
    static bool compareEtaMax(std::pair< std::vector<double>, TH1*> map, double value){ return map.first.at(1) < value; };

  private:
    CLHEP::HepRandomEngine             *m_randomEngine;       //!< Random Engine
    TF1                                *m_pdf;                //!< the probability density function
    TH1                                *m_randmin;            //!< the fit-functions minimum values
    TH1                                *m_randmax;            //!< the fit-functions maximum values
    std::vector<TH1*>                   m_par;                /*!< the 'histograms' which hold the parameters
                                                                for the above PDF */
    std::string                         m_name;               //!< Give pdf a name for debug purposes 
    std::map< std::vector<double>, TH1*>          m_energyEtaMinEtaMax_hists; //!< map to store eta and energy slice values linked with corresponding histogram dists.
    std::map< double , std::map< std::vector<double>, TH1*> > m_energy_etaRange_hists; //!< map of energies to map of eta ranges to histograms


  };
}

#endif
