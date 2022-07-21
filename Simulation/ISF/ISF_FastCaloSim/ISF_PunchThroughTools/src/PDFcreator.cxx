/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PDFcreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "PDFcreator.h"

// std
#include <algorithm>

// Random number generators
#include "CLHEP/Random/RandFlat.h"

// ROOT
#include "TFile.h"
#include "TH2F.h"
#include "TAxis.h"
#include "TF1.h"




/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void ISF::PDFcreator::addToEnergyEtaHist1DMap(int energy, int etaMin, TH1 *hist) {

 if(m_energy_eta_hists1D.find(energy) != m_energy_eta_hists1D.end()){ //if energy entry exists, insert into inner eta map
   (m_energy_eta_hists1D.find(energy)->second).insert(std::make_pair(etaMin, hist));
 }
 else{ //if energy entry does not exist create new full energy entry
   std::map< int, TH1*> inner;
   inner.insert(std::make_pair(etaMin, hist));
   m_energy_eta_hists1D.insert(std::make_pair(energy, inner));
 }
}

double ISF::PDFcreator::getRand(const std::vector<int>& inputParameters) const
{

    //define variable to return from getRand call, should never return zero
    float randomHist = 0.;

    if( m_energy_eta_hists1D.find(inputParameters.at(0)) == m_energy_eta_hists1D.end()) {
        //this should never be reached
        return 0.;
    }

    const std::map< int, TH1*>& etaMin_hists = m_energy_eta_hists1D.at(inputParameters.at(0));

    if( etaMin_hists.find(inputParameters.at(1)) == etaMin_hists.end()) {
        //this should never be reached
        return 0.;
    }

    //get the chosen histogram from the map
    TH1*  hist = etaMin_hists.at(inputParameters.at(1));

    //Draw randomly from the histogram CDF distribution.
    double randomShoot = CLHEP::RandFlat::shoot(m_randomEngine);

    //first select the matching CDF bin
    int iBinSelect = 0;
    for(int iBin = 0; iBin < hist->GetNbinsX(); iBin ++){
        iBinSelect = iBin;
        if(hist->GetBinContent(iBin) > randomShoot){
            break;
        }
    }

    //Select random value within bin
    randomHist = CLHEP::RandFlat::shoot(m_randomEngine, hist->GetBinLowEdge(iBinSelect) , hist->GetBinLowEdge(iBinSelect+1));


    return randomHist;

}
