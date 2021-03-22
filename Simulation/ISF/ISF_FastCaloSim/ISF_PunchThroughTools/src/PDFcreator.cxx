/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
void ISF::PDFcreator::addToEnergyEtaRangeHist1DMap(double energy, std::vector<double> etaMinEtaMax, TFCS1DFunction *hist) {

 if(m_energy_etaRange_hists1D.find(energy) != m_energy_etaRange_hists1D.end()){ //if energy entry exists, insert into inner eta map
   (m_energy_etaRange_hists1D.find(energy)->second).insert(std::make_pair(etaMinEtaMax, hist));
 }
 else{ //if energy entry does not exist create new full energy entry
   std::map< std::vector<double>, TFCS1DFunction*> inner;
   inner.insert(std::make_pair(etaMinEtaMax, hist));
   m_energy_etaRange_hists1D.insert(std::make_pair(energy, inner));
 }
}

void ISF::PDFcreator::addToEnergyEtaRangeHist2DMap(double energy, std::vector<double> etaMinEtaMax, std::map< double , TFCS1DFunction* >  *hist){
  if(m_energy_etaRange_hists2D.find(energy) != m_energy_etaRange_hists2D.end()){ //if energy entry exists, insert into inner eta map
    (m_energy_etaRange_hists2D.find(energy)->second).insert(std::make_pair(etaMinEtaMax, hist));
  }
  else{ //if energy entry does not exist create new full energy entry
    std::map< std::vector<double>, std::map< double , TFCS1DFunction* > *> inner;
    inner.insert(std::make_pair(etaMinEtaMax, hist));
    m_energy_etaRange_hists2D.insert(std::make_pair(energy, inner));
  }
}

double ISF::PDFcreator::getRand(const std::vector<double>& inputParameters, const double& outEnergy, const double& randMin, const double& randMax) const
{

    //define variable to return from getRand call, should never return zero
    float random = 0.;

    //Implementation for 1D hist
    if(!m_energy_etaRange_hists1D.empty()){
    //Select energy values neighbouring input energy
    std::map< double , std::map< std::vector<double>, TFCS1DFunction*> >::const_iterator itUpperEnergy, itPrevEnergy, selectedEnergy, secondSelectedEnergy;

    //selects first energy that is not less than input energy
    itUpperEnergy = std::lower_bound(m_energy_etaRange_hists1D.begin(), std::prev(m_energy_etaRange_hists1D.end()), inputParameters.at(0), compareEnergy1D);


    //if we have more than one energy parameterisation then run interpolation code
    if(m_energy_etaRange_hists1D.size() > 1){

      //if closest energy is largest entry in pdf select that
      if (itUpperEnergy == std::prev(m_energy_etaRange_hists1D.end())) {
        selectedEnergy = std::prev(m_energy_etaRange_hists1D.end());
      }
      //if closest energy is smallest entry in pdf select that
      else if (itUpperEnergy == m_energy_etaRange_hists1D.begin()) {
        selectedEnergy = m_energy_etaRange_hists1D.begin();
      }
      else {
        //Check if iterator input energy is closer to previous energy, if yes choose this instead
        itPrevEnergy = std::prev(itUpperEnergy);
        if (std::fabs(inputParameters.at(0) - itPrevEnergy->first) < std::fabs(itUpperEnergy->first - inputParameters.at(0))){
          selectedEnergy = itPrevEnergy;
          secondSelectedEnergy = itUpperEnergy;
        }
        else{
          selectedEnergy = itUpperEnergy;
          secondSelectedEnergy = itPrevEnergy;
        }
      }

      //next interpolate between energy values
      //only interpolate if energy is not greater than max in param, and not lower than min in param
      if( selectedEnergy->first < std::prev(m_energy_etaRange_hists1D.end())->first && selectedEnergy != m_energy_etaRange_hists1D.begin() ){

        //select the smaller of the two energies to find the bin edge between them (energy bands are logarithmic)
        double energyBinEdge;
        if(selectedEnergy->first < secondSelectedEnergy->first){
          energyBinEdge = selectedEnergy->first*sqrtOf2;
        }
        else{
          energyBinEdge = secondSelectedEnergy->first*sqrtOf2;
        }

        //calculate a distance of the input energy to the bin edge
        double distance = std::fabs(energyBinEdge - inputParameters.at(0))/std::fabs(selectedEnergy->first - energyBinEdge);

        //if we get a random number larger than the distance then choose other energy.
        double rand = CLHEP::RandFlat::shoot(m_randomEngine);
        if(rand > distance){
          selectedEnergy = secondSelectedEnergy;
        }

      }
    }
    else{
      selectedEnergy = itUpperEnergy;
    }


    //Now move on to selecting the correct eta window
    //first get the map of eta windows to hists.

    const std::map< std::vector<double>, TFCS1DFunction*>& etaMinEtaMax_hists = selectedEnergy->second;

    std::map< std::vector<double>, TFCS1DFunction*>::const_iterator itSelectedEtaWindow, itSecondEtaWindow;

    //choose first max eta that is not less than input eta
    itSelectedEtaWindow = std::lower_bound(etaMinEtaMax_hists.begin(), std::prev(etaMinEtaMax_hists.end()), inputParameters.at(1), compareEtaMax1D);

    //if we have multiple eta params do interpolation
    if(m_energy_etaRange_hists1D.size() > 1){

    //if input eta is closer to upper eta boundary in itSelecteEtaWindow then select next window as second best choice
    if(std::fabs(inputParameters.at(1) - itSelectedEtaWindow->first.at(1)) <  std::fabs(inputParameters.at(1) - itSelectedEtaWindow->first.at(0))){
      if(itSelectedEtaWindow != std::prev(etaMinEtaMax_hists.end())){
        itSecondEtaWindow = std::next(itSelectedEtaWindow);
      }
      else{
        itSecondEtaWindow = std::prev(itSelectedEtaWindow);
      }
    }
    //if input eta is closer to min boundary of itSelectedEtaWindow select prev window as second best choice unless first entry
    else if(itSelectedEtaWindow != etaMinEtaMax_hists.begin()){
      itSecondEtaWindow = std::prev(itSelectedEtaWindow);
    }
    else{
      itSecondEtaWindow = std::next(itSelectedEtaWindow);
    }
    //find the boundary between the two selected eta windows
    double etaBinEdge;
    if(itSelectedEtaWindow->first.at(0) > itSecondEtaWindow->first.at(0)){
      etaBinEdge = itSelectedEtaWindow->first.at(0);
    }
    else{
      etaBinEdge = itSecondEtaWindow->first.at(0);
    }
    //calculate a distance of the input eta to the bin edge
    double distance = std::fabs(etaBinEdge - inputParameters.at(1))/std::fabs(itSelectedEtaWindow->first.at(0)  - itSelectedEtaWindow->first.at(1));
    //if we get a random number larger than the distance then choose other energy.
    double rand = CLHEP::RandFlat::shoot(m_randomEngine);
    if(rand > distance){
      itSelectedEtaWindow = itSecondEtaWindow;
    }
  }


    //get the chosen histogram from the map
    TFCS1DFunction* hist = itSelectedEtaWindow->second;
    //Draw randomly from the histogram distribution.
    //if obj is 1D histogram just draw randomly from it
    random = hist->rnd_to_fct(CLHEP::RandFlat::shoot(m_randomEngine));
    if(randMax != 0.){
      while (random < randMin || random > randMax){
      random = hist->rnd_to_fct(CLHEP::RandFlat::shoot(m_randomEngine));
      }
    }
    else{
      while (random < randMin){
        random = hist->rnd_to_fct(CLHEP::RandFlat::shoot(m_randomEngine));
      }
    }


    }


    //Implementation for 2D hist
    if(!m_energy_etaRange_hists2D.empty()){
    //Select energy values neighbouring input energy
    std::map< const double , std::map< std::vector<double>, std::map< double , TFCS1DFunction* >* > >::const_iterator itUpperEnergy, itPrevEnergy, selectedEnergy, secondSelectedEnergy;

    //selects first energy that is not less than input energy
    itUpperEnergy = std::lower_bound(m_energy_etaRange_hists2D.begin(), std::prev(m_energy_etaRange_hists2D.end()), inputParameters.at(0), compareEnergy2D);


    //if we have more than one energy parameterisation then run interpolation code
    if(m_energy_etaRange_hists2D.size() > 1){
      if (itUpperEnergy == std::prev(m_energy_etaRange_hists2D.end())) {
        //select final iterator in map
        selectedEnergy = std::prev(m_energy_etaRange_hists2D.end());
      }
      else if (itUpperEnergy == m_energy_etaRange_hists2D.begin()) {
        //choose first iterator in map
        selectedEnergy = m_energy_etaRange_hists2D.begin();
        //do some sort of interpolation in this case to zero with log energy
      } else {
        //Check if iterator input energy is closer to previous iterator energy, if yes choose this instead
        itPrevEnergy = std::prev(itUpperEnergy);
        if (std::fabs(inputParameters.at(0) - itPrevEnergy->first) < std::fabs(itUpperEnergy->first - inputParameters.at(0))){
          selectedEnergy = itPrevEnergy;
          secondSelectedEnergy = itUpperEnergy;
        }
        else{
          selectedEnergy = itUpperEnergy;
          secondSelectedEnergy = itPrevEnergy;
        }
      }

      //next interpolate between energy values
      //only interpolate if we haven't chosen an edge case
      if( selectedEnergy->first < std::prev(m_energy_etaRange_hists2D.end())->first && selectedEnergy != m_energy_etaRange_hists2D.begin() ){

        //select the smaller of the two energies to find the bin edge between them (energy bands are logarithmic)
        double energyBinEdge;
        if(selectedEnergy->first < secondSelectedEnergy->first){
          energyBinEdge = selectedEnergy->first*sqrtOf2;
        }
        else{
          energyBinEdge = secondSelectedEnergy->first*sqrtOf2;
        }

        //calculate a distance of the input energy to the bin edge
        double distance = std::fabs(energyBinEdge - inputParameters.at(0))/std::fabs(selectedEnergy->first - energyBinEdge);

        //if we get a random number larger than the distance then choose other energy.
        double rand = CLHEP::RandFlat::shoot(m_randomEngine);
        if(rand > distance){
          selectedEnergy = secondSelectedEnergy;
        }

      }
    }
    else{
      selectedEnergy = itUpperEnergy;
    }


    //Now move on to selecting the correct eta window
    //first get the map of eta windows to hists.
    const std::map< std::vector<double>, std::map< double , TFCS1DFunction* >* >& etaMinEtaMax_hists = selectedEnergy->second;

    std::map< std::vector<double>, std::map< double , TFCS1DFunction* >* >::const_iterator itSelectedEtaWindow, itSecondEtaWindow;

    //choose first max eta that is not less than input eta
    itSelectedEtaWindow = std::lower_bound(etaMinEtaMax_hists.begin(), std::prev(etaMinEtaMax_hists.end()), inputParameters.at(1), compareEtaMax2D);

    //if we have multiple eta params do interpolation
    if(etaMinEtaMax_hists.size() > 1){
      //if input eta is closer to upper eta boundary in itSelecteEtaWindow then select next window as second best choice
      if(std::abs(inputParameters.at(1) - itSelectedEtaWindow->first.at(1)) <  std::abs(inputParameters.at(1) - itSelectedEtaWindow->first.at(0))){
        if(itSelectedEtaWindow != std::prev(etaMinEtaMax_hists.end())){
          itSecondEtaWindow = std::next(itSelectedEtaWindow);
        }
        else{
          itSecondEtaWindow = std::prev(itSelectedEtaWindow);
        }
      }
      //if closer to min boundary of itSelectedEtaWindow do selection based on this
      else if(itSelectedEtaWindow != etaMinEtaMax_hists.begin()){
        itSecondEtaWindow = std::prev(itSelectedEtaWindow);
      }
      else{
        itSecondEtaWindow = std::next(itSelectedEtaWindow);
      }

      //find the boundary between the two selected eta windows
      double etaBinEdge;
      if(itSelectedEtaWindow->first.at(0) > itSecondEtaWindow->first.at(0)){
        etaBinEdge = itSelectedEtaWindow->first.at(0);
      }
      else{
        etaBinEdge = itSecondEtaWindow->first.at(0);
      }
      //calculate a distance of the input eta to the bin edge
      double distance = std::fabs(etaBinEdge - inputParameters.at(1))/std::fabs(itSelectedEtaWindow->first.at(0)  - itSelectedEtaWindow->first.at(1));
      //if we get a random number larger than the distance then choose other energy.
      double rand = CLHEP::RandFlat::shoot(m_randomEngine);
      if(rand > distance){
        itSelectedEtaWindow = itSecondEtaWindow;
      }
    }

    //get map entry corresponding to outEnergy bin
    std::map< double , TFCS1DFunction* >* hist2d = itSelectedEtaWindow->second;

    std::map< double , TFCS1DFunction* >::iterator upperBin2D, chosenBin;
    upperBin2D = hist2d->lower_bound(outEnergy);

    //if enenrgy greater than max in hist, choose highest bin
    if(upperBin2D == hist2d->end()){
      chosenBin = std::prev(upperBin2D);
    }
    else if(upperBin2D == hist2d->begin()){ //if energy smaller than first bin choose this
      chosenBin = upperBin2D;
    }
    else if(abs(outEnergy - upperBin2D->first) < abs(outEnergy - std::prev(upperBin2D)->first)){ //choose closest bin to outEnergy
      chosenBin = upperBin2D;
    }
    else{
      chosenBin = std::prev(upperBin2D);
    }

    TFCS1DFunction* hist = chosenBin->second;

    //draw random number from chosen hist
    random = hist->rnd_to_fct(CLHEP::RandFlat::shoot(m_randomEngine));
    if(randMax != 0.){
      while (random < randMin || random > randMax){
      random = hist->rnd_to_fct(CLHEP::RandFlat::shoot(m_randomEngine));
      }
    }
    else{
      while (random < randMin){
        random = hist->rnd_to_fct(CLHEP::RandFlat::shoot(m_randomEngine));
      }
    }

    }

    return random;



}
