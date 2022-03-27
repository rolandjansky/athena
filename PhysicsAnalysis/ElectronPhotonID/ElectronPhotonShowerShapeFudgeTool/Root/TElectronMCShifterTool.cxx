/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonShowerShapeFudgeTool/TElectronMCShifterTool.h"
#include <cmath>
#include <iostream>

// Default constructor.
TElectronMCShifterTool::TElectronMCShifterTool():
  Shifts(),
  Widths()
  {
    //Intentionally empty
  }

//Default Destructor
TElectronMCShifterTool::~TElectronMCShifterTool(){}
//---------------------------------------------------------------------------------------
// Gets the Et bin given the et (MeV)
unsigned int TElectronMCShifterTool::getShifterEtBin(double eT) {
  const unsigned int nEtBins = 6;
  const double GeV = 1000;
  const double eTBins[nEtBins] = {10*GeV,15*GeV,20*GeV,30*GeV,40*GeV,50*GeV};

  for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
    if(eT < eTBins[eTBin])
      return eTBin;
  }

  return 5;  // Return the last bin if > the last bin.
}

//---------------------------------------------------------------------------------------
// Gets the Eta bin [0-8] given the eta
unsigned int TElectronMCShifterTool::getShifterEtaBin(double eta) {
  const unsigned int nEtaBins = 9;
  const double etaBins[nEtaBins] = {0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47};

  for(unsigned int etaBin = 0; etaBin < nEtaBins; ++etaBin){
    if(fabs(eta) < etaBins[etaBin])
      return etaBin;
  }

  return 8;
}

//----------------------------------------------------------------------
// Shift individual variables. User decides which to shift. Do not call for
// variable you do not wish to shift.
//----------------------------------------------------------------------
double TElectronMCShifterTool::shiftVar(double eT,double eta,ElePIDNames::Var var,double val){
  if (var == ElePIDNames::DeltaPoverP           && val == 0.) {return val;}
  if (var == ElePIDNames::eratio                && val == 1.) {return val;}
  if (var == ElePIDNames::f1                    && val == 0.) {return val;}
  if (var == ElePIDNames::f3                    && val == 0.) {return val;}
  if (var == ElePIDNames::fside                 && val == 0.) {return val;}

  int ibin_combined = getShifterEtBin(eT)*9+getShifterEtaBin(eta); //Convert the 2d
  if(Shifts[var].empty()) return val;
  return val + Shifts[var].at(ibin_combined);
}

//------------------------------------------------------------------------------
// Shift variables. Shift by the et and eta bin numbers. Needs fixin'. Dont use
//------------------------------------------------------------------------------
double TElectronMCShifterTool::shiftVarByBin(int eTBin,int etaBin,ElePIDNames::Var var,double val){
  if (var == ElePIDNames::DeltaPoverP           && val == 0.) {return val;}
  if (var == ElePIDNames::eratio                && val == 1.) {return val;}
  if (var == ElePIDNames::f1                    && val == 0.) {return val;}
  if (var == ElePIDNames::f3                    && val == 0.) {return val;}
  if (var == ElePIDNames::fside                 && val == 0.) {return val;}

  int ibin_combined = eTBin*9+etaBin; //Convert the 2d
  if(Shifts[var].empty()) return val;
  return val + Shifts[var].at(ibin_combined);
}

//-----------------------------------------------------------------------------
// Shift references to all varialbes. For use in xAOD wrapper. Or stand alone
// if thats how you roll.
// ----------------------------------------------------------------------------
void TElectronMCShifterTool::shiftAll( float eT,
                                       float eta,
                                       float& rhad1  ,
                                       float& rhad   ,
                                       float& reta   ,
                                       float& rphi   ,
                                       float& weta2  ,
                                       float& f1     ,
                                       float& f3     ,
                                       float& fside  ,
                                       float& ws3    ,
                                       float& eratio ,
                                       float& e277   ,
                                       float& DeltaE ,
                                       float& deltaeta1,
                                       float& deltaphiRescaled
                                     ) const {
  int ibin_combined = getShifterEtBin(eT)*9+getShifterEtaBin(eta); //Convert the 2d

  if(!Widths[ElePIDNames::Var::deltaeta1].empty()){
    deltaeta1 = deltaeta1 * Widths[ElePIDNames::Var::deltaeta1].at(ibin_combined);
  }
  if(!Widths[ElePIDNames::Var::deltaphiRescaled].empty()){
    deltaphiRescaled = deltaphiRescaled * Widths[ElePIDNames::Var::deltaphiRescaled].at(ibin_combined);
  }
  if(!Widths[ElePIDNames::Var::rhad].empty()){
    rhad = rhad * Widths[ElePIDNames::Var::rhad].at(ibin_combined);
    rhad1 = rhad1 * Widths[ElePIDNames::Var::rhad].at(ibin_combined);
  }
  if(!Widths[ElePIDNames::Var::f1].empty()){
    f1 = f1 * Widths[ElePIDNames::Var::f1].at(ibin_combined);
  }
  if(!Widths[ElePIDNames::Var::f3].empty()){
    f3 = f3 * Widths[ElePIDNames::Var::f3].at(ibin_combined);
  }
  if(!Widths[ElePIDNames::Var::weta2].empty()){
    weta2 = weta2 * Widths[ElePIDNames::Var::weta2].at(ibin_combined);
  }

  if(!Shifts[ElePIDNames::Var::f1].empty()){
    if ( f1 != 0){ f1 = f1 + Shifts[ElePIDNames::Var::f1].at(ibin_combined); }
  }
  if(!Shifts[ElePIDNames::Var::f3].empty()){
    if ( f3 != 0){ f3 = f3 + Shifts[ElePIDNames::Var::f3].at(ibin_combined); }
  }
  if(!Shifts[ElePIDNames::Var::reta].empty()){
    reta = reta + Shifts[ElePIDNames::Var::reta].at(ibin_combined);
  }
  if(!Shifts[ElePIDNames::Var::weta2].empty()){
    weta2 = weta2 + Shifts[ElePIDNames::Var::weta2].at(ibin_combined);
  }
  if(!Shifts[ElePIDNames::Var::eratio].empty()){
    if ( eratio != 1){ eratio = eratio + Shifts[ElePIDNames::Var::eratio].at(ibin_combined); }
  }
  if(!Shifts[ElePIDNames::Var::rhad].empty()){
    rhad = rhad + Shifts[ElePIDNames::Var::rhad].at(ibin_combined);
    rhad1 = rhad1 + Shifts[ElePIDNames::Var::rhad].at(ibin_combined);
  }
  if(!Shifts[ElePIDNames::Var::rphi].empty()){
    rphi = rphi + Shifts[ElePIDNames::Var::rphi].at(ibin_combined);
  }
  if(!Shifts[ElePIDNames::Var::fside].empty()){
    if( fside!=0){ fside = fside + Shifts[ElePIDNames::Var::fside].at(ibin_combined); };
  }
  if(!Shifts[ElePIDNames::Var::ws3].empty()){
    ws3 = ws3 + Shifts[ElePIDNames::Var::ws3].at(ibin_combined);
  }
  if(!Shifts[ElePIDNames::Var::e277].empty()){
    e277 = e277 + Shifts[ElePIDNames::Var::e277].at(ibin_combined);
  }
  if(!Shifts[ElePIDNames::Var::DeltaE].empty()){
    DeltaE = DeltaE + Shifts[ElePIDNames::Var::DeltaE].at(ibin_combined);
  }

}
