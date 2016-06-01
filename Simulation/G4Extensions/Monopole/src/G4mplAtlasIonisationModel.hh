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
// $Id: G4mplAtlasIonisationModel.hh,v 1.2 2009-04-21 13:03:52 zmarshal Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4mplAtlasIonisationModel
//
// Author:        Vladimir Ivanchenko
//
// Creation date: 06.09.2005
//
// Modifications:
//
//
// Class Description:
//
// Implementation of model of energy loss of the magnetic monopole

// -------------------------------------------------------------------
//

#ifndef MONOPOLE_G4mplAtlasIonisationModel_h
#define MONOPOLE_G4mplAtlasIonisationModel_h 1

#include "G4VEmModel.hh"
#include "G4VEmFluctuationModel.hh"
#include "G4Version.hh"
#include "CLHEP/Units/PhysicalConstants.h"

class G4ParticleChangeForLoss;

class G4mplAtlasIonisationModel : public G4VEmModel, public G4VEmFluctuationModel
{

public:

  G4mplAtlasIonisationModel(G4double mCharge, const G4String& nam = "mplAtlasIonisation");

  virtual ~G4mplAtlasIonisationModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&) override final;

  virtual G4double ComputeDEDXPerVolume(const G4Material*,
                                        const G4ParticleDefinition*,
                                        G4double kineticEnergy,
                                        G4double cutEnergy) override final;

  virtual void SampleSecondaries(     std::vector<G4DynamicParticle*>* ,
                                      const G4MaterialCutsCouple*,
                                      const G4DynamicParticle*,
                                      G4double tmin,
                                      G4double maxEnergy) override final;


  virtual G4double SampleFluctuations(const G4MaterialCutsCouple* material,
                                      const G4DynamicParticle* dp,
                                      G4double tmax,
                                      G4double length,
                                      G4double meanLoss) override final;

  virtual G4double Dispersion(        const G4Material*,
                                      const G4DynamicParticle*,
                                      G4double tmax,
                                      G4double length) override final;

  // hide assignment operator and copy constructor
  G4mplAtlasIonisationModel & operator=(const  G4mplAtlasIonisationModel &right) = delete;
  G4mplAtlasIonisationModel(const  G4mplAtlasIonisationModel&) = delete;

private:

  void SetParticle(const G4ParticleDefinition* p);

  const G4ParticleDefinition* monopole;
  G4ParticleChangeForLoss*    fParticleChange;

  G4double mass;
  G4double magCharge;
  G4double chargeSquare;
  G4double twoln10;
  G4double beta2low;
  G4double beta2lim;
  G4double bg2lim;
  G4double factlow;
  G4int    nmpl;
};

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void G4mplAtlasIonisationModel::SampleSecondaries(
                                                         std::vector<G4DynamicParticle*>* ,
                                                         const G4MaterialCutsCouple*,
                                                         const G4DynamicParticle*,
                                                         G4double,
                                                         G4double)
{;}

inline G4double G4mplAtlasIonisationModel::Dispersion(
                                                      const G4Material* material,
                                                      const G4DynamicParticle* dp,
                                                      G4double tmax,
                                                      G4double length)
{
  G4double siga = 0.0;
  G4double tau   = dp->GetKineticEnergy()/mass;
  if(tau > 0.0) {
    G4double electronDensity = material->GetElectronDensity();
    G4double gam   = tau + 1.0;
    G4double invbeta2 = (gam*gam)/(tau * (tau+2.0));
    siga  = (invbeta2 - 0.5) * CLHEP::twopi_mc2_rcl2 * tmax * length
      * electronDensity * chargeSquare;
  }
  return siga;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
