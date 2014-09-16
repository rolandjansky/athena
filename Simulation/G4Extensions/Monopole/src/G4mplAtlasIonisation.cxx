//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4mplAtlasIonisation.cxx 526171 2012-11-15 18:33:35Z zmarshal $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4mplAtlasIonisation
//
// Author:        Vladimir Ivanchenko
//
// Creation date: 25.08.2005
//
// Modifications:
//
//
// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "Monopole/G4mplAtlasIonisation.hh"
#include "G4Electron.hh"
//#include "Monopole/G4mplIonisationModel.hh"
//#include "Monopole/G4mplAtlasIonisationModel.hh"
#include "Monopole/G4mplAtlasIonisationWithDeltaModel.hh"
//#include "Monopole/G4mplAtlasIonisationModel.hh"
#include "G4BohrFluctuations.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "G4EmProcessSubType.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

using namespace std;

G4mplAtlasIonisation::G4mplAtlasIonisation(G4double mCharge, const G4String& name)
  : G4VEnergyLossProcess(name),
    magneticCharge(mCharge),
    isInitialised(false)
{

  std::cout <<"!!! G4mplAtlasIonisation constructor"<<std::endl;

  SetProcessSubType(  fIonisation );

  // By default classical magnetic charge is used
  if(magneticCharge == 0.0) magneticCharge = CLHEP::eplus*0.5/CLHEP::fine_structure_const;

  SetDEDXBinning(120);
  SetLambdaBinning(120);
  SetMinKinEnergy(0.1*CLHEP::keV);
  SetMaxKinEnergy(100.0*CLHEP::TeV);
  SetVerboseLevel(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4mplAtlasIonisation::~G4mplAtlasIonisation()
{
  std::cout <<"!!! G4mplAtlasIonisation destructor"<<std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4mplAtlasIonisation::InitialiseEnergyLossProcess(const G4ParticleDefinition*,
						  const G4ParticleDefinition*)
{
  if(isInitialised) return;

  std::cout <<"!!! G4mplAtlasIonisation::InitialiseEnergyLossProcess"<<std::endl;

  SetBaseParticle(0);
  SetSecondaryParticle(G4Electron::Electron());
  
  //  G4mplIonisationModel* ion  = new G4mplIonisationModel(magneticCharge);
  //  G4mplIonisationModel* ion  = new G4mplIonisationModel(magneticCharge,"PAI");
  //  G4mplAtlasIonisationModel* ion  = new G4mplAtlasIonisationModel(magneticCharge,"PAI");
  G4mplAtlasIonisationWithDeltaModel* ion  = new G4mplAtlasIonisationWithDeltaModel(magneticCharge,"PAI");
  ion->SetLowEnergyLimit(0.1*CLHEP::keV);
  //  ion->SetLowEnergyLimit(1000.0*keV);
  //  ion->SetHighEnergyLimit(100.*TeV);
  ion->SetHighEnergyLimit(7.*CLHEP::TeV);
  AddEmModel(0,ion,ion);
  
  SetStepFunction(0.2, 1*CLHEP::mm);
  
  isInitialised = true;
  
  std::cout <<"!!! G4mplAtlasIonisation::InitialiseEnergyLossProcess done"<<std::endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4mplAtlasIonisation::PrintInfo()
{
  //  G4cout << "SB:   My   No delta-electron production, only dE/dx"
  G4cout << "!!! G4mplAtlasIonisation:    Delta-electron production"
         << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
