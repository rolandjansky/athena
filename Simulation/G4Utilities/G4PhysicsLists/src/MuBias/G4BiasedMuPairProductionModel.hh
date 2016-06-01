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
// $Id: G4MuPairProductionModel.hh,v 1.29 2010-10-26 13:52:32 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4MuPairProductionModel
//
// Author:        Vladimir Ivanchenko on base of Laszlo Urban code
//
// Creation date: 18.05.2002
//
// Modifications:
//
// 23-12-02 Change interface in order to move to cut per region (V.Ivanchenko)
// 27-01-03 Make models region aware (V.Ivanchenko)
// 13-02-03 Add name (V.Ivanchenko)
// 10-02-04 Update parameterisation using R.Kokoulin model (V.Ivanchenko)
// 10-02-04 Add lowestKinEnergy (V.Ivanchenko)
// 13-02-06 Add ComputeCrossSectionPerAtom (mma)
// 12-05-06 Add parameter to SelectRandomAtom (A.Bogdanov) 
// 11-10-07 Add ignoreCut flag (V.Ivanchenko) 
// 28-02-08 Reorganized protected methods and members (V.Ivanchenko) 

//
// Class Description:
//
// Implementation of e+e- pair production by muons
//

// -------------------------------------------------------------------
//

#ifndef G4BiasedMuPairProductionModel_h
#define G4BiasedMuPairProductionModel_h 1

#include "G4VEmModel.hh"
#include "G4NistManager.hh"
#include <vector>

class G4Element;
class G4ParticleChangeForLoss;
class G4ParticleChangeForGamma;

class G4BiasedMuPairProductionModel : public G4VEmModel
{
public:

  G4BiasedMuPairProductionModel(const G4ParticleDefinition* p = 0,
                          const G4String& nam = "muPairProd");

  virtual ~G4BiasedMuPairProductionModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);
			
  virtual G4double ComputeCrossSectionPerAtom(
				 const G4ParticleDefinition*,
				 G4double kineticEnergy,
				 G4double Z, G4double A,
				 G4double cutEnergy,
				 G4double maxEnergy);
				 
  virtual G4double ComputeDEDXPerVolume(const G4Material*,
                                const G4ParticleDefinition*,
                                G4double kineticEnergy,
                                G4double cutEnergy);

  virtual void SampleSecondaries(std::vector<G4DynamicParticle*>*, 
				 const G4MaterialCutsCouple*,
				 const G4DynamicParticle*,
				 G4double tmin,
				 G4double maxEnergy);

  virtual G4double MinEnergyCut(const G4ParticleDefinition*,
			       const G4MaterialCutsCouple*);

  inline void SetLowestKineticEnergy(G4double e);

  inline void SetParticle(const G4ParticleDefinition*);
  
  inline void SetBiasFactor(G4double x);

protected:

  G4double ComputMuPairLoss(G4double Z, G4double tkin, G4double cut,
                            G4double tmax);

  G4double ComputeMicroscopicCrossSection(G4double tkin,
                                          G4double Z,
                                          G4double cut);

  virtual G4double ComputeDMicroscopicCrossSection(G4double tkin,
						   G4double Z,
						   G4double pairEnergy);

  virtual G4double MaxSecondaryEnergy(const G4ParticleDefinition*,
				      G4double kineticEnergy);

  inline void SetCurrentElement(G4double Z);


private:

  const G4Element* SelectRandomAtom(G4double kinEnergy, 
				    G4double dt, 
				    G4int it,
				    const G4MaterialCutsCouple* couple, 
				    G4double tmin);

  void MakeSamplingTables();

  inline G4double InterpolatedIntegralCrossSection(
		     G4double dt, G4double dz, G4int iz,
		     G4int it, G4int iy, G4double z);

  // hide assignment operator
  G4BiasedMuPairProductionModel & operator=(const  G4BiasedMuPairProductionModel &right);
  G4BiasedMuPairProductionModel(const  G4BiasedMuPairProductionModel&);

//++++++++++++++++++++++++++++++++++++++++++++++
// X-section bias factor (ADA 2011/04/27)
//++++++++++++++++++++++++++++++++++++++++++++++
  G4double m_biasFactor;

protected:

  const G4ParticleDefinition* m_particle;
  G4NistManager*              m_nist;

  G4double m_factorForCross;
  G4double m_sqrte;
  G4double m_particleMass;
  G4double m_currentZ;
  G4double m_z13;
  G4double m_z23;
  G4double m_lnZ;

  static G4double s_xgi[8],s_wgi[8];

private:

  G4ParticleDefinition*       m_theElectron;
  G4ParticleDefinition*       m_thePositron;
  G4ParticleChangeForLoss*    m_fParticleChange;

  G4double m_minPairEnergy;
  G4double m_lowestKinEnergy;

  // tables for sampling
  G4int m_nzdat;
  G4int m_ntdat;
  G4int m_nbiny;
  size_t m_nmaxElements;
  static G4double s_zdat[5], s_adat[5], s_tdat[8];
  G4double m_ya[1001], m_proba[5][8][1001];

  G4double m_ymin;
  G4double m_ymax;
  G4double m_dy;

  G4bool  m_samplingTablesAreFilled;
  std::vector<G4double> m_partialSum;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void G4BiasedMuPairProductionModel::SetLowestKineticEnergy(G4double e) 
{
  m_lowestKinEnergy = e;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline
void G4BiasedMuPairProductionModel::SetParticle(const G4ParticleDefinition* p)
{
  if(!m_particle) {
    m_particle = p;
    m_particleMass = m_particle->GetPDGMass();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void G4BiasedMuPairProductionModel::SetCurrentElement(G4double Z)
{
  if(Z != m_currentZ) {
    m_currentZ = Z;
    G4int iz = G4int(Z);
    m_z13 = m_nist->GetZ13(iz);
    m_z23 = m_z13*m_z13;
    m_lnZ = m_nist->GetLOGZ(iz);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline G4double G4BiasedMuPairProductionModel::InterpolatedIntegralCrossSection(
	        G4double dt, G4double dz,
                G4int iz, G4int it, G4int iy, G4double z)
{
  G4double fac =  1./(s_zdat[iz]  *(s_zdat[iz]  +1.));
  G4double fac1 = 1./(s_zdat[iz-1]*(s_zdat[iz-1]+1.));
  G4double f0 = fac1*m_proba[iz-1][it-1][iy] + 
                (fac*m_proba[iz][it-1][iy]-fac1*m_proba[iz-1][it-1][iy])*dz;
  G4double f1 = fac1*m_proba[iz-1][it  ][iy] + 
                (fac*m_proba[iz][it  ][iy]-fac1*m_proba[iz-1][it  ][iy])*dz;
  return (f0 + (f1-f0)*dt)*z*(z+1.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void G4BiasedMuPairProductionModel::SetBiasFactor(G4double x)
{
	std::cout<<" this is G4BiasedMuPairProductionModel::SetBiasFactor -"<<
		" bias factor "<<x<<std::endl;
	m_biasFactor=x;
}

#endif
