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
// $Id: G4mplAtlasIonisationWithDeltaModel.cxx 729653 2016-03-14 15:55:47Z jchapman $
// GEANT4 tag $Name: geant4-09-03-ref-00 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4mplAtlasIonisationWithDeltaModel
//
// Author:        Vladimir Ivanchenko
//
// Creation date: 06.09.2005
//
// Modifications:
// 12.08.2007 Changing low energy approximation and extrapolation.
//            Small bug fixing and refactoring (M. Vladymyrov)
// 13.11.2007 Use low-energy asymptotic from [3] (V.Ivanchenko)
// 2014-03-28 Allow for fractional magnetic charges (G. Palacino)
//
//
// -------------------------------------------------------------------
// References
// [1] Steven P. Ahlen: Energy loss of relativistic heavy ionizing particles,
//     S.P. Ahlen, Rev. Mod. Phys 52(1980), p121
// [2] K.A. Milton arXiv:hep-ex/0602040
// [3] S.P. Ahlen and K. Kinoshita, Phys. Rev. D26 (1982) 2347


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// class header
#include "G4mplAtlasIonisationWithDeltaModel.hh"
// Geant4 headers
#include "Randomize.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleChangeForLoss.hh"
#include "G4Electron.hh"
#include "G4DynamicParticle.hh"
#include "G4Version.hh"
// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4mplAtlasIonisationWithDeltaModel::G4mplAtlasIonisationWithDeltaModel(G4double mCharge, const G4String& nam)
  : G4VEmModel(nam),G4VEmFluctuationModel(nam),
    monopole(0),
    mass(0.0),
    magCharge(mCharge),
    twoln10(std::log(100.0)),
    betalow(0.01),
    betalim(0.1),
    beta2lim(betalim*betalim),
    bg2lim(beta2lim*(1.0 + beta2lim))
{
  nmpl = std::abs(magCharge) * 2 * CLHEP::fine_structure_const;
  //if(nmpl > 6)      { nmpl = 6; }
  //else if(nmpl < 1) { nmpl = 1; }
  pi_hbarc2_over_mc2 = CLHEP::pi * CLHEP::hbarc * CLHEP::hbarc / CLHEP::electron_mass_c2;
  chargeSquare = magCharge * magCharge;
  dedxlim = 45.*nmpl*nmpl*CLHEP::GeV*CLHEP::cm2/CLHEP::g;
  fParticleChange = 0;
  theElectron = G4Electron::Electron();
  G4cout << "### Monopole ionisation model with d-electron production, Gmag= "
         << magCharge/CLHEP::eplus << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4mplAtlasIonisationWithDeltaModel::~G4mplAtlasIonisationWithDeltaModel()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void
G4mplAtlasIonisationWithDeltaModel::Initialise(const G4ParticleDefinition* p,
                                               const G4DataVector&)
{
  monopole = p;
  mass     = monopole->GetPDGMass();
  //  if(!fParticleChange) { fParticleChange = GetParticleChangeForLoss(); }

  if(pParticleChange)
    fParticleChange = reinterpret_cast<G4ParticleChangeForLoss*>(pParticleChange);
  else
    fParticleChange = new G4ParticleChangeForLoss();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double
G4mplAtlasIonisationWithDeltaModel::ComputeDEDXPerVolume(const G4Material* material,
                                                         const G4ParticleDefinition* p,
                                                         G4double kineticEnergy,
                                                         G4double maxEnergy)
{
  const G4double tmax = MaxSecondaryEnergy(p,kineticEnergy);
  const G4double cutEnergy = std::min(tmax, maxEnergy);
  const G4double tau   = kineticEnergy / mass;
  const G4double gam   = tau + 1.0;
  const G4double bg2   = tau * (tau + 2.0);
  const G4double beta2 = bg2 / (gam * gam);
  const G4double beta  = std::sqrt(beta2);

  // low-energy asymptotic formula
  G4double dedx  = dedxlim*beta*material->GetDensity();

  // above asymptotic
  if(beta > betalow) {

    // high energy
    if(beta >= betalim) {
      dedx = ComputeDEDXAhlen(material, bg2, cutEnergy);

    } else {

      const G4double dedx1 = dedxlim*betalow*material->GetDensity();
      const G4double dedx2 = ComputeDEDXAhlen(material, bg2lim, cutEnergy);

      // extrapolation between two formula
      const G4double kapa2 = beta - betalow;
      const G4double kapa1 = betalim - beta;
      dedx = (kapa1*dedx1 + kapa2*dedx2)/(kapa1 + kapa2);
    }
  }
  return dedx;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double
G4mplAtlasIonisationWithDeltaModel::ComputeDEDXAhlen(const G4Material* material,
                                                     G4double bg2,
                                                     G4double cutEnergy)
{
  const G4double eDensity = material->GetElectronDensity();
  const G4double eexc  = material->GetIonisation()->GetMeanExcitationEnergy();

  // Ahlen's formula for nonconductors, [1]p157, f(5.7)
  G4double dedx =
    0.5*(std::log(2.0 * CLHEP::electron_mass_c2 * bg2*cutEnergy / (eexc*eexc)) - 1.0);

  // Kazama et al. cross-section correction
  G4double  k = 0.406;
  if(nmpl > 1) { k = 0.346; }

  // Bloch correction
  const G4double B[7] = { 0.0, 0.248, 0.672, 1.022, 1.243, 1.464, 1.685};

  if(nmpl > 6.0) { dedx += 0.5 * k - B[6]; }
  else if(nmpl < 1.0) { dedx += 0.5 * k - B[1]; }
  else {
    dedx += 0.5 * k - B[G4int(std::abs(magCharge) * 2 * CLHEP::fine_structure_const + 0.5)];
  }
  // density effect correction
  const G4double x = std::log(bg2)/twoln10;

  dedx -= material->GetIonisation()->DensityCorrection(x);

  // now compute the total ionization loss
  dedx *=  pi_hbarc2_over_mc2 * eDensity * nmpl * nmpl;

  if (dedx < 0.0) { dedx = 0; }
  return dedx;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double
G4mplAtlasIonisationWithDeltaModel::ComputeCrossSectionPerElectron(
                                                                   const G4ParticleDefinition* p,
                                                                   G4double kineticEnergy,
                                                                   G4double cutEnergy,
                                                                   G4double maxKinEnergy)
{
  G4double cross = 0.0;
  const G4double tmax = MaxSecondaryEnergy(p, kineticEnergy);
  const G4double maxEnergy = std::min(tmax,maxKinEnergy);
  if(cutEnergy < maxEnergy) {
    cross = (1.0/cutEnergy - 1.0/maxEnergy)*CLHEP::twopi_mc2_rcl2*chargeSquare;
  }
  return cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double
G4mplAtlasIonisationWithDeltaModel::ComputeCrossSectionPerAtom(
                                                               const G4ParticleDefinition* p,
                                                               G4double kineticEnergy,
                                                               G4double Z, G4double,
                                                               G4double cutEnergy,
                                                               G4double maxEnergy)
{
  const G4double cross =
    Z*ComputeCrossSectionPerElectron(p,kineticEnergy,cutEnergy,maxEnergy);
  return cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void
G4mplAtlasIonisationWithDeltaModel::SampleSecondaries(std::vector<G4DynamicParticle*>* vdp,
                                                      const G4MaterialCutsCouple*,
                                                      const G4DynamicParticle* dp,
                                                      G4double minKinEnergy,
                                                      G4double maxEnergy)
{
  G4double kineticEnergy = dp->GetKineticEnergy();
  const G4double tmax = MaxSecondaryEnergy(dp->GetDefinition(),kineticEnergy);

  G4double maxKinEnergy = std::min(maxEnergy,tmax);
  if(minKinEnergy >= maxKinEnergy) { return; }

  //G4cout << "G4mplAtlasIonisationWithDeltaModel::SampleSecondaries: E(GeV)= "
  //     << kineticEnergy/GeV << " M(GeV)= " << mass/GeV
  //     << " tmin(MeV)= " << minKinEnergy/MeV << G4endl;

  const G4double totEnergy     = kineticEnergy + mass;
  const G4double etot2         = totEnergy*totEnergy;
  const G4double beta2         = kineticEnergy*(kineticEnergy + 2.0*mass)/etot2;

  // sampling without nuclear size effect
  const G4double q = G4UniformRand();
  const G4double deltaKinEnergy = minKinEnergy*maxKinEnergy
    /(minKinEnergy*(1.0 - q) + maxKinEnergy*q);

  // delta-electron is produced
  const G4double totMomentum = totEnergy*std::sqrt(beta2);
  const G4double deltaMomentum =
    std::sqrt(deltaKinEnergy * (deltaKinEnergy + 2.0*CLHEP::electron_mass_c2));
  G4double cost = deltaKinEnergy * (totEnergy + CLHEP::electron_mass_c2) /
    (deltaMomentum * totMomentum);
  if(cost > 1.0) { cost = 1.0; }

  //  to << deltaKinEnergy << "  " << deltaMomentum << G4endl;

  const G4double sint = std::sqrt((1.0 - cost)*(1.0 + cost));

  const G4double phi = CLHEP::twopi * G4UniformRand() ;

  G4ThreeVector deltaDirection(sint*cos(phi),sint*sin(phi), cost);
  const G4ThreeVector& direction = dp->GetMomentumDirection();
  deltaDirection.rotateUz(direction);

  // create G4DynamicParticle object for delta ray
  G4DynamicParticle* delta =
    new G4DynamicParticle(theElectron,deltaDirection,deltaKinEnergy);

  vdp->push_back(delta);

  // Change kinematics of primary particle
  kineticEnergy       -= deltaKinEnergy;
  G4ThreeVector finalP = direction*totMomentum - deltaDirection*deltaMomentum;
  finalP               = finalP.unit();

  fParticleChange->SetProposedKineticEnergy(kineticEnergy);
  fParticleChange->SetProposedMomentumDirection(finalP);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4mplAtlasIonisationWithDeltaModel::SampleFluctuations(
#if G4VERSION_NUMBER > 1009
                                                                const G4MaterialCutsCouple* material,
                                                                const G4DynamicParticle* dp,
                                                                G4double tmax,
                                                                G4double length,
                                                                G4double meanLoss
#else
                                                                const G4Material* material,
                                                                const G4DynamicParticle* dp,
                                                                G4double& tmax,
                                                                G4double& length,
                                                                G4double& meanLoss
#endif
                                                                )
{
#if G4VERSION_NUMBER > 1009
  G4double siga = Dispersion(material->GetMaterial(),dp,tmax,length);
#else
  G4double siga = Dispersion(material,dp,tmax,length);
#endif
  G4double loss = meanLoss;
  siga = std::sqrt(siga);
  const G4double twomeanLoss = meanLoss + meanLoss;

  if(twomeanLoss < siga) {
    G4double x;
    do {
      loss = twomeanLoss*G4UniformRand();
      x = (loss - meanLoss)/siga;
    } while (1.0 - 0.5*x*x < G4UniformRand());
  } else {
    do {
      loss = G4RandGauss::shoot(meanLoss,siga);
    } while (0.0 > loss || loss > twomeanLoss);
  }
  return loss;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double
G4mplAtlasIonisationWithDeltaModel::Dispersion(const G4Material* material,
                                               const G4DynamicParticle* dp,
#if G4VERSION_NUMBER > 1009
                                               G4double tmax,
                                               G4double length
#else
                                               G4double& tmax,
                                               G4double& length
#endif
                                               )
{
  G4double siga = 0.0;
  const G4double tau   = dp->GetKineticEnergy()/mass;
  if(tau > 0.0) {
    const G4double electronDensity = material->GetElectronDensity();
    const G4double gam   = tau + 1.0;
    const G4double invbeta2 = (gam*gam)/(tau * (tau+2.0));
    siga  = (invbeta2 - 0.5) * CLHEP::twopi_mc2_rcl2 * tmax * length
      * electronDensity * chargeSquare;
  }
  return siga;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double
G4mplAtlasIonisationWithDeltaModel::MaxSecondaryEnergy(const G4ParticleDefinition*,
                                                       G4double kinEnergy)
{
  const G4double tau = kinEnergy/mass;
  return 2.0*CLHEP::electron_mass_c2*tau*(tau + 2.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
