/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PDFcreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "PDFcreator.h"

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
double ISF::PDFcreator::getRand(std::vector<double> inputParameters, double outEnergy, double randMin, double randMax)
{  

    //define variable to return from getRand call, should never returh zero
    double random = 0;


    //Implementation for 1D hist
    if(!m_energy_etaRange_hists1D.empty()){
    //Select energy values neighbouring input energy
    std::map< double , std::map< std::vector<double>, TH1*> >::iterator itUpperEnergy, itPrevEnergy, selectedEnergy, secondSelectedEnergy;

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
          energyBinEdge = selectedEnergy->first*pow(2,0.5);
        }
        else{
          energyBinEdge = secondSelectedEnergy->first*pow(2,0.5);
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
    std::map< std::vector<double>, TH1*> etaMinEtaMax_hists = selectedEnergy->second;

    std::map< std::vector<double>, TH1*>::iterator itSelectedEtaWindow, itSecondEtaWindow;
    
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
    TH1* hist = itSelectedEtaWindow->second;
    //Draw randomly from the histogram distribution.
    //if obj is 1D histogram just draw randomly from it
    random = hist->GetRandom();
    if(randMax != 0.){
      while (random < randMin || random > randMax){
      random = hist->GetRandom();
      }
    }
    else{
      while (random < randMin){
        random = hist->GetRandom();
      }
    }


    }



    //Implementation for 2D hist
    if(!m_energy_etaRange_hists2D.empty()){
    //Select energy values neighbouring input energy
    std::map< double , std::map< std::vector<double>, TH2*> >::iterator itUpperEnergy, itPrevEnergy, selectedEnergy, secondSelectedEnergy;

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
          energyBinEdge = selectedEnergy->first*pow(2,0.5);
        }
        else{
          energyBinEdge = secondSelectedEnergy->first*pow(2,0.5);
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
    std::map< std::vector<double>, TH2*> etaMinEtaMax_hists = selectedEnergy->second;

    std::map< std::vector<double>, TH2*>::iterator itSelectedEtaWindow, itSecondEtaWindow;
    
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


    TH2* hist2d = itSelectedEtaWindow->second;

    TAxis *xaxis = hist2d->GetXaxis();
    Int_t binx = xaxis->FindBin(outEnergy);
    TH1* hist = hist2d->ProjectionY("projectionHist",binx,binx);
    //Draw randomly from the histogram distribution.
    //if obj is 1D histogram just draw randomly from it
    random = hist->GetRandom();
    if(randMax != 0.){
      while (random < randMin || random > randMax){
      random = hist->GetRandom();
      }
    }
    else{
      while (random < randMin){
        random = hist->GetRandom();
      }
    }

    } 

    return random;

  

}
