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
//
// GEANT4 Class header file
//
//
// File name:     NewUrbanMscModel95
//
// Author:        Laszlo Urban
//
// Creation date: 20.03.2011
//
// Created from G4UrbanMscModel93
//
// Class Description:
//
// Implementation of the model of multiple scattering based on
// H.W.Lewis Phys Rev 78 (1950) 526 and L.Urban model

// -------------------------------------------------------------------
//

#ifndef NewUrbanMscModel95_h
#define NewUrbanMscModel95_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4VMscModel.hh"
#include "G4PhysicsTable.hh"
#include "G4MscStepLimitType.hh"

class G4ParticleChangeForMSC;
class G4SafetyHelper;
class G4LossTableManager;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NewUrbanMscModel95 : public G4VMscModel
{

public:

  NewUrbanMscModel95(const G4String& nam = "UrbanMsc95");

  virtual ~NewUrbanMscModel95();

  void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  G4double ComputeCrossSectionPerAtom(const G4ParticleDefinition* particle,
				      G4double KineticEnergy,
				      G4double AtomicNumber,
				      G4double AtomicWeight=0., 
				      G4double cut =0.,
				      G4double emax=DBL_MAX);

  void SampleScattering(const G4DynamicParticle*,
			G4double safety);

  G4double ComputeTruePathLengthLimit(const G4Track& track,
				      G4PhysicsTable* theLambdaTable,
				      G4double currentMinimalStep);

  G4double ComputeGeomPathLength(G4double truePathLength);

  G4double ComputeTrueStepLength(G4double geomStepLength);

  G4double ComputeTheta0(G4double truePathLength,
                         G4double KineticEnergy);

private:

  G4double SimpleScattering(G4double xmeanth, G4double x2meanth);

  G4double SampleCosineTheta(G4double trueStepLength, G4double KineticEnergy);

  G4double SampleDisplacement();

  G4double LatCorrelation();

  inline G4double GetLambda(G4double kinEnergy);

  inline void SetParticle(const G4ParticleDefinition*);

  inline void UpdateCache();

  //  hide assignment operator
  NewUrbanMscModel95 & operator=(const  NewUrbanMscModel95 &right);
  NewUrbanMscModel95(const  NewUrbanMscModel95&);

  const G4ParticleDefinition* particle;
  G4ParticleChangeForMSC*     fParticleChange;

  G4PhysicsTable*             theLambdaTable;
  const G4MaterialCutsCouple* couple;
  G4LossTableManager*         theManager;

  G4double mass;
  G4double charge,ChargeSquare;
  G4double masslimite,lambdalimit,fr;

  G4double taubig;
  G4double tausmall;
  G4double taulim;
  G4double currentTau;
  G4double tlimit;
  G4double tlimitmin;
  G4double tlimitminfix;
  G4double tgeom;

  G4double geombig;
  G4double geommin;
  G4double geomlimit;
  G4double skindepth;
  G4double smallstep;

  G4double presafety;

  G4double lambda0;
  G4double lambdaeff;
  G4double tPathLength;
  G4double zPathLength;
  G4double par1,par2,par3;

  G4double stepmin;

  G4double currentKinEnergy;
  G4double currentRange; 
  G4double rangeinit;
  G4double currentRadLength;

  G4double theta0max,rellossmax;
  G4double third;

  G4int    currentMaterialIndex;

  G4double y;
  G4double Zold;
  G4double Zeff,Z2,Z23,lnZ;
  G4double coeffth1,coeffth2;
  G4double coeffc1,coeffc2,coeffc3,coeffc4;
  G4double scr1ini,scr2ini,scr1,scr2;

  G4bool   isInitialized;
  G4bool   inside;
  G4bool   insideskin;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline
G4double NewUrbanMscModel95::GetLambda(G4double e)
{
  G4double x;
  if(theLambdaTable) {
    x = ((*theLambdaTable)[currentMaterialIndex])->Value(e);
  } else {
    x = CrossSection(couple,particle,e);
  }
  if(x > DBL_MIN) { x = 1./x; }
  else            { x = DBL_MAX; }
  return x;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline
void NewUrbanMscModel95::SetParticle(const G4ParticleDefinition* p)
{
  if (p != particle) {
    particle = p;
    mass = p->GetPDGMass();
    charge = p->GetPDGCharge()/CLHEP::eplus;
    ChargeSquare = charge*charge;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline
void NewUrbanMscModel95::UpdateCache()                                   
{
    lnZ = std::log(Zeff);
    // correction in theta0 formula
    coeffth1 = (1. - 8.7780e-2/Zeff)*(0.87 + 0.03*lnZ);                   
    coeffth2 = (4.0780e-2 + 1.7315e-4*Zeff)*(0.87 + 0.03*lnZ);              

    // tail parameters
    G4double Z13 = std::exp(lnZ/3.);
    coeffc1  = 2.3785    - Z13*(4.1981e-1 - Z13*6.3100e-2);
    coeffc2  = 4.7526e-1 + Z13*(1.7694    - Z13*3.3885e-1);
    coeffc3  = 2.3683e-1 - Z13*(1.8111    - Z13*3.2774e-1);
    coeffc4  = 1.7888e-2 + Z13*(1.9659e-2 - Z13*2.6664e-3);

    // for single scattering
    Z2   = Zeff*Zeff;
    Z23  = Z13*Z13;               
    scr1 = scr1ini*Z23;
    scr2 = scr2ini*Z2*ChargeSquare;
                                              
    Zold = Zeff;
}

#endif

