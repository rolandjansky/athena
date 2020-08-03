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


  //testing new param code

    std::cout << "testing new getRand code" << std::endl;
    std::cout << "looking at pdf " << getName() << std::endl;
    std::cout << "input parameters " << std::endl;
    for (unsigned int i = 0; i < inputParameters.size(); i++ ){
      std::cout << inputParameters.at(i) << " ";
    }
    std::cout << std::endl;

    //std::cout << "randMin " << randMin << " randMax " << randMax << std::endl;



    double random;

    //std::cout << "m_energy_etaRange_hists1D.empty() " << m_energy_etaRange_hists1D.empty() << std::endl;

    //Implementation for 1D hist
    if(!m_energy_etaRange_hists1D.empty()){
    //Select energy values neighbouring input energy
    std::map< double , std::map< std::vector<double>, TH1*> >::iterator itUpperEnergy, itPrevEnergy, selectedEnergy, secondSelectedEnergy;

    //selects first energy that is not less than input energy
    itUpperEnergy = std::lower_bound(m_energy_etaRange_hists1D.begin(), m_energy_etaRange_hists1D.end(), inputParameters.at(0), compareEnergy1D);


    //if we have more than one energy parameterisation then run interpolation code
    if(m_energy_etaRange_hists1D.size() > 1){
      if (itUpperEnergy == m_energy_etaRange_hists1D.end()) {
        //select final iterator in map
        selectedEnergy = m_energy_etaRange_hists1D.end();
      } 
      else if (itUpperEnergy == m_energy_etaRange_hists1D.begin()) {
        //choose first iterator in map
        selectedEnergy = m_energy_etaRange_hists1D.begin();
        //do some sort of interpolation in this case to zero with log energy
      } else {
        //Check if iterator input energy is closer to previous iterator energy, if yes choose this instead
        itPrevEnergy = std::prev(itUpperEnergy);
        if (abs(inputParameters.at(0) - itPrevEnergy->first) < abs(itUpperEnergy->first - inputParameters.at(0))){
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
      if( selectedEnergy->first < m_energy_etaRange_hists1D.end()->first && selectedEnergy != m_energy_etaRange_hists1D.begin() ){
        std::cout << std::to_string(selectedEnergy->first) << " " << std::to_string(secondSelectedEnergy->first) << std::endl;  

        //select the smaller of the two energies to find the bin edge between them (energy bands are logarithmic)
        double energyBinEdge;
        if(selectedEnergy->first < secondSelectedEnergy->first){
          energyBinEdge = selectedEnergy->first*pow(2,0.5);
        }
        else{
          energyBinEdge = secondSelectedEnergy->first*pow(2,0.5);
        }

        //calculate a distance of the input energy to the bin edge
        double distance = fabs(energyBinEdge - inputParameters.at(0))/fabs(selectedEnergy->first - energyBinEdge);

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
    itSelectedEtaWindow = std::lower_bound(etaMinEtaMax_hists.begin(), etaMinEtaMax_hists.end(), inputParameters.at(1), compareEtaMax1D);

    //if we have multiple eta params do interpolation
    if(etaMinEtaMax_hists.size() > 1){
      //if input eta is closer to upper eta boundary in itSelecteEtaWindow then select next window as second best choice
      if(abs(inputParameters.at(1) - itSelectedEtaWindow->first.at(1)) <  abs(inputParameters.at(1) - itSelectedEtaWindow->first.at(0))){
        if(itSelectedEtaWindow != etaMinEtaMax_hists.end()){
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

      std::cout << "selected SelectedEtaWindow " << itSelectedEtaWindow->first.at(0) << " " << itSelectedEtaWindow->first.at(1) << std::endl;
      std::cout << "selected SecondEtaWindow " << itSecondEtaWindow->first.at(0) << " " << itSecondEtaWindow->first.at(1) << std::endl;
    
      //find the boundary between the two selected eta windows
      double etaBinEdge;
      if(itSelectedEtaWindow->first.at(0) > itSecondEtaWindow->first.at(0)){
        etaBinEdge = itSelectedEtaWindow->first.at(0);
      }
      else{
        etaBinEdge = itSecondEtaWindow->first.at(0);
      }
      //calculate a distance of the input eta to the bin edge
      double distance = fabs(etaBinEdge - inputParameters.at(1))/fabs(itSelectedEtaWindow->first.at(0)  - itSelectedEtaWindow->first.at(1));
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
    itUpperEnergy = std::lower_bound(m_energy_etaRange_hists2D.begin(), m_energy_etaRange_hists2D.end(), inputParameters.at(0), compareEnergy2D);


    //if we have more than one energy parameterisation then run interpolation code
    if(m_energy_etaRange_hists2D.size() > 1){
      if (itUpperEnergy == m_energy_etaRange_hists2D.end()) {
        //select final iterator in map
        selectedEnergy = m_energy_etaRange_hists2D.end();
      } 
      else if (itUpperEnergy == m_energy_etaRange_hists2D.begin()) {
        //choose first iterator in map
        selectedEnergy = m_energy_etaRange_hists2D.begin();
        //do some sort of interpolation in this case to zero with log energy
      } else {
        //Check if iterator input energy is closer to previous iterator energy, if yes choose this instead
        itPrevEnergy = std::prev(itUpperEnergy);
        if (abs(inputParameters.at(0) - itPrevEnergy->first) < abs(itUpperEnergy->first - inputParameters.at(0))){
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
      if( selectedEnergy->first < m_energy_etaRange_hists2D.end()->first && selectedEnergy != m_energy_etaRange_hists2D.begin() ){
        std::cout << std::to_string(selectedEnergy->first) << " " << std::to_string(secondSelectedEnergy->first) << std::endl;  

        //select the smaller of the two energies to find the bin edge between them (energy bands are logarithmic)
        double energyBinEdge;
        if(selectedEnergy->first < secondSelectedEnergy->first){
          energyBinEdge = selectedEnergy->first*pow(2,0.5);
        }
        else{
          energyBinEdge = secondSelectedEnergy->first*pow(2,0.5);
        }

        //calculate a distance of the input energy to the bin edge
        double distance = fabs(energyBinEdge - inputParameters.at(0))/fabs(selectedEnergy->first - energyBinEdge);

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
    itSelectedEtaWindow = std::lower_bound(etaMinEtaMax_hists.begin(), etaMinEtaMax_hists.end(), inputParameters.at(1), compareEtaMax2D);

    //if we have multiple eta params do interpolation
    if(etaMinEtaMax_hists.size() > 1){
      //if input eta is closer to upper eta boundary in itSelecteEtaWindow then select next window as second best choice
      if(abs(inputParameters.at(1) - itSelectedEtaWindow->first.at(1)) <  abs(inputParameters.at(1) - itSelectedEtaWindow->first.at(0))){
        if(itSelectedEtaWindow != etaMinEtaMax_hists.end()){
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

      std::cout << "selected SelectedEtaWindow " << itSelectedEtaWindow->first.at(0) << " " << itSelectedEtaWindow->first.at(1) << std::endl;
      std::cout << "selected SecondEtaWindow " << itSecondEtaWindow->first.at(0) << " " << itSecondEtaWindow->first.at(1) << std::endl;
    
      //find the boundary between the two selected eta windows
      double etaBinEdge;
      if(itSelectedEtaWindow->first.at(0) > itSecondEtaWindow->first.at(0)){
        etaBinEdge = itSelectedEtaWindow->first.at(0);
      }
      else{
        etaBinEdge = itSecondEtaWindow->first.at(0);
      }
      //calculate a distance of the input eta to the bin edge
      double distance = fabs(etaBinEdge - inputParameters.at(1))/fabs(itSelectedEtaWindow->first.at(0)  - itSelectedEtaWindow->first.at(1));
      //if we get a random number larger than the distance then choose other energy.
      double rand = CLHEP::RandFlat::shoot(m_randomEngine);
      if(rand > distance){
        itSelectedEtaWindow = itSecondEtaWindow;
      }
    }


    TH2* hist2d = itSelectedEtaWindow->second;

    TAxis *xaxis = hist2d->GetXaxis();
    Int_t binx = xaxis->FindBin(outEnergy);
    std::cout << "outEnergy " << outEnergy << " bin " << binx << std::endl;
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
    std::cout << "random " << random << std::endl; 

    return random;

  

}
