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
// $Id: G4MuBremsstrahlungModel.hh,v 1.23 2010-10-26 13:52:32 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4MuBremsstrahlungModel
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
// 10-02-04 Add lowestKinEnergy (V.Ivanchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivantchenko)
// 13-02-06 Add ComputeCrossSectionPerAtom (mma)
// 11-10-07 Add ignoreCut flag (V.Ivanchenko) 
// 28-02-08 Reorganized protected methods and members (V.Ivanchenko) 
// 06-03-08 Remove obsolete methods and members (V.Ivanchenko) 
//

//
// Class Description:
//
// Implementation of bremssrahlung by muons

// -------------------------------------------------------------------
//

#ifndef G4BiasedMuBremsstrahlungModel_h
#define G4BiasedMuBremsstrahlungModel_h 1

#include "G4VEmModel.hh"
#include "G4NistManager.hh"

#include "CLHEP/Units/PhysicalConstants.h"

class G4Element;
class G4ParticleChangeForLoss;

class G4BiasedMuBremsstrahlungModel : public G4VEmModel
{

public:

  G4BiasedMuBremsstrahlungModel(const G4ParticleDefinition* p = 0,
                          const G4String& nam = "MuBrem");

  virtual ~G4BiasedMuBremsstrahlungModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  virtual G4double MinEnergyCut(const G4ParticleDefinition*,
				const G4MaterialCutsCouple*);
			      
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

  inline void SetLowestKineticEnergy(G4double e);
  
  inline void SetBiasFactor(G4double x);

protected:

  G4double ComputMuBremLoss(G4double Z, G4double tkin, G4double cut);
  
  G4double ComputeMicroscopicCrossSection(G4double tkin,
					  G4double Z,
					  G4double cut);

  virtual G4double ComputeDMicroscopicCrossSection(G4double tkin,
						   G4double Z,
						   G4double gammaEnergy);

  inline void SetParticle(const G4ParticleDefinition*);

private:

  G4DataVector* ComputePartialSumSigma(const G4Material* material,
				       G4double tkin, G4double cut);

  const G4Element* SelectRandomAtom(const G4MaterialCutsCouple* couple) const;

  // hide assignment operator
  G4BiasedMuBremsstrahlungModel & operator=(const  G4BiasedMuBremsstrahlungModel &right);
  G4BiasedMuBremsstrahlungModel(const  G4BiasedMuBremsstrahlungModel&);

protected:

  const G4ParticleDefinition* m_particle;
  G4NistManager* m_nist;
  G4double m_mass;
  G4double m_rmass;
  G4double m_cc;
  G4double m_coeff;
  G4double m_sqrte;
  G4double m_bh;
  G4double m_bh1;
  G4double m_btf;
  G4double m_btf1;

private:

  G4ParticleDefinition*       m_theGamma;
  G4ParticleChangeForLoss*    m_fParticleChange;

  G4double m_lowestKinEnergy;
  G4double m_minThreshold;

//++++++++++++++++++++++++++++++++++++++++++++++
// X-section bias factor (ADA 2011/04/27)
//++++++++++++++++++++++++++++++++++++++++++++++
  G4double m_biasFactor;

  std::vector<G4DataVector*> m_partialSumSigma;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void G4BiasedMuBremsstrahlungModel::SetLowestKineticEnergy(G4double e) 
{
  m_lowestKinEnergy = e;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline
void G4BiasedMuBremsstrahlungModel::SetParticle(const G4ParticleDefinition* p)
{
  if(!m_particle) {
    m_particle = p;
    m_mass = m_particle->GetPDGMass();
    m_rmass=m_mass/CLHEP::electron_mass_c2 ;
    m_cc=CLHEP::classic_electr_radius/m_rmass ;
    m_coeff= 16.*CLHEP::fine_structure_const*m_cc*m_cc/3. ;
  }
}

inline void G4BiasedMuBremsstrahlungModel::SetBiasFactor(G4double x)
{
	std::cout<<" this is G4BiasedMuBremsstrahlungModel::SetBiasFactor - "<<
		" bias factor "<<x<<std::endl;
	m_biasFactor=x;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
