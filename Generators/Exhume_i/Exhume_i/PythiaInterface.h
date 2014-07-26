/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-*-C++-*-
//-*-Hadronisation-*-
//   Written by James Monk and Andrew Pilkington
/////////////////////////////////////////////////////////////////////////////

#ifndef PYTHIA_INTERFACE_HH
#define PYTHIA_INTERFACE_HH

#include "PythiaRecord.h"
#include "CrossSection.h"
#include "Particle.h"
#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

namespace Exhume{
  class PythiaInterface{
  public:
    PythiaInterface(CrossSection&);
    ~PythiaInterface();
    void Hadronise();
    void SuperSymmetry(std::string);
  
    void InsertProtons();//Gets Both Protons puts them in hepevt_
    void InsertPartons();
    void SingleResonance();
    void TwoParticles();
    void ThreeParticles();
    void SetStableHeavyObject(bool _b){
      StableHeavyObject = _b;
      return;
    };
    double SetHiggsDecay(int);
    double SetWDecay(int);
    void SetTauDecay(bool);
    void SetQEDRadiationFromLeptons(double);
  private:
    //void SetColourIndex();
    //void SetColouredObjects();
    void SetPartonRecord();
    //void SetNonColouredObjects();
    //void PartonShower();
    //void SetString();
    CrossSection* xsecn;
    //std::vector<int> ColourIndex;
    //max number of joined partons from M.E.
    //int NQuarks;
    //int NQQPairs;
    //int NGluons;
    std::vector<Exhume::Particle> SubProcessRecord;
    int NumberSubProcessParticles;
    //std::vector<std::pair<int,int> > ColourRecord;
    
    //std::vector<std::pair<int,int> > StringRecord;
    bool StableHeavyObject;

    
  };
}
#endif
