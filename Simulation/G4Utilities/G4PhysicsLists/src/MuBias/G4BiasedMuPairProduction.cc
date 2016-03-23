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
// $Id: G4BiasedMuPairProduction.cc,v 1.52 2009-02-20 14:48:16 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4BiasedMuPairProduction
//
// Author:        Laszlo Urban
//
// Creation date: 30.05.1998
//
// Modifications:
//
// 04-06-98 in DoIt,secondary production condition:
//          range>std::min(threshold,safety)
// 26-10-98 new stuff from R. Kokoulin + cleanup , L.Urban
// 06-05-99 bug fixed , L.Urban
// 10-02-00 modifications+bug fix , new e.m. structure, L.Urban
// 29-05-01 V.Ivanchenko minor changes to provide ANSI -wall compilation
// 10-08-01 new methods Store/Retrieve PhysicsTable (mma)
// 17-09-01 migration of Materials to pure STL (mma)
// 20-09-01 (L.Urban) in ComputeMicroscopicCrossSection, remove:
//          if(MaxPairEnergy<CutInPairEnergy) MaxPairEnergy=CutInPairEnergy
// 26-09-01 completion of store/retrieve PhysicsTable
// 28-09-01 suppression of theMuonPlus ..etc..data members (mma)
// 29-10-01 all static functions no more inlined (mma)
// 07-11-01 particleMass becomes a local variable (mma)
// 19-08-02 V.Ivanchenko update to new design
// 23-12-02 Change interface in order to move to cut per region (V.Ivanchenko)
// 26-12-02 Secondary production moved to derived classes (V.Ivanchenko)
// 13-02-03 SubCutoff regime is assigned to a region (V.Ivanchenko)
// 08-08-03 STD substitute standard  (V.Ivanchenko)
// 27-09-03 e+ set to be a secondary particle (V.Ivanchenko)
// 12-11-03 G4EnergyLossSTD -> G4EnergyLossProcess (V.Ivanchenko)
// 10-02-04 Add m_lowestKinEnergy (V.Ivanchenko)
// 17-08-04 Utilise mu+ tables for mu- (V.Ivanchenko)
// 08-11-04 Migration to new interface of Store/Retrieve tables (V.Ivantchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivantchenko)
//
// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4BiasedMuPairProduction.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4BiasedMuPairProductionModel.hh"
#include "G4UniversalFluctuation.hh"

#include "CLHEP/Units/PhysicalConstants.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

using namespace std;

G4BiasedMuPairProduction::G4BiasedMuPairProduction(const G4String& name)
  : G4VEnergyLossProcess(name),
    m_theParticle(0),
    m_theBaseParticle(0),
    m_fParticleChange(0),
    m_lowestKinEnergy(1.*CLHEP::GeV),
    m_isInitialised(false),
	m_biasFactor(1.)
{
  SetProcessSubType(fPairProdByCharged);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4BiasedMuPairProduction::~G4BiasedMuPairProduction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4bool G4BiasedMuPairProduction::IsApplicable(const G4ParticleDefinition& p)
{
  return (p.GetPDGCharge() != 0.0 && p.GetPDGMass() > 10.0*CLHEP::MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4BiasedMuPairProduction::MinPrimaryEnergy(const G4ParticleDefinition*,
					      const G4Material*,
					      G4double)
{
  return m_lowestKinEnergy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4BiasedMuPairProduction::InitialiseEnergyLossProcess(
                         const G4ParticleDefinition* part,
			 const G4ParticleDefinition*)
{
  if (!m_isInitialised) {
    m_isInitialised = true;

    m_theParticle = part;
    SetSecondaryParticle(G4Positron::Positron());
    SetIonisation(false);

    G4BiasedMuPairProductionModel* em = new G4BiasedMuPairProductionModel();
    em->SetLowestKineticEnergy(m_lowestKinEnergy);
    G4VEmFluctuationModel* fm = 0;
    em->SetLowEnergyLimit(MinKinEnergy());
    em->SetHighEnergyLimit(MaxKinEnergy());
	
	em->SetBiasFactor(m_biasFactor);
	
    AddEmModel(1, em, fm);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4BiasedMuPairProduction::PrintInfo()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....




